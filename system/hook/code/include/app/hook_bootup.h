/*
 * hook_bootup.h -- 应用镜像加载启动 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __HOOK_BOOTUP_H__
#define __HOOK_BOOTUP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <config.h>
#include <base/ts_type.h>
#include <app/boot_param.h>
#include <bootup.h>

/* 共享内存地址和大小 hook与应用镜像通过共享内存交互 */
#define HOOK_APP_SHM_ADDR               (0x2207FBFF)
#define HOOK_APP_SHM_SIZE               (0x400)

/* 双镜像地址 */
#define A_IMG_ADDRESS                   (0x00080000)
#define B_IMG_ADDRESS                   (0x00180000)

/* 应用镜像最大坏块数 */
#define APP_IMG_MAX_BAD_CNT             (2)

/* 应用镜像最大使用块数 */
#define IMG_SAVE_BLK_MAX_CNT            (5)

/* 加载应用镜像上下文 */
typedef struct boot_ctx_s {
    /* 启动参数 */
    bt_param_s_t st_bt_param;

    /* 启动参数是否更新 */
    ts_u32 u32_param_upd_flg;

    /* 镜像入口地址 */
    ts_u32 u32_ep_addr;

    /* 镜像头信息 */
    uboot_ih_s_t st_ih;
}boot_ctx_s_t;


ts_void hook_boot_app(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HOOK_BOOTUP_H__ */

