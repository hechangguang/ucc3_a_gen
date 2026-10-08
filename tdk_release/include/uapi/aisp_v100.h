/* 
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_AISP_V100_H__
#define __API_AISP_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/aisp_comm.h>


typedef enum ts_npu_ops_en {
    /* npu 关闭 */
    TS_NPU_OPS_DISABLE = 0,

    /* 超分应用 */
    TS_NPU_OPS_SR = 1,

    /* AISP 应用 */
    TS_NPU_OPS_AISP = 2,

    TS_NPU_OPS_BUTT
} ts_npu_ops_en_t;


/* 定义旋转方向 */
#define TS_NPU_ROTATE_DISABLE       (0)
#define TS_NPU_ROTATE_R90           (1)
#define TS_NPU_ROTATE_L90           (2)

#define TS_NPU_MODEL_SIZE           (96 * 1024)

typedef struct ts_aisp_attr_s {
    /* npu 任务类型 */
    ts_npu_ops_en_t en_ops;

    /* npu 时钟 */
    ts_u32 u32_npu_clk;

    /* 模型句柄 */
    ts_void *pv_model;

    /* 模型大小 */
    ts_u32 u32_model_size;

    /* npu 任务个数 */
    ts_u16 u16_task_num;

    /* npu 子任务节点个数 */
    ts_u16 u16_task_node_num;

    /* 运行方式， 0: 连续模式， >0: 触发模式 */
    ts_u08 u08_trig_en;

    /* 旋转方向, 选配： TS_NPU_ROTATE_DISABLE TS_NPU_ROTATE_R90 TS_NPU_ROTATE_L90 */
    ts_u08 u08_rotate_dir;

    /* 初始运行状态 */
    ts_u08 u08_run_en;

    /* 开启 ROI, 仅对感兴趣区域做超分或者 AISP */
    ts_u08 u08_roi_en;

    /* ROI 坐标与宽高 */
    ts_u16 u16_roi_left;
    ts_u16 u16_roi_top;
    ts_u16 u16_roi_width;
    ts_u16 u16_roi_height;

    /* AISP 模式下，开启坏斑功能， 需要与 extbp */
    ts_u08 u08_extbp_en;

    /* 补黑边大小 */
    ts_u08 u08_pad_lines;

    ts_u16 u16_rev;
    ts_u32 u32_rev;
} ts_aisp_attr_s_t;

typedef struct ts_aisp_tri_task_attr_s {
    /* npu 时钟， 为0 则维持现状不做修改 */
    ts_u32 u32_npu_clk;

    /* npu 任务个数， 为0则维持现状不做修改 */
    ts_u32 u16_task_num;

    /* 模型句柄 */
    ts_void *pv_model;

    /* 模型大小 */
    ts_u32 u32_model_size;

    /* npu 子任务节点个数 */
    ts_u16 u16_task_node_num;

    ts_u16 u16_rev;

    /* 兼容保留 */
    ts_u32 au32_rev[2];
} ts_aisp_tri_task_attr_s_t;

ts_s32 ts_tdk_aisp_create(ts_u32 u32_prio, ts_u32 u32_stack_size, ts_aisp_attr_s_t *pst_attr, ts_u32 u32_start_dly_ms);

ts_s32 ts_tdk_aisp_set_sr_status(ts_u32 u32_on);

ts_s32 ts_tdk_aisp_set_rotate_dir(ts_u32 u32_dir);

ts_s32 ts_tdk_aisp_set_npu_status(ts_u32 u32_on);

ts_s32 ts_tdk_aisp_set_roi_clk(ts_u32 u32_clk);

ts_s32 ts_tdk_aisp_set_roi_coor(ts_u32 u32_roi_en, ts_u32 u32_left, ts_u32 u32_top, ts_u32 u32_width, ts_u32 u32_height);

ts_s32 ts_tdk_aisp_tri_task_create(ts_mdev_t h_aisp_dev, ts_aisp_tri_task_attr_s_t *pst_attr, ts_handle_t *ph_task);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_AISP_V100_H__ */

