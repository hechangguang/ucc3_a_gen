/*
 * scpu_v100.c -- slave cpu v100 驱动实现,将从 CPU 镜像加载到指定内存位置，并启动
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <io.h>
#include <bootup.h>
#include <scpu.h>
#include "scpu_v100.h"


/* 打开 scpu 的时钟、hold cpu , 并释放复位 */
static ts_void scpu_enable(ts_void)
{
    ts_u32 u32_tmp;

    /* 开启 CPU 时钟 */
    u32_tmp = readl(0x001300C8);
    u32_tmp |= (1 << 0);
    writel(u32_tmp, 0x001300C8);

    /* 保持复位状态 */
    u32_tmp = readl(0x00190004);
    u32_tmp &= ~(1 << 1);
    writel(u32_tmp, 0x00190004);
}

/* 释放 hold */
static ts_s32 scpu_start(ts_u32 u32_addr)
{
    ts_u32 i, u32_tmp;

    /* 写入跳转地址 */
    writel(u32_addr, 0x0016023C);

    /* 释放 CPU */
    u32_tmp = readl(0x00190004);
    u32_tmp |= (1 << 1);
    writel(u32_tmp, 0x00190004);

    /* 读回启动地址， CPU 再启动后，会将启动地址改写成 0x55aaff00 */
    for (i = 0; i < 100; i++) {
        u32_tmp = readl(0x0016023C);
        if (0x55aaff00 == u32_tmp)
            return TS_OK;
    }

    return TS_ERR;
}


ts_s32 scpu_load(ts_void)
{
    ts_u32 u32_img_addr, u32_start_addr, u32_end_addr;
    ts_u32 i, u32_loop, u32_size;
    ts_u32 u32_chk_crc, u32_rd_crc;
    ts_u08 *pu08_buf;
    bootup_ih_s_t st_img_hdr;
    ts_s32 s32_ret;

    hook_uart_init(1, 115200);

    printf("scpu in\n");
    hook_uart_puts("load...\n");

    scpu_enable();

    u32_img_addr = CONFIG_SCPU_LOAD_ADDR;
    u32_loop = CONFIG_SCPU_LOAD_BCNT + 1;

    /* 确定备份镜像的范围 */
    u32_start_addr = CONFIG_SCPU_LOAD_ADDR;
    u32_end_addr = CONFIG_SCPU_LOAD_ADDR + (CONFIG_SCPU_LOAD_BAK_OFFSET * CONFIG_SCPU_LOAD_BCNT);

    /* 读入头 */
    for (i = 0; i < u32_loop; i++) {
        s32_ret = loader_read(u32_img_addr, (ts_u08 *)&st_img_hdr, sizeof(bootup_ih_s_t));
        if (TS_OK == s32_ret) {
            pu08_buf = (ts_u08 *)&st_img_hdr;

            /* 跳过头和 crc 部分 */
            pu08_buf += 8;

            u32_rd_crc = st_img_hdr.u32_ih_hcrc;
            u32_size = sizeof(bootup_ih_s_t) - 8;
            u32_chk_crc = verify_lib_calc_crc32(0, pu08_buf, u32_size);
            if (u32_chk_crc == u32_rd_crc)
                goto __load_img;
        } else if (TS_EIO == s32_ret) {
            return TS_EIO;
        }

        u32_img_addr += CONFIG_SCPU_LOAD_BAK_OFFSET;
    }

    return TS_ERR;

__load_img:

    /* 加载镜像 */
    u32_rd_crc = st_img_hdr.u32_ih_dcrc;
    u32_size = st_img_hdr.u32_ih_size;

    /* fix , 主 cpu 不能直接访问到从 cpu 的 ildm， 需要重新映射到全局地址 */
    if (0x01000000 == st_img_hdr.u32_ih_ep)
        pu08_buf = (ts_u08 *)(0x02100000);
    else
        pu08_buf = (ts_u08 *)st_img_hdr.u32_ih_ep;

    printf("scpu load %#x to %#x\n", u32_img_addr, (ts_u32)pu08_buf);
    for (i = 0; i < u32_loop; i++) {
        s32_ret = loader_read((u32_img_addr + sizeof(bootup_ih_s_t)), pu08_buf, u32_size);
        if (TS_OK == s32_ret) {
            u32_chk_crc = verify_lib_calc_crc32(0, pu08_buf, u32_size);
            if (u32_chk_crc == u32_rd_crc) {
                s32_ret = scpu_start(st_img_hdr.u32_ih_ep);
                printf("scpu start to %#x, ret = %d\n", (ts_u32)pu08_buf, s32_ret);

                return s32_ret;
            }
        }

        u32_img_addr += CONFIG_SCPU_LOAD_BAK_OFFSET;
        if (u32_img_addr > u32_end_addr)
            u32_img_addr = u32_start_addr;
    }

    return TS_ERR;
}