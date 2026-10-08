/* 
 * pwm_tg_v100.c -- pwm_tg_v100 驱动实现
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

#include <linux/io.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_base_v100.h>
#include <kapi/k_pwm_v100.h>

#include "pwm_tg_v100.h"


TS_STATIC_INLINE ts_u32 pwm_read_reg_bits(ts_u32 u32_regbase, ts_u32 u32_mask, ts_u32 u32_offset)
{
    return (readl(u32_regbase) & u32_mask) >> u32_offset;
}


TS_STATIC_INLINE ts_void pwm_write_reg_bits(ts_u32 u32_regbase, ts_u32 u32_mask, ts_u32 u32_offset, ts_u32 u32_val)
{
    writel(((readl(u32_regbase) & (~u32_mask)) | (u32_val << u32_offset)), u32_regbase);
}


TS_STATIC_INLINE ts_s32 verify_pwm_state(ts_u32 u32_regbase, ts_u32 u32_channel)
{
    ts_s32 s32_ret;
    ts_u32 u32_reg_sta, u32_sta_busy;

    u32_reg_sta = u32_regbase + PWM_TG_OFFSET_STA + PWM_CHANNEL_REG(u32_channel);
    u32_sta_busy = pwm_read_reg_bits(u32_reg_sta, PWM_TG_REG_STA_BUSY_MASK, PWM_TG_REG_STA_BUSY_OFFSET);
    if (PWM_BUSY == u32_sta_busy)
        s32_ret = -TS_EBUSY;
    else
        s32_ret = TS_OK;

    return s32_ret;
}


TS_STATIC_INLINE ts_u32 rounding(ts_u32 u32_dividend, ts_u32 u32_divisor, ts_pwm_duty_adj_en_t en_duty_adj)
{
    ts_u32 u32_quotient;
    ts_bool u32_rounding_flag;

    if (0 == u32_dividend % u32_divisor) {
        u32_quotient = u32_dividend / u32_divisor;
    } else {
        if (TS_PWM_DUTY_ROUNDING == en_duty_adj) {
            u32_quotient = (u32_dividend << 1) / u32_divisor;
            u32_rounding_flag = u32_quotient & 1;
            if (u32_rounding_flag)
                u32_quotient = (u32_quotient + 1) >> 1;
            else
                u32_quotient >>= 1;
        } else if (TS_PWM_DUTY_ROUND_DOWN == en_duty_adj) {
            u32_quotient = u32_dividend / u32_divisor;
        } else {
            u32_quotient = (u32_dividend / u32_divisor) + 1;
        }
    }

    return u32_quotient;
}


/* 
 * 函数名 :  tdl_pwm_get_int_status
 * 描  述 :  驱动层获取 pwm 中断状态。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           ts_u32             pwm 中断状态
 */
ts_u32 tdl_pwm_get_int_status(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel)
{
    ts_u32 u32_reg_intsta;

    u32_reg_intsta = pst_dev->u32_regbase + PWM_TG_OFFSET_INTSTA + PWM_CHANNEL_REG(u32_channel);

    return pwm_read_reg_bits(u32_reg_intsta, PWM_TG_REG_INTSTA_INTR_MASK, PWM_TG_REG_INTSTA_INTR_OFFSET);
}


/* 
 * 函数名 :  tdl_pwm_clear_int_status
 * 描  述 :  驱动层清除 pwm 中断标志。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           无                  无
 */
ts_void tdl_pwm_clear_int_status(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel)
{
    ts_u32 u32_reg_intsta;

    u32_reg_intsta = pst_dev->u32_regbase + PWM_TG_OFFSET_INTSTA + PWM_CHANNEL_REG(u32_channel);
    pwm_write_reg_bits(u32_reg_intsta, PWM_TG_REG_INTSTA_RAWINTR_MASK,\
                             PWM_TG_REG_INTSTA_RAWINTR_OFFSET, PWM_CLEAR_INTSTA);
}



/* 
 * 函数名 :  tdl_pwm_hadl_wakeup
 * 描  述 :  驱动层处理 pwm 唤醒。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           无                  无
 */
ts_void tdl_pwm_hadl_wakeup(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&pst_dev->st_sema[u32_channel]);
}


ts_void tdl_qdr_hadl(ts_qdr_dev_s_t *pst_dev, ts_pwm_qdr_attr_s_t *pst_attr)
{
    ts_u32 u32_regaddr;
    qdr_work_ctrl_un_t un_work_ctl;

    u32_regaddr = pst_dev->u32_regbase;

    /* 关闭中断，避免 cval 和 ccnt 正好一致时连续产生中断 */
    if (pst_attr->un_ctrl.auto_clr_en > 0) {
        un_work_ctl.u32_val = ts_cpl_readl(u32_regaddr + 0x0000);
        un_work_ctl.int_en = 0;
        ts_cpl_writel(un_work_ctl.u32_val, (u32_regaddr + 0x0000));
    }

    ts_cpl_writel(1, (u32_regaddr + 0x0010));
}


/* 
 * 函数名 :  tdl_pwm_hadl_one_shot
 * 描  述 :  驱动层处理 pwm ONE-SHOT 模式。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           无                  无
 */
ts_void tdl_pwm_hadl_one_shot(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel)
{
    ts_u32 u32_reg_cr, u32_cr_mode;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);
    u32_cr_mode = pwm_read_reg_bits(u32_reg_cr, PWM_TG_REG_CR_MODE_MASK, PWM_TG_REG_CR_MODE_OFFSET);

    if (TS_PWM_MODE_ONESHOT == u32_cr_mode) {
        tdl_pwm_stop(pst_dev, u32_channel);
    }
}


/* 
 * 函数名 :  tdl_pwm_init
 * 描  述 :  驱动层按照默认配置初始化。
 * 输  入 :  参数               描述
 *           pst_dev           pwm 设备
 *           ppst_irqcb_attr   回调函数属性指针
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK             初始化成功
 */
ts_s32 tdl_pwm_init(ts_pwm_dev_s_t *pst_dev, ts_pwm_irqcb_attr_s_t *ppst_irqcb_attr)
{
    ts_u32 i, u32_channels_num, u32_reg_data_up;

    u32_channels_num = pst_dev->u32_channels_num;

    for (i = 0; i < u32_channels_num; i++) {
        ppst_irqcb_attr[i].fp_irqcb = ts_null;
        ppst_irqcb_attr[i].pv_priv = ts_null;
        ppst_irqcb_attr[i].u32_channel = i;

        /* 设置所有通道的数据更新为使能状态 */
        u32_reg_data_up = pst_dev->u32_regbase + PWM_TG_OFFSET_DATA_UP + (i * PWM_CHANNEL_REG_OFFSET);
        pwm_write_reg_bits(u32_reg_data_up, PWM_TG_OFFSET_DATA_UP, PWM_CHANNEL_REG_OFFSET, PWM_DATA_UP_ENABLE);

        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&(pst_dev->st_sema[i]), 0);
    }

    return TS_OK;
}


/* 
 * 函数名 :  tdl_pwm_deinit
 * 描  述 :  驱动层取消初始化。
 * 输  入 :  参数               描述
 *           pst_dev           pwm 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK             初始化成功
 */
ts_void tdl_pwm_deinit(ts_pwm_dev_s_t *pst_dev)
{
    for (int i = 0; i < pst_dev->u32_channels_num; i++)
        ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&pst_dev->st_sema[i]);
}


/* 
 * 函数名 :  tdl_pwm_set_mode
 * 描  述 :  驱动层设置 pwm 指定的通道为 ONE-SHOT 模式。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 *           en_mode            pwm 的工作模式
 *           en_fpout           pwm 第一个相位的输出电平
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           TS_OK              设置模式成功
 *           TS_EBUSY           pwm 设备忙
 */
ts_s32 tdl_pwm_set_mode(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, ts_pwm_mode_en_t en_mode,\
                                 ts_pwm_fpout_en_t en_fpout)
{
    ts_s32 s32_ret;
    ts_u32 u32_reg_cr, u32_mbit;

    /* 若 pwm 通道为 busy 状态则直接返回 TS_EBUSY */
    s32_ret = verify_pwm_state(pst_dev->u32_regbase, u32_channel);
    if (s32_ret != TS_OK)
        return s32_ret;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);

    /* 若 pwm 工作模式为 CONTINUOUS， 则关闭中断使能 */
    if (TS_PWM_MODE_CONTINUOUS == en_mode)
        pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_INTEN_MASK, PWM_TG_REG_CR_INTEN_OFFSET, PWM_INT_INACTIVE);
    
    pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_MODE_MASK, PWM_TG_REG_CR_MODE_OFFSET, en_mode);
    pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_FPOUT_MASK, PWM_TG_REG_CR_FPOUT_OFFSET, en_fpout);
    pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_INACTOUT_MASK, PWM_TG_REG_CR_INACTOUT_OFFSET, en_fpout);

    u32_mbit = 1 << u32_channel;
    if (TS_PWM_PFOUT_LOW == en_fpout)
        pst_dev->u32_fpout &= ~u32_mbit;
    else
        pst_dev->u32_fpout |= u32_mbit;

    return TS_OK;
}


