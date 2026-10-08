/* 
 * e907_encoding.h -- e907 csr 等寄存器定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __E907_ENCODING_H__
#define __E907_ENCODING_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <asm/riscv_bits.h>

/* === Standard CSR bit mask === */
#define MSTATUS_UIE         0x00000001
#define MSTATUS_SIE         0x00000002
#define MSTATUS_HIE         0x00000004
#define MSTATUS_MIE         0x00000008
#define MSTATUS_UPIE        0x00000010
#define MSTATUS_SPIE        0x00000020
#define MSTATUS_UBE         0x00000040
#define MSTATUS_MPIE        0x00000080
#define MSTATUS_SPP         0x00000100
#define MSTATUS_VS          0x00000600
#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000
#define MSTATUS_XS          0x00018000
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_SUM         0x00040000
#define MSTATUS_MXR         0x00080000
#define MSTATUS_TVM         0x00100000
#define MSTATUS_TW          0x00200000
#define MSTATUS_TSR         0x00400000
#define MSTATUS32_SD        0x80000000
#define MSTATUS_UXL         0x0000000300000000
#define MSTATUS_SXL         0x0000000C00000000
#define MSTATUS_SBE         0x0000001000000000
#define MSTATUS_MBE         0x0000002000000000
#define MSTATUS_GVA         0x0000004000000000
#define MSTATUS_MPV         0x0000008000000000
#define MSTATUS64_SD        0x8000000000000000

#define MSTATUS_FS_INITIAL  0x00002000
#define MSTATUS_FS_CLEAN    0x00004000
#define MSTATUS_FS_DIRTY    0x00006000

#define MSTATUS_VS_INITIAL  0x00000200
#define MSTATUS_VS_CLEAN    0x00000400
#define MSTATUS_VS_DIRTY    0x00000600

#define MSTATUSH_SBE        0x00000010
#define MSTATUSH_MBE        0x00000020
#define MSTATUSH_GVA        0x00000040
#define MSTATUSH_MPV        0x00000080

#define SSTATUS_UIE         0x00000001
#define SSTATUS_SIE         0x00000002
#define SSTATUS_UPIE        0x00000010
#define SSTATUS_SPIE        0x00000020
#define SSTATUS_UBE         0x00000040
#define SSTATUS_SPP         0x00000100
#define SSTATUS_VS          0x00000600
#define SSTATUS_FS          0x00006000
#define SSTATUS_XS          0x00018000
#define SSTATUS_SUM         0x00040000
#define SSTATUS_MXR         0x00080000
#define SSTATUS32_SD        0x80000000
#define SSTATUS_UXL         0x0000000300000000
#define SSTATUS64_SD        0x8000000000000000

#define USTATUS_UIE         0x00000001
#define USTATUS_UPIE        0x00000010

#define DCSR_XDEBUGVER      (3U<<30)
#define DCSR_NDRESET        (1<<29)
#define DCSR_FULLRESET      (1<<28)
#define DCSR_EBREAKM        (1<<15)
#define DCSR_EBREAKH        (1<<14)
#define DCSR_EBREAKS        (1<<13)
#define DCSR_EBREAKU        (1<<12)
#define DCSR_STOPCYCLE      (1<<10)
#define DCSR_STOPTIME       (1<<9)
#define DCSR_CAUSE          (7<<6)
#define DCSR_DEBUGINT       (1<<5)
#define DCSR_HALT           (1<<3)
#define DCSR_STEP           (1<<2)
#define DCSR_PRV            (3<<0)

#define DCSR_CAUSE_NONE     0
#define DCSR_CAUSE_SWBP     1
#define DCSR_CAUSE_HWBP     2
#define DCSR_CAUSE_DEBUGINT 3
#define DCSR_CAUSE_STEP     4
#define DCSR_CAUSE_HALT     5

#define MCONTROL_TYPE(xlen)    (0xfULL<<((xlen)-4))
#define MCONTROL_DMODE(xlen)   (1ULL<<((xlen)-5))
#define MCONTROL_MASKMAX(xlen) (0x3fULL<<((xlen)-11))

#define MCONTROL_SELECT     (1<<19)
#define MCONTROL_TIMING     (1<<18)
#define MCONTROL_ACTION     (0x3f<<12)
#define MCONTROL_CHAIN      (1<<11)
#define MCONTROL_MATCH      (0xf<<7)
#define MCONTROL_M          (1<<6)
#define MCONTROL_H          (1<<5)
#define MCONTROL_S          (1<<4)
#define MCONTROL_U          (1<<3)
#define MCONTROL_EXECUTE    (1<<2)
#define MCONTROL_STORE      (1<<1)
#define MCONTROL_LOAD       (1<<0)

