/*
 * bootup_sign.h -- bootup 安全相关头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __BOOTUP_SIGN_H__
#define __BOOTUP_SIGN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_bitop.h>

/* sign packet */
#define TS_BOOTUP_SIGN_HOOK             (0)
#define TS_BOOTUP_SIGN_INF              (1)

#define TS_BOOTUP_RSA_KEY_LEN           (520)
#define TS_BOOTUP_SIGN_PACKET_SIZE      (776)
#define TS_CRC_INIT_VALUE               (0)
#define TS_KEY_NAME_MAX_LENGTH          (32)
#define TS_SIGN_MAX_SIZE                (512)

#define TS_BOOTUP_RSA_BITS              (2048)
#define TS_BOOTUP_RSA_BYTES             (TS_BOOTUP_RSA_BITS / 8)
#define TS_BOOTUP_RSA_DWORDS            (TS_BOOTUP_RSA_BITS / 32)

typedef struct ts_bootup_sign_s {
    ts_u32 u32_ras_len;
    ts_u32 u32_rsa_n0inv;
    ts_u32 au32_rsa_modules[64];
    ts_u32 au32_rsa_rr[64];

    ts_u08 au08_sign[256];
} ts_bootup_sign_s_t;


/* 用于网络签名 */
typedef struct ts_sign_packet_s {
    /* 包校验，用于保证传输正确 */
    ts_u32 u32_packet_crc;

    /* 使用的 key 名字，用于多 key 场合 */
    ts_char ac_key_name[TS_KEY_NAME_MAX_LENGTH];

    ts_u32 u32_hash_size;

    ts_u32 u32_sign_size;

    /* 无需传输文件，只需要把文件的 hash 传过去，对 hash 签名 */
    ts_u08 au08_hash[32];

    /* 签名， 最终用于校验 */
    ts_bootup_sign_s_t st_sign;
} ts_sign_packet_s_t;


/* 包含安全相关接口 */
#include <bootup_sign_api.h>

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __BOOTUP_SIGN_H__ */
