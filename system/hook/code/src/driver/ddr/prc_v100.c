/*
 * prc_v100.c -- prc v100 驱动实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <io.h>
#include <bootup.h>
#include "prc_v100.h"


#define PRC_MAX_TIMEOUT         (10)


#define prc_init_fail()\
do {\
    printf("<%d> : prc init fail\n", __LINE__); \
    return TS_ERR;\
} while (0)

static ts_s32 wait_psram_phy_dll_lock(ts_u32 u32_regbase)
{
    ts_u32 u32_value, u32_tmout;

    u32_tmout = 0;
    u32_value = readl(u32_regbase + PRC_REG_OFF_DLL_LOCK_STATE);
    while ((u32_value & 0x1) != 0x1) {
        u32_tmout++;
        if (u32_tmout > PRC_MAX_TIMEOUT)
            return TS_ERR;

        dly_lib_delay_us(10);
        u32_value = readl(u32_regbase + PRC_REG_OFF_DLL_LOCK_STATE);
    }

    return TS_OK;
}

static ts_u32 psram_reg_readl(ts_u32 u32_regbase, ts_u32 u32_opt_addr)
{
    ts_u32 u32_rd_data, u32_tmout;

    /* mem_reg_ca_l */
    writel(u32_opt_addr, (u32_regbase + PRC_REG_OFF_CA_L));

    /* mem_reg_opt_type */
    u32_rd_data = PRC_OPT_TYPE_RD << 4;
    writel(u32_rd_data, (u32_regbase + PRC_REG_OFF_OPS));

    /* mem_reg_opt_start */
    u32_rd_data |= 0x01;
    writel(u32_rd_data, (u32_regbase + PRC_REG_OFF_OPS));

    /* wait mem_reg_op_done */
    u32_tmout = 0;
    u32_rd_data = readl(u32_regbase + PRC_REG_OFF_OPS);
    while ((u32_rd_data & 0x100) != 0x100 ) {
        u32_tmout++;
        if (u32_tmout > PRC_MAX_TIMEOUT)
            return 0;

        dly_lib_delay_us(1);
        u32_rd_data = readl(u32_regbase + PRC_REG_OFF_OPS);
    }

    /* read */
    u32_rd_data = readl(u32_regbase + PRC_REG_OFF_RDAT);

    return u32_rd_data;
}


static __attribute__((unused)) ts_void psram_reg_writel(ts_u32 u32_regbase, ts_u32 u32_opt_addr, ts_u32 u32_opt_data)
{
    ts_u32 u32_rd_data, u32_tmout;

    /* mem_reg_ca_l */
    writel(u32_opt_addr, (u32_regbase + PRC_REG_OFF_CA_L));

    /* mem_reg_wdat */
    writel(u32_opt_data, (u32_regbase + PRC_REG_OFF_WDAT));

    /* mem_reg_opt_type */
    u32_rd_data = PRC_OPT_TYPE_WR << 4;
    writel(u32_rd_data, (u32_regbase + PRC_REG_OFF_OPS));

    /* mem_reg_opt_start */
    u32_rd_data |= 0x01;
    writel(u32_rd_data, (u32_regbase + PRC_REG_OFF_OPS));

    /* wait mem_reg_op_done */
    u32_tmout = 0;
    u32_rd_data = readl(u32_regbase + PRC_REG_OFF_OPS);
    while ((u32_rd_data & 0x100) != 0x100 ) {
        u32_tmout++;
        if (u32_tmout > PRC_MAX_TIMEOUT)
            break;

        dly_lib_delay_us(1);
        u32_rd_data = readl(u32_regbase + PRC_REG_OFF_OPS);
    }
}


static void psram_wait_150us(void)
{
    dly_lib_delay_us(150);
}


