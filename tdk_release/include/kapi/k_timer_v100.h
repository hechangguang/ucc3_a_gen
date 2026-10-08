/* 
 * k_timer_v100.h -- timer_v100 驱动层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_TIMER_V100_H__
#define __K_TIMER_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>

#include <uapi/timer_v100.h>


#define HWT_MAX_NUM     (4)

/* ioctl */
typedef enum ts_timer_ioctl_en {
    IOC_NR_TIMER_INIT = 0,
    IOC_NR_TIMER_DEINIT = 1,

    IOC_NR_GPIO_BUTT,
} ts_timer_ioctl_en_t;


#define TS_IOCTL_IOC_NR_TIMER_INIT       TS_CPL_IOW(TS_IOC_MAGIC_TIMER, IOC_NR_TIMER_INIT, ts_u32)
#define TS_IOCTL_IOC_NR_TIMER_DEINIT     TS_CPL_IOW(TS_IOC_MAGIC_TIMER, IOC_NR_TIMER_DEINIT, ts_u32)

/* 初始、重载值 */
typedef union hwt_load_count_un {
    ts_u32 u32_val;
} hwt_load_count_un_t;


/* 当前计数 */
typedef union hwt_cur_count_un {
    ts_u32 u32_val;
} hwt_cur_count_un_t;

/* 控制寄存器 */
typedef union hwt_ctrl_un {
    struct  {
        /* 使能 */
        ts_u32 enable:1;            //[0..0]

        /* 模式 0：自由运行模式；   1：周期模式*/
        ts_u32 mode:1;              //[1..1]

        /* 中断屏蔽 */
        ts_u32 intrpt_mask:1;    //[2..2]
   
        /* Reserved */
        ts_u32 rev03_31:29;         //[3..31]
    };
    ts_u32 u32_val;
} hwt_ctrl_un_t;


/* 中断读清 */
typedef union hwt_eoi_un {
    ts_u32 u32_val;
} hwt_eoi_un_t;

/* 中断状态 */
typedef union hwt_intrpt_un {
    struct  {
        /* 状态 */
        ts_u32 status:1;            //[0..0]

        /* Reserved */
        ts_u32 rev01_31:31;         //[1..31]
    };
    ts_u32 u32_val;
} hwt_intrpt_un_t;

typedef struct hwt_reg_s {
    __IOM hwt_load_count_un_t un_load_count;
    __IOM hwt_cur_count_un_t un_cur_count;
    __IOM hwt_ctrl_un_t un_ctrl;
    __IOM hwt_eoi_un_t un_eoi;
    __IOM hwt_intrpt_un_t un_intrpt;
} hwt_reg_s_t;


typedef struct hwt_sub_dev_s {
    /* 子设备寄存器地址*/
    ts_u32 u32_reg;

    /* 子设备状态 */
    ts_u32 u32_busy;

    /* 用于临界处理 */
    ts_cpl_spinlock_s_t st_lock;

    /* 定时器回调函数 */
    ts_hwt_cb_fp_t fp_cb;

    /* 回调参数 */
    ts_void *pv_param;
} hwt_sub_dev_s_t;


typedef struct hwt_ctx_s {

    ts_u32 u32_us_clk;

    /* 统一互斥 */
    ts_cpl_mutex_s_t st_mutex;

    /* 子设备 */
    hwt_sub_dev_s_t ast_sub_dev[HWT_MAX_NUM];
} hwt_ctx_s_t;





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_TIMER_V100_H__ */
