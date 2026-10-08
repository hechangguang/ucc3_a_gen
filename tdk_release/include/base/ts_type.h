/*
 * ts_type.h -- tdk 数据类型重定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_TYPE_H__
#define __TS_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* 通用数据类型定义 */
typedef unsigned char           ts_u08;
typedef unsigned short          ts_u16;
typedef unsigned int            ts_u32;

typedef signed char             ts_s08;
typedef short                   ts_s16;
typedef int                     ts_s32;

typedef unsigned long long      ts_u64;
typedef long long               ts_s64;

typedef unsigned long           ts_ulong;
typedef long                    ts_long;
typedef char                    ts_char;
typedef void                    ts_void;

typedef float                   ts_f32;
typedef double                  ts_f64;

typedef unsigned char           ts_fast_u08;
typedef unsigned int            ts_fast_u16;
typedef unsigned int            ts_fast_u32;
typedef signed char             ts_fast_s08;
typedef int                     ts_fast_s16;
typedef int                     ts_fast_s32;

typedef unsigned char           ts_least_u08;
typedef unsigned short          ts_least_u16;
typedef unsigned int            ts_least_u32;
typedef signed char             ts_least_s08;
typedef short                   ts_least_s16;
typedef int                     ts_least_s32;


/* 特定的数据类型重定义, 通过 -D__TDK_BW_64__ 选择 */
#ifdef __TDK_BW_64__
typedef long long               ts_loff_t;
typedef int                     ts_off_t;
typedef long long               ts_ssize_t;
typedef unsigned long long      ts_size_t;
typedef unsigned long long      ts_phys_addr_t;
typedef unsigned long long      ts_dma_addr_t;
typedef unsigned long long      ts_uintptr_t;
#else
typedef int                     ts_loff_t;
typedef int                     ts_off_t;
typedef int                     ts_ssize_t;
typedef unsigned int            ts_size_t;
typedef unsigned int            ts_phys_addr_t;
typedef unsigned int            ts_dma_addr_t;
typedef unsigned int            ts_uintptr_t;

/* 为某些移植代码定义 可以通过 DCONFIG_UINT */
#ifdef CONFIG_NEED_UINT
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;
typedef unsigned int            uint;
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;

typedef unsigned int            uintptr_t;
typedef unsigned int            size_t;
#define UINT32_MAX              0xffffffff
#endif

#endif

#ifdef CONFIG_NEED_UINT
typedef enum {
    false = 0,
    true  = 1,
} bool;
#endif

#define TS_UINT32_MAX           (0xffffffff)


typedef enum {
    ts_false = 0,
    ts_true  = 1,
} ts_bool;

#ifndef null
    #define null                (0L)
#endif

#define ts_null                 (null)

#define TS_BITS_PER_LONG        32
#define TS_BITS_PER_LONG_LONG   64

#ifndef __CHECKER__
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER)	__compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER)	((ts_size_t)&((TYPE *)0)->MEMBER)
#endif
#endif

#define ts_container_of(ptr, type, member) ({               \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_TYPE_H__ */
