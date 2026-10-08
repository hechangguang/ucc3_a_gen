/* 
 * uart.h -- tg uart_v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TG_UART_V100_H__
#define __TG_UART_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_compiler.h>


typedef union rbr_un {
    struct {
        /* (DLAB =0) Receiver Buffer Register */
        ts_u32 rbr:8;           //[0..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} rbr_un_t;

typedef union thr_un {
    struct  {
        /* (DLAB =0) Transmit Holding Register */
        ts_u32 thr:8;           //[0..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} thr_un_t;

typedef union dll_un {
    struct  {
        /* (DLAB =1) Divisor Latch LSB */
        ts_u32 dll:8;           //[0..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} dll_un_t;

typedef union dlh_un {
    struct  {
        /* (DLAB =1) Divisor Latch MSB */
        ts_u32 dlh:8;           //[0..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} dlh_un_t;


/* (DLAB =0) Interrupt Enable Register */
typedef union ier_un {
    struct  {
        /* Bit 0: Enable received data available interrupt */
        ts_u32 erbfi:1;         //[0..0]

        /* Bit 1: Enable THR empty interrupt */
        ts_u32 etbei:1;         //[1..1]

        /* Bit 2: Enable receiver line status interrupt */
        ts_u32 elsi:1;          //[2..2]

        /* Bit 3: Enable MODEM status interrupt */
        ts_u32 edssi:1;         //[3..3]

        /* Reserved */
        ts_u32 rev04_31:28;     //[8..31]
    };
    ts_u32 u32_val; 
} ier_un_t;

/* IIR Interrupt ID Register */
typedef union iir_un {
    struct  {
        /* Interrupt id */
        ts_u32 intrpt_id:4;     //[0..3]

        /* Reserved */
        ts_u32 rev04_05:2;      //[4..5]

        /* FIFO enable */
        ts_u32 fifo_en:2;       //[6..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} iir_un_t;

/* tx empty interrupt id  */
#define UART_IIR_THR_EMPTY      (0x02)

/* rx interrupt id */
#define UART_IIR_RECV_DATA      (0x04)

/* FIFO Control Register */
typedef union fcr_un {
    struct  {
        /* Bit 0:  Enable FIFOs */
        ts_u32 fifo_en:1;       //[0..0]

        /* Bit 1:  RX FIFO Reset */
        ts_u32 rx_rst:1;        //[1..1]

        /* Bit 2:  TX FIFO Reset */
        ts_u32 tx_rst:1;        //[2..2]

        /* Bit 3:  DMA Mode Select */
        ts_u32 dma_mode:1;      //[3..3]

        /* Bits 4-5: TX Trigger Level */
        ts_u32 tx_trigger:2;    //[4..5]

        /* Bits 6-7: RX Trigger Level */
        ts_u32 rx_trigger:2;    //[6..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} fcr_un_t;


/* Line Control Register */
typedef union lcr_un {
    struct  {
        /* Bit 0-1: Word Length Select */
        ts_u32 wls:2;           //[0..1]

        /* Bit 2:  Number of Stop Bits */
        ts_u32 stb:1;           //[2..2]

        /* Bit 3:  Parity Enable */
        ts_u32 pen:1;           //[3..3]

        /* Bit 4:  Even Parity Select */
        ts_u32 eps:1;           //[4..4]

        /* Bit 5:  Stick Parity, force 0 */
        ts_u32 sticky:1;        //[5..5]

        /* Bit 6: Break Control */
        ts_u32 brk:1;           //[6..6]

        /* Bit 7: Divisor Latch Access Bit (DLAB) */
        ts_u32 dlab:1;          //[7..7]

        /* Reserved */
        ts_u32 rev08_31:24;     //[8..31]
    };
    ts_u32 u32_val; 
} lcr_un_t;


/* LSR Line Status Register  */
typedef union lsr_un {
    struct  {
        /* Bit 0:  Data Ready */
        ts_u32 dr:1;        //[0..0]

        /* Bit 1:  Overrun Error */
        ts_u32 oe:1;        //[1..1]

        /* Bit 2:  Parity Error */
        ts_u32 pe:1;        //[2..2]

        /* Bit 3:  Framing Error */
        ts_u32 fe:1;        //[3..3]

        /* Bit 4:  Break Interrupt */
        ts_u32 bi:1;        //[4..4]

        /* Bit 5:  Transmitter Holding Register Empty */
        ts_u32 thre:1;      //[5..5]

        /* Bit 6: Transmitter Empty */
        ts_u32 temt:1;      //[6..6]

        /* Bit 7: Error in RX FIFO (RXFE) */
        ts_u32 rxfe:1;      //[7..7]

        /* Reserved */
        ts_u32 rev08_31:24; //[8..31]
    };
    ts_u32 u32_val; 
} lsr_un_t;


/* User Status Register  */
typedef union usr_un {
    struct  {
        ts_u32 busy:1;      //[0..0]
        ts_u32 tfnf:1;      //[1..1]
        ts_u32 tfe:1;       //[2..2]
        ts_u32 rfne:1;      //[3..3]
        ts_u32 rff:1;       //[4..4]

        /* Reserved */
        ts_u32 rev05_31:27; //[5..31]
    };
    ts_u32 u32_val; 
} usr_un_t;


/* LCR Line Control Register */
#define UART_LCR_WLS_5BIT       (0)
#define UART_LCR_WLS_6BIT       (1)
#define UART_LCR_WLS_7BIT       (2)
#define UART_LCR_WLS_8BIT       (3)


/* FCR FIFO Control Register */

/* Bits 4-5: TX Trigger Level */
#define UART_FCR_TXTRIGGER_1    (0)         /* Trigger level 0 (1 character) */
#define UART_FCR_TXTRIGGER_4    (1)         /* Trigger level 1 (4 characters) */
#define UART_FCR_TXTRIGGER_8    (2)         /* Trigger level 2 (8 characters) */
#define UART_FCR_TXTRIGGER_14   (3)         /* Trigger level 3 (14 characters) */

/* Bits 6-7: RX Trigger Level */
#define UART_FCR_RXTRIGGER_1    (0)         /* Trigger level 0 (1 character) */
#define UART_FCR_RXTRIGGER_4    (1)         /* Trigger level 1 (4 characters) */
#define UART_FCR_RXTRIGGER_8    (2)         /* Trigger level 2 (8 characters) */
#define UART_FCR_RXTRIGGER_14   (3)         /* Trigger level 3 (14 characters) */


typedef struct uart_reg_s {
    /* 0x0000 */
    union {
        __IOM rbr_un_t un_rbr;
        __IOM thr_un_t un_thr;
        __IOM dll_un_t un_dll;
    };

    /* 0x0004 */
    union {
        __IOM dlh_un_t un_dlh;
        __IOM ier_un_t un_ier;
    };

    /* 0x0008 */
    union {
        __IOM iir_un_t un_iir;
        __IOM fcr_un_t un_fcr;
    };

    /* 0x000C */
    __IOM lcr_un_t un_lcr;

    /* 0x0010 */
    __IOM ts_u32 u32_rev10;

    /* 0x0014 */
    __IOM lsr_un_t un_lsr;

    /* 0x0018 - 0x0078 */
    __IOM ts_u32 au32_rev18_78[25];

    /* 0x007C */
    __IOM usr_un_t un_usr;
} uart_reg_s_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TG_UART_V100_H__ */