static ts_s32 read_psram_ir_wb(ts_u32 u32_regbase, ts_u32 u32_device_type, ts_u32 u32_size_type){
    ts_u32 u23_rdata;

    writel(0x00000000, (u32_regbase + PRC_REG_OFF_CA_L));
    writel(0x00000000, (u32_regbase + PRC_REG_OFF_CA_H));
    writel(0x00000030, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000031, (u32_regbase + PRC_REG_OFF_OPS));

    while(1){
        u23_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        u23_rdata = u23_rdata & 0x100;
        if(u23_rdata == 0x100)
            break;
    }

    u23_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);
    if(u32_size_type == 0){//32Mbit
        if((u23_rdata & 0xffff) != 0x005f)
            prc_init_fail();
    }

    writel(0x00000001, (u32_regbase + PRC_REG_OFF_CA_L));
    writel(0x00000000, (u32_regbase + PRC_REG_OFF_CA_H));
    writel(0x00000030, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000031, (u32_regbase + PRC_REG_OFF_OPS));

    while(1){
        u23_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u23_rdata & 0x100) == 0x100)
            break;
    }

    u23_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);
    /* 3.1 */
    if(u32_device_type == 2) {
        if((u23_rdata & 0xffff) != 0x000a)
            prc_init_fail();
    } else if(u32_device_type == 1) { //3.0
        if((u23_rdata & 0xffff) != 0x0009)
            prc_init_fail();
    } else if(u32_device_type == 0) { //2.0
        if((u23_rdata & 0xffff) != 0x0001)
            prc_init_fail();
    } else { //1.0
        if((u23_rdata & 0xffff) != 0x000f)
            prc_init_fail();
    }

    return TS_OK;
}

//#define PRC_DS_EN

#ifdef PRC_DS_EN
//ds from 0 to 7, 1:minium drive strength, 7:max drive strength
static ts_s32 setup_psram_ds_wb(ts_u32 u32_regbase, ts_u32 u32_ds, ts_u32 u32_size_type) {
    ts_u32 u32_rdata;

    writel(0x01000000, (u32_regbase + PRC_REG_OFF_CA_L));
    writel(0x00000000, (u32_regbase + PRC_REG_OFF_CA_H));
    writel(0x00000030, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000031, (u32_regbase + PRC_REG_OFF_OPS));

    while(1){
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }
    u32_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);
    if(u32_size_type == 0){
        if((u32_rdata & 0xffff) != 0x8f1f)
            prc_init_fail();
    } else {
        if((u32_rdata & 0xffff) != 0x8f2f)
            prc_init_fail();
    }

    u32_rdata = (u32_rdata & 0xffff8fff) | (u32_ds << 12);
    writel(u32_rdata, (u32_regbase + PRC_REG_OFF_WDAT));

    writel(0x00000020, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000021, (u32_regbase + PRC_REG_OFF_OPS));
    while(1){
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }

    return TS_OK;
}
#endif

static ts_s32 setup_psram_cr_wb(ts_u32 u32_regbase, ts_u32 u32_device_type, ts_u32 u32_size_type) {
    ts_u32 u32_rdata;
    ts_u32 u32_init_latency;
    ts_u32 u32_latency_count;

    writel(0x01000000, (u32_regbase + PRC_REG_OFF_CA_L));
    writel(0x00000000, (u32_regbase + PRC_REG_OFF_CA_H));
    writel(0x00000030, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000031, (u32_regbase + PRC_REG_OFF_OPS));

    while(1){
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }
    u32_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);
    if(u32_size_type == 0){
        if((u32_rdata & 0xffff) != 0x8f1f)
            prc_init_fail();
    } else {
        if((u32_rdata & 0xffff) != 0x8f2f)
            prc_init_fail();
    }
    
    u32_init_latency = 1;
    u32_rdata = (u32_rdata & 0xffffff04) | (u32_init_latency << 4);
    writel(u32_rdata, (u32_regbase + PRC_REG_OFF_WDAT));

    writel(0x00000020, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000021, (u32_regbase + PRC_REG_OFF_OPS));
    while(1){
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }

    u32_latency_count = 6;
    u32_rdata = readl(u32_regbase + 0x001c);
    u32_rdata &= 0xFFFFFF00;
    u32_rdata |= (u32_latency_count << 4) | u32_latency_count;
    writel(u32_rdata, (u32_regbase + 0x001c));

    REG(0x001d0018) = 0x00000030;
    REG(0x001d0018) = 0x00000031;
    while(1){
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }
    u32_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);
    u32_rdata = u32_rdata & 0xf1;
    u32_rdata = u32_rdata >> 3;
    if(u32_rdata != (u32_init_latency << 1))
        prc_init_fail();


    writel(0x01000001, (u32_regbase + PRC_REG_OFF_CA_L));
    writel(0x00, (u32_regbase + PRC_REG_OFF_CA_H));
    writel(0x00000030, (u32_regbase + PRC_REG_OFF_OPS));
    writel(0x00000031, (u32_regbase + PRC_REG_OFF_OPS));

    while(1) {
        u32_rdata = readl(u32_regbase + PRC_REG_OFF_OPS);
        if((u32_rdata & 0x100) == 0x100)
            break;
    }

    u32_rdata = readl(u32_regbase + PRC_REG_OFF_RDAT);

    if(u32_size_type == 0){
        if((u32_rdata & 0xFFA7) != 0x0)
            prc_init_fail();
    } else {
        if((u32_rdata & 0xffc) != 0xfc0)
            prc_init_fail();
    }

    return TS_OK;
}


