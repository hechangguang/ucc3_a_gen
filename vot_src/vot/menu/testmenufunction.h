/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:01:33
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-29 16:48:14
 * @FilePath: /ucc3_a_gen/vot_src/vot/menu/testmenufunction.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef  __TEST_MENU_FUNCTION_H__
#define __TEST_MENU_FUNCTION_H__
#include <base/ts_type.h>
#include <stdio.h>
#include"usermenu.h"
#include"testmenu.h"
#include"votmenu.h"
#include"display.h"
#include"name.h"
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_time.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
#include <uapi/upl_v100.h>
#include <math.h>
#include <menu/votmain.h>
#include <menu/usermenu.h>
#include <menu/ispfunction.h>
#include <vot_config.h>
#include <common/TempCal.h>

ts_void cursor_set_position(ts_nodebuf_s_t *pst_osd_node, ts_u16 pos_x, ts_u16 pox_y);

void EnterOrExitBPCMaualModeAuto(void);
void EnterOrExitBPCMaualMode(ts_u08 exit_flag);
void Exit_BPC_Maual_mode(void);
void GetAndDisplayBPC_Maual(void);
void BPC_Maual_mode_move_cursor(ts_u08 pos_x_status, ts_u08 pos_y_status);
void BPC_Maual_mode_set_or_unset_bpc_with_xy(ts_u16 pos_x, ts_u16 pos_y);

void AutomaticDetParameterConfiguration(void);
void AutomaticOccParameterConfiguration(void);

void sffc_Init(ts_u08 state);
void sffc_calculate(ts_u08 state);
void sffc_nr_refresh(ts_u08 state);
void SFFC_Refresh(ts_u08 state);
void NUC_Init(ts_u08 state);
void NUC_Calc(ts_u08 state);
void NUC_Calc_K(ts_u08 state);
void NUC_Save(ts_u08 state);

void OCC_Init(ts_u08 state);
void OCC_Calculate(ts_u08 state);
void OCC_Save(ts_u08 state);
void OCC_c_and_b(ts_u08 state);

void BPC_Maual_mode_set_or_unset_bpc(void);
void BPC_Maual_mode_set_or_unset_bpc_with_auto(void);
void GetAndDisplayIspStatisInfo(int status);
void MoveCusorToCenter(void);

void GetDisplayFixSncSwitch(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayEeEnhanceMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaySFFCSwitchStatus(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayTDCMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayTDCCalibrationIndex(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayTDCCalibrationOperation(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayTDCAispMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);

#ifdef CONFIG_SNR_PLANCK1280C
//光智1280探测器Det参数在菜单二显示
void Vot1280_GetAndDisplayGainParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void Vot1280_GetAndDisplayGofParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void Vot1280_GetAndDisplayCintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayNintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaySel_PJ_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayNTCC_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayVoccm_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaydetConfig_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayVfid_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
#endif

//光智640探测器Det参数在菜单二显示
void GetAndDisplayGainParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayGofParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayCintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayTintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayGosParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayNDAC_LParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayNDAC_HParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayGosCalcParameter(Point_MenuStructure Pt_Temp,u8 state);
void GetAndDisplayBAD_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplayBpcCfgParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplayBpcViewParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplaybpc_ext_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void Bpc_ext_Init(ts_u08 state);
void Bpc_ext_Del(ts_u08 state);
void GetDisplayDeviceFpsMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDeviceRawYuvModeMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void vGetAndDisplay_SemiAutoBPC(void);

void Automatic_Temp_GOS_Configuration(void);
ts_void getDisplay_Crosshair_Switch(void);

#endif
