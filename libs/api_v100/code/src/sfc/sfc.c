/* 
 * sfc.c -- sfc_v100 接口实现， raw 读写 flash 以及套用文件系统模式两套接口
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/ts_cpl.h>
#include <api/sfc_v100.h>
#include <otdl/sfc_v100.h>
#include <api/hwlock_v100.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* 实现简单的文件操作，避免直接操作地址 */
#define SFC_FS_BASE_INDEX           (1)
#define SFC_FS_MAX_NAME_LEN         (128)

typedef struct sfc_fs_s {
    /* 文件名 */
    const ts_char *pc_fname;

    /* 基于 0 地址的偏移，需要扇区对齐 */
    ts_u32 u32_offset;

    /* 分配的大小， 扇区对齐 */
    ts_u32 u32_size;
} sfc_fs_s_t;


/* 内部保存，可以减少函数入参 */
typedef struct sfc_ctx_s {
    /* 内部保存 sfc 设备句柄 */
    ts_s32 s32_hdl;

    /* 内部保存硬件锁句柄 */
    ts_void *pv_lock;

    /* 屏蔽直接地址操作的文件接口 */
    ts_u32 u32_fs_num;
    sfc_fs_s_t *pst_fs;
} sfc_ctx_s_t;

static sfc_ctx_s_t sst_ctxs = {
    .s32_hdl = -1,
    .pv_lock = ts_null,
    .pst_fs = ts_null,
};

#define SFC_DEV_NAME        "/dev/tdk/sfc"

static ts_void sfc_fs_clear(ts_void)
{
    ts_u32 i;
    sfc_fs_s_t *pst_fs;

    pst_fs = sst_ctxs.pst_fs;
    for (i = 0; i < sst_ctxs.u32_fs_num; i++) {
        pst_fs->pc_fname = ts_null;
        pst_fs->u32_offset = 0;
        pst_fs->u32_size = 0;

        pst_fs++;
    }
}

/* 
 * 函数名 :  ts_lib_sfc_deinit
 * 描  述 :  sfc 去初始化
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_sfc_deinit(ts_void)
{
    if (sst_ctxs.pv_lock != ts_null) {
        ts_lib_hwlock_release(sst_ctxs.pv_lock);

        sst_ctxs.pv_lock = ts_null;
    }

    if (sst_ctxs.pst_fs != ts_null) {
        ts_cpl_kfree(sst_ctxs.pst_fs);

        sst_ctxs.pst_fs = ts_null;
    }

    if (sst_ctxs.s32_hdl >= 0) {
        close(sst_ctxs.s32_hdl);

        sst_ctxs.s32_hdl = -1;
    }
}



/* 
 * 函数名 :  ts_lib_sfc_init
 * 描  述 :  初始化 sfc
 * 输  入 :  参数               描述
 *           u32_fs_num         内部虚拟文件个数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              初始化成功
 *           TS_ERR             SFC 设备打开失败
 */
ts_s32 ts_lib_sfc_init(ts_u32 u32_fs_num)
{
    ts_u32 u32_fs_size;

    /* 硬件锁申请 */
    if (ts_null == sst_ctxs.pv_lock) {
        sst_ctxs.pv_lock = ts_lib_hwlock_get_free();
        if (ts_null == sst_ctxs.pv_lock) {
            ts_cpl_log_err("%s : get hwlock fail!", TS_EBUSY_STR);

            return -TS_EBUSY;
        }
    }

    /* 打开 sfc 设备句柄 */
    if (sst_ctxs.s32_hdl < 0) {
        sst_ctxs.s32_hdl = open(SFC_DEV_NAME, O_RDWR);
        if (sst_ctxs.s32_hdl < 0) {
            ts_lib_sfc_deinit();

            ts_cpl_log_err("%s", TS_ENODEV_STR);

            return -TS_ENODEV;
        }
    }

    /* 申请文件内存 */
    if (u32_fs_num > 0) {
        u32_fs_size = sizeof(sfc_fs_s_t) * u32_fs_num;
        sst_ctxs.pst_fs = (sfc_fs_s_t *)ts_cpl_kmalloc(u32_fs_size, TS_CPL_GFP_KERNEL);
        if (ts_null == sst_ctxs.pst_fs) {
            ts_cpl_log_err("%s : malloc for sfc fs fail!", TS_ENOMEM_STR);

            return -TS_ENOMEM;
        }
        sst_ctxs.u32_fs_num = u32_fs_num;
        sfc_fs_clear();
    } else {
        if (sst_ctxs.pst_fs != ts_null)
            ts_cpl_kfree(sst_ctxs.pst_fs);

        sst_ctxs.pst_fs = ts_null;
        sst_ctxs.u32_fs_num = 0;
    }

    return TS_OK;
}


