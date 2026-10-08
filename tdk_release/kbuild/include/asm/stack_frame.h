/* 
 * stack_frame.h -- riscv 栈帧设置
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __STACK_FRAME_H__
#define __STACK_FRAME_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <asm/riscv_bits.h>

/* 按照 8 字节排布 */
typedef struct tsp_stack_frame_s {
    ts_rv_int_t ra;         /* x1  - ra     - return address for jumps            */
    
#if 0
    ts_rv_int_t gp;         /* x3  - gp     - global pointer                      */
    ts_rv_int_t tp;         /* x4  - tp     - thread pointer                      */
#endif

    ts_rv_int_t t0;         /* x5  - t0     - temporary register 0                */

    ts_rv_int_t t1;         /* x6  - t1     - temporary register 1                */
    ts_rv_int_t t2;         /* x7  - t2     - temporary register 2                */

    ts_rv_int_t s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   */
    ts_rv_int_t s1;         /* x9  - s1     - saved register 1                    */

    ts_rv_int_t a0;         /* x10 - a0     - return value or function argument 0 */
    ts_rv_int_t a1;         /* x11 - a1     - return value or function argument 1 */

    ts_rv_int_t a2;         /* x12 - a2     - function argument 2                 */
    ts_rv_int_t a3;         /* x13 - a3     - function argument 3                 */

    ts_rv_int_t a4;         /* x14 - a4     - function argument 4                 */
    ts_rv_int_t a5;         /* x15 - a5     - function argument 5                 */

#ifndef CONFIG_SUPPORT_32E
    ts_rv_int_t a6;         /* x16 - a6     - function argument 6                 */
    ts_rv_int_t a7;         /* x17 - a7     - function argument 7                 */

    ts_rv_int_t s2;         /* x18 - s2     - saved register 2                    */
    ts_rv_int_t s3;         /* x19 - s3     - saved register 3                    */

    ts_rv_int_t s4;         /* x20 - s4     - saved register 4                    */
    ts_rv_int_t s5;         /* x21 - s5     - saved register 5                    */

    ts_rv_int_t s6;         /* x22 - s6     - saved register 6                    */
    ts_rv_int_t s7;         /* x23 - s7     - saved register 7                    */

    ts_rv_int_t s8;         /* x24 - s8     - saved register 8                    */
    ts_rv_int_t s9;         /* x25 - s9     - saved register 9                    */

    ts_rv_int_t s10;        /* x26 - s10    - saved register 10                   */
    ts_rv_int_t s11;        /* x27 - s11    - saved register 11                   */

    ts_rv_int_t t3;         /* x28 - t3     - temporary register 3                */
    ts_rv_int_t t4;         /* x29 - t4     - temporary register 4                */

    ts_rv_int_t t5;         /* x30 - t5     - temporary register 5                */
    ts_rv_int_t t6;         /* x31 - t6     - temporary register 6                */
#endif

    ts_rv_int_t epc;        /* epc - epc    - program counter                     */
    ts_rv_int_t mstatus;    /*              - machine status register             */

#ifdef TSP_SUPPORT_FLOAT
    ts_rv_float_t f0;      /* f0  */
    ts_rv_float_t f1;      /* f1  */
    ts_rv_float_t f2;      /* f2  */
    ts_rv_float_t f3;      /* f3  */
    ts_rv_float_t f4;      /* f4  */
    ts_rv_float_t f5;      /* f5  */
    ts_rv_float_t f6;      /* f6  */
    ts_rv_float_t f7;      /* f7  */
    ts_rv_float_t f8;      /* f8  */
    ts_rv_float_t f9;      /* f9  */
    ts_rv_float_t f10;     /* f10 */
    ts_rv_float_t f11;     /* f11 */
    ts_rv_float_t f12;     /* f12 */
    ts_rv_float_t f13;     /* f13 */
    ts_rv_float_t f14;     /* f14 */
    ts_rv_float_t f15;     /* f15 */
    ts_rv_float_t f16;     /* f16 */
    ts_rv_float_t f17;     /* f17 */
    ts_rv_float_t f18;     /* f18 */
    ts_rv_float_t f19;     /* f19 */
    ts_rv_float_t f20;     /* f20 */
    ts_rv_float_t f21;     /* f21 */
    ts_rv_float_t f22;     /* f22 */
    ts_rv_float_t f23;     /* f23 */
    ts_rv_float_t f24;     /* f24 */
    ts_rv_float_t f25;     /* f25 */
    ts_rv_float_t f26;     /* f26 */
    ts_rv_float_t f27;     /* f27 */
    ts_rv_float_t f28;     /* f28 */
    ts_rv_float_t f29;     /* f29 */
    ts_rv_float_t f30;     /* f30 */
    ts_rv_float_t f31;     /* f31 */
#endif
} tsp_stack_frame_s_t;

//#define TSP_SF_SIZE         sizeof(tsp_stack_frame_s_t)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __STACK_FRAME_H__ */
