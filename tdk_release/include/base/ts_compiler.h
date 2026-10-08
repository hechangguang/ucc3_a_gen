/*
 * ts_compiler.h -- tdk 编译相关配置
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __TS_COMPILER_H__
#define __TS_COMPILER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define __I         volatile const
#define __O         volatile
#define __IO        volatile
#define __IM        volatile const
#define __OM        volatile
#define __IOM       volatile


#ifdef __GNUC__

#define TS_ASM_VOLATILE         __asm volatile
#define TS_INLINE               inline
#define TS_ALWAYS_INLINE        __attribute__((always_inline)) inline
#define TS_STATIC               static
#define TS_STATIC_INLINE        static inline

#define ts_printf_like(a, b)    __attribute__((__format__(__printf__, a, b)))

#define TS_WEAK                 __attribute__((weak)) 

#define TS_FUNC_UNUSED          __attribute__((unused))

#define TS_PACK(n)              __attribute__((aligned(n)))

#define TS_PACKED               __attribute__((__packed__))

#define TS_ALIGNED(x)           __attribute__((__aligned__(x)))

#else

#define TS_ASM_VOLATILE
#define TS_INLINE
#define TS_ALWAYS_INLINE
#define TS_STATIC
#define TS_STATIC_INLINE
#define ts_printf_like(a, b)
#define TS_WEAK
#define TS_FUNC_UNUSED
#define TS_PACK(n)
#define TS_PACKED
#define TS_ALIGNED(x)
#endif

/* for tsp(nuttx) port */

#define FAR
#define CODE
#define __must_check
#define __force

/* Indicate that a local variable is not used */
#define TS_UNUSED(a)        ((void)(1 || (a)))


/* only for endian */
#define ts_uswap_16(x) \
    ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))

#define ts_uswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24))

#define _ts_uswap_64(x, sfx) \
    ((((x) & 0xff00000000000000##sfx) >> 56) | \
     (((x) & 0x00ff000000000000##sfx) >> 40) | \
     (((x) & 0x0000ff0000000000##sfx) >> 24) | \
     (((x) & 0x000000ff00000000##sfx) >>  8) | \
     (((x) & 0x00000000ff000000##sfx) <<  8) | \
     (((x) & 0x0000000000ff0000##sfx) << 24) | \
     (((x) & 0x000000000000ff00##sfx) << 40) | \
     (((x) & 0x00000000000000ff##sfx) << 56))

#define ts_uswap_64(x)          _ts_uswap_64(x, ull)

#define ts_cpu_to_le16(x)       (x)
#define ts_cpu_to_le32(x)       (x)
#define ts_cpu_to_le64(x)       (x)

#define ts_le16_to_cpu(x)       (x)
#define ts_le32_to_cpu(x)       (x)
#define ts_le64_to_cpu(x)       (x)

#define ts_cpu_to_be16(x)       ts_uswap_16(x)
#define ts_cpu_to_be32(x)       ts_uswap_32(x)
#define ts_cpu_to_be64(x)       ts_uswap_64(x)

#define ts_be16_to_cpu(x)       ts_uswap_16(x)
#define ts_be32_to_cpu(x)       ts_uswap_32(x)
#define ts_be64_to_cpu(x)       ts_uswap_64(x)


#define TS_TOKEN(x)             x

/* stringify without expand */
#define TS_STRING(x)            #x

/* expand then stringify */
#define TS_XSTRING(x)           TS_STRING(x)

/* concat without expand */
#define TS_STRCAT(a, b)         a##b

/* concat without expand */
#define TS_STRCAT3(a, b, c)     a##b##c

/* expand then concat */
#define TS_XSTRCAT(a, b)        TS_STRCAT(a, b)

/* expand then concat 3 tokens */
#define TS_XSTRCAT3(a, b, c)    TS_STRCAT3(a, b, c)

#define TS_VERIFY_STATIC(const_expr, _mess)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMPILER_H__ */
