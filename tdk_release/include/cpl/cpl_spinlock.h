/* 
 * cpl_spinlock.h -- tdk spinlock 跨平台层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_SPINLOCK_H__
#define __CPL_SPINLOCK_H__

typedef struct ts_cpl_spinlock_s {
    ts_void *pv_spinlock;
} ts_cpl_spinlock_s_t;

extern ts_s32 ts_cpl_spinlock_init(ts_cpl_spinlock_s_t *pv_spinlock);
extern ts_void ts_cpl_spinlock_destroy(ts_cpl_spinlock_s_t *pv_spinlock);

extern ts_void ts_cpl_spin_lock_irqsave(ts_cpl_spinlock_s_t *pv_spinlock, ts_ulong *plg_flags);
extern ts_s32 ts_cpl_spin_trylock_irqsave(ts_cpl_spinlock_s_t *pv_spinlock, ts_ulong *plg_flags);
extern ts_void ts_cpl_spin_unlock_irqrestore(ts_cpl_spinlock_s_t *pv_spinlock, ts_ulong *plg_flags);

extern ts_void ts_cpl_spin_lock(ts_cpl_spinlock_s_t *pv_spinlock);
extern ts_s32 ts_cpl_spin_trylock(ts_cpl_spinlock_s_t *pv_spinlock);
extern ts_void ts_cpl_spin_unlock(ts_cpl_spinlock_s_t *pv_spinlock);

extern ts_void ts_cpl_enter_critical(ts_void);
extern ts_void ts_cpl_exit_critical(ts_void);

#endif /* __CPL_SPINLOCK_H__ */
