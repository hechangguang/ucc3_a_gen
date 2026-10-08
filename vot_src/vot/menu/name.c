/*
 * name.c
 *
 *  Created on: 2024年2月27日
 *      Author: ZJ
 */

#include"name.h"

const char *EnlargeTab[4]       = {"OFF","X2","X4","X8"};
const char *CaptureMaxTab[4]    = {"Max_X:","Max_Y:","Max_AD:","Max_T:"};
const char *CaptureMinTab[4]    = {"Min_X:","Min_Y:","Min_AD:","Min_T:"};
const char *CaptureParameter[4] = {"OFF","MIN","MAX","MMX"};
const char *ShutterParameter[4] = {"OFF","30S","60S","90S"};
const char *MirrorParameter[4]  = {"OFF","X","Y","X_Y"};
const char *SwitchTab[2]  	   = {"OFF","ON"};


const char *SceneTab[4]         = {"Manl","Indo","Outd"};
const char *SceneOpr[4]         = {"GD","M3","JL"};

const char *InitTab[][2] =
{
	{":初始化",":Init"},
	{":完成"," :OK "},
};

const char *SaveTab[][2] =
{
	{":保存中",":Save"},
	{":完成"," :OK "},
};

const char *BrightContrastTab[][3] =
{
	{"自动","Auto"},
};

const char *Deviceinfo[][3] =
{
	{"序列号:","Serial:","Serl:"},
	{"镜头信息:","Lens:","Lens:"},
	{"生产日期:","Data:","Data:"},
	{"DET_ID:","DET_ID","DetID"},
};

const char *LanguageNameTab[][3] =
{
	{"中文","En","En"},
};

const char *FontColorNameTab[][3] =
{
	{"灰色","Gray"},
	{"白色","White"},
	{"动态","Vary"},
	{"背景","BG"},
};

const char *ShutterModeNameTab[][3] =
{
	{"OFF","OFF","OFF"},
	{"定时","Cycle","Cycle"},
	{"温度","Temperature","Temp"},
};

const char *PaletteNameTab[][3] =
{
	{"白热","White","White"},
	{"黑热","Black","Black"},
	{"铁红","Iron","Iron"},
	{"反铁红","IronIN","IronIN"},
	{"彩虹","Rainbow","Rainbow"},
	{"红热","RedHot","RedHot"},
	{"熔岩","Lava","Lava"},
	{"羽红","RainRed","RainRed"},
	{"春羽红","SRainRed","SRainRed"},
	{"琥珀","Amber","Amber"},
	{"铁灰","IronGray","IronGray"},
	{"蓝黑","BBlack","BBlack"},
	{"蓝红","BuleRed","BuleRed"},
	{"多色","MultiCol","MultiCol"},
	{"RGB反色","IN_RGB","IN_RGB"},
	{"铁红T","IronT","IronT"},
	
};

const char *ModeNameTab[][3] =
{
	{"线性模式","Linear",""},
	{"常温模式","Ordinary",""},
	{"低温模式","Low",""},
	{"高温模式","High",""},
};

const char *ImageAdjustNameTab[][3] =
{
	{"手动","Manual",""},
	{"室内","Indoor",""},
	{"自动","Auto",""},
	{"室外","Outdoor",""},
};

const char *UsermenuNameTab1[][3] =	{
	{"图像","Image","Image"},
	{"测温","Measurement","Measu"},
	{"系统","System","System"},
	{"调节","Adjust","Adjust"},
};

const char *UsermenuNameTab2[][3] =	{
//	{"场景:","Scene:"},
//	{"模式:","Mode:"},
	{"亮度:","Bright:","Bri:"},
	{"对比度:","Contrast:","Ctr:"},
	{"伪彩:","Palette:"," "},
	{"电子放大:","D-zoom:","zoom:"},
	{"阳光保护","SunPt","SunPt"},
	{"中心光标:","Crosshair:","Crshr:"},
	{"PIP:","PIP:","PIP:"},
	{"HST:","HST:","HST:"}, //Hot Spot Tracking
};

