/* 
 * pwm_tg_v100.h -- pwm_tg_v100 驱动实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __PWM_TG_V100_H__
#define __PWM_TG_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#define PWM_CLEAR_INTSTA                    (0x1)
#define PWM_DATA_UP_ENABLE                  (0x1)

#define PWM_STOP                            (0x0)
#define PWM_START                           (0x1)

#define PWM_IDLE                            (0x0)
#define PWM_BUSY                            (0x1)

#define PWM_WAIT                            (0x0)
#define PWM_WAKEUP                          (0x1)

#define PWM_INT_INACTIVE                    (0x0)
#define PWM_INT_ACTIVE                      (0x1)

#define PWM_OUT_DONE                        (0x1)

/* pwm channel register address offset */
#define PWM_CHANNEL_REG_OFFSET              (0x0020)

/* channel register */
#define PWM_CHANNEL_REG(channel)            (channel * PWM_CHANNEL_REG_OFFSET)

/* register CR */
#define PWM_TG_OFFSET_CR                    (0x0000)

#define PWM_TG_REG_CR_START_MASK            (0x0001)
#define PWM_TG_REG_CR_START_OFFSET          (0)

#define PWM_TG_REG_CR_SRST_MASK             (0x0002)
#define PWM_TG_REG_CR_SRST_OFFSET           (1)

#define PWM_TG_REG_CR_INTEN_MASK            (0x0008)
#define PWM_TG_REG_CR_INTEN_OFFSET          (3)

#define PWM_TG_REG_CR_MODE_MASK             (0x0030)
#define PWM_TG_REG_CR_MODE_OFFSET           (4)

#define PWM_TG_REG_CR_FPOUT_MASK            (0x0100)
#define PWM_TG_REG_CR_FPOUT_OFFSET          (8)

#define PWM_TG_REG_CR_INACTOUT_MASK            (0x0200)
#define PWM_TG_REG_CR_INACTOUT_OFFSET          (9)

/* register RPT */
#define PWM_TG_OFFSET_RPT                      (0x0004)

#define PWM_TG_REG_RPT_RPT_MASK                (0xFFFF)
#define PWM_TG_REG_RPT_RPT_OFFSET              (0)

/* register PER */
#define PWM_TG_OFFSET_PER                      (0x0008)

#define PWM_TG_REG_PER_PER_MASK                (0xFFFFFFF)
#define PWM_TG_REG_PER_PER_OFFSET              (0)

/* register FP */
#define PWM_TG_OFFSET_FP                       (0x000C)

#define PWM_TG_REG_FP_FP_MASK                  (0xFFFFFFF)
#define PWM_TG_REG_FP_FP_OFFSET                (0)

/* register STA */
#define PWM_TG_OFFSET_STA                      (0x0010)

#define PWM_TG_REG_STA_CYCLE_MASK              (0xFFFF)
#define PWM_TG_REG_STA_CYCLE_OFFSET            (0)

#define PWM_TG_REG_STA_BUSY_MASK               (0x10000)
#define PWM_TG_REG_STA_BUSY_OFFSET             (16)

#define PWM_TG_REG_STA_OUT_MASK                (0x20000)
#define PWM_TG_REG_STA_OUT_OFFSET              (17)

/* register INTSTA */
#define PWM_TG_OFFSET_INTSTA                   (0x0014)

#define PWM_TG_REG_INTSTA_RAWINTR_MASK         (0x0001)
#define PWM_TG_REG_INTSTA_RAWINTR_OFFSET       (0)

#define PWM_TG_REG_INTSTA_INTR_MASK            (0x10000)
#define PWM_TG_REG_INTSTA_INTR_OFFSET          (16)

/* register DATA_UP */
#define PWM_TG_OFFSET_DATA_UP                  (0x0018)

#define PWM_TG_REG_DATA_UP_DATA_UP_MASK        (0x0001)
#define PWM_TG_REG_DATA_UP_DATA_UP_OFFSET      (0)

/* qdr */
typedef union qdr_work_ctrl_un {
    struct {
        /* 工作使能，高有效 */
        ts_u32 work_en:1;       //[0]

        /* 中断使能，高有效; 计数比较模式 */
        ts_u32 int_en:1;        //[1]

        ts_u32 rev02_03:2;      //[2..3]

        /* 工作模式，0x0：单纯计数模式；0x1：计数比较模式 */
        ts_u32 work_mode:1;     //[4]

        /* 去毛刺使能，高有效 */
        ts_u32 deglitch_en:1;   //[5]

        ts_u32 rev06_07:2;      //[6..7]

        /* A相计数使能，高有效 */
        ts_u32 pa_cnt_en:1;     //[8]

        /* B相计数使能，高有效 */
        ts_u32 pb_cnt_en:1;     //[9]

        ts_u32 rev10_31:22;     //[10..31]
    };
    ts_u32 u32_val;
} qdr_work_ctrl_un_t;


typedef union qdr_cnt_ctrl_un {
    struct {
        /* pa相反信号的电平选择；0x0：低电平；0x1：高电平 */
        ts_u32 pa_oppo_level:1;     //[0]

        /* pa上升沿计数使能，高有效 */
        ts_u32 pa_posedge_en:1;     //[1]

        /* pa下降沿计数使能，高有效 */
        ts_u32 pa_negedge_en:1;     //[2]

        ts_u32 rev03:1;             //[3]

        /* pb相反信号的电平选择；0x0：低电平；0x1：高电平 */
        ts_u32 pb_oppo_level:1;     //[4]

        /* pb上升沿计数使能，高有效 */
        ts_u32 pb_posedge_en:1;     //[5]

        /* pb下降沿计数使能，高有效 */
        ts_u32 pb_negedge_en:1;     //[6]

        ts_u32 rev07_31:25;         //[7..31]
    };
    ts_u32 u32_val;
} qdr_cnt_ctrl_un_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PWM_TG_V100_H__ */
