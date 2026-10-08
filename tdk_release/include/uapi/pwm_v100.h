/* 
 * pwm_v100.h -- pwm v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_PWM_V100_H__
#define __API_PWM_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>

#include <cpl/cpl_ioctl.h>

#define DUTY_MAX    100
#define TIMEOUT_MAX 0xFFFFFFFF

/* pwm 中断回调函数定义 */
typedef ts_void (*ts_pwm_irqcb_fp_t) (ts_u32, ts_void *);

/* pwm 工作模式 */
typedef enum ts_pwm_mode_en {
    /* ONE-SHOT 模式 */
    TS_PWM_MODE_ONESHOT = 1,

    /* CONTINUOUS 模式 */
    TS_PWM_MODE_CONTINUOUS,

    TS_PWM_MODE_BUTT
} ts_pwm_mode_en_t;

/* pwm 初始相位枚举 */
typedef enum ts_pwm_fpout_en {
    /* 输出第一相位为低电平 */
    TS_PWM_PFOUT_LOW = 0,

    /* 输出第一相位为高电平 */
    TS_PWM_PFOUT_HIGH,

    TS_PWM_PFOUT_BUTT
} ts_pwm_fpout_en_t;

/* pwm 占空比微调枚举 */
typedef enum ts_pwm_duty_adj_en {
    /* 四舍五入调整 */
    TS_PWM_DUTY_ROUNDING = 0,

    /* 向下取整调整 */
    TS_PWM_DUTY_ROUND_DOWN,

    /* 向上取整调整 */
    TS_PWM_DUTY_ROUND_UP,

    TS_PWM_DUTY_BUTT
} ts_pwm_duty_adj_en_t;

/* pwm 中断开关枚举 */
typedef enum ts_pwm_irqen_en {
    /* 打开 pwm 中断 */
    TS_PWM_IRQ_DISABLE = 0,

    /* 关闭 pwm 中断 */
    TS_PWM_IRQ_ENABLE,

    TS_PWM_IRQ_EN_BUTT
} ts_pwm_irqen_en_t;

/* pwm 模式 */
typedef struct ts_pwm_mode_s {
    /* pwm 通道 */
    ts_u32 u32_channel;

    /* pwm 工作模式 */
    ts_pwm_mode_en_t en_mode;

    /* pwm 第一个相位的输出电平 */
    ts_pwm_fpout_en_t en_fpout;
} ts_pwm_mode_s_t;

/* pwm 频率、占空比配置 */
typedef struct ts_pwm_freq_duty_s {
    /* pwm 通道 */
    ts_u32 u32_channel;

    /* pwm 输出波形的频率 */
    ts_u32 u32_freq;

    /* pwm 占空比 */
    ts_u32 u32_duty;

    /* pwm 占空比有误差时的微调配置 */
    ts_pwm_duty_adj_en_t en_duty_adj;
} ts_pwm_freq_duty_s_t;

/* pwm 中断使能配置 */
typedef struct ts_pwm_irq_s {
    /* pwm 通道*/
    ts_u32 u32_channel;

    /* 打开/关闭 pwm 中断 */
    ts_pwm_irqen_en_t en_irqen;
} ts_pwm_irqen_s_t;

/* pwm 中断回调属性 */
typedef struct ts_pwm_irqcb_attr_s {
    /* pwm 通道 */
    ts_u32 u32_channel;

    /* pwm 中断回调函数 */
    ts_pwm_irqcb_fp_t fp_irqcb;

    /* pwm 中断回调函数的私有参数 */
    void *pv_priv;
} ts_pwm_irqcb_attr_s_t;

/* pwm 输出配置 */
typedef struct ts_pwm_output_s {
    /* pwm 通道 */
    ts_u32 u32_channel;

    /* pwm ONE-SHOT 模式输出的周期数 */
    ts_u32 u32_period_count;

    /* pwm 超时时间（单位：ms） */
    ts_u32 u32_timeout_ms;
} ts_pwm_output_s_t;


