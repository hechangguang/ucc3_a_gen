/*
 * ts_sting.h -- 区别与标准 string ， 此处只是简单实现常用字符函数, 改写自 linux
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_STRING_H__
#define __TS_STRING_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>

ts_char *ts_strcpy(ts_char * dest, const ts_char *src);
ts_s32 ts_strcmp(const ts_char *cs, const ts_char *ct);
ts_u32 ts_strlen(const ts_char * s);
ts_u32 ts_strtoul(const ts_char *str);

ts_u08 ts_memcpy_sum(ts_void *dest, const ts_void *src, ts_u32 count, ts_u08 in_sum);
ts_void *ts_memcpy(ts_void *dest, const ts_void *src, ts_u32 count);
ts_s32 ts_memcmp(const ts_void * cs, const ts_void * ct, ts_u32 count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_STRING_H__ */
