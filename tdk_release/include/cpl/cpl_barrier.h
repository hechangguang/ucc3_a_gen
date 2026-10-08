/*
 * cpl_barrier.h -- tdk 内存屏障实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_BARRIER_H__
#define __CPL_BARRIER_H__

#define ts_cpl_barrier()          __asm__ __volatile__("": : :"memory")

extern ts_void ts_cpl_mb(ts_void);
extern ts_void ts_cpl_rmb(ts_void);
extern ts_void ts_cpl_wmb(ts_void);
extern ts_void ts_cpl_smp_mb(ts_void);
extern ts_void ts_cpl_smp_rmb(ts_void);
extern ts_void ts_cpl_smp_wmb(ts_void);
extern ts_void ts_cpl_isb(ts_void);
extern ts_void ts_cpl_dsb(ts_void);
extern ts_void ts_cpl_dmb(ts_void);

#endif /* __CPL_BARRIER_H__ */
