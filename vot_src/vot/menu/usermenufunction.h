/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:10:17
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-04-16 14:37:00
 * @FilePath: /ucc3_a_gen/vot_src/vot/menu/usermenufunction.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef  __USER_MENU_FUNCTION_H__
#define  __USER_MENU_FUNCTION_H__
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

void  ResetDefaultMenuDisplay(void);

void GetDisplayDeviceSenceMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaySFilterParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayEnhanceLeverParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void ResetFactorySettingsParameter(Point_MenuStructure Pt_Temp,ts_u08 state);
void SaveUsermenuParameter(Point_MenuStructure Pt_Temp,ts_u08 state);
void GetAndDisplayAutoShutterParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayMirrorParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplayFontColorParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplayLanguageParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayEnlargeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayPIPParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayHSTParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayPaletteParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayContrastParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayBrightParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaySunPtGapParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplaySunPtModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);

/* 捕获参数 */
void getAndDisplay_lutCapture_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayOperatorSpeedParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayFocusFullSpeedParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayFoucs_DownNumSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayFocus_UpNumSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayCalibration_ScaleParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayADDR_SelParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);

void UpdatePromptInfoWithTimeMs(ts_char *user_info, ts_u32 u32_msecs, ts_u08 log_type);
void UpdatePromptInfo(ts_char *user_info, ts_u08 is_active, ts_u08 log_type);
void DisPlayDeviceInfoAndSoftVersionInfoAuto(ts_u08 Status);
void getAndDisplay_lutLevel_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
/* 测温点测试 */
ts_void getAndDisplay_Spots_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
/* 测温区域 */
ts_void getAndDisplay_Areas_Parameter (Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayRadianceParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayReflectionTemperatureParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayEnvironmentTemperatureParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayRelativeHumidityParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayDistanceParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayLensSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplay_Crosshair_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active);

#endif
