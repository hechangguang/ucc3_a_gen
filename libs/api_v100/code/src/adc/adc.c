/* 
 * adc.c -- OPEN TDK 对 adc 驱动的封装，上移到用户层实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/adc_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/iomux.h>
#include "adc.h"

#define ADC_MAX_VAL     (0xFFF)

typedef struct adc_ctx_s {
    adc_reg_s_t *pst_reg;

    /* 参考电压 */
    ts_u32 u32_refv;

    /* 多路互斥 */
    ts_cpl_mutex_s_t st_mutex;
} adc_ctx_s_t;

static adc_ctx_s_t sst_ctx = {0};

#define libgpio_chk_init()\
do{\
    if (ts_null == sst_ctx.pst_reg) {\
        ts_uapi_log_err("%s\n", TS_ENOINIT_STR);\
        return -TS_ENOINIT;\
    }\
} while(0)


/* -1 为超时 */
static ts_s32 adc_get_sample(ts_u32 u32_ch_num)
{
    ts_u32 i;
    volatile ts_u32 u32_val;
    volatile adc_data_un_t un_adc_data;

    /* 首先停止 */
    sst_ctx.pst_reg->un_ctrl_stop.u32_val = 0x01;

    /* 等待完成 */
    i = 0;
    while (1) {
        if (0 == sst_ctx.pst_reg->un_ctrl_stop.stop)
            break;

        i++;
        if (i > 200)
            break;
    }

    /* 配置 ADC 采样通道 */
    sst_ctx.pst_reg->un_ch_vld.u32_val = 1 << u32_ch_num;

    /* 启动采样 */
    sst_ctx.pst_reg->un_ctrl_start.u32_val = 0x01;

    /* 硬件上会延迟采样，丢弃前面 99 个 */
    for (i = 0; i < 100; i++) {
        if (u32_ch_num >  1) {
            un_adc_data.u32_val = sst_ctx.pst_reg->un_data32_un_t.u32_val;
            if (2 == u32_ch_num)
                u32_val = un_adc_data.ch0_2;
            else
                u32_val =  un_adc_data.ch1_3;
        } else {
            un_adc_data.u32_val = sst_ctx.pst_reg->un_data10_un_t.u32_val;
            if (0 == u32_ch_num)
                u32_val = un_adc_data.ch0_2;
            else
                u32_val = un_adc_data.ch1_3;
        }
    }

    //ts_cpl_printk("v=%x\n", u32_val);

    if (sst_ctx.u32_refv > 0)
        u32_val = (u32_val * sst_ctx.u32_refv) / ADC_MAX_VAL;

    /* 节省功耗，停止 */
    sst_ctx.pst_reg->un_ctrl_stop.u32_val = 0x01;

    return u32_val;
}


ts_s32 ts_lib_adc_read(ts_u32 u32_ch_num, ts_u32 u32_sample_cnt, ts_u32 u32_dly_ms)
{
    ts_u32 i, u32_val, u32_cnt;
    ts_s32 s32_ret;

    libgpio_chk_init();

    if (u32_ch_num >= TDK_ADC_MAX_NUM) {
        ts_uapi_log_err("%s\n", TS_EINVALCHNID_STR);

        return -TS_EINVALCHNID;
    }

    if (u32_sample_cnt > 1)
        u32_cnt = u32_sample_cnt;
    else
        u32_cnt = 1;

    u32_val = 0;

    ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_ctx.st_mutex);

    for (i = 0; i < u32_cnt; i++) {
        s32_ret = adc_get_sample(u32_ch_num);
        if (s32_ret >= 0) {
            u32_val += (ts_u32)s32_ret;
        } else {
            break;
        }

        if (u32_dly_ms > 0)
            ts_cpl_msleep(u32_dly_ms);
    }

    if (u32_cnt > 1)
        u32_val = u32_val / u32_cnt;

    ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_ctx.st_mutex);

    if (s32_ret >= 0)
        return u32_val;
    else
        return TS_ERR;
}



ts_s32 ts_lib_adc_deinit(ts_void)
{
    if (sst_ctx.pst_reg != ts_null) {
        sst_ctx.pst_reg->un_ctrl_stop.u32_val = 0x01;

        sst_ctx.pst_reg = ts_null;

        ts_cpl_mutex_destory((ts_cpl_mutex_s_t *)&sst_ctx.st_mutex);
    }

    return TS_OK;
}


ts_s32 ts_lib_adc_init(ts_u32 u32_sample_clk, ts_u32 u32_refv)
{
    ts_u32 u32_div;

    if (ts_null == sst_ctx.pst_reg) {
        /* ADC 通道仅能整组配置复用关系 */
        ts_lib_gpio_set_mux(TS_IO_ADC_CH, TS_IO_ADC_CH_FUN0);

        sst_ctx.u32_refv = u32_refv;

        /* FIXME: 从驱动获取 */
        sst_ctx.pst_reg = (adc_reg_s_t *)0x00170000;

        /* 停止 ADC */
        sst_ctx.pst_reg->un_ctrl_stop.u32_val = 0x01;

        /* 选择连续 */
        sst_ctx.pst_reg->un_work_ctrl.u32_val = 0x02;

        /* 只能偶数分频 */
        u32_div = 24000000 / u32_sample_clk;
        if (u32_div > 2) {
            if ((u32_div & 0x01) > 0)
                u32_div++;
        } else {
            u32_div = 2;
        }

        sst_ctx.pst_reg->un_baudr.u32_val = u32_div;

        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ctx.st_mutex);
    }

    return TS_OK;
}
