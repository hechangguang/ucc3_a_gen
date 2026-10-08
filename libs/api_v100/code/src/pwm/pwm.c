/* 
 * pwm.c -- OPEN TDK 对 pwm 驱动的封装，实现 pwm 的功能
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
#include <uapi/pwm_v100.h>
#include <kapi/k_pwm_v100.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

static ts_u32 su32_channels_num;

/* 
 * 函数名 :  ts_lib_pwm_set_mode
 * 描  述 :  设置 pwm 工作模式
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          en_mode              pwm 的工作模式
 *          en_fpout             pwm 第一个相位的输出电平
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                设置工作模式成功
 *          TS_EINVAL            参数错误
 *          TS_EBUSY             pwm 设备忙
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_set_mode(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_mode_en_t en_mode, ts_pwm_fpout_en_t en_fpout)
{
    ts_s32 s32_ret;
    ts_pwm_mode_s_t st_mode_arg;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num) || (en_mode >= TS_PWM_MODE_BUTT)\
                 || (en_fpout >= TS_PWM_PFOUT_BUTT)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_mode_arg.u32_channel = u32_channel;
        st_mode_arg.en_mode = en_mode;
        st_mode_arg.en_fpout = en_fpout;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_MODE, (ts_long)&st_mode_arg);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_set_freq_duty
 * 描  述 :  设置 pwm 的频率和占空比
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          u32_freq             pwm 输出波形的频率(单位：Hz)
 *          u32_duty             占空比（0-100）
 *          en_duty_adj          占空比有误差时的微调配置
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                设置 pwm 周期和占空比成功
 *          TS_EINVAL            参数错误
 *          TS_EBUSY             pwm 设备忙
 *          TS_EPWMMODE          pwm 模式错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_set_freq_duty(ts_s32 s32_handle, ts_u32 u32_channel, \
    ts_u32 u32_freq, ts_u32 u32_duty, ts_pwm_duty_adj_en_t en_duty_adj)
{
    ts_s32 s32_ret;
    ts_pwm_freq_duty_s_t st_freq_duty_arg;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num) || (0 == u32_freq)\
         || (u32_duty > DUTY_MAX) || (en_duty_adj >= TS_PWM_DUTY_BUTT)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_freq_duty_arg.u32_channel = u32_channel;
        st_freq_duty_arg.u32_freq = u32_freq;
        st_freq_duty_arg.u32_duty = u32_duty;
        st_freq_duty_arg.en_duty_adj = en_duty_adj;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_FREQ_DUTY, (ts_long)&st_freq_duty_arg);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_set_irqen
 * 描  述 :  打开/关闭 pwm 指定通道的中断
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          en_irqen             打开/关闭 pwm 中断
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                打开/关闭中断成功
 *          TS_EINVAL            参数错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_set_irqen(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_irqen_en_t en_irqen)
{
    ts_s32 s32_ret;
    ts_pwm_irqen_s_t st_irq_arg;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num) || (en_irqen >= TS_PWM_IRQ_EN_BUTT)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_irq_arg.u32_channel = u32_channel;
        st_irq_arg.en_irqen = en_irqen;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_IRQEN, (ts_long)&st_irq_arg);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_set_callback
 * 描  述 :  设置 pwm 指定的通道的中断回调函数(只支持 RTOS 使用)
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          fp_cb                pwm 中断回调函数
 *          pv_priv              pwm 中断回调函数的私有参数
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                设置中断回调函数成功
 *          TS_EINVAL            参数错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_set_callback(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_irqcb_fp_t fp_cb, ts_void *pv_priv)
{
    ts_s32 s32_ret;
    ts_pwm_irqcb_attr_s_t st_irqcb_attr;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_irqcb_attr.u32_channel = u32_channel;
        st_irqcb_attr.fp_irqcb = fp_cb;
        st_irqcb_attr.pv_priv = pv_priv;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_CALLBACK, (ts_long)&st_irqcb_attr);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_start
 * 描  述 :  不阻塞的启动 pwm 输出
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          u32_period_count     pwm ONE-SHOT 模式输出的周期数（此参数在 CONTINUOUS 模式下无效）
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                启动 pwm 输出成功
 *          TS_EINVAL            参数错误
 *          TS_EPWMMODE          模式错误   
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_start(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_period_count)
{
    ts_s32 s32_ret;
    ts_pwm_output_s_t st_output_arg;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_output_arg.u32_channel = u32_channel;
        st_output_arg.u32_period_count = u32_period_count;
    
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_START, (ts_long)&st_output_arg);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_start_timeout
 * 描  述 :  阻塞的启动 pwm ONE-SHOT 模式输出
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 *          u32_period_count     pwm ONE-SHOT 模式输出的周期数
 *          u32_timeout_ms       pwm 超时时间（单位：ms）
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                启动 pwm 输出成功
 *          TS_EINVAL            参数错误
 *          TS_EPWMMODE          模式错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_start_timeout(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_period_count, ts_u32 u32_timeout_ms)
{
    ts_s32 s32_ret;
    ts_pwm_output_s_t st_output_arg;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num) || (0 == u32_period_count)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_output_arg.u32_channel = u32_channel;
        st_output_arg.u32_period_count = u32_period_count;

        if (0 == u32_timeout_ms)
            st_output_arg.u32_timeout_ms = TIMEOUT_MAX;
        else
            st_output_arg.u32_timeout_ms = u32_timeout_ms;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_START_TIMEOUT, (ts_long)&st_output_arg);
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_stop
 * 描  述 :  停止 pwm 输出
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          u32_channel          pwm 指定的通道
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                停止 pwm 输出成功
 *          TS_EINVAL            参数错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_stop(ts_s32 s32_handle, ts_u32 u32_channel)
{
    ts_s32 s32_ret;

    if ((s32_handle < 0) || (u32_channel >= su32_channels_num))
        s32_ret = -TS_EINVAL;
    else
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_STOP, (ts_long)u32_channel);

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_set_qdr_attr
 * 描  述 :  set qdr attr
 * 输  入 :  参数                 描述
 *          s32_handle           设备句柄
 *          pst_qdr_attr         qdr attr point
 * 输  出 :  参数                 描述
 *           无                   无
 * 返  回 :  返回值               描述
 *          TS_OK                停止 pwm 输出成功
 *          TS_EINVAL            参数错误
 *          TS_ERR               其他错误
 */