typedef union qdr_ctrl_un {
    struct {
        /* 工作使能，高有效 */
        ts_u32 work_en:1;

        /* 中断使能，高有效; 计数比较模式 */
        ts_u32 interrupt_en:1;

        /* 工作模式，0x0：单纯计数模式；0x1：计数比较模式 */
        ts_u32 work_mode:1;

        /* auto clear count */
        ts_u32 auto_clr_en:1;

        /* 去毛刺使能，高有效 */
        ts_u32 deglitch_en:1;

        /* A相计数使能，高有效 */
        ts_u32 pa_cnt_en:1;

        /* B相计数使能，高有效 */
        ts_u32 pb_cnt_en:1;

        /* pa相反信号的电平选择；0x0：低电平；0x1：高电平 */
        ts_u32 pa_oppo_level:1;

        /* pa上升沿计数使能，高有效 */
        ts_u32 pa_posedge_en:1;

        /* pa下降沿计数使能，高有效 */
        ts_u32 pa_negedge_en:1;

        /* pb相反信号的电平选择；0x0：低电平；0x1：高电平 */
        ts_u32 pb_oppo_level:1;

        /* pb上升沿计数使能，高有效 */
        ts_u32 pb_posedge_en:1;

        /* pb下降沿计数使能，高有效 */
        ts_u32 pb_negedge_en:1;
    };
    ts_u32 u32_val;
} qdr_ctrl_un_t;

/* qdr attr */
typedef struct ts_pwm_qdr_attr_s {
    /* qdr dev channel */
    ts_u32 u32_ch;

    /* mode and param */
    qdr_ctrl_un_t un_ctrl;

    /* 计数比较模式有效 */
    ts_u32 u32_compare_cnt;

    /* 中心值 */
    ts_u32 u32_center_val;

    /* qdr 中断回调函数 */
    ts_pwm_irqcb_fp_t fp_irqcb;
    ts_void *pv_priv;
} ts_pwm_qdr_attr_s_t;

/* API */
extern ts_s32 ts_lib_pwm_set_mode(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_mode_en_t en_mode,\
                                     ts_pwm_fpout_en_t en_fpout);
extern ts_s32 ts_lib_pwm_set_freq_duty(ts_s32 s32_handle, ts_u32 u32_channel, \
                                    ts_u32 u32_freq, ts_u32 u32_duty, ts_pwm_duty_adj_en_t en_duty_adj);

extern ts_s32 ts_lib_pwm_set_irqen(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_irqen_en_t en_irqen);
extern ts_s32 ts_lib_pwm_set_callback(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_irqcb_fp_t fp_cb, \
                                    ts_void *pv_priv);

extern ts_s32 ts_lib_pwm_start(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_period_count);
extern ts_s32 ts_lib_pwm_start_timeout(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_period_count,\
                                         ts_u32 u32_timeout_ms);
extern ts_s32 ts_lib_pwm_stop(ts_s32 s32_handle, ts_u32 u32_channel);

extern ts_s32 ts_lib_pwm_open(ts_char *pc_dev_name);
extern ts_void ts_lib_pwm_close(ts_s32 s32_handle);

/* Advanced API */
extern ts_s32 ts_lib_pwm_set_qdr_attr(ts_s32 s32_handle, ts_pwm_qdr_attr_s_t *pst_qdr_attr);
extern ts_s32 ts_lib_pwm_get_qdr_attr(ts_s32 s32_handle, ts_pwm_qdr_attr_s_t *pst_qdr_attr);
extern ts_s32 ts_lib_pwm_set_qdr_compare_cnt(ts_s32 s32_handle, ts_u32 u32_ch, ts_u32 u32_cnt);
extern ts_s32 ts_lib_pwm_set_qdr_center_val(ts_s32 s32_handle, ts_u32 u32_ch, ts_u32 u32_cnt);
extern ts_s32 ts_lib_pwm_get_qdr_cnt(ts_s32 s32_handle, ts_u32 u32_ch, ts_u32 *pu32_cnt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_PWM_V100_H__ */
