/*
 * io.h -- 基本 io 功能实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef _IO_H_
#define _IO_H_

#include <config.h>

#include <base/ts_type.h>

/* Generic read/write */
#define __mem_readb(a)			(*(volatile unsigned char *)(a))
#define __mem_readw(a)			(*(volatile unsigned short *)(a))
#define __mem_readl(a)			(*(volatile unsigned int *)(a))
#define __mem_readq(a)			(*(volatile unsigned long long *)(a))

#define __mem_writeb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define __mem_writew(v,a)		(*(volatile unsigned short *)(a) = (v))
#define __mem_writel(v,a)		(*(volatile unsigned int *)(a) = (v))
#define __mem_writeq(v,a)		(*(volatile unsigned long long *)(a) = (v))

#define writeb(v,c)	            (*(volatile unsigned char *)(c) = (v))
#define writew(v,c)	            (*(volatile unsigned short *)(c) = (v))
#define writel(v,c)	            (*(volatile unsigned int *)(c) = (v))
#define writeq(v,c)	            (*(volatile unsigned long long *)(c) = (v))

#define readb(c)	            (*(volatile unsigned char *)(c))
#define readw(c)	            (*(volatile unsigned short *)(c))
#define readl(c)	            (*(volatile unsigned int *)(c))
#define readq(c)	            (*(volatile unsigned long long *)(c))

#ifndef REG
#define REG(x)					(*(volatile unsigned int *)(x))

#endif

#endif /* _IO_H_ */

