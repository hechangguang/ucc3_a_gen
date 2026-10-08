/* 
 * cpl_atomic.h -- cpl 原子操作实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_ATOMIC_H__
#define __CPL_ATOMIC_H__

typedef struct ts_cpl_atomic_s {
    ts_void *pv_atomic;
} ts_cpl_atomic_s_t;

extern ts_s32 ts_cpl_atomic_init(ts_cpl_atomic_s_t *pv_atomic);
extern ts_void ts_cpl_atomic_destroy(ts_cpl_atomic_s_t *pv_atomic);
extern ts_s32 ts_cpl_atomic_read(ts_cpl_atomic_s_t *pv_atomic);
extern ts_void ts_cpl_atomic_set(ts_cpl_atomic_s_t *pv_atomic, ts_s32 s32_val);
extern ts_void ts_cpl_atomic_inc(ts_cpl_atomic_s_t *pv_atomic);
extern ts_s32 ts_cpl_atomic_inc_return(ts_cpl_atomic_s_t *pv_atomic);
extern ts_void ts_cpl_atomic_dec(ts_cpl_atomic_s_t *pv_atomic);
extern ts_s32 ts_cpl_atomic_dec_return(ts_cpl_atomic_s_t *pv_atomic);

#endif /* __CPL_ATOMIC_H__ */
