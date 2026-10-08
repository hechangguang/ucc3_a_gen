/*
 * cpl_mutex.h -- tdk 跨平台 mutex 实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_MUTEX_H__
#define __CPL_MUTEX_H__

typedef struct ts_cpl_mutex_s {
    ts_void * pv_mutex;
} ts_cpl_mutex_s_t;

extern ts_s32 ts_cpl_mutex_init(ts_cpl_mutex_s_t *pst_mutex);
extern ts_void ts_cpl_mutex_destory(ts_cpl_mutex_s_t *pst_mutex);

extern ts_s32 ts_cpl_mutex_lock(ts_cpl_mutex_s_t *pst_mutex);
extern ts_s32 ts_cpl_mutex_lock_interruptible(ts_cpl_mutex_s_t *pst_mutex);
extern ts_s32 ts_cpl_mutex_trylock(ts_cpl_mutex_s_t *pst_mutex);
extern ts_void ts_cpl_mutex_unlock(ts_cpl_mutex_s_t *pst_mutex);

#endif /* __CPL_MUTEX_H__ */
