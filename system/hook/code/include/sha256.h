/*
 * sha256.c -- sha256 头文件，移植自 u-boot
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __SHA256_H__
#define __SHA256_H__

#include <base/ts_type.h>

#define SHA256_SUM_LEN    32

/* Reset watchdog each time we process this many bytes */
#define CHUNKSZ_SHA256    (64 * 1024)

typedef struct {
    ts_u32 total[2];
    ts_u32 state[8];
    ts_u08 buffer[64];
} sha256_context;

void sha256_starts(sha256_context * ctx);
void sha256_update(sha256_context *ctx, const ts_u08 *input, ts_u32 length);
void sha256_finish(sha256_context * ctx, ts_u08 digest[SHA256_SUM_LEN]);
void sha256_csum_wd(const ts_u08 *input, ts_u32 ilen, ts_u08 *output, ts_u32 chunk_sz);

#endif /* __SHA256_H_ */