/* 初始化需完成，不加锁 */
ts_s32 ts_lib_sfc_add_file(const ts_char *pc_name, ts_u32 u32_offset, ts_u32 u32_size)
{
    ts_u32 i;
    sfc_fs_s_t *pst_fs;

    pst_fs = sst_ctxs.pst_fs;
    for (i = 0; i < sst_ctxs.u32_fs_num; i++) {
        if (ts_null == pst_fs->pc_fname) {
            pst_fs->pc_fname = pc_name;
            pst_fs->u32_offset = u32_offset;
            pst_fs->u32_size = u32_size;

            return TS_OK;
        }

        pst_fs++;
    }

    return TS_ERR;
}


ts_s32 ts_lib_sfc_open(ts_char *pc_fname)
{
    ts_u32 i;
    sfc_fs_s_t *pst_fs;

    if ((sst_ctxs.s32_hdl < 0) || (ts_null == sst_ctxs.pst_fs))
        return -1;

    pst_fs = sst_ctxs.pst_fs;
    for (i = 0; i < sst_ctxs.u32_fs_num; i++) {
        if (0 == ts_cpl_strncmp(pc_fname, pst_fs->pc_fname, SFC_FS_MAX_NAME_LEN))
            return (i + SFC_FS_BASE_INDEX);

        pst_fs++;
    }

    return -1;
}


ts_void ts_lib_sfc_close(ts_s32 s32_fhdl)
{
    /* do nothing */
}


/* 
 * 函数名 :  ts_lib_sfc_read
 * 描  述 :  读取 flash 指定地址的数据
 * 输  入 :  参数               描述
 *           s32_fhdl           文件句柄
 *           s32_addr           flash 地址， 基于配置的 offset 为 0 地址, 
 *                              如果配置成 -1， 则按照内部偏移依次读取
 *           u32_rd_size        指定读个数
 * 输  出 :  参数               描述
 *           pu08_rd_buf        数据缓冲
 * 返  回 :  返回值             描述
 *          -TS_EINVAL          参数错误
 *          -TS_ENOSPC          读取地址范围错误
 *          0                   读错误
 *          u32_rd_size         读成功
 */
ts_s32 ts_lib_sfc_read(ts_s32 s32_fhdl, ts_u32 u32_addr, ts_u08 *pu08_rd_buf, ts_u32 u32_rd_size)
{
    ts_s32 s32_ret, s32_idx, u32_end, u32_rd_end;
    sfc_ioc_s_t st_ioc;
    sfc_fs_s_t *pst_fs;

    s32_idx = s32_fhdl - SFC_FS_BASE_INDEX;

    if ((s32_idx < 0) || (s32_idx >= (sst_ctxs.u32_fs_num)) || (ts_null == pu08_rd_buf) || (0 == u32_rd_size))
        return -TS_EINVAL;
    
    pst_fs = sst_ctxs.pst_fs;
    pst_fs += s32_idx;

    u32_end = (pst_fs->u32_offset + pst_fs->u32_size - 1);
    u32_rd_end = (pst_fs->u32_offset + u32_addr + u32_rd_size - 1);
    if (u32_rd_end > u32_end) {
        ts_cpl_log_err("%s : read end addr(%#x) big than file end addr(%#x)", TS_ENOSPC_STR, u32_rd_end, u32_end);

        return -TS_EFAULT;
    }

    st_ioc.pu08_buf = pu08_rd_buf;
    st_ioc.u32_rw_addr = u32_addr + pst_fs->u32_offset;
    st_ioc.u32_rw_size = u32_rd_size;
    s32_ret = ioctl(sst_ctxs.s32_hdl, TS_SFC_IOCTL_READ, (ts_long)&st_ioc);
    if ((ts_u32)s32_ret == u32_rd_size)
        return (ts_s32)u32_rd_size;
    else
        return 0;
}