const char *UsermenuNameTab3[][3] =	{
	{"测温点","Spot","Spot"},
	{"测温区域","Area","Area"},
	{"捕获:","Capture:","Cap:"},
	{"测温参数","Tem_Para","TemPar"},
	{"测温档位:","Tem_SW:","T_SW:"},
	{"四边形:","Quad_SW:","Q_SW:"},
	{"AlarmSet:","AlarmSet:","ASet:"},
};
const char *UsermenuNameTab4[][3] =	{
	{"语言:","Language:","Lang:"},
	//{ "字符颜色:","FontColor:"},
	{"镜像:","Mirror:","Mir:"},
	// "挡片模式:","ShutterMode:",
	{"自动挡片:","Shutter:","Sht:"},
	{"保存设置","Save","Save"},
	{"默认设置","Default","Default"},
	{"系统信息","Information","Info"},
	// {"阳光保护:","SunProtect:"},
	// {"保护时间:","ProtectTime:"},
};
const char *UsermenuNameTab5[][3] =	{
	{"图像增强:","DDE_LV:","dde:"},
	{"空间滤波:","SFilter:","sf:"},
	{"场景","Scene:",""},
//	{"中心点:","CentSpot:"},
};
const char *UsermenuNameTab6[][3] =	{
	{"测温点1:","Spot1:","Spot1:"},
	{"测温点2:","Spot2:","Spot2:"},
	{"测温点3:","Spot3:","Spot3:"},
	{"测温点4:","Spot4:","Spot4:"},
	{"测温点5:","Spot5:","Spot5:"},
	{"测温点6:","Spot6:","Spot6:"},
	{"测温点7:","Spot7:","Spot7:"},
	{"测温点8:","Spot8:","Spot8:"},
	{"测温点9:","Spot9:","Spot9:"},
};
const char *UsermenuNameTab7[][3] =	{
	{"测温区域1:","Area1:","Area1:"},
	{"测温区域2:","Area2:","Area2:"},
	{"测温区域3:","Area3:","Area3:"},
	{"测温区域4:","Area4:","Area4:"},
	{"测温区域5:","Area5:","Area5:"},
};

const char *UsermenuNameTab8[][3] =	{
	{"辐射率:","Emissivity:","EMT:"},
	{"反射温度:","Reflect:","RFT:"},
	{"环境温度:","Humidity:","HDT:"},
	{"相对湿度:","RH:","RH:"},
	{"距离:","Distance:","DST:"},
	{"镜头设置:","Lens:","Len:"},
};

const char *UsermenuNameTab9[][3] =	{
	{"模式:","Mode:","Md:"},
	{"定时:","Time:","Tm:"},

};


const char *TestmenuNameTab1[][3] =	{
	{"Algorithm","Algorithm","Alg"},
	{"OCC","OCC","NUC_OCC"},
	{"NUC","NUC","DET"},
	{"DET","DET","BPC"},
	{"BPC","BPC","Sys"},
	{"System","System",""},
};

const char *TestmenuNameTab2[][3] =	{
#ifdef CONFIG_SNR_PLANCK1280C
    {"Gain:","Gain:","Gain:"},
	{"GOF:","GOF:","GOF:"},
	{"Cint:","Cint:","Cint:"},
	{"Nint:","Nint:","Nint:"},
	{"Sel_PJ:","Sel_PJ:","Sel_PJ:"},
	{"NTCC:","NTCC:","NTCC:"},
	{"Voccm:","Voccm:","Voccm:"},
	{"Config:","Config:","Config:"},
	{"Vfid:","Vfid:","Vfid:"},
#else	
	{"Gain:","Gain:","Gain:"},
	{"GOF:","GOF:","GOF:"},
	{"Cint:","Cint:","Cint:"},
	{"Tint:","Tint:","Tint:"},
	{"GOS:","GOS:","GOS:"},
#ifdef SUPPORT_VOT_DETECTOR
	{"NdacL:","NdacL:","NdL:"},
	{"NdacH:","NdacH:","NdH:"},
#endif
#endif
	{"Init","Init","Init"},
#if DET_Change_Para
	{"Mode:","Mode:",""},
	{"Calc","Calc","Calc"},
#endif
};

