/* 
 * pwm_v100.h -- pwm v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_PWM_V100_H__
#define __K_PWM_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <uapi/chip_def.h>
#include <uapi/pwm_v100.h>
#include <cpl/ts_cpl.h>

#ifndef TS_PWM_MAX_NUM
#define PWM_MAX_NUM     (8)
#else
#define PWM_MAX_NUM     TS_PWM_MAX_NUM
#endif

#ifndef TS_QDR_MAX_NUM
#define QDR_MAX_NUM     (2)
#else
#define QDR_MAX_NUM     TS_QDR_MAX_NUM
#endif


typedef struct ts_qdr_dev_s {
    ts_u32 u32_regbase;

    ts_pwm_qdr_attr_s_t st_attr;
} ts_qdr_dev_s_t;


typedef struct ts_pwm_dev_s {
    ts_u32 u32_regbase;

    /* channel 的数量 */
    ts_u32 u32_channels_num;

    /* pwm 的时钟 */
    ts_u32 u32_clk;

    /* 按照每个 bit 一个通道保存 PFOUT 配置 */
    ts_u32 u32_fpout;

    /* 同步 */
    ts_cpl_semphr_s_t st_sema[PWM_MAX_NUM];

    /* qdr num, if = 0, the qdr disable */
    ts_u32 u32_qdr_num;

    /* qdr dev */
    ts_qdr_dev_s_t ast_qdr[QDR_MAX_NUM];
} ts_pwm_dev_s_t;


/* qdr ioctl */
typedef struct qdr_ioctl_param_s {
    ts_u32 u32_id;
    ts_u32 u32_val;
} qdr_ioctl_param_s_t;


/* ioctl */
typedef enum ts_pwm_ioctl_en {
    IOC_NR_PWM_SET_MODE = 0,
    IOC_NR_PWM_SET_FREQ_DUTY,
    IOC_NR_PWM_SET_IRQEN,
    IOC_NR_PWM_SET_CALLBACK,
    IOC_NR_PWM_START,
    IOC_NR_PWM_START_TIMEOUT,
    IOC_NR_PWM_STOP,
    IOC_NR_PWM_GET_CHN_NUM,

    /* qdr */
    IOC_NR_PWM_SET_QDR_ATTR,
    IOC_NR_PWM_GET_QDR_ATTR,
    IOC_NR_PWM_SET_QDR_CCNT,
    IOC_NR_PWM_SET_QDR_CVAL,
    IOC_NR_PWM_GET_QDR_CNT,

    IOC_NR_PWM_BUTT,
} ts_pwm_ioctl_en_t;

#define TS_PWM_IOCTL_SET_MODE       TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_MODE, ts_pwm_mode_s_t)
#define TS_PWM_IOCTL_SET_FREQ_DUTY  TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_FREQ_DUTY, ts_pwm_freq_duty_s_t)
#define TS_PWM_IOCTL_SET_IRQEN      TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_IRQEN, ts_pwm_irqen_s_t)
#define TS_PWM_IOCTL_SET_CALLBACK   TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_CALLBACK, ts_pwm_irqcb_attr_s_t)
#define TS_PWM_IOCTL_START          TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_START, ts_pwm_output_s_t)
#define TS_PWM_IOCTL_START_TIMEOUT  TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_START_TIMEOUT, ts_pwm_output_s_t)
#define TS_PWM_IOCTL_STOP           TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_STOP, ts_u32)
#define TS_PWM_IOCTL_GET_CHN_NUM    TS_CPL_IOR(TS_IOC_MAGIC_PWM, IOC_NR_PWM_GET_CHN_NUM, ts_u32)

#define TS_PWM_IOCTL_SET_QDR_ATTR   TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_QDR_ATTR, ts_pwm_qdr_attr_s_t)
#define TS_PWM_IOCTL_GET_QDR_ATTR   TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_GET_QDR_ATTR, ts_pwm_qdr_attr_s_t)
#define TS_PWM_IOCTL_SET_QDR_CCNT   TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_QDR_CCNT, ts_u32)
#define TS_PWM_IOCTL_SET_QDR_CVAL   TS_CPL_IOW(TS_IOC_MAGIC_PWM, IOC_NR_PWM_SET_QDR_CVAL, ts_u32)
#define TS_PWM_IOCTL_GET_QDR_CNT    TS_CPL_IOR(TS_IOC_MAGIC_PWM, IOC_NR_PWM_GET_QDR_CNT, ts_u32)

#define TS_EPWMMODE                 TS_DEFINE_ERRNO(TS_LOG_ERR, TS_IOC_MAGIC_PWM, 0x01)
#define TS_EPWMMODE_STR             "Pwm wrong mode"

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
ts_u32 tdl_pwm_get_int_status(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel);

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
ts_void tdl_pwm_clear_int_status(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel);

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
ts_void tdl_pwm_hadl_wakeup(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel);

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
ts_s32 tdl_pwm_init(ts_pwm_dev_s_t *pst_dev, ts_pwm_irqcb_attr_s_t *ppst_irqcb_attr);

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
ts_void tdl_pwm_deinit(ts_pwm_dev_s_t *pst_dev);

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
                                 ts_pwm_fpout_en_t en_fpout);

/* 
 * 函数名 :  tdl_pwm_set_freq_duty
 * 描  述 :  驱动层设置 pwm 指定的通道的周期和占空比。
 * 输  入 :  参数                描述
 *           pst_dev            pwm 设备
 *           u32_channel        pwm 指定的通道
 *           u32_freq           pwm 输出波形的频率
 *           u32_duty           pwm 的占空比
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
    ts_u32 u32_freq, ts_u32 u32_duty, ts_pwm_duty_adj_en_t en_duty_adj);

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
ts_void tdl_pwm_set_irqen(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, ts_pwm_irqen_en_t en_irqen);


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
ts_s32 tdl_pwm_start_timeout(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel, ts_u32 u32_period_count,\
                                 ts_u32 u32_timeout_ms);

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
ts_void tdl_pwm_stop(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel);

ts_s32 tdl_qdr_do_attr(ts_pwm_dev_s_t *pst_dev, ts_pwm_qdr_attr_s_t *pst_attr, ts_u32 u32_cmd);
ts_s32 tdl_qdr_set_ccnt(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 u32_ccnt);
ts_s32 tdl_qdr_set_cval(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 u32_cval);
ts_s32 tdl_qdr_get_cnt(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_ch, ts_u32 *pu32_cnt);
ts_void tdl_qdr_hadl(ts_qdr_dev_s_t *pst_dev, ts_pwm_qdr_attr_s_t *pst_attr);
ts_void tdl_pwm_hadl_one_shot(ts_pwm_dev_s_t *pst_dev, ts_u32 u32_channel);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_PWM_V100_H__ */
