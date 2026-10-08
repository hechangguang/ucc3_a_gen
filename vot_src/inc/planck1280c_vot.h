/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:23:04
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-04-07 16:03:21
 * @FilePath: /ucc3_a_gen/vot_src/inc/planck1280c_vot.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef  __PLANCK1280C_VOT_H__
#define  __PLANCK1280C_VOT_H__

#include <base/ts_type.h>
#include <stdio.h>
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_time.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
#include <uapi/upl_v100.h>
#include <math.h>
#include <menu/usermenu.h>
ts_s32 planck1280c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value);
ts_u16 planck1280c_get_tes(ts_void);
ts_void vot_planck1280c_write_config_256bit(ts_u08* config_256bit);
//光智1280探测器Det参数256bit写
ts_void planck1280c_write_gain_adc(ts_u32 u32_gain_adc);
ts_void planck1280c_write_gof(ts_u32 u32_gof);
ts_void planck1280c_write_cint(ts_u32 u32_cint);
ts_void planck1280c_write_Nint(ts_u32 u32_Nint);
ts_void planck1280c_write_Sel_PJ(ts_u32 u32_Sel_PJ);
ts_void planck1280c_write_NTCC(ts_u32 u32_NTCC);
ts_void planck1280c_write_Voccm(ts_u32 u32_Voccm);
ts_void planck1280c_write_detConfig(ts_u32 u32_detConfig);
ts_void planck1280c_write_Vfid(ts_u32 u32_Vfid);
//光智1280探测器Det参数256bit读
ts_u32 planck1280c_read_gain_adc(ts_void);
ts_u32 planck1280c_read_gof(ts_void);
ts_u32 planck1280c_read_Cint(ts_void);
ts_u32 planck1280c_read_Nint(ts_void);
ts_u32 planck1280c_read_Sel_PJ(ts_void);
ts_u32 planck1280c_read_NTCC(ts_void);
ts_u32 planck1280c_read_Voccm(ts_void);
ts_u32 planck1280c_read_detConfig(ts_void);
ts_u32 planck1280c_read_Vfid(ts_void);

ts_u32 vot1280_detetor_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb);
ts_void vot1280_detetor_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata);
ts_void brd_planck1280_vot_init(ts_void);


#endif