#define MCONTROL_TYPE_NONE      0
#define MCONTROL_TYPE_MATCH     2

#define MCONTROL_ACTION_DEBUG_EXCEPTION   0
#define MCONTROL_ACTION_DEBUG_MODE        1
#define MCONTROL_ACTION_TRACE_START       2
#define MCONTROL_ACTION_TRACE_STOP        3
#define MCONTROL_ACTION_TRACE_EMIT        4

#define MCONTROL_MATCH_EQUAL     0
#define MCONTROL_MATCH_NAPOT     1
#define MCONTROL_MATCH_GE        2
#define MCONTROL_MATCH_LT        3
#define MCONTROL_MATCH_MASK_LOW  4
#define MCONTROL_MATCH_MASK_HIGH 5

#define MIP_SSIP            (1 << IRQ_S_SOFT)
#define MIP_HSIP            (1 << IRQ_H_SOFT)
#define MIP_MSIP            (1 << IRQ_M_SOFT)
#define MIP_STIP            (1 << IRQ_S_TIMER)
#define MIP_HTIP            (1 << IRQ_H_TIMER)
#define MIP_MTIP            (1 << IRQ_M_TIMER)
#define MIP_SEIP            (1 << IRQ_S_EXT)
#define MIP_HEIP            (1 << IRQ_H_EXT)
#define MIP_MEIP            (1 << IRQ_M_EXT)

#define MIE_SSIE            MIP_SSIP
#define MIE_HSIE            MIP_HSIP
#define MIE_MSIE            MIP_MSIP
#define MIE_STIE            MIP_STIP
#define MIE_HTIE            MIP_HTIP
#define MIE_MTIE            MIP_MTIP
#define MIE_SEIE            MIP_SEIP
#define MIE_HEIE            MIP_HEIP
#define MIE_MEIE            MIP_MEIP

#define MCAUSE_INTR         (1ULL << (__riscv_xlen - 1))
#define MCAUSE_CAUSE        0x00000FFFUL
#define SCAUSE_INTR         MCAUSE_INTR
#define SCAUSE_CAUSE        0x000003FFUL

/* === Standard RISC-V CSR Registers === */
#define CSR_USTATUS     0x0
#define CSR_FFLAGS      0x1
#define CSR_FRM         0x2
#define CSR_FCSR        0x3
#define CSR_VSTART      0x8
#define CSR_VXSAT       0x9
#define CSR_VXRM        0xa
#define CSR_VCSR        0xf
#define CSR_SEED        0x15


#define CSR_CYCLE       0xc00
#define CSR_TIME        0xc01
#define CSR_INSTRET     0xc02
#define CSR_CYCLEH      0xc80
#define CSR_TIMEH       0xc81
#define CSR_INSTRETH    0xc82

#define CSR_MVENDORID   0xf11
#define CSR_MARCHID     0xf12
#define CSR_MIMPID      0xf13
#define CSR_MHARTID     0xf14


#define CSR_MSTATUS     0x300
#define CSR_MISA        0x301
#define CSR_MEDELEG     0x302
#define CSR_MIDELEG     0x303
#define CSR_MIE         0x304
#define CSR_MTVEC       0x305
#define CSR_MSCRATCH    0x340
#define CSR_MEPC        0x341
#define CSR_MCAUSE      0x342
#define CSR_MTVAL       0x343
#define CSR_MIP         0x344

#define CSR_TSELECT     0x7a0
#define CSR_TDATA1      0x7a1
#define CSR_TDATA2      0x7a2
#define CSR_TDATA3      0x7a3
#define CSR_TINFO       0x7a4
#define CSR_TCONTROL    0x7a5
#define CSR_MCONTEXT    0x7a8
#define CSR_MSCONTEXT   0x7aa

#define CSR_DCSR        0x7b0
#define CSR_DPC         0x7b1
#define CSR_DSCRATCH0   0x7b2

#define CSR_MCYCLE      0xb00
#define CSR_MINSTRET    0xb02
#define CSR_MCYCLEH     0xb80
#define CSR_MINSTRETH   0xb82


/* === Non-standard RISC-V CSR Registers === */
#define CSR_MCOUNTEN    0x7e0

#define CSR_IPICCISV    0xbf0
#define CSR_IPICCICSR   0xbf1
#define CSR_IPICIPR     0xbf2
#define CSR_IPICISVR    0xbf3
#define CSR_IPICEOI     0xbf4
#define CSR_IPICSOI     0xbf5
#define CSR_IPICIDX     0xbf6
#define CSR_IPICICSR    0xbf7


#ifdef __cplusplus
}
#endif
#endif /* __E907_ENCODING_H__ */