ts_s32 ts_lib_pwm_set_qdr_attr(ts_s32 s32_handle, ts_pwm_qdr_attr_s_t *pst_qdr_attr)
{
    ts_s32 s32_ret;

    if ((s32_handle < 0) || (ts_null == pst_qdr_attr))
        s32_ret = -TS_EINVAL;
    else
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_QDR_ATTR, (ts_long)pst_qdr_attr);

    return s32_ret;
}


ts_s32 ts_lib_pwm_get_qdr_attr(ts_s32 s32_handle, ts_pwm_qdr_attr_s_t *pst_qdr_attr)
{
    ts_s32 s32_ret;

    if ((s32_handle < 0) || (ts_null == pst_qdr_attr))
        s32_ret = -TS_EINVAL;
    else
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_GET_QDR_ATTR, (ts_long)pst_qdr_attr);

    return s32_ret;
}


ts_s32 ts_lib_pwm_set_qdr_compare_cnt(ts_s32 s32_handle, ts_u32 u32_qdr_id, ts_u32 u32_cnt)
{
    ts_s32 s32_ret;
    qdr_ioctl_param_s_t st_param;

    if (s32_handle < 0) {
        s32_ret = -TS_EINVAL;
    } else {
        st_param.u32_id = u32_qdr_id;
        st_param.u32_val = u32_cnt;
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_QDR_CCNT, (ts_long)&st_param);
    }

    return s32_ret;
}


ts_s32 ts_lib_pwm_set_qdr_center_val(ts_s32 s32_handle, ts_u32 u32_qdr_id, ts_u32 u32_cnt)
{
    ts_s32 s32_ret;
    qdr_ioctl_param_s_t st_param;

    if (s32_handle < 0) {
        s32_ret = -TS_EINVAL;
    } else {
        st_param.u32_id = u32_qdr_id;
        st_param.u32_val = u32_cnt;
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_SET_QDR_CVAL, (ts_long)&st_param);
    }

    return s32_ret;
}


ts_s32 ts_lib_pwm_get_qdr_cnt(ts_s32 s32_handle, ts_u32 u32_qdr_id, ts_u32 *pu32_cnt)
{
    ts_s32 s32_ret;
    qdr_ioctl_param_s_t st_param;

    if (s32_handle < 0) {
        s32_ret = -TS_EINVAL;
    } else {
        st_param.u32_id = u32_qdr_id;
        st_param.u32_val = 0;
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_GET_QDR_CNT, (ts_long)&st_param);
        if (TS_OK == s32_ret)
            *pu32_cnt = st_param.u32_val;
    }

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_pwm_open
 * 描  述 :  打开 pwm 设备
 * 输  入 :  参数           描述
 *          pc_dev_name    设备文件名
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           >= 0           设备句柄
 *           < 0            打开设备失败
 */
ts_s32 ts_lib_pwm_open(ts_char *pc_dev_name)
{
    ts_s32 s32_handle, s32_ret;
    ts_u32 u32_channels_num;

    s32_handle = open(pc_dev_name, O_RDWR);

    if (s32_handle >= 0) {
        s32_ret = (ts_s32)ioctl(s32_handle, TS_PWM_IOCTL_GET_CHN_NUM, (ts_long)&u32_channels_num);
        if (s32_ret != TS_OK)
            return s32_ret;
        else
            su32_channels_num = u32_channels_num;
    }

    return s32_handle;
}


/* 
 * 函数名 :  ts_lib_pwm_close
 * 描  述 :  关闭 pwm 设备句柄
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void ts_lib_pwm_close(ts_s32 s32_handle)
{
    if (s32_handle >= 0)
        close(s32_handle);
}