void setup_seq(ts_u32 u32_regbase, ts_u32 u32_bound_sel){
    writel(0x00000001, (u32_regbase + 0x0030));
    writel(u32_bound_sel, (u32_regbase + 0x0034));
    writel(0x00000000, (u32_regbase + 0x0038));
}



static ts_s32 psram_x16_256Mb_T250_init(ts_u32 u32_regbase, ts_u32 u32_bus_clk)
{
    ts_u32 u32_temp_data;
    ts_u32 phase_start_point = 40;
    int i;

    writel(0xe060a020, (u32_regbase + 0x0004));
    writel(0x00072733, (u32_regbase + 0x0020));
    writel(0x00000000, (u32_regbase + 0x0024));
    writel(0x00000204, (u32_regbase + 0x0100));
    writel(0x00002020, (u32_regbase + 0x0110));
    writel(0x00002020, (u32_regbase + 0x0120));

    if (u32_bus_clk >= 100) {
        for (i = 0; i <= 4; i++) {
            /* reset to 0 */
            writel(0x00000000, (u32_regbase + 0x0104));
            writel((0x00360428 + phase_start_point * i), (u32_regbase + 0x010c));
            writel((0x00360428 + phase_start_point * i), (u32_regbase + 0x011c));
    
            /* reset to 1 */
            writel(0x00000001, (u32_regbase + 0x0104));
    
            wait_psram_phy_dll_lock(u32_regbase);
    
            u32_temp_data = readl(u32_regbase + 0x0114);
            u32_temp_data &= 0xff00;
            u32_temp_data = u32_temp_data >> 8;
            if (u32_temp_data > 20)
                break;
        }
    
        /* tunning set */
        writel(0x04000701, (u32_regbase + 0x0028));
    } else {
        writel(0x00000000, (u32_regbase + 0x0104));
        writel(0x00b60428, (u32_regbase + 0x010c));
        writel(0x00b60428, (u32_regbase + 0x011c));
        writel(0x00000001, (u32_regbase + 0x0104));

        writel(0x04000700, (u32_regbase + 0x0028));
    }

    /* reset */
    writel(0x00000001, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000001, (u32_regbase + 0x012c));

    psram_wait_150us();

    //IR0 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x0); //read reg 0x0
    if((u32_temp_data & 0xffff) != 0x0e76)
        prc_init_fail();

    //IR1 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x1); //read reg 0x1
    if( (u32_temp_data & 0xffff) != 0x0009)
       prc_init_fail();

    //CR0 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x01000000); //read reg 0x01000000
    if((u32_temp_data & 0xffff) != 0x8f2f)
        prc_init_fail();
    
#ifdef PRC_DS_EN
    //set drive strength from 0 to 7, 1=mininum strength, 7=maximum strength
    setup_psram_ds_wb(u32_regbase, 1, 2);
#endif

    //CR1 reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x01000001); //read reg 0x01000001
    if((u32_temp_data & 0xfffc) != 0xffc0)
        prc_init_fail();

    return TS_OK;
}


