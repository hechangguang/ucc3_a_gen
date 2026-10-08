/*
 * k_sfc_v100.h -- sfc v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 *
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_SFC_V100_H__
#define __K_SFC_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/sfc_v100.h>

/* norflash erase 方式4k or 64k */
#define SNOR_ERASE_SECTOR           (0)
#define SNOR_ERASE_BLOCK            (1)


/* io mode */
#define SFC_IO_STANDARD         TS_SFC_MODE_STANDARD
#define SFC_IO_DUAL             TS_SFC_MODE_DUAL
#define SFC_IO_QUAD             TS_SFC_MODE_QUAD

/* 读写方向 */
#define SFC_OPS_READ        (0)
#define SFC_OPS_WRITE       (1)

/* flash 类型 */
#define SFC_TYPE_UNKNOW     (0)
#define SFC_TYPE_NOR        (1)
#define SFC_TYPE_NAND       (2)

#define SFC_NOR_MAX_INFO    (5)
#define SFC_NAND_MAX_INFO   (2)

#define SFC_1LINE                   (1)
#define SFC_2LINE                   (2)
#define SFC_4LINE                   (3)
#define SFC_8LINE                   (4)

//#define SFC_NAND_MASK_LINES         (0x7F)
//#define SFC_NAND_MASK_CHK_PAGES     (0x80)

typedef struct sfc_v100_qio_s {
    /* 多线寄存器配置 */
    ts_u32 u32_rd_qset;
    ts_u32 u32_wr_qset;

    /* 多线控制寄存器配置 */
    ts_u32 u32_qrd_ctrl;
    ts_u32 u32_qwr_ctrl;

    ts_u32 u32_drd_ctrl;
    ts_u32 u32_dwr_ctrl;

    /* 单线控制寄存器配置 */
    ts_u32 u32_rd_ctrl;
    ts_u32 u32_wr_ctrl;
} sfc_v100_qio_s_t;

typedef struct snor_attr_s {
    /* 页、区、块地址计算 */
    ts_u08 u08_page_addr_shift;
    ts_u08 u08_sec_addr_shift;
    ts_u08 u08_blk_addr_shift;

    /* 擦除标志，是否可以支持最大 64KB 大小擦除，如果不能，仅按照 4KB 擦除 */
    ts_u08 u08_erase_64kb;
    ts_u08 u08_lines;
    ts_u08 u08_addr_bits;
    ts_u08 au08_rev[2];

    /* 总大小 */
    ts_u32 u32_size;

    /* qio 配置 */
    ts_void *pv_qio;
} snor_attr_s_t;


/* SPI-NAND FLASH 属性 */
typedef struct snand_attr_s {
    /* 页和块地址计算 */
    ts_u08 u08_page_addr_shift;
    ts_u08 u08_blk_addr_shift;

    /* > 0 则表明有多 plane */
    ts_u08 u08_plane_col_bit;
    ts_u08 u08_plane_chk_bit;

    /* 是否多线 */
    ts_u08 u08_lines;

    /* 坏块多页检测 */
    ts_u08 u08_multi_bad;

    /* 每块页数 */
    ts_u08 u08_pages_pre_blk;

    /* 保留 */
    ts_u08 u08_rev;

    /* 总大小 */
    ts_u32 u32_size;

    /* qio 配置 */
    ts_void *pv_qio;
} snand_attr_s_t;


/* flash info */
typedef struct sfc_info_s {
    /* 器件名称 */
    ts_char *pc_name;

    /* 器件 ID */
    ts_u32 u32_id;
    ts_u32 u32_cfg_idx;
} sfc_info_s_t;

#if 0
typedef struct sfc_fops_s {
    ts_u32 (*read)(ts_void *pv_dev, ts_u32 u32_addr, ts_u08 *pu08_buf, ts_u32 u32_size);

} sfc_fops_s_t;
#endif

/* sfc 设备驱动结构体 */
typedef struct ts_sfc_dev_s {
    /* 寄存器基地址 */
    ts_u32 u32_regbase;

    ts_sfc_attr_s_t st_attr;

    /* spi bus 时钟配置 */
    ts_u32 u32_clk_div;

    /* 器件属性 */
    ts_u32 u32_fid;
    ts_u32 u32_ftype;

    /* 当前对接的 flash 配置 */
    ts_void *pv_flash;

    /* 当前的 IO 配置 */
    ts_void *pv_qio;

    /* parent */
    ts_void *pv_parent;

    /* 便于 sffs 引用 */
    ts_void *pv_super;
    ts_void *pv_map;

    /* 底层软件保护配置 */
    ts_void *pv_protect;

    /* 私有变量 */
    //ts_void *pv_priv;
} ts_sfc_dev_s_t;


