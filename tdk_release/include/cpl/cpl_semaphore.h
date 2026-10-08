/* 
 * cpl_semaphore.h -- cpl 同步信号头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_SEMAPHORE_H__
#define __CPL_SEMAPHORE_H__

typedef struct ts_cpl_semphr_s {
    ts_void *pv_semphr;
} ts_cpl_semphr_s_t;

extern ts_s32 ts_cpl_semphr_init(ts_cpl_semphr_s_t *pst_semphr, ts_s32 s32_val);
extern ts_void ts_cpl_semphr_destory(ts_cpl_semphr_s_t *pst_semphr);

extern ts_s32 ts_cpl_down(ts_cpl_semphr_s_t *pst_semphr);
extern ts_s32 ts_cpl_down_interruptible(ts_cpl_semphr_s_t *pst_semphr);
extern ts_s32 ts_cpl_down_trylock(ts_cpl_semphr_s_t *pst_semphr);
extern ts_s32 ts_cpl_down_timeout(ts_cpl_semphr_s_t *pst_semphr, ts_long lg_ms);

extern ts_void ts_cpl_up(ts_cpl_semphr_s_t *pst_semphr);

#endif /* __CPL_SEMAPHORE_H__ */
