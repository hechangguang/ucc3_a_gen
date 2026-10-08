/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ATMOIC_H
#define _LINUX_ATMOIC_H

#include <config.h>

typedef struct {
	int counter;
} atomic_t;

#define __scalar_type_to_expr_cases(type)				\
		unsigned type:	(unsigned type)0,			\
		signed type:	(signed type)0

#define __unqual_scalar_typeof(x) typeof(				\
		_Generic((x),						\
			 char:	(char)0,				\
			 __scalar_type_to_expr_cases(char),		\
			 __scalar_type_to_expr_cases(short),		\
			 __scalar_type_to_expr_cases(int),		\
			 __scalar_type_to_expr_cases(long),		\
			 __scalar_type_to_expr_cases(long long),	\
			 default: (x)))

#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)


#define READ_ONCE(x)			__READ_ONCE(x)
#define WRITE_ONCE(x, val)		__WRITE_ONCE(x, val)

static inline __attribute__((__always_inline__)) int atomic_read(const atomic_t *v)
{
	return READ_ONCE(v->counter);
}

static inline __attribute__((__always_inline__)) void atomic_set(atomic_t *v, int i)
{
	WRITE_ONCE(v->counter, i);
}

#include <asm/atomic.h>

static inline __attribute__((__always_inline__)) void set_bit(unsigned int nr, volatile unsigned long *p)
{
	/*  linux/bits.h  BIT_WORD(nr) ==> nr / BITS_PER_LONG */
	p += (nr >> 5);
	atomic_or(1 << (nr & 31), (atomic_t *)p);
}


#endif	/* _LINUX_ATMOIC_H */