/* sfc 驱动上下文 */
typedef struct ts_sfc_ctx_s {
    /* 硬件使用的时钟 */
    ts_u32 u32_sys_clk;

    /* 硬件最大 fifo 深度 */
    ts_u32 u32_fifo_max_dep;

    ts_sfc_dev_s_t ast_dev[TS_SFC_MAX_NUM];

    /* 保存 cpl_dev 句柄 */
    ts_cpl_dev_s_t *pst_cpl_dev;

    /* 申请 128KB 大小缓存，用于写扇区不对齐或者不满的时候，读回保存做 merge
        多设备共用此缓冲
    */
    ts_u08 *pu08_save;
} ts_sfc_ctx_s_t;


/* sfc 传输上下文 */
typedef struct ts_sfc_msg_s {
    ts_u32 u32_cmd_size;
    ts_u08 *pu08_cmd_buf;

    ts_u32 u32_tx_size;
    ts_u08 *pu08_tx_buf;

    /* recv */
    ts_u32 u32_rx_size;
    ts_u08 *pu08_rx_buf;

    ts_u32 u32_xfer_line;
    ts_u32 u32_mode;
    ts_u32 u32_noflash;
}ts_sfc_msg_s_t;

/* ioctl */
enum {
    /* SFC 初始化 */
    IOC_NR_SFC_INIT = 0,

    /*  SFC 去初始化 */
    IOC_NR_SFC_DEINIT = 1,

    IOC_NR_SFC_BUTT,
};

#define TS_SFC_IOCTL_INIT       TS_CPL_IOW(TS_IOC_MAGIC_SFC, IOC_NR_SFC_INIT, ts_u32)
#define TS_SFC_IOCTL_DEINIT     TS_CPL_IOR(TS_IOC_MAGIC_SFC, IOC_NR_SFC_DEINIT, ts_u32)


/*
 * 函数名 :  tlib_sfc_xfer
 * 描  述 :  sfc 读
 * 输  入 :  参数           描述
 *          pst_dev         sfc 设备句柄
 *          ts_sfc_msg_s_t  sfc 传输上下文
 * 输  出 :  参数           描述
 *           pu08_rd_buf     读缓存
 * 返  回 :  返回值         描述
 *           TS_OK          读成功
 *           TS_ETIMEDOUT   读超时
 */
ts_s32 tlib_sfc_xfer(ts_sfc_dev_s_t *pst_dev,ts_sfc_msg_s_t *pst_msg);



/*
 * 函数名 :  tlib_sfc_write
 * 描  述 :  sfc 写
 * 输  入 :  参数           描述
 *          pst_dev         sfc 设备句柄
 *          pst_msg         sfc 写消息上下文
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_ERR         写失败
 *           TS_OK          写成功
 */
ts_s32 tlib_sfc_write(ts_sfc_dev_s_t *pst_dev, ts_sfc_msg_s_t *pst_msg);


/*
 * 函数名 :  tlib_sfc_set_attr
 * 描  述 :  设置 sfc 属性，配置时钟分频，以及采样延时
 * 输  入 :  参数           描述
 *          pst_dev         sfc 设备句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void tlib_sfc_set_attr(ts_sfc_dev_s_t *pst_dev, ts_sfc_attr_s_t *pst_attr);


/*
 * 函数名 :  tlib_sfc_erase
 * 描  述 :  擦除
 * 输  入 :  参数               描述
 *          pst_dev             sfc 设备句柄
 *          u32_erase_cmd       擦除命令， 4KB， 32KB 等
 *          u32_flash_addr     擦除地址，必须擦除扇区对齐
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void tlib_sfc_erase(ts_sfc_dev_s_t *pst_dev, ts_u32 u32_erase_cmd, ts_u32 u32_flash_addr, ts_u32 u32_addr_size);


/*
 * 函数名 :  tlib_sfc_set_wen
 * 描  述 :  设置写使能
 * 输  入 :  参数               描述
 *          pst_dev         sfc 设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void tlib_sfc_set_wen(ts_sfc_dev_s_t *pst_dev);


/*
 * 函数名 :  tlib_sfc_read_id
 * 描  述 :  读取设备id
 * 输  入 :  参数           描述
 *          pst_dev         sfc 设备句柄
 *          u32_sfc_type    指定 flash 类型, 可配置范围： SFC_TYPE_NOR SFC_TYPE_NAND
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          0               读取失败
 *          u32_fid         设备的id
 */
ts_u32 tlib_sfc_read_id(ts_sfc_dev_s_t *pst_dev, ts_u32 u32_sfc_type);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_SFC_V100_H__ */