const char *TestmenuNameTab4[][3] =	{
	// {"OCC_Mode:","OCC_Mode:"},
	// {"OCC_Fix:","OCC_Fix:"},
	// {"OCC_way:","OCC_way:"},
	{"Init","Init","OccInit"},
	{"Calc","Calc","OccCal"},
	{"Save","Save","OccSav"},
	{"Occ_b","Occ_b","Occb"},
	// {"Zoom_L:","Zoom_L:"},
	// {"L_Val:","L_Val:"},
	// {"R_Val:","R_Val:"},
	{"Speed:","Speed:","Spd:"},
	{"Focus_Init","Focus_Init","FsInt"},
	// {"FuSpeed:","FuSpeed:","FSpd:"},
	{"DNumSet:","DNumSet:","DNSet:"},
	{"UNumSet:","UNumSet:","UNSet:"},
	{"TabSel:","TabSel:","TaSel:"},
	{"ADDRSel:","ADDRSel:","ADDR:"},
};

const char *TestmenuNameTab5[][3] =	{
	{"NUC_SW:","NUC_SW:","N_SW:"},
	// {"Sel:","Sel:"},
	{"Init","Init","N_Init"},
	{"Calc","Calc","N_Calc"},
	{"Save","Save","N_Save"},
#ifdef OPR_ALARM
	{"Alarm:","Alarm:"},
	{"A_Calc:","A_Calc:"},
	{"A_Val:","A_Val:"},
	#ifdef AnFangProject
	{"A_Area","A_Area"},
	{"AF_Val:","AF_Val:"},
	#endif
#endif

#if  ((Fire_Alarm ==1)||(MYFirePoint_Alarm==1))
	{"A_AD:","A_AD:"},
	{"A_Num:","A_Num:"},
#endif
};
const char *TestmenuNameTab6[][3] =	{


	{"SFFC:","SFFC:","Sf:"},
	{"SFFC_Calc","SFFC_Calc","SfCal"},
	{"SFFC_Refr","SFFC_Refr","SfRf"},
	{"TempDC","TempDC","TpDC"},
	{"EE_Mode","EE_Mode","EeMd"},
	{"Fix_Snc","Fix_Snc","FSnc"},
	// "Effect:","Effect:",
	// "DDE_Pre:","DDE_Pre:",
	// "DDE_SW:","DDE_SW:",
	// "DDE_LV:","DDE_LV:",
	// "Gama_pre:","Gama_pre:",
	// "GamTAB:","GamTAB:",
	// "TFilter:","TFilter:",
	// "SF_LV:","SF_LV:",
};

const char *TestmenuNameTab7[][3] =	{
	{"BAD_Cro","BAD_Cro","BadCro"},
	{"BAD_SW:","BAD_SW:","BpSW:"},
	{"Mode:","Mode:","Md:"},
 	{"Semi_A:","Semi_A:","SemA:"},
	{"Cfg","Cfg","Cfg"},
	{"View:","View:","View:"},
	{"Ext_SW:","Ext_SW:","Ext:"},
	{"Ext_Init","Ext_Init","ExtInit"},
	{"Ext_Del","Ext_Del","ExtDel"},
// 	"PixOfst:","PixOfst:",
// 	"PixAuto","PixAuto",
// 	"Init","Init",
// #ifdef SUPPORT_VOT_DETECTOR
// 	"DTMY_SW:","DTMY_SW:",
// 	"DTMY_Init","DTMY_Init",
// #else
// 	#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
// 		"T_AlSW:","T_AlSW:",
// 		"T_AlLmt:","T_AlLmt:",
// 		"PixLmt:","PixLmt:",
// 	#endif
// #endif

};

