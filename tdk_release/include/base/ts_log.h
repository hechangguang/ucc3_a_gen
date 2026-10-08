/*
 * ts_log.h -- tdk 日志头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_LOG_H__
#define __TS_LOG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 日志输出颜色定义 */
#define TS_LOG_NONE_C       "\033[m"
#define TS_LOG_RED          "\033[0;32;31m"
#define TS_LOG_GREEN        "\033[0;32;32m"
#define TS_LOG_BLUE         "\033[0;32;34m"
#define TS_LOG_DARY_GRAY    "\033[1;30m"
#define TS_LOG_CYAN         "\033[0;36m"
#define TS_LOG_PURPLE       "\033[0;35m"
#define TS_LOG_BROWN        "\033[0;33m"
#define TS_LOG_YELLOW       "\033[1;33m"
#define TS_LOG_WHITE        "\033[1;37m"

/* 日志等级 */
#define TS_LOG_BYPASS       (0) /* Dont't output any log */
#define TS_LOG_EMERG        (1) /* System is unusable */
#define TS_LOG_ALERT        (2) /* Action must be taken immediately */
#define TS_LOG_CRIT         (3) /* Critical conditions */
#define TS_LOG_ERR          (4) /* Error conditions */
#define TS_LOG_WARNING      (5) /* Warning conditions */
#define TS_LOG_NOTICE       (6) /* Normal, but significant, condition */
#define TS_LOG_INFO         (7) /* Informational message */
#define TS_LOG_DEBUG        (8) /* Debug-level message */

#define TS_LOG_GET_ERRNO_LEVEL(errno)   ((errno >> 16) & 0xFF)


ts_void ts_log(ts_s32 s32_level, const ts_char *fmt, ...);
ts_void ts_log_err(ts_s32 s32_errno, ts_char *pc_err_str);
ts_void ts_log_set_level(ts_s32 s32_level);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_LOG_H__ */
