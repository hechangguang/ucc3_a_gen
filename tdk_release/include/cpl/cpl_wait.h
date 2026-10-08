/* 
 * cpl_wait.h -- cpl 等待实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_WAIT_H__
#define __CPL_WAIT_H__

#include <base/ts_type.h>

typedef ts_s32 (*ts_cpl_wait_cond_t)(ts_void *pv_param);

typedef struct ts_cpl_wait_s {
    ts_void *pv_wait;
} ts_cpl_wait_s_t;

//extern ts_ulong ts_cpl_msecs_to_jiffies(ts_u32 u32_msecs);

extern ts_s32 ts_cpl_wait_init(ts_cpl_wait_s_t *pst_wait);
extern ts_void ts_cpl_wait_destroy(ts_cpl_wait_s_t *pst_wait);

extern ts_s32 ts_cpl_wait_timeout_uninterruptible(ts_cpl_wait_s_t *pst_wait,
                       ts_cpl_wait_cond_t fp_func,ts_void *pv_param, ts_ulong ulg_ms);
extern ts_s32 ts_cpl_wait_timeout_interruptible(ts_cpl_wait_s_t *pst_wait,
                       ts_cpl_wait_cond_t fp_func, ts_void *pv_param, ts_ulong ulg_ms);

extern ts_void ts_cpl_wait_uninterruptible(ts_cpl_wait_s_t *pst_wait, 
						ts_cpl_wait_cond_t fp_func, ts_void *pv_param);
extern ts_s32 ts_cpl_wait_interruptible(ts_cpl_wait_s_t *pst_wait, 
						ts_cpl_wait_cond_t fp_func, ts_void *pv_param);


extern ts_void ts_cpl_wakeup(ts_cpl_wait_s_t *pst_wait);
extern ts_void ts_cpl_wake_up_interruptible(ts_cpl_wait_s_t *pst_wait);


#define ts_cpl_wait_event_interruptible(wait, func, param)                       \
({                                                                       \
    int __ret = 0;                                                   \
    if (!func(param)) {                                       \
        __ret = ts_cpl_wait_interruptible(wait, (func), param);    \
    }                                                               \
    __ret;                                                           \
})


#define ts_cpl_wait_event(wait, func, param)                     \
({                                                          \
    if (!func(param)) {                                       \
        ts_cpl_wait_uninterruptible(wait, (func), param);               \
    }                                                               \
})


#define ts_cpl_wait_event_timeout_interruptible(wait, func, param, timeout)      \
({                                                                       \
    long __ret = timeout;                                           \
    if (!func(param)) {                                       \
        __ret = ts_cpl_wait_timeout_interruptible(wait, (func), param, __ret);    \
    }                                                               \
    __ret;                                                           \
})

#define ts_cpl_wait_event_timeout_uninterruptible(wait, func, param, timeout)    \
({                                                                       \
    long __ret = timeout;                                           \
    if (!func(param)) {                                       \
        __ret = ts_cpl_wait_timeout_uninterruptible(wait, (func), param, __ret);    \
    }                                                               \
    __ret;                                                       \
})

#endif /* __CPL_WAIT_H__ */
