/*
 * ts_bitop.h -- tdk bit 操作
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __TS_BITOP_H__
#define __TS_BITOP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 定义位宽 */
#define TS_BW8                  (0x00)
#define TS_BW16                 (0x01)
#define TS_BW32                 (0x02)
#define TS_BW64                 (0x03)
#define TS_BW128                (0x04)

#define TS_BIT(nr)              (1UL << (nr))
#define TS_BIT_ULL(nr)          (1ULL << (nr))
#define TS_BIT_MASK(nr)         (1UL << ((nr) % TS_BITS_PER_LONG))
#define TS_BIT_WORD(nr)         ((nr) / TS_BITS_PER_LONG)
#define TS_BTS_IT_ULL_MASK(nr)  (1ULL << ((nr) % TS_BITS_PER_LONG_LONG))
#define TS_BIT_ULL_WORD(nr)     ((nr) / TS_BITS_PER_LONG_LONG)
#define TS_BITS_PER_BYTE        8

#define TS_LSHIFT(val, bw, lsb)     ((val & ((1 << bw) - 1)) << lsb)

#ifndef uswap_32
#define uswap_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24))
#endif

#ifndef uswap_16
#define uswap_16(x) \
     ((((x) & 0x0000ff00) >> 8) | \
     (((x) & 0x000000ff) << 8))
#endif

#ifndef cpu_to_be16
#define cpu_to_be16(x)        uswap_16(x)
#endif

#ifndef be16_to_cpu
#define be16_to_cpu(x)        uswap_16(x)
#endif


#ifndef cpu_to_be32
#define cpu_to_be32(x)        uswap_32(x)
#endif

#ifndef be32_to_cpu
#define be32_to_cpu(x)        uswap_32(x)
#endif

#ifndef fdt32_to_cpu
#define fdt32_to_cpu(x)        be32_to_cpu(x)
#endif

#ifndef cpu_to_fdt32
#define cpu_to_fdt32(x)        cpu_to_be32(x)
#endif

static inline void ts_generic_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = TS_BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + TS_BIT_WORD(nr);

	*p  |= mask;
}

static inline void ts_generic_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = TS_BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + TS_BIT_WORD(nr);

	*p &= ~mask;
}

static inline int ts_test_bit(int nr, const void *addr)
{
	return ((unsigned char *)addr)[nr >> 3] & (1U << (nr & 7));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_BITOP_H__ */
