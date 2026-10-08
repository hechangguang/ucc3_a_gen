/* 
 * sfc_v100.c -- sfc_v100 实现, 不采用中断，使用查询方式完成
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/chip_def.h>
#include <kapi/k_sfc_v100.h>
#include <kapi/k_base_v100.h>

static ts_sfc_ctx_s_t sst_ctx = {0};

#define SFC_MAX_ERASE_SIZE      (128 * 1024)

extern ts_s32 tdl_clkm_get_axi_clk(ts_void);

static ts_s32 sfc_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_s32 sfc_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_long sfc_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_dev;
    ts_long lg_ret;
    ts_u32 u32_ioclt_nr;
    ts_u32 *pu32_grp;

    pst_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_dev) || (ts_null == pst_dev->pv_private))
        return TS_ERR;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    lg_ret = TS_OK;
    u32_ioclt_nr = _TS_CPL_IOC_NR(u32_cmd);
    switch (u32_ioclt_nr) {
        case IOC_NR_SFC_INIT:
            pu32_grp = (ts_u32 *)ulg_arg;
            if (pu32_grp != ts_null)
                *pu32_grp = (ts_u32)pst_dev->pv_private;
            else
                lg_ret = -TS_EINVAL;

            break;

        case IOC_NR_SFC_DEINIT:
            break;

        default:
            lg_ret = TS_ERR;

            break;

    }

    return lg_ret;
}


static ts_cpl_fileops_s_t sst_sfc_v100_ops = {
    .open = sfc_v100_open,
    .release = sfc_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = sfc_v100_ioctl,
};


static ts_s32 sfc_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 i, u32_dev_cnt;
    ts_cpl_res_s_t st_res;
    ts_s32 s32_ret;
    ts_sfc_dev_s_t *pst_sfc_dev;
    ts_sfc_attr_s_t *pst_attr;

    sst_ctx.pst_cpl_dev = ts_cpl_dev_register("/dev/sfc", (ts_cpl_fileops_s_t *)&sst_sfc_v100_ops, 0);
    if (ts_null == sst_ctx.pst_cpl_dev) {
        ts_kapi_log_err("Failed to register dev for sfc\n");

        return TS_ENOMEM;
    }

    /* 将上下文挂回 cpl_dev */
    sst_ctx.pst_cpl_dev->pv_private = (ts_void *)&sst_ctx;

    /* 按照最大 128KB 擦除空间申请 */
    sst_ctx.pu08_save = ts_cpl_tmm_malloc(gpv_tmm_sys, SFC_MAX_ERASE_SIZE, TS_HW_ALIGN_SIZE, "sfc_buf");
    if (ts_null == sst_ctx.pu08_save) {
        s32_ret = TS_ENOMEM;
        ts_kapi_log_err("%s\n", TS_ENOMEM_STR);

        goto __probe_get_err;
    }

    sst_ctx.u32_fifo_max_dep = (ts_u32)ts_cpl_plat_get_value(pst_drv, "fifo-dep", 0, 128);

#if 0
    s32_ret = tdl_clkm_get_axi_clk();
    if (s32_ret > 0)
        sst_ctx.u32_sys_clk = (ts_u32)s32_ret;
    else
        sst_ctx.u32_sys_clk = ts_cpl_plat_get_clocks(pst_drv, 0);
#else
    sst_ctx.u32_sys_clk = 100000000;
#endif

    /* 至少需要有一个设备 */
    u32_dev_cnt = 0;
    for (i = 0; i < TS_SFC_MAX_NUM; i++) {
        s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, i);
        if (TS_OK == s32_ret) {
            pst_sfc_dev = (ts_sfc_dev_s_t *)&sst_ctx.ast_dev[i];
            pst_attr = (ts_sfc_attr_s_t *)&pst_sfc_dev->st_attr;

            pst_sfc_dev->u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);

            /* 初始化默认 attr, 默认 20 jiffies 的超时时间 */
            pst_attr->u08_rx_sample_dly = 0;
            pst_attr->u08_cs = 0;
            pst_attr->u08_tmout_jfs = 20;

            pst_sfc_dev->u32_clk_div = 8;
            pst_sfc_dev->pv_parent = (ts_void *)&sst_ctx;

            u32_dev_cnt++;
        } else {
            if (0 == i) {
                ts_kapi_log_err("Failed to get regbase for sfc\n");

                goto __probe_get_err;
            }
        }
    }

    ts_kapi_log_notice("sfc num=%d\n", u32_dev_cnt);

    return TS_OK;

__probe_get_err:
    ts_cpl_dev_unregister(sst_ctx.pst_cpl_dev);

    return s32_ret;
}


static ts_cpl_of_devid_s_t sst_sfc_drv_id = {
    .compatible = "tg,sfc_v100",
};

static ts_cpl_plat_drv_s_t sst_sfc_drv = {
    .probe = sfc_v100_probe,
    .driver = {
        .name = "sfc",
        .of_match_table = &sst_sfc_drv_id,
    }
};

ts_s32 sfc_init(ts_void)
{
    return ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_sfc_drv);
}

static TS_FUNC_UNUSED ts_void sfc_exit(ts_void)
{
    if (sst_ctx.pst_cpl_dev != ts_null) {
        ts_cpl_dev_unregister(sst_ctx.pst_cpl_dev);

        sst_ctx.pst_cpl_dev = ts_null;
    }
}

ts_cpl_ins_module_init(sfc, sfc_init);
ts_cpl_module_exit(sfc_exit);
