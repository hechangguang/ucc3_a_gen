/*
 * bootup.c -- bootup 打包头，根据 make menuconfig 配置修改，不建议手工修改。
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

#include <bootup.h>

bootup_ih_s_t sst_image_head = {

    .u32_ih_magic = CONFIG_IH_MAGIC,
    .u32_ih_hcrc = 0,
    .u32_ih_time = 0,
    .u32_ih_size = 0,
    .u32_ih_load = CONFIG_IH_LOAD,
    .u32_ih_ep = CONFIG_IH_EP,
    .u32_ih_dcrc = 0,

    #if defined(CONFIG_IH_VERIFY_SUM8)
        .u08_ih_verify_type = BOOTUP_VERIFY_SUM8,
    #elif defined(CONFIG_IH_VERIFY_SUM32)
        .u08_ih_verify_type = BOOTUP_VERIFY_SUM32,
    #elif defined(CONFIG_IH_VERIFY_CRC8)
        .u08_ih_verify_type = BOOTUP_VERIFY_CRC8,
    #elif defined(CONFIG_IH_VERIFY_SIGN)
        .u08_ih_verify_type = BOOTUP_VERIFY_SIGN,
    #else
        .u08_ih_verify_type = BOOTUP_VERIFY_CRC32,
    #endif

    #ifdef CONFIG_IH_IGNORE_VERIFY
        .u08_ignore_verify = 1,
    #else
        .u08_ignore_verify = 0,
    #endif

    #ifdef CONFIG_IH_SIGN_EN
        .u08_ih_sign_en = 1,
    #else
        .u08_ih_sign_en = 0,
    #endif

    #if defined(CONFIG_IH_COMP_LZO)
        .u08_ih_comp = BOOTUP_COMPRESS_LZO,
    #elif defined(CONFIG_IH_COMP_RLE)
        .u08_ih_comp = BOOTUP_COMPRESS_RLE,
    #elif defined(CONFIG_IH_COMP_FSE)
        .u08_ih_comp = BOOTUP_COMPRESS_FSE,
    #else
        .u08_ih_comp = BOOTUP_COMPRESS_NONE,
    #endif

    #if defined(CONFIG_LOG_LEVEL_EMERG)
        .u08_ih_debug_level = TS_LOG_EMERG,
    #elif defined(CONFIG_LOG_LEVEL_ALERT)
        .u08_ih_debug_level = TS_LOG_ALERT,
    #elif defined(CONFIG_LOG_LEVEL_CRIT)
        .u08_ih_debug_level = TS_LOG_CRIT,
    #elif defined(CONFIG_LOG_LEVEL_ERROR)
        .u08_ih_debug_level = TS_LOG_ERR,
    #elif defined(CONFIG_LOG_LEVEL_NOTICE)
        .u08_ih_debug_level = TS_LOG_NOTICE,
    #elif defined(CONFIG_LOG_LEVEL_INFO)
        .u08_ih_debug_level = TS_LOG_INFO,
    #elif defined(CONFIG_LOG_LEVEL_DEBUG)
        .u08_ih_debug_level = TS_LOG_DEBUG,
    #else
        .u08_ih_debug_level = TS_LOG_WARNING,
    #endif

    .u08_ih_bk_cnt = CONFIG_IH_BK_COUNT,
    .u08_ih_img_bk_cnt = CONFIG_IH_IMAGE_BK_COUNT,
    .u08_rev = 0,

    .u32_ih_bk_offset = CONFIG_IH_BK_OFFSET,
    .u32_ih_img_bk_offset = CONFIG_IH_IMAGE_BK_OFFSET,

    .u32_ih_img_load_addr = CONFIG_IH_IMG_LOAD_ADDR,
    .u32_ih_img_ep_addr = CONFIG_IH_IMG_EP_ADDR,
    .u32_ih_img_size = CONFIG_IH_IMG_SIZE,
    .u32_ih_img_crc = 0,

    .u32_rev = 0,
};