const  char *TestmenuNameTab8[][3] =	{
	{"Para_Init","Para_Init","ParInt"},
	{"Para_Save","Para_Save","ParSav"},
	{"","",""},
	{"Misc","Misc","Misc"},
	{"Temp_Meas", "Temp_Meas","T_Meas"},
	{"Temp_Opt","Temp_Opt","T_Opt"},
	{"Sun_Prot","Sun_Prot","SunProt"},
	{"FPS","FPS","FPS"},
	{"ST1","ST1","ST1"},
	{"ST0","ST0","ST0"},
	{"DET_ST","DET_ST","DSt"},
	{"T8","T8","T8"},

#if(No_Shutter == 1)
	"Shutter:","Shutter:",
#endif

#ifdef WLP
	"Lens:","Lens:",
#endif

#ifdef  TEST_SAVE_Image
	"Img_num:","Img_num:",
	"Img_Save","Img_Save:",
#endif

};

const  char *TestmenuNameTab9[][3] =	{
	{"AGC_SW:","AGC_SW:"},
	{"Mode:","Mode:"},
	{"Scene:","Scene:"},
	{"Bri_A:","Bri_A:"},
	{"Ctrs_A:","Ctrs_A:"},
	{"Bri_M:","Bri_M:"},
	{"Ctrs_M:","Ctrs_M:"},
	{"RngLim:","RngLim:"},
	{"PlaLim:","PlaLim:"},
	{"Alpha:","Alpha:"},
	{"JLEPS:","JLEPS:"},
};

const char *TestmenuNameTab10[][3] =
{
#ifdef AnFangProject
	"Area1:","Area1:",
	"Area2:","Area2:",
	"Area3:","Area3:",
	"Area4:","Area4:",
#endif
};

const char *TestmenuNameTab11[][3] =
{
	{"Switch:","Switch:","SW:"},
	{"Limit:","Limit:","Lmt:"},
	{"Num:","Num:","Num:"},
	{"Time_S:","Time_S:","TIM:"},
};

const char *TestmenuNameTab12[][3] =
{
	{"V_DeSt:","V_DeSt:"},
	{"H_DeSt:","H_DeSt:"},
	{"TF8_LV:","TF8_LV:"},
	{"FK8_SW:","FK8_SW:"},
	{"Plat_NX:","Plat_NX:"},
};

const char *TestmenuNameTab13[][3] =
{
	{"AD_Set:","AD_Set:"},
	{"IR_TD:","IR_TD:"},
	{"Temp_TD:","Temp_TD:"},
	{"Test1:","Test1:"},
};

const char *TestmenuNameTab14[][3] =
{
	{"Temp_SW:","Temp_SW:","TSW:"},
	{"AD/T:","AD/T:","ADT:"},
	{"Calib:","Calib:","Cab:"},
	{"Temp_T:","Temp_T:","Tp_T:"},
	{"T_Limit:","T_Limit:","TLmt:"},
};

const char *TestmenuNameTab15[][3] =
{
	{"ShutFun:","ShutFun:","ShtF:"},
	{"SADDR:","SADDR:","SADR:"},
	{"DType:","DType:","DT:"},
	{"UVFmt:","UVFmt:","UF:"},
	{"T_RUL:","T_RUL:","TR:"},
};


const char *TestmenuNameTab16[][3] =
{
	{"CVBS","CVBS","CVBS"},
	{"UVC","UVC","UVC"},
	{"MIPI","MIPI","MIPI"},
	{"",""," "},
	{"",""," "},
	{"Cid","Cid","C"},
	{"Lens","Lens","L"},
	{"Log","Log","Log"},
	{"",""," "},
};

const char *TestmenuNameTab17[][3] =
{
	{"Mode","Mode","Md"},
	{"Aisp","Aisp","Ai"},
	{"Index:","Index:","Idx:"},
	{"TDCali","TDCali","TDCali"},
};

const char *TestmenuNameTab18[][3] =
{
	{"ST1_M","ST1_M","S1M"},
	{"ST1_S","ST1_S","S1S:"},
};

const char *TestmenuNameTab19[][3] =
{
	{"ST0_S","ST0_S","S0S"},
	{"ST0_S","ST0_S","S0S:"},
};
