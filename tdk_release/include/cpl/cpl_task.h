/* 
 * cpl_task.h -- cpl 任务实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_TASK_H__
#define __CPL_TASK_H__

/* 获取任务信息 */
typedef struct ts_cpl_task_cfg_s {
    /* 保存在系统中的任务名称 */
    ts_char *pc_name;
    ts_u32 u32_sp_base;
    ts_u32 u32_sp_size;
    ts_u32 u32_priority;
} ts_cpl_task_cfg_s_t;

typedef struct ts_cpl_task_s {
    ts_void *pv_task;
} ts_cpl_task_s_t;

typedef ts_s32 (*ts_cpl_kthread_t)(ts_void *pv_data);

extern ts_s32 ts_cpl_kthread_create(ts_cpl_task_s_t *pv_task, ts_cpl_kthread_t fp_thread, ts_void *pv_data,
                    const ts_char *pc_name, ts_s32 s32_prio, ts_u32 u32_stack_size);

extern ts_s32 ts_cpl_kthread_tmm_create(ts_cpl_task_s_t *pv_task, ts_cpl_kthread_t fp_thread, ts_void *pv_data,
                    const ts_char *pc_name, ts_s32 s32_prio, ts_u32 u32_stack_size, ts_void *pv_tmm);

extern ts_void ts_cpl_kthread_destroy(ts_cpl_task_s_t *pv_task, ts_u32 u32_stop_flag);
extern ts_void ts_cpl_kthread_bind(ts_cpl_task_s_t *pv_task, ts_u32 u32_cpu);
extern ts_s32 ts_cpl_kthread_should_stop(ts_void);

extern ts_s32 ts_cpl_get_task_cfg(ts_cpl_task_cfg_s_t *pst_cfg);

#endif /* __CPL_TASK_H__ */