/* 
 * 函数名 :  tdl_pwm_set_freq_duty
 * 描  述 :  驱动层设置 pwm 指定的通道的周期和占空比。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 *           u32_freq           pwm 输出波形的频率(单位：Hz)
 *           u32_duty           pwm 的占空比（0-100）
 *           en_duty_adj        占空比有误差时的微调配置
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           TS_OK              设置周期和占空比成功
 *           TS_EBUSY           pwm 设备忙
 *           TS_EPWMMODE        pwm 模式错误
 *           TS_ERR             设置周期和占空比失败
 */
ts_s32 tdl_pwm_set_freq_duty(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, \
    ts_u32 u32_freq, ts_u32 u32_duty, ts_pwm_duty_adj_en_t en_duty_adj)
{
    ts_u32 u32_reg_cr, u32_reg_per, u32_reg_fp;
    ts_u32 u32_cr_fpout, u32_fp, u32_mbit;
    ts_u32 u32_period;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);
    u32_reg_per = pst_dev->u32_regbase + PWM_TG_OFFSET_PER + PWM_CHANNEL_REG(u32_channel);
    u32_reg_fp = pst_dev->u32_regbase + PWM_TG_OFFSET_FP + PWM_CHANNEL_REG(u32_channel);

    //u32_cr_fpout = pwm_read_reg_bits(u32_reg_cr, PWM_TG_REG_CR_FPOUT_MASK, PWM_TG_REG_CR_FPOUT_OFFSET);
    u32_mbit = 1 << u32_channel;
    if ((pst_dev->u32_fpout & u32_mbit) > 0)
        u32_cr_fpout = TS_PWM_PFOUT_HIGH;
    else
        u32_cr_fpout = TS_PWM_PFOUT_LOW;

    u32_period = rounding(pst_dev->u32_clk, u32_freq, TS_PWM_DUTY_ROUNDING);

    /* Avoiding the initial phase of PWM will inevitably lead to a clock issue */
    if ((0 == u32_duty) || (100 == u32_duty)) {
        u32_fp = u32_period;
        if (0 == u32_duty) {
            if (TS_PWM_PFOUT_LOW == u32_cr_fpout)
                u32_cr_fpout = TS_PWM_PFOUT_HIGH;
            else
                u32_cr_fpout = TS_PWM_PFOUT_LOW;
        }
    } else {
        if (TS_PWM_PFOUT_LOW == u32_cr_fpout)
            u32_fp = rounding((u32_period * (DUTY_MAX - u32_duty)), 100, en_duty_adj);
        else
            u32_fp = rounding((u32_period * u32_duty), 100, en_duty_adj);
    }

    pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_FPOUT_MASK, PWM_TG_REG_CR_FPOUT_OFFSET, u32_cr_fpout);

    writel((u32_period - 1), u32_reg_per);
    writel((u32_fp - 1), u32_reg_fp);

    return TS_OK;
}


/* 
 * 函数名 :  tdl_pwm_set_irqen
 * 描  述 :  驱动层设置 pwm 中断。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 *           en_irqen           pwm 使能/失能中断
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           无                  无
 */
ts_void tdl_pwm_set_irqen(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, ts_pwm_irqen_en_t en_irqen)
{
    ts_u32 u32_reg_cr, u32_cr_mode;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);
    u32_cr_mode = pwm_read_reg_bits(u32_reg_cr, PWM_TG_REG_CR_MODE_MASK, PWM_TG_REG_CR_MODE_OFFSET);

    if (u32_cr_mode != TS_PWM_MODE_CONTINUOUS)
        pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_INTEN_MASK, PWM_TG_REG_CR_INTEN_OFFSET, en_irqen);
    else
        pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_INTEN_MASK, PWM_TG_REG_CR_INTEN_OFFSET, PWM_INT_INACTIVE);
}


static ts_s32 qdr_chk_param(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_char *pc_call, ts_u32 u32_call_line)
{
    if (0 == pst_dev->u32_qdr_num) {
        ts_cpl_printk("[%s@%d]: QDR not supported!\n", pc_call, u32_call_line);

        return TS_ERR;
    }

    if (u32_ch >= QDR_MAX_NUM) {
        ts_cpl_printk("[%s@%d]: Invalid channel number(%d) > %d!\n", pc_call, u32_call_line, u32_ch, QDR_MAX_NUM);

        return TS_ERR;
    }

    return TS_OK;
}


