/* 
 * rsicv_ops.h -- rsicv 常见功能汇编内联
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

#include <asm/riscv_encoding.h>

#define csr_readl(csr)                     \
    ({                                      \
        ts_u32 __v;                         \
        asm volatile("csrr %0, " __STR(csr)     \
                     : "=r"(__v)            \
                     :                      \
                     : "memory");           \
        __v;                                \
    })

#define csr_writel(val, csr)               \
    ({                                      \
        ts_u32 __v = (ts_u32)(val);         \
        asm volatile("csrw " __STR(csr) ", %0"    \
                     :                      \
                     : "r"(__v)             \
                     : "memory");           \
    })

#define csr_set(csr, val)                                  \
    ({                                                          \
        ts_u32 __v = (ts_u32)(val);                         \
        asm volatile("csrs " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "r"(__v)                                \
                     : "memory");                               \
    })

#define csr_clear(csr, val)                                \
    ({                                                          \
        ts_u32 __v = (ts_u32)(val);                         \
        asm volatile("csrc " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "r"(__v)                                \
                     : "memory");                               \
    })


static inline void __FENCE_I(void)
{
    asm("fence.i");
}

static inline void __WFI(void)
{
    asm("wfi");
}


static inline void __enable_irq(void)
{
    csr_set(CSR_MSTATUS, MSTATUS_MIE);
}

/**
 * \brief   Disable IRQ Interrupts
 * \details Disables IRQ interrupts by clearing the MIE-bit in the MSTATUS Register.
 * \remarks
 *          Can only be executed in Privileged modes.
 */
static inline void __disable_irq(void)
{
    csr_clear(CSR_MSTATUS, MSTATUS_MIE);
}


#ifdef __cplusplus
}
#endif
#endif /* __RISCV_ENCODING_H__ */
