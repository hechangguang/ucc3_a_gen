/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Generic barrier definitions.
 *
 * It should be possible to use these on really simple architectures,
 * but it serves more as a starting point for new ports.
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */
#ifndef __RSICV_BARRIER_H__
#define __RSICV_BARRIER_H__

#define RISCV_FENCE(p, s)       __asm__ __volatile__ ("fence " #p "," #s : : : "memory")

/* These barriers need to enforce ordering on both devices or memory. */
#define mb()            RISCV_FENCE(iorw,iorw)
#define rmb()           RISCV_FENCE(ir,ir)
#define wmb()           RISCV_FENCE(ow,ow)

/* These barriers do not need to enforce ordering on devices, just memory. */
#define __smp_mb()	    RISCV_FENCE(rw,rw)
#define __smp_rmb()	    RISCV_FENCE(r,r)
#define __smp_wmb()	    RISCV_FENCE(w,w)

#ifndef isb
#define isb() __asm__ __volatile__("" : : : "memory")
#endif

#ifndef dsb
#define dsb() __asm__ __volatile__("" : : : "memory")
#endif

#ifndef dmb
#define dmb() __asm__ __volatile__("" : : : "memory")
#endif


#endif /* __RSICV_BARRIER_H */
