/*
 * hook_bootup.c -- 应用镜像引导升级
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/
#include <app/boot_param.h>
#include <bootup.h>
#include <ts_string.h>
#include <base/ts_type.h>
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <app/snand_flash.h>
#include <bootup_sign_api.h>
#include <app/hook_bootup.h>
#include <io.h>


typedef ts_void (*pfn_next_t)(ts_void);

static boot_ctx_s_t sst_bt_ctx;


static ts_s32 try_load(ts_s32 s32_type)
{
    ts_u32 u32_try_cnt;
    bt_param_s_t *pst_param;
    ts_u08 *pu08_buf;
    ts_u32 u32_verify_crc;
    ts_u32 u32_rd_addr, u32_rd_size, u32_rd_cnt;
    ts_u32 u32_ep_addr;
    uboot_ih_s_t *pst_ih;
    ts_u32 (*flash_read)(ts_u32, ts_u08 *, ts_u32);

    u32_try_cnt = 0;
    pst_param = (bt_param_s_t *)&sst_bt_ctx.st_bt_param;
    pst_ih = (uboot_ih_s_t *)&sst_bt_ctx.st_ih;

    if (BOOTUP_DEVTYPE_SPI_NOR ==  s32_type)
        flash_read = lib_sfc_nor_read;
    else if (BOOTUP_DEVTYPE_SPI_NAND == s32_type)
        flash_read = hook_snand_read;
    else
        return TS_ERR;

    while(1) {
        if (u32_try_cnt > 1)
            return TS_ERR;

        u32_try_cnt += 1;

        if (A_IMG_INDEX == pst_param->u32_active_sys)
            u32_rd_addr = A_IMG_ADDRESS;
        else
            u32_rd_addr = B_IMG_ADDRESS;

        ts_log(TS_LOG_INFO,"try boot from flash(%x)\n", u32_rd_addr);
        /* 读取镜像头信息 */
        pu08_buf = (ts_u08 *)pst_ih;
        u32_rd_size = sizeof (uboot_ih_s_t);
        u32_rd_cnt = flash_read(u32_rd_addr, (ts_u08 *)pu08_buf, u32_rd_size);
        if (u32_rd_cnt != u32_rd_size)
            goto _err_deal;

        /* 头部校验值需要将magic 和 hcrc跳过 */
        pu08_buf += 8;
        u32_rd_size -= 8;

        u32_verify_crc = verify_lib_calc_crc32(0x00, pu08_buf, u32_rd_size);
        if (pst_ih->u32_ih_hcrc != u32_verify_crc){
            ts_log(TS_LOG_INFO,"first verify_lib_calc_crc32 u32_verify_crc (%x) pst_ih->u32_ih_dcrc  (%x) \n", u32_verify_crc, pst_ih->u32_ih_dcrc);
            goto _err_deal;
        }

        /* 应用镜像读取到内存 */
        u32_ep_addr = pst_ih->u32_ih_ep;

        ts_log(TS_LOG_INFO,"try boot from flash(%x) to ram(%x)\n", u32_rd_addr, u32_ep_addr);

        /* 读取地址偏移，跳过img头部信息 */
        u32_rd_addr += sizeof (uboot_ih_s_t);
        pu08_buf = (ts_u08 *)u32_ep_addr;
        u32_rd_size = pst_ih->u32_ih_size;
        u32_rd_cnt = flash_read(u32_rd_addr, pu08_buf, u32_rd_size);
        if (u32_rd_cnt != u32_rd_size){
            ts_log(TS_LOG_INFO," flash_read u32_rd_cnt(%x)\n", u32_rd_cnt);
            goto _err_deal;
        }

        /* 进行校验值计算 */
        u32_verify_crc = verify_lib_calc_crc32(0x00, pu08_buf, u32_rd_size);
        if (pst_ih->u32_ih_dcrc != u32_verify_crc){
            ts_log(TS_LOG_INFO," sencond erify_lib_calc_crc32 u32_verify_crc (%x) pst_ih->u32_ih_dcrc  (%x) \n", u32_verify_crc, pst_ih->u32_ih_dcrc);
            goto _err_deal;
        }

        sst_bt_ctx.u32_ep_addr = u32_ep_addr;

        break;
_err_deal:
        if (A_IMG_INDEX == pst_param->u32_active_sys)
            pst_param->u32_active_sys = B_IMG_INDEX;
        else
            pst_param->u32_active_sys = A_IMG_INDEX;

        continue;
    }

    return TS_OK;
}


ts_void hook_boot_app(ts_void)
{
    bt_param_s_t *pst_param;
    ts_s32 s32_ret;
    pfn_next_t fp_next;
    ts_s32 s32_type;

    pst_param = (bt_param_s_t *)&sst_bt_ctx.st_bt_param;

    /* 启动参数赋值 */
    pst_param->u32_magic = BOOT_PARAM_MAGIC;
    pst_param->u32_active_sys = A_IMG_INDEX;
    pst_param->u32_a_state = 1;
    pst_param->u32_b_state = 1;
    pst_param->u32_upgr_flg = 0;

    s32_type = lib_sfc_scan();

    /* 向 uimg 传递 FLASH 类型,使用用户寄存器 0x00160220 */
    writel(s32_type, 0x00160220);


    /* 尝试启动 */
    s32_ret = try_load(s32_type);
    if (s32_ret != TS_OK) {
        ts_log(TS_LOG_ERR,"%s:img can't bootup\n", __FUNCTION__);
        while(1);
    }

    /* 进行跳转至应用镜像 */
    fp_next = (pfn_next_t)(sst_bt_ctx.u32_ep_addr);
    fp_next();

    while(1);
}
