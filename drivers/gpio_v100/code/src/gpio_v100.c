/* 
 * gpio_v100.c -- gpio v100 实现, 对外接口是标准设备驱动，内部使用固定接口调用，避免过多函数指针操作
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <linux/io.h>
#include <kapi/k_base_v100.h>
#include <kapi/k_gpio_v100.h>
#include "gpio_tg_v100.h"


static ts_cpl_irqreturn_en_t gpio_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    ts_u32 i, u32_mask, u32_int_status;
    gpio_dev_s_t *pst_gpio_dev;

    pst_gpio_dev = (gpio_dev_s_t *)pv_dev;
    u32_int_status = ts_cpl_readl(pst_gpio_dev->u32_regbase + GPIO_TG_OFFSET_INTSTATUS);
    ts_cpl_writel(u32_int_status, (pst_gpio_dev->u32_regbase + GPIO_TG_OFFSET_EOI));

    for (i = 0; i < GPIO_BANK_MAX_IONUM; i++) {
        u32_mask = 1 << i;
        if ((u32_int_status & u32_mask) > 0) {
            if (pst_gpio_dev->afp_cb[i] != ts_null)
                pst_gpio_dev->afp_cb[i]();
        }
    }

    return TS_CPL_IRQ_HANDLED;
}

static ts_s32 gpio_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_s32 gpio_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_long gpio_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_cpl_dev;
    ts_u32 u32_ioctl_nr, *pu32_grp;
    ts_long lg_ret;

    pst_cpl_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_cpl_dev) || (ts_null == pst_cpl_dev->pv_private))
        return TS_ERR;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    lg_ret = TS_OK;
    u32_ioctl_nr = _TS_CPL_IOC_NR(u32_cmd);
    switch (u32_ioctl_nr) {
        case IOC_NR_GPIO_INIT:
            pu32_grp = (ts_u32 *)ulg_arg;
            if (pu32_grp != ts_null)
                *pu32_grp = (ts_u32)pst_cpl_dev->pv_private;
            else
                lg_ret = -TS_EINVAL;

            break;
        case IOC_NR_GPIO_DEINIT:

            break;
     
        default :
            lg_ret = TS_ERR;

            break;
    }

    return lg_ret;
}

static ts_cpl_fileops_s_t sst_gpio_v100_ops = {
    .open = gpio_v100_open,
    .release = gpio_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = gpio_v100_ioctl,
};

static ts_s32 gpio_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 i, j;
    ts_cpl_res_s_t st_res;
    ts_s32 s32_ret, s32_irq_no;
    ts_cpl_dev_s_t *pst_cpl_dev;
    gpio_grp_s_t *pst_gpio_grp;

    pst_cpl_dev = ts_cpl_dev_register("/dev/gpio", (ts_cpl_fileops_s_t *)&sst_gpio_v100_ops, sizeof(gpio_grp_s_t));
    if (ts_null == pst_cpl_dev) {
        ts_kapi_log_err("Failed to register dev for gpio\n");

        return -TS_ENOMEM;
    }

    pst_gpio_grp = (gpio_grp_s_t *)pst_cpl_dev->pv_private;
    for (i = 0; i < TS_GPIO_MAX_GRP; i++) {
        s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, i);
        if (s32_ret != TS_OK) {
            ts_kapi_log_err("Failed to get regbase for gpio\n");

            break;
        }

        s32_irq_no = ts_cpl_plat_get_irq(pst_drv, i);
        if (s32_irq_no <= 0) {
            ts_kapi_log_err("Failed to get irq num for gpio\n");

            break;
        }

        pst_gpio_grp->u32_bank_num++;
        pst_gpio_grp->ast_dev[i].u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
        pst_gpio_grp->ast_dev[i].s32_irq_no = s32_irq_no;
        for (j = 0; j < GPIO_BANK_MAX_IONUM; j++)
            pst_gpio_grp->ast_dev[i].afp_cb[j] = ts_null;

        s32_ret = ts_cpl_request_irq(s32_irq_no, gpio_handler, 0, "gpio", (ts_void *)&pst_gpio_grp->ast_dev[i]);
        if (s32_ret != TS_OK) {
            ts_kapi_log_err("Failed to register irq for gpio grp[%d]\n", i);

            pst_gpio_grp->ast_dev[i].s32_irq_no = 0;
        }
    }

    return TS_OK;
}

static ts_cpl_of_devid_s_t sst_gpio_drv_id = {
    .compatible = "tg,gpio_v100",
};

static ts_cpl_plat_drv_s_t sst_gpio_drv = {
    .probe = gpio_v100_probe,
    .driver = {
        .name = "gpio",
        .of_match_table = &sst_gpio_drv_id,
    }
};

ts_s32 gpio_init(ts_void)
{
    ts_s32 s32_ret;

    s32_ret = ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_gpio_drv);

    return s32_ret;
}

ts_cpl_ins_module_init(gpio, gpio_init);
