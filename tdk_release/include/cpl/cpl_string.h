/* 
 * cpl_string.h -- tdk 跨平台字符相关抽象层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_STRING_H__
#define __CPL_STRING_H__

#include <stdarg.h>
#include <base/ts_compiler.h>

#ifndef __TDK_VA_LIST__
#define __TDK_VA_LIST__
#define ts_cpl_va_list 					va_list
#define ts_cpl_va_arg(ap, T) 			va_arg(ap, T)
#define ts_cpl_va_end(ap) 				va_end(ap)
#define ts_cpl_va_start(ap, A) 			va_start(ap, A)
#endif

#if 0
/* ts_cpl_isdigit */
TS_STATIC_INLINE ts_s32 ts_cpl_isdigit(ts_s32 c)
{
    return (c >= '0' && c <= '9');
}
#endif

#define ts_cpl_isblank(c)      (c == ' ' || c == '\t')

/* strcasecmp strncasecmp */
extern int ts_cpl_strcasecmp(const char *s1, const char *s2);
extern int ts_cpl_strncasecmp(const char *s1, const char *s2, ts_size_t len);

/* strcpy strncpy strlcpy */
extern char *ts_cpl_strcpy(char *dest, const char *src);
extern char *ts_cpl_strncpy(char *dest, const char *src, ts_size_t count);
extern ts_size_t ts_cpl_strlcpy(char *dest, const char *src, ts_size_t size);

/* strcat, strncat, strlcat */
extern char *ts_cpl_strcat(char *dest, const char *src);
extern char *ts_cpl_strncat(char *dest, const char *src, ts_size_t count);
extern ts_size_t ts_cpl_strlcat(char *dest, const char *src, ts_size_t count);

/* strcmp, strncmp */
extern int ts_cpl_strcmp(const char *cs, const char *ct);
extern int ts_cpl_strncmp(const char *cs, const char *ct, ts_size_t count);

/* strchr, strchrnul, strnchrnul, strnchr, strrchr */
extern char *ts_cpl_strchr(const char *s, int c);
extern char *ts_cpl_strchrnul(const char *s, int c);
extern char *ts_cpl_strnchrnul(const char *s, ts_size_t count, int c);
extern char *ts_cpl_strrchr(const char *s, int c);
extern char *ts_cpl_strnchr(const char *s, ts_size_t count, int c);

/* strlen, strnlen */
extern ts_size_t ts_cpl_strlen(const char *s);
extern ts_size_t ts_cpl_strnlen(const char *s, ts_size_t count);

/* strspn, strcspn, strpbrk, strsep */
extern ts_size_t ts_cpl_strspn(const char *s, const char *accept);
extern ts_size_t ts_cpl_strcspn(const char *s, const char *reject);
extern char *ts_cpl_strpbrk(const char *cs, const char *ct);
extern char *ts_cpl_strsep(char **s, const char *ct);

/* memset, memset16, memset32, memset64, memcpy, memmove, memcmp, memscan */
extern void *ts_cpl_memset(void *s, int c, ts_size_t count);
extern void *ts_cpl_memset16(ts_u16 *s, ts_u16 v, ts_size_t count);
extern void *ts_cpl_memset32(ts_u32 *s, ts_u32 v, ts_size_t count);
extern void *ts_cpl_memset64(ts_u64 *s, ts_u64 v, ts_size_t count);
extern void *ts_cpl_memcpy(void *dest, const void *src, ts_size_t count);
extern void *ts_cpl_memmove(void *dest, const void *src, ts_size_t count);
extern int ts_cpl_memcmp(const void *cs, const void *ct, ts_size_t count);
extern void *ts_cpl_memscan(void *addr, int c, ts_size_t size);

/* strstr strnstr */
extern char *ts_cpl_strstr(const char *s1, const char *s2);
extern char *ts_cpl_strnstr(const char *s1, const char *s2, ts_size_t len);

/* str2num num2str */
extern ts_s32 ts_cpl_atoi(char *s);
extern ts_s32 ts_cpl_atoh(char *s);
extern ts_s32 ts_cpl_str2num(char *s);
extern ts_char *ts_cpl_split_args(char *s, ts_s32 *ps32_val);

/* sprint */
extern ts_s32 ts_cpl_vsnprintf(char *buf, ts_s32 size, const char *fmt, ts_cpl_va_list args);
extern ts_s32 ts_cpl_vsprintf(char *buf, const char *fmt, ts_cpl_va_list args);
extern ts_s32 ts_cpl_snprintf(char *buf, ts_s32 size, const char *fmt, ...);
extern ts_s32 ts_cpl_sprintf(char *buf, const char *fmt, ...);

#endif /* __CPL_STRING_H__ */
