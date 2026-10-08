/* 
 * dmac_v100.c -- dmac_v100 实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <kapi/k_base_v100.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_dmac_v100.h>


ts_u32 tdl_dmac_read_int_status(dmac_dev_s_t *pst_dev)
{
    return ts_cpl_readl(pst_dev->u32_regbase + DMAC_TG_OFFSET_INT_STATUS);
}

ts_void tdl_dmac_irq_hdl(dmac_dev_s_t *pst_dev, ts_u32 u32_chn)
{
    ts_u32 u32_val, u32_max_chn;
    dmac_chn_s_t *pst_chn;
    ts_cpl_wait_s_t *pst_wait;

    u32_max_chn = (ts_u32)pst_dev->u16_chn_num;

    if (u32_chn >= u32_max_chn) {
        /* comm int */
        u32_val = ts_cpl_readl(pst_dev->u32_regbase + DMAC_TG_OFFSET_COMM_INT_STATUS);
        ts_cpl_writel(u32_val, (pst_dev->u32_regbase + DMAC_TG_OFFSET_COMM_INT_CLEAR));
    } else {
        pst_chn = (dmac_chn_s_t *)pst_dev->pv_priv;
        pst_chn += u32_chn;

        /* clear */
        u32_val = ts_cpl_readl(pst_chn->u32_reg_addr + DMAC_CHN_OFFSET_INT_STATUS);
        ts_cpl_writel(u32_val, (pst_chn->u32_reg_addr + DMAC_CHN_OFFSET_INT_CLEAR));

        /* 唤醒 */
        pst_chn->u32_int_status = u32_val;
        pst_wait = (ts_cpl_wait_s_t *)&pst_chn->st_wait;
        ts_cpl_wakeup(pst_wait);
    }
}

static ts_cpl_irqreturn_en_t dmac_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    dmac_dev_s_t *pst_dmac_dev;
    ts_u32 u32_chns, u32_chn;

    pst_dmac_dev = (dmac_dev_s_t *)pv_dev;
    u32_chns = tdl_dmac_read_int_status(pst_dmac_dev);
    u32_chn = 0;

    while (1) {
        if (0 == u32_chns)
            break;
        
        if ((u32_chns & 0x01) > 0)
            tdl_dmac_irq_hdl(pst_dmac_dev, u32_chn);

        u32_chns = u32_chns >> 1;
        u32_chn++;
    }

    return TS_CPL_IRQ_HANDLED;
}


static ts_s32 dmac_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_s32 dmac_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_long dmac_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_cpl_dev;
    ts_u32 u32_ioctl_nr, *pu32_dev;
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
        case IOC_NR_DMAC_INIT:
            pu32_dev = (ts_u32 *)ulg_arg;
            if (pu32_dev != ts_null)
                *pu32_dev = (ts_u32)pst_cpl_dev->pv_private;
            else
                lg_ret = -TS_EINVAL;

            break;
        case IOC_NR_DMAC_DEINIT:

            break;
     
        default :
            lg_ret = TS_ERR;

            break;
    }

    return lg_ret;
}


static ts_cpl_fileops_s_t sst_dmac_v100_ops = {
    .open = dmac_v100_open,
    .release = dmac_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = dmac_v100_ioctl,
};


/* 
 * 函数名 :  tdl_dmac_free_priv
 * 描  述 :  释放申请的私有缓存
 * 输  入 :  参数               描述
 *           pst_dev           dmac 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void tdl_dmac_free_priv(dmac_dev_s_t *pst_dev)
{
    ts_s32 i, s32_chn_num;
    dmac_chn_s_t *pst_chns;

    if (pst_dev->pv_priv != ts_null) {
        pst_chns = (dmac_chn_s_t *)pst_dev->pv_priv;
        s32_chn_num = (ts_s32)pst_dev->u16_chn_num;
        for (i = 0; i < s32_chn_num; i++) {
            ts_cpl_wait_destroy((ts_cpl_wait_s_t *)&pst_chns->st_wait);

            pst_chns++;
        }

        ts_cpl_kfree(pst_dev->pv_priv);
    }
}


/* 
 * 函数名 :  tdl_dmac_alloc_priv
 * 描  述 :  按照通道个数申请私有缓冲
 * 输  入 :  参数               描述
 *           pst_dev           dmac 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            申请失败
 *           非 ts_null         申请成功
 */
