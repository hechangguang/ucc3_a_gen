/* 
 * adc.h -- OPEN TDK 对 adc 驱动的封装，上移到用户层实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_compiler.h>

/* 内部寄存器 */
typedef union adc_ctl_un {
    struct {
        ts_u32 power_down:1;        //[00..00]
        ts_u32 mode_sel:1;          //[01..01]
        ts_u32 pd_force_high:1;     //[02..02]
        ts_u32 reset:1;             //[03..03]
        ts_u32 p2s_en:1;            //[04..04]
        ts_u32 highf:1;             //[05..05]
        ts_u32 rev06_31:26;         //[06..31]
    };
    ts_u32 u32_val;
} adc_ctl_un_t;

typedef union adc_baudr_un {
    struct {
        ts_u32 clk_baudr:16;        //[00..15]
        ts_u32 rev16_31:16;         //[16..31]
    };
    ts_u32 u32_val;
} adc_baudr_un_t;

typedef union adc_pd_seoc_time_un {
    struct {
        ts_u32 pd_soc_time:16;      //[00..15]
        ts_u32 soc_pd_time:16;      //[16..31]
    };
    ts_u32 u32_val;
} adc_pd_seoc_time_un_t;


typedef union adc_soc_width_un {
    struct {
        ts_u32 adc_soc_width:7;     //[00..06]
        ts_u32 rev08_31:25;         //[07..31]
    };
    ts_u32 u32_val;
} adc_soc_width_un_t;

typedef union adc_scan_time_un {
    struct {
        ts_u32 scan_time:7;     //[00..31]
    };
    ts_u32 u32_val;
} adc_scan_time_un_t;

typedef union adc_ch_vld_un {
    struct {
        ts_u32 ch0_vld:1;       //[00..00]
        ts_u32 ch1_vld:1;       //[01..01]
        ts_u32 ch2_vld:1;       //[02..02]
        ts_u32 ch3_vld:1;       //[03..03]
        ts_u32 rev04_31:28;     //[04..31]
    };
    ts_u32 u32_val;
} adc_ch_vld_un_t;

typedef union adc_intr_en_un {
    struct {
        ts_u32 intr_ch0_en:1;       //[00..00]
        ts_u32 intr_ch1_en:1;       //[01..01]
        ts_u32 intr_ch2_en:1;       //[02..02]
        ts_u32 intr_ch3_en:1;       //[03..03]
        ts_u32 rev04_31:28;         //[04..31]
    };
    ts_u32 u32_val;
} adc_intr_en_un_t;

typedef union adc_intr_cfg_un {
    struct {
        ts_u32 adc_intr_delta:12;   //[00..11]
        ts_u32 rev12_15:4;          //[12..15]
        ts_u32 adc_intr_thr:12;     //[16..27]
        ts_u32 rev28_31:4;          //[28..31]
    };
    ts_u32 u32_val;
} adc_intr_cfg_un_t;

typedef union adc_intr_sta_un {
    struct {
        ts_u32 intr_ch0:1;       //[00..00]
        ts_u32 intr_ch1:1;       //[01..01]
        ts_u32 intr_ch2:1;       //[02..02]
        ts_u32 intr_ch3:1;       //[03..03]
        ts_u32 rev04_31:28;     //[04..31]
    };
    ts_u32 u32_val;
} adc_intr_sta_un_t;

typedef union adc_ctrl_start_un {
    struct {
        ts_u32 start:1;       //[00..00]
        ts_u32 rev01_31:31;     //[01..31]
    };
    ts_u32 u32_val;
} adc_ctrl_start_un_t;


typedef union adc_ctrl_stop_un {
    struct {
        ts_u32 stop:1;       //[00..00]
        ts_u32 rev01_31:31;     //[01..31]
    };
    ts_u32 u32_val;
} adc_ctrl_stop_un_t;


typedef union adc_data_un {
    struct {
        ts_u32 ch0_2:12;   //[00..11]
        ts_u32 rev12_15:4;          //[12..15]
        ts_u32 ch1_3:12;     //[16..27]
        ts_u32 rev28_31:4;          //[28..31]
    };
    ts_u32 u32_val;
} adc_data_un_t;

typedef union adc_ctrl_state_un {
    struct {
        ts_u32 busy:1;              //[00..00]
        ts_u32 eoc_err:1;           //[01..01]
        ts_u32 rev2:1;              //[02..02]
        ts_u32 rev3:1;              //[03..03]
        ts_u32 sample_fsm_state:4;  //[04..07]
        ts_u32 rev08_31:24;         //[08..31]
    };
    ts_u32 u32_val;
} adc_ctrl_state_un_t;

/* adc 寄存器组 */
typedef struct adc_reg_s {
    __IOM adc_ctl_un_t un_work_ctrl;                    /* 0x0000 */
    __IOM adc_baudr_un_t un_baudr;                      /* 0x0004 */
    __IOM adc_pd_seoc_time_un_t un_pd_seoc_time;        /* 0x0008 */
    __IOM adc_soc_width_un_t un_soc_width;              /* 0x000C */

    __IOM adc_scan_time_un_t un_scan_time;              /* 0x0010 */
    __IOM adc_ch_vld_un_t un_ch_vld;                    /* 0x0014 */
    __IOM adc_intr_en_un_t un_intr_en;                  /* 0x0018 */
    __IOM adc_intr_cfg_un_t un_intr_cfg;                /* 0x001C */

    __IOM adc_intr_sta_un_t un_intr_sta;                /* 0x0020 */
    __IOM adc_ctrl_start_un_t un_ctrl_start;            /* 0x0024 */
    __IOM adc_ctrl_stop_un_t un_ctrl_stop;              /* 0x0028 */
    __IOM adc_data_un_t un_data10_un_t;                 /* 0x002C */
    __IOM adc_data_un_t un_data32_un_t;                 /* 0x0030 */

    __IOM ts_u32 au32_rev2[2];                          /* 0x0034-0x0038 */

    __IOM adc_ctrl_state_un_t un_ctrl_state;            /* 0x003C */
} adc_reg_s_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __ADC_H__ */