/* 
 * 函数名 :  ts_lib_sfc_write
 * 描  述 :  向 flash 指定地址写入数据，支持扇区不对齐或不满扇区写入
 * 输  入 :  参数               描述
 *           s32_fhdl           文件句柄
 *           s32_addr           flash 地址
 *           pu08_wr_buf        写缓存
 *           u32_wr_size        指定个数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *          -TS_EINVAL          参数错误
 *          0                   写错误
 *          u32_wr_size         写成功
 */
ts_s32 ts_lib_sfc_write(ts_s32 s32_fhdl, ts_u32 u32_addr, ts_u08 *pu08_wr_buf, ts_u32 u32_wr_size)
{
    ts_s32 s32_ret, s32_idx, u32_end, u32_rd_end;
    sfc_ioc_s_t st_ioc;
    sfc_fs_s_t *pst_fs;

    s32_idx = s32_fhdl - SFC_FS_BASE_INDEX;
    if ((s32_idx < 0) || (s32_idx >= (sst_ctxs.u32_fs_num)) || (ts_null == pu08_wr_buf) || (0 == u32_wr_size))
        return -TS_EINVAL;

    pst_fs = sst_ctxs.pst_fs;
    pst_fs += s32_idx;
    u32_end = (pst_fs->u32_offset + pst_fs->u32_size - 1);
    u32_rd_end = (pst_fs->u32_offset + u32_addr + u32_wr_size - 1);
    if (u32_rd_end > u32_end) {
        ts_cpl_log_err("%s : read end addr(%#x) big than file end addr(%#x)\n", TS_ENOSPC_STR, u32_rd_end, u32_end);

        return -TS_EFAULT;
    }

    st_ioc.pu08_buf = pu08_wr_buf;
    st_ioc.u32_rw_addr = u32_addr + pst_fs->u32_offset;
    st_ioc.u32_rw_size = u32_wr_size;
    s32_ret = ioctl(sst_ctxs.s32_hdl, TS_SFC_IOCTL_WRITE, (ts_long)&st_ioc);
    if (TS_OK == s32_ret)
        return (ts_s32)u32_wr_size;
    else
        return 0;
}


/* 
 * 函数名 :  ts_lib_sfc_set_wrmode
 * 描  述 :  设置写 cover 模式
 * 输  入 :  参数               描述
 *           u32_wr_mode        cover 模式，包括 TS_SFC_WRITE_COVER or TS_SFC_WRITE_MERGE
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_sfc_set_wrmode(ts_u32 u32_wr_mode)
{
    if (sst_ctxs.s32_hdl >= 0)
        ioctl(sst_ctxs.s32_hdl, IOC_NR_SFC_SET_COVER, (ts_long)u32_wr_mode);
}


/* 
 * 函数名 :  ts_lib_sfc_set_attr
 * 描  述 :  设置 sfc 驱动属性
 * 输  入 :  参数               描述
 *           pst_attr           sfc 属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_ERR             设置失败
 *           TS_OK              设置成功
 */
ts_s32 ts_lib_sfc_set_attr(ts_sfc_attr_s_t *pst_attr)
{
    if ((sst_ctxs.s32_hdl < 0) || (ts_null == pst_attr))
        return -TS_EINVAL;

    return (ts_s32)ioctl(sst_ctxs.s32_hdl, TS_SFC_IOCTL_SET_ATTR, (ts_long)pst_attr);
}


/* 
 * 函数名 :  ts_lib_sfc_get_attr
 * 描  述 :  获取 sfc 驱动属性
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           pst_attr           sfc 属性
 * 返  回 :  返回值             描述
 *           TS_ERR             设置失败
 *           TS_OK              设置成功
 */
ts_s32 ts_lib_sfc_get_attr(ts_sfc_attr_s_t *pst_attr)
{
    if ((sst_ctxs.s32_hdl < 0) || (ts_null == pst_attr))
        return -TS_EINVAL;

    return (ts_s32)ioctl(sst_ctxs.s32_hdl, TS_SFC_IOCTL_GET_ATTR, (ts_long)pst_attr);
}

