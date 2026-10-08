/*
 * ts_media.h -- tdk 多媒体公共结构体定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_MEDIA_H__
#define __TS_MEDIA_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>

/* 色彩空间 */
typedef struct ts_rgb_s {
    ts_u08 r;
    ts_u08 g;
    ts_u08 b;
    ts_u08 alpha;
} ts_rgb_s_t;


typedef struct ts_yuv_s {
    ts_u08 v;
    ts_u08 u;
    ts_u08 y;
    ts_u08 alpha;
} ts_yuv_s_t;


/* 视频类型 */
typedef enum ts_video_type_en {
    /* 可见光 */
    TS_VIDEO_TYPE_VIS   = 0,

    /* 红外线 */
    TS_VIDEO_TYPE_IR    = 1,

    TS_VIDEO_TYPE_BUTT
} ts_video_type_en_t;


/* 像素位宽 */
typedef enum ts_pixel_bw_en {
    TS_PIXEL_BW_UNKONW = 0,

    TS_PIXEL_BW_1BIT = 1,
    TS_PIXEL_BW_2BIT = 2,
    TS_PIXEL_BW_4BIT = 3,
    TS_PIXEL_BW_8BIT = 4,
    TS_PIXEL_BW_16BIT = 5,
    TS_PIXEL_BW_24BIT = 6,
    TS_PIXEL_BW_32BIT = 7,
    TS_PIXEL_BW_64BIT = 8,
    TS_PIXEL_BW_128BIT = 9,
    TS_PIXEL_BW_256BIT = 10,

    TS_PIXEL_BW_BUTT
} ts_pixel_bw_en_t;


/* 视频制式 */
typedef enum ts_video_fmt_en {
    TS_VIDEO_FMT_120X90     = 0,
    TS_VIDEO_FMT_256X192    = 1,
    TS_VIDEO_FMT_384X288    = 2,
    TS_VIDEO_FMT_640X512    = 3,
    TS_VIDEO_FMT_800X600    = 4,
    TS_VIDEO_FMT_1280X1024  = 5,

    TS_VIDEO_FMT_BUTT
} ts_video_fmt_en_t;

/* CVBS 制式 */
#define TS_CVBS_WIDTH           (720)
#define TS_CVBS_PAL_HEIGHT      (576)
#define TS_CVBS_NTSC_HEIGHT     (480)
typedef enum ts_cvbs_fmt_en {
    TS_CVBS_FMT_PAL     = 0,
    TS_CVBS_FMT_NTSC    = 1,

    TS_CVBS_FMT_BUTT
} ts_cvbs_fmt_en_t;


/* 旋转、镜像 */
typedef enum ts_rotate_type_en {
    /* BYPASS */
    TS_ROTATE_TYPE_BYPASS   = 0,

    /* 水平镜像 */
    TS_ROTATE_TYPE_MIRROR   = 1,

    /* 垂直翻转 */
    TS_ROTATE_TYPE_FLIP     = 2,

    /* 顺时针 90 度旋转 */
    TS_ROTATE_TYPE_90       = 3,

    /* 顺时针 180 度旋转 */
    TS_ROTATE_TYPE_180      = 4,

    /* 顺时针 270 度旋转 */
    TS_ROTATE_TYPE_270      = 5,

    TS_ROTATE_TYPE_BUTT
} ts_rotate_type_en_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_MEDIA_H__ */
