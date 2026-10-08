/* 
 * soc_main.c -- t10x 支持
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <io.h>

#define PRC_MAX_NUM     (2)
#define UART_MAX_NUM    (5)

typedef enum pmp_size_en {
    PMP_SIZE_128B = 4,
    PMP_SIZE_256B = 5,
    PMP_SIZE_512B = 6,
    PMP_SIZE_1KB = 7,
    PMP_SIZE_2KB = 8,
    PMP_SIZE_4KB = 9,
    PMP_SIZE_8KB = 10,
    PMP_SIZE_16KB = 11,
    PMP_SIZE_32KB = 12,
    PMP_SIZE_64KB = 13,
    PMP_SIZE_128KB = 14,
    PMP_SIZE_256KB = 15,
    PMP_SIZE_512KB = 16,
    PMP_SIZE_1MB = 17,
    PMP_SIZE_2MB = 18,
    PMP_SIZE_4MB = 19,
    PMP_SIZE_8MB = 20,
    PMP_SIZE_16MB = 21,
    PMP_SIZE_32MB = 22,
    PMP_SIZE_64MB = 23,
    PMP_SIZE_128MB = 24,

    PMP_BUTT
} pmp_size_en_t;

/* 用于支持多片 psram */
ts_u32 gau32_regbase[PRC_MAX_NUM] = {0x001d0000, 0x001e0000};

/* 串口寄存器 */
ts_u32 gau32_uart_regs[UART_MAX_NUM] = {0x00210000, 0x00220000, 0x00100000, 0x00110000, 0x00120000};


ts_u32 soc_get_ddr_bank_regbase(ts_u32 u32_bank_idx)
{
    if (u32_bank_idx < PRC_MAX_NUM)
        return gau32_regbase[u32_bank_idx];
    else
        return 0;
}


ts_u32 soc_get_osc_clk(ts_void)
{
    return 24000000;
}


ts_u32 soc_get_uart_regbase(ts_u32 u32_dev_idx)
{
    if (u32_dev_idx < UART_MAX_NUM)
        return gau32_uart_regs[u32_dev_idx];
    else
        return 0;
}


ts_void soc_uart_set_mux(ts_u32 u32_dev_idx)
{
    //
}


static ts_u32 soc_calc_pmp_napot(ts_u32 u32_addr, pmp_size_en_t en_size)
{
    ts_u32 u32_ret_addr;

    u32_ret_addr = ((u32_addr >> 2) & (0xFFFFFFFFU - ((1 << (en_size + 1)) - 1))) | ((1 << en_size) - 1);

    return u32_ret_addr;
}


static ts_void pmp_init(ts_void)
{
    ts_u32 u32_tmp;

    u32_tmp = soc_calc_pmp_napot(0x2000000, PMP_SIZE_2MB);
    asm volatile("csrw pmpaddr0, %0" : : "rK"(u32_tmp) : "memory");

    asm volatile("csrr %0, pmpcfg0" : "=r"(u32_tmp):: "memory");
    u32_tmp |= 0x9B;
    asm volatile("csrw pmpcfg0, %0" : : "rK"(u32_tmp) : "memory");
}


ts_void soc_init(ts_void)
{
    ts_u32 u32_reg_addr;
    ts_u32 u32_reg_val;

    pmp_init();

    /* UART0 和 UART1 的 RXD 管脚下拉关闭 */
    u32_reg_addr = 0x00160000 + 0x0130;
    u32_reg_val = readl(u32_reg_addr);
    
    /* 12, 14 for RXD */
    u32_reg_val &= ~(1 << 12) | (1 << 14);
    writel(u32_reg_val, u32_reg_addr);

    /* 上拉 */
    u32_reg_addr = 0x00160000 + 0x0150;
    u32_reg_val = readl(u32_reg_addr);
    
    /* 12, 14 for RXD */
    u32_reg_val |= (1 << 12) | (1 << 14);
    writel(u32_reg_val, u32_reg_addr);
}
