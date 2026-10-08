/* 
 * cpl_ioctl.h -- cpl ioctl 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_IOCTL_H__
#define __CPL_IOCTL_H__

#define _TS_CPL_IOC_NRBITS          (8)
#define _TS_CPL_IOC_TYPEBITS        (8)
#define _TS_CPL_IOC_SIZEBITS        (14)
#define _TS_CPL_IOC_DIRBITS         (2)

#define _TS_CPL_IOC_NRMASK          ((1 << _TS_CPL_IOC_NRBITS) - 1)
#define _TS_CPL_IOC_TYPEMASK        ((1 << _TS_CPL_IOC_TYPEBITS) - 1)
#define _TS_CPL_IOC_SIZEMASK        ((1 << _TS_CPL_IOC_SIZEBITS) - 1)
#define _TS_CPL_IOC_DIRMASK         ((1 << _TS_CPL_IOC_DIRBITS) - 1)

#define _TS_CPL_IOC_NRSHIFT         (0)
#define _TS_CPL_IOC_TYPESHIFT       (_TS_CPL_IOC_NRSHIFT + _TS_CPL_IOC_NRBITS)
#define _TS_CPL_IOC_SIZESHIFT       (_TS_CPL_IOC_TYPESHIFT + _TS_CPL_IOC_TYPEBITS)
#define _TS_CPL_IOC_DIRSHIFT        (_TS_CPL_IOC_SIZESHIFT + _TS_CPL_IOC_SIZEBITS)

#define _TS_CPL_IOC_NONE            (0U)
#define _TS_CPL_IOC_WRITE           (1U)
#define _TS_CPL_IOC_READ            (2U)

#define _TS_CPL_IOC(dir, type, nr, size)                                         \
	(((dir) << _TS_CPL_IOC_DIRSHIFT) | ((type) << _TS_CPL_IOC_TYPESHIFT) |     \
	 ((nr) << _TS_CPL_IOC_NRSHIFT) | ((size) << _TS_CPL_IOC_SIZESHIFT))

#define _TS_CPL_IOC_TYPECHECK(t)    (sizeof(t))

#define TS_CPL_IO(type, nr)         _TS_CPL_IOC(_TS_CPL_IOC_NONE, (type), (nr), 0)
#define TS_CPL_IOR(type, nr, size)                                               \
	_TS_CPL_IOC(_TS_CPL_IOC_READ, (type), (nr), (_TS_CPL_IOC_TYPECHECK(size)))
#define TS_CPL_IOW(type, nr, size)                                               \
	_TS_CPL_IOC(_TS_CPL_IOC_WRITE, (type), (nr), (_TS_CPL_IOC_TYPECHECK(size)))
#define TS_CPL_IOWR(type, nr, size)                                              \
	_TS_CPL_IOC(_TS_CPL_IOC_READ | _TS_CPL_IOC_WRITE, (type), (nr), (_TS_CPL_IOC_TYPECHECK(size)))
#define TS_CPL_IOR_BAD(type, nr, size)                                           \
	_TS_CPL_IOC(_TS_CPL_IOC_READ, (type), (nr), sizeof(size))
#define TS_CPL_IOW_BAD(type, nr, size)                                           \
	_TS_CPL_IOC(_TS_CPL_IOC_WRITE, (type), (nr), sizeof(size))
#define TS_CPL_IOWR_BAD(type, nr, size)                                          \
	_TS_CPL_IOC(_TS_CPL_IOC_READ | _TS_CPL_IOC_WRITE, (type), (nr), sizeof(size))

/* used to decode ioctl numbers.. */
#define _TS_CPL_IOC_DIR(nr)         (((nr) >> _TS_CPL_IOC_DIRSHIFT) & _TS_CPL_IOC_DIRMASK)
#define _TS_CPL_IOC_TYPE(nr)        (((nr) >> _TS_CPL_IOC_TYPESHIFT) & _TS_CPL_IOC_TYPEMASK)
#define _TS_CPL_IOC_NR(nr)          (((nr) >> _TS_CPL_IOC_NRSHIFT) & _TS_CPL_IOC_NRMASK)
#define _TS_CPL_IOC_SIZE(nr)        (((nr) >> _TS_CPL_IOC_SIZESHIFT) & _TS_CPL_IOC_SIZEMASK)

#define TS_CPL_IOC_IN               (_TS_CPL_IOC_WRITE << _TS_CPL_IOC_DIRSHIFT)
#define TS_CPL_IOC_OUT              (_TS_CPL_IOC_READ << _TS_CPL_IOC_DIRSHIFT)
#define TS_CPL_IOC_INOUT            ((_TS_CPL_IOC_WRITE | _TS_CPL_IOC_READ) << _TS_CPL_IOC_DIRSHIFT)
#define TS_CPL_IOCSIZE_MASK         (_TS_CPL_IOC_SIZEMASK << _TS_CPL_IOC_SIZESHIFT)
#define TS_CPL_IOCSIZE_SHIFT        (_TS_CPL_IOC_SIZESHIFT)


/* 统一管理模块魔术字 */

/* 基础模块 0x01 - 0x1F */
#define TS_IOC_MAGIC_UNKNOW     (0x00)
#define TS_IOC_MAGIC_SYS        (0x01)
#define TS_IOC_MAGIC_BASE       (0x02)
#define TS_IOC_MAGIC_LOG        (0x03)
#define TS_IOC_MAGIC_CLKM		(0x04)

/* 大型模块 0x20 - 0x4F */
#define TS_IOC_MAGIC_VIU		(0x20)
#define TS_IOC_MAGIC_ISP        (0x21)
#define TS_IOC_MAGIC_VPO		(0x22)
#define TS_IOC_MAGIC_OSD        (0x23)
#define TS_IOC_MAGIC_AF         (0x24)
#define TS_IOC_MAGIC_AISP       (0x25)

/* 基础外设 0x50 - 0x7F */
#define TS_IOC_MAGIC_GPIO       (0x50)
#define TS_IOC_MAGIC_SPI        (0x51)
#define TS_IOC_MAGIC_SFC        (0x52)
#define TS_IOC_MAGIC_PWM        (0x53)
#define TS_IOC_MAGIC_USB        (0x54)
#define TS_IOC_MAGIC_IIC        (0x55)
#define TS_IOC_MAGIC_UART       (0x56)
#define TS_IOC_MAGIC_SSBUS      (0x57)
#define TS_IOC_MAGIC_TIMER      (0x58)
#define TS_IOC_MAGIC_RTC        (0x59)
#define TS_IOC_MAGIC_MBOX       (0x5A)
#define TS_IOC_MAGIC_RNG        (0x5B)
#define TS_IOC_MAGIC_WDT        (0x5C)
#define TS_IOC_MAGIC_DMA        (0x5E)

/* 供外部客户使用 0x80 - 0xFF */



#endif /* __CPL_IOCTL_H__ */
