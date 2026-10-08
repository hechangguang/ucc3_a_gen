/*
 * testmenu.h
 *
 *  Created on: 2024年3月15日
 *      Author: Administrator
 */

#ifndef SRC_MENU_TESTMENU_H_
#define SRC_MENU_TESTMENU_H_

#include "../config.h"
#include <menu/votmain.h>
#include <menu/usermenu.h>

struct _MenuStructure;



// #ifdef AnFangProject
	// #define TestmenuTabNum 		14 	//测试菜单组数，一共8组测试菜单(菜单二)
// #else
	#define TestmenuTabNum 		17 	//测试菜单组数，一共8组测试菜单(菜单二)
// #endif

/*******************************************菜单二配置项 **********************************************************************/
	#define TTab1_Length 	6

#ifdef SUPPORT_VOT_DETECTOR
#ifdef CONFIG_SNR_PLANCK1280C
#define TTab2_Length 	12
#else
	#if DET_Change_Para
		#define TTab2_Length 	10
	#else
		#define TTab2_Length 	8
	#endif
#endif
#else
	#if DET_Change_Para
		#define TTab2_Length 	10
	#else
		#define TTab2_Length 	8
	#endif
#endif




#define TTab3_Length_Focus 	6
#define TTab3_Length_Gen 	4
#define TTab3_Length 	(TTab3_Length_Gen+TTab3_Length_Focus)


#define TTab3_andTab4_Length 	8

#ifdef OPR_ALARM
 	#ifdef AnFangProject
		#define TTab4_Length 	9
	#else
		#define TTab4_Length 	7
	#endif
#else
	#if  ((Fire_Alarm ==1)||(MYFirePoint_Alarm==1))
		#define TTab4_Length 	6
	#else
		#define TTab4_Length 	4
	#endif
#endif

	#define TTab5_Length	6

#ifdef SUPPORT_VOT_DETECTOR
	#define TTab6_Length	9
#else
	#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
		#define TTab6_Length	5
	#else
		#define TTab6_Length	5
	#endif
#endif

#if WLP
	#define TTab7_Length 	8
#else
	#ifdef  TEST_SAVE_Image
		#define TTab7_Length 	9
	#else
		#define TTab7_Length 	12
	#endif
#endif

#ifdef SUPPORT_VOT_DETECTOR
	#define TTab8_Length 		11
#else
	#define TTab8_Length 		10
#endif

#ifdef AnFangProject
	#define TTab9_Length 		4
#endif



#define TTab10_Length  4

#define TTab11_Length  5

#define TTab12_Length  4

#define TTab13_Length  5

#define TTab14_Length  4


#define TTab16_Length  9

#define TTab17_Length  4

#define TTab18_Length  2

#define TTab19_Length  2


#define TEST_X_START				32
#define TEST_Y_START				48
#define TEST_X_INCREMENT 			96

// typedef  MenuStructure* struct  _MenuStructure *;

void TestMenuInit(void);
void Key_EnterOrExitTestmenu(void);
void OCC_Init(ts_u08 state);
void OCC_Calculate(ts_u08 state);
void OCC_Save(ts_u08 state);
void NUC_Init(ts_u08 state);
void NUC_Calc(ts_u08 state);
void NUC_Save(ts_u08 state);
void Bad_Init(ts_u08 state);
void DynamicBlindPixelInit(ts_u08 state);
void InitializeTestmenuParameter(ts_u08 state);
void SaveTestmenuParameter(ts_u08 state);
void refreshDetMenu(void);

void DisplayTestmenuItemParameter(struct  _MenuStructure * Pt_Temp7,ts_u08 Status,ts_u08 is_active);
void GetCvbsDisplayEnlargeMode(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetUvcDisplayEnlargeMode(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetForceDectorType(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetForceHardwarePortType(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetMipiLanesParameter(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetBtTimingSequence(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetForceCustomerId(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetLensFocusType(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDebugLogEn(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayST1_M(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayST1_S(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayDET_ST(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayOutputVideoMode(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayBAD_ModeParameter(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);
void GetAndDisplayNUC_SWParameter(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetAndDisplayAutoSimiParameter(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);
void menu2_NUC_SunPT_SW(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);
void menu2_NUC_SunPT_RanGe(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);
void menu2_NUC_SunPT_Val(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);
void menu2_NUC_Protect_Time(struct  _MenuStructure * Pt_Temp,ts_u08 state, ts_u08 is_active);

void GetDisplayST0_total_duration(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void GetDisplayST0_interval(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);

//void GetChipType(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void SetNPU_SR(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);
void Set_TEMP_REGULARITY(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active);

void Update_Display_TestMenuItem(struct  _MenuStructure * Pt_T0, ts_u08 number, unsigned short ID );

#endif /* SRC_MENU_TESTMENU_H_ */
