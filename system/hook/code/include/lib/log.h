/*
 * log.h -- 日志输出模块头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022 Hangzhou JiTai Silicon Co., Ltd. All rights reserved.
*/
#ifndef _LOG_H_
#define _LOG_H_

#include <config.h>

#include <base/ts_type.h>
#include <base/ts_log.h>
#include <bootup.h>

/* log level */
#ifdef CONFIG_LOG_LEVEL_EMERG
#define LOG_DEFAULT_LEVEL       TS_LOG_EMERG
#endif

#ifdef CONFIG_LOG_LEVEL_ALERT
#define LOG_DEFAULT_LEVEL       TS_LOG_ALERT
#endif

#ifdef CONFIG_LOG_LEVEL_CRIT
#define LOG_DEFAULT_LEVEL       TS_LOG_CRIT
#endif

#ifdef CONFIG_LOG_LEVEL_ERROR
#define LOG_DEFAULT_LEVEL       TS_LOG_ERR
#endif

#ifdef CONFIG_LOG_LEVEL_WARNING
#define LOG_DEFAULT_LEVEL       TS_LOG_WARNING
#endif

#ifdef CONFIG_LOG_LEVEL_NOTICE
#define LOG_DEFAULT_LEVEL       TS_LOG_NOTICE
#endif

#ifdef CONFIG_LOG_LEVEL_INFO
#define LOG_DEFAULT_LEVEL       TS_LOG_INFO
#endif

#ifdef CONFIG_LOG_LEVEL_DEBUG
#define LOG_DEFAULT_LEVEL       TS_LOG_DEBUG
#endif

/* 底层 putchar 接口 */
typedef ts_void (*ts_putc_fn_t)(ts_u08 u08_val);

ts_void log_set_putc(ts_putc_fn_t fn_putc);
ts_void log_set_bypass(ts_s32 s32_bypass);
ts_void log_set_level(ts_s32 s32_level);

ts_s32 printf(const ts_char *fmt, ...);

#endif /* _LOG_H_ */
