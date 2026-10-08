/*
 * uart_v100.h -- uart v100 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UART_V100_H__
#define __UART_V100_H__

/* serial register map */
#define UART_REG_OFFSET_RBR     (0x0000)
#define UART_REG_OFFSET_THR     (0x0000)
#define UART_REG_OFFSET_DLL     (0x0000)
#define UART_REG_OFFSET_DLH     (0x0004)
#define UART_REG_OFFSET_IER     (0x0004)
#define UART_REG_OFFSET_FCR     (0x0008)
#define UART_REG_OFFSET_LCR     (0x000C)
#define UART_REG_OFFSET_LSR     (0x0014)
#define UART_REG_OFFSET_USR     (0x007C)
#define UART_REG_OFFSET_TFL     (0x0080)
#define UART_REG_OFFSET_RFL     (0x0084)

/* define for DLL and DLH */
#define UART_MAX_DL             (0xFFFF)

/* bits of UART_REG_FCR */
#define UART_FCR_RT             (6)
#define UART_FCR_TET            (4)
#define UART_FCR_DMAM           (3)
#define UART_FCR_XFIFOR         (2)
#define UART_FCR_RFIFOR         (1)
#define UART_FCR_FIFOE          (0)

/* bits of UART_REG_LCR */
#define UART_LCR_DLAB           (7)
#define UART_LCR_BC             (6)
#define UART_LCR_SP             (5)
#define UART_LCR_EPS            (4)
#define UART_LCR_PEN            (3)
#define UART_LCR_STOP           (2)
#define UART_LCR_DLS            (0)

#define UART_ENABLE_DIVISOR     (1 << UART_LCR_DLAB)
#define UART_ENABLE_8BIT        (3 << UART_LCR_DLS)

/* bits of UART_REG_OFFSET_LSR */
#define UART_LSR_RFE            (7)
#define UART_LSR_TEMT           (6)
#define UART_LSR_THRE           (5)
#define UART_LSR_BI             (4)
#define UART_LSR_FE             (3)
#define UART_LSR_PE             (2)
#define UART_LSR_OE             (1)
#define UART_LSR_DR             (0)

/* bits of  UART_REG_OFFSET_USR */
#define UART_USR_RFF            (4)
#define UART_USR_RFNE           (3)
#define UART_USR_TFE            (2)
#define UART_USR_TFNF           (1)
#define UART_USR_BUSY           (0)


#define UART_MAX_TIMEOUT        (0x100000)

#endif /* __UART_V100_H__ */