static ts_s32 psram_x16_128Mb_T250_init(ts_u32 u32_regbase, ts_u32 u32_bus_clk)
{
    ts_u32 u32_temp_data;

    writel(0xe060a020, (u32_regbase + 0x0004));
    writel(0x00000077, (u32_regbase + 0x001c));
    writel(0x02211721, (u32_regbase + 0x0020));
    writel(0x00000000, (u32_regbase + 0x0024));
    writel(0x00000000, (u32_regbase + 0x0104));
    writel(0x00360436, (u32_regbase + 0x010c));
    writel(0x00002020, (u32_regbase + 0x0110));
    writel(0x00000001, (u32_regbase + 0x0104));

    wait_psram_phy_dll_lock(u32_regbase);

    writel(0xc003ffff, (u32_regbase + 0x0140));
    writel(0x00000003, (u32_regbase + 0x012c));
    writel(0x00000002, (u32_regbase + 0x012c));
    writel(0x00000002, (u32_regbase + 0x012c));
    writel(0x00000003, (u32_regbase + 0x012c));

    psram_wait_150us();

    u32_temp_data = psram_reg_readl(u32_regbase, 0x01000001); //read reg 0x01000001
    u32_temp_data &= 0xFFF;
    u32_temp_data |= 0xA000;
    psram_reg_writel(u32_regbase, 0x01000001, u32_temp_data); //write reg 0x01000000
    dly_lib_delay_us(10);

    //IR0 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x0); //read reg 0x0
    if((u32_temp_data & 0xffff) != 0x0c86)
        prc_init_fail();

    //IR1 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x1); //read reg 0x1
    if((u32_temp_data & 0xffff) != 0x000a)
        prc_init_fail();

    //CR0 Reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x01000000); //read reg 0x01000000
    if((u32_temp_data & 0xffff) != 0x8f2f)
        prc_init_fail();

    //CR1 reading
    u32_temp_data = psram_reg_readl(u32_regbase, 0x01000001); //read reg 0x01000001
    if((u32_temp_data & 0xfffc) != 0xffc0)
        prc_init_fail();

    return TS_OK;
}


