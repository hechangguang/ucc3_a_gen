/*
 * Testmenu.c
 *
 *  Created on: 2024年3月15日
 *      Author: Administrator
 */
#include <base/ts_type.h>
#include <stdio.h>
#include "testmenu.h"
#include "name.h"
#include <menu/usermenufunction.h>
#include <menu/testmenufunction.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>


extern SysCtrlInfo_s_t global_sys_ctrl_info;

int test_xIncrement_spacing = 96;

/*************************************************************************/
/*********************************菜单二************************************/
/***************************所有的指针域初始化为NULL****************************/
/***************************所有的数据域域初始化不为空****************************/
// MenuStructure TestmenuTab1[TTab1_Length]  =
// {
// 	/***********************一级菜单**************************/
// 	{{TestmenuNameTab1[0],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x90},NULL,NULL,NULL,NULL},//"Algorithm"
// 	{{TestmenuNameTab1[1],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x70},NULL,NULL,NULL,NULL},//"OCC"
// 	{{TestmenuNameTab1[2],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x80},NULL,NULL,NULL,NULL},//"NUC"
// 	{{TestmenuNameTab1[3],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x60},NULL,NULL,NULL,NULL},//"DET"
// 	{{TestmenuNameTab1[4],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xA0},NULL,NULL,NULL,NULL},//"BPC"
// 	{{TestmenuNameTab1[5],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xB0},NULL,NULL,NULL,NULL},//"System"
// 	/*********************************************************/
// };

MenuStructure TestmenuTab1[TTab1_Length];
int  Real_TTab1_Length=0;

