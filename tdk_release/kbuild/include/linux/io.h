/*
 * io.h -- 基本 io 功能实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __LINUX_IO_H__
#define __LINUX_IO_H__

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_compiler.h>

/* Generic virtual read/write. */
#define __arch_getb(a)      (*(volatile unsigned char *)(a))
#define __arch_getw(a)      (*(volatile unsigned short *)(a))
#define __arch_getl(a)      (*(volatile unsigned int *)(a))
#define __arch_getq(a)      (*(volatile unsigned long long *)(a))

#define __arch_putb(v,a)    (*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)    (*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)    (*(volatile unsigned int *)(a) = (v))
#define __arch_putq(v,a)    (*(volatile unsigned long long *)(a) = (v))

/* Generic read/write */
#define __mem_readb(a)      (*(volatile unsigned char *)(a))
#define __mem_readw(a)      (*(volatile unsigned short *)(a))
#define __mem_readl(a)      (*(volatile unsigned int *)(a))
#define __mem_readq(a)      (*(volatile unsigned long long *)(a))

#define __mem_writeb(v,a)   (*(volatile unsigned char *)(a) = (v))
#define __mem_writew(v,a)   (*(volatile unsigned short *)(a) = (v))
#define __mem_writel(v,a)   (*(volatile unsigned int *)(a) = (v))
#define __mem_writeq(v,a)   (*(volatile unsigned long long *)(a) = (v))

#define writeb(v,c)         (*(volatile unsigned char *)(c) = (v))
#define writew(v,c)         (*(volatile unsigned short *)(c) = (v))
#define writel(v,c)         (*(volatile unsigned int *)(c) = (v))
#define writeq(v,c)         (*(volatile unsigned long long *)(c) = (v))

#define readb(c)            (*(volatile unsigned char *)(c))
#define readw(c)            (*(volatile unsigned short *)(c))
#define readl(c)            (*(volatile unsigned int *)(c))
#define readq(c)            (*(volatile unsigned long long *)(c))


#define REG(x)              (*(volatile unsigned int *)(x))


#define __raw_writeb(v,a)    __arch_putb(v,a)
#define __raw_writew(v,a)    __arch_putw(v,a)
#define __raw_writel(v,a)    __arch_putl(v,a)
#define __raw_writeq(v,a)    __arch_putq(v,a)

#define __raw_readb(a)      __arch_getb(a)
#define __raw_readw(a)      __arch_getw(a)
#define __raw_readl(a)      __arch_getl(a)
#define __raw_readq(a)      __arch_getq(a)


/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type,endian,a,v)    __raw_write##type(ts_cpu_to_##endian(v),a)
#define in_arch(type,endian,a)        ts_endian##_to_cpu(__raw_read##type(a))

#define out_le64(a,v)       out_arch(q,le64,a,v)
#define out_le32(a,v)       out_arch(l,le32,a,v)
#define out_le16(a,v)       out_arch(w,le16,a,v)

#define in_le64(a)          in_arch(q,le64,a)
#define in_le32(a)          in_arch(l,le32,a)
#define in_le16(a)          in_arch(w,le16,a)

#define out_be64(a,v)       out_arch(l,be64,a,v)
#define out_be32(a,v)       out_arch(l,be32,a,v)
#define out_be16(a,v)       out_arch(w,be16,a,v)

#define in_be64(a)          in_arch(l,be64,a)
#define in_be32(a)          in_arch(l,be32,a)
#define in_be16(a)          in_arch(w,be16,a)

#define out_64(a,v)         __raw_writeq(v,a)
#define out_32(a,v)         __raw_writel(v,a)
#define out_16(a,v)         __raw_writew(v,a)
#define out_8(a,v)          __raw_writeb(v,a)

#define in_64(a)            __raw_readq(a)
#define in_32(a)            __raw_readl(a)
#define in_16(a)            __raw_readw(a)
#define in_8(a)             __raw_readb(a)

#define clrbits(type, addr, clear)          out_##type((addr), in_##type(addr) & ~(clear))
#define setbits(type, addr, set)            out_##type((addr), in_##type(addr) | (set))
#define clrsetbits(type, addr, clear, set)  out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

#define clrbits_be32(addr, clear)           clrbits(be32, addr, clear)
#define setbits_be32(addr, set)             setbits(be32, addr, set)
#define clrsetbits_be32(addr, clear, set)   clrsetbits(be32, addr, clear, set)

#define clrbits_le32(addr, clear)           clrbits(le32, addr, clear)
#define setbits_le32(addr, set)             setbits(le32, addr, set)
#define clrsetbits_le32(addr, clear, set)   clrsetbits(le32, addr, clear, set)

#define clrbits_32(addr, clear)             clrbits(32, addr, clear)
#define setbits_32(addr, set)               setbits(32, addr, set)
#define clrsetbits_32(addr, clear, set)     clrsetbits(32, addr, clear, set)

#define clrbits_be16(addr, clear)           clrbits(be16, addr, clear)
#define setbits_be16(addr, set)             setbits(be16, addr, set)
#define clrsetbits_be16(addr, clear, set)   clrsetbits(be16, addr, clear, set)

#define clrbits_le16(addr, clear)           clrbits(le16, addr, clear)
#define setbits_le16(addr, set)             setbits(le16, addr, set)
#define clrsetbits_le16(addr, clear, set)   clrsetbits(le16, addr, clear, set)

#define clrbits_16(addr, clear)             clrbits(16, addr, clear)
#define setbits_16(addr, set)               setbits(16, addr, set)
#define clrsetbits_16(addr, clear, set)     clrsetbits(16, addr, clear, set)

#define clrbits_8(addr, clear)              clrbits(8, addr, clear)
#define setbits_8(addr, set)                setbits(8, addr, set)
#define clrsetbits_8(addr, clear, set)      clrsetbits(8, addr, clear, set)

#define clrbits_be64(addr, clear)           clrbits(be64, addr, clear)
#define setbits_be64(addr, set)             setbits(be64, addr, set)
#define clrsetbits_be64(addr, clear, set)   clrsetbits(be64, addr, clear, set)

#define clrbits_le64(addr, clear)           clrbits(le64, addr, clear)
#define setbits_le64(addr, set)             setbits(le64, addr, set)
#define clrsetbits_le64(addr, clear, set)   clrsetbits(le64, addr, clear, set)

#define clrbits_64(addr, clear)             clrbits(64, addr, clear)
#define setbits_64(addr, set)               setbits(64, addr, set)
#define clrsetbits_64(addr, clear, set)         clrsetbits(64, addr, clear, set)

#endif /* __LINUX_IO_H__ */
