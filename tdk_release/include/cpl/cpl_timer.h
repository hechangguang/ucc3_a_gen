/* 
 * cpl_timer.h -- cpl 定时器实现头文件, 直接 freeRTOS 定时器，精度为 tick
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_TIMER_H__
#define __CPL_TIMER_H__

#include <base/ts_type.h>

typedef ts_void (*ts_cpl_timer_cb_t)(ts_void *pv_timer);

typedef struct ts_cpl_timer_s {
	ts_void *pv_timer;	
	ts_cpl_timer_cb_t fp_callback;
	ts_void *pv_priv;
} ts_cpl_timer_s_t;

typedef enum TS_CPL_TIMER_SYNC_E {
	TS_CPL_TIMER_ASYNC,
	TS_CPL_TIMER_SYNC
} TS_CPL_TIMER_SYNC_E_T;

extern ts_s32 ts_cpl_timer_init(ts_cpl_timer_s_t *pst_timer, ts_cpl_timer_cb_t fp_cb, ts_void *pv_priv);
extern ts_s32 ts_cpl_timer_destroy(ts_cpl_timer_s_t *pst_timer, TS_CPL_TIMER_SYNC_E_T en_sync);

extern ts_s32 ts_cpl_set_timer(ts_cpl_timer_s_t *pst_timer, ts_ulong ulg_interval);
extern ts_void ts_cpl_timer_set_priv(ts_cpl_timer_s_t *pst_timer, ts_void *pv_priv);
extern ts_void *ts_cpl_timer_get_priv(ts_cpl_timer_s_t *pst_timer);

#endif /* __CPL_TIMER_H__ */
