/* 
 * arch_atomic.h -- rsicv 原子操作
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __RISCV_OPS_H__
#define __RISCV_OPS_H__

#ifdef __cplusplus
 extern "C" {
#endif
#include <config.h>

#ifdef CONFIG_HAVE_ATOMIC_INSTR

#define ATOMIC_OP(op, asm_op, I, asm_type, c_type)		\
static inline __attribute__((__always_inline__))							\
void atomic_##op(c_type i, atomic_t *v)		\
{									\
	__asm__ __volatile__ (						\
		"	amo" #asm_op "." #asm_type " zero, %1, %0"	\
		: "+A" (v->counter)					\
		: "r" (I)						\
		: "memory");						\
}

#define ATOMIC_OPS(op, asm_op, I)		ATOMIC_OP(op, asm_op, I, w, int)

ATOMIC_OPS(add, add,  i)
ATOMIC_OPS(sub, add, -i)
ATOMIC_OPS(and, and,  i)
ATOMIC_OPS( or,  or,  i)
ATOMIC_OPS(xor, xor,  i)

#undef ATOMIC_OP
#undef ATOMIC_OPS

#define ATOMIC_FETCH_OP(op, asm_op, I, asm_type, c_type)	\
static inline __attribute__((__always_inline__))							\
c_type atomic_fetch_##op(c_type i, atomic_t *v)	\
{									\
	register c_type ret;						\
	__asm__ __volatile__ (						\
		"	amo" #asm_op "." #asm_type ".aqrl  %1, %2, %0"	\
		: "+A" (v->counter), "=r" (ret)				\
		: "r" (I)						\
		: "memory");						\
	return ret;							\
}

#define ATOMIC_OP_RETURN(op, asm_op, c_op, I, asm_type, c_type)	\
static inline __attribute__((__always_inline__))							\
c_type atomic_##op##_return(c_type i, atomic_t *v)	\
{									\
	return atomic_fetch_##op(i, v) c_op I;		\
}

#define ATOMIC_OPS(op, asm_op, c_op, I)					\
        ATOMIC_FETCH_OP( op, asm_op,       I, w, int)		\
        ATOMIC_OP_RETURN(op, asm_op, c_op, I, w, int)

ATOMIC_OPS(add, add, +,  i)
ATOMIC_OPS(sub, add, +, -i)

#undef ATOMIC_OPS
#undef ATOMIC_FETCH_OP
#undef ATOMIC_OP_RETURN
#else

#include <base/ts_type.h>

extern ts_u32 irq_save(ts_void);
extern ts_void irq_restore(ts_u32 u32_flag);

static __inline__ void atomic_add(int i, atomic_t *v)
{
    ts_u32 u32_flags;

    u32_flags = irq_save();
    v->counter += i;
    irq_restore(u32_flags);
}

static __inline__ void atomic_sub(int i, atomic_t *v) 
{
    ts_u32 u32_flags;

    u32_flags = irq_save();
    v->counter -= i;
    irq_restore(u32_flags);
}

static __inline__ void atomic_inc(atomic_t *v)
{
    atomic_add(1, v);
}


static __inline__ int atomic_add_return(int i, atomic_t * v)
{
    ts_u32 u32_temp, u32_flags;

    u32_flags = irq_save();
    u32_temp = v->counter;
    u32_temp += i;
    v->counter = u32_temp;
    irq_restore(u32_flags);

    return u32_temp;
}

static __inline__ int atomic_sub_return(int i, atomic_t * v)
{
    ts_u32 u32_temp, u32_flags;

    u32_flags = irq_save();
    u32_temp = v->counter;
    u32_temp -= i;
    v->counter = u32_temp;
    irq_restore(u32_flags);

    return u32_temp;
}

#endif

#ifdef __cplusplus
}
#endif
#endif /* __RISCV_ENCODING_H__ */
