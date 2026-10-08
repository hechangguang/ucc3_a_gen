/*
 * ts_comm.h -- tdk 公共结构体定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_COMM_H__
#define __TS_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <cpl/cpl_list.h>

#define TS_MDEV_MAGIC       (0x57)

/* 设备句柄定义 */
typedef ts_s32 ts_hdev_t;

/* 组句柄定义 */
typedef ts_s32 ts_grp_t;

/* 通道句柄定义 */
typedef ts_s32 ts_chn_t;

/* ID 类型定义 */
typedef ts_s32 ts_id_t;

/* 
 * 音视频等设备句柄定义, 不用结构体便于数据转化
 * 0..7     8..15   16..23  24..29 30..31
 * custom    modid    chn     grp    0
 */
typedef ts_s32 ts_mdev_t;

/* 通用指针型句柄 */
typedef ts_void * ts_handle_t;

#define TS_MDEV_GET_MODID(mdev)                 (ts_s32)((mdev << 16) >> 24)
#define TS_MDEV_GET_GRPID(mdev)                 (ts_s32)((mdev << 0) >> 24)
#define TS_MDEV_GET_CHNID(mdev)                 (ts_s32)((mdev << 8) >> 24)
#define TS_MDEV_GET_CUSTOM(mdev)                (ts_s32)((mdev << 24) >> 24)
#define TS_MDEV_SET(modid, grp, chn, custom)    ((((ts_u08)grp & 0x3F) << 24) | (((ts_u08)chn & 0xFF) << 16) \
                                                    | ((((ts_u08)modid & 0xFF) << 8) | ((ts_u08)custom & 0xFF)))


/* 定义使能 */
#define TS_DISABLE          (0)
#define TS_ENABLE           (1)


/* 设备等状态定义 */
typedef enum ts_status_en {
    TS_STATUS_STOP = 0,
    TS_STATUS_RUN  = 1,

    TS_STATUS_BUTT
} ts_status_en_t;


/* 电平状态定义 */
typedef enum ts_elec_level_en {
    /* 低电平 */
    TS_ELEC_LVL_LO = 0,

    /* 高电平 */
    TS_ELEC_LVL_HI = 1,

    /* 高阻 */
    TS_ELEC_LVL_Z = 2,

    /* 输入低电平 */
    TS_ELEC_LVL_VIL = 3,

    /* 输入高电平 */
    TS_ELEC_LVL_VIH = 4,

    /* 输出低电平 */
    TS_ELEC_LVL_VOL = 5,

    /* 输出高电平 */
    TS_ELEC_LVL_VOH = 6,

    /* 上升沿 */
    TS_ELEC_LVL_REDGE = 7,

    /* 下降沿 */
    TS_ELEC_LVL_FEDGE = 8,

    /* 双沿 */
    TS_ELEC_LVL_BEDGE = 9,

    TS_ELEC_LVL_BUTT
} ts_elec_level_en_t;

typedef enum ts_option_en {
    TS_OPT_DISABLE  = 0,
    TS_OPT_ENABLE   = 1,

    TS_OPT_BUTT
} ts_option_en_t;


typedef enum ts_switch_en {
    TS_SWITCH_OFF   = 0,
    TS_SWITCH_ON    = 1,

    TS_SWITCH_BUTT
} ts_switch_en_t;


typedef struct ts_rect_s {
    ts_s32 s32_left;
    ts_s32 s32_top;
    ts_u32 u32_width;
    ts_u32 u32_height;
} ts_rect_s_t;

typedef struct ts_rect_pos_s {
    ts_s32 s32_begin_x;
    ts_s32 s32_begin_y;
    ts_s32 s32_end_x;
    ts_s32 s32_end_y;
} ts_rect_pos_s_t;

typedef struct ts_point_s {
    ts_s32 s32_x;
    ts_s32 s32_y;
} ts_point_s_t;

typedef struct ts_size_s {
    ts_u32 u32_width;
    ts_u32 u32_height;
} ts_size_s_t;


/* 定义寄存器建议地址和数值组合结构体 */
typedef struct ts_reg_val_s {
    ts_u16 u16_offset;
    ts_u16 u16_dly_us;
    ts_u32 u32_val;
} ts_reg_val_s_t;


/* 定义同步模式 */
typedef enum ts_sync_mode_en {
    /* 内同步 */
    TS_SYNC_MODE_INTERNAL = 0,

    /* 外同步 */
    TS_SYNC_MODE_EXTERNAL = 1,

    TS_SYNC_MODE_BUTT
} ts_sync_mode_en_t;


/* 通用 ringbuf 类型缓冲 */
typedef struct ts_ringbuf_s {
    /* 此节点的缓存物理地址 */
    ts_phys_addr_t u32_phy_addr;

    /* 此节点的虚拟地址 */
    ts_phys_addr_t u32_virt_addr;

    /* 此节点的缓存大小 */
    ts_u32 u32_size;

    /* 此节点序号 */
    ts_u32 u32_idx;
} ts_ringbuf_s_t;


/* 通用 node 类型缓冲 */
typedef struct ts_nodebuf_s {
    /* 内部节点控制，用户无需关心 */
    ts_cpl_list_head_s_t st_list;

    /* 此节点的缓存物理地址 */
    ts_phys_addr_t u32_phy_addr;

    /* 此节点的虚拟地址 */
    ts_phys_addr_t u32_virt_addr;

    /* 此节点的缓存大小 = u32_stride_size * u16_stride_num; */
    ts_u32 u32_size;

    /* 多个缓冲拼接的话，此处配置单个缓冲的大小 */
    ts_u32 u32_stride_size;

    /* 拼接个数 */
    ts_u16 u16_stride_num;

    /* 此节点引用次数 */
    ts_u16 u16_ref;

    /* 私有变量指针 */
    ts_void *pv_priv;
} ts_nodebuf_s_t;


/* 通用缓冲 */
typedef struct ts_buffer_s {
    /* 缓冲实际使用地址，可能是虚拟地址或者是物理地址 */
    ts_u08 *pu08_ptr;

    /* 保存物理地址，便于释放 */
    ts_phys_addr_t phys_addr;

    /* 缓冲实际大小 */
    ts_size_t sz_size;

    /* 缓冲宽高 */
    ts_u32 u32_width;
    ts_u32 u32_height;
} ts_buffer_s_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_H__ */
