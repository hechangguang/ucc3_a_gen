/* 
 * cpl_workqueue.h -- cpl 工作队列实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_WORKQUEUE_H__
#define __CPL_WORKQUEUE_H__

typedef struct ts_cpl_wqueue_s {
    ts_void *pv_wqueue;
} ts_cpl_wqueue_s_t;

typedef ts_void (*ts_cpl_wqueue_handler_t)(ts_void *pv_priv);

enum {
    TS_CPL_WQ_MEM_RECLAIM   = 1 << 3,
    TS_CPL_WQ_HIGHPRI       = 1 << 4,
    TS_CPL_WQ_DFL_ACTIVE    = 256,
};

extern ts_void ts_cpl_set_workqueue_priv(ts_cpl_wqueue_s_t *pv_wqueue, ts_void *pv_priv);
extern ts_void *ts_cpl_get_workqueue_priv(ts_cpl_wqueue_s_t *pv_wqueue);

extern ts_s32 ts_cpl_alloc_workqueue(ts_cpl_wqueue_s_t *pv_wqueue, \
                    const ts_char *fmt, ts_u32 u32_flags, ts_s32 u32_max_active);
extern ts_s32 ts_cpl_init_work(ts_cpl_wqueue_s_t *pv_wqueue, ts_cpl_wqueue_handler_t fp_func, ts_void *pv_priv);
extern ts_void ts_cpl_destroy_workqueue(ts_cpl_wqueue_s_t *pv_wqueue);

extern ts_void ts_cpl_flush_workqueue(ts_cpl_wqueue_s_t *pv_wqueue);
extern ts_void ts_cpl_queue_work(ts_cpl_wqueue_s_t *pv_wqueue);



#endif /* __CPL_WORKQUEUE_H__ */
