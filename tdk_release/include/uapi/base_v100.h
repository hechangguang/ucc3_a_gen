/* 
 * base_v100.h -- base v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UAPI_BASE_V100_H__
#define __UAPI_BASE_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <uapi/chip_def.h>
#include <cpl/ts_cpl.h>

#define TDK_VERISON     "VB230"

/* UAPI 日志控制输出 */
extern ts_s32 gs32_uapi_log_lvl;

#define ts_uapi_log_err(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_ERR, gs32_uapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_uapi_log_warn(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_WARNING, gs32_uapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_uapi_log_notice(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_NOTICE, gs32_uapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_uapi_log_debug(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_uapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_uapi_log_info(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_INFO, gs32_uapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_uapi_log_trace() \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_uapi_log_lvl, __func__, __LINE__, ts_null); \
} \
while(0)


#define uapi_chk_ptr(ptr, ret)\
do{\
    if (ts_null == ptr) {\
        ts_uapi_log_err("%s\n", TS_ENULLPTR_STR);\
        return -ret;\
    }\
}while(0)


/* 全局图像信息 */
typedef struct ts_img_info_s {
    /* 输入图像尺寸和帧率 */
    ts_u16 u16_width;
    ts_u16 u16_height;

    ts_f32 f32_in_fps;

    /* 根据帧率、时钟等信息计算出来的总宽高 */
    ts_u16 u16_ro_total_w;
    ts_u16 u16_ro_total_h;

    /* 计算需要用到的时钟 */
    ts_u32 u32_ro_lcd_freq;
    ts_u32 u32_ro_isp_freq;
    ts_u32 u32_ro_vpo_pixel_freq;
    ts_u32 u32_ro_snr_mc_freq;
    ts_u32 u32_ro_viu_freq;
    ts_u32 u32_ro_vpo_freq;
} ts_img_info_s_t;

ts_img_info_s_t *tdl_base_get_img_info(ts_void);


/* --- tmm --- */

/* 单 DDR1 应用模式 */
#define TS_TMM_SBANK1_640_MODE  (0)

/* 双 DDR 应用模式 */
#define TS_TMM_DBANK_1280_MODE  (1)

/* for N6 */
#define TS_TMM_N6_MODE          (2)

/* for A12 */
#define TS_TMM_A12_MODE         (3)

/* for N2 */
#define TS_TMM_N2_MODE          (4)

/* for A3 */
#define TS_TMM_A3_MODE          (5)

/* for 1280_60HZ */
#define TS_TMM_1280_60HZ_MODE   (6)

/* for A6/A12 spec  */
#define TS_TMM_A6_SPEC_MODE     (7)

/* 自定义 */
#define TS_TMM_CUSTOM           (10)


/* 
 * 函数名 :  ts_tdk_init_tmm
 * 描  述 :  指定 tmm 使用方式
 * 输  入 :  参数               描述
 *          u32_mode            分配模式，支持自定模式(TS_TMM_CUSTOM)
 *          u32_dbg_en          是否开启调试信息
 *          u32_node0_cnt       ddr0 配置节点
 *          u32_node1_cnt       ddr1 配置节点
 *          u32_txt_size        分配最大代码大小,需要大于实际代码尺寸，并作 32 字节对齐
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          无                  无
 */
ts_void ts_tdk_init_tmm(ts_u32 u32_mode, ts_u32 u32_dbg_en, ts_u32 u32_node0_cnt, ts_u32 u32_node1_cnt, ts_u32 u32_txt_size);

/* 获取 tdk 版本 */
ts_u32 ts_tdk_get_version(ts_void);

/* 初始化硬件锁 */
ts_s32 ts_lib_hwlock_init(ts_u32 u32_regbase, ts_u32 u32_max_num);


/* 
 * 函数名 :  ts_lib_uart_hbp_task
 * 描  述 :  开启 hbp 协议守护线程
 * 输  入 :  参数           描述
 *          s32_hdev        hbp 对接串口设备句柄
 *          u32_prio        线程优先级
 *          u32_stack_size  线程栈
 *          u32_flag        hbp接口与终端接口相同标志
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          启动成功
 *           TS_EINVAL      无效入参
 */
ts_s32 ts_lib_uart_hbp_task(ts_s32 s32_hdev, ts_u32 u32_prio, ts_u32 u32_stack_size, ts_u32 u32_flag);


/* 
 * 函数名 :  ts_tdk_read_uuid
 * 描  述 :  获取芯片的 uuid, 在获取芯片 uuid 失败的情况下，可以通过随机数替代
 * 输  入 :  参数               描述
 *          u32_rand_en         在获取 uuid 失败的时候，是否启用随机数; >0 使能
 *          u32_size            最大输入缓冲，大小至少 17，其中 16 个uuid + 一个结束符
 * 输  出 :  参数               描述
 *          pc_uuid             uuid 字符串，固定 16 个字符
 * 返  回 :  返回值             描述
 *          无                  无
 */
ts_void ts_tdk_read_uuid(ts_u32 u32_rand_en, ts_char *pc_uuid, ts_u32 u32_size);


/* isp 驱动指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_isp0;
extern ts_void *gpv_tmm_isp1;

/* viu 驱动指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_viu;

/* vpo 驱动指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_vpo;

/* osd 驱动指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_osd;

/* usb 驱动指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_usb;

/* 系统指定 tmm 申请句柄 */
extern ts_void *gpv_tmm_sys;

/* 用户指定 tmm 申请句柄，同时定义在 k_base_v100.h */
extern ts_void *gpv_tmm_usr;

/* dtcm 指定句柄 */
extern ts_void *pgv_tmm_dtcm;

/* 特殊句柄 */
extern ts_void *pgv_tmm_spec;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_BASE_V100_H__ */

