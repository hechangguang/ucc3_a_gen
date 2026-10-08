/* 
 * lvgl_v100.h -- lvgl 移植接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_LVGL_V100_H__
#define __API_LVGL_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_media.h>

/* lvgl 旋转角度 */
typedef enum osd_angle_en {
    OSD_ANGLE_0 = 0,
    OSD_ANGLE_90,
    OSD_ANGLE_180,
    OSD_ANGLE_270,
} osd_angle_en_t;


typedef struct lvgl_osd_attr_s {
    /* 宽高 */
    ts_u32 u32_width;
    ts_u32 u32_height;

    /* 数字通道左顶点坐标 */
    ts_u32 u32_disp_top;
    ts_u32 u32_disp_left;

    /* 模拟通道左顶点坐标 */
    ts_u32 u32_cvbs_top;
    ts_u32 u32_cvbs_left;

    /* osd通道 */
    ts_u32 u32_chn;

    ts_u32 u32_angle;

    ts_rgb_s_t *pst_rgbs;
} lvgl_osd_attr_s_t;


/* --- function list --- */
typedef void (*ts_lvgl_tick_inc)(ts_u32);


ts_void ts_tdk_lvgl_disp_init(ts_rgb_s_t *pst_rgbs);

ts_void ts_tdk_lvgl_disp_flush(ts_s32 x1, ts_s32 y1, ts_s32 x2, ts_s32 y2, ts_u08 *pu08_map);

ts_s32 ts_tdk_lvgl_init(lvgl_osd_attr_s_t *pst_attr);

ts_void ts_tdk_lvgl_start_tick(ts_lvgl_tick_inc fp_tick_inc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_OSD_V100_H__ */

