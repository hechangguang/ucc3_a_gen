/*
 * k_console_v100.h -- tdk console 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_CONSOLE_V100_H__
#define __K_CONSOLE_V100_H__

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_log.h>
#include <cpl/ts_cpl.h>

/* tsp 日志输出控制 */
extern ts_s32 gs32_tsp_log_lvl;

#define tsp_log_err(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_ERR, gs32_tsp_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define tsp_log_warn(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_WARNING, gs32_tsp_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define tsp_log_notice(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_NOTICE, gs32_tsp_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define tsp_log_debug(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_tsp_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define tsp_log_info(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_INFO, gs32_tsp_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define tsp_log_trace() \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_tsp_log_lvl, __func__, __LINE__, ts_null); \
} \
while(0)

#define TSP_KCON_MAX_SIZE       (256)

/* vsnprint 内部输出控制 */
typedef struct kcon_attr_s {
    /* 是否需要静默, set 1 is quiet */
    ts_u32 u32_quiet;

    /* 指定缓存大小 */
    ts_u32 u32_size;

    /* 缓存指针 */
    ts_char *pc_buf;

    /* 输出，如串口、USB等 */
    int (*putc)(int c);
} kcon_attr_s_t;


/* */
extern int k_enh_vsnprintf(kcon_attr_s_t *pst_attr, const char* format, va_list va);
extern ts_s32 tsp_kcon_puts(const char *str, ts_u32 u32_max_len);

/* k_con */
extern ts_void _ts_init_ tsp_kcon_init(ts_char *pc_dev_name);
extern ts_void tsp_kcon_early_init(ts_u32 u32_quiet);
extern ts_s32 tsp_kcon_vsnprintk(const char *fmt, ts_cpl_va_list args);
extern ts_u32 tsp_kcon_get_quiet(ts_void);
extern ts_void tsp_kcon_set_quiet(ts_u32 u32_quiet);

/* serial.c 需实现 early_putc 功能 */
extern ts_void tsp_early_init(ts_u32 u32_baudrate);
extern int tsp_early_putc(int c);
extern ts_void tsp_early_flush(ts_u32 u32_flush_en);

#endif /* __K_CONSOLE_V100_H__ */