ts_s32 tdl_qdr_set_ccnt(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 u32_ccnt)
{
    ts_qdr_dev_s_t *pst_qdr_dev;
    ts_u32 u32_regaddr;
    ts_s32 s32_ret;

    s32_ret = qdr_chk_param(pst_dev, u32_ch, (ts_char *)__func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_dev->ast_qdr[u32_ch];

    u32_regaddr = pst_qdr_dev->u32_regbase;
    ts_cpl_writel(u32_ccnt, (u32_regaddr + 0x000c));

    //ts_cpl_printk("set : ch = %d, reg = %#x\n", u32_ch, u32_regaddr);

    /* 保存到内部参数中 */
    pst_qdr_dev->st_attr.u32_compare_cnt = u32_ccnt;

    return TS_OK;
}

ts_s32 tdl_qdr_set_cval(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 u32_cval)
{
    ts_qdr_dev_s_t *pst_qdr_dev;
    ts_u32 u32_regaddr;
    ts_s32 s32_ret;

    s32_ret = qdr_chk_param(pst_dev, u32_ch, (ts_char *)__func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_dev->ast_qdr[u32_ch];

    u32_regaddr = pst_qdr_dev->u32_regbase;
    ts_cpl_writel(u32_cval, (u32_regaddr + 0x0008));

    //ts_cpl_printk("set : ch = %d, reg = %#x\n", u32_ch, u32_regaddr);

    /* 保存到内部参数中 */
    pst_qdr_dev->st_attr.u32_center_val = u32_cval;

    return TS_OK;
}


ts_s32 tdl_qdr_get_cnt(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 *pu32_cnt)
{
    ts_qdr_dev_s_t *pst_qdr_dev;
    ts_u32 u32_val, u32_regaddr;
    ts_s32 s32_ret;

    s32_ret = qdr_chk_param(pst_dev, u32_ch, (ts_char *)__func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_dev->ast_qdr[u32_ch];
    u32_regaddr = pst_qdr_dev->u32_regbase;
    u32_regaddr += 0x0008;
    u32_val = ts_cpl_readl(u32_regaddr);
    if (pst_qdr_dev->st_attr.un_ctrl.auto_clr_en > 0)
        ts_cpl_writel(pst_qdr_dev->st_attr.u32_center_val, u32_regaddr);

    *pu32_cnt = u32_val;

    //ts_cpl_printk("get : ch = %d, reg = %#x\n", u32_ch, u32_regaddr);

    return TS_OK;
}


ts_s32 tdl_qdr_do_attr(ts_pwm_dev_s_t *pst_dev, ts_pwm_qdr_attr_s_t *pst_attr, ts_u32 u32_cmd)
{
    ts_qdr_dev_s_t *pst_qdr_dev;
    ts_u32 u32_regaddr;
    ts_s32 s32_ret;
    qdr_work_ctrl_un_t un_work_ctl;
    qdr_cnt_ctrl_un_t un_cnt_ctl;

    s32_ret = qdr_chk_param(pst_dev, pst_attr->u32_ch, (ts_char *)__func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_qdr_dev = (ts_qdr_dev_s_t *)&pst_dev->ast_qdr[pst_attr->u32_ch];

    if (IOC_NR_PWM_GET_QDR_ATTR == u32_cmd) {
        ts_cpl_copy_to_user((ts_void *)pst_attr, (ts_void *)&pst_qdr_dev->st_attr, sizeof(ts_pwm_qdr_attr_s_t));
    } else {
        u32_regaddr = pst_qdr_dev->u32_regbase;

        //ts_cpl_printk("ch = %d, reg = %#x\n", pst_attr->u32_ch, u32_regaddr);

        /* clear int */
        //ts_cpl_writel(0x01, (u32_regaddr + 0x0008));
        ts_cpl_writel(0x01, (u32_regaddr + 0x0010));

        /* work ctrl */
        un_work_ctl.u32_val = 0;
        un_work_ctl.work_en = pst_attr->un_ctrl.work_en;
        un_work_ctl.int_en = pst_attr->un_ctrl.interrupt_en;
        un_work_ctl.work_mode = pst_attr->un_ctrl.work_mode;
        un_work_ctl.deglitch_en = pst_attr->un_ctrl.deglitch_en;
        un_work_ctl.pa_cnt_en = pst_attr->un_ctrl.pa_cnt_en;
        un_work_ctl.pb_cnt_en = pst_attr->un_ctrl.pb_cnt_en;

        /* cnt ctrl */
        un_cnt_ctl.u32_val = 0;
        un_cnt_ctl.pa_oppo_level = pst_attr->un_ctrl.pa_oppo_level;
        un_cnt_ctl.pa_posedge_en = pst_attr->un_ctrl.pa_posedge_en;
        un_cnt_ctl.pa_negedge_en = pst_attr->un_ctrl.pa_negedge_en;

        un_cnt_ctl.pb_oppo_level = pst_attr->un_ctrl.pb_oppo_level;
        un_cnt_ctl.pb_posedge_en = pst_attr->un_ctrl.pb_posedge_en;
        un_cnt_ctl.pb_negedge_en = pst_attr->un_ctrl.pb_negedge_en;

        /* compare cnt */
        ts_cpl_writel(pst_attr->u32_center_val, (u32_regaddr + 0x0008));
        ts_cpl_writel(pst_attr->u32_compare_cnt, (u32_regaddr + 0x000c));
        ts_cpl_writel(un_work_ctl.u32_val, (u32_regaddr + 0x0000));
        ts_cpl_writel(un_cnt_ctl.u32_val, (u32_regaddr + 0x0004));

        //ts_cpl_printk("ctl : %#x, r %#x\n", un_cnt_ctl.u32_val, pst_attr->un_ctrl.u32_val);

        /* clear */
        //ts_cpl_writel(0x01, (u32_regaddr + 0x0008));
        ts_cpl_writel(0x01, (u32_regaddr + 0x0010));

        ts_cpl_copy_from_user((ts_void *)&pst_qdr_dev->st_attr, (ts_void *)pst_attr, sizeof(ts_pwm_qdr_attr_s_t));
    }

    return TS_OK;
}


/* 
 * 函数名 :  tdl_pwm_start_timeout
 * 描  述 :  驱动层开始 pwm 输出。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 *           u32_period_count   pwm ONE-SHOT 模式输出的周期数
 *           u32_timeout_ms     pwm 超时时间（单位：ms）
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           TS_OK              启动 pwm 成功
 *           TS_EPWMMODE           pwm 模式错误
 */
ts_s32 tdl_pwm_start_timeout(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, ts_u32 u32_period_count, \
                                 ts_u32 u32_timeout_ms)
{
    ts_s32 s32_ret;
    ts_u32 u32_reg_cr, u32_cr_mode, u32_reg_rpt, u32_clk_count;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);
    u32_cr_mode = pwm_read_reg_bits(u32_reg_cr, PWM_TG_REG_CR_MODE_MASK, PWM_TG_REG_CR_MODE_OFFSET);
    
    if (TS_PWM_MODE_ONESHOT == u32_cr_mode ) {
        if (0 == u32_period_count) {
            ts_cpl_printk("In one-shot mode,the period_count must be greater than 0.\n");

            return -TS_EINVAL;
        }
            
        u32_reg_rpt = pst_dev->u32_regbase + PWM_TG_OFFSET_RPT + PWM_CHANNEL_REG(u32_channel);
        u32_clk_count = u32_period_count;

        pwm_write_reg_bits(u32_reg_rpt, PWM_TG_REG_RPT_RPT_MASK, PWM_TG_REG_RPT_RPT_OFFSET, (u32_clk_count - 1));
        pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_START_MASK, PWM_TG_REG_CR_START_OFFSET, PWM_START);

        if (u32_timeout_ms > 0) {
            s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&pst_dev->st_sema[u32_channel], u32_timeout_ms);
            if (TS_ERR == s32_ret)
                return -TS_ETIMEDOUT;
        }

        return TS_OK;
    } else if (TS_PWM_MODE_CONTINUOUS == u32_cr_mode) {
        pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_START_MASK, PWM_TG_REG_CR_START_OFFSET, PWM_START);
    } else {
        return -TS_EPWMMODE;
    }

    return TS_OK;
}


/* 
 * 函数名 :  tdl_pwm_stop
 * 描  述 :  驱动层停止 pwm 输出。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 * 输  出 :  参数                描述
 *           无                  无
 * 返  回 :  返回值              描述
 *           无                  无
 */
ts_void tdl_pwm_stop(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel)
{
    ts_u32 u32_reg_cr;

    u32_reg_cr = pst_dev->u32_regbase + PWM_TG_OFFSET_CR + PWM_CHANNEL_REG(u32_channel);

    pwm_write_reg_bits(u32_reg_cr, PWM_TG_REG_CR_START_MASK, PWM_TG_REG_CR_START_OFFSET, PWM_STOP);
}
