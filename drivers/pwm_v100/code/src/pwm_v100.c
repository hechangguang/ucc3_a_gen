/* 
 * pwm_v100.c -- pwm v100 实现, 对外接口是标准设备驱动，内部使用固定接口调用，避免过多函数指针操作
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
#include <cpl/ts_cpl.h>
#include <kapi/k_base_v100.h>
#include <kapi/k_pwm_v100.h>

#include "pwm_tg_v100.h"

static ts_pwm_irqcb_attr_s_t *spst_pwn_irqcb_attr;

static ts_cpl_irqreturn_en_t qdr_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    ts_qdr_dev_s_t *pst_dev;
    ts_pwm_qdr_attr_s_t *pst_attr;

    pst_dev = (ts_qdr_dev_s_t *)pv_dev;
    pst_attr = (ts_pwm_qdr_attr_s_t *)&pst_dev->st_attr;

    tdl_qdr_hadl(pst_dev, pst_attr);

    if (pst_attr->fp_irqcb != ts_null)
        pst_attr->fp_irqcb(0, pst_attr->pv_priv);

    return TS_CPL_IRQ_HANDLED;
}


static ts_cpl_irqreturn_en_t pwm_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    ts_u32 i;
    ts_u32 u32_intsta_intr;
    ts_pwm_dev_s_t *pst_pwm_dev;

    pst_pwm_dev = (ts_pwm_dev_s_t *)pv_dev;
    for (i = 0; i < pst_pwm_dev->u32_channels_num; i++) {
        u32_intsta_intr = tdl_pwm_get_int_status(pst_pwm_dev, i);
        tdl_pwm_clear_int_status(pst_pwm_dev, i);
        if (PWM_INT_ACTIVE == u32_intsta_intr) {
            if (spst_pwn_irqcb_attr[i].fp_irqcb != ts_null)
                spst_pwn_irqcb_attr[i].fp_irqcb(i, spst_pwn_irqcb_attr[i].pv_priv);

            tdl_pwm_hadl_one_shot(pst_pwm_dev, i);
            tdl_pwm_hadl_wakeup(pst_pwm_dev, i);
        }
    }

    return TS_CPL_IRQ_HANDLED;
}


static ts_s32 pwm_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    ts_u32 u32_size;
    ts_s32 s32_ret;
    ts_cpl_dev_s_t *pst_dev;
    ts_pwm_dev_s_t *pst_pwm_dev;

    if (ts_null == pv_file)
        return TS_ERR;

    pst_dev = ts_cpl_get_dev(pv_file);
    pst_pwm_dev = pst_dev->pv_private;

    u32_size = pst_pwm_dev->u32_channels_num * sizeof(ts_pwm_irqcb_attr_s_t);
    spst_pwn_irqcb_attr = (ts_pwm_irqcb_attr_s_t *)ts_cpl_kmalloc(u32_size, TS_CPL_GFP_KERNEL);
    if (ts_null == spst_pwn_irqcb_attr)
        return -TS_ENOMEM;

    s32_ret = tdl_pwm_init(pst_pwm_dev, spst_pwn_irqcb_attr);

    return s32_ret;
}


static ts_s32 pwm_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    ts_cpl_dev_s_t *pst_dev;
    ts_pwm_dev_s_t *pst_pwm_dev;

    if (ts_null == pv_file)
        return TS_ERR;

    pst_dev = ts_cpl_get_dev(pv_file);
    pst_pwm_dev = pst_dev->pv_private;

    if (spst_pwn_irqcb_attr != ts_null) {
        for (int i = 0; i < pst_pwm_dev->u32_channels_num; i++) {
            spst_pwn_irqcb_attr[i].fp_irqcb = ts_null;
            spst_pwn_irqcb_attr[i].pv_priv = ts_null;
        }
        ts_cpl_kfree(spst_pwn_irqcb_attr);
        spst_pwn_irqcb_attr = ts_null;
    }

    tdl_pwm_deinit(pst_pwm_dev);

    return TS_OK;
}


static ts_long pwm_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_dev;
    ts_pwm_dev_s_t *pst_pwm_dev;
    ts_pwm_mode_s_t *pst_mode_arg;
    ts_pwm_freq_duty_s_t *pst_freq_duty_arg;
    ts_pwm_irqen_s_t *pst_irq_arg;
    ts_pwm_irqcb_attr_s_t *st_irqcb_attr;
    ts_pwm_output_s_t *pst_output_arg;
    ts_pwm_qdr_attr_s_t *pst_qdr_attr;
    ts_u32 *pu32_channels_num;
    ts_long lg_ret;
    ts_u32 u32_ioclt_nr, u32_val;
    qdr_ioctl_param_s_t st_param;

    if (ts_null == pv_file)
        return TS_ERR;

    pst_dev = ts_cpl_get_dev(pv_file);
    pst_pwm_dev = pst_dev->pv_private;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    lg_ret = TS_OK;
    u32_ioclt_nr = _TS_CPL_IOC_NR(u32_cmd);
    switch (u32_ioclt_nr) {
    case IOC_NR_PWM_SET_MODE:
        pst_mode_arg = (ts_pwm_mode_s_t *)ulg_arg;
        lg_ret = tdl_pwm_set_mode(pst_pwm_dev, pst_mode_arg->u32_channel, pst_mode_arg->en_mode,\
                                     pst_mode_arg->en_fpout);

        break;

    case IOC_NR_PWM_SET_FREQ_DUTY:
        pst_freq_duty_arg = (ts_pwm_freq_duty_s_t *)ulg_arg;
        lg_ret = tdl_pwm_set_freq_duty(pst_pwm_dev, pst_freq_duty_arg->u32_channel,\
            pst_freq_duty_arg->u32_freq, pst_freq_duty_arg->u32_duty, pst_freq_duty_arg->en_duty_adj);

        break;

    case IOC_NR_PWM_SET_IRQEN:
        pst_irq_arg = (ts_pwm_irqen_s_t *)ulg_arg;
        tdl_pwm_set_irqen(pst_pwm_dev, pst_irq_arg->u32_channel, pst_irq_arg->en_irqen);

        break;

    case IOC_NR_PWM_SET_CALLBACK:
        st_irqcb_attr = (ts_pwm_irqcb_attr_s_t *)ulg_arg;
        spst_pwn_irqcb_attr[st_irqcb_attr->u32_channel].fp_irqcb = st_irqcb_attr->fp_irqcb;
        spst_pwn_irqcb_attr[st_irqcb_attr->u32_channel].pv_priv = st_irqcb_attr->pv_priv;

        break;

    case IOC_NR_PWM_START:
        pst_output_arg = (ts_pwm_output_s_t *)ulg_arg;
        lg_ret = tdl_pwm_start_timeout(pst_pwm_dev, pst_output_arg->u32_channel, pst_output_arg->u32_period_count, 0);

        break;

    case IOC_NR_PWM_START_TIMEOUT:
        pst_output_arg = (ts_pwm_output_s_t *)ulg_arg;
        lg_ret = tdl_pwm_start_timeout(pst_pwm_dev, pst_output_arg->u32_channel, pst_output_arg->u32_period_count,\
                                             pst_output_arg->u32_timeout_ms);

        break;

    case IOC_NR_PWM_STOP:
        tdl_pwm_stop(pst_pwm_dev, (ts_u32)ulg_arg);

        break;

    case IOC_NR_PWM_GET_CHN_NUM:
        pu32_channels_num = (ts_u32 *)ulg_arg;
        *pu32_channels_num = pst_pwm_dev->u32_channels_num;

        break;
    
    case IOC_NR_PWM_SET_QDR_ATTR:
    case IOC_NR_PWM_GET_QDR_ATTR:
        pst_qdr_attr = (ts_pwm_qdr_attr_s_t *)ulg_arg;

        lg_ret = (ts_long)tdl_qdr_do_attr(pst_pwm_dev, pst_qdr_attr, u32_ioclt_nr);

        break;
    
    case IOC_NR_PWM_SET_QDR_CCNT:
    case IOC_NR_PWM_GET_QDR_CNT:
    case IOC_NR_PWM_SET_QDR_CVAL:
        ts_cpl_copy_from_user((ts_void *)&st_param, (ts_void *)ulg_arg, sizeof(qdr_ioctl_param_s_t));
        if (IOC_NR_PWM_SET_QDR_CCNT == u32_ioclt_nr) {
            lg_ret = (ts_long)tdl_qdr_set_ccnt(pst_pwm_dev, st_param.u32_id, st_param.u32_val);
        } else if (IOC_NR_PWM_GET_QDR_CNT == u32_ioclt_nr) {
            lg_ret = (ts_long)tdl_qdr_get_cnt(pst_pwm_dev, st_param.u32_id, (ts_u32 *)&u32_val);
            st_param.u32_val = u32_val;

            ts_cpl_copy_to_user((ts_void *)ulg_arg, (ts_void *)&st_param, sizeof(qdr_ioctl_param_s_t));
        } else {
            lg_ret = (ts_long)tdl_qdr_set_cval(pst_pwm_dev, st_param.u32_id, st_param.u32_val);
        }

        break;

    default :
        lg_ret = TS_ERR;

        break;
    }

    return lg_ret;
}


static ts_cpl_fileops_s_t sst_pwm_v100_ops = {
    .open = pwm_v100_open,
    .release = pwm_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = pwm_v100_ioctl,
};


static ts_s32 pwm_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 i;
    ts_cpl_res_s_t st_res;
    ts_s32 s32_ret, s32_irq_no, s32_qdr_irqno;
    ts_s32 as32_irq_no[QDR_MAX_NUM];
    ts_cpl_dev_s_t *pst_dev;
    ts_pwm_dev_s_t *pst_pwm_dev;
    ts_qdr_dev_s_t *pst_qdr_dev;

    pst_dev = ts_cpl_dev_register("/dev/pwm", (ts_cpl_fileops_s_t *)&sst_pwm_v100_ops, sizeof(ts_pwm_dev_s_t));
    if (ts_null == pst_dev) {
        ts_kapi_log_err("Failed to register dev for /dev/pwm\n");

        return -TS_ENOMEM;
    }

    pst_pwm_dev = (ts_pwm_dev_s_t *)pst_dev->pv_private;

    /* confirm whether QDR is supported */
    pst_pwm_dev->u32_qdr_num = (ts_u32)ts_cpl_plat_get_value(pst_drv, "qdr-num", 0, 0);
    if (pst_pwm_dev->u32_qdr_num > QDR_MAX_NUM) {
        ts_kapi_log_warn("QDR num read %d, but the max num is %d\n", pst_pwm_dev->u32_qdr_num, QDR_MAX_NUM);

        pst_pwm_dev->u32_qdr_num = QDR_MAX_NUM;
    }

    s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, 0);
    s32_irq_no = ts_cpl_plat_get_irq(pst_drv, 0);
    if ((s32_ret != TS_OK) || (s32_irq_no < 0)) {
        s32_ret = -TS_ENODEV;
        ts_kapi_log_err("Failed to get source\n");

        goto __probe_err;
    }

    pst_pwm_dev->u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
    pst_pwm_dev->u32_channels_num = (ts_u32)ts_cpl_plat_get_value(pst_drv, "channels-num", 0, 0);
    pst_pwm_dev->u32_clk = ts_cpl_plat_get_clocks(pst_drv, 0);
    pst_pwm_dev->u32_fpout = 0;
    ts_cpl_request_irq(s32_irq_no, pwm_handler, 0, "pwm", pst_pwm_dev);

    /* read qdr regbase and irqno */
    s32_qdr_irqno = 0;
    if (pst_pwm_dev->u32_qdr_num > 0) {
        for (i = 0; i < pst_pwm_dev->u32_qdr_num; i++) {
            pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_pwm_dev->ast_qdr[i];
            s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, (i + 1));
            as32_irq_no[i] = ts_cpl_plat_get_irq(pst_drv, (i + 1));
            if ((s32_ret != TS_OK) || (as32_irq_no[i] < 0)) {
                s32_ret = -TS_ENODEV;
                ts_kapi_log_err("Failed to get qdr source\n");

                goto __probe_final_err;
            }

            pst_qdr_dev->u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
            ts_cpl_request_irq(as32_irq_no[i], qdr_handler, 0, "qdr", pst_qdr_dev);

            s32_qdr_irqno++;
        }
    }

    ts_kapi_log_notice("found pwm,and qdr=%d\n", pst_pwm_dev->u32_qdr_num);

    return TS_OK;

__probe_final_err:
    for (i = 0; i < s32_qdr_irqno; i++) {
        pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_pwm_dev->ast_qdr[i];
        ts_cpl_free_irq(as32_irq_no[i], pst_qdr_dev);
    }

    ts_cpl_free_irq(s32_irq_no, pst_pwm_dev);

__probe_err:
    ts_cpl_dev_unregister(pst_dev);

    return s32_ret;
}


static ts_cpl_of_devid_s_t sst_pwm_drv_id = {
    .compatible = "tg,pwm_v100",
};


static ts_cpl_plat_drv_s_t sst_pwm_drv = {
    .probe = pwm_v100_probe,
    .driver = {
        .name = "pwm",
        .of_match_table = &sst_pwm_drv_id,
    }
};


ts_s32 pwm_init(ts_void)
{
    ts_s32 s32_ret;

    s32_ret = ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_pwm_drv);

    return s32_ret;
}


ts_cpl_ins_module_init(pwm, pwm_init);
