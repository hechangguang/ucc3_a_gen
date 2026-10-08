/*
 * acc.c -- tdk dsp 加速库,部分纯 C 实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>

typedef union matrix_sub_un {
    struct {
        ts_u32 lo16:16;     //[00..15]
        ts_u32 hi16:16;     //[16..31]
    };
    ts_u32 u32_val;
} matrix_sub_un_t;

/* 必须4对齐 */
static ts_void ts_tdk_acc_add8(ts_u08 *pu08_matrix_a, ts_u08 *pu08_matrix_b, ts_u08 *pu08_matrix_d, ts_u32 u32_width, ts_u32 u32_height)
{
    ts_u32 i, j, u32_cnt, u32_matrix_a, u32_matrix_b, u32_matrix_d;
    ts_u32 u32_addr;
    ts_u32 *pu32_matrix_a, *pu32_matrix_b, *pu32_matrix_d;

    pu32_matrix_a = (ts_u32 *)pu08_matrix_a;
    pu32_matrix_b = (ts_u32 *)pu08_matrix_b;
    pu32_matrix_d = (ts_u32 *)pu08_matrix_d;
    if ((ts_null == pu32_matrix_a) || (ts_null == pu32_matrix_b) || (ts_null == pu32_matrix_d))
        return 0;

    u32_cnt = u32_width >> 2;
    for (i = 0; i < u32_height; i++) {
        for (j = 0; j < u32_cnt; j++) {
            u32_matrix_a = *pu32_matrix_a++;
            u32_matrix_b = *pu32_matrix_b++;

            asm volatile("add8 %0, %1, %2" 
                : "=r"(u32_matrix_d)
                : "r"(u32_matrix_a), "r"(u32_matrix_b)
            );

            *pu32_matrix_d++ = u32_matrix_d;
        }
    }
}

/* 个数能被 8 整除 */
void ts_tdk_acc_matrix_sub_32bit(ts_s32 *result, const ts_u16 *a, const ts_u16 *b, ts_u32 u32_num)
{
    ts_u32 i, j, u32_cnt;
    matrix_sub_un_t *pun_a_src;
    matrix_sub_un_t *pun_b_src;

    pun_a_src = (matrix_sub_un_t *)a;
    pun_b_src = (matrix_sub_un_t *)b;
    u32_cnt = u32_num >> 3;
    j = 0;
    for (i = 0; i < u32_cnt; i++) {
        result[j++] = pun_a_src->lo16 - pun_b_src->lo16;
        result[j++] = pun_a_src->hi16 - pun_b_src->hi16;
        pun_a_src++;
        pun_b_src++;

        result[j++] = pun_a_src->lo16 - pun_b_src->lo16;
        result[j++] = pun_a_src->hi16 - pun_b_src->hi16;
        pun_a_src++;
        pun_b_src++;

        result[j++] = pun_a_src->lo16 - pun_b_src->lo16;
        result[j++] = pun_a_src->hi16 - pun_b_src->hi16;
        pun_a_src++;
        pun_b_src++;

        result[j++] = pun_a_src->lo16 - pun_b_src->lo16;
        result[j++] = pun_a_src->hi16 - pun_b_src->hi16;
        pun_a_src++;
        pun_b_src++;
    }
}

void ts_tdk_acc_matrix_add_32bit_t_32bit(ts_s32 *result, const ts_s32 *a, const ts_s32 *b, ts_u32 u32_num)
{
    ts_s32 *ps32_ret, *ps32_a, *ps32_b;
    ts_u32 i, u32_cnt;

    u32_cnt = (ts_u32)u32_num;
    u32_cnt = u32_cnt >> 2;

    ps32_ret = result;
    ps32_a = a;
    ps32_b = b;
    for (i = 0; i < u32_cnt; i++) {
        *ps32_ret++ = *ps32_a++ + *ps32_b++;
        *ps32_ret++ = *ps32_a++ + *ps32_b++;
        *ps32_ret++ = *ps32_a++ + *ps32_b++;
        *ps32_ret++ = *ps32_a++ + *ps32_b++;
    }
}


ts_void ts_tdk_acc_Bprocess_Add(ts_u16 *result, ts_u16 *bbuf, ts_u16 *b, ts_u32 u32_num)
{
    ts_u32 i, u32_cnt, u32_res, u32_bbuf, u32_b;
    ts_u32 *pu32_result, *pu32_bbuf, *pu32_b;

    pu32_result = (ts_u32 *)result;
    pu32_bbuf = (ts_u32 *)bbuf;
    pu32_b = (ts_u32 *)b;

    u32_cnt = u32_num >> 1;
    for (i = 0; i < u32_cnt; i++) {
        u32_bbuf = *pu32_bbuf++;
        u32_b = *pu32_b++;

        asm volatile("add16 %0, %1, %2" 
            : "=r"(u32_res)
            : "r"(u32_bbuf), "r"(u32_b)
        );

        *pu32_result++ = u32_res;
    }
}