ts_void * tdl_dmac_alloc_priv(dmac_dev_s_t *pst_dev)
{
    ts_void *pv_priv;
    ts_s32 i, s32_chn_num;
    dmac_chn_s_t *pst_chns;
    ts_u32 u32_offset;

    s32_chn_num = (ts_s32)pst_dev->u16_chn_num;
    pv_priv = (ts_void *)ts_cpl_kmalloc((sizeof(dmac_chn_s_t) * s32_chn_num), TS_CPL_GFP_KERNEL);
    if (pv_priv != ts_null) {
        pst_chns = (dmac_chn_s_t *)pv_priv;
        u32_offset = DMAC_TG_CHAN_OFFSET_BASE;
        for (i = 0; i < s32_chn_num; i++) {
            pst_chns->u32_reg_addr = pst_dev->u32_regbase + u32_offset;
            pst_chns->u16_id = i;
            pst_chns->u16_busy = DMAC_CHN_IDLE;
            pst_chns->pv_priv = (ts_void *)pst_dev;

            ts_cpl_wait_init((ts_cpl_wait_s_t *)&pst_chns->st_wait);

            u32_offset += DMAC_TG_CHAN_OFFSET_BASE;
            pst_chns++;
        }
    }

    return pv_priv;
}


static ts_s32 dmac_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 u32_regbase;
    ts_cpl_res_s_t st_res;
    ts_s32 s32_ret, s32_irq_no, s32_chn_num;
    ts_cpl_dev_s_t *pst_cpl_dev;
    dmac_dev_s_t *pst_dmac_dev;

    pst_cpl_dev = ts_cpl_dev_register("/dev/dmac", (ts_cpl_fileops_s_t *)&sst_dmac_v100_ops, sizeof(dmac_dev_s_t));
    if (ts_null == pst_cpl_dev) {
        ts_kapi_log_err("Failed to register dev for dmac\n");

        return -TS_ENOMEM;
    }

    s32_chn_num = ts_cpl_plat_get_value(pst_drv, "chn-num", 0, 0);
    if (0 == s32_chn_num) {
        s32_ret = -TS_ENXIO;

        goto _probe_get_err;
    }

    s32_irq_no = ts_cpl_plat_get_irq(pst_drv, 0);
    if (s32_irq_no < 0) {
        s32_ret = -TS_ENXIO;

        goto _probe_get_err;
    }

    s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, 0);
    if (s32_ret != TS_OK)
        goto _probe_get_err;

    pst_dmac_dev = (dmac_dev_s_t *)pst_cpl_dev->pv_private;
    u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
    
    pst_dmac_dev->u32_regbase = u32_regbase;

    if (ts_true == ts_cpl_plat_get_bool(pst_drv, "ecc"))
        pst_dmac_dev->u08_support_ecc = 1;
    else
        pst_dmac_dev->u08_support_ecc = 0;
    
    pst_dmac_dev->u08_port_num = (ts_u08)ts_cpl_plat_get_value(pst_drv, "port-num", 0, 1);

    pst_dmac_dev->u16_chn_num = (ts_u16)s32_chn_num;
    pst_dmac_dev->u16_bus_width = (ts_u16)ts_cpl_plat_get_value(pst_drv, "bus-width", 0, 64);
    pst_dmac_dev->u16_burst_len = (ts_u16)ts_cpl_plat_get_value(pst_drv, "burst-len", 0, 64);

    /* 注册中断 */
    ts_cpl_request_irq(s32_irq_no, dmac_handler, 0, "dmac", pst_dmac_dev);

    s32_ret = ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&pst_dmac_dev->st_mutex);

    if (s32_ret != TS_OK) 
        goto _probe_get_err;

    /* 为dma通道申请内存并初始化 */
    pst_dmac_dev->pv_priv = tdl_dmac_alloc_priv(pst_dmac_dev);
    if (ts_null == pst_dmac_dev->pv_priv) {
        s32_ret = -TS_ENOMEM;

        goto _probe_get_err;
    }

    return TS_OK;

_probe_get_err:
    ts_cpl_dev_unregister(pst_cpl_dev);

    return s32_ret;
}


static ts_cpl_of_devid_s_t sst_dmac_drv_id = {
    .compatible = "tg,dmac_apb_v100",
};

static ts_cpl_plat_drv_s_t sst_dmac_drv = {
    .probe = dmac_v100_probe,
    .driver = {
        .name = "dmac",
        .of_match_table = &sst_dmac_drv_id,
    }
};

ts_s32 dmac_init(ts_void)
{
    return ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_dmac_drv);
}

ts_cpl_ins_module_init(dmac, dmac_init);

