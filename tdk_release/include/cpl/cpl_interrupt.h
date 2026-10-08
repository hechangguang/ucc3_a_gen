/* 
 * cpl_interrupt.h -- ts_cpl 中断操作实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_INTERRUPT_H__
#define __CPL_INTERRUPT_H__

#define TS_CPL_IRQF_TRIGGER_NONE          (0x00000000)
#define TS_CPL_IRQF_TRIGGER_RISING        (0x00000001)
#define TS_CPL_IRQF_TRIGGER_FALLING       (0x00000002)
#define TS_CPL_IRQF_TRIGGER_HIGH          (0x00000004)
#define TS_CPL_IRQF_TRIGGER_LOW           (0x00000008)
#define TS_CPL_IRQF_TRIGGER_MASK          (TS_CPL_IRQF_TRIGGER_HIGH | TS_CPL_IRQF_TRIGGER_LOW | \
                                            TS_CPL_IRQF_TRIGGER_RISING | TS_CPL_IRQF_TRIGGER_FALLING)
#define TS_CPL_IRQF_TRIGGER_PROBE         (0x00000010)

typedef enum ts_cpl_irqreturn_en {
    TS_CPL_IRQ_NONE         = (0 << 0),
    TS_CPL_IRQ_HANDLED      = (1 << 0),
    TS_CPL_IRQ_WAKE_THREAD  = (1 << 1),

    TS_CPL_BUTT
} ts_cpl_irqreturn_en_t;

typedef struct ts_cpl_tasklet_s {
    ts_void *pv_tasklet;
    ts_void (*fp_func)(ts_ulong);
    ts_ulong lg_data;
} ts_cpl_tasklet_s_t;

typedef ts_cpl_irqreturn_en_t (*ts_cpl_irq_handler_t)(ts_s32, ts_void *);

extern ts_s32 ts_cpl_request_irq(ts_u32 u32_irq_no, ts_cpl_irq_handler_t fp_handler,
                ts_ulong lg_flags, const ts_char *pc_name, ts_void *pv_dev);
extern ts_s32 ts_cpl_request_threaded_irq(ts_u32 u32_irq_no, ts_cpl_irq_handler_t fp_handler,
              ts_cpl_irq_handler_t fp_thread, ts_ulong lg_flags, const ts_char *pc_name, ts_void *pv_dev);

extern ts_void ts_cpl_free_irq(ts_u32 u32_irq_no, ts_void *pv_dev);
extern ts_s32 ts_cpl_in_interrupt(ts_void);

extern ts_s32 ts_cpl_tasklet_init(ts_cpl_tasklet_s_t *pst_tasklet);
extern ts_s32 ts_cpl_tasklet_kill(ts_cpl_tasklet_s_t *pst_tasklet);
extern ts_s32 ts_cpl_tasklet_schedule(ts_cpl_tasklet_s_t *pst_tasklet);

#endif /* __CPL_INTERRUPT_H__ */
