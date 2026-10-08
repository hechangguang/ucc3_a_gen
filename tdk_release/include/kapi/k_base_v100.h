/* 
 * base_v100.h -- base v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __KAPI_BASE_V100_H__
#define __KAPI_BASE_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <cpl/ts_cpl.h>

/* UAPI 日志控制输出 */
extern ts_s32 gs32_kapi_log_lvl;

#define ts_kapi_log_err(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_ERR, gs32_kapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_kapi_log_warn(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_WARNING, gs32_kapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_kapi_log_notice(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_NOTICE, gs32_kapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_kapi_log_debug(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_kapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_kapi_log_info(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_INFO, gs32_kapi_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_kapi_log_trace() \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_kapi_log_lvl, __func__, __LINE__, ts_null); \
} \
while(0)


#define kapi_chk_ptr(ptr, ret)\
do{\
    if (ts_null == ptr) {\
        ts_kapi_log_err("%s\n", TS_ENULLPTR_STR);\
        return -ret;\
    }\
}while(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __KAPI_BASE_V100_H__ */
