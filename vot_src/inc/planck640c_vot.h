/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:23:04
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-18 14:20:16
 * @FilePath: /ucc3_a_gen/vot_src/inc/planck640c_vot.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef  __PLANCK640C_VOT_H__
#define  __PLANCK640C_VOT_H__

#include <base/ts_type.h>
#include <stdio.h>
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_time.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
#include <uapi/upl_v100.h>
#include <math.h>
#include <menu/usermenu.h>

ts_s32 planck640c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value);
ts_s32 planck384c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value);
ts_u16 planck640c_get_tes(ts_void);
ts_u16 planck384c_get_tes(ts_void);
ts_void planck640c_read_config_408bit(ts_u08* config_408bit, ts_u08 DataLength);
ts_void planck640c_write_config_408bit(ts_u08* config_408bit);
ts_void vot_planck640c_write_config_408bit(ts_u08* config_408bit);
ts_void planck640c_stop(ts_void);

ts_u32 planck640c_read_extdata(ts_u32 u32_msb,ts_u32 u32_lsb);
ts_u32 planck640c_read_gain_adc(ts_void);
ts_u32 planck640c_read_gos(ts_void);
ts_u32 planck640c_read_cint(ts_void);
ts_u32 planck640c_read_rst_data(ts_void);
ts_u32 planck640c_read_tint(ts_void);
ts_u32 planck640c_read_ndac(ts_void);
ts_u32 planck640c_read_gof(ts_void);
ts_u32 planck640c_read_gain_protect_flag(ts_void);

ts_void planck640c_write_extdata(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_gsp);
ts_void planck640c_write_gof(ts_u32 u32_gof);
ts_void planck640c_write_ndac(ts_u32 u32_ndac);
ts_void planck640c_write_gos(ts_u32 u32_gos);
ts_void planck640c_write_cint(ts_u32 u32_cint);
ts_void planck640c_write_rst_data(ts_u32 u32_rst_data);
ts_void planck640c_write_tint(ts_u32 u32_tint);
ts_void planck640c_write_gain_adc(ts_u32 u32_gain_adc);
ts_void planck640c_write_gain_protect_flag(ts_u32 u32_flag);

ts_u32 vot_detetor_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb);
ts_void vot_detetor_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata);

ts_void brd_planck640c_init_org(ts_void);
ts_void brd_planck640_vot_init(ts_void);
ts_void bt656_bt1120_pin_init(void);
ts_void brd_gpio_mux_vot_init(ts_void);


ts_s32 planck640c_vpo_freeze(ts_void);
ts_s32 planck640c_vpo_unfreeze(ts_void);

dector_type_t get_dector_name_type(void);
board_type_t get_board_port_type(void);
project_name_type_t get_project_name_type(void);
dector_resolution_type_t get_dector_resolution_type(void);
ts_u08 get_customer_type(void);
ts_u08 is_dector_type_manual(void);
ts_u08 is_hardware_port_type_manual(void);
ts_u08 is_customer_type_manual(void);
ts_u08 get_focus_type(void);
ts_u08 is_focus_type_manual(void);
ts_u08 get_function_type(void);
ts_u08 is_function_type_manual(void);
ts_u08 get_powerup_dector_resolution_type(void);
ts_void set_powerup_dector_resolution_type(void);
ts_u08 get_powerup_dector_name_type(void);
ts_void set_powerup_dector_name_type(void);
ts_u08 get_powerup_board_port_type(void);
ts_void set_powerup_board_port_type(void);
ts_u08 get_powerup_temp_drift_comp_mode(void);
ts_void set_powerup_temp_drift_comp_mode(ts_u08 tdc_mode);

ts_void show_dector_resolution_type(void);
ts_void show_dector_name_type(void) ;


#endif