MenuStructure TestmenuTab2[TTab2_Length]=
{
	/***********************二级菜单第一级-DET********************/
#ifdef CONFIG_SNR_PLANCK1280C
	{{TestmenuNameTab2[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x61},NULL,NULL,NULL,NULL},//"Gain"
	{{TestmenuNameTab2[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x62},NULL,NULL,NULL,NULL},//"GOF"
	{{TestmenuNameTab2[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x63},NULL,NULL,NULL,NULL},//"Cint"
	{{TestmenuNameTab2[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x64},NULL,NULL,NULL,NULL},//"Nint"
	{{TestmenuNameTab2[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x65},NULL,NULL,NULL,NULL},//"Sel_PJ"
	{{TestmenuNameTab2[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x66},NULL,NULL,NULL,NULL},//"NTCC"
	{{TestmenuNameTab2[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x67},NULL,NULL,NULL,NULL},//"Voccm"
	{{TestmenuNameTab2[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x68},NULL,NULL,NULL,NULL},//"Config"
	{{TestmenuNameTab2[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x69},NULL,NULL,NULL,NULL},//"Vfid"
	{{TestmenuNameTab2[9],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6A},NULL,NULL,NULL,NULL},//"Init"
	{{TestmenuNameTab2[10],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6B},NULL,NULL,NULL,NULL},//"Mode"
	{{TestmenuNameTab2[11],12,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6C},NULL,NULL,NULL,NULL},//"Calc"	
#else

#ifdef SUPPORT_VOT_DETECTOR
	{{TestmenuNameTab2[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x61},NULL,NULL,NULL,NULL},//"Gain"
	{{TestmenuNameTab2[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x62},NULL,NULL,NULL,NULL},//"GOF"
	{{TestmenuNameTab2[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x63},NULL,NULL,NULL,NULL},//"Cint"
	{{TestmenuNameTab2[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x64},NULL,NULL,NULL,NULL},//"Tint"
	{{TestmenuNameTab2[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x65},NULL,NULL,NULL,NULL},//"GOS"
	{{TestmenuNameTab2[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x66},NULL,NULL,NULL,NULL},//"NdacL"
	{{TestmenuNameTab2[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x67},NULL,NULL,NULL,NULL},//"NdacH"
	{{TestmenuNameTab2[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x68},NULL,NULL,NULL,NULL},//"Init"
	#if DET_Change_Para
	{{TestmenuNameTab2[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x69},NULL,NULL,NULL,NULL},//"Mode"
	{{TestmenuNameTab2[9],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6A},NULL,NULL,NULL,NULL},//"Calc"
	// {{TestmenuNameTab2[10],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6B},NULL,NULL,NULL,NULL},//"one_click"
	#endif
#else
	{{TestmenuNameTab2[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x61},NULL,NULL,NULL,NULL},//"Gain"
	{{TestmenuNameTab2[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x62},NULL,NULL,NULL,NULL},//"GOF"
	{{TestmenuNameTab2[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x63},NULL,NULL,NULL,NULL},//"Cint"
	{{TestmenuNameTab2[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x64},NULL,NULL,NULL,NULL},//"Tint"
	{{TestmenuNameTab2[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x65},NULL,NULL,NULL,NULL},//"GOS"
	{{TestmenuNameTab2[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x68},NULL,NULL,NULL,NULL},//"Init"
	#if DET_Change_Para
	{{TestmenuNameTab2[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x69},NULL,NULL,NULL,NULL},//"Mode:"
	{{TestmenuNameTab2[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x6A},NULL,NULL,NULL,NULL},//"Calc"
	#endif
#endif
#endif

};

MenuStructure TestmenuTab3[TTab3_Length]=
{
	/***********************二级菜单第二级-OCC********************/
	// {{TestmenuNameTab4[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x71},NULL,NULL,NULL,NULL},//"OCC_Mode"
	// {{TestmenuNameTab4[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x72},NULL,NULL,NULL,NULL},//"OCC_Fix"
	// {{TestmenuNameTab4[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x79},NULL,NULL,NULL,NULL},//"OCC_way"
	{{TestmenuNameTab4[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x73},NULL,NULL,NULL,NULL},//"Init"
	{{TestmenuNameTab4[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x74},NULL,NULL,NULL,NULL},//"Calc"
	{{TestmenuNameTab4[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x75},NULL,NULL,NULL,NULL},//"Save"
	{{TestmenuNameTab4[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x76},NULL,NULL,NULL,NULL},//"Occ_b"
	/**********************AutoFocus Menu************************/
	// {{TestmenuNameTab4[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7A},NULL,NULL,NULL,NULL},//"Zoom_L"
	// {{TestmenuNameTab4[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7B},NULL,NULL,NULL,NULL},//"L_Val"
	// {{TestmenuNameTab4[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7C},NULL,NULL,NULL,NULL},//"R_Val"
	{{TestmenuNameTab4[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7D},NULL,NULL,NULL,NULL},//"Speed"
	{{TestmenuNameTab4[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7E},NULL,NULL,NULL,NULL},//"Focus_Init"
	// {{TestmenuNameTab4[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7F},NULL,NULL,NULL,NULL},//"FullSpeedSet" //测试菜单 05.28暂时去掉
	{{TestmenuNameTab4[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7D1},NULL,NULL,NULL,NULL},//"Foucs_DownNumSet"
	{{TestmenuNameTab4[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7D2},NULL,NULL,NULL,NULL},//"Focus_UpNumSet"	
	{{TestmenuNameTab4[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7D3},NULL,NULL,NULL,NULL},//"Table_Sel"	
	{{TestmenuNameTab4[9],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x7D4},NULL,NULL,NULL,NULL},//"ADDR_Sel"	
	/***********************************************************/
	/*********************************************************/
};

MenuStructure TestmenuTab4[TTab4_Length]=
{
	/***********************二级菜单第三级-NUC********************/
	{{TestmenuNameTab5[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x81},NULL,NULL,NULL,NULL},//"NUC_SW"
	// {{TestmenuNameTab5[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x85},NULL,NULL,NULL,NULL},//"Sel"
	{{TestmenuNameTab5[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x82},NULL,NULL,NULL,NULL},//"Init"
	{{TestmenuNameTab5[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x83},NULL,NULL,NULL,NULL},//"Calc"
	{{TestmenuNameTab5[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x84},NULL,NULL,NULL,NULL},//"Save"

#ifdef OPR_ALARM
	{{TestmenuNameTab5[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x86},NULL,NULL,NULL,NULL},//"Alarm",
	{{TestmenuNameTab5[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x87},NULL,NULL,NULL,NULL},//"A_Calc"
	{{TestmenuNameTab5[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x88},NULL,NULL,NULL,NULL},//"A_Val",
	#ifdef AnFangProject
	{{TestmenuNameTab5[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8D},NULL,NULL,NULL,NULL},//"A_Area"
	{{TestmenuNameTab5[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8E},NULL,NULL,NULL,NULL},//"AF_Val"
	#endif
#else

	#if  ((Fire_Alarm ==1)||(MYFirePoint_Alarm==1))
	{{TestmenuNameTab5[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x86},NULL,NULL,NULL,NULL},//"A_AD"
	{{TestmenuNameTab5[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x87},NULL,NULL,NULL,NULL},//"A_Num"
	#endif

#endif

};

MenuStructure TestmenuTab5[TTab5_Length]=
{
	/***********************二级菜单第四级-Algorithm********************/
	{{TestmenuNameTab6[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x101},NULL,NULL,NULL,NULL}, //sffc switch
	{{TestmenuNameTab6[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x102},NULL,NULL,NULL,NULL}, //sffc calc
	{{TestmenuNameTab6[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x104},NULL,NULL,NULL,NULL}, //sffc refresh
	{{TestmenuNameTab6[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x106},NULL,NULL,NULL,NULL}, //Temperature calibration
	{{TestmenuNameTab6[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x103},NULL,NULL,NULL,NULL}, //ee mode
	{{TestmenuNameTab6[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x105},NULL,NULL,NULL,NULL}, //fix_snc
	// {{TestmenuNameTab6[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x912},NULL,NULL,NULL,NULL},//"Outdoor:"
	// {{TestmenuNameTab6[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x9E},NULL,NULL,NULL,NULL}, //DDE_Pre
	// {{TestmenuNameTab6[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x9B},NULL,NULL,NULL,NULL}, //DDE_SW
	// {{TestmenuNameTab6[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x95},NULL,NULL,NULL,NULL}, //DDE_LV
	// {{TestmenuNameTab6[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x92},NULL,NULL,NULL,NULL}, //Gamma
	// {{TestmenuNameTab6[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x9A},NULL,NULL,NULL,NULL}, //Gama_Tab
	// {{TestmenuNameTab6[9],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x96},NULL,NULL,NULL,NULL},//TFliter
	// {{TestmenuNameTab6[10],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x97},NULL,NULL,NULL,NULL}, //SFliter
};

MenuStructure TestmenuTab6[TTab6_Length]=
{
	/***********************二级菜单第四级-DeadDot********************/
	{{TestmenuNameTab7[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa2},NULL,NULL,NULL,NULL},//"Manual"
	{{TestmenuNameTab7[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa1},NULL,NULL,NULL,NULL},//"BAD_SW"
	{{TestmenuNameTab7[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa3},NULL,NULL,NULL,NULL},//"bpc Mode"
	{{TestmenuNameTab7[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xaa},NULL,NULL,NULL,NULL},//"Semi_A"
	{{TestmenuNameTab7[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xab},NULL,NULL,NULL,NULL},//"bpc isp cfg"
	{{TestmenuNameTab7[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xac},NULL,NULL,NULL,NULL},//"bpc view"
	{{TestmenuNameTab7[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xad},NULL,NULL,NULL,NULL},//"Ext_SW"
	{{TestmenuNameTab7[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xae},NULL,NULL,NULL,NULL},//"Ext_Init"
	{{TestmenuNameTab7[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xaf},NULL,NULL,NULL,NULL},//"Ext_Del"
	// {{TestmenuNameTab7[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xaa},NULL,NULL,NULL,NULL},//"PixOfst"
	// {{TestmenuNameTab7[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xab},NULL,NULL,NULL,NULL},//"PixAuto"
	// {{TestmenuNameTab7[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa4},NULL,NULL,NULL,NULL},//"Init"
	
// #ifdef SUPPORT_VOT_DETECTOR
// 	{{TestmenuNameTab7[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa5},NULL,NULL,NULL,NULL},//"DTMY_SW"
// 	{{TestmenuNameTab7[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa6},NULL,NULL,NULL,NULL},//"DTMY_Init"
// #else
// 	#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
// 	{{TestmenuNameTab7[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa7},NULL,NULL,NULL,NULL},//"T_AlSW"
// 	{{TestmenuNameTab7[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa8},NULL,NULL,NULL,NULL},//"T_AlLmt"
// 	{{TestmenuNameTab7[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xa9},NULL,NULL,NULL,NULL},//"PixLmt"
// 	#endif
// #endif
	/*********************************************************/
};

MenuStructure TestmenuTab7[TTab7_Length]=
{
	/***********************二级菜单第四级-System********************/
	{{TestmenuNameTab8[0],1,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb1},NULL,NULL,NULL,NULL},//"Para_Init"
	{{TestmenuNameTab8[1],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb2},NULL,NULL,NULL,NULL},//"Para_Save"
	{{TestmenuNameTab8[2],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xbe},NULL,NULL,NULL,NULL},//"USB Mode"
	{{TestmenuNameTab8[3],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xbf},NULL,NULL,NULL,NULL},//"Enlarge ode"
	{{TestmenuNameTab8[4],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xfe},NULL,NULL,NULL,NULL},//"Temp_Meas"
	{{TestmenuNameTab8[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xff},NULL,NULL,NULL,NULL},//"Shutter"
	{{TestmenuNameTab8[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xff},NULL,NULL,NULL,NULL},//"Sun_Prot"
	{{TestmenuNameTab8[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xbd},NULL,NULL,NULL,NULL},//"fps"
	{{TestmenuNameTab8[8],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xff},NULL,NULL,NULL,NULL},//"ST1"
	{{TestmenuNameTab8[9],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xff},NULL,NULL,NULL,NULL},//"ST0"
	{{TestmenuNameTab8[10],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb6},NULL,NULL,NULL,NULL},//"DET_ST"
	{{TestmenuNameTab8[11],12,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb7},NULL,NULL,NULL,NULL},//"raw_yuv"

#ifdef WLP
	{{TestmenuNameTab8[5],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb9},NULL,NULL,NULL,NULL},//"Lens"
#endif

#ifdef  TEST_SAVE_Image
	{{TestmenuNameTab8[6],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xb9},NULL,NULL,NULL,NULL},//"Img_num"
	{{TestmenuNameTab8[7],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xbA},NULL,NULL,NULL,NULL},//"Img_Save"
#endif
	/*********************************************************/
};

MenuStructure TestmenuTab8[TTab8_Length]=
{
	/***********************Algorithm菜单下的AGC********************/
	{{TestmenuNameTab9[0],2,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x920},NULL,NULL,NULL,NULL},//"AGC_SW"
	{{TestmenuNameTab9[1],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x910},NULL,NULL,NULL,NULL},//"Mode"
	{{TestmenuNameTab9[2],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x911},NULL,NULL,NULL,NULL},//"Scene"
	{{TestmenuNameTab9[3],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x913},NULL,NULL,NULL,NULL},//"Bri_A"
	{{TestmenuNameTab9[4],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x914},NULL,NULL,NULL,NULL},//"Ctrs_A"
	{{TestmenuNameTab9[5],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x915},NULL,NULL,NULL,NULL},//"Bri_M"
	{{TestmenuNameTab9[6],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x916},NULL,NULL,NULL,NULL},//"Ctrs_M"
	{{TestmenuNameTab9[7],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x918},NULL,NULL,NULL,NULL},//"RanLim"
	{{TestmenuNameTab9[8],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x919},NULL,NULL,NULL,NULL},//"PlaLim"
	{{TestmenuNameTab9[9],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x921},NULL,NULL,NULL,NULL},//"Alpha"
#ifdef SUPPORT_VOT_DETECTOR
	{{TestmenuNameTab9[10],12,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x922},NULL,NULL,NULL,NULL},//"JLEPS"
#endif
};

#ifdef AnFangProject
MenuStructure TestmenuTab9[TTab9_Length]=
{
	{{TestmenuNameTab10[0],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8D0},NULL,NULL,NULL,NULL},//"Area1:"
	{{TestmenuNameTab10[1],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8D1},NULL,NULL,NULL,NULL},//"Area2"
	{{TestmenuNameTab10[2],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8D2},NULL,NULL,NULL,NULL},//"Area3"
	{{TestmenuNameTab10[3],12,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x8D3},NULL,NULL,NULL,NULL},//"Area4"
};
#endif

MenuStructure TestmenuTab10[TTab10_Length]=//Sun_Prot
{
	{{TestmenuNameTab11[0],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0x89},NULL,NULL,NULL,NULL},//"Switch"
	{{TestmenuNameTab11[1],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0x8A},NULL,NULL,NULL,NULL},//"Limit"
	{{TestmenuNameTab11[2],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0x8B},NULL,NULL,NULL,NULL},//"Num"
	{{TestmenuNameTab11[3],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0x8C},NULL,NULL,NULL,NULL},//"Time"
};

MenuStructure TestmenuTab11[TTab11_Length]=//Filter.son
{
	{{TestmenuNameTab12[0],3,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x98},NULL,NULL,NULL,NULL},//"V_DeSt"
	{{TestmenuNameTab12[1],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x99},NULL,NULL,NULL,NULL},//"H_DeSt"
	{{TestmenuNameTab12[2],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x9C},NULL,NULL,NULL,NULL},//"TF8_LV"
	{{TestmenuNameTab12[3],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x94},NULL,NULL,NULL,NULL},//"FK8_SW"
	{{TestmenuNameTab12[4],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x93},NULL,NULL,NULL,NULL},//"Plat_NX"
};

MenuStructure TestmenuTab12[TTab12_Length]=//Test.son
{
	{{TestmenuNameTab13[0],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x9F},NULL,NULL,NULL,NULL},//"AD_Set"
	{{TestmenuNameTab13[1],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0xb5},NULL,NULL,NULL,NULL},//"IR_TestD"
	{{TestmenuNameTab13[2],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0xb3},NULL,NULL,NULL,NULL},//"Temp_TD"
	{{TestmenuNameTab13[3],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0xb4},NULL,NULL,NULL,NULL},//"Test1"
};

MenuStructure TestmenuTab13[TTab13_Length]=//Temp_Meas.son
{
	{{TestmenuNameTab14[0],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x8D},NULL,NULL,NULL,NULL},//"Temp"
	{{TestmenuNameTab14[1],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x8E},NULL,NULL,NULL,NULL},//"AD_T"
	{{TestmenuNameTab14[2],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x8F},NULL,NULL,NULL,NULL},//"Calib"
	{{TestmenuNameTab14[3],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x900},NULL,NULL,NULL,NULL},//"Temp_Test"
	{{TestmenuNameTab14[4],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x901},NULL,NULL,NULL,NULL},//"Temp_Limit"
};

MenuStructure TestmenuTab14[TTab14_Length]=//Shutter.son
{
	// {{TestmenuNameTab15[0],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0x904},NULL,NULL,NULL,NULL},//"ShutFun"
	{{TestmenuNameTab15[1],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x902},NULL,NULL,NULL,NULL},//"Sensor"
	{{TestmenuNameTab15[2],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x903},NULL,NULL,NULL,NULL},//"data type"
	{{TestmenuNameTab15[3],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x904},NULL,NULL,NULL,NULL},//"UVC Format"
	{{TestmenuNameTab15[4],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x905},NULL,NULL,NULL,NULL},//"select temp regularity"	
};


MenuStructure TestmenuTab16[TTab16_Length]=//Enlarge
{
	{{TestmenuNameTab16[0],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x111},NULL,NULL,NULL,NULL},//"CVBS"
	{{TestmenuNameTab16[1],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x112},NULL,NULL,NULL,NULL},//"UVC"
	{{TestmenuNameTab16[2],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x114},NULL,NULL,NULL,NULL},//"MIPI Lane"
	{{TestmenuNameTab16[3],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x113},NULL,NULL,NULL,NULL},//"force detector select"
	{{TestmenuNameTab16[4],8,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x115},NULL,NULL,NULL,NULL},//"force hardware port select"
	{{TestmenuNameTab16[5],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x116},NULL,NULL,NULL,NULL},//"force customer id"
	{{TestmenuNameTab16[6],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x117},NULL,NULL,NULL,NULL},//"force lens type"
	{{TestmenuNameTab16[7],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x118},NULL,NULL,NULL,NULL},//"force debug log"
	{{TestmenuNameTab16[8],12,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x119},NULL,NULL,NULL,NULL},//"set NPU SR"
	
};


MenuStructure TestmenuTab17[TTab17_Length]=//Temperature Drift Compensation
{
	{{TestmenuNameTab17[0],4,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x121},NULL,NULL,NULL,NULL},//"Mode"
	{{TestmenuNameTab17[1],5,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x124},NULL,NULL,NULL,NULL},//"AISP 后处理"
	{{TestmenuNameTab17[2],6,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x122},NULL,NULL,NULL,NULL},//"Temperature calibration node index"
	{{TestmenuNameTab17[3],7,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x123},NULL,NULL,NULL,NULL},//"Temperature calibration operation"
};


MenuStructure TestmenuTab18[TTab18_Length]=//ST1
{
	{{TestmenuNameTab18[0],9,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x131},NULL,NULL,NULL,NULL},//"t1 total duration"
	{{TestmenuNameTab18[1],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x132},NULL,NULL,NULL,NULL},//"t1 interval"
};

MenuStructure TestmenuTab19[TTab19_Length]=//ST0 
{
	{{TestmenuNameTab19[0],10,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x141},NULL,NULL,NULL,NULL},//"t0 total duration"
	{{TestmenuNameTab19[1],11,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0x142},NULL,NULL,NULL,NULL},//"t0 interval"
};


void InitTestmenuTab1(void)
{
	static ts_s32 first_flag=1;
	// ts_u08 Serial_Number = 1;

	if(first_flag==1){
		ts_s32 TTab1_Son_Length=0;
		Real_TTab1_Length=0;
	
		ts_u08 osd_type  =  vot_menu_osd_type();

		if(osd_type==VOT_MENU_OSD_RES_384){
			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x90},NULL,NULL,NULL,NULL};
		
			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x70},NULL,NULL,NULL,NULL}; //occ_b

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x80},NULL,NULL,NULL,NULL};
		
			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x60},NULL,NULL,NULL,NULL};

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xA0},NULL,NULL,NULL,NULL};
		}
		else{

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*0,0x90},NULL,NULL,NULL,NULL};
		
			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x70},NULL,NULL,NULL,NULL};

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x80},NULL,NULL,NULL,NULL};
		
			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3,0x60},NULL,NULL,NULL,NULL};

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*4,0xA0},NULL,NULL,NULL,NULL};

			TestmenuTab1[Real_TTab1_Length++] = (MenuStructure){{TestmenuNameTab1[TTab1_Son_Length++],0,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*5,0xB0},NULL,NULL,NULL,NULL};
		}

		first_flag=0;
	}
}


/* occ and nuc for 384 mode */
MenuStructure TestmenuTab3andTab4[TTab3_andTab4_Length];
ts_s32 Real_UTab3_Tab4_Length=0;


void InitTestmenuTab2AndTab3(void)
{
	static ts_s32 first_flag=1;
	ts_u08 Serial_Number = 1;

	if(first_flag==1){
		ts_s32 UTab3_Tab4_Son_Length=0;
		Real_UTab3_Tab4_Length=0;
		// nuc tab
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab5[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x81},NULL,NULL,NULL,NULL};
	
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab5[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x82},NULL,NULL,NULL,NULL};

		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab5[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x83},NULL,NULL,NULL,NULL};
	
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab5[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*2,0x84},NULL,NULL,NULL,NULL};

		//occ tab
		UTab3_Tab4_Son_Length=0;
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab4[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x73},NULL,NULL,NULL,NULL};
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab4[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x74},NULL,NULL,NULL,NULL};
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab4[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x75},NULL,NULL,NULL,NULL};
		TestmenuTab3andTab4[Real_UTab3_Tab4_Length++] = (MenuStructure){{TestmenuNameTab4[UTab3_Tab4_Son_Length++],Serial_Number++,TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1,0x76},NULL,NULL,NULL,NULL};

		first_flag=0;
	}
}

/*************************************************************************/
// AllMenuItem TestmenuItemTable[TestmenuTabNum] =
// {
//  	{TestmenuTab1,TTab1_Length},//0---一级菜单

// 	{TestmenuTab5,TTab5_Length},//1---Algorithm
//  	{TestmenuTab3,TTab3_Length},//2---OCC
//  	{TestmenuTab4,TTab4_Length},//3---NUC
// 	{TestmenuTab2,TTab2_Length},//4---DET
// 	{TestmenuTab6,TTab6_Length},//5---BAD
// 	{TestmenuTab7,TTab7_Length},//6---System

// 	{TestmenuTab8,TTab8_Length},//7---AGC.Son
// 	{TestmenuTab10,TTab10_Length},//8---Sun_Prot.Son
// 	{TestmenuTab11,TTab11_Length},//9---Filter.Son
// 	{TestmenuTab12,TTab12_Length},//10---Test.Son
// 	{TestmenuTab13,TTab13_Length},//11---Temp_Meas
// 	{TestmenuTab14,TTab14_Length},//12---Shutter
// 	{TestmenuTab16,TTab16_Length},//13---enlarge

// #ifdef AnFangProject
// 	{TestmenuTab9,TTab9_Length},//" Area1:"," Area2:"," Area3:"," Area4:"
// #endif
// };

AllMenuItem TestmenuItemTable[TestmenuTabNum];
ts_s32 Real_TestmenuTab_Length = 0;

void InitTestmenuItemTable(void)
{
	static ts_s32 first_flag=1;

	
	InitTestmenuTab1();

	ts_u08 osd_type  =  vot_menu_osd_type();

	if(first_flag==1){
		
		if(osd_type==VOT_MENU_OSD_RES_384){
			InitTestmenuTab2AndTab3();
			vision_log_warn("Real_UTab3_Tab4_Length %d \n",Real_UTab3_Tab4_Length);
			TestmenuItemTable[0].number = Real_TTab1_Length; 
		}
		else{
			TestmenuItemTable[0].number = Real_TTab1_Length; //默认6栏模式
		}

		Real_TestmenuTab_Length = 0;
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab1,Real_TTab1_Length}; //0---一级菜单

		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab5,TTab5_Length}; //1---Algorithm

		if(osd_type==VOT_MENU_OSD_RES_384){
			TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab3andTab4,Real_UTab3_Tab4_Length}; //2-NUC & occ
		}
		else{
			if(get_focus_type()>0){ //带调焦相关菜单项
			TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab3,TTab3_Length};//2---OCC
			}else{
			TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab3,TTab3_Length_Gen};//2---OCC
			}
			
			TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab4,TTab4_Length};//3---NUC
		}

		/*4-6 3-5*/
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab2,TTab2_Length};//4---DET
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab6,TTab6_Length};//5---BAD
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab7,TTab7_Length};//6---System

		/*7-13 6-12*/
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab8,TTab8_Length};//7---AGC.Son
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab10,TTab10_Length};//8---Sun_Prot.Son
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab11,TTab11_Length};//9---Filter.Son
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab12,TTab12_Length};//10---Test.Son
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab13,TTab13_Length};//11---Temp_Meas
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab14,TTab14_Length};//12---Shutter Temp_Opt
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab16,TTab16_Length};//13---enlarge
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab17,TTab17_Length};//14---Temperature Drift Compensation
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab18,TTab18_Length};//15---ST1
		TestmenuItemTable[Real_TestmenuTab_Length++] = (AllMenuItem){TestmenuTab19,TTab19_Length};//16---ST0

		first_flag=0;
	}
	// vision_log_warn("TestmenuItemTable[0].number %d \n",TestmenuItemTable[0].number);

}

void TestMenuTabSetSpacing(AllMenuItem* item , int text_xIncrement_spacing, int test_x_start, int tab_index)
{
	Point_MenuStructure pt = item->pt;
	int item_len = item->number;

	ts_u08 osd_type  =  vot_menu_osd_type();
	
	for(item_len=0; item_len<item->number; item_len++)
	{
		//主菜单项
		if(tab_index==0){
			pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*item_len;
		}
		else{
			//子菜单项1开始计算
			if(tab_index>TestmenuItemTable[0].number){

				if(tab_index==TestmenuItemTable[0].number+1){
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*0; //AGC.Son 第一栏
				}
				else if(tab_index==TestmenuItemTable[0].number+2){ //Sun_Prot.Son 最后一栏
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //最后一栏
				}
				else if(tab_index==TestmenuItemTable[0].number+3){ //Filter.Son
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*0; //第一栏
				}
				else if(tab_index==TestmenuItemTable[0].number+4){ //Test.Son
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*0; //第一栏
				}
				else if(tab_index==TestmenuItemTable[0].number+5){ ////---Temp_Meas
					if(osd_type==VOT_MENU_OSD_RES_384){ //避免遮挡
						pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //倒数第二
					}
					else{
						pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //倒数第二
					}
					
				}
				else if(tab_index==TestmenuItemTable[0].number+6){ ////---Shutter
					if(osd_type==VOT_MENU_OSD_RES_384){
						pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //倒数第二
					}
					else{
						pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //倒数第二
					}
					
				}
				else if(tab_index==TestmenuItemTable[0].number+7){ ////---enlarge
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); //倒数第二
				}
				else if(tab_index==TestmenuItemTable[0].number+8){ ////---Temperature Drift Compensation
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*1; //第二列
				}
				else if(tab_index==TestmenuItemTable[0].number+9){ ////---ST1
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); 
				}
				else if(tab_index==TestmenuItemTable[0].number+10){ ////---ST0
					pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(TestmenuItemTable[0].number-2); 
				}
			}
			else{
				pt[item_len].Info.line_X = test_x_start+(text_xIncrement_spacing+X_GAP)*(tab_index-1);
			}
		}
	}

}

void TestMenu_SetSpacing(int test_xIncrement_spacing, int test_x_start)
{
	int tab_index = 0;
	for(tab_index=0; tab_index<Real_TestmenuTab_Length; tab_index++){
		TestMenuTabSetSpacing(&TestmenuItemTable[tab_index] ,  test_xIncrement_spacing,test_x_start, tab_index);
	}

}

/**
 * @brief:为所有的父节点添加子链
 * @parameter:NULL
 */
void Add_TestMenuItemSonList(void)
{
	 vision_log_warn("TestmenuItemTable[0].number %d \n",TestmenuItemTable[0].number);
	for(ts_u08 i=0;i<TestmenuItemTable[0].number;i++)
	{
		TestmenuItemTable[0].pt[i].Son = &TestmenuItemTable[1+i].pt[0];
		TestmenuItemTable[0].pt[i].Parent = &TestmenuItemTable[0].pt[i];
		TestmenuItemTable[0].pt[i].Info.SonNum = TestmenuItemTable[1+i].number;
	}
	// TestmenuTab5[0].Son = &TestmenuTab8[0];//AGC
	// TestmenuTab5[0].Info.SonNum = TTab8_Length;

	// TestmenuTab5[1].Son = &TestmenuTab11[0];//Filter
	// TestmenuTab5[1].Info.SonNum = TTab11_Length;

	// TestmenuTab5[2].Son = &TestmenuTab12[0];//Test
	// TestmenuTab5[2].Info.SonNum = TTab12_Length;

	
	TestmenuTab5[3].Son = &TestmenuTab17[0];//Temperature Drift Compensation
	TestmenuTab5[3].Info.SonNum = TTab17_Length;

	TestmenuTab7[3].Son = &TestmenuTab16[0];//Enlarge
	TestmenuTab7[3].Info.SonNum = TTab16_Length;

	TestmenuTab7[6].Son = &TestmenuTab10[0];//Sun_Prot
	TestmenuTab7[6].Info.SonNum = TTab10_Length;

	TestmenuTab7[4].Son = &TestmenuTab13[0];//Temp_Mesa
	TestmenuTab7[4].Info.SonNum = TTab13_Length;

	TestmenuTab7[5].Son = &TestmenuTab14[0];//Shutter Temp_Opt
	TestmenuTab7[5].Info.SonNum = TTab14_Length;

	TestmenuTab7[8].Son = &TestmenuTab18[0];//ST1
	TestmenuTab7[8].Info.SonNum = TTab18_Length;

	TestmenuTab7[9].Son = &TestmenuTab19[0];//ST0
	TestmenuTab7[9].Info.SonNum = TTab19_Length;

#ifdef AnFangProject
	TestmenuTab4[12].Son = &TestmenuTab9[0];
	TestmenuTab4[12].Info.SonNum = TTab9_Length;
#endif

}

/**
 * @brief:创建测试菜单(菜单二)
 * @parameter:NULL
 */
void CreatTestmenu(void)
{
	static int first_flag=1;
	int test_x_start = 10;

	InitTestmenuItemTable();

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	//只在开机第一次调整，避免切探测器类型后，菜单刷新异常
	if(first_flag==1){
		
		if(res_type==DECTOR_RESOLUTION_384X288){
			test_xIncrement_spacing=72; 
			SelectionMenu.IsEnableEnligh = 2; //384专用字符菜单
			test_x_start = 0;
		}
		else{
			test_xIncrement_spacing=96;
			test_x_start = TEST_X_START;//32
		}
		
	}
	else{
		first_flag=0;
	}

	TestMenu_SetSpacing(test_xIncrement_spacing,test_x_start);

	for(ts_u08 count = 0;count<Real_TestmenuTab_Length;count++)
	{
		CreatMenuList(TestmenuItemTable[count].pt,TestmenuItemTable[count].number);
	}
	Add_TestMenuItemSonList();

	for(ts_u08 count = 0;count<TestmenuItemTable[0].number;count++)
	{
		Add_MenuItemFatherList(TestmenuItemTable[count+1].pt,&TestmenuItemTable[0].pt[count]);
	}



	if(res_type==DECTOR_RESOLUTION_384X288){
		Add_MenuItemFatherList(TestmenuItemTable[6].pt,&TestmenuItemTable[1].pt[0]);

		Add_MenuItemFatherList(TestmenuItemTable[7].pt,&TestmenuItemTable[5].pt[6]); //Sun_Prot.Son

		Add_MenuItemFatherList(TestmenuItemTable[12].pt,&TestmenuItemTable[5].pt[3]); //enlarge mode
		Add_MenuItemFatherList(TestmenuItemTable[13].pt,&TestmenuItemTable[1].pt[3]); //Temperature Drift Compensation

		Add_MenuItemFatherList(TestmenuItemTable[8].pt,&TestmenuItemTable[1].pt[1]);

		Add_MenuItemFatherList(TestmenuItemTable[9].pt,&TestmenuItemTable[1].pt[2]);
		/* Temp_Meas */
		Add_MenuItemFatherList(TestmenuItemTable[10].pt,&TestmenuItemTable[5].pt[4]);
		/* Temp_Opt */
		Add_MenuItemFatherList(TestmenuItemTable[11].pt,&TestmenuItemTable[5].pt[5]);

		//ST1
		Add_MenuItemFatherList(TestmenuItemTable[14].pt,&TestmenuItemTable[5].pt[8]);

		//ST0
		Add_MenuItemFatherList(TestmenuItemTable[15].pt,&TestmenuItemTable[5].pt[9]);

	#ifdef AnFangProject
		Add_MenuItemFatherList(TestmenuItemTable[7].pt,&TestmenuItemTable[3].pt[12]);
	#endif
	}
	else{

		Add_MenuItemFatherList(TestmenuItemTable[7].pt,&TestmenuItemTable[1].pt[0]);

		Add_MenuItemFatherList(TestmenuItemTable[8].pt,&TestmenuItemTable[6].pt[6]); //Sun_Prot.Son

		Add_MenuItemFatherList(TestmenuItemTable[13].pt,&TestmenuItemTable[6].pt[3]); //enlarge mode
		Add_MenuItemFatherList(TestmenuItemTable[14].pt,&TestmenuItemTable[1].pt[3]); //Temperature Drift Compensation

		Add_MenuItemFatherList(TestmenuItemTable[9].pt,&TestmenuItemTable[1].pt[1]);

		Add_MenuItemFatherList(TestmenuItemTable[10].pt,&TestmenuItemTable[1].pt[2]);
		/* Temp_Meas */
		Add_MenuItemFatherList(TestmenuItemTable[11].pt,&TestmenuItemTable[6].pt[4]);

		/* Temp_Opt */
		Add_MenuItemFatherList(TestmenuItemTable[12].pt,&TestmenuItemTable[6].pt[5]);

		//ST1
		Add_MenuItemFatherList(TestmenuItemTable[15].pt,&TestmenuItemTable[6].pt[8]);

		//ST0
		Add_MenuItemFatherList(TestmenuItemTable[16].pt,&TestmenuItemTable[6].pt[9]);
		
		
	#ifdef AnFangProject
		Add_MenuItemFatherList(TestmenuItemTable[8].pt,&TestmenuItemTable[3].pt[12]);
	#endif
	}

	//暂时不显示Algorithm子菜单
	// TestmenuTab1[0].Son = NULL;
}

/**
 * @brief:用户菜单初始化
 * @parameter:NULL
 */
void TestMenuInit(void)
{
	CreatTestmenu();
}

/**
 * @brief:使用按键进入或者退出测试菜单(菜单二)界面
 * @parameter：NULL
 */
void Key_EnterOrExitTestmenu(void)
{
	//当前不再测温点或者测温区域的移动界面/半自动校点界面,就可以进入菜单二
	if((SelectionMenu.MenuState &0x03) != 0x03)
	{
		//已经在菜单二,再按菜单二需要退出菜单二
		if(SelectionMenu.MenuState == 0x26)
		{
			//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
			while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
			{
				SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
				Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
						SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
			}
			Clear_MenuInfo(SelectionMenu.SelectItem,Real_TTab1_Length);//清除一级菜单，退出一级菜单
			// GetAndDisplayIspStatisInfo(0);
			
			if(SystemParaMenu2.Auto_Semi)//如果现在打开了半自动校点，然后需要退出菜单二，把半自动校点也关掉
			{
				if(Area_state&0x0010)
				{
					Area_state = (Area_state&0xffef)|0x0000;
					// DisPlay_BAD_Opr(&BadMes,2,0);
					BadArea_Display(0);
				}
				SystemParaMenu2.Auto_Semi = 0;
			}
			// if(UsermenuParameter.CenterSpotSW)	Display_CenterSpot(UsermenuParameter.CenterSpotSW);  //中心光标显示（无温度）
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,
//					(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
			// ClearFont(0,64);
			SelectionMenu.SelectItem = NULL;//当前选中项是空，退出一级菜单
			SelectionMenu.MenuState = 0x00;//当前菜单状态为0，表示没有任何菜单，也没有进入任何界面
			ISP_set_bottom_statistic_force_flag(1);
		}
		else
		{
// #ifdef  SUPPORT_CONTINUOUS_ZOOM		
// 			//当前选中项是电子放大界面，且电子放大不是1.0倍,则不能进入菜单二
// 			if(UsermenuParameter.Enlarge>10)
// 			{
// 				return;
// 			}
// #else
// 			//当前选中项是电子放大界面，且电子放大选中了X2/X4/X8,则不能进入菜单二
// 			if(UsermenuParameter.Enlarge)
// 			{
// 				return;
// 			}
// #endif			
// 			else

			{
				if(SelectionMenu.MenuState == 0x25)//在用户菜单，需要先退出
				{
					//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
					while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
					{
						SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
						Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
								SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
					}
#ifdef  SUPPORT_CONTINUOUS_ZOOM					
					// EraseZoomMeg();//清除变倍显示
#endif					
					Clear_MenuInfo(SelectionMenu.SelectItem,Real_TTab1_Length);//清除一级菜单，退出一级菜单
				}
				Erase_Deviceinfo();//清除设备信息
				ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
				ClearSportsInfo();//清除测温点信息
				ClearAreasInfo();//清除测温区域信息
				// if(UsermenuParameter.CaptureFlag)//打开过捕获点，都要清掉
				// {
				// 	ClearSize16X16(X_START,28*16+150,Y_START+392);
				// 	DisPlaySpot(spot_max.spot_X_Pos,spot_max.spot_Y_Pos,2,0,0);
				// 	DisPlaySpot(spot_min.spot_X_Pos,spot_min.spot_Y_Pos,2,0,0);
				// }
				Display_CenterSpot(0);  //清除中心光标显示（无温度）
				SelectionMenu.SelectItem = &TestmenuTab1[0];//选中菜单二
				SelectionMenu.MenuState = 0x26;//当前菜单状态为0x26,进入菜单二
				Display_MenuItem(SelectionMenu.SelectItem,Real_TTab1_Length, 0);

//				APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,
//									(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(1<<6));
				// ClearFont(0,64);	//清除HypRAM中的所有数据
			}
		}
	}
}



// ts_u08 menu2_Val_SET_ts_u08(Point_MenuStructure temp_node3,ts_u08 state,ts_u08 *par,ts_u08 min,ts_u08 max)
// {
// 	ts_u08 SetFlag= 0;
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = temp_node3->Info.line_X+8*GetStringSize(temp_node3->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*temp_node3->Info.line_Y;
// 	if(state == 0)
// 	{
// 		//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 	}
// 	else if(state ==1)
// 	{
// 		*par+=1;
// 		if(*par > max)
// 		{
// 			*par = max;
// 		}
// 		else
// 		{
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 		}
// 	}
// 	else
// 	{
// 		if(*par>min)
// 		{
// 			*par-=1;
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 		}
// 		else
// 			*par = min;
// 	}
// 	return SetFlag;
// }
// ts_u08 menu2_Val_SET_ts_u16(Point_MenuStructure temp_node3,ts_u08 state,ts_u16 *par,ts_u16 min,ts_u16 max)
// {
// 	ts_u08 SetFlag= 0;
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = temp_node3->Info.line_X+8*GetStringSize(temp_node3->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*temp_node3->Info.line_Y;
// 	if(state == 0)
// 	{
// 		//display_ts_u16number(Xline,Yline,*par ,4,CharacterColor);
// 	}
// 	else if(state ==1)
// 	{
// 		*par+=1;
// 		if(*par > max)
// 		{
// 			*par = max;
// 		}
// 		else
// 		{
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u16number(Xline,Yline,*par,4,CharacterColor);
// 		}
// 	}
// 	else
// 	{
// 		if(*par>min)
// 		{
// 			*par-=1;
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u16number(Xline,Yline,*par,4,CharacterColor);
// 		}
// 		else
// 			*par = min;
// 	}
// 	return SetFlag;
// }

// ts_u08 menu2_Val_SET_ts_u32(Point_MenuStructure temp_node3,ts_u08 state,ts_u32 *par,ts_u32 min,ts_u32 max)
// {
// 	ts_u08 SetFlag= 0;
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = temp_node3->Info.line_X+8*GetStringSize(temp_node3->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*temp_node3->Info.line_Y;
// 	if(state == 0)
// 	{
// 		//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 	}
// 	else if(state ==1)
// 	{
// 		*par+=1;
// 		if(*par > max)
// 		{
// 			*par = max;
// 		}
// 		else
// 		{
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 		}
// 	}
// 	else
// 	{
// 		if(*par>min)
// 		{
// 			*par-=1;
// 			SetFlag =1;
// 			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
// 			//display_ts_u08number(Xline,Yline,*par,3,CharacterColor);
// 		}
// 		else
// 			*par = min;
// 	}
// 	return SetFlag;
// }
// #ifdef OPR_ALARM
// void menu2_NUC_Alarm_SW(Point_MenuStructure temp_node3,ts_u08 state)
// {
// 	if(menu2_Val_SET_ts_u08(temp_node3,state,&SystemParaMenu2.Alarm_SW,0,2))
// 	{
// 		APB3_REGW_Alarm_Range(SystemParaMenu2.Alarm_SW);
// 	}
// }
// void menu2_NUC_range_SW(Point_MenuStructure temp_node3,ts_u08 state)
// {
// 	if(menu2_Val_SET_ts_u08(temp_node3,state,&SystemParaMenu2.Range_SW,0,2))
// 	{
// 		APB3_REGW_Alarm_SW(SystemParaMenu2.Range_SW);
// 		if(SystemParaMenu2.Range_SW ==1)
// 		{
// 			APB3_REGW_Alarm_AutoVal(SystemParaMenu2.AlarmAutoVal);
// 		}
// 		else if(SystemParaMenu2.Range_SW ==2)   APB3_REGW_Alarm_Val(SystemParaMenu2.AlarmVal);
// 	}
// }
// void menu2_NUC_Val_SW(Point_MenuStructure temp_node3,ts_u08 state)
// {
// 	if(menu2_Val_SET_ts_u16(temp_node3,state,&SystemParaMenu2.AlarmVal,0,0x3FF))
// 	{
// 		 APB3_REGW_Alarm_Val(SystemParaMenu2.AlarmVal);
// 	}
// }
// #endif



void menu2_NUC_SunPT_SW(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 SunPT_SW = SystemParaMenu2.SunPT_SW;

	if(SunPT_SW>1){
		SunPT_SW = 1;
	}

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("menu2_NUC_SunPT_SW Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.SunPT_SW=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.SunPT_SW);
	if(state)
	{
		if(SunPT_SW==0)
		{
			SunPT_SW = 1;
		}
		else if(SunPT_SW==1)
		{
			SunPT_SW = 0;
		}
	}

	if(SunPT_SW == 0){
		sprintf((ts_char*)AddBuffer, "%s", "OFF");
		
	}
	else if(SunPT_SW == 1){
		sprintf((ts_char*)AddBuffer, "%s", "ON");
	}

	if(state){
		if(SunPT_SW!=SystemParaMenu2.SunPT_SW){
			ISP_set_sun_pt_sw(SunPT_SW);
			if(SunPT_SW==0){
				  if ( global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_UPDATE_FIRMWARE){
					global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
				  }
			}
		}
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

}

void menu2_NUC_SunPT_RanGe(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("menu2_NUC_SunPT_RanGe Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.S_Range=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.S_Range);
	if(MAX_SUN_PT_GRAY_LEVEL<SystemParaMenu2.S_Range){
		SystemParaMenu2.S_Range = DEFAULT_SUN_PT_GRAY_LEVEL; // 灰度值 1000 ，ad值 1000*16=16000
	}

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.S_Range<(MAX_SUN_PT_GRAY_LEVEL-10))
		{
			SystemParaMenu2.S_Range += 10;
		}
		else
		{
			SystemParaMenu2.S_Range = MAX_SUN_PT_GRAY_LEVEL;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.S_Range > 10)
		{
			SystemParaMenu2.S_Range-=10;
		}
		else
		{
			SystemParaMenu2.S_Range = 0;
		}
	}
	display_append_u16number((ts_u16*)&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.S_Range);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
	// 	if(SystemParaMenu2.Usermenu_SW){
	// 	APB3_REGW_S_Range(SystemParaMenu2.S_Range);
	// 	if(SystemParaMenu2.Usermenu_SW==1)				{SystemParaMenu2.S_Range1 = SystemParaMenu2.S_Range;}
	// 	else if(SystemParaMenu2.Usermenu_SW==2)		{SystemParaMenu2.S_Range2 = SystemParaMenu2.S_Range;}}
	// else	{
	// 	SystemParaMenu2.S_Range0 = SystemParaMenu2.S_Range;
	// 	APB3_REGW_S_Range(SystemParaMenu2.S_Range - FPGA_Bak_AVG()/2);}
	}
	
}

void menu2_NUC_SunPT_Val(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayDDELeverParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.S_Val=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.S_Val);
	if(MAX_SUN_PT_THRESHOLD<SystemParaMenu2.S_Val){
		SystemParaMenu2.S_Val = DEFAULT_SUN_PT_THRESHOLD;
	}

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.S_Val<MAX_SUN_PT_THRESHOLD)
		{
			SystemParaMenu2.S_Val++;
		}
		else
		{
			SystemParaMenu2.S_Val = MAX_SUN_PT_THRESHOLD;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.S_Val > 1)
		{
			SystemParaMenu2.S_Val--;
		}
		else
		{
			SystemParaMenu2.S_Val = 1;
		}
	}
	display_append_u16number((ts_u16*)&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.S_Val);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){

	// if(SystemParaMenu2.Usermenu_SW){
	// 	if(SystemParaMenu2.Usermenu_SW==1)				{SystemParaMenu2.S_Val1 = SystemParaMenu2.S_Val;}
	// 	else if(SystemParaMenu2.Usermenu_SW==2)		{SystemParaMenu2.S_Val2 = SystemParaMenu2.S_Val;}}
	// else	{SystemParaMenu2.S_Val0 = SystemParaMenu2.S_Val;}
	}
}


void menu2_NUC_Protect_Time(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("menu2_NUC_Protect_Time Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.P_Time_U16=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.P_Time_U16);

	ts_u16  P_Time_U16 = SystemParaMenu2.P_Time_U16;
	if(P_Time_U16>=MAX_SUN_PT_TIME_SECS){
		P_Time_U16 = DEFAULT_SUN_PT_TIME_SECS;
	}
	
	if(state == 1)
	{
		if(P_Time_U16 < MAX_SUN_PT_TIME_SECS)
		{
			P_Time_U16++;
		}

	}
	else if(state == 2)
	{
		if(P_Time_U16 > 1)
		{
			P_Time_U16--;
		}
		else
		{
			P_Time_U16 = 1;
		}
	}
	display_append_u16number((ts_u16*)&Pt_Temp->Info.NameAdd[MenuNameLen], P_Time_U16);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		if(P_Time_U16!= SystemParaMenu2.P_Time_U16){
			ISP_set_sun_pt_gap(P_Time_U16);
		}
	}
	vision_log_debug("MAX_SUN_PT_TIME_SECS =%d, P_Time_U16=%u\n",MAX_SUN_PT_TIME_SECS, P_Time_U16);

}
/** @note 测温挡位量程限制 */
void menu2_templut_TempSW_Limit(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	const ts_char* limitStr = NULL;
    /* 挡位范围保护 */
    if (SystemParaMenu2.m_templutMode_Limit < 1 || SystemParaMenu2.m_templutMode_Limit > 3) {
        SystemParaMenu2.m_templutMode_Limit = 1;
    }
    menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
    vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.m_templutMode_Limit = %d\n", 
                     Pt_Temp, state, menuName_Len, SystemParaMenu2.m_templutMode_Limit);
    if (state == 0) {
        switch (SystemParaMenu2.m_templutMode_Limit) {
            case 1:  limitStr = "L1"; break;
            case 2:  limitStr = "L2"; break;
            case 3:  limitStr = "L3"; break;
            default: limitStr = "L1"; break;
        }
        strcpy((ts_char*)&Pt_Temp->Info.NameAdd[menuName_Len], limitStr);
        Display_MenuItemLabel(Pt_Temp, is_active);
    }
    else if (state == 1) {
        SystemParaMenu2.m_templutMode_Limit++;
        if (SystemParaMenu2.m_templutMode_Limit > 3) {
            SystemParaMenu2.m_templutMode_Limit = 3;
            UpdatePromptInfoWithTimeMs("Max Limit Reached", 1000, VOT_LOG_TYPE_MENU2);
        }
        limitStr = (SystemParaMenu2.m_templutMode_Limit == 1) ? "L1" :
                   (SystemParaMenu2.m_templutMode_Limit == 2) ? "L2" : "L3";
        strcpy((ts_char*)&Pt_Temp->Info.NameAdd[menuName_Len], limitStr);
        Display_MenuItemLabel(Pt_Temp, is_active);
    }
    else {
        if (SystemParaMenu2.m_templutMode_Limit > 1) {
            SystemParaMenu2.m_templutMode_Limit--;
        } else {
            SystemParaMenu2.m_templutMode_Limit = 1;
            UpdatePromptInfoWithTimeMs("Min Limit Reached", 1000, VOT_LOG_TYPE_MENU2);
        }
        limitStr = (SystemParaMenu2.m_templutMode_Limit == 1) ? "L1" :
                   (SystemParaMenu2.m_templutMode_Limit == 2) ? "L2" : "L3";
        strcpy((ts_char*)&Pt_Temp->Info.NameAdd[menuName_Len], limitStr);
        Display_MenuItemLabel(Pt_Temp, is_active);
    }
}
/** @note 菜单二测温挡位切换(运行挡位与目标挡位分离) */
void menu2_templut_TempSW_Switch(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	static const char* const gear_str[4] = {"OFF", "ONE", "TWO", "THR"};
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
    ts_u08 display_gear;

    /* 确保限制值有效 */
    if (SystemParaMenu2.m_templutMode_Limit < 1 || SystemParaMenu2.m_templutMode_Limit > 3) {
        SystemParaMenu2.m_templutMode_Limit = 1;
    }
    if (SystemParaMenu2.Usermenu_SW > SystemParaMenu2.m_templutMode_Limit) {
        SystemParaMenu2.Usermenu_SW = SystemParaMenu2.m_templutMode_Limit;
		vision_log_debug("SystemParaMenu2.Usermenu_SW = %d, SystemParaMenu2.m_templutMode_Limit = %d\n", SystemParaMenu2.Usermenu_SW, SystemParaMenu2.m_templutMode_Limit);
    }

    /* 确定菜单显示用的挡位 */
    if (SystemParaMenu2.m_templut_target_gear == 0 || SystemParaMenu2.m_templut_target_gear == 1) {
        display_gear = SystemParaMenu2.m_templut_target_gear;
	}
    else {
        display_gear = SystemParaMenu2.Usermenu_SW;
	}
    menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
    vision_log_debug("Pt_Temp=%p, state=%d, display_gear=%d, Usermenu_SW=%d, target_gear=%d\n", Pt_Temp, state, display_gear, SystemParaMenu2.Usermenu_SW, SystemParaMenu2.m_templut_target_gear);

	if (state == 0) {
        sprintf((ts_char*)addBuffer, "%s", gear_str[display_gear]);
        UsermenuItemTable[0].pt[1].Son = (display_gear == 0) ? NULL : &UsermenuItemTable[2].pt[0];
        memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
        Display_MenuItemLabel(Pt_Temp, is_active);
	}
    else if (SystemParaMenu2.m_templut_target_gear != 0xFF) {
        /* 有待生效的挡位切换，必须先重启，拒绝继续操作 */
		vision_log_debug("Blocked: target=%d pending, reboot required\n", SystemParaMenu2.m_templut_target_gear);
        UpdatePromptInfoWithTimeMs("Please power off/on...", 1000, VOT_LOG_TYPE_MENU2);
		return;
    }
	else if (state == 1) {
		if (display_gear < SystemParaMenu2.m_templutMode_Limit) {
            ts_u08 new_gear = display_gear + 1;
            // 从观测版切到测温版，需要重启
            ts_u08 need_reboot = (display_gear == 0);
            if (need_reboot) {
                SystemParaMenu2.m_templut_target_gear = new_gear;	// SystemParaMenu2.m_templut_target_gear = 1
                ISP_write_sys_menu2_cfg(SystemParaMenu2);

                sprintf((ts_char*)addBuffer, "%s", gear_str[new_gear]);
                UsermenuItemTable[0].pt[1].Son = &UsermenuItemTable[2].pt[0];
                memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
                Display_MenuItemLabel(Pt_Temp, is_active);

				vision_log_debug("OFF->%d: target set, REBOOT REQUIRED\n", new_gear);
                UpdatePromptInfoWithTimeMs("Please power off/on...", 1000, VOT_LOG_TYPE_MENU2);
                return;
            }
            /* 测温版内部切换：立即生效 */
            SystemParaMenu2.Usermenu_SW = new_gear;
            UsermenuItemTable[0].pt[1].Son = &UsermenuItemTable[2].pt[0];

            global_sys_ctrl_info.Usermenu_SW_Switching = 1;
            // ts_cpl_fix_msleep(60);
            UpdatePromptInfo("Temperature SW Change...", 1, VOT_LOG_TYPE_MENU2);

            sprintf((ts_char*)addBuffer, "%s", gear_str[new_gear]);
            memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
            Display_MenuItemLabel(Pt_Temp, is_active);

            global_sys_ctrl_info.Shifting_Gears_Flag = 1;
            TemLevel_convert();
            global_sys_ctrl_info.Shifting_Gears_Flag = 0;
            UpdatePromptInfo("Temperature SW Change...", 0, VOT_LOG_TYPE_MENU2);
            global_sys_ctrl_info.Usermenu_SW_Switching = 0;
		}
		else {
			UpdatePromptInfoWithTimeMs("Max Level Reached", 1000, VOT_LOG_TYPE_MENU2);
			return;
		}
	}
	else {
		if (display_gear > 0) {
			ts_u08 new_gear = display_gear - 1;
            // 从测温版切回观测版，需要重启
            ts_u08 need_reboot = (new_gear == 0);
            if (need_reboot) {
                SystemParaMenu2.m_templut_target_gear = new_gear;	// SystemParaMenu2.m_templut_target_gear = 0
				Clear_Templut_Information();
				ISP_write_sys_cfg();
                ISP_write_sys_menu2_cfg(SystemParaMenu2);
				Write_TempParameter_cfg(&Temp_Parameter);

                sprintf((ts_char*)addBuffer, "%s", gear_str[new_gear]);
                UsermenuItemTable[0].pt[1].Son = NULL;
                memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
                Display_MenuItemLabel(Pt_Temp, is_active);

				vision_log_debug("%d->OFF: target set, REBOOT REQUIRED\n", display_gear);
                UpdatePromptInfoWithTimeMs("Please power off/on...", 1000, VOT_LOG_TYPE_MENU2);
                return;
            }
            /* 测温版内部切换：立即生效 */
            SystemParaMenu2.Usermenu_SW = new_gear;

            global_sys_ctrl_info.Usermenu_SW_Switching = 1;
            // ts_cpl_fix_msleep(60);
            UpdatePromptInfo("Temperature SW Change...", 1, VOT_LOG_TYPE_MENU2);

            UsermenuItemTable[0].pt[1].Son = (new_gear == 0) ? NULL : &UsermenuItemTable[2].pt[0];
            sprintf((ts_char*)addBuffer, "%s", gear_str[new_gear]);
            memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
            Display_MenuItemLabel(Pt_Temp, is_active);

            global_sys_ctrl_info.Shifting_Gears_Flag = 1;
            TemLevel_convert();
            global_sys_ctrl_info.Shifting_Gears_Flag = 0;
            UpdatePromptInfo("Temperature SW Change...", 0, VOT_LOG_TYPE_MENU2);
            global_sys_ctrl_info.Usermenu_SW_Switching = 0;
		}
		else {
			UpdatePromptInfoWithTimeMs("Min Level Reached", 1000, VOT_LOG_TYPE_MENU2);
			return;
		}
	}
	if (state != 0 && SystemParaMenu2.m_templut_target_gear == 0xFF) {
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG) {
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
			cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
			cvbs_osd_refresh_display(1);
		}
	}
}
static ts_u08 normalize_AD_T(ts_u08 ad_t, ts_u08 calib_flag)
{
    if (calib_flag == 1) {
        return 3;   // 强制约束
    }

    if (ad_t > 3) {
        return 0;   // 防御性修复
    }

    return ad_t;
}
/** @note 温度帧之AD_T */
void menu2_templut_ADT_Param(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
	ts_u08 tmp_AD_T;

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.AD_T = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.AD_T);
	/* ① 先规范化 */
    tmp_AD_T = normalize_AD_T(SystemParaMenu2.AD_T, SystemParaMenu2.m_temperCalibra_Flag);

	if (state == 0)
	{
		if (tmp_AD_T == 0) {
			sprintf((ts_char*)addBuffer, "%s", "ORAD");
		}
		else if (tmp_AD_T == 1) {
			sprintf((ts_char*)addBuffer, "%s", "LTAD");
		}
		else if (tmp_AD_T == 2) {
			sprintf((ts_char*)addBuffer, "%s", "TEMP");
		}
		else if (tmp_AD_T == 3) {
			sprintf((ts_char*)addBuffer, "%s", "CAAD");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1) {
		if (SystemParaMenu2.m_temperCalibra_Flag) {
			sprintf((ts_char*)addBuffer, "%s", "CAAD");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
		else {
			tmp_AD_T++;
			if (tmp_AD_T > 2) {
				tmp_AD_T = 2;
			}
			else {
				if (tmp_AD_T == 1) {
					sprintf((ts_char*)addBuffer, "%s", "LTAD");
				}
				else if (tmp_AD_T == 2) {
					sprintf((ts_char*)addBuffer, "%s", "TEMP");
				}
				memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
				Display_MenuItemLabel(Pt_Temp, is_active);
			}
		}
	}
	else {
		if (SystemParaMenu2.m_temperCalibra_Flag) {
			sprintf((ts_char*)addBuffer, "%s", "CAAD");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
		else {
			if (tmp_AD_T > 0) {
				tmp_AD_T--;
				if (tmp_AD_T == 0) {
					sprintf((ts_char*)addBuffer, "%s", "ORAD");
				}
				else if (tmp_AD_T == 1) {
					sprintf((ts_char*)addBuffer, "%s", "LTAD");
				}
				memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
				Display_MenuItemLabel(Pt_Temp, is_active);
			}
			else {
				tmp_AD_T = 0;
			}
		}
	}
    /* ④ 再次规范化（防御） */
    SystemParaMenu2.AD_T = normalize_AD_T(tmp_AD_T, SystemParaMenu2.m_temperCalibra_Flag);
}
/** @note 温度帧之标定 */
void menu2_templut_Calib_State(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
	ts_u08 tmp_CalibraStatus = SystemParaMenu2.m_temperCalibra_Flag;

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, is_active = %d, menuName_Len = %d, SystemParaMenu2.m_temperCalibra_Flag = %d\n", 
					Pt_Temp, state, is_active, menuName_Len, SystemParaMenu2.m_temperCalibra_Flag);
	tmp_CalibraStatus = !!tmp_CalibraStatus;
	if (state == 0) {
		if (tmp_CalibraStatus) {
			sprintf((ts_char*)addBuffer, "%s", "ON");
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "OFF");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1) {
		if (tmp_CalibraStatus == 0) {
			tmp_CalibraStatus = 1;
			sprintf((ts_char*)addBuffer, "%s", "ON");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
	else {
		if (tmp_CalibraStatus == 1) {
			tmp_CalibraStatus = 0;
			/* 退出标定时同步 AD_T */
			if (SystemParaMenu2.AD_T == 3) {
				SystemParaMenu2.AD_T = 2;
			}
			sprintf((ts_char*)addBuffer, "%s", "OFF");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
	SystemParaMenu2.m_temperCalibra_Flag = tmp_CalibraStatus;
}
/** @note 温度帧之Temp_T */
void getAndDisplay_TemTestSW_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.TemTest_SW = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.TemTest_SW);
	
	if (state == 0) {
		if(SystemParaMenu2.TemTest_SW == 0)
		{
			sprintf((ts_char*)addBuffer, "%s", "OFF");
		}
		else if(SystemParaMenu2.TemTest_SW == 1)
		{
			sprintf((ts_char*)addBuffer, "%s", "INT");
		}
		else if(SystemParaMenu2.TemTest_SW == 2)
		{
			sprintf((ts_char*)addBuffer, "%s", "Sen");
		}
		else if(SystemParaMenu2.TemTest_SW == 3)
		{
			sprintf((ts_char*)addBuffer, "%s", "All");
		}
		else if(SystemParaMenu2.TemTest_SW == 4)
		{
			sprintf((ts_char*)addBuffer, "%s", "NAG");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(state == 1)
	{
	//	if(SystemParaMenu2.Usermenu_SW)
		{
			SystemParaMenu2.TemTest_SW++;
			if(SystemParaMenu2.TemTest_SW > 4) {
				SystemParaMenu2.TemTest_SW = 4;
			}
			else {
				if(SystemParaMenu2.TemTest_SW == 1)
				{
					sprintf((ts_char*)addBuffer, "%s", "INT");
				}
				else if(SystemParaMenu2.TemTest_SW == 2)
				{
					sprintf((ts_char*)addBuffer, "%s", "Sen");
				}
				else if(SystemParaMenu2.TemTest_SW == 3)
				{
					sprintf((ts_char*)addBuffer, "%s", "All");
				}
				else if(SystemParaMenu2.TemTest_SW == 4)
				{
//					AutoGOF_Flag = 0;
					sprintf((ts_char*)addBuffer, "%s", "NAG");
				}
//				if(SystemParaMenu2.TemTest_SW<4)	{AutoGOF_Flag = 1;}
				memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
				Display_MenuItemLabel(Pt_Temp, is_active);				
			}
		}
	}
	else
	{
	//	if(SystemParaMenu2.Usermenu_SW)
		{
			if(SystemParaMenu2.TemTest_SW > 0)
			{
				SystemParaMenu2.TemTest_SW --;
				if(SystemParaMenu2.TemTest_SW == 0)
				{
					sprintf((ts_char*)addBuffer, "%s", "OFF");
				}
				else if(SystemParaMenu2.TemTest_SW == 1)
				{
					sprintf((ts_char*)addBuffer, "%s", "INT");
				}
				else if(SystemParaMenu2.TemTest_SW == 2)
				{
					sprintf((ts_char*)addBuffer, "%s", "Sen");
				}
				else if(SystemParaMenu2.TemTest_SW == 3)
				{
					sprintf((ts_char*)addBuffer, "%s", "All");
				}
				else if(SystemParaMenu2.TemTest_SW == 4)
				{
//					AutoGOF_Flag = 0;
					sprintf((ts_char*)addBuffer, "%s", "NAG");
				}
//				if(SystemParaMenu2.TemTest_SW<4)	{AutoGOF_Flag = 1;}
			}
			else
			{
				SystemParaMenu2.TemTest_SW = 0;
				sprintf((ts_char*)addBuffer, "%s", "OFF");
			}
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
	vision_log_debug("SystemParaMenu2.TemTest_SW = %d\n", SystemParaMenu2.TemTest_SW);
}
/** @note 切换UVC终端输出数据格式*/
void menu2_templut_UVCFormat(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
	ts_u08 uvc_setFormat = SystemParaMenu2.m_uvc_output_format;

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.m_uvc_output_format = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.m_uvc_output_format);

	if (state == 0) {
		if (uvc_setFormat) {
			uvc_setFormat = UVC_YUV_FORMAT_UYVY;
			sprintf((ts_char*)addBuffer, "%s", "UYVY");
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "YUYV");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1) {
		if (uvc_setFormat == UVC_YUV_FORMAT_YUYV) {
			uvc_setFormat = UVC_YUV_FORMAT_UYVY;
			sprintf((ts_char*)addBuffer, "%s", "UYVY");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
	else {
		if (uvc_setFormat) {
			uvc_setFormat = UVC_YUV_FORMAT_YUYV;
			sprintf((ts_char*)addBuffer, "%s", "YUYV");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
	if (state) {
		if (uvc_setFormat != SystemParaMenu2.m_uvc_output_format) {
			SystemParaMenu2.m_uvc_output_format = uvc_setFormat;
			ts_char tip_info[64];
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			snprintf(tip_info, 64, "Please power off/on for %s", addBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
		}
	}
	vision_log_debug("m_uvc_output_format = %d, uvc_setFormat = %d\n", SystemParaMenu2.m_uvc_output_format, uvc_setFormat);
}
/** @note 切换 Sensor IIC地址 */
void menu2_templut_Sensor_Sel(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.TemTest_SW = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.TemTest_SW);

	if (state == 0) {
		if(SystemParaMenu2.sensor) {
			sprintf((ts_char*)addBuffer, "%s", "GC");
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "JK");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(state == 1) {
		if(SystemParaMenu2.sensor == 0) {
			SystemParaMenu2.sensor = 1;
			Sensor_Init();
			sprintf((ts_char*)addBuffer, "%s", "GC");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}	
	}
	else {
		if(SystemParaMenu2.sensor) {
			SystemParaMenu2.sensor = 0;
			Sensor_Init();
			sprintf((ts_char*)addBuffer, "%s", "JK");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}	
	}
}
/** @note 菜单二用户测温显示模式切换 */
void menu2_templut_OutputType(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	ts_u08 menuName_Len = 0;
	ts_u08 addBuffer[32] = {0};
	ts_u08 tmp_FrameMode = SystemParaMenu2.m_user_tempFrame_Mode;
	ts_u08 old_FrameMode = tmp_FrameMode;
    /* ===== 局部静态字符串表 ===== */
    static const char * const FrameMode_str[] = {
        "ORAD",
        "LTAD",
        "TEMP",
        "CAAD"
    };
    /* ---------- 非法值兜底 ---------- */
    if (tmp_FrameMode > TEMP_CALIBRATION_DISP_AD) {
        tmp_FrameMode = TEMP_NORMAL_DISP_LUT_AD;
    }
	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, m_user_tempFrame_Mode = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.m_user_tempFrame_Mode);

	if (state == 0) {
        sprintf((ts_char *)addBuffer, "%s", FrameMode_str[tmp_FrameMode]);
        memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((const char *)addBuffer) + 1);
        Display_MenuItemLabel(Pt_Temp, is_active);
	}
	/* ---------- 正向：CAAD → TEMP → LTAD → ORAD ---------- */
	else if (state == 1) {
        if (tmp_FrameMode > 0) {
            tmp_FrameMode--;
        }

        sprintf((ts_char *)addBuffer, "%s", FrameMode_str[tmp_FrameMode]);
        memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((const char *)addBuffer) + 1);
        Display_MenuItemLabel(Pt_Temp, is_active);
	}
	/* ---------- 反向：ORAD → LTAD → TEMP ---------- */
	else {
        if (tmp_FrameMode < 2) {
            tmp_FrameMode++;
        }

        sprintf((ts_char *)addBuffer, "%s", FrameMode_str[tmp_FrameMode]);
        memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((const char *)addBuffer) + 1);
        Display_MenuItemLabel(Pt_Temp, is_active);
	}
	if (old_FrameMode != tmp_FrameMode) {
        SystemParaMenu2.m_user_tempFrame_Mode = tmp_FrameMode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
        vision_log_debug("SystemParaMenu2.m_user_tempFrame_Mode changed: %d -> %d\n", old_FrameMode, tmp_FrameMode);
	}
	else {
        vision_log_debug("SystemParaMenu2.m_user_tempFrame_Mode unchanged (%d)\n", tmp_FrameMode);		
	}
}
#if  ((Fire_Alarm ==1)||(MYFirePoint_Alarm==1))
void menu2_NUC_AlarmAD(Point_MenuStructure temp_node3,ts_u08 state)
{
	if(menu2_Val_SET_ts_u16(temp_node3,state,&SystemParaMenu2.AlarmAD,0,0xFFFF)){
		APB3_REGW_AlarmAD(SystemParaMenu2.AlarmAD);
	}
}

void menu2_NUC_AlarmNum(Point_MenuStructure temp_node3,ts_u08 state)
{
	if(menu2_Val_SET_ts_u32(temp_node3,state,&SystemParaMenu2.AlarmNum,0,0xFFFFFFFF))
	{
#ifdef Fire_Alarm
		APB3_REGW_AlarmNum(SystemParaMenu2.AlarmNum);
#endif
	}
}
#endif
/**
 * @brief:显示测试菜单的参数
 * @parameter:Pt_T6:指向当前选中菜单列表项的指针
 * @parameter:Status:0:初始化显示参数,1:参数++,2:参数--
 */
void DisplayTestmenuItemParameter(Point_MenuStructure Pt_Temp7,ts_u08 Status,ts_u08 is_active)
{
	/********************************************************************************/
	/*
	 * 以下每一项函数对应每一个菜单的功能函数
	 * 以下将每个功能函数单独列出来的原因：
	 * 1、为了后面方便维护.
	 * 2、增加代码的可读性.
	 * 3、此种做法,无疑是增加了Flash的开销,增加了代码量,是比较冗余的写法,考虑到外部Flash容量够大,所以不考虑对Flash的影响.(牺牲空间换便利)
	 */
	/********************************************************************************/
	switch(Pt_Temp7->Info.ID)
	{

#ifdef CONFIG_SNR_PLANCK1280C
	//光智1280探测器Det参数在菜单二显示
	case 0x61://"Gain:"
			Vot1280_GetAndDisplayGainParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x62://"GOF:"
			Vot1280_GetAndDisplayGofParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x63://"Cint:"
			Vot1280_GetAndDisplayCintParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x64://"Nint:"
			GetAndDisplayNintParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x65://"Sel_PJ"
			GetAndDisplaySel_PJ_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x66://"NTCC"
			GetAndDisplayNTCC_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x67://"Voccm:"
			GetAndDisplayVoccm_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x68://"Config:"
			GetAndDisplaydetConfig_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x69://"Vfid"
			GetAndDisplayVfid_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x6A://"DetInit:"
			if(Status){
				DetParInit(Status);
				UpdatePromptInfoWithTimeMs("Det init finished!", 2500,VOT_LOG_TYPE_MENU2);
			}
			break;
		case 0x6B://"Mode:"
			GetAndDisplayModeParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x6C://"Calc:"
			GetAndDisplayGosCalcParameter(Pt_Temp7,Status);
			if(Status){
				UpdatePromptInfoWithTimeMs("Det calc finished!", 2500,VOT_LOG_TYPE_MENU2);
			}
			break;

#else		
		case 0x61://"Gain:"
			GetAndDisplayGainParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x62://"GOF:"
			GetAndDisplayGofParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x63://"Cint:"
			GetAndDisplayCintParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x64://"Tint:"
			GetAndDisplayTintParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x65://"GOS:"或"OCC_Par"
			GetAndDisplayGosParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x66://,"NdacL:"//此参数只有光智探测器版本会执行
			GetAndDisplayNDAC_LParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x67://"NdacH:"//此参数只有光智探测器版本会执行
			GetAndDisplayNDAC_HParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x68://"DetInit:"
			if(Status){
				DetParInit(Status);
				UpdatePromptInfoWithTimeMs("Det init finished!", 2500,VOT_LOG_TYPE_MENU2);
			}
			break;
		case 0x69://"Mode:"
			GetAndDisplayModeParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x6A://"Calc:"
			GetAndDisplayGosCalcParameter(Pt_Temp7,Status);
			if(Status){
				UpdatePromptInfoWithTimeMs("Det calc finished!", 2500,VOT_LOG_TYPE_MENU2);
			}
			break;

#endif

		case 0x71://"OCC_Mode:"
			//GetAndDisplayOCCModeParameter(Pt_Temp7,Status);
			break;
		case 0x72://"OCC_Fix:"
			//GetAndDisplayOCCFixParameter(Pt_Temp7,Status);
			break;
		case 0x73://"OCC_Init"
			OCC_Init(Status);
			break;
		case 0x74://"OCC_Calc"
			OCC_Calculate(Status);
			break;
		case 0x75://"OCC_Save"
			OCC_Save(Status);
			break;
		case 0x76://"OCC_b"
			OCC_c_and_b(Status);
			break;
		case 0x79://"OCC_way"
			//GetAndDisplayOCCwayOpr(Pt_Temp7,Status);
			break;

	/**********************AutoFocus******************/
		// case 0x7A://"Zoom_Lens"
		// 	GetAndDisplayZoomLensParameter(Pt_Temp7,Status);
		// 	break;
		// case 0x7B:
		// 	GetAndDisplayLeftValueParameter(Pt_Temp7,Status);
		// 	break;
		// case 0x7C:
		// 	GetAndDisplayRightValueParameter(Pt_Temp7,Status);
		// 	break;
		case 0x7D:
			GetAndDisplayOperatorSpeedParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x7E:
			if(Status){
				FocusParInit(Status);
				UpdatePromptInfoWithTimeMs("FocusPara init finished!", 2500,VOT_LOG_TYPE_MENU2);
			}		
			break;
		// case 0x7F: //"FullSpeed"
		// 	GetAndDisplayFocusFullSpeedParameter(Pt_Temp7,Status,is_active);
		// 	break;	
		case 0x7D1: //"Foucs_DownNumSet"
			GetAndDisplayFoucs_DownNumSetParameter(Pt_Temp7,Status,is_active);
			break;	
		case 0x7D2: //"Focus_UpNumSet"
			GetAndDisplayFocus_UpNumSetParameter(Pt_Temp7,Status,is_active);
			break;	
		case 0x7D3: //"Table_Sel"
			GetAndDisplayCalibration_ScaleParameter(Pt_Temp7,Status,is_active);
			break;	
		case 0x7D4: //"ADDR_Sel"
			GetAndDisplayADDR_SelParameter(Pt_Temp7,Status,is_active);
			break;			
	/*************************************************/

		case 0x81://"NUC_SW"
			GetAndDisplayNUC_SWParameter(Pt_Temp7,Status, is_active);
			break;
		case 0x82://"Init"
			NUC_Init(Status);
			break;
		case 0x83://"Calc"
			NUC_Calc(Status);
			break;
		case 0x84://"Save"
			NUC_Save(Status);
			break;
		//坏点校准
		case 0xA1://"BAD_SW:"
			GetAndDisplayBAD_SWParameter(Pt_Temp7,Status,is_active);
			
			break;
		case 0xA2://"BAD_Manual:"
			// 非初始化
			if(Status!=0){
				EnterOrExitBPCMaualMode(0);
			}
			break;
		case 0xA3://"BAD_Mode:"
			GetAndDisplayBAD_ModeParameter(Pt_Temp7,Status, is_active);
			break;
		case 0xAA://"Semi:"
			GetAndDisplayAutoSimiParameter(Pt_Temp7,Status,is_active);
			break;

		case 0xab: //"bpc  cfg"
			GetAndDisplayBpcCfgParameter(Pt_Temp7,Status,is_active);
			break;
		
		case 0xac: //"bpc view"
			GetAndDisplayBpcViewParameter(Pt_Temp7,Status,is_active);
			break;

		case 0xad: //"bpc_ext_SW"
			GetAndDisplaybpc_ext_SWParameter(Pt_Temp7,Status,is_active);
			break;
		case 0xae: //"bpc_ext_Init"
			Bpc_ext_Init(Status);
		case 0xaf: //"bpc_ext_Del"
			Bpc_ext_Del(Status);
			break;
		
		// case 0xA4://"Init"
		// 	Bad_Init(Status);
		// 	break;
		//system 
		case 0xB1://"Para_Init"
			InitializeTestmenuParameter(Status);
			break;
		case 0xB2://"Para_Save"
			SaveTestmenuParameter(Status);
			break;	
		case 0xBE://"usb output mode"
			GetDisplayOutputVideoMode(Pt_Temp7,Status,is_active);
			break;
		case 0xBD://"fps"
			GetDisplayDeviceFpsMode(Pt_Temp7,Status,is_active);
			break;
		case 0xB7://"raw_yuv"
			GetDeviceRawYuvModeMode(Pt_Temp7,Status,is_active);
			break; 
/***************************Misc**********************************///
		case 0x111://" cvbs enlarge  mode"
			GetCvbsDisplayEnlargeMode(Pt_Temp7,Status,is_active);
			break;
		case 0x112://" uvc enlarge  mode"
			GetUvcDisplayEnlargeMode(Pt_Temp7,Status,is_active);
			break;
		case 0x113://" force detector type "
			GetForceDectorType(Pt_Temp7,Status,is_active);
			break;
		case 0x114://" MIPI Lanes "
			GetMipiLanesParameter(Pt_Temp7,Status,is_active);
			break;
		case 0x115://" force hardware port type "
			GetForceHardwarePortType(Pt_Temp7,Status,is_active);
			break;
		case 0x116://" force customer id  "
			GetForceCustomerId(Pt_Temp7,Status,is_active);
			break;
		case 0x117://" force lens focus type "
			GetLensFocusType(Pt_Temp7,Status,is_active);
			break;
		case 0x118://" force debug log "
			GetDebugLogEn(Pt_Temp7,Status,is_active);
			break;
		// case 0x119://" get chip type "
		// 	GetChipType(Pt_Temp7,Status,is_active);
		// 	break;
		case 0x119://" 超分配置"
			SetNPU_SR(Pt_Temp7,Status,is_active);
			break;	
		case 0x905://" select temp regularity"
			Set_TEMP_REGULARITY(Pt_Temp7,Status,is_active);
			break;		

/*****************************************************************/
		case 0x141: {//m_st0_total_duration_s			
			GetDisplayST0_total_duration(Pt_Temp7,Status,is_active);
			break;
		}
		case 0x142: {//m_st0_interval_s
			GetDisplayST0_interval(Pt_Temp7,Status,is_active);
			break;
		}
		case 0x131: {//ST1_M			
			GetDisplayST1_M(Pt_Temp7,Status,is_active);
			break;
		}
		case 0x132: {//ST1_S
			GetDisplayST1_S(Pt_Temp7,Status,is_active);
			break;
		}
		case 0xB6: {//DET_ST
			GetDisplayDET_ST(Pt_Temp7,Status,is_active);
			break;
		}
		case 0x101://"sffc switch "
			GetAndDisplaySFFCSwitchStatus(Pt_Temp7,Status,is_active);
			break;
		case 0x102://"sffc calc "
			#ifndef VOT_NUC_SFFC_TEST
				sffc_calculate(Status);
			#endif
			break;
		case 0x104://"sffc refresh "
			#ifndef VOT_NUC_SFFC_TEST
				SFFC_Refresh(Status);
			#else
				sffc_nr_refresh(Status);
			#endif
			break;
		case 0x103://"ee mode "
			GetDisplayEeEnhanceMode(Pt_Temp7,Status,is_active);
			break;
		case 0x105://"fix snc  "
			GetDisplayFixSncSwitch(Pt_Temp7,Status,is_active);
			break;

		case 0x121://" temp drift comp mode "
			GetDisplayTDCMode(Pt_Temp7,Status,is_active);
			break;
		case 0x122://"temp drift comp calibration index  "
			GetDisplayTDCCalibrationIndex(Pt_Temp7,Status,is_active);
			break;
		case 0x123://"temp drift comp calibration  "
			GetDisplayTDCCalibrationOperation(Pt_Temp7,Status,is_active);
			break;
		case 0x124://"temp drift comp aisp  "
			GetDisplayTDCAispMode(Pt_Temp7,Status,is_active);
			break;

/************************阳光保护*********************************/
		case 0x89://"SunPT:"
			menu2_NUC_SunPT_SW(Pt_Temp7,Status,is_active);
			break;
		case 0x8A://"S_RG:"
			menu2_NUC_SunPT_RanGe(Pt_Temp7,Status,is_active);
			break;
		case 0x8B://"S_Val:"
			menu2_NUC_SunPT_Val(Pt_Temp7,Status,is_active);
			break;
		case 0x8C://"P_Time:"
			menu2_NUC_Protect_Time(Pt_Temp7,Status,is_active);
			break;
/***************************温度帧**********************************/
		case 0x8D://"Temp_SW"
			menu2_templut_TempSW_Switch(Pt_Temp7,Status,is_active);
			break;
		case 0x8E:
			menu2_templut_ADT_Param(Pt_Temp7,Status,is_active);
			break;
		case 0x8F:
			menu2_templut_Calib_State(Pt_Temp7,Status,is_active);
			/* 为了改变菜单二上的显示不匹配问题 */
			menu2_templut_ADT_Param(Pt_Temp7->Previous, 0, 0);
			break;
		case 0x900:
			getAndDisplay_TemTestSW_Parameter(Pt_Temp7,Status,is_active);
			break;
		case 0x901:
			menu2_templut_TempSW_Limit(Pt_Temp7,Status,is_active);
			break;
		case 0x902:// Sensor iic 地址切换
			menu2_templut_Sensor_Sel(Pt_Temp7,Status,is_active);
			break;
		case 0x903:// 控制温度矩阵输出原始AD/测温AD/查表温度
			menu2_templut_OutputType(Pt_Temp7,Status,is_active);
			break;
		case 0x904:
			menu2_templut_UVCFormat(Pt_Temp7,Status,is_active);
			break;
/*************************************************************/

#if 0
		case 0x85://"Sel:"
			GetAndDisplayNUC_FlagParameter(Pt_Temp7,Status);
			break;

#ifdef OPR_ALARM
		case 0x86://"Alarm:"
			menu2_NUC_Alarm_SW(Pt_Temp7,Status);
			break;
		case 0x87://"A_Calc:"
			menu2_NUC_range_SW(Pt_Temp7,Status);
			break;
		case 0x88://"A_Val:"
			menu2_NUC_Val_SW(Pt_Temp7,Status);
			break;
#ifdef AnFangProject
		case 0x8D0:
		case 0x8D1:
		case 0x8D2:
		case 0x8D3:
			GetAndDisplayA_AreasParameter(Pt_Temp7,Status);
			break;
		case 0x8E:
			GetAndDisplayAF_ValParameter(Pt_Temp7,Status);
			break;
#endif

	#endif

#if  ((Fire_Alarm ==1)||(MYFirePoint_Alarm==1))
		case 0x86://"AlarmAD:"
			menu2_NUC_AlarmAD(Pt_Temp7,Status);
			break;
		case 0x87://"AlarmNum:"
			menu2_NUC_AlarmNum(Pt_Temp7,Status);
			break;
#endif

		case 0x92://"Gamma:"
			GetAndDisplayGammaParameter(Pt_Temp7,Status);
			break;
		case 0x93://"Plat_NX:"
			GetAndDisplay_Plat_NX(Pt_Temp7,Status);
			break;
		case 0x94://"FK8_SW:"
			GetAndDisplay_FK8_SW(Pt_Temp7,Status);
			break;
		case 0x9B://"DDE_SW:"
			GetAndDisplayDDE_SWParameter(Pt_Temp7,Status);
			break;
		case 0x95://"DDE_LV:"
			GetAndDisplayDDELeverParameter(Pt_Temp7,Status);
			break;
		case 0x96://"TFliter:"
			GetAndDisplayTFilterParameter(Pt_Temp7,Status);
			break;
		case 0x9C://"SFliter_SW:"
			//GetAndDisplaySFilter_SWParameter(Pt_Temp7,Status);
			GetAndDisplayAGC_AFliterParameter(Pt_Temp7,Status);
			break;
		case 0x97://"SFliter:"
			GetAndDisplaySFilterParameter(Pt_Temp7,Status);
			break;
		case 0x98://"V_DeSt:"
			GetAndDisplayDEST_SWParameter(Pt_Temp7,Status);
			break;
		case 0x99://"H_DeSt:"
			GetAndDisplayHOST_SWParameter(Pt_Temp7,Status);
			break;
		case 0x9A://"Gama_Tab:"
			GetAndDisplayGama_TabParameter(Pt_Temp7,Status);
			break;
		case 0x9D://"BF_SW:"
			GetAndDisplay_BF_SW(Pt_Temp7,Status);
			break;
		case 0x9E:
			GetAndDisplaySFDDE_SWParameter(Pt_Temp7,Status);
			break;
		case 0x9F:
			GetAndDisplayAD_SetOPR(Pt_Temp7,Status);
			break;

		case 0xA1://"BAD_SW:"
			GetAndDisplayBAD_SWParameter(Pt_Temp7,Status);
			break;
		case 0xA2://"BAD_Cro:"
			GetAndDisplayBAD_CroParameter(Pt_Temp7,Status);
			break;
		case 0xA3://"Semi:"
			GetAndDisplayAutoSimiParameter(Pt_Temp7,Status);
			break;
		case 0xA4://"Init"
			Bad_Init(Status);
			break;
#ifdef SUPPORT_VOT_DETECTOR
		case 0xA5://"DTMY_SW:"
			GetAndDisplayDTMY_SWParameter(Pt_Temp7,Status);
			break;
		case 0xA6://"DTMY_Init"
			DynamicBlindPixelInit(Status);
			break;
#endif
		case 0xA7://"MaxTempAlarmSW"
			GetAndDisplayMaxT_AlarmSwitchParameter(Pt_Temp7,Status);
			break;
		case 0xA8://"MaxTempAlarmLimit"
			GetAndDisplayMaxT_AlarmLimitParameter(Pt_Temp7,Status);
			break;
		case 0xA9://"PixelNum"
			GetAndDisplayPixelLimitParameter(Pt_Temp7,Status);
			break;

		case 0xaa://"piexloffset"
			GetAndDisplayPixelOffsetParameter(Pt_Temp7,Status);
			break;

		case 0xab://"piexlfunction"
			GetAndDisplayPixelAutoFunctionParameter(Pt_Temp7,Status);
			break;


		case 0xB3://"Temp_TD:"
			GetAndDisplayTemp_TDParameter(Pt_Temp7,Status);
			break;
		case 0xB4://"FFT1:",
			GetAndDisplayFFT1Parameter(Pt_Temp7,Status);
			break;
		case 0xB5://"FFT2:"
			GetAndDisplayFFT2TParameter(Pt_Temp7,Status);
			break;
		case 0xB6://"Temp:"
			GetAndDisplayTemperatureSwitchParameter(Pt_Temp7,Status);
			break;
		case 0xB7://"AD_T:"
			GetAndDisplayAD_TempParameter(Pt_Temp7,Status);
			break;
		case 0xB8://"Tem_T:"
			GetAndDisplayTemTest_SWParameter(Pt_Temp7,Status);
			break;
#ifdef 	TEST_SAVE_Image
		case 0xB9:// Img_num
			DisplayImgNum(Pt_Temp7,Status);
			break;
		case 0xbA://Img_Save
			DisplayImgNumSave(Pt_Temp7,Status);
			break;
#else
#if WLP
		case 0xB9://Lens
			GetAndDisplayLensParameter(Pt_Temp7,Status);
			break;
#else
		case 0xB9://No_Shutter
			GetAndDisplayNoShutterParameter(Pt_Temp7,Status);
			break;
#endif
#endif
		case 0xBB://"Sensor:"
			GetAndDisplaySensorParameter(Pt_Temp7,Status);
			break;
		case 0xBC://"ShutFun:"
			GetAndDisplayShutFunParameter(Pt_Temp7,Status);
			break;
		case 0x910://"Mode:"
			GetAndDisplayAGC_SETParameter(Pt_Temp7,Status);
			break;
		case 0x911://"Scene:"
			GetAndDisplayAGC_AutoParameter(Pt_Temp7,Status);
			break;
		case 0x912://"Outdoor"
			GetAndDisplayAGC_OutdoorOPR(Pt_Temp7,Status);
			break;
		case 0x913://"Bri_A"
			GetAndDisplayAGC_AutoBrightParameter(Pt_Temp7,Status);
			break;
		case 0x914://"Ctrs_A:"
			GetAndDisplayAGC_AutoContrastParameter(Pt_Temp7,Status);
			break;
		case 0x915://"Bri_M:"
			GetAndDisplayAGC_ManualBrightParameter(Pt_Temp7,Status);
			break;
		case 0x916://"Ctrs_M:"
			GetAndDisplayAGC_ManualContrastParameter(Pt_Temp7,Status);
			break;
		case 0x917://" AFliter:"
			//GetAndDisplayAGC_AFliterParameter(Pt_Temp7,Status);
			break;
		case 0x918://" RanLim:"
			GetAndDisplayAGC_RangeLimitParameter(Pt_Temp7,Status);
			break;
		case 0x919://" PlaLim:"
			GetAndDisplayAGC_PlatLimitParameter(Pt_Temp7,Status);
			break;
		case 0x920://" AGC_SW:"
			DisplayAGC_SW(Pt_Temp7,Status);
			break;
		case 0x921://" Alpha:"
			GetAndDisplayAlphaParameter(Pt_Temp7,Status);
			break;
		case 0x922://" JLEPS:"
			GetAndDisplayJLEPSParameter(Pt_Temp7,Status);
			break;

#endif
		default:
			break;
	}
}


void Update_Display_TestMenuItem(Point_MenuStructure Pt_T0, ts_u08 number, unsigned short ID )
{
	vision_log_debug("Update_Display_TestMenuItem SelectionMenu.MenuState=0x%02x, number=%d, ID=%d\n", SelectionMenu.MenuState, number, ID);
	if(Pt_T0 == NULL)
	{
		return;
	}
	else
	{
		for(ts_u08 i=0;i<number;i++)
		{
			vision_log_debug("i=%d,Pt_T0=%p number=%d, number_active=0x%x\n", i, Pt_T0, number, ID);

			if(Pt_T0->Info.ID ==ID){
				DisplayTestmenuItemParameter(Pt_T0,0,1);
			}
			else{
				DisplayTestmenuItemParameter(Pt_T0,0,0);
			}

			Pt_T0 = Pt_T0->Next;
		}
	}
}