/*
 * cpl_debug.h -- cpl 实现调试功能
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_DEBUG_H__
#define __CPL_DEBUG_H__

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_log.h>
#include <base/ts_compiler.h>


extern ts_void ts_cpl_log(ts_s32 s32_level, ts_s32 s32_ctl_lvl, const ts_char *pc_func, \
            ts_s32 s32_line, const ts_char *fmt, ...) ts_printf_like(5, 6);

/* 内核态打印输出 */
extern ts_s32 ts_cpl_printk(const ts_char *fmt, ...) ts_printf_like(1, 2);

/* 内核态 panic 输出 */
extern ts_void ts_cpl_panic(const ts_char *fmt, const ts_char *file, \
            const ts_char *fun, ts_s32 line, const ts_char *cond);

/* 设置底层打印输出，如果关闭，调试串口将不再输出任何打印日志 */
extern ts_void ts_cpl_set_log_en(ts_u32 u32_enable);

/* 断言输出 */
extern ts_void ts_cpl_assert_handler(const ts_char *pc_expr, const ts_char *pc_func, \
                ts_s32 s32_line, ts_s32 s32_bug_on);

#define TS_CPL_BUG() do { } while (1)

#define TS_CPL_PARAM_INVALID(func, line) ts_cpl_printk("%s:%d : Parameter invalid!\n", func, line);
#define TS_CPL_MALLOC_FAILED(func, line) ts_cpl_printk("%s:%d : Malloc failed!\n", func, line);


#define ts_cpl_trace(level, ctl, fmt, args ...) \
do \
{ \
    if (level <= ctl) { \
        do { \
            if (level <= TS_LOG_ERR) \
                ts_cpl_printk(TS_LOG_RED "Err(%s@%d): " fmt TS_LOG_NONE_C,  __func__, __LINE__, ## args); \
            else if (TS_LOG_WARNING == level) \
                ts_cpl_printk(TS_LOG_YELLOW "Warn(%s@%d): " fmt TS_LOG_NONE_C, __func__, __LINE__, ## args); \
            else if (TS_LOG_NOTICE == level) \
                ts_cpl_printk(TS_LOG_GREEN "Info(%s@%d): " fmt TS_LOG_NONE_C, __func__, __LINE__, ## args); \
            else if (TS_LOG_DEBUG == level) \
                ts_cpl_printk(TS_LOG_PURPLE "Dbg(%s@%d): " fmt TS_LOG_NONE_C, __func__, __LINE__, ## args); \
            else \
                ts_cpl_printk("%s@%d: " fmt, __func__, __LINE__, ## args); \
        } while (0); \
    } \
} while(0)


/* 通用入口函数检测 */
#define ts_cpl_chk_null_ptr(ptr, ctl_lvl)\
do{\
    if (ts_null == ptr) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "%s\n", TS_ENULLPTR_STR);\
        return -TS_ENULLPTR;\
    }\
}while(0)

#define ts_cpl_chk_tsbuf(ptr, ctl_lvl)\
do{\
    if ((ts_null == ptr) || (ts_null == ptr->pu08_ptr) || (0 == ptr->sz_size)) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "%s\n", TS_ETSBUF_STR);\
        return -TS_ETSBUF;\
    }\
}while(0)

#define ts_cpl_chk_mdev(mdev, ctl_lvl)\
do{\
    if (0 == TS_MDEV_GET_MODID(mdev)) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "HMDEV not created or creation failed!\n");\
        return -TS_ENOTREADY;\
    }\
} while(0)

#define ts_cpl_chk_hdl(hdl, ctl_lvl)\
do{\
    if (hdl < 0) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "Handle not created or creation failed!\n");\
        return -TS_ENOTREADY;\
    }\
} while(0)


#define ts_cpl_chk_grp(gt_grp, max_grp, ctl_lvl)\
do{\
    if ((ts_u32)gt_grp >= max_grp) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "%s = %d\n", TS_EINVALGRPID_STR, (ts_u32)gt_grp);\
        return -TS_EINVALGRPID;\
    }\
} while(0)

#define ts_cpl_chk_chn(en_chn, max_chn, ctl_lvl)\
do{\
    if ((ts_u32)en_chn >= max_chn) {\
        ts_cpl_log(TS_LOG_ERR, ctl_lvl, __func__, __LINE__, "%s = %d\n", TS_EINVALCHNID_STR, (ts_u32)en_chn);\
        return -TS_EINVALCHNID;\
    }\
} while(0)


/* 标志日志输出接口 */
#define ts_cpl_log_err(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_ERR, TS_LOG_ERR, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_cpl_log_warn(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_WARNING, TS_LOG_WARNING, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_cpl_log_notice(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_NOTICE, TS_LOG_NOTICE, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_cpl_log_debug(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_DEBUG, TS_LOG_DEBUG, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define ts_cpl_log_info(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_INFO, TS_LOG_INFO, __func__, __LINE__, fmt, ## args); \
} \
while(0)


#define TS_CPL_ASSERT(expr) \
do {                        \
    if (!(expr)) {          \
        ts_cpl_printk("\nTS_CPL_ASSERT failed at:\n  >Condition: %s\n", #expr);    \
        ts_cpl_assert_handler(#expr, __func__, __LINE__, 0);       \
    }                       \
} while (0)

#define TS_CPL_BUG_ON(expr)   \
do {                        \
    if (expr) {              \
        ts_cpl_printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
        ts_cpl_assert_handler(#expr, __func__, __LINE__, 1);       \
    }                       \
} while (0)

#endif /* __CPL_DEBUG_H__ */