static ts_s32 psram_x8_32Mb_T200_init(ts_u32 u32_regbase, ts_u32 u32_bus_clk)
{
    ts_u32 u32_temp_data;
    ts_u32 phase_start_point;
    ts_u32 i;

    writel(0x00000020, (u32_regbase + 0x0000));
    writel(0x00000020, (u32_regbase + 0x0000));

    writel(0xe060a020, (u32_regbase + 0x0004));
    writel(0x00000e66, (u32_regbase + 0x001c));
    writel(0x00072733, (u32_regbase + 0x0020));
    writel(0x00000000, (u32_regbase + 0x0024));


    phase_start_point = 40;
    for (i = 1; i <= 4; i++) {
        writel(0x00000000, (u32_regbase + 0x0104));

        u32_temp_data = readl(u32_regbase + 0x010c);
        u32_temp_data = (u32_temp_data & 0xFFFFFF00) | ((phase_start_point * i) & 0xff);
        writel(u32_temp_data, (u32_regbase + 0x010c));

        u32_temp_data = readl(u32_regbase + 0x0110);
        u32_temp_data = u32_temp_data & 0xFFFF8080;
        u32_temp_data = u32_temp_data | (0x20);
        u32_temp_data = u32_temp_data | (0x22 << 8);
        writel(u32_temp_data, (u32_regbase + 0x0110));

        u32_temp_data = readl(u32_regbase + 0x011c);
        u32_temp_data = u32_temp_data & 0xFFFFFF00;
        u32_temp_data = u32_temp_data | ((phase_start_point * i) & 0xFF);
        writel(u32_temp_data, (u32_regbase + 0x011c));

        u32_temp_data = readl(u32_regbase + 0x0120);
        u32_temp_data = u32_temp_data & 0xFFFF8080;
        u32_temp_data = u32_temp_data | (0x20);
        u32_temp_data = u32_temp_data | (0x22 << 8);
        writel(u32_temp_data, (u32_regbase + 0x0120));

        /* reset to 1 */
        writel(0x00000001, (u32_regbase + 0x0104));

        wait_psram_phy_dll_lock(u32_regbase);

        u32_temp_data = readl(u32_regbase + 0x0114);
        u32_temp_data &= 0xff00;
        u32_temp_data = u32_temp_data >> 8;
        if (u32_temp_data > 20)
            break;
    }

    writel(0x04000701, (u32_regbase + 0x0028));
    writel(0x00201111, (u32_regbase + 0x0130));
    writel(0x00200111, (u32_regbase + 0x0134));
    writel(0x00201111, (u32_regbase + 0x0138));
    writel(0x00200111, (u32_regbase + 0x013c));
    writel(0x00200111, (u32_regbase + 0x0140));
    writel(0x00200111, (u32_regbase + 0x0144));
    writel(0x00200111, (u32_regbase + 0x0148));
    writel(0x00200111, (u32_regbase + 0x014c));
    writel(0x00200111, (u32_regbase + 0x0150));
    writel(0x00200111, (u32_regbase + 0x0154));
    writel(0x00200111, (u32_regbase + 0x0158));
    writel(0x00200111, (u32_regbase + 0x015c));
    writel(0x00200111, (u32_regbase + 0x0160));
    writel(0x00200111, (u32_regbase + 0x0164));
    writel(0x00200111, (u32_regbase + 0x0168));
    writel(0x00200111, (u32_regbase + 0x016c));
    writel(0x00200111, (u32_regbase + 0x0170));
    writel(0x00200111, (u32_regbase + 0x0174));
    writel(0x00200111, (u32_regbase + 0x0178));
    writel(0x00200111, (u32_regbase + 0x017c));
    writel(0x00200111, (u32_regbase + 0x0180));

    writel(0x00000204, (u32_regbase + 0x0100));


    /* reset */
    writel(0x00000001, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000000, (u32_regbase + 0x012c));
    writel(0x00000001, (u32_regbase + 0x012c));

    psram_wait_150us();

    read_psram_ir_wb(u32_regbase, 3, 0);

    setup_psram_cr_wb(u32_regbase, 3, 0);

    setup_seq(u32_regbase, 0);

    if(readl(u32_regbase + 0x0058) != 0)
        prc_init_fail();
    if(readl(u32_regbase + 0x0060) != 0)
        prc_init_fail();
    if(readl(u32_regbase + 0x0064) != 0)
        prc_init_fail();

    return TS_OK;
}


ts_s32 hook_ddr_init(ts_u32 u32_bank0_en, ts_u32 u32_bank1_en, ts_u32 u32_volume, ts_u32 u32_bus_clk)
{
    ts_u32 u32_regbase;
    ts_s32 s32_ret;

    if (u32_bank0_en > 0) {
        u32_regbase = soc_get_ddr_bank_regbase(0);
        if (BOOTUP_DDR_VOLUME_16MB == u32_volume)
            s32_ret = psram_x16_128Mb_T250_init(u32_regbase, u32_bus_clk);
        else if (BOOTUP_DDR_VOLUME_4MB == u32_volume)
            s32_ret = psram_x8_32Mb_T200_init(u32_regbase, u32_bus_clk);
        else
            s32_ret = psram_x16_256Mb_T250_init(u32_regbase, u32_bus_clk);
    }

    if (u32_bank1_en > 0) {
        u32_regbase = soc_get_ddr_bank_regbase(1);
        if (BOOTUP_DDR_VOLUME_16MB == u32_volume)
            s32_ret = psram_x16_128Mb_T250_init(u32_regbase, u32_bus_clk);
        else if (BOOTUP_DDR_VOLUME_4MB == u32_volume)
            s32_ret = psram_x8_32Mb_T200_init(u32_regbase, u32_bus_clk);
        else
            s32_ret = psram_x16_256Mb_T250_init(u32_regbase, u32_bus_clk);
    }

    ts_log(TS_LOG_NOTICE, "prc init done\n");

    return s32_ret;
}

