/*
 * testmenufunction.c
 *
 *  Created on: 2024年3月18日
 *      Author: Administrator
 */

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
#include <menu/testmenufunction.h>
#include <menu/usermenufunction.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <menu/usermenu.h>
#include <snr_comm_vot.h>
#include <fontlib_vot.h>
#include <uapi/snr_v100.h>
#include <common/common.h>
#include <menu/votmenu.h>

extern ts_u32 det_gos_switch_force_last;
extern SysCtrlInfo_s_t global_sys_ctrl_info;

extern  ts_mdev_t shdl_osd_dev;

#ifdef No_Shutter
	ts_u08 NoShutter_ExtFlag = 0;
#endif
/********************************************************************************/
/*
 * 以下每一项函数对应每一个菜单的功能函数
 * 以下将每个功能函数单独列出来的原因：
 * 1、为了后面方便维护.
 * 2、增加代码的可读性.
 * 3、此种做法,无疑是增加了Flash的开销,增加了代码量,是比较冗余的写法,考虑到外部Flash容量够大,所以不考虑对Flash的影响.(牺牲空间换便利)
 */
/********************************************************************************/

#ifdef CONFIG_SNR_PLANCK1280C
/**
 * @brief:获得以及显示：Gain参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void Vot1280_GetAndDisplayGainParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	//vision_log_debug("GetAndDisplayGainParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.GAIN=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.GAIN);
	//读回当前最新gain值
	vision_log_debug(" planck1280c_read_gain_adc %d \n",planck1280c_read_gain_adc());
	
	if(state == 0)
	{
//		display_u8number(Xline,Yline,SystemParaMenu2.GAIN,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.GAIN1280++;
		if(SystemParaMenu2.GAIN1280 >0x7f)
			SystemParaMenu2.GAIN1280 = 0x7f;
	}
	else
	{
		if(SystemParaMenu2.GAIN1280 >0)
		{
			SystemParaMenu2.GAIN1280--;

		}
		else
			SystemParaMenu2.GAIN1280 = 0;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.GAIN1280);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
	}
}

/**
 * @brief:获得以及显示：GOF参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void Vot1280_GetAndDisplayGofParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	//vision_log_debug("GetAndDisplayGofParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.GOF=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.GOF);
	if(state == 0)
	{
//		display_u8number(Xline,Yline,SystemParaMenu2.GOF,3,CharacterColor);
	}
	else
	{
		if(state ==1)
		{
			SystemParaMenu2.GOF1280++;
			if(SystemParaMenu2.GOF1280 > 0x7f)
				SystemParaMenu2.GOF1280 = 0x7f;
		}
		else
		{
			if(SystemParaMenu2.GOF1280>0)
			{
				SystemParaMenu2.GOF1280--;
			}
			else
				SystemParaMenu2.GOF1280 = 0;
		}
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.GOF1280);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_gof(SystemParaMenu2.GOF1280);
	}
}

/**
 * @brief:获得以及显示：INT_C参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void Vot1280_GetAndDisplayCintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	//vision_log_debug("GetAndDisplayCintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.INT_C=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.INT_C);
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.INT_C,3,CharacterColor);
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.Cint1280 < 7)
		{
			SystemParaMenu2.Cint1280++;

		}else{
			SystemParaMenu2.Cint1280 = 7;
		}
	}
	else
	{
		if(SystemParaMenu2.Cint1280 > 0){
			SystemParaMenu2.Cint1280--;
		}else{
			SystemParaMenu2.Cint1280 = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Cint1280);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_cint(SystemParaMenu2.Cint1280);
	}
}

/**
 * @brief:获得以及显示：Nint参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayNintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	//vision_log_debug("GetAndDisplayCintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.Nint=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.Nint);

	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.Nint,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.Nint < 2047){
			SystemParaMenu2.Nint++;
		}else{
			SystemParaMenu2.Nint = 2047;
		}
	}else{
		if(SystemParaMenu2.Nint > 0){
			SystemParaMenu2.Nint--;
		}else{
			SystemParaMenu2.Nint = 0;
		}
	}
	display_append_u16number((ts_u16*)&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Nint);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_Nint(SystemParaMenu2.Nint);
	}
	// vision_log_debug(" SystemParaMenu2.INT_C %d \n",SystemParaMenu2.INT_C);
}

/**
 * @brief:获得以及显示：Sel_PJ参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaySel_PJ_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.Sel_PJ,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.Sel_PJ < 1){
			SystemParaMenu2.Sel_PJ++;
		}else{
			SystemParaMenu2.Sel_PJ = 1;
		}
	}else{
		if(SystemParaMenu2.Sel_PJ > 0){
			SystemParaMenu2.Sel_PJ--;
		}else{
			SystemParaMenu2.Sel_PJ = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Sel_PJ);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
	}
}

/**
 * @brief:获得以及显示：NTCC参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayNTCC_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.NTCC,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.NTCC < 127){
			SystemParaMenu2.NTCC++;
		}else{
			SystemParaMenu2.NTCC = 127;
		}
	}else{
		if(SystemParaMenu2.NTCC > 0){
			SystemParaMenu2.NTCC--;
		}else{
			SystemParaMenu2.NTCC = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.NTCC);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_NTCC(SystemParaMenu2.NTCC);
	}
}

/**
 * @brief:获得以及显示：Voccm参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayVoccm_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.Voccm,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.Voccm < 63){
			SystemParaMenu2.Voccm++;
		}else{
			SystemParaMenu2.Voccm = 63;
		}
	}else{
		if(SystemParaMenu2.Voccm > 0){
			SystemParaMenu2.Voccm--;
		}else{
			SystemParaMenu2.Voccm = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Voccm);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
	}
}

/**
 * @brief:获得以及显示：detConfig参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaydetConfig_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.detConfig,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.detConfig < 1){
			SystemParaMenu2.detConfig++;
		}else{
			SystemParaMenu2.detConfig = 1;
		}
	}else{
		if(SystemParaMenu2.detConfig > 0){
			SystemParaMenu2.detConfig--;
		}else{
			SystemParaMenu2.detConfig = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.detConfig);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_detConfig(SystemParaMenu2.detConfig);
	}
}

/**
 * @brief:获得以及显示：Vfid参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayVfid_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.Vfid,3,CharacterColor);
	}else if(state ==1){
		if(SystemParaMenu2.Vfid < 127){
			SystemParaMenu2.Vfid++;
		}else{
			SystemParaMenu2.Vfid = 127;
		}
	}else{
		if(SystemParaMenu2.Vfid > 0){
			SystemParaMenu2.Vfid--;
		}else{
			SystemParaMenu2.Vfid = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Vfid);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck1280c_write_Vfid(SystemParaMenu2.Vfid);
	}
}
#endif

/**
 * @brief:获得以及显示：Gain参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayGainParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayGainParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.GAIN=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.GAIN);
	//读回当前最新gain值
	// SystemParaMenu2.GAIN = planck640c_read_gain_adc();
	vision_log_debug(" planck640c_read_gain_adc %d \n",planck640c_read_gain_adc());
	if(state == 0)
	{
//		display_u8number(Xline,Yline,SystemParaMenu2.GAIN,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.GAIN++;
		if(SystemParaMenu2.GAIN >0x7f)
			SystemParaMenu2.GAIN = 0x7f;
	}
	else
	{
		if(SystemParaMenu2.GAIN >0)
		{
			SystemParaMenu2.GAIN--;

		}
		else
			SystemParaMenu2.GAIN = 0;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.GAIN);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_gain_adc(SystemParaMenu2.GAIN);
	}

}

/**
 * @brief:获得以及显示：GOF参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayGofParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayGofParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.GOF=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.GOF);
	if(state == 0)
	{
//		display_u8number(Xline,Yline,SystemParaMenu2.GOF,3,CharacterColor);
	}
	else
	{
	 if(state ==1)
	{
#ifdef IRAY_AutoGOFTest
		if(AutoGOF_Flag)	return;
#endif
		SystemParaMenu2.GOF++;
		if(SystemParaMenu2.GOF > 0x7f)
			SystemParaMenu2.GOF = 0x7f;
		else
		{
//			write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.GOF,3,CharacterColor);
		}
	}
	else
	{
#ifdef IRAY_AutoGOFTest
		if(AutoGOF_Flag)	return;
#endif
		if(SystemParaMenu2.GOF>0)
		{
			SystemParaMenu2.GOF--;
//			write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.GOF,3,CharacterColor);
		}
		else
			SystemParaMenu2.GOF = 0;
	}
		if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
		{
			SystemParaMenu2.GOF_Val[0] = SystemParaMenu2.GOF ;
		}
		else if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
		{
			SystemParaMenu2.GOF_Val[1] = SystemParaMenu2.GOF;
		}
		else if(SystemParaMenu2.Usermenu_SW == 3) { //三档测温
			SystemParaMenu2.GOF_Val[2] = SystemParaMenu2.GOF;
		}
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.GOF);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_gof(SystemParaMenu2.GOF);
	}
}

/**
 * @brief:获得以及显示：INT_C参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayCintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayCintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.INT_C=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.INT_C);

	if(state == 0){
		// display_u8number(Xline,Yline,SystemParaMenu2.INT_C,3,CharacterColor);
	}else if(state ==1){
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode != 4) return;
//#endif

		if(SystemParaMenu2.INT_C < 7){
			SystemParaMenu2.INT_C++;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.INT_C,3,CharacterColor);
		}else{
			SystemParaMenu2.INT_C = 7;
		}
	}else{
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode != 4) return;
//#endif

		if(SystemParaMenu2.INT_C > 0){
			SystemParaMenu2.INT_C--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.INT_C,3,CharacterColor);
		}else{
			SystemParaMenu2.INT_C = 0;
		}
	}

	if(state){
		if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
		{
			SystemParaMenu2.INT_C_Val[0] = SystemParaMenu2.INT_C ;
		}
		else if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
		{
			SystemParaMenu2.INT_C_Val[1] = SystemParaMenu2.INT_C;
		}
		else if(SystemParaMenu2.Usermenu_SW == 3) { //三档测温
			SystemParaMenu2.INT_C_Val[2] = SystemParaMenu2.INT_C;
		}
	}

#if DET_Change_Para
	switch(SystemParaMenu2.DetMode){
	default:break;
	case 1:SystemParaMenu2.LowTemp.INT_C = SystemParaMenu2.INT_C; 		break;
	case 2:SystemParaMenu2.NormalTemp.INT_C = SystemParaMenu2.INT_C;    break;
	case 3:SystemParaMenu2.HighTemp.INT_C = SystemParaMenu2.INT_C;		break;
	case 4:SystemParaMenu2.offChange.INT_C = SystemParaMenu2.INT_C;		break;
	}
	
#endif

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.INT_C);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_cint(SystemParaMenu2.INT_C);
	}
	// vision_log_debug(" SystemParaMenu2.INT_C %d \n",SystemParaMenu2.INT_C);
}

/**
 * @brief:获得以及显示：INT_T参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayTintParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayTintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.INT_T=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.INT_T);
	if(state == 0){
//		display_u8number(Xline,Yline,SystemParaMenu2.INT_T,3,CharacterColor);
	}else if(state ==1){
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode != 4) return;
//#endif

		if(SystemParaMenu2.INT_T < 76){
			SystemParaMenu2.INT_T++;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.INT_T,3,CharacterColor);
		}else{
			SystemParaMenu2.INT_T = 76;
		}
	}else{
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode != 4) return;
//#endif

		if(SystemParaMenu2.INT_T > 0){
			SystemParaMenu2.INT_T--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));
//			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.INT_T,3,CharacterColor);
		}else{
			SystemParaMenu2.INT_T = 0;
		}
	}

	if(state){
		if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
		{
			SystemParaMenu2.INT_T_Val[0] = SystemParaMenu2.INT_T ;
		}
		else if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
		{
			SystemParaMenu2.INT_T_Val[1] = SystemParaMenu2.INT_T;
		}
		else if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
		{
			SystemParaMenu2.INT_T_Val[2] = SystemParaMenu2.INT_T;
		}
	}

#if DET_Change_Para
	switch(SystemParaMenu2.DetMode){
	default:break;
	case 1:SystemParaMenu2.LowTemp.INT_T = SystemParaMenu2.INT_T; 		break;
	case 2:SystemParaMenu2.NormalTemp.INT_T = SystemParaMenu2.INT_T;    break;
	case 3:SystemParaMenu2.HighTemp.INT_T = SystemParaMenu2.INT_T;		break;
	case 4:SystemParaMenu2.offChange.INT_T = SystemParaMenu2.INT_T;		break;
	}
#endif

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.INT_T);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_tint(SystemParaMenu2.INT_T);
		planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
	}
	// vision_log_debug(" SystemParaMenu2.INT_T %d \n",SystemParaMenu2.INT_T);
}

/**
 * @brief:获得以及显示：NDAC_L参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayNDAC_LParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;
	// Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	// Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayTintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.NDAC_L=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.NDAC_L);
	if(state == 0)
	{
		// display_u8number(Xline,Yline,SystemParaMenu2.NDAC_L,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.NDAC_L++;
		if(SystemParaMenu2.NDAC_L > 31)
		{
			SystemParaMenu2.NDAC_L = 31;
		}
		else
		{
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffffc1ff)|(SystemParaMenu2.NDAC_L<<9)));
			// DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			// display_u8number(Xline,Yline,SystemParaMenu2.NDAC_L,3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.NDAC_L>0)
		{
			SystemParaMenu2.NDAC_L--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffffc1ff)|(SystemParaMenu2.NDAC_L<<9)));
			// DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			// display_u8number(Xline,Yline,SystemParaMenu2.NDAC_L,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.NDAC_L = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.NDAC_L);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_ndac((SystemParaMenu2.NDAC_H<<5)|SystemParaMenu2.NDAC_L);
	}
}

/**
 * @brief:获得以及显示：NDAC_H参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayNDAC_HParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;
	// Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	// Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayTintParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.NDAC_H=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.NDAC_H);
	if(state == 0)
	{
		// display_u8number(Xline,Yline,8-(Change_NdacValue(SystemParaMenu2.NDAC_H^0xff)+1),3,CharacterColor);
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.NDAC_H == 0)
		{
			SystemParaMenu2.NDAC_H = 128;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));
			// DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			// display_u8number(Xline,Yline,1,3,CharacterColor);
		}
		else if((SystemParaMenu2.NDAC_H<255)&&(SystemParaMenu2.NDAC_H))
		{
			SystemParaMenu2.NDAC_H = SystemParaMenu2.NDAC_H+(1<<Change_NdacValue(SystemParaMenu2.NDAC_H^0xff));
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));
			// DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			// display_u8number(Xline,Yline,8-(Change_NdacValue(SystemParaMenu2.NDAC_H^0xff)+1),3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.NDAC_H = 255;
		}
	}
	else
	{
		if(SystemParaMenu2.NDAC_H>0)
		{
			SystemParaMenu2.NDAC_H = SystemParaMenu2.NDAC_H-(1<<(Change_NdacValue(SystemParaMenu2.NDAC_H^0xff)+1));
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));
			// DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			// display_u8number(Xline,Yline,8-(Change_NdacValue(SystemParaMenu2.NDAC_H^0xff)+1),3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.NDAC_H = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.NDAC_H);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_ndac((SystemParaMenu2.NDAC_H<<5)|SystemParaMenu2.NDAC_L);
	}
}

/**
 * @brief:获得以及显示：Gos或者OCC_Par参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayGosParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayGosParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.OOC_Par=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.OOC_Par);
	if(state == 0){
//		display_u8number(Xline,Yline,SystemParaMenu2.OOC_Par,3,CharacterColor);
	}else if(state ==1){
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode == 4 || SystemParaMenu2.DetMode == 2){}
//		else return;
//#endif

		if(SystemParaMenu2.OOC_Par < 63){
			SystemParaMenu2.OOC_Par++;
//			write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
//			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Par,3,CharacterColor);
		}else{
			SystemParaMenu2.OOC_Par = 63;
		}
	}else{
//#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
//		if(SystemParaMenu2.DetMode == 4 || SystemParaMenu2.DetMode == 2){}
//		else return;
//#endif

		if(SystemParaMenu2.OOC_Par > 0){
			SystemParaMenu2.OOC_Par--;
//			write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
//			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Par,3,CharacterColor);
		}else{
			SystemParaMenu2.OOC_Par = 0;
		}
	}

	if(state){
		if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
		{
			SystemParaMenu2.OOC_Par_Val[0] = SystemParaMenu2.OOC_Par ;
		}
		else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
		{
			SystemParaMenu2.OOC_Par_Val[1] = SystemParaMenu2.OOC_Par;
		}
		else if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
		{
			SystemParaMenu2.OOC_Par_Val[2] = SystemParaMenu2.OOC_Par;
		}
	}

#if DET_Change_Para
	switch(SystemParaMenu2.DetMode){
	default:break;
	case 1:SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par; 		break;
	case 2:
		//SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 8;
		SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
		//SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 4;
		break;
	case 3:SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par;		break;
	case 4:SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;		break;
	}
#endif

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.OOC_Par);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state!=0){
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
	}
	// vision_log_debug(" SystemParaMenu2.OOC_Par %d \n",SystemParaMenu2.OOC_Par);
}

/**
 * @brief:获得以及显示：OCCMode参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayOCCModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.OOC_Mode,3,CharacterColor);
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.OOC_Mode==0)
		{
			SystemParaMenu2.OOC_Mode = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffbf)|(SystemParaMenu2.OOC_Mode<<6)));
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Mode,3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.OOC_Mode==1)
		{
			SystemParaMenu2.OOC_Mode = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffbf)|(SystemParaMenu2.OOC_Mode<<6)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Mode,3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：OCCFix参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayOCCFixParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.OOC_Fix,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.OOC_Fix++;
		if(SystemParaMenu2.OOC_Fix > 63)
		{
			SystemParaMenu2.OOC_Fix = 63;
		}
		else
		{
			if(SystemParaMenu2.OOC_Mode==0)
			{
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
			}
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Fix,3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.OOC_Fix>0)
		{
			SystemParaMenu2.OOC_Fix--;
			if(SystemParaMenu2.OOC_Mode==0)
			{
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
			}
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.OOC_Fix,3,CharacterColor);
		}
		else
			SystemParaMenu2.OOC_Fix = 0;
	}
}

void GetAndDisplaySFFCSwitchStatus(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplaySFFCSwitch Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.m_sffc_mode=%d\n", Pt_Temp, state, MenuNameLen,SystemParaMenu2.m_sffc_mode);

	if(state == 0)
	{

	}
	else if(state == 1)
	{
		if(SystemParaMenu2.m_sffc_mode< SFFC_MODE_MANUAl)
		{
			SystemParaMenu2.m_sffc_mode++;
		}
		else{
			SystemParaMenu2.m_sffc_mode = SFFC_MODE_MANUAl;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.m_sffc_mode>0)
		{
			SystemParaMenu2.m_sffc_mode--;
		}
		else
		{
			SystemParaMenu2.m_sffc_mode = SFFC_MODE_OFF;
		}
	}

	if(SystemParaMenu2.m_sffc_mode == SFFC_MODE_OFF)
	{
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "OFF", 4);
		// Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(SystemParaMenu2.m_sffc_mode == SFFC_MODE_AUTO)
	{
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "Auto", 4);
		// Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(SystemParaMenu2.m_sffc_mode == SFFC_MODE_MANUAl)
	{
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "Manu", 5);
	
	}

	if(state ){
		sffc_on_off(SystemParaMenu2.m_sffc_mode , OPERATION_MODE_NONE);
	}

	Display_MenuItemLabel(Pt_Temp, is_active);

}


void GetDisplayOutputVideoMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayOutputVideoMode state=%u SystemParaMenu2.m_video_out_mode %d \n", state,SystemParaMenu2.m_video_out_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  usb_video_out_mode = SystemParaMenu2.m_video_out_mode;
	if(usb_video_out_mode>=VIDEO_OUTPUT_MODE_ENUM_BUTT){
		usb_video_out_mode = VIDEO_OUTPUT_MODE_ENUM_CVBS;
	}

	vision_log_debug("GetDisplayOutputVideoMode state=%u usb_video_out_mode %d  %d\n", state,usb_video_out_mode,SystemParaMenu2.m_video_out_mode);

	if(state){	
	
		if(state==1){
			if(usb_video_out_mode<(VIDEO_OUTPUT_MODE_ENUM_BUTT-1)){
				usb_video_out_mode++;
			}
		}
		else if(state==2){
			if(usb_video_out_mode>0){
				usb_video_out_mode--;
			}
		}
		
	}

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	if(res_type==DECTOR_RESOLUTION_384X288){
		if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG){
			sprintf((ts_char*)AddBuffer, "%s", "debug");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC){
			sprintf((ts_char*)AddBuffer, "%s", "UVC");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_UVC;
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656){
			sprintf((ts_char*)AddBuffer, "%s", "BT656");
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601){
			sprintf((ts_char*)AddBuffer, "%s", "BT601");
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI){
			sprintf((ts_char*)AddBuffer, "%s", "MIPI");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "CVBS");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		}
	}
	else{
		if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG){
			sprintf((ts_char*)AddBuffer, "%s", "USB:debug");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC){
			sprintf((ts_char*)AddBuffer, "%s", "USB:UVC");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_UVC;
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656){
			sprintf((ts_char*)AddBuffer, "%s", "Out:BT656");
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601){
			sprintf((ts_char*)AddBuffer, "%s", "Out:BT601");
		}
		else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI){
			sprintf((ts_char*)AddBuffer, "%s", "Out:MIPI");
		}
		// else if(usb_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_LIBUSB){
		// 	sprintf((ts_char*)AddBuffer, "%s", "USB:libusb");
		// 	SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		// }
		else{
			sprintf((ts_char*)AddBuffer, "%s", "Out:CVBS");
			SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		}
	}




	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);


	if(state){
		
		if(SystemParaMenu2.m_video_out_mode != usb_video_out_mode){
			SystemParaMenu2.m_video_out_mode = usb_video_out_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
	
		if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_UVC ){
			UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
		}
		else if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG ){
			UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
		}
		// else if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB ){
		// 	UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
		// }
		else {
			UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
		}
	}
	vision_log_debug("GetDisplayOutputVideoMode state=%u usb_video_out_mode %d  %d\n", state,usb_video_out_mode,SystemParaMenu2.m_video_out_mode);

}


void GetDisplayDeviceEnvMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayDeviceEnvMode state=%u UsermenuParameter.m_dev_env_mode %d \n", state,UsermenuParameter.m_dev_env_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  dev_env_mode = UsermenuParameter.m_dev_env_mode;
	if(dev_env_mode>=DEVICE_MODE_BUTT){
		dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
	}

	vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

	if(state){	
	
		if(state==1){
			if(dev_env_mode<(DEVICE_MODE_BUTT-1)){
				dev_env_mode++;
			}
		}
		else if(state==2){
			if(dev_env_mode>0){
				dev_env_mode--;
			}
		}
	}

	if(dev_env_mode == DEVICE_MODE_OUTDOOR){
		sprintf((ts_char*)AddBuffer, "%s", ":outdoor");
	}
	else if(dev_env_mode == DEVICE_MODE_INDOOR){
		sprintf((ts_char*)AddBuffer, "%s", ":indoor");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(UsermenuParameter.m_dev_env_mode != dev_env_mode){
			UsermenuParameter.m_dev_env_mode = dev_env_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			reset_usr_isp_cfg();
		}
	}
	vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

}

void GetDisplayDeviceFpsMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayDeviceFpsMode state=%u SystemParaMenu2.m_device_fps %d \n", state,SystemParaMenu2.m_device_fps);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_device_fps = SystemParaMenu2.m_device_fps;
	if(m_device_fps>=DEVICE_FPS_MODE_BUTT){
		m_device_fps = DEVICE_FPS_MODE_50FPS;
	}

	if(m_device_fps==DEVICE_FPS_MODE_40FPS || m_device_fps==DEVICE_FPS_MODE_30FPS || m_device_fps==DEVICE_FPS_MODE_20FPS){
		m_device_fps = DEVICE_FPS_MODE_50FPS;
	}

	vision_log_debug("GetDisplayDeviceFpsMode state=%u m_device_fps %d  %d\n", state,m_device_fps,SystemParaMenu2.m_device_fps);

	//对于定制时序，帧率不可以调节
	if(SystemParaMenu2.m_bt_time_sequece_mode == BT_TIMING_SEQ_MODE_WIN2_50HZ){
		m_device_fps = DEVICE_FPS_MODE_50FPS;
		state = 0;
	}
	else if(SystemParaMenu2.m_bt_time_sequece_mode == BT_TIMING_SEQ_MODE_WIN2_25HZ){
		m_device_fps = DEVICE_FPS_MODE_25FPS;
		state = 0;
	}

	if(state){	
	
		if(state==1){
			if(m_device_fps<(DEVICE_FPS_MODE_BUTT-1)){
				m_device_fps++;
				//过滤掉无效的帧率
				if(m_device_fps==DEVICE_FPS_MODE_40FPS){
					m_device_fps = DEVICE_FPS_MODE_25FPS;
				}
				else if(m_device_fps==DEVICE_FPS_MODE_20FPS){
					m_device_fps = DEVICE_FPS_MODE_10FPS;
				}
			}
		}
		else if(state==2){
			if(m_device_fps>0){
				m_device_fps--;
				//过滤掉无效的帧率
				if(m_device_fps==DEVICE_FPS_MODE_20FPS){
					m_device_fps = DEVICE_FPS_MODE_25FPS;
				}
				else if(m_device_fps==DEVICE_FPS_MODE_30FPS){
					m_device_fps = DEVICE_FPS_MODE_50FPS;
				}
			}
		}
	}

	if(m_device_fps == DEVICE_FPS_MODE_50FPS){
		sprintf((ts_char*)AddBuffer, "%s", ":50");
	}
	else if(m_device_fps == DEVICE_FPS_MODE_25FPS){
		sprintf((ts_char*)AddBuffer, "%s", ":25");
	}
	else if(m_device_fps == DEVICE_FPS_MODE_10FPS){
		sprintf((ts_char*)AddBuffer, "%s", ":10");
	}
	else if(m_device_fps == DEVICE_FPS_MODE_5FPS){
		sprintf((ts_char*)AddBuffer, "%s", ":5");
	}


	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_device_fps != m_device_fps){
			SystemParaMenu2.m_device_fps = m_device_fps;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}

	}
	vision_log_debug("GetDisplayDeviceFpsMode state=%u m_device_fps %d  %d\n", state,m_device_fps,SystemParaMenu2.m_device_fps);

}



void GetDeviceRawYuvModeMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDeviceRawYuvModeMode state=%u SystemParaMenu2.m_raw_yuv_mode %d \n", state,SystemParaMenu2.m_raw_yuv_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  raw_yuv_mode = SystemParaMenu2.m_raw_yuv_mode;
	if(raw_yuv_mode>=RAW_YUV_MODE_BUTT){
		raw_yuv_mode = RAW_YUV_MODE_NONE;
	}


    //自定义时序和raw+yuv只能二选一
    if(SystemParaMenu2.m_bt_time_sequece_mode !=BT_TIMING_SEQ_MODE_NONE && raw_yuv_mode != RAW_YUV_MODE_NONE){
        raw_yuv_mode = RAW_YUV_MODE_NONE;
		state = 0;
		UpdatePromptInfoWithTimeMs(" T8 Mode is illegal for win2's bt656/bt610 timing !", 1500,VOT_LOG_TYPE_MENU2);
    }

	vision_log_debug("GetDeviceRawYuvModeMode state=%u m_device_fps %d  %d\n", state,raw_yuv_mode,SystemParaMenu2.m_raw_yuv_mode);

	if(state){	
	
		if(state==1){
			if(raw_yuv_mode<(RAW_YUV_MODE_BUTT-1)){
				raw_yuv_mode++;
			}
		}
		else if(state==2){
			if(raw_yuv_mode>0){
				raw_yuv_mode--;
			}
		}
	}

	if(raw_yuv_mode == RAW_YUV_MODE_NONE){
		sprintf((ts_char*)AddBuffer, "%s", ":NONE");
	}
	else if(raw_yuv_mode == RAW_YUV_MODE_RAW_YUV){
		sprintf((ts_char*)AddBuffer, "%s", ":R_Y");
	}
	else if(raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
		sprintf((ts_char*)AddBuffer, "%s", ":Y_R");
	}
	else {
		sprintf((ts_char*)AddBuffer, "%s", ":NONE");
	}


	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_raw_yuv_mode != raw_yuv_mode){
			SystemParaMenu2.m_raw_yuv_mode = raw_yuv_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
			
		}

	}
	vision_log_debug("GetDeviceRawYuvModeMode state=%u raw_yuv_mode %d  %d\n", state,raw_yuv_mode,SystemParaMenu2.m_raw_yuv_mode);

}

void GetCvbsDisplayEnlargeMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
    ts_u08 AddBuffer[32];
    
    ts_u08 cvbs_display_mode = SystemParaMenu2.m_cvbs_display_mode;
    if (cvbs_display_mode >= DEVICE_CVBS_DISPLAY_MODE_BUTT) {
        cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
    }

    vision_log_debug("GetCvbsDisplayEnlargeMode enter: state=%u, current_mode=%d\n", state, cvbs_display_mode);

    /* 2. 最高优先级：Usermenu_SW 有效时强制关闭，否则允许切换 */
    if (SystemParaMenu2.Usermenu_SW) {
        /* Usermenu_SW 有效：无论何种情况强制 ORG(OFF)，且不响应 state 切换 */
        cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
    } else {
        /* Usermenu_SW 无效：允许 state 触发 ++/-- 切换 */
        if (state == 1) {
            if (cvbs_display_mode < (DEVICE_CVBS_DISPLAY_MODE_BUTT - 1)) {
                cvbs_display_mode++;
            }
        } else if (state == 2) {
            if (cvbs_display_mode > 0) {
                cvbs_display_mode--;
            }
        }
    }

    if (cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE) {
        snprintf((ts_char *)AddBuffer, sizeof(AddBuffer), ":ON");
    } else {
        snprintf((ts_char *)AddBuffer, sizeof(AddBuffer), ":OFF");
    }
    
    MenuNameLen = strlen((ts_char *)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
    strcpy((ts_char *)&Pt_Temp->Info.NameAdd[MenuNameLen], (ts_char *)AddBuffer);
    Display_MenuItemLabel(Pt_Temp, is_active);

    if (state && (SystemParaMenu2.m_cvbs_display_mode != cvbs_display_mode)) {
        SystemParaMenu2.m_cvbs_display_mode = cvbs_display_mode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
        
        if (cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE) {
            UpdatePromptInfoWithTimeMs("Enlarge ON mode !", 1000, VOT_LOG_TYPE_MENU2);
        } else {
            UpdatePromptInfoWithTimeMs("Enlarge OFF mode !", 1000, VOT_LOG_TYPE_MENU2);
        }
        
        cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
        cvbs_osd_refresh_display(1);
        getDisplay_Crosshair_Switch();
    }

    vision_log_debug("GetCvbsDisplayEnlargeMode exit: state=%u, final_mode=%d\n", state, cvbs_display_mode);
}

void GetUvcDisplayEnlargeMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];

    ts_u08 m_uvc_enlarger_mode = SystemParaMenu2.m_uvc_enlarger_mode;
    if (m_uvc_enlarger_mode >= OUTPUT_MODE_ENLARGE_BUTT) {
        m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }
    vision_log_debug("GetUvcDisplayEnlargeMode enter: state=%u, current_mode=%d\n", state, m_uvc_enlarger_mode);

	dector_resolution_type_t res_type = get_powerup_dector_resolution_type();
    if (SystemParaMenu2.Usermenu_SW) {
        /* Usermenu_SW 有效：无论什么分辨率，一律强制关闭 */
        m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }
    else if (res_type == DECTOR_RESOLUTION_384X288) {
        /* Usermenu_SW 无效 + 384x288：允许切换 */
        if (state == 1) {
            if (m_uvc_enlarger_mode < (OUTPUT_MODE_ENLARGE_BUTT - 1)) {
                m_uvc_enlarger_mode++;
            }
        } else if (state == 2) {
            if (m_uvc_enlarger_mode > 0) {
                m_uvc_enlarger_mode--;
            }
        }
    }
    else {
        /* Usermenu_SW 无效 + 非384x288（640x512等其他分辨率）：强制关闭 */
        m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }
    if (m_uvc_enlarger_mode == OUTPUT_MODE_ENLARGE_ON) {
        snprintf((ts_char *)AddBuffer, sizeof(AddBuffer), ":ON");
    } else {
        snprintf((ts_char *)AddBuffer, sizeof(AddBuffer), ":OFF");
    }
    MenuNameLen = strlen((ts_char *)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
    strcpy((ts_char *)&Pt_Temp->Info.NameAdd[MenuNameLen], (ts_char *)AddBuffer);
	Display_MenuItemLabel(Pt_Temp, is_active);

    if (state && (SystemParaMenu2.m_uvc_enlarger_mode != m_uvc_enlarger_mode)) {
        SystemParaMenu2.m_uvc_enlarger_mode = m_uvc_enlarger_mode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
        
        if (m_uvc_enlarger_mode == OUTPUT_MODE_ENLARGE_ON) {
            UpdatePromptInfoWithTimeMs("UVC Enlarge ON mode !", 1000, VOT_LOG_TYPE_MENU2);
        } else {
            UpdatePromptInfoWithTimeMs("UVC Enlarge OFF mode !", 1000, VOT_LOG_TYPE_MENU2);
        }
        
        vot_uvc_usr_reset();
        getDisplay_Crosshair_Switch();
        vot_refresh_osd_bind();
    }

    vision_log_debug("GetUvcDisplayEnlargeMode exit: state=%u, final_mode=%d\n", state, m_uvc_enlarger_mode);	
}

void GetForceHardwarePortType(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetForceHardwarePortType state=%u SystemParaMenu2.m_force_hardware_port_type %d \n", state,SystemParaMenu2.m_force_hardware_port_type);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_force_hardware_port_type = SystemParaMenu2.m_force_hardware_port_type;
	if(m_force_hardware_port_type>=FORCE_HARDWARE_PORT_TYPE_BUTT){
		m_force_hardware_port_type = FORCE_HARDWARE_PORT_TYPE_UAD3_LITE;
	}

	vision_log_debug("GetForceHardwarePortType state=%u m_force_hardware_port_type %d  %d\n", state,m_force_hardware_port_type,SystemParaMenu2.m_force_hardware_port_type);

	if(m_force_hardware_port_type!=FORCE_HARDWARE_PORT_TYPE_NMOD){
		if(state){	
		
			if(state==1){
		
				if(m_force_hardware_port_type<(FORCE_HARDWARE_PORT_TYPE_NMOD-1)) 
				{
					m_force_hardware_port_type++;
				}
			}
			else if(state==2){
				if(m_force_hardware_port_type>0){
					m_force_hardware_port_type--;
				}
			}
		}
	}

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    if(res_type==DECTOR_RESOLUTION_384X288){
		if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3_LITE){
			sprintf((ts_char*)AddBuffer, "%s", "UAD3Li");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3){
			sprintf((ts_char*)AddBuffer, "%s", "UAD3");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW){
			sprintf((ts_char*)AddBuffer, "%s", "UAW");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW_LITE){
			sprintf((ts_char*)AddBuffer, "%s", "UAWLi");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "P_NMOD");
		}
	}
	else{
		if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3_LITE){
			sprintf((ts_char*)AddBuffer, "%s", "UAD3_LITE");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3){
			sprintf((ts_char*)AddBuffer, "%s", "UAD3");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW){
			sprintf((ts_char*)AddBuffer, "%s", "UAW");
		}
		else if(m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW_LITE){
			sprintf((ts_char*)AddBuffer, "%s", "UAW_LITE");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "PORT_NMOD");
		}
	}

	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_force_hardware_port_type != m_force_hardware_port_type){
			SystemParaMenu2.m_force_hardware_port_type = m_force_hardware_port_type;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for force %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

		// vot_vpo_freeze();
		// // 软件主动断开电源，保护探测器
		// planck640c_stop();
		// // 自动重启
		// cli_simple_run_command("reboot");
	}
	vision_log_debug("GetForceHardwarePortType state=%u m_force_hardware_port_type %d  %d\n", state,m_force_hardware_port_type,SystemParaMenu2.m_force_hardware_port_type);

}
/** @note 切换探测器后重置测温工具坐标和状态 */
static void TempTool_ResetCoordState(ts_u08 vDetectorType) {
	int vx;
	ts_u16 center_x = 0, center_y = 0;

	switch (vDetectorType) {
		case FORCE_DETECTOR_TYPE_VOT_640X512: 
		case FORCE_DETECTOR_TYPE_VOT_640X512_9C1: 
		case FORCE_DETECTOR_TYPE_VOT_640X512_C122: 
		case FORCE_DETECTOR_TYPE_VOT_WLP_640X512:
		case FORCE_DETECTOR_TYPE_RTD_640X512:
		{
			center_x = 640 / 2;
			center_y = 512 / 2;
			break;
		}
		case FORCE_DETECTOR_TYPE_VOT_384X288: 
		case FORCE_DETECTOR_TYPE_VOT_384X288_9C1: 
		case FORCE_DETECTOR_TYPE_VOT_384X288_C122: 
		case FORCE_DETECTOR_TYPE_VOT_WLP_384X288:
		case FORCE_DETECTOR_TYPE_RTD_384X288:
		{
			center_x = 384 / 2;
			center_y = 288 / 2;
			break;
		}
		case FORCE_DETECTOR_TYPE_VOT_1280X1024: 
		case FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024:
		case FORCE_DETECTOR_TYPE_RTD_1280X1024:
		{
			center_x = 1280 / 2;
			center_y = 1024 / 2;
			break;
		}
		default:
		{
			center_x = 50;
			center_y = 50;
			break;
		}
	}
	Temp_Parameter.AD_sensor = 0;
	Temp_Parameter.SpotDisplay = 0;
	Temp_Parameter.AreaDisplay = 0;

	for (vx = 0; vx < TEMPERATURE_POINTS_NUMBER; vx++) {
		SpotInit[vx].spot_X_Pos = center_x + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx%3);
		SpotInit[vx].spot_Y_Pos = center_y + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx/3);
	}
	for (vx = 0; vx < TEMPERATURE_AREAS_NUMBER; vx++) {
		Areainit[vx].Area_Lx = (center_x - 30) + 40 * vx;
		Areainit[vx].Area_Ly = (center_y + 24);
		Areainit[vx].Area_Rx = (center_x - 30) + 32 + 40 * vx;
		Areainit[vx].Area_Ry = (center_y + 24) + 32;
	}
	Clear_Templut_Information();
	ISP_write_sys_cfg();
	Write_TempParameter_cfg(&Temp_Parameter);
}
void GetForceDectorType(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetForceDectorType state=%u SystemParaMenu2.m_force_detector_type %d \n", state,SystemParaMenu2.m_force_detector_type);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_force_detector_type = SystemParaMenu2.m_force_detector_type;
	if(m_force_detector_type>=FORCE_DETECTOR_TYPE_BUTT){
		m_force_detector_type = FORCE_DETECTOR_TYPE_VOT_640X512;
	}

	vision_log_debug("GetForceDectorType state=%u m_force_detector_type %d  %d\n", state,m_force_detector_type,SystemParaMenu2.m_force_detector_type);

	if(m_force_detector_type!=FORCE_DETECTOR_TYPE_NMOD){
		if(state){	
		
			if(state==1){
				if(m_force_detector_type<(FORCE_DETECTOR_TYPE_NMOD-1))
				//if(m_force_detector_type<(FORCE_DETECTOR_TYPE_RTD_1280X1024)) 
				{
					m_force_detector_type++;
				}
			}
			else if(state==2){
				if(m_force_detector_type>0){
					m_force_detector_type--;
				}
			}
		}
	}

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	if(res_type==DECTOR_RESOLUTION_384X288){
		if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "VOT640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "VOT384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "VOT1280");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "WLP640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "WLP384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "WLP1280");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "R640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "R384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "R1280");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1){
			sprintf((ts_char*)AddBuffer, "%s", "VOT64C");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122){
			sprintf((ts_char*)AddBuffer, "%s", "V64C12");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1){
			sprintf((ts_char*)AddBuffer, "%s", "VOT38C");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122){
			sprintf((ts_char*)AddBuffer, "%s", "V38C12");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "DetNMOD");
		}
	}
	else{

	
		if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "VOT_640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "VOT_384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1){
			sprintf((ts_char*)AddBuffer, "%s", "VOT_64C");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1){
			sprintf((ts_char*)AddBuffer, "%s", "VOT_38C");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122){
			sprintf((ts_char*)AddBuffer, "%s", "V_64C12");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122){
			sprintf((ts_char*)AddBuffer, "%s", "V_38C12");
		}	
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "VOT_1280");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "WLP_640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "WLP_384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "WLP_1280");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_640X512){
			sprintf((ts_char*)AddBuffer, "%s", "R_640");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_384X288){
			sprintf((ts_char*)AddBuffer, "%s", "R_384");
		}
		else if(m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_1280X1024){
			sprintf((ts_char*)AddBuffer, "%s", "R_1280");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "DET_NMOD");
		}
	}
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_force_detector_type != m_force_detector_type){
			SystemParaMenu2.m_force_detector_type = m_force_detector_type;
			/* 对测温功能相关变量进行清零操作 */
			TempTool_ResetCoordState(SystemParaMenu2.m_force_detector_type);
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for force %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
		}

		// vot_vpo_freeze();
		// // 软件主动断开电源，保护探测器
		// planck640c_stop();
		// // 自动重启
		// cli_simple_run_command("reboot");
	}
	vision_log_debug("GetForceDectorType state=%u m_force_detector_type %d  %d\n", state,m_force_detector_type,SystemParaMenu2.m_force_detector_type);

}


void GetMipiLanesParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetMipiLanesParameter state=%u SystemParaMenu2.m_mipi_lane_mode %d \n", state,SystemParaMenu2.m_mipi_lane_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_mipi_lane_mode = SystemParaMenu2.m_mipi_lane_mode;
	if(m_mipi_lane_mode>=MIPI_LANE_MODE_BUTT){
		m_mipi_lane_mode = MIPI_LANE_MODE_2_LANES;
	}

	vision_log_debug("GetMipiLanesParameter state=%u m_mipi_lane_mode %d  %d\n", state,m_mipi_lane_mode,SystemParaMenu2.m_mipi_lane_mode);

	if(state){	
	
		if(state==1){
			if(m_mipi_lane_mode<(MIPI_LANE_MODE_BUTT-1)) 
			{
				m_mipi_lane_mode++;
			}
		}
		else if(state==2){
			if(m_mipi_lane_mode>0){
				m_mipi_lane_mode--;
			}
		}
	}
	

	if(m_mipi_lane_mode==MIPI_LANE_MODE_2_LANES){
		sprintf((ts_char*)AddBuffer, "%s", ":2L");
	}
	else if(m_mipi_lane_mode==MIPI_LANE_MODE_4_LANES){
		sprintf((ts_char*)AddBuffer, "%s", ":4L");
	}
	else{
		sprintf((ts_char*)AddBuffer, "%s", ":2L");
	}
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_mipi_lane_mode != m_mipi_lane_mode){
			SystemParaMenu2.m_mipi_lane_mode = m_mipi_lane_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for mipi lanes %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_mipi_lane_mode %d  %d\n", state,m_mipi_lane_mode,SystemParaMenu2.m_mipi_lane_mode);

}


void GetForceCustomerId(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetForceCustomerId state=%u SystemParaMenu2.m_force_customer_id %d \n", state,SystemParaMenu2.m_force_customer_id);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_force_customer_id = SystemParaMenu2.m_force_customer_id;
	if(m_force_customer_id>=CUSTOMER_TYPE_BUTT){
		m_force_customer_id = CUSTOMER_TYPE_STANDARD;
	}

	vision_log_debug("GetForceCustomerId state=%u m_force_customer_id %d  %d\n", state,m_force_customer_id,SystemParaMenu2.m_force_customer_id);

	if(m_force_customer_id!=CUSTOMER_TYPE_NMOD){
		if(state){	
		
			if(state==1){
		
				if(m_force_customer_id<(CUSTOMER_TYPE_NMOD-1)) 
				{
					m_force_customer_id++;
				}
			}
			else if(state==2){
				if(m_force_customer_id>0){
					m_force_customer_id--;
				}
			}
		}
	}

	if(m_force_customer_id==CUSTOMER_TYPE_STANDARD){
		sprintf((ts_char*)AddBuffer, "%s", ":Normal");
	}
	else if(m_force_customer_id==CUSTOMER_TYPE_DSW){
		sprintf((ts_char*)AddBuffer, "%s", ":DSW");
	}
	else if(m_force_customer_id==CUSTOMER_TYPE_GENERAL){
		sprintf((ts_char*)AddBuffer, "%s", ":GEN");
	}
	else if(m_force_customer_id==CUSTOMER_TYPE_ZM){
		sprintf((ts_char*)AddBuffer, "%s", ":ZM");
	}
	else if(m_force_customer_id==CUSTOMER_TYPE_HAND){
		sprintf((ts_char*)AddBuffer, "%s", ":HAND");
	}
	else{
		sprintf((ts_char*)AddBuffer, "%s", ":NMOD");
	}
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_force_customer_id != m_force_customer_id){
			SystemParaMenu2.m_force_customer_id = m_force_customer_id;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for force %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_force_customer_id %d  %d\n", state,m_force_customer_id,SystemParaMenu2.m_force_customer_id);

}


void GetLensFocusType(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetLensFocusType state=%u SystemParaMenu2.m_force_focus_type %d \n", state,SystemParaMenu2.m_force_focus_type);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_force_focus_type = SystemParaMenu2.m_force_focus_type;
	if(m_force_focus_type>=FOCUS_TYPE_BUTT){
		m_force_focus_type = FOCUS_TYPE_PRIME_LENS;
	}

	vision_log_debug("GetLensFocusType state=%u m_force_focus_type %d  %d\n", state,m_force_focus_type,SystemParaMenu2.m_force_focus_type);

	if(m_force_focus_type!=FOCUS_TYPE_NMOD){
		if(state){	
		
			if(state==1){
		
				if(m_force_focus_type<(FOCUS_TYPE_NMOD-1)) 
				{
					m_force_focus_type++;
				}
			}
			else if(state==2){
				if(m_force_focus_type>0){
					m_force_focus_type--;
				}
			}
		}
	}

	if(m_force_focus_type==FOCUS_TYPE_PRIME_LENS){
		sprintf((ts_char*)AddBuffer, "%s", ":PRIME");
	}
	else if(m_force_focus_type==FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER){
		sprintf((ts_char*)AddBuffer, "%s", ":F_POT");
	}
	else if(m_force_focus_type==FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
		sprintf((ts_char*)AddBuffer, "%s", ":Z_POT");
	}
	else if(m_force_focus_type==FOCUS_TYPE_FOCUS_LENS_ENCODER){
		sprintf((ts_char*)AddBuffer, "%s", ":F_ENC");
	}
	else if(m_force_focus_type==FOCUS_TYPE_ZOOM_LENS_ENCODER){
		sprintf((ts_char*)AddBuffer, "%s", ":Z_ENC");
	}
	else{
		sprintf((ts_char*)AddBuffer, "%s", ":NMOD");
	}
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_force_focus_type != m_force_focus_type){
			SystemParaMenu2.m_force_focus_type = m_force_focus_type;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for force %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_force_focus_type %d  %d\n", state,m_force_focus_type,SystemParaMenu2.m_force_focus_type);

}


void GetDebugLogEn(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDebugLogEn state=%u global_vot_misc_cfg.m_force_debug_confirm %d \n", state,global_vot_misc_cfg.m_force_debug_confirm);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u32  m_force_debug_confirm = global_vot_misc_cfg.m_force_debug_confirm;
	if( !(m_force_debug_confirm==0 || m_force_debug_confirm==VOT_DEBUG_OUTPUT_CONFIRM_MAGIC)){
		m_force_debug_confirm = 0;
	}

	vision_log_debug("GetDebugLogEn state=%u m_force_debug_confirm 0x%x  %d\n", state,m_force_debug_confirm,global_vot_misc_cfg.m_force_debug_confirm);

	if(state){	
	
		if(state==1){
	
			if(m_force_debug_confirm==0) 
			{
				m_force_debug_confirm=VOT_DEBUG_OUTPUT_CONFIRM_MAGIC;
			}
		}
		else if(state==2){
			if(m_force_debug_confirm==VOT_DEBUG_OUTPUT_CONFIRM_MAGIC){
				m_force_debug_confirm=0;
			}
		}
	}
	
	if(m_force_debug_confirm==VOT_DEBUG_OUTPUT_CONFIRM_MAGIC){
		sprintf((ts_char*)AddBuffer, "%s", ":ON");
	}
	else if(m_force_debug_confirm==0){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
	}

	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(global_vot_misc_cfg.m_force_debug_confirm != m_force_debug_confirm){
			global_vot_misc_cfg.m_force_debug_confirm = m_force_debug_confirm;
			ISP_write_vot_misc_cfg(global_vot_misc_cfg);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for debug log%s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_force_debug_confirm %d  %d\n", state,m_force_debug_confirm,global_vot_misc_cfg.m_force_debug_confirm);

}

void Set_TEMP_REGULARITY(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("Set_TEMP_REGULARITY state=%u global_vot_misc_cfg.m_temp_regularity %d \n", state,SystemParaMenu2.m_temp_regularity);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u32  m_temp_regularity = SystemParaMenu2.m_temp_regularity;
	if( m_temp_regularity>=TEMP_REGULARITY_BUTT){
		m_temp_regularity = 0;
	}

	vision_log_debug("Set_TEMP_REGULARITY state=%u m_temp_regularity 0x%x  %d\n", state,m_temp_regularity,SystemParaMenu2.m_temp_regularity);

	if(state){	
	
		if(state==1){
	
				if(m_temp_regularity<(TEMP_REGULARITY_BUTT-1)) 
				{
					m_temp_regularity++;
				}
		}
		else if(state==2){
				if(m_temp_regularity>0){
					m_temp_regularity--;
				}
		}
	}
	
	if(m_temp_regularity==TEMP_REGULARITY_GENERAL){
		sprintf((ts_char*)AddBuffer, "%s", "GEN");
	}
	else if(m_temp_regularity==TEMP_REGULARITY_QC){
		sprintf((ts_char*)AddBuffer, "%s", "QC");
	}	
	else if(m_temp_regularity==TEMP_REGULARITY_HAND384){
		sprintf((ts_char*)AddBuffer, "%s", "HAND3");
	}

	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_temp_regularity != m_temp_regularity){
			SystemParaMenu2.m_temp_regularity = m_temp_regularity;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"The Temp Regularity is set to %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_temp_regularity %d  %d\n", state,m_temp_regularity,SystemParaMenu2.m_temp_regularity);

}
// void GetChipType(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active)
// {
// 	ts_u08 MenuNameLen;
// 	ts_u08 AddBuffer[32];
// 	vision_log_debug("GetChipType state=%u global_sys_ctrl_info.m_t10_tpu_type %d \n", state,global_sys_ctrl_info.m_t10_tpu_type);
// 	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
// 	ts_u32 u32_tmp = ts_cpl_readl(0x001e0014);
//     if (0 == u32_tmp) {
//         global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_N6;
//     } else {
//         global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_A12;
//     }
// 	if(global_sys_ctrl_info.m_t10_tpu_type==T10_CPU_TYPE_N6){
// 		sprintf((ts_char*)AddBuffer, "%s", ":N6");
// 	}
// 	else if(global_sys_ctrl_info.m_t10_tpu_type==T10_CPU_TYPE_A12){
// 		sprintf((ts_char*)AddBuffer, "%s", ":A12");
// 	}
// 	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
// 	Display_MenuItemLabel(Pt_Temp, is_active);
// }

extern void NPU_SR_ParamAvailable(void);
void SetNPU_SR(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	vision_log_debug("SetNPU_SR state=%u SystemParaMenu2.m_ts_npu_ops_sr_en %d \n", state,SystemParaMenu2.m_ts_npu_ops_sr_en);
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	ts_u32  m_ts_npu_ops_sr_en = SystemParaMenu2.m_ts_npu_ops_sr_en;
	if(m_ts_npu_ops_sr_en>=NPU_SR_BUTT){m_ts_npu_ops_sr_en=NPU_SR_OFF;}
	ts_u32 u32_tmp = ts_cpl_readl(0x001e0014);
    if (0 == u32_tmp) {
        global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_N6;
    } 
	else {
        global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_A12;
    }
    if(global_sys_ctrl_info.m_t10_tpu_type==T10_CPU_TYPE_N6)
	{
		sprintf((ts_char*)AddBuffer, "%s", "SR:OFF");//N6芯片不支持NPU，无法SR超分
		m_ts_npu_ops_sr_en = SystemParaMenu2.m_ts_npu_ops_sr_en=NPU_SR_OFF;
		if(state)
		{
			state=0;
			ts_char tip_info[64];
			snprintf(tip_info,64,"This Chip does not support NPU,super-resolution is unavailable");
			UpdatePromptInfoWithTimeMs(tip_info, 2000,VOT_LOG_TYPE_MENU2);
		}
	}
	else
	{
		dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
		if(res_type==DECTOR_RESOLUTION_384X288)
		{
			if(state){	
				if(state==1){
					m_ts_npu_ops_sr_en++;
					if(m_ts_npu_ops_sr_en<NPU_SR_384X288_640X512)
					{
						m_ts_npu_ops_sr_en=NPU_SR_384X288_640X512;
					}
					else if(m_ts_npu_ops_sr_en>(NPU_SR_BUTT-1))
					{
						m_ts_npu_ops_sr_en=NPU_SR_OFF;	
					}
					if (get_customer_type() == CUSTOMER_TYPE_HAND)m_ts_npu_ops_sr_en=NPU_SR_384X288_768X576;//手持机定制,384分辨率下，A6和A12芯片 NPU超分为384->768
				}
				else if(state==2){
					m_ts_npu_ops_sr_en--;
					if(m_ts_npu_ops_sr_en<NPU_SR_384X288_640X512)
					m_ts_npu_ops_sr_en=NPU_SR_OFF;	
					if (get_customer_type() == CUSTOMER_TYPE_HAND)m_ts_npu_ops_sr_en=NPU_SR_OFF;
				}
			}
			if(m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)
			{
				sprintf((ts_char*)AddBuffer, "%s", "384_640");//384分辨率下，A6和A12芯片 NPU超分为384->640
			}
			else if(m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)
			{
				sprintf((ts_char*)AddBuffer, "%s", "384_768");//384分辨率下，A6和A12芯片 NPU超分为384->768
			}
			else
			{	
				sprintf((ts_char*)AddBuffer, "%s", "SR:OFF");
			}
		}
		else if(res_type==DECTOR_RESOLUTION_640X512)
		{
			if(state){	
				if(state==1){	
					if(m_ts_npu_ops_sr_en<NPU_SR_640X512_1280X1024)
					{
						m_ts_npu_ops_sr_en++;
					}
				}
				else if(state==2){
					if(m_ts_npu_ops_sr_en>0){
						m_ts_npu_ops_sr_en--;
					}
				}
			}
			if(m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768){
			sprintf((ts_char*)AddBuffer, "%s", "640->960");//640分辨率下，仅A12芯片 NPU超分为640->960
			}
			else if(m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024){
				sprintf((ts_char*)AddBuffer, "%s", "640->1280");//640分辨率下，仅A12芯片 NPU超分为640->1280
			}
			else
			{	
				sprintf((ts_char*)AddBuffer, "%s", "SR:OFF");
			}
		}
		else if(res_type==DECTOR_RESOLUTION_1280X1024)
		{
			sprintf((ts_char*)AddBuffer, "%s", "SR:OFF");//1280分辨率下，暂时不支持SR超分
		}	
	}


	if(SystemParaMenu2.m_temp_drift_aisp_mode == TEMP_DRIFT_COMPENSATION_AI_POST_PROCESS_ON){
		UpdatePromptInfoWithTimeMs(" only support aisp or sr !", 1000,VOT_LOG_TYPE_MENU2);
		return;
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		if(SystemParaMenu2.m_ts_npu_ops_sr_en != m_ts_npu_ops_sr_en){
			SystemParaMenu2.m_ts_npu_ops_sr_en = m_ts_npu_ops_sr_en;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			if (get_customer_type() == CUSTOMER_TYPE_HAND)//手持机定制,384分辨率下，A6和A12芯片 NPU超分为384->768,无需重启设备，参数直接生效
			{
				NPU_SR_ParamAvailable();//超分使能切换,无需设备重启，参数直接生效
			}
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for NPU SR %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 2000,VOT_LOG_TYPE_MENU2);
		}
	}
	vision_log_debug(" state=%u m_ts_npu_ops_sr_en %d  %d\n", state,m_ts_npu_ops_sr_en,SystemParaMenu2.m_ts_npu_ops_sr_en);
}

void GetBtTimingSequence(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetBtTimingSeqForWin2 state=%u SystemParaMenu2.m_bt_time_sequece_mode %d \n", state,SystemParaMenu2.m_bt_time_sequece_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_bt_time_sequece_mode = SystemParaMenu2.m_bt_time_sequece_mode;
	if(m_bt_time_sequece_mode>=BT_TIMING_SEQ_MODE_BUTT){
		m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_NONE;
	}

	vision_log_debug("GetBtTimingSeqForWin2 state=%u m_bt_time_sequece_mode %d  %d\n", state,m_bt_time_sequece_mode,SystemParaMenu2.m_bt_time_sequece_mode);

	if(state){	
	
		if(state==1){
			if(m_bt_time_sequece_mode<(BT_TIMING_SEQ_MODE_BUTT-1)) 
			{
				m_bt_time_sequece_mode++;
			}
		}
		else if(state==2){
			if(m_bt_time_sequece_mode>0){
				m_bt_time_sequece_mode--;
			}
		}
	}
	

	if(m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
		sprintf((ts_char*)AddBuffer, "%s", ":Win50");
	}
	else if(m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
		sprintf((ts_char*)AddBuffer, "%s", ":Win25");
	}
	else{
		sprintf((ts_char*)AddBuffer, "%s", ":None");
	}
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_bt_time_sequece_mode != m_bt_time_sequece_mode){
			SystemParaMenu2.m_bt_time_sequece_mode = m_bt_time_sequece_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ts_char tip_info[64];
			snprintf(tip_info,64,"Please restart system for bt's timing seq %s", AddBuffer);
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	
		}

	}
	vision_log_debug(" state=%u m_bt_time_sequece_mode %d  %d\n", state,m_bt_time_sequece_mode,SystemParaMenu2.m_bt_time_sequece_mode);

}

//@parameter:Status:0:初始化显示参数,1:参数++,2:参数--
void GetDisplayST0_total_duration(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active) 
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug(" state=%u SystemParaMenu2.m_st0_total_duration_s %d \n", state,SystemParaMenu2.m_st0_total_duration_s);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_st0_total_duration_s = SystemParaMenu2.m_st0_total_duration_s;
	if(m_st0_total_duration_s>ST_0_TOTAL_DURATION_S_MAX){
		m_st0_total_duration_s = ST_0_TOTAL_DURATION_S_DEFAULT;
	}

	vision_log_debug(" state=%u m_st0_total_duration_s %d  %d\n", state,m_st0_total_duration_s,SystemParaMenu2.m_st0_total_duration_s);


	if(state){	
	
		if(state==1){
			if(m_st0_total_duration_s<(ST_0_TOTAL_DURATION_S_MAX-1)){
				m_st0_total_duration_s++;
			}
		}
		else if(state==2){
			if(m_st0_total_duration_s>0){
				m_st0_total_duration_s--;
			}
		}
	}

	sprintf((ts_char*)AddBuffer, ":%d",m_st0_total_duration_s);

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_st0_total_duration_s != m_st0_total_duration_s){
			SystemParaMenu2.m_st0_total_duration_s = m_st0_total_duration_s;
		}

	}
	
}


void GetDisplayST0_interval(struct  _MenuStructure * Pt_Temp,ts_u08 state,ts_u08 is_active) 
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug(" state=%u SystemParaMenu2.m_st0_interval_s %d \n", state,SystemParaMenu2.m_st0_interval_s);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_st0_interval_s = SystemParaMenu2.m_st0_interval_s;
	if(m_st0_interval_s>ST_0_INTERVAL_S_MAX){
		m_st0_interval_s = ST_0_INTERVAL_S_DEFAULT;
	}

	vision_log_debug(" state=%u m_st0_interval_s %d  %d\n", state,m_st0_interval_s,SystemParaMenu2.m_st0_interval_s);


	if(state){	
	
		if(state==1){
			if(m_st0_interval_s<(ST_0_INTERVAL_S_MAX-1)){
				m_st0_interval_s++;
			}
		}
		else if(state==2){
			if(m_st0_interval_s>0){
				m_st0_interval_s--;
			}
		}
	}

	sprintf((ts_char*)AddBuffer, ":%d",m_st0_interval_s);

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_st0_interval_s != m_st0_interval_s){
			SystemParaMenu2.m_st0_interval_s = m_st0_interval_s;
		}

	}
}


//@parameter:Status:0:初始化显示参数,1:参数++,2:参数--
void GetDisplayST1_M(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	//0-10min
	if(SystemParaMenu2.m_st1_m < 0 && SystemParaMenu2.m_st1_m > 10) {
		SystemParaMenu2.m_st1_m = 0;
	}
	ts_s08 m_st1_m = SystemParaMenu2.m_st1_m;
	ts_s08 last_st1_m = SystemParaMenu2.m_st1_m;
	if(state == 1) {
		m_st1_m++;
		if(m_st1_m > 10) m_st1_m = 0;
		
	}else if(state == 2) {
		m_st1_m--;	
		if(m_st1_m < 0)  m_st1_m= 10;
	}

	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(m_st1_m == 0) {
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");	
		
	}else {
		sprintf((ts_char*)AddBuffer, ":%dM", m_st1_m );	
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){	
		if(SystemParaMenu2.m_st1_m != m_st1_m){
			SystemParaMenu2.m_st1_m = m_st1_m;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
	}
	if(m_st1_m == 0) {
		GetDisplayST1_S(Pt_Temp->Next, state, 0);
		return;
	}
	if(last_st1_m == 0 && m_st1_m != 0) {
		GetDisplayST1_S(Pt_Temp->Next, 0, 0);
	}
	
}

void GetDisplayST1_S(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	//5-30s
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(SystemParaMenu2.m_st1_m == 0) {
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");	
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		Display_MenuItemLabel(Pt_Temp, is_active);
		return;
	}
	if(SystemParaMenu2.m_st1_s < 0 && SystemParaMenu2.m_st1_s > 30) {
		SystemParaMenu2.m_st1_s = 5;
	}
	ts_u08 m_st1_s = SystemParaMenu2.m_st1_s;
	if(state == 1) {
		m_st1_s = m_st1_s + 5;
		if(m_st1_s > 30) m_st1_s = 5;	
	}else if(state == 2) {
		m_st1_s = m_st1_s - 5;	
		if(m_st1_s <= 0)  m_st1_s= 30;
	}

	
	
	sprintf((ts_char*)AddBuffer, ":%dS", m_st1_s );	
	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){	
		if(SystemParaMenu2.m_st1_s != m_st1_s){
			SystemParaMenu2.m_st1_s = m_st1_s;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}

	}
}

void GetDisplayDET_ST(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	//vision_log_notice("GetDisplayDET_ST state=%u SystemParaMenu2.m_st1_m %d \n", state,SystemParaMenu2.m_det_st);
	//0-50  --> 0-5°C
	ts_s08 m_det_st = SystemParaMenu2.m_det_st;
	if(state == 1) {
		m_det_st = m_det_st + 1;
		if(m_det_st > 50) m_det_st = 0;
		
	}else if(state == 2) {
		m_det_st = m_det_st - 1;	
		if(m_det_st < 0)  m_det_st= 50;
	}

	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(m_det_st == 0) {
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");	
	}else {
		ts_cpl_snprintf((ts_char*)AddBuffer, 32, ":%.1f", (m_det_st/10.0f + 0.099));
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){	
		if(SystemParaMenu2.m_det_st != m_det_st){
			SystemParaMenu2.m_det_st = m_det_st;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}

	}	
}

void sffc_Init(ts_u08 state)
{
	// if(state)
	// {
	// 	sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
	// 	shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);
	// 	ts_cpl_msleep(1000);

	// 	ISP_sffc_init();
	// 	UpdatePromptInfoWithTimeMs("sffc init completed", 2000,VOT_LOG_TYPE_MENU2);

	// }
}

void sffc_calculate(ts_u08 state)
{
	if(state)
	{
		UpdatePromptInfo("sffc calc ing", 1,VOT_LOG_TYPE_MENU2);
		ISP_set_det_calibrationing_flag(1);
		ISP_sffc_calc();
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		ISP_set_det_calibrationing_flag(0);
		UpdatePromptInfoWithTimeMs("sffc calc completed",2000,VOT_LOG_TYPE_MENU2);

	}
}

void sffc_nr_refresh(ts_u08 state)
{
	if(state)
	{
		UpdatePromptInfo("sffc nr calc ing", 1,VOT_LOG_TYPE_MENU2);
		ISP_set_det_calibrationing_flag(1);
		ISP_sffc_nr_calc_and_save();
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		ISP_set_det_calibrationing_flag(0);
		UpdatePromptInfoWithTimeMs("sffc nr calc completed",2000,VOT_LOG_TYPE_MENU2);

	}
}

void GetDisplayEeEnhanceMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayEeEnhanceMode state=%u SystemParaMenu2.m_ee_enhance_mode %d \n", state,SystemParaMenu2.m_ee_enhance_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  ee_enhance_mode = SystemParaMenu2.m_ee_enhance_mode;
	if(ee_enhance_mode>=EE_ENHANCE_MODE_BUTT){
		ee_enhance_mode = EE_ENHANCE_MODE_OFF;
	}

	vision_log_debug("GetDisplayDeviceEnvMode state=%u ee_enhance_mode %d  %d\n", state,ee_enhance_mode,SystemParaMenu2.m_ee_enhance_mode);

	if(state){	
	
		if(state==1){
			if(ee_enhance_mode<(EE_ENHANCE_MODE_BUTT-1)){
				ee_enhance_mode++;
			}
		}
		else if(state==2){
			if(ee_enhance_mode>0){
				ee_enhance_mode--;
			}
		}
	}

	if(ee_enhance_mode == EE_ENHANCE_MODE_OFF){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_WHITE){
		sprintf((ts_char*)AddBuffer, "%s", ":1");
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_BLACK){
		sprintf((ts_char*)AddBuffer, "%s", ":2");
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_WHITE_BLACK){
		sprintf((ts_char*)AddBuffer, "%s", ":3");
	}


	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_ee_enhance_mode != ee_enhance_mode){
			SystemParaMenu2.m_ee_enhance_mode = ee_enhance_mode;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ISP_ee_enhance_mode_set(SystemParaMenu2.m_ee_enhance_mode, 0);
		}

	}
	vision_log_debug("GetDisplayDeviceEnvMode state=%u ee_enhance_mode %d  %d\n", state,ee_enhance_mode,SystemParaMenu2.m_ee_enhance_mode);

}


void GetDisplayFixSncSwitch(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayFixSncSwitch state=%u SystemParaMenu2.m_fix_snc_switch %d \n", state,SystemParaMenu2.m_fix_snc_switch);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_fix_snc_switch = SystemParaMenu2.m_fix_snc_switch;
	if(m_fix_snc_switch>=VOT_SWITCH_BUTT){
		m_fix_snc_switch = VOT_SWITCH_OFF;
	}

	vision_log_debug("GetDisplayDeviceEnvMode state=%u m_fix_snc_switch %d  %d\n", state,m_fix_snc_switch,SystemParaMenu2.m_fix_snc_switch);

	if(state){	
	
		if(state==1){
			if(m_fix_snc_switch<(VOT_SWITCH_BUTT-1)){
				m_fix_snc_switch++;
			}
		}
		else if(state==2){
			if(m_fix_snc_switch>0){
				m_fix_snc_switch--;
			}
		}
	}

	if(m_fix_snc_switch == VOT_SWITCH_OFF){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
	}
	else if(m_fix_snc_switch == VOT_SWITCH_ON){
		sprintf((ts_char*)AddBuffer, "%s", ":ON");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_fix_snc_switch != m_fix_snc_switch){
			SystemParaMenu2.m_fix_snc_switch = m_fix_snc_switch;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
			ISP_fix_snc_switch_set(SystemParaMenu2.m_fix_snc_switch);
			UpdatePromptInfoWithTimeMs(" please restart system for fix snc !", 1000,VOT_LOG_TYPE_MENU2);
			
		}
		

	}
	vision_log_debug("GetDisplayDeviceEnvMode state=%u m_fix_snc_switch %d  %d\n", state,m_fix_snc_switch,SystemParaMenu2.m_fix_snc_switch);

}


void GetDisplayTDCMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayTDCMode state=%u SystemParaMenu2.m_temp_drift_comp_mode %d \n", state,SystemParaMenu2.m_temp_drift_comp_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_temp_drift_comp_mode = SystemParaMenu2.m_temp_drift_comp_mode;
	if(m_temp_drift_comp_mode>=TEMP_DRIFT_COMPENSATION_MODE_BUTT){
		m_temp_drift_comp_mode = TEMP_DRIFT_COMPENSATION_MODE_OFF;
	}

	vision_log_debug("GetDisplayTDCMode state=%u m_temp_drift_comp_mode %d  %d\n", state,m_temp_drift_comp_mode,SystemParaMenu2.m_temp_drift_comp_mode);

	if(state){	
	
		if(state==1){
			if(m_temp_drift_comp_mode<(TEMP_DRIFT_COMPENSATION_MODE_BUTT-1)){
				m_temp_drift_comp_mode++;
			}
		}
		else if(state==2){
			if(m_temp_drift_comp_mode>0){
				m_temp_drift_comp_mode--;
			}
		}
	}

	if(m_temp_drift_comp_mode == TEMP_DRIFT_COMPENSATION_MODE_OFF){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
	}
	else if(m_temp_drift_comp_mode == TEMP_DRIFT_COMPENSATION_MODE_ON){
		sprintf((ts_char*)AddBuffer, "%s", ":ON");
	}
	else if(m_temp_drift_comp_mode == TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){
		sprintf((ts_char*)AddBuffer, "%s", ":Cali");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(SystemParaMenu2.m_temp_drift_comp_mode != m_temp_drift_comp_mode){
			if(m_temp_drift_comp_mode==TEMP_DRIFT_COMPENSATION_MODE_ON || m_temp_drift_comp_mode==TEMP_DRIFT_COMPENSATION_MODE_OFF){
				UpdatePromptInfoWithTimeMs(" please restart system for temp drift comp !", 1000,VOT_LOG_TYPE_MENU2);
			}
			vot_set_temp_drift_calibration_mode(m_temp_drift_comp_mode,0);
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}

	}
	vision_log_debug("GetDisplayTDCMode state=%u m_temp_drift_comp_mode %d  %d\n", state,m_temp_drift_comp_mode,SystemParaMenu2.m_temp_drift_comp_mode);

}

void GetDisplayTDCCalibrationIndex(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayTDCCalibrationIndex state=%u global_sys_ctrl_info.m_cali_index_offset %d \n", state,global_sys_ctrl_info.m_cali_index_offset);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_s32  s32_oft = global_sys_ctrl_info.m_cali_index_offset;
	if(state){	
	
		if(state==1){
			if(s32_oft<9){
				s32_oft++;
			}
		}
		else if(state==2){
			if(s32_oft>0){
				s32_oft--;
			}
		}
	}

	if(s32_oft>8 || s32_oft<0){
		s32_oft = 0;
	}

	vision_log_debug("GetDisplayTDCCalibrationIndex state=%u s32_oft %d  %d\n", state,s32_oft,global_sys_ctrl_info.m_cali_index_offset);

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen],(ts_u08)s32_oft );
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(global_sys_ctrl_info.m_cali_index_offset != s32_oft){
			global_sys_ctrl_info.m_cali_index_offset = s32_oft;
		}

	}
	vision_log_debug("GetDisplayTDCCalibrationIndex state=%u s32_oft %d  %d\n", state,s32_oft,global_sys_ctrl_info.m_cali_index_offset);

}


void GetDisplayTDCCalibrationOperation(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	vision_log_debug("GetDisplayTDCCalibration state=%u  global_sys_ctrl_info.m_cali_index_offset %d \n", state,global_sys_ctrl_info.m_cali_index_offset);


	if(state){
		vot_read_det_ad();//读取det温度值,并转换为温度值存入global_sys_ctrl_info.m_det_temp_value全局变量中
		
		SystemParaMenu2.m_temp_drift_temp[global_sys_ctrl_info.m_cali_index_offset] = 10*global_sys_ctrl_info.m_det_temp_value;
		SystemParaMenu2.m_temp_drift_index &= ~(1<<global_sys_ctrl_info.m_cali_index_offset);	
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		ISP_set_TDC_node(global_sys_ctrl_info.m_cali_index_offset);
	}

}

void GetDisplayTDCAispMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayTDCAispMode state=%u SystemParaMenu2.m_temp_drift_aisp_mode %d \n", state,SystemParaMenu2.m_temp_drift_aisp_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_temp_drift_aisp_mode = SystemParaMenu2.m_temp_drift_aisp_mode;
	if(m_temp_drift_aisp_mode>=TEMP_DRIFT_COMPENSATION_AI_APP_BUTT){
		m_temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
	}

	if(state){	
		if(vot_get_temp_drift_aisp_enable() == 0 || SystemParaMenu2.m_ts_npu_ops_sr_en > NPU_SR_OFF){
            UpdatePromptInfoWithTimeMs(" N6/N6E don't support temp drift aisp !", 1000,VOT_LOG_TYPE_MENU2);
		}
	}

	if(vot_get_temp_drift_aisp_enable() == 0 || SystemParaMenu2.m_ts_npu_ops_sr_en > NPU_SR_OFF){
		m_temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
		SystemParaMenu2.m_temp_drift_aisp_mode = m_temp_drift_aisp_mode;
		state = 0;
	}

	vision_log_debug("GetDisplayTDCMode state=%u m_temp_drift_aisp_mode %d  %d\n", state,m_temp_drift_aisp_mode,SystemParaMenu2.m_temp_drift_aisp_mode);

	if(state){	
	
		if(state==1){
			if(m_temp_drift_aisp_mode<(TEMP_DRIFT_COMPENSATION_AI_APP_BUTT-1)){
				m_temp_drift_aisp_mode++;
			}
		}
		else if(state==2){
			if(m_temp_drift_aisp_mode>0){
				m_temp_drift_aisp_mode--;
			}
		}
		
	}

	if(m_temp_drift_aisp_mode == TEMP_DRIFT_COMPENSATION_AI_APP_OFF){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
	}
	else if(m_temp_drift_aisp_mode == TEMP_DRIFT_COMPENSATION_AI_POST_PROCESS_ON){
		sprintf((ts_char*)AddBuffer, "%s", ":PP");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){

		if(SystemParaMenu2.m_temp_drift_aisp_mode != m_temp_drift_aisp_mode){
			
			UpdatePromptInfoWithTimeMs(" please restart system for temp drift  aisp !", 1000,VOT_LOG_TYPE_MENU2);
			
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}

	}
	vision_log_debug("GetDisplayTDCMode state=%u m_temp_drift_aisp_mode %d  %d\n", state,m_temp_drift_aisp_mode,SystemParaMenu2.m_temp_drift_aisp_mode);

}



/**
 * @brief:OCC初始化
 * @parameter:state:0:不执行,1-2:OCC初始化
 */
void OCC_Init(ts_u08 state)
{

	if(state)
	{
		UpdatePromptInfo("occ init doing ...", 1,VOT_LOG_TYPE_MENU2);
		sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
		shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);
		ts_cpl_msleep(1000);

		ISP_occ_init();
		UpdatePromptInfoWithTimeMs("occ init completed", 1000,VOT_LOG_TYPE_MENU2);

	}
}

/**
 * @brief:OCC计算
 * @parameter:state:0:不执行,1-2:OCC计算
 */
void OCC_Calculate(ts_u08 state)
{
	if(state)
	{

		UpdatePromptInfo("occ calc ing", 1,VOT_LOG_TYPE_MENU2);
		ISP_set_det_calibrationing_flag(1);
		ISP_occ_calc();
		ISP_set_det_calibrationing_flag(0);
		UpdatePromptInfoWithTimeMs("occ calc completed",2000,VOT_LOG_TYPE_MENU2);
		// 测温版本需调整到ADV=0x1500
		if(SystemParaMenu2.Usermenu_SW)	
		{
			ISP_cut_new();
			ts_cpl_msleep(500);
			Automatic_Temp_GOS_Configuration();
		}
	}
}

/**
 * @brief:OCC保存
 * @parameter:0:不执行,1-2:OCC保存
 */
void OCC_Save(ts_u08 state)
{
	if(state)
	{
		ISP_occ_save();
		
		UpdatePromptInfoWithTimeMs("occ save completed",2000,VOT_LOG_TYPE_MENU2);

//		displayReminderInfo(Done);
	}
}


void OCC_c_and_b(ts_u08 state)
{
	if(state)
	{
		sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
		shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);
		ts_cpl_msleep(1000);
		ISP_occ_init();
		ts_cpl_msleep(1000);
		ISP_occ_calc_with_shutter();
		// ts_cpl_msleep(1000);
		// ISP_occ_save();
		ts_cpl_msleep(1000);
		ISP_update_b();
		ISP_run();
	}
}

/**
 * @brief:获得以及显示：OCC_way参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:0:初始化显示此参数,1:参数++,2:参数--
 * 执行切换OCC计方式  0逐次祝词逼近 1二分法
 */
void GetAndDisplayOCCwayOpr(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if 0
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 0)
	{
//		display_u8number(Xline,Yline,systemPar.OOC_Way,3,CharacterColor);
	}
	else if(state ==1)
	{
		if(systemPar.OOC_Way==0)
		{
			systemPar.OOC_Way = 1;
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,systemPar.OOC_Way,3,CharacterColor);
		}
	}
	else
	{
		if(systemPar.OOC_Way==1)
		{
			systemPar.OOC_Way = 0;
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,systemPar.OOC_Way,3,CharacterColor);
		}
	}
#endif
}
#ifdef  TEST_SAVE_Image   //暂时用于测试
/**
 * @brief:获得以及显示：ImgNum
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:0:初始化显示此参数,1:参数++,2:参数--
 * 执行切换 存取一帧图像的测试数据
 */
void DisplayImgNum(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 0)
	{
		display_u8number(Xline,Yline,systemPar.ImgNum,3,CharacterColor);
	}
	else if(state ==1)
	{
		if(systemPar.ImgNum < 5)  systemPar.ImgNum++;
		else 					  systemPar.ImgNum = 0;
		DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,systemPar.ImgNum,3,CharacterColor);

	}
	else
	{
		if(systemPar.ImgNum > 0)  systemPar.ImgNum--;
		else 					  systemPar.ImgNum = 5;
		DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,systemPar.ImgNum,3,CharacterColor);
	}
}

void DisplayImgNumSave(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	switch(state)
	{
		case 0:  break;
		case 1: WriteToFlash_ImageTest(); break;//++
		case 2: ReadFromFlash_ImageTest(); break;//--
	}
}


#endif

/**
 * @brief:获得以及显示：NUC_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayNUC_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	ts_u08 nuc_sw = SystemParaMenu2.NUC_SW;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayNUC_SWParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.AutoShutter=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.NUC_SW);

	if(nuc_sw > 1){
		nuc_sw = 1;
	}

	if(state)
	{
		if(nuc_sw == 0)
		{
			nuc_sw = 1;

		}
		else if(nuc_sw == 1)
		{
			nuc_sw= 0;

		}
	}
	

	if(nuc_sw == 0)
	{
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "OFF", 4);
		Display_MenuItemLabel(Pt_Temp, is_active);
		
	}
	else if(nuc_sw == 1)
	{
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "ON", 3);
		Display_MenuItemLabel(Pt_Temp, is_active);
		
	}

	if(nuc_sw != SystemParaMenu2.NUC_SW){
		SystemParaMenu2.NUC_SW = nuc_sw;
		if(SystemParaMenu2.NUC_SW == 0)
		{
			ISP_clr_nuc_k();
		}
		else if(SystemParaMenu2.NUC_SW == 1)
		{
			ISP_reset_nuc_k();
		}
	}

}



/**
 * @brief:NUC初始化
 * @parameter:state:0:不执行,1-2:OCC初始化
 */
void NUC_Init(ts_u08 state)
{
	if(state)
	{
		UpdatePromptInfo("nuc init doing ...", 1,VOT_LOG_TYPE_MENU2);
		//确保是关闭的
		sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
		shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);
		ts_cpl_msleep(2000);
		

		ISP_nuc_init();
		SystemParaMenu2.BAD_SW = 1;
    	SystemParaMenu2.NUC_SW = 1;
		NUC_Calc(0);
		UpdatePromptInfoWithTimeMs("nuc init completed!", 2000,VOT_LOG_TYPE_MENU2);
	}
}

void NUC_Calc_Low_Temperature(ts_u08 state)
{
	if(state==0)
	{
		return;
	}

	if(state )
	{
		UpdatePromptInfo("nuc low temp calc k ...", 1,VOT_LOG_TYPE_MENU2);
		ISP_nuc_calc_k_lowtemp();
		if(global_sys_ctrl_info.m_low_temp_ad_buff==0){
			ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
			ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;
			global_sys_ctrl_info.m_low_temp_ad_buff = (ts_u32)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,(s32_img_w * s32_img_h * 2 )) ;
		}
		isp_dump_ad_info(TS_UPL_POS_RAW , NULL , NULL, NULL,(ts_void*)global_sys_ctrl_info.m_low_temp_ad_buff);
		UpdatePromptInfoWithTimeMs("nuc low temp calc k completed!", 2500,VOT_LOG_TYPE_MENU2);
	}
}

void NUC_Calc_High_Temperature(ts_u08 state)
{
	if(state==0)
	{
		return;
	}

	if(state )
	{
		UpdatePromptInfo("nuc high temp calc k ...", 1,VOT_LOG_TYPE_MENU2);
		ISP_nuc_calc_k_hightemp();
		if(global_sys_ctrl_info.m_high_temp_ad_buff==0){
			ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
			ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;
			global_sys_ctrl_info.m_high_temp_ad_buff = (ts_u32)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,(s32_img_w * s32_img_h * 2 )) ;
		}
		isp_dump_ad_info(TS_UPL_POS_RAW , NULL , NULL, NULL,(ts_void*)global_sys_ctrl_info.m_high_temp_ad_buff);
		UpdatePromptInfoWithTimeMs("nuc high temp calc k completed!", 2500,VOT_LOG_TYPE_MENU2);
	}
}

/**
 * @brief:NUC计算
 * @parameter:state:0:不执行,1:30度对着黑体,2:60度对着黑体,OCC计算
 */
void NUC_Calc(ts_u08 state)
{

	static ts_u08 low_temp_done_flag=0, high_temp_done_flag=0;

	if(state==0)
	{
		low_temp_done_flag=0;
		high_temp_done_flag=0;
		return;
	}
	else{
		ISP_set_det_calibrationing_flag(1);
#ifndef VOT_NUC_SFFC_TEST
		if(state == 1)
		{
			//必须是低温帧先标定完，才进行高温帧标定
			if(low_temp_done_flag == 1){
				if(high_temp_done_flag==0){
					NUC_Calc_High_Temperature(state);
					high_temp_done_flag = 1;
				}
				
				UpdatePromptInfoWithTimeMs("nuc high temp calc had completed!", 2000,VOT_LOG_TYPE_MENU2);
				// #ifdef CONFIG_SNR_PLANCK1280C
				// SystemParaMenu2.NUC_SW = 1;//做完10度、60度高低温黑体进行NUC校正,开启NUC开关，
				// ISP_reset_nuc_k();
				// UpdatePromptInfoWithTimeMs("NUC_SW ON", 2500,VOT_LOG_TYPE_MENU2);
				// //如果当前界面在菜单二的NUC参数设置界面，则刷新界面显示
				// if(SelectionMenu.MenuState == 0x26)
				// {
				// 	Update_Display_TestMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,SelectionMenu.SelectItem->Info.ID);
				// }
				// #endif
			}
			else{
				UpdatePromptInfoWithTimeMs("please first low temp calc !", 2000,VOT_LOG_TYPE_MENU2);
			}
			
		}
		else if(state == 2)
		{
			
			if(low_temp_done_flag == 0 ){
				NUC_Calc_Low_Temperature(state);
				low_temp_done_flag = 1;
			}
			else{
				UpdatePromptInfoWithTimeMs("nuc low temp calc had completed !", 2000,VOT_LOG_TYPE_MENU2);
			}

		}

#else
		if(state == 1)
		{
			
			if(high_temp_done_flag == 0){
				NUC_Calc_High_Temperature(state);
				high_temp_done_flag = 1;
			}
			else{
				UpdatePromptInfoWithTimeMs("nuc high temp calc had completed!", 2000,VOT_LOG_TYPE_MENU2);
			}
			
		}
		else if(state == 2)
		{
			//必须是高温帧先标定完，才进行低温帧标定，新的sffc算法
			if(high_temp_done_flag == 1 ){
				NUC_Calc_Low_Temperature(state);
				low_temp_done_flag = 1;
			}
			else{
				low_temp_done_flag = 0;	
				UpdatePromptInfoWithTimeMs("please first high temp calc !", 2000,VOT_LOG_TYPE_MENU2);
			}

		}
#endif
		if(high_temp_done_flag==1 && low_temp_done_flag==1){
			
			high_temp_done_flag = 0;
			low_temp_done_flag = 0;
			NUC_Calc_K(state);
			ISP_nuc_calc_k_save();
			
			if(global_sys_ctrl_info.m_low_temp_ad_buff){
				vot_tlsf_safe_free(&g_VotMemPoolInfo,(void**)&global_sys_ctrl_info.m_low_temp_ad_buff);
				global_sys_ctrl_info.m_low_temp_ad_buff=0;
			}

			if(global_sys_ctrl_info.m_high_temp_ad_buff){
				vot_tlsf_safe_free(&g_VotMemPoolInfo,(void**)&global_sys_ctrl_info.m_high_temp_ad_buff);
				global_sys_ctrl_info.m_high_temp_ad_buff=0;
			}

			//nuc calc后，主动保存一次vot 坏点表，避免产线操作流程未主动保存，导致画面异常
			ISP_nuc_vot_kb_save();
			
		}

		ISP_set_det_calibrationing_flag(0);
	}

}


void NUC_Calc_K(ts_u08 state)
{
	if(state==0)
	{
		return;
	}

	if(state)
	{
		UpdatePromptInfo("calc k ing...", 1,VOT_LOG_TYPE_MENU2);
		ISP_nuc_calc_k_calc();
		DeadPointCorrection();
		UpdatePromptInfoWithTimeMs("calc k completed!", 2500,VOT_LOG_TYPE_MENU2);

	}

	
}

/**
 * @brief:NUC保存
 * @parameter:0:不执行,1-2:NUC保存
 */
void NUC_Save(ts_u08 state)
{
	if(state==0)
	{
		return;
	}

	if(state)
	{
		UpdatePromptInfo("nuc save k...", 1,VOT_LOG_TYPE_MENU2);
		ISP_nuc_vot_kb_save();
		UpdatePromptInfoWithTimeMs("nuc save k completed!", 2500,VOT_LOG_TYPE_MENU2);
		ISP_cut();
	}
	
}

void SFFC_Refresh(ts_u08 state)
{
	if(state==0)
	{
		return;
	}

	if(state)
	{
		UpdatePromptInfo("sffc refresh ing..", 1,VOT_LOG_TYPE_MENU2);
		ISP_set_det_calibrationing_flag(1);
		ISP_sffc_refresh();
		ISP_set_det_calibrationing_flag(0);
		UpdatePromptInfoWithTimeMs("sffc refresh finished !", 1000,VOT_LOG_TYPE_MENU2);
	}
	
}

/**
 * @brief:获得以及显示：NUC_Flag参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayNUC_FlagParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.NUC_Flag)
		{
			DisPlayNewStr(Xline,Yline,"Black",3,CharacterColor); //30.60度黑体
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"Shutter",3,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.NUC_Flag == 0)
		{
			SystemParaMenu2.NUC_Flag = 1;
			DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"Black",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.NUC_Flag == 1)
		{
			SystemParaMenu2.NUC_Flag = 0;
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"Shutter",3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：TFilter参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayTFilterParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.TFilter == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else
		{
			display_u8number(Xline,Yline,SystemParaMenu2.TFilter,3,CharacterColor);
		}

	}
	else if(state ==1)
	{
		SystemParaMenu2.TFilter++;
		if(SystemParaMenu2.TFilter > 3)
		{
			SystemParaMenu2.TFilter = 3;
		}
		else
		{
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff)|(SystemParaMenu2.TFilter<<10)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.TFilter,3,CharacterColor);

		}
	}
	else
	{
		if(SystemParaMenu2.TFilter > 0)
		{
			SystemParaMenu2.TFilter--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff)|(SystemParaMenu2.TFilter<<10)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			if(SystemParaMenu2.TFilter == 0)
			{
				DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
			}
			else
			{
				display_u8number(Xline,Yline,SystemParaMenu2.TFilter,3,CharacterColor);
			}
		}
		else
		{
			SystemParaMenu2.TFilter = 0;
		}
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.TFilter = SystemParaMenu2.TFilter;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.TFilter = SystemParaMenu2.TFilter;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.TFilter = SystemParaMenu2.TFilter;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.TFilter = SystemParaMenu2.TFilter;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.TFilter = SystemParaMenu2.TFilter;}
		}
	}

}

/**
 * @brief:获得以及显示：DEST_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayDEST_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.DEST_SW == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.DEST_SW == 0)
		{
			SystemParaMenu2.DEST_SW = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffdfff)|(SystemParaMenu2.DEST_SW<<13)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);

		}
	}
	else
	{
		if(SystemParaMenu2.DEST_SW == 1)
		{
			SystemParaMenu2.DEST_SW = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffdfff)|(SystemParaMenu2.DEST_SW<<13)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：HOST_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayHOST_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.HOST_SW == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.HOST_SW == 0)
		{
			SystemParaMenu2.HOST_SW = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffbfff)|(SystemParaMenu2.HOST_SW<<14)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.HOST_SW == 1)
		{
			SystemParaMenu2.HOST_SW = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffbfff)|(SystemParaMenu2.HOST_SW<<14)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：Gamma_Tab参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayGama_TabParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		//display_u8number(Xline,Yline,SystemParaMenu2.Gama_TabVal,3,CharacterColor);
		if( SystemParaMenu2.Gama_TabVal == 0)
		{DisPlayNewStr(Xline,Yline,"70",3,CharacterColor);}
		else if( SystemParaMenu2.Gama_TabVal == 1)
		{DisPlayNewStr(Xline,Yline,"85",3,CharacterColor);}
		else if(SystemParaMenu2.Gama_TabVal== 2)
		{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else if( SystemParaMenu2.Gama_TabVal== 3)
		{DisPlayNewStr(Xline,Yline,"115",3,CharacterColor);}
		else if( SystemParaMenu2.Gama_TabVal == 4)
		{DisPlayNewStr(Xline,Yline,"125",3,CharacterColor);}
		else if( SystemParaMenu2.Gama_TabVal == 5)
		{DisPlayNewStr(Xline,Yline,"135",3,CharacterColor);}
		else if( SystemParaMenu2.Gama_TabVal == 6)
		{DisPlayNewStr(Xline,Yline,"145",3,CharacterColor);}
		load_palette(UsermenuParameter.Palette);//加载伪彩
	}
	else if(state ==1)
	{
		SystemParaMenu2.Gama_TabVal++;
		if(SystemParaMenu2.Gama_TabVal > 6)
		{
			SystemParaMenu2.Gama_TabVal = 0;
		}
	//	else
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//	display_u8number(Xline,Yline,SystemParaMenu2.Gama_TabVal,3,CharacterColor);

			if( SystemParaMenu2.Gama_TabVal == 0)
			{DisPlayNewStr(Xline,Yline,"70",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 1)
			{DisPlayNewStr(Xline,Yline,"85",3,CharacterColor);}
			else if(SystemParaMenu2.Gama_TabVal== 2)
			{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal== 3)
			{DisPlayNewStr(Xline,Yline,"115",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 4)
			{DisPlayNewStr(Xline,Yline,"125",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 5)
			{DisPlayNewStr(Xline,Yline,"135",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 6)
			{DisPlayNewStr(Xline,Yline,"145",3,CharacterColor);}
		}
		load_palette(UsermenuParameter.Palette);//加载伪彩
	}
	else
	{
		if(SystemParaMenu2.Gama_TabVal > 0)
		{
			SystemParaMenu2.Gama_TabVal--;
		}
		else
		{
			SystemParaMenu2.Gama_TabVal = 6;
		}
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			//display_u8number(Xline,Yline,SystemParaMenu2.Gama_TabVal,3,CharacterColor);
			if( SystemParaMenu2.Gama_TabVal == 0)
			{DisPlayNewStr(Xline,Yline,"70",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 1)
			{DisPlayNewStr(Xline,Yline,"85",3,CharacterColor);}
			else if(SystemParaMenu2.Gama_TabVal== 2)
			{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal== 3)
			{DisPlayNewStr(Xline,Yline,"115",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 4)
			{DisPlayNewStr(Xline,Yline,"125",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 5)
			{DisPlayNewStr(Xline,Yline,"135",3,CharacterColor);}
			else if( SystemParaMenu2.Gama_TabVal == 6)
			{DisPlayNewStr(Xline,Yline,"145",3,CharacterColor);}

		load_palette(UsermenuParameter.Palette);//加载伪彩
	}
	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.Gama_TabVal = SystemParaMenu2.Gama_TabVal;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.Gama_TabVal = SystemParaMenu2.Gama_TabVal;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Gama_TabVal = SystemParaMenu2.Gama_TabVal;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Gama_TabVal = SystemParaMenu2.Gama_TabVal;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Gama_TabVal = SystemParaMenu2.Gama_TabVal;}
		}
	}

}


/**
 * @brief:获得以及显示：Gamma参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayGammaParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Gamma,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.Gamma++;
		if(SystemParaMenu2.Gamma > 7)
		{
			SystemParaMenu2.Gamma = 7;
		}
		else
		{
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffe3ffff)|(SystemParaMenu2.Gamma<<18)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Gamma,3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.Gamma > 0)
		{
			SystemParaMenu2.Gamma--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffe3ffff)|(SystemParaMenu2.Gamma<<18)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Gamma,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.Gamma = 0;
		}
	}
	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.Gamma = SystemParaMenu2.Gamma;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.Gamma = SystemParaMenu2.Gamma;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Gamma = SystemParaMenu2.Gamma;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Gamma = SystemParaMenu2.Gamma;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Gamma = SystemParaMenu2.Gamma;}
		}
	}
}

/**
 * @brief:获得以及显示：Plat_NX参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplay_Plat_NX(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Plat_NX,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.Plat_NX++;
		if(SystemParaMenu2.Plat_NX > 64) SystemParaMenu2.Plat_NX = 64;
	}
	else
	{
		if(SystemParaMenu2.Plat_NX > 0) SystemParaMenu2.Plat_NX--;
		else							SystemParaMenu2.Plat_NX = 0;
	}
	if(state)
	{
//		APB3_REGW_Plat_NX(SystemParaMenu2.Plat_NX);
		DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.Plat_NX,3,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.Plat_NX = SystemParaMenu2.Plat_NX;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.Plat_NX = SystemParaMenu2.Plat_NX;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Plat_NX = SystemParaMenu2.Plat_NX;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Plat_NX = SystemParaMenu2.Plat_NX;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Plat_NX = SystemParaMenu2.Plat_NX;}
		}
	}

}

/**
 * @brief:获得以及显示：FK8_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplay_FK8_SW(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.FK8_SW)  DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		else   					    DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
	}
	else
	{
		SystemParaMenu2.FK8_SW = !SystemParaMenu2.FK8_SW;
	}
	if(state)
	{
//		APB3_REGW_FK8_SW(SystemParaMenu2.FK8_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		if(SystemParaMenu2.FK8_SW)  DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		else   					    DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.FK8_SW = SystemParaMenu2.FK8_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.FK8_SW = SystemParaMenu2.FK8_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.FK8_SW = SystemParaMenu2.FK8_SW;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.FK8_SW = SystemParaMenu2.FK8_SW;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.FK8_SW = SystemParaMenu2.FK8_SW;}
		}
	}
}
/**
 * @brief:获得以及显示：BAD_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayBAD_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 bad_sw = SystemParaMenu2.BAD_SW;

	vision_log_debug("GetAndDisplayBAD_SWParameter state=%u SystemParaMenu2.BAD_SW %d \n", state,SystemParaMenu2.BAD_SW);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);


	if(state)
	{
		if(bad_sw==0)
		{
			bad_sw = 1;
		}
		else if(bad_sw==1)
		{
			bad_sw = 0;
		}
		
	}


	if(bad_sw == 0){
		sprintf((ts_char*)AddBuffer, "%s", "OFF");
		
	}
	else if(bad_sw == 1){
		sprintf((ts_char*)AddBuffer, "%s", "ON");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("bad_sw %d \n",bad_sw);

	if(SystemParaMenu2.BAD_SW != bad_sw){
		SystemParaMenu2.BAD_SW = bad_sw;
		if(bad_sw==0){
			// ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3 , 1 , 1);
			ISP_bpc_off();
			
		}
		else{
			// ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3 , 0 , 1);
			ISP_bpc_on();
			
		}
	}

	
}

void GetAndDisplayBpcViewParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 bpc_view = SystemParaMenu2.m_bpc_view;

	vision_log_debug("GetAndDisplayBpcViewParameter state=%u SystemParaMenu2.m_bpc_view %d \n", state,SystemParaMenu2.m_bpc_view);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(state)
	{
		if(bpc_view==0)
		{
			bpc_view = 1;
		}
		else if(bpc_view==1)
		{
			bpc_view = 0;
		}
		
	}


	if(bpc_view == 0){
		sprintf((ts_char*)AddBuffer, "%s", "OFF");
		
	}
	else if(bpc_view == 1){
		sprintf((ts_char*)AddBuffer, "%s", "ON");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("bpc_view %d \n",bpc_view);

	if(SystemParaMenu2.m_bpc_view != bpc_view){
		SystemParaMenu2.m_bpc_view = bpc_view;
		if(bpc_view==0){
			ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3  , bpc_view, 1);
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
			cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
			cvbs_osd_refresh_display(1);
		}
		else{
			ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3  , bpc_view, 1);
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
			cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
			cvbs_osd_refresh_display(1);
		}
	}

	
}


void GetAndDisplayBpcCfgParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 bad_cfg = SystemParaMenu2.m_bpc_cfg;

	vision_log_debug("GetAndDisplayBpcCfgParameter state=%u SystemParaMenu2.m_bpc_cfg %d \n", state,SystemParaMenu2.m_bpc_cfg);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(bad_cfg >= BPC_CFG_MODE_BUTT  ){
		bad_cfg = BPC_CFG_MODE_ON;
	}

	
	if(state==1)
	{
		if(bad_cfg<BPC_CFG_MODE_BUTT-1){
			bad_cfg++;
		}
		
	}
	else if(state==2)
	{
		if(bad_cfg>0){
			bad_cfg--;
		}
		
	}


	if(bad_cfg == BPC_CFG_MODE_OFF){
		sprintf((ts_char*)AddBuffer, "%s", ":OFF");
		
	}
	else if(bad_cfg == BPC_CFG_MODE_ON){
		sprintf((ts_char*)AddBuffer, "%s", ":ON");
	}
	else if(bad_cfg == BPC_CFG_MODE_ON_EE_OFF_DDE){
		sprintf((ts_char*)AddBuffer, "%s", ":OEFD");
	}
	else if(bad_cfg == BPC_CFG_MODE_ON_EE_ON_DDE){
		sprintf((ts_char*)AddBuffer, "%s", ":OEOD");
	}
	else if(bad_cfg == BPC_CFG_MODE_OFF_EE_ON_DDE){
		sprintf((ts_char*)AddBuffer, "%s", ":FEOD");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("bad_cfg %d \n",bad_cfg);

	if(SystemParaMenu2.m_bpc_cfg != bad_cfg){
		SystemParaMenu2.m_bpc_cfg = bad_cfg;
	}

	
}





void GetAndDisplayBAD_ModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 bpc_ref_mode = SystemParaMenu2.m_bpc_ref_mode;

	vision_log_debug("GetAndDisplayBAD_ModeParameter state=%u SystemParaMenu2.m_bpc_ref_mode %d \n", state,SystemParaMenu2.m_bpc_ref_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(bpc_ref_mode >= BPC_MODE_ON_BUTT){
		bpc_ref_mode = BPC_MODE_ON_CVBS;
	}

	if(state )
	{
		if(bpc_ref_mode==BPC_MODE_ON_CVBS)
		{
			bpc_ref_mode = BPC_MODE_ON_UVC;
		}
		else if(bpc_ref_mode==BPC_MODE_ON_UVC)
		{
			bpc_ref_mode = BPC_MODE_ON_CVBS;
		}
	}

	if(bpc_ref_mode == BPC_MODE_ON_CVBS){
		sprintf((ts_char*)AddBuffer, "%s", "CVBS");
		
	}
	else if(bpc_ref_mode == BPC_MODE_ON_UVC){
		sprintf((ts_char*)AddBuffer, "%s", "UVC");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("bpc_ref_mode %d \n",bpc_ref_mode);

	if(SystemParaMenu2.m_bpc_ref_mode != bpc_ref_mode){
		SystemParaMenu2.m_bpc_ref_mode = bpc_ref_mode;
	}
	
}

/**
 * @brief:获得以及显示：Bpc_ext_SW总开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplaybpc_ext_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 BPCext_sw = SystemParaMenu2.Bpc_ext_SW;

	// vision_log_debug("GetAndDisplayBAD_SWParameter state=%u SystemParaMenu2.BAD_SW %d \n", state,SystemParaMenu2.BAD_SW);
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(state)
	{
		if(BPCext_sw==0)
		{
			BPCext_sw = 1;
		}
		else if(BPCext_sw==1)
		{
			BPCext_sw = 0;
		}
	}

	if(BPCext_sw == 0){
		sprintf((ts_char*)AddBuffer, "%s", "OFF");
	}
	else if(BPCext_sw == 1){
		sprintf((ts_char*)AddBuffer, "%s", "ON");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("BPCext_sw %d \n",BPCext_sw);

	if(SystemParaMenu2.Bpc_ext_SW != BPCext_sw){
		SystemParaMenu2.Bpc_ext_SW = BPCext_sw;
		UpdatePromptInfoWithTimeMs(" please restart system !", 1000,VOT_LOG_TYPE_MENU2);
		if(BPCext_sw>0){
			ts_tdk_extbpc_run();
		}
		else{
			ts_tdk_extbpc_pause();
		}
	}
}

/**
 * @brief:Bpc_ext_Init初始化
 * @parameter:state:0:不执行,1-2:初始化
 */
void Bpc_ext_Init(ts_u08 state)
{
	if(state)
	{
		UpdatePromptInfo("Bpc_ext init doing ...", 1,VOT_LOG_TYPE_MENU2);
		if(SystemParaMenu2.Bpc_ext_SW>0){
			for(ts_u08 i = 0;i <= EXTBPC_POSITION_MAX; i++){
				SystemParaMenu2.Extbpc_para[i].enable = 0;
				vot_snr_set_ext_bp2(i,SystemParaMenu2.Extbpc_para[i].enable,SystemParaMenu2.Extbpc_para[i].bad_x,SystemParaMenu2.Extbpc_para[i].bad_y,\
					SystemParaMenu2.Extbpc_para[i].bad_w,SystemParaMenu2.Extbpc_para[i].bad_h);
			}
			ts_cpl_msleep(500);	
		}
		
		UpdatePromptInfoWithTimeMs("Bpc_ext init completed", 1000,VOT_LOG_TYPE_MENU2);
	}
}

/**
 * @brief:Bpc_ext_Del清除
 * @parameter:state:0:不执行,1-2:清楚标记
 */
void Bpc_ext_Del(ts_u08 state)
{
	if(state)
	{
		// UpdatePromptInfo("Bpc_ext Delete doing ...", 1);
		// for(ts_u08 i=0;i<16;i++){
		// 	vot_snr_del_ext_bp(i);
		// }
		// ts_cpl_msleep(500);		
		// UpdatePromptInfoWithTimeMs("Bpc_ext Delete completed", 1000,VOT_LOG_TYPE_MENU2);

	}
}


/**
 * @brief:获得以及显示：AutoSimi参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAutoSimiParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];

	vision_log_debug("GetAndDisplayAutoSimiParameter state=%u SystemParaMenu2.Auto_Semi %d \n", state,SystemParaMenu2.Auto_Semi);
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(state)
	{
		if(state == 1){
			SystemParaMenu2.Auto_Semi++;
			if(SystemParaMenu2.Auto_Semi > 2)		{SystemParaMenu2.Auto_Semi = 2;}
		}else if(state == 2){
			if(SystemParaMenu2.Auto_Semi>0)			{SystemParaMenu2.Auto_Semi--;}
			else{SystemParaMenu2.Auto_Semi = 0;}
		}
		
	}


	if(SystemParaMenu2.Auto_Semi == 0){
		sprintf((ts_char*)AddBuffer, "%s", "OFF");		
	}
	else if(SystemParaMenu2.Auto_Semi == 1){
		sprintf((ts_char*)AddBuffer, "%s", "Min");
	}else if(SystemParaMenu2.Auto_Semi == 2){
		sprintf((ts_char*)AddBuffer, "%s", "Max");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	vision_log_debug("Auto_Semi = %d \n",SystemParaMenu2.Auto_Semi);

	switch(SystemParaMenu2.Auto_Semi)
	{
	case 0:
		if(Area_state&0x0010)
		{
			Area_state = (Area_state&0xffef)|0x0000;
			// DisPlay_BAD_Opr(&BadMes,2,0);
			BadArea_Display(0);
		}

		break;
	case 1:
	case 2:
		if((Area_state&0x0010) == 0) {
		 	Area_state = (Area_state&0xffef)|0x0010;
			select_Area = &Areainit[4];
		 	BadSemiAutoOpr(select_Area,0,AreaBad_XY_Normal);
		}
		else if ((Area_state & 0x0010) != 0) {
			BadSemiAutoOpr(&Areainit[4],0,AreaBad_XY_Normal);
		}
		break;
	}
}



/**
 * @brief:获得以及显示：BAD_Cro参数,此功能保留
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayBAD_CroParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.BAD_Cross)
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		else
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.BAD_Cross == 0)
		{
			SystemParaMenu2.BAD_Cross = 1;
			DisPlay_BAD_Cro(&badCro,2,CharacterColor);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.BAD_Cross == 1)
		{
			SystemParaMenu2.BAD_Cross = 0;
			DisPlay_BAD_Cro(&badCro,2,0);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:BAD初始化
 * @parameter:state:0:不执行,1-2:BAD初始化
 */
void Bad_Init(ts_u08 state)
{
	if(state)
	{
		BAD_Init_INC_DEC();
	}
}

#if 0
/**
 * @brief:获得以及显示：AutoSimi参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAutoSimiParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.Auto_Semi == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else if(SystemParaMenu2.Auto_Semi == 1)
		{
			DisPlayNewStr(Xline,Yline,"MIN",3,CharacterColor);
		}
		else if(SystemParaMenu2.Auto_Semi == 2)
		{
			DisPlayNewStr(Xline,Yline,"MAX",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		SystemParaMenu2.Auto_Semi++;
		if(SystemParaMenu2.Auto_Semi > 2)
		{
			SystemParaMenu2.Auto_Semi = 2;
		}
		else
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			if(SystemParaMenu2.Auto_Semi == 1)
			{
				DisPlayNewStr(Xline,Yline,"MIN",3,CharacterColor);
			}
			else if(SystemParaMenu2.Auto_Semi == 2)
			{
				DisPlayNewStr(Xline,Yline,"MAX",3,CharacterColor);
			}
		}
	}
	else
	{
		if(SystemParaMenu2.Auto_Semi > 0)
		{
			SystemParaMenu2.Auto_Semi --;
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			if(SystemParaMenu2.Auto_Semi == 0)
			{
				DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
			}
			else if(SystemParaMenu2.Auto_Semi == 1)
			{
				DisPlayNewStr(Xline,Yline,"MIN",3,CharacterColor);
			}
			else if(SystemParaMenu2.Auto_Semi == 2)
			{
				DisPlayNewStr(Xline,Yline,"MAX",3,CharacterColor);
			}
		}
		else
		{
			SystemParaMenu2.Auto_Semi = 0;
		}
	}

	switch(SystemParaMenu2.Auto_Semi)
	{
		case 0:
			if(Area_state&0x0010)
			{
				Area_state = (Area_state&0xffef)|0x0000;
				DisPlay_BAD_Opr(&BadMes,2,0);
			}
			break;
		case 1:
		case 2:
			if((Area_state&0x0010) == 0)
			{
				Area_state = (Area_state&0xffef)|0x0010;
				select_Area = &Areainit[4];
				BadSemiAutoOpr(select_Area,0,AreaBad_XY_Normal);
			}
			else
			{
				BadSemiAutoOpr(&Areainit[4],0,AreaBad_XY_Normal);
			}
			break;
	}
}

#endif

#ifdef SUPPORT_VOT_DETECTOR
/**
 * @brief:获得以及显示：DTMY_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayDTMY_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.DTMY_SW)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.DTMY_SW==0)
		{
			SystemParaMenu2.DTMY_SW = 1;
//			APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffffbff)|(SystemParaMenu2.DTMY_SW<<10)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.DTMY_SW==1)
		{
			SystemParaMenu2.DTMY_SW = 0;
//			APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffffbff)|(SystemParaMenu2.DTMY_SW<<10)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：动态盲元初始话
 * @parameter:state:1:执行初始化动作
 */
void DynamicBlindPixelInit(ts_u08 state)
{
	if(state == 1)
	{
		displayReminderInfo(Init);
		SystemParaMenu2.DTMY_Init = 1;
//		APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));
		delay_1s(1);
		SystemParaMenu2.DTMY_Init = 0;
//		APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));
		displayReminderInfo(Done);
	}
}
#endif

void GetAndDisplayMaxT_AlarmSwitchParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if 0
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(MaxTempAlarmFunSW)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(MaxTempAlarmFunSW == 0)
		{
			MaxTempAlarmFunSW = 1;
//			APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xBFFFFFFF)|(MaxTempAlarmFunSW<<30)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(MaxTempAlarmFunSW == 1)
		{
			MaxTempAlarmFunSW = 0;
//			APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xBFFFFFFF)|(MaxTempAlarmFunSW<<30)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
			DisPlayWarnIcon((ts_u08*)Aralm_Icon,340,10,4,0,0);
		}
	}
#endif
}

void GetAndDisplayMaxT_AlarmLimitParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if 0
	ts_u16 temp;
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
//		//display_ts_u16number(Xline,Yline,MaxTempLimit,3,CharacterColor);
	}
	else if(state == 1)
	{
//		MaxTempLimit = MaxTempLimit + 50;
		if(MaxTempLimit > 65535)
		{
			MaxTempLimit = 65535;
		}
//		APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xFFFFC000)|(MaxTempLimit&0x00003FFF)));
//		OverPixNum = (APB3_REGR(0xf8100098,0)&0x3FFFC000)>>14;
		DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,MaxTempLimit,3,CharacterColor);
	}
	else
	{
		if(MaxTempLimit > 50)
		{
			MaxTempLimit =  MaxTempLimit - 50;
		}
		else
		{
			MaxTempLimit = 0;
		}
//		APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xFFFFC000)|(MaxTempLimit&0x00003FFF)));
//		OverPixNum = (APB3_REGR(0xf8100098,0)&0x3FFFC000)>>14;
		DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,MaxTempLimit,3,CharacterColor);
	}
#endif
}

// void GetAndDisplayPixelLimitParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
// {
// 	// ts_u16 temp;
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
// 	if(state == 0)
// 	{
// 		//display_ts_u16number(Xline,Yline,SystemParaMenu2.pixeLimit,3,CharacterColor);
// 	}
// 	else if(state == 1)
// 	{
// 		SystemParaMenu2.pixeLimit = SystemParaMenu2.pixeLimit + 50;
// 		if(SystemParaMenu2.pixeLimit > 65535)
// 		{
// 			SystemParaMenu2.pixeLimit= 65535;
// 		}
// 		DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
// 		//display_ts_u16number(Xline,Yline,SystemParaMenu2.pixeLimit,3,CharacterColor);
// 	}
// 	else
// 	{
// 		if(SystemParaMenu2.pixeLimit > 50)
// 		{
// 			SystemParaMenu2.pixeLimit = SystemParaMenu2.pixeLimit - 50;
// 		}
// 		else
// 		{
// 			SystemParaMenu2.pixeLimit = 0;
// 		}
// 		DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
// 		//display_ts_u16number(Xline,Yline,SystemParaMenu2.pixeLimit,3,CharacterColor);
// 	}
// }

// void GetAndDisplayPixelOffsetParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
// {
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
// 	if(state == 0)
// 	{
// 		if(SystemParaMenu2.piexl_offset)
// 			display_u8number(Xline,Yline,SystemParaMenu2.piexl_offset,3,CharacterColor);
// 		else
// 			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
// 	}
// 	else if(state == 1)
// 	{
// 		if(SystemParaMenu2.piexl_offset < 255)
// 		{
// 			SystemParaMenu2.piexl_offset++;
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// 			display_u8number(Xline,Yline,SystemParaMenu2.piexl_offset,3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.piexl_offset = 255;
// 		}
// 	}
// 	else
// 	{
// 		if(SystemParaMenu2.piexl_offset > 0)
// 		{
// 			SystemParaMenu2.piexl_offset--;
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// 			if(SystemParaMenu2.piexl_offset)
// 				display_u8number(Xline,Yline,SystemParaMenu2.piexl_offset,3,CharacterColor);
// 			else
// 				DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.piexl_offset = 0;
// 		}
// 	}
// }

void GetAndDisplayPixelAutoFunctionParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
// 	if(state == 1)
// 	{
// //		displayReminderInfo(Calc);
// 		GetPixelMaxAndMinValue();
// //		displayReminderInfo(Done);
// 	}
// 	else if(state == 2)
// 	{
// //		displayReminderInfo(Do);
// 		CancalPixelMaxAndMinValue();
// //		displayReminderInfo(Done);
// 	}
}


/**
 * @brief:初始化菜单二的所有参数
 * @parameter:state:0:不执行,1_2:执行初始化的动作
 */
void InitializeTestmenuParameter(ts_u08 state)
{
	if(state)
	{
		ts_u08 m_menu2_info_tip_visible = SystemParaMenu2.m_menu2_info_tip_visible;
		menu1_default_parameters_init();
		menu2_default_parameters_init();
		Templut_Menu2_LoadDefault();
		//恢复isp出厂的默认配置
		reset_usr_isp_cfg();
		userpara_reset();
		testpara_reset();
		ISP_cut();
		ts_u08 m_menu2_info_tip_visible_tmp = SystemParaMenu2.m_menu2_info_tip_visible;
		SystemParaMenu2.m_menu2_info_tip_visible = m_menu2_info_tip_visible; 
		UpdatePromptInfoWithTimeMs("Para Init finished!", 2000,VOT_LOG_TYPE_MENU2);
		SystemParaMenu2.m_menu2_info_tip_visible = m_menu2_info_tip_visible_tmp;

	}
}

/**
 * @brief:保存菜单二的所有参数
 * @parameter:state:0:不执行,1_2:执行保持的动作
 */
void SaveTestmenuParameter(ts_u08 state)
{
	if(state)
	{
		user_isp_cfg_save();
		ISP_write_sys_cfg();	//菜单一参数保存
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		Write_TempParameter_cfg(&Temp_Parameter);
		UpdatePromptInfoWithTimeMs("save test menu para completed", 2000,VOT_LOG_TYPE_MENU2);
	}
	vision_log_debug(" SaveTestmenuParameter state %d \n",state);
}

/**
 * @brief:获得以及显示：Temp_TD参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayTemp_TDParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.Temp_TD)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.Temp_TD==0)
		{
			SystemParaMenu2.Temp_TD = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffdf)|(SystemParaMenu2.Temp_TD<<5)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.Temp_TD==1)
		{
			SystemParaMenu2.Temp_TD = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffdf)|(SystemParaMenu2.Temp_TD<<5)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示:FFT1参数:FPGA测试专用的参数1
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayFFT1Parameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.offset_X,3,CharacterColor);
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.offset_X < 255)
		{
			SystemParaMenu2.offset_X++;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffff00ff)|(SystemParaMenu2.offset_X<<8)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.offset_X,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.offset_X = 255;
		}
	}
	else
	{
		if(SystemParaMenu2.offset_X > 0)
		{
			SystemParaMenu2.offset_X--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffff00ff)|(SystemParaMenu2.offset_X<<8)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.offset_X,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.offset_X = 0;
		}
	}
}

/**
 * @brief:FFT2参数:FPGA测试专用的参数2
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayFFT2TParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.ImgT)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.ImgT==0)
		{
			SystemParaMenu2.ImgT = 1;
			#ifdef  TEST_SAVE_Image
//				APB3_REGW_IMG_1(DDR_IR_Image_T00);
//				APB3_REGW_IMG_2(DDR_IR_Image_T10);
//				APB3_REGW_IMG_3(DDR_IR_Image_T20);
//				APB3_REGW_IMG_4(DDR_IR_Image_T30);
			#else
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xfffffffb)|(SystemParaMenu2.ImgT<<2)));
			#endif

			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.ImgT==1)
		{
			SystemParaMenu2.ImgT = 0;
			#ifdef  TEST_SAVE_Image
//				APB3_REGW_IMG_1(DDR_IR_Image_T0);
//				APB3_REGW_IMG_2(DDR_IR_Image_T0);
//				APB3_REGW_IMG_3(DDR_IR_Image_T0);
//				APB3_REGW_IMG_4(DDR_IR_Image_T0);
			#else
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xfffffffb)|(SystemParaMenu2.ImgT<<2)));

			#endif

			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:TemperatureSwitch参数:测温功能开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayTemperatureSwitchParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.Usermenu_SW == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else if(SystemParaMenu2.Usermenu_SW == 1)
		{
			DisPlayNewStr(Xline,Yline,"ONE",3,CharacterColor);
		}
		else if(SystemParaMenu2.Usermenu_SW == 2)
		{
			DisPlayNewStr(Xline,Yline,"TWO",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		SystemParaMenu2.Usermenu_SW++;
		UsermenuItemTable[0].pt[1].Son = &UsermenuItemTable[2].pt[0];
		if(SystemParaMenu2.Usermenu_SW > 2)
		{
			DisPlayNewStr(Xline,Yline,"TWO",3,CharacterColor);
			SystemParaMenu2.Usermenu_SW = 2;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xffffdfff));
		}
		else
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Change...",4,6);
			if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
			{
				DisPlayNewStr(Xline,Yline,"ONE",3,CharacterColor);
//				SetFreeze_img(ON);		//图像冻结
				delay_25ms(5);
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)|0x2000));
//				Write_TestmenuParameterToFPGA();
				TemLevel_convert();
				delay_25ms(5);
//				Int_Shutter_pro();
				//delay_25ms(40);
				//Int_Shutter_pro();
//				SetFreeze_img(OFF); //图像激活
			}
			else if(SystemParaMenu2.Usermenu_SW == 2) //二档测温  0
			{
				DisPlayNewStr(Xline,Yline,"TWO",3,CharacterColor);
//				SetFreeze_img(ON);		//图像冻结
				delay_25ms(5);
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xffffdfff));
//				Write_TestmenuParameterToFPGA();

				TemLevel_convert();
//				delay_25ms(5);
				delay_25ms(20);
 				Int_Shutter_pro();
				delay_25ms(30);
				Int_Shutter_pro();
//				SetFreeze_img(OFF); //图像激活
			}
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Done",4,6);
			delay_25ms(25);
			DisplayBackground(32,400,12,BACKGROUND_LAYER,0);
		}
//		load_AD2Temp();
	}
	else
	{
		if(SystemParaMenu2.Usermenu_SW > 0)
		{
			SystemParaMenu2.Usermenu_SW --;
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Change...",4,6);
			if(SystemParaMenu2.Usermenu_SW == 0)
			{
				DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
				UsermenuItemTable[0].pt[1].Son = NULL;
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)|0x2000));
			}
			else if(SystemParaMenu2.Usermenu_SW == 1)
			{
				DisPlayNewStr(Xline,Yline,"ONE",3,CharacterColor);
				UsermenuItemTable[0].pt[1].Son = &UsermenuItemTable[2].pt[0];
//				SetFreeze_img(ON);		//图像冻结
				delay_25ms(5);
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)|0x2000));
//				Write_TestmenuParameterToFPGA();
				TemLevel_convert();
				delay_25ms(20);
 				Int_Shutter_pro();
 				delay_25ms(20);
 				Int_Shutter_pro();
//				delay_25ms(40);
//				Int_Shutter_pro();
//				SetFreeze_img(OFF); //图像激活
			}
			else if(SystemParaMenu2.Usermenu_SW == 2)
			{
				DisPlayNewStr(Xline,Yline,"TWO",3,CharacterColor);
				UsermenuItemTable[0].pt[1].Son = &UsermenuItemTable[2].pt[0];
//				SetFreeze_img(ON);		//图像冻结
				delay_25ms(5);
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xffffdfff));
//				Write_TestmenuParameterToFPGA();

				TemLevel_convert();
				delay_25ms(5);
			//	Int_Shutter_pro();
//				delay_25ms(25);
//				Int_Shutter_pro();
//				delay_25ms(35);
//				Int_Shutter_pro();
//				SetFreeze_img(OFF); //图像激活
			}
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Done",4,6);
			delay_25ms(25);
			DisplayBackground(32,400,12,BACKGROUND_LAYER,0);
		}
		else
		{
			SystemParaMenu2.Usermenu_SW = 0;
		}
//		load_AD2Temp();
	}
}

/**
 * @brief:AD_T参数:显示AD值还是温度值开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAD_TempParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.AD_T)
		{
			DisPlayNewStr(Xline,Yline,"AD",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"Temp",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.AD_T==0)
		{
			SystemParaMenu2.AD_T = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"AD",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.AD_T==1)
		{
			SystemParaMenu2.AD_T = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"Temp",3,CharacterColor);
		}
	}
}

/**
 * @brief:TemTest_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayTemTest_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.TemTest_SW == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else if(SystemParaMenu2.TemTest_SW == 1)
		{
			DisPlayNewStr(Xline,Yline,"A_INT",3,CharacterColor);
		}
		else if(SystemParaMenu2.TemTest_SW == 2)
		{
			DisPlayNewStr(Xline,Yline,"Sen",3,CharacterColor);
		}
		else if(SystemParaMenu2.TemTest_SW == 3)
		{
			DisPlayNewStr(Xline,Yline,"All",3,CharacterColor);
		}
		else if(SystemParaMenu2.TemTest_SW == 4)
		{
//			AutoGOF_Flag = 0;
			DisPlayNewStr(Xline,Yline,"NAGOF",3,CharacterColor);
		}
//		if(SystemParaMenu2.TemTest_SW<4)	{AutoGOF_Flag = 1;}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.Usermenu_SW)
		{
			SystemParaMenu2.TemTest_SW++;
			if(SystemParaMenu2.TemTest_SW > 4)
			{
				SystemParaMenu2.TemTest_SW = 4;
			}
			else
			{
				DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
				if(SystemParaMenu2.TemTest_SW == 1)
				{
					DisPlayNewStr(Xline,Yline,"A_INT",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 2)
				{
					DisPlayNewStr(Xline,Yline,"Sen",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 3)
				{
					DisPlayNewStr(Xline,Yline,"All",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 4)
				{
//					AutoGOF_Flag = 0;
					DisPlayNewStr(Xline,Yline,"NAGOF",3,CharacterColor);
				}
//				if(SystemParaMenu2.TemTest_SW<4)	{AutoGOF_Flag = 1;}
			}
		}
	}
	else
	{
		if(SystemParaMenu2.Usermenu_SW)
		{
			if(SystemParaMenu2.TemTest_SW > 0)
			{
				SystemParaMenu2.TemTest_SW --;
				DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
				if(SystemParaMenu2.TemTest_SW == 0)
				{
					DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 1)
				{
					DisPlayNewStr(Xline,Yline,"A_INT",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 2)
				{
					DisPlayNewStr(Xline,Yline,"Sen",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 3)
				{
					DisPlayNewStr(Xline,Yline,"All",3,CharacterColor);
				}
				else if(SystemParaMenu2.TemTest_SW == 4)
				{
//					AutoGOF_Flag = 0;
					DisPlayNewStr(Xline,Yline,"NAGOF",3,CharacterColor);
				}
//				if(SystemParaMenu2.TemTest_SW<4)	{AutoGOF_Flag = 1;}
			}
			else
			{
				SystemParaMenu2.TemTest_SW = 0;
			}
		}
	}
}

/**
 * @brief:NoShutter参数:无挡片功能测试：此功能暂且只用于研发测试
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayNoShutterParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if(No_Shutter == 1)
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.NoShutterFlag,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.NoShutterFlag++;
		if(SystemParaMenu2.NoShutterFlag > 24)
		{
			SystemParaMenu2.NoShutterFlag = 24;
		}
		else
		{
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.NoShutterFlag,3,CharacterColor);
//			SetFreeze_img(ON);	//图像冻结
//			delay_1ms(100);
//			SetBaking_SW(ON);	//采集本底
//
//			delay_1ms(800);
//			SetBaking_SW(OFF);	//采集本底结束
//			DisplayBackground(32,400,5,BACKGROUND_LAYER,0);
			if(!NoShutter_ExtFlag)	{shutterOpr(OFF);}
			delay_1ms(15);
			//IR_NUC_OPR(1);  //set mPrePara.BD_ID = 2;//地址待确认
			Ext_Shutter_pro();   //外校正
			delay_25ms(30);
			//IR_NUC_OPR(0); //set mPrePara.BD_ID = 0;

			DisplayBackground(32,400,9,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Writing",4,6);
			Write_NoShutterDataToFlash();//采集数据

			delay_1ms(10);
			if(!NoShutter_ExtFlag) {shutterOpr(ON);}
//			delay_1ms(100);
//			SetFreeze_img(OFF); //图像激活

			delay_1s(1);
		//	menu_clear_oneline(32,8*16,Y_START+400);
			DisplayBackground(32,400,9,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Done",4,6);
			delay_1s(1);
			DisplayBackground(32,400,9,BACKGROUND_LAYER,0);
		}
	}
	else
	{
		if(SystemParaMenu2.NoShutterFlag > 0)
		{
			SystemParaMenu2.NoShutterFlag--;
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.NoShutterFlag,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.NoShutterFlag = 24;
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.NoShutterFlag,3,CharacterColor);
		}
	}
#endif
}

/**
 * @brief:ShutFun参数:关闭打档功能函数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayShutFunParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.ShutFun)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.ShutFun == 0)
		{
			SystemParaMenu2.ShutFun = 1;
#ifdef No_Shutter
			NoShutter_ExtFlag =1;
#endif
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.ShutFun == 1)
		{
			SystemParaMenu2.ShutFun = 0;
#ifdef No_Shutter
			NoShutter_ExtFlag =0;
#endif
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:AGC_Mode参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_SETParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,UsermenuParameter.ImageMode,3,CharacterColor);
	}
	else if(state == 1)
	{
		if(UsermenuParameter.ImageAdjust == 0)
		{
			return;
		}
		else
		{
			UsermenuParameter.ImageMode++;
			if(UsermenuParameter.ImageMode>3)
				UsermenuParameter.ImageMode = 3;
			else
			{
//				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(Xline,Yline,1,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(Xline,Yline,UsermenuParameter.ImageMode,3,CharacterColor);
			}
		}
	}
	else
	{
		if(UsermenuParameter.ImageAdjust == 0)
		{
			return;
		}
		else
		{
			if(UsermenuParameter.ImageMode > 1)
			{
				UsermenuParameter.ImageMode--;
//				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(Xline,Yline,1,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(Xline,Yline,UsermenuParameter.ImageMode,3,CharacterColor);
			}
			else
			{
				UsermenuParameter.ImageMode = 1;
			}
		}
	}
}

/**
 * @brief:AGC_Scene参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_AutoParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if 0
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 0)
	{
		DisPlayNewStr(Xline,Yline,SceneTab[UsermenuParameter.ImageAdjust],3,CharacterColor);
	}
	else if(state == 1)
	{
		UsermenuParameter.ImageAdjust++;

		if(UsermenuParameter.ImageAdjust > 2)
		{
			UsermenuParameter.ImageAdjust = 0;
		}
		else
		{
			UsermenuParameter.ImageAdjust = 2;
		}
		DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,SceneTab[UsermenuParameter.ImageAdjust],3,CharacterColor);

		if(UsermenuParameter.ImageAdjust)//室内/室外：4种模式可切换
		{
			UsermenuParameter.ImageMode = 1;
//			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
//			DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);
		}
		else//手动：只能是模式0
		{
			UsermenuParameter.ImageMode = 0;
//			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
//			DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,1,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);
		}
		if(UsermenuParameter.ImageAdjust == 0)
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorM3Para.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorM3Para.Ctrs_A;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorPara.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorPara.Ctrs_A;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorJLPara.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorJLPara.Ctrs_A;}
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0xffff0000)|(SystemParaMenu2.Bri_A)));
//			DisplayBackground(TestmenuItemTable[7].pt[3].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[3].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[3].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[3].Info.line_Y,SystemParaMenu2.Bri_A,3,CharacterColor);

//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_A<<16)));
//			DisplayBackground(TestmenuItemTable[7].pt[4].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[4].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[4].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[4].Info.line_Y,SystemParaMenu2.Ctrs_A,3,CharacterColor);

//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
//			DisplayBackground(TestmenuItemTable[7].pt[5].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[5].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[5].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[5].Info.line_Y,SystemParaMenu2.Bri_M,3,CharacterColor);

//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
//			DisplayBackground(TestmenuItemTable[7].pt[6].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[6].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
//			display_u8number(TestmenuItemTable[7].pt[6].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[6].Info.line_Y,SystemParaMenu2.Ctrs_M,3,CharacterColor);
		}
		ImageAdjustSet(UsermenuParameter.ImageAdjust); //参数设置
	}
	else
	{
		if(UsermenuParameter.ImageAdjust > 0)
		{
			UsermenuParameter.ImageAdjust--;
			if(UsermenuParameter.ImageAdjust > 0) UsermenuParameter.ImageAdjust = 0;
			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,SceneTab[UsermenuParameter.ImageAdjust],3,CharacterColor);

			if(UsermenuParameter.ImageAdjust == 0)	//手动调节：只能是线性模式
			{
				UsermenuParameter.ImageMode = 0;
//				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,1,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);

				DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
				DisPlayNewStr(Xline,Yline,SceneTab[UsermenuParameter.ImageAdjust],3,CharacterColor);
				if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorM3Para.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorM3Para.Ctrs_A;}
				else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorPara.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorPara.Ctrs_A;}
				else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.Bri_A = SystemParaMenu2.ImageOutdoorJLPara.Bri_A;SystemParaMenu2.Ctrs_A = SystemParaMenu2.ImageOutdoorJLPara.Ctrs_A;}

//				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0xffff0000)|(SystemParaMenu2.Bri_A)));
				DisplayBackground(TestmenuItemTable[7].pt[3].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[3].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[3].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[3].Info.line_Y,SystemParaMenu2.Bri_A,3,CharacterColor);

//				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_A<<16)));
				DisplayBackground(TestmenuItemTable[7].pt[4].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[4].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[4].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[4].Info.line_Y,SystemParaMenu2.Ctrs_A,3,CharacterColor);

//				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
				DisplayBackground(TestmenuItemTable[7].pt[5].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[5].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[5].Info.line_X+56,Y_START+Y_GAP*TestmenuItemTable[7].pt[5].Info.line_Y,SystemParaMenu2.Bri_M,3,CharacterColor);

//				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
				DisplayBackground(TestmenuItemTable[7].pt[6].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[6].Info.line_Y,3,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[6].Info.line_X+64,Y_START+Y_GAP*TestmenuItemTable[7].pt[6].Info.line_Y,SystemParaMenu2.Ctrs_M,3,CharacterColor);
			}
			else//室内,室外调节：四种模式切换，默认是常温模式
			{
				UsermenuParameter.ImageMode = 1;
//				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,1,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);
			}
		}
		else
		{
			UsermenuParameter.ImageAdjust = 2;
			SystemParaMenu2.SceneFlag = 1;

			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,SceneTab[UsermenuParameter.ImageAdjust],3,CharacterColor);
			UsermenuParameter.ImageMode = 1;
//			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
			DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,1,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);
		}
		ImageAdjustSet(UsermenuParameter.ImageAdjust); //参数设置
	}
#endif
}
/**
 * @brief:AGC_Outdoor参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
// void GetAndDisplayAGC_OutdoorOPR(Point_MenuStructure Pt_Temp,ts_u08 state)
// {
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

// 	if(state == 0)
// 	{
// 		DisPlayNewStr(Xline,Yline,SceneOpr[SystemParaMenu2.OutdoorSel],2,CharacterColor);
// 	}
// 	else if(state == 1)
// 	{
// 		SystemParaMenu2.OutdoorSel++;
// #ifdef SUPPORT_VOT_DETECTOR
// 		if(SystemParaMenu2.OutdoorSel > 2)
// 		{
// 			SystemParaMenu2.OutdoorSel = 0;
// 		}
// #else
// 		if(SystemParaMenu2.OutdoorSel > 1)
// 		{
// 			SystemParaMenu2.OutdoorSel = 0;
// 		}
// #endif
// 	}
// 	else
// 	{
// #ifdef SUPPORT_VOT_DETECTOR
// 		if(SystemParaMenu2.OutdoorSel > 0)
// 		{
// 			SystemParaMenu2.OutdoorSel--;
// 		}
// 		else
// 		{
// 			SystemParaMenu2.OutdoorSel = 2;
// 		}
// #else
// 		if(SystemParaMenu2.OutdoorSel > 0)
// 		{
// 			SystemParaMenu2.OutdoorSel--;
// 		}
// 		else
// 		{
// 			SystemParaMenu2.OutdoorSel = 1;
// 		}
// #endif
// 	}
// 	if(state)
// 	{
// 		DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
// 		DisPlayNewStr(Xline,Yline,(ts_char*)SceneOpr[SystemParaMenu2.OutdoorSel],2,CharacterColor);
// //		if(SystemParaMenu2.OutdoorSel == 1)				APB3_REGW(IO_APB_PAT_0X35,0,((APB3_REGR(IO_APB_PAT_0X35,0)&0xfffcffff)|(0<<16))); //2025.1.2
// //		else if(SystemParaMenu2.OutdoorSel == 0)		APB3_REGW(IO_APB_PAT_0X35,0,((APB3_REGR(IO_APB_PAT_0X35,0)&0xfffcffff)|(1<<16))); //2025.1.2
// //		APB3_REGW(IO_APB_PAT_0X35,0,((APB3_REGR(IO_APB_PAT_0X35,0)&0xfffcffff)|((SystemParaMenu2.OutdoorSel&0x03)<<16))); //2025.04.02

// 		if(UsermenuParameter.ImageAdjust==2)
// 		{
// 			UsermenuParameter.ImageMode = 1;
// //			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
// //			DisplayBackground(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
// //			display_u8number(TestmenuItemTable[7].pt[1].Info.line_X+48,Y_START+Y_GAP*TestmenuItemTable[7].pt[1].Info.line_Y,UsermenuParameter.ImageMode,3,CharacterColor);
// 			ImageAdjustSet(UsermenuParameter.ImageAdjust);
// 		}
// 	}
// }


/**
 * @brief:AGC_Bri_A参数:自动亮度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
// void GetAndDisplayAGC_AutoBrightParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
// {
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
// 	if(state == 0)
// 	{
// //		display_u8number(Xline,Yline,SystemParaMenu2.Bri_A,3,CharacterColor);
// 	}
// 	else if(state == 1)
// 	{
// 		if(SystemParaMenu2.Bri_A < 255)
// 		{
// 			SystemParaMenu2.Bri_A++;
// //			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0xffff0000)|(SystemParaMenu2.Bri_A)));
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// //			display_u8number(Xline,Yline,SystemParaMenu2.Bri_A,3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.Bri_A = 255;
// 		}
// 	}
// 	else
// 	{
// 		if(SystemParaMenu2.Bri_A > 0)
// 		{
// 			SystemParaMenu2.Bri_A--;
// //			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0xffff0000)|(SystemParaMenu2.Bri_A)));
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// //			display_u8number(Xline,Yline,SystemParaMenu2.Bri_A,3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.Bri_A = 0;
// 		}
// 	}
// 	if(state)
// 	{
// 		if(UsermenuParameter.ImageAdjust ==2)//室外
// 		{
// 			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Bri_A = SystemParaMenu2.Bri_A;}
// 			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Bri_A = SystemParaMenu2.Bri_A;}
// 			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Bri_A = SystemParaMenu2.Bri_A;}
// 		}
// 	}
// }


/**
 * @brief:AGC_Ctrs_A参数:自动亮度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
// void GetAndDisplayAGC_AutoContrastParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
// {
// 	ts_u32 Xline = 0;
// 	ts_u32 Yline = 0;
// 	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
// 	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
// 	if(state == 0)
// 	{
// //		display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_A,3,CharacterColor);
// 	}
// 	else if(state == 1)
// 	{
// 		if(SystemParaMenu2.Ctrs_A < 127)
// 		{
// 			SystemParaMenu2.Ctrs_A++;
// //			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_A<<16)));
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// //			display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_A,3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.Ctrs_A = 127;
// 		}
// 	}
// 	else
// 	{
// 		if(SystemParaMenu2.Ctrs_A > 0)
// 		{
// 			SystemParaMenu2.Ctrs_A--;
// //			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_A<<16)));
// 			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
// //			display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_A,3,CharacterColor);
// 		}
// 		else
// 		{
// 			SystemParaMenu2.Ctrs_A = 0;
// 		}
// 	}
// 	if(state)
// 	{
// 		if(UsermenuParameter.ImageAdjust ==2)//室外
// 		{
// 			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Ctrs_A = SystemParaMenu2.Ctrs_A;}
// 			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Ctrs_A = SystemParaMenu2.Ctrs_A;}
// 			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Ctrs_A = SystemParaMenu2.Ctrs_A;}
// 		}
// 	}
// }


/**
 * @brief:AGC_Bri_M参数:手动亮度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_ManualBrightParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Bri_M,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.Bri_M++;
		if(SystemParaMenu2.Bri_M > 127)
		{
			SystemParaMenu2.Bri_M = 127;
		}
		else
		{
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Bri_M,3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.Bri_M)
		{
			SystemParaMenu2.Bri_M--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
			DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Bri_M,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.Bri_M  = 0;
		}
	}
}

/**
 * @brief:AGC_Ctrs_M参数：手动对比度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_ManualContrastParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_M,3,CharacterColor);
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.Ctrs_M < 127)
		{
			SystemParaMenu2.Ctrs_M++;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_M,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.Ctrs_M = 127;
		}
	}
	else
	{
		if(SystemParaMenu2.Ctrs_M > 0)
		{
			SystemParaMenu2.Ctrs_M--;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.Ctrs_M,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.Ctrs_M = 0;
		}
	}

}

/**
 * @brief:AGC_AFliter参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_AFliterParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.AGC_Filter,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.AGC_Filter++;
		if(SystemParaMenu2.AGC_Filter > 0x03)
		{
			SystemParaMenu2.AGC_Filter = 0;
		}

			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.AGC_Filter,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.AGC_Filter>0)
		{
		SystemParaMenu2.AGC_Filter--;

			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.AGC_Filter,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.AGC_Filter = 0;
		}
	}
}

/**
 * @brief:AGC_RangeLimit参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_RangeLimitParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Ran_Limit,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.Ran_Limit++;
		if(SystemParaMenu2.Ran_Limit > 0xFF)
		{
			SystemParaMenu2.Ran_Limit = 0;
		}
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xff00ffff)|(SystemParaMenu2.Ran_Limit<<16)));
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.Ran_Limit,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.Ran_Limit > 0)
		{
			SystemParaMenu2.Ran_Limit--;
		}
		else
		{
			SystemParaMenu2.Ran_Limit = 0xFF;
		}
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xff00ffff)|(SystemParaMenu2.Ran_Limit<<16)));
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.Ran_Limit,3,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.Ran_Limit = SystemParaMenu2.Ran_Limit;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.Ran_Limit = SystemParaMenu2.Ran_Limit;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.Ran_Limit = SystemParaMenu2.Ran_Limit;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.Ran_Limit = SystemParaMenu2.Ran_Limit;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.Ran_Limit = SystemParaMenu2.Ran_Limit;}
		}
	}
}

/**
 * @brief:AGC_PlatLimit参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayAGC_PlatLimitParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.PlatLimit,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.PlatLimit++;
		if(SystemParaMenu2.PlatLimit > 0x1FF)
		{
			SystemParaMenu2.PlatLimit = 0;
		}
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_IMAGE_PARA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_IMAGE_PARA,0)&0xfffffe00)|(SystemParaMenu2.PlatLimit&0x1FF))); //PlatLimit
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.PlatLimit,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.PlatLimit > 0)
		{
			SystemParaMenu2.PlatLimit--;
		}
		else
		{
			SystemParaMenu2.PlatLimit = 0x1FF;
		}
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_IMAGE_PARA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_IMAGE_PARA,0)&0xfffffe00)|(SystemParaMenu2.PlatLimit&0x1FF))); //PlatLimit
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.PlatLimit,3,CharacterColor);
	}

}
/**
 * @brief:AGC_PlatLimit参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void DisplayAGC_SW(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.AGC_SW)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.AGC_SW==0)
		{
			SystemParaMenu2.AGC_SW = 1;
//			APB3_REGW_AGC_SW(SystemParaMenu2.AGC_SW);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.AGC_SW==1)
		{
			SystemParaMenu2.AGC_SW = 0;
//			APB3_REGW_AGC_SW(SystemParaMenu2.AGC_SW);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}

}
/**
 * @brief:获得以及显示：BF_SW参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplay_BF_SW(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.BF_SW)  DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		else   					    DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
	}
	else
	{
		SystemParaMenu2.BF_SW  = !SystemParaMenu2.BF_SW;
	}
	if(state)
	{
//		APB3_REGW_BF_SW(SystemParaMenu2.BF_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		if(SystemParaMenu2.BF_SW)  DisPlayNewStr(Xline,Yline,"ON",PARAMETER_LAYER,CharacterColor);
		else   					    DisPlayNewStr(Xline,Yline,"OFF",PARAMETER_LAYER,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.BF_SW = SystemParaMenu2.BF_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.BF_SW = SystemParaMenu2.BF_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.BF_SW = SystemParaMenu2.BF_SW;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.BF_SW = SystemParaMenu2.BF_SW;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.BF_SW = SystemParaMenu2.BF_SW;}
		}
	}
}
#if 0
void GetAndDisplayZoomLensParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef FunctionAutoFocus
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.Zoom_Lens)
		{
			DisPlayNewStr(Xline,Yline,"New",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"Old",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.Zoom_Lens==0)
		{
			SystemParaMenu2.Zoom_Lens = 1;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"New",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.Zoom_Lens==1)
		{
			SystemParaMenu2.Zoom_Lens = 0;
//			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"Old",3,CharacterColor);
		}
	}
#endif
}


void GetAndDisplayLeftValueParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef FunctionAutoFocus
	ts_u16 temp;
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Left_Val,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.Left_Val ++;
		if(SystemParaMenu2.Left_Val > 65535)
		{
			SystemParaMenu2.Left_Val = 65535;
		}
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Left_Val,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.Left_Val > 0)
		{
			SystemParaMenu2.Left_Val --;
		}
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Left_Val,3,CharacterColor);
	}
#endif
}

void GetAndDisplayRightValueParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef FunctionAutoFocus
	ts_u16 temp;
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Right_Val,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.Right_Val ++;
		if(SystemParaMenu2.Right_Val > 500)
		{
			SystemParaMenu2.Right_Val = 500;
		}
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Right_Val,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.Right_Val > 0)
		{
			SystemParaMenu2.Right_Val --;
		}
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		//display_ts_u16number(Xline,Yline,SystemParaMenu2.Right_Val,3,CharacterColor);
	}
#endif
}
#endif
void GetAndDisplayOperatorSpeedParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayOperatorSpeedParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.OperatorSpeed=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.OperatorSpeed);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		SystemParaMenu2.OperatorSpeed ++;
		if(SystemParaMenu2.OperatorSpeed > 100)
		{
			SystemParaMenu2.OperatorSpeed = 100;
		}
	}
	else
	{
		if(SystemParaMenu2.OperatorSpeed > 0)
		{
			SystemParaMenu2.OperatorSpeed --;
		}
		else
			SystemParaMenu2.OperatorSpeed = 0;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.OperatorSpeed);
	Display_MenuItemLabel(Pt_Temp, is_active);


}

void GetAndDisplayFocusFullSpeedParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayFocusFullSpeedParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, FocusParameters.FocusSpeed=%d\n", Pt_Temp, state, MenuNameLen, FocusParameters.FocusSpeed);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		FocusParameters.FocusSpeed ++;
		if(FocusParameters.FocusSpeed > 100)
		{
			FocusParameters.FocusSpeed = 100;
		}
	}
	else
	{
		if(FocusParameters.FocusSpeed > 1)
		{
			FocusParameters.FocusSpeed --;
		}
		else
			FocusParameters.FocusSpeed = 1;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], FocusParameters.FocusSpeed);
	Display_MenuItemLabel(Pt_Temp, is_active);


}

void GetAndDisplayFoucs_DownNumSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	
	if(state == 0)
	{
	}
	else if(state == 1)
	{
		SystemParaMenu2.Focus_DNumSet++;
		if(SystemParaMenu2.Focus_DNumSet > 100)
		{
			SystemParaMenu2.Focus_DNumSet = 100;
		}
	}
	else
	{
		if(SystemParaMenu2.Focus_DNumSet > 1)
		{
			SystemParaMenu2.Focus_DNumSet --;
		}
		else
			SystemParaMenu2.Focus_DNumSet = 1;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Focus_DNumSet);
	Display_MenuItemLabel(Pt_Temp, is_active);


}

void GetAndDisplayFocus_UpNumSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		SystemParaMenu2.Focus_UNumSet++;
		if(SystemParaMenu2.Focus_UNumSet > 100)
		{
			SystemParaMenu2.Focus_UNumSet = 100;
		}
	}
	else
	{
		if(SystemParaMenu2.Focus_UNumSet > 1)
		{
			SystemParaMenu2.Focus_UNumSet --;
		}
		else
			SystemParaMenu2.Focus_UNumSet = 1;
	}

	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], SystemParaMenu2.Focus_UNumSet);
	Display_MenuItemLabel(Pt_Temp, is_active);


}

void GetAndDisplayCalibration_ScaleParameter(Point_MenuStructure Pt_Temp,u8 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Calibration_ScaleParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.MotorSenFlag=%d\n", Pt_Temp, state, MenuNameLen,SystemParaMenu2.MotorSenFlag);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.MotorSenFlag == 0)
		{
			SystemParaMenu2.MotorSenFlag = 1;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.MotorSenFlag == 1)
		{
			SystemParaMenu2.MotorSenFlag = 0;
		}
	}

	if(SystemParaMenu2.MotorSenFlag == 0)
	{
		sprintf((ts_char*)AddBuffer, "%s", "One");
	}
	else if(SystemParaMenu2.MotorSenFlag == 1)
	{
		sprintf((ts_char*)AddBuffer, "%s", "Mult");
		// Display_MenuItemLabel(Pt_Temp, is_active);
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);

	Display_MenuItemLabel(Pt_Temp, is_active);

}


void GetAndDisplayADDR_SelParameter(Point_MenuStructure Pt_Temp,u8 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("ADDR_SelParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.ADDR_Sel=%d\n", Pt_Temp, state, MenuNameLen,SystemParaMenu2.ADDR_Sel);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.ADDR_Sel < 2)
		{
			SystemParaMenu2.ADDR_Sel++;
		}else{
			return;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.ADDR_Sel > 0)
		{
			SystemParaMenu2.ADDR_Sel--;
		}else{
			return;
		}
	}

	if(SystemParaMenu2.ADDR_Sel == 0)
	{
		sprintf((ts_char*)AddBuffer, "%s", "MY");
	}
	else if(SystemParaMenu2.ADDR_Sel == 1)
	{
		sprintf((ts_char*)AddBuffer, "%s", "SS1");
		// Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(SystemParaMenu2.ADDR_Sel == 2)
	{
		sprintf((ts_char*)AddBuffer, "%s", "CML");
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		Motor_AddressType_Select();
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
		ts_char tip_info[64];
		snprintf(tip_info,64,"Please restart system for force %s", AddBuffer);
		UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU2);
	}

}


/**
 * @brief:报警区域开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplayA_AreasParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef AnFangProject
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if((Pt_Temp->Info.ID<0x8D0)||(Pt_Temp->Info.ID>0x8D3))
	{
		return;
	}
	ts_u08 Serial = (u8)(Pt_Temp->Info.ID - 0x8D0);
	ts_u16 Order = 1<<Serial;
	if(state == 0)
	{
		if(Area_state&Order)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
			select_Area = &Areainit[Serial];
			DisPlay_AREA_Icon(select_Area,2,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)//右启动
	{
		if((Area_state&Order) == 0)
		{
			Area_state = (Area_state&(0xffff-Order))|Order;
			Alarm_Flag = (Alarm_Flag&(0xffff-Order))|Order;
			select_Area = &Areainit[Serial];
			SystemParaMenu2.Alarm_Switch = SystemParaMenu2.Alarm_Switch|Order;
			DisPlay_AREA_Icon(select_Area,2,CharacterColor);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else//左关
	{
		if(Area_state&Order)
		{
			Area_state = (Area_state&(0xffff-Order))|0x0000;
			Alarm_Flag = (Alarm_Flag&(0xffff-Order))|0x0000;
			SystemParaMenu2.Alarm_Switch = SystemParaMenu2.Alarm_Switch&(~Order);
			ClearUncheckAreaIcon(&Areainit[Serial]);
			if(Area_state == 0)
			{
				select_Area = NULL;
			}
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
//	APB3_REGW(Hypprram_Alarm4,0,(APB3_REGR(Hypprram_Alarm4,0)&0xfffffff0)|SystemParaMenu2.Alarm_Switch);
#endif
}

void GetAndDisplayAF_ValParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef AnFangProject
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.AF_Val,3,CharacterColor);
	}
	else if(state == 1)
	{
		SystemParaMenu2.AF_Val++;
		if(SystemParaMenu2.AF_Val > 127)
		{
			SystemParaMenu2.AF_Val = 127;
		}
//		APB3_REGW_AF_Val_Range(SystemParaMenu2.AF_Val);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.AF_Val,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.AF_Val > 0)
		{
			SystemParaMenu2.AF_Val --;
		}
//		APB3_REGW_AF_Val_Range(SystemParaMenu2.AF_Val);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.AF_Val,3,CharacterColor);
	}
#endif
}

void GetAndDisplaySFDDE_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.SFDDE,3,CharacterColor);
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.SFDDE < 1)
		{
			SystemParaMenu2.SFDDE++;
//			APB3_REGW(IO_APB_PAT_0X23,0,((APB3_REGR(IO_APB_PAT_0X23,0)&0xfffffdff)|(SystemParaMenu2.SFDDE<<9)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.SFDDE,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.SFDDE = 1;
		}
	}
	else
	{
		if(SystemParaMenu2.SFDDE > 0)
		{
			SystemParaMenu2.SFDDE--;
//			APB3_REGW(IO_APB_PAT_0X23,0,((APB3_REGR(IO_APB_PAT_0X23,0)&0xfffffdff)|(SystemParaMenu2.SFDDE<<9)));
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,SystemParaMenu2.SFDDE,3,CharacterColor);
		}
		else
		{
			SystemParaMenu2.SFDDE = 0;
		}
	}
}
void GetAndDisplayAD_SetOPR(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 1)
	{
		if(SystemParaMenu2.AD_Set < 2)
		{
			SystemParaMenu2.AD_Set++;

		}
		else
		{
			SystemParaMenu2.AD_Set = 0;
		}
	}
	else if(state == 2)
	{
		if(SystemParaMenu2.AD_Set > 0)
		{
			SystemParaMenu2.AD_Set--;
		}
		else
		{
			SystemParaMenu2.AD_Set = 2;
		}
	}
	if(state)
	{
//		APB3_REGW_AGC_AD_Set(SystemParaMenu2.AD_Set);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.AD_Set,3,CharacterColor);
	}
	else
	{
		display_u8number(Xline,Yline,SystemParaMenu2.AD_Set,3,CharacterColor);
	}
}

/**
 * @brief:Sensor参数:切换sensor函数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1:参数++,2:参数--
 */
void GetAndDisplaySensorParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.sensor)
		{
			DisPlayNewStr(Xline,Yline,"GC",3,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"JK",3,CharacterColor);
		}
	}
	else if(state ==1)
	{
		if(!SystemParaMenu2.sensor)
		{
			SystemParaMenu2.sensor = 1;
			Init_TMP112();
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"GC",3,CharacterColor);
		}
	}
	else
	{
		if(SystemParaMenu2.sensor)
		{
			SystemParaMenu2.sensor = 0;
			Init_TMP112();
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"JK",3,CharacterColor);
		}
	}
}

void refreshDetMenu(void){

	if((SelectionMenu.MenuState == 0x26)&&((SelectionMenu.SelectItem->Info.ID >= 0x61)&&(SelectionMenu.SelectItem->Info.ID <= 0x69)))//刷新显示
	{
		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*1,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*1,SystemParaMenu2.GAIN,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+40,TEST_Y_START+Y_GAP*2,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+40,Y_START+Y_GAP*2,SystemParaMenu2.GOF,3,CharacterColor);
#if	SUPPORT_VOT_DETECTOR
		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*3,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*3,SystemParaMenu2.INT_C,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*6,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*6,SystemParaMenu2.NDAC_L,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*7,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*7,8-(Change_NdacValue(SystemParaMenu2.NDAC_H^0xff)+1),3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+40,TEST_Y_START+Y_GAP*5,2,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+40,Y_START+Y_GAP*5,SystemParaMenu2.OOC_Par,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*9,5,BACKGROUND_LAYER,BackgroundColor);
		switch(SystemParaMenu2.DetMode){
		default:break;
		case 0:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*9,"Auto",3,CharacterColor);  break;
		case 1:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*9,"LowT",3,CharacterColor);  break;
		case 2:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*9,"NormT",3,CharacterColor); break;
		case 3:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*9,"HighT",3,CharacterColor); break;
		case 4:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*9,"OFF",3,CharacterColor);   break;
		}
#else
		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*3,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*3,SystemParaMenu2.INT_C,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+72,TEST_Y_START+Y_GAP*5,2,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+72,Y_START+Y_GAP*5,SystemParaMenu2.OOC_Par,3,CharacterColor);

		DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*7,5,BACKGROUND_LAYER,BackgroundColor);
#if 0
		switch(SystemParaMenu2.DetMode){
		default:break;
		case 0:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*7,"Auto",3,CharacterColor);  break;
		case 1:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*7,"LowT",3,CharacterColor);  break;
		case 2:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*7,"NormT",3,CharacterColor); break;
		case 3:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*7,"HighT",3,CharacterColor); break;
		case 4:DisPlayNewStr(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*7,"OFF",3,CharacterColor);   break;
		}
#endif
#endif
	}
}

void GetAndDisplayModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
#if DET_Change_Para
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;

	// Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	// Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 DetModeNameTab[6]={0};
	ts_u08 MenuNameLen;
	ts_u08 m_DetMode = SystemParaMenu2.DetMode;
	if(m_DetMode>=DET_TEMP_MODE_BUTT){
		m_DetMode = DET_TEMP_MODE_AUTO;
	}
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayModeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d,m_DetMode %d SystemParaMenu2.DetMode=%d\n", Pt_Temp, state, MenuNameLen, m_DetMode,SystemParaMenu2.DetMode);
	if(state == 0){
		// switch(m_DetMode){
		// 	default:break;
		// 	case 0:DisPlayNewStr(Xline,Yline,"Auto",3,CharacterColor);  break;
		// 	case 1:DisPlayNewStr(Xline,Yline,"LowT",3,CharacterColor);  break;
		// 	case 2:DisPlayNewStr(Xline,Yline,"NormT",3,CharacterColor); break;
		// 	case 3:DisPlayNewStr(Xline,Yline,"HighT",3,CharacterColor); break;
		// 	case 4:DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor); break;
		// }
	
	}
	else if(state>0)
	{
		
		ISP_set_det_calibrationing_flag(1);

		ts_cpl_fix_msleep(100);
		if(state==1){
			if(m_DetMode < (DET_TEMP_MODE_BUTT-1)){	
				m_DetMode++;
			}

		}
		else{
			if(m_DetMode > 0){	
				m_DetMode--;
			}

		}
		
		vision_log_warn("m_DetMode=%d\n",  m_DetMode);
		// DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);

		ts_u08 m_DetMode_temp = m_DetMode;
		switch(m_DetMode){
			default:break;
			case DET_TEMP_MODE_AUTO:

				m_DetMode_temp = GetDetTempStatus(0);
				
				// DisPlayNewStr(Xline,Yline,"Auto",3,CharacterColor);
				break;
			case DET_TEMP_MODE_LOW:

				// DisPlayNewStr(Xline,Yline,"LowT",3,CharacterColor);
				break;
			case DET_TEMP_MODE_NORMAL:

	//
	//			SystemParaMenu2.SFDDE = 1;
	//			switch(UsermenuParameter.ImageAdjust){
	//			case 0:  //手动
	//				SystemParaMenu2.DDE_Level =SystemParaMenu2.ImageManualPara.DDELevel; break;
	//			case 1: //室内
	//				SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageIndoorPara.DDELevel; break;
	//			case 2: //室外
	//				switch(SystemParaMenu2.OutdoorSel)
	//				{
	//					case 0:  //JL
	//						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorPara.DDELevel; break;
	//					case 1:  //M3
	//						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorM3Para.DDELevel;break;
	//				}
	//				break;
	//			}

				// DisPlayNewStr(Xline,Yline,"NormT",3,CharacterColor);
				break;
			case DET_TEMP_MODE_HIGH:

	//
	//			SystemParaMenu2.SFDDE = 0;
	//			SystemParaMenu2.DDE_Level = 8;

				// DisPlayNewStr(Xline,Yline,"HighT",3,CharacterColor);
				break;
			case DET_TEMP_MODE_OFF:

	//
	//			SystemParaMenu2.SFDDE = 1;
	//			switch(UsermenuParameter.ImageAdjust){
	//			case 0:  //手动
	//				SystemParaMenu2.DDE_Level =SystemParaMenu2.ImageManualPara.DDELevel; break;
	//			case 1: //室内
	//				SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageIndoorPara.DDELevel; break;
	//			case 2: //室外
	//				switch(SystemParaMenu2.OutdoorSel)
	//				{
	//					case 0:  //JL
	//						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorPara.DDELevel; break;
	//					case 1:  //M3
	//						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorM3Para.DDELevel;break;
	//				}
	//				break;
	//			}

			
				// DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
				break;
		}//end switch

		if(m_DetMode_temp==DET_TEMP_MODE_OFF){
			SystemParaMenu2.INT_C = SystemParaMenu2.offChange.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.offChange.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.offChange.OOC_Par;
		}
		else if(m_DetMode_temp==DET_TEMP_MODE_LOW){
			SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;
		}
		else if(m_DetMode_temp==DET_TEMP_MODE_HIGH){
			SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;
		}
		else if(m_DetMode_temp==DET_TEMP_MODE_NORMAL){
			SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;
		}
		

		if( m_DetMode!=SystemParaMenu2.DetMode){
			SystemParaMenu2.DetMode = m_DetMode;
			ISP_set_det_switch_force_flag(2);
		}

		ISP_set_det_calibrationing_flag(0);
		
	}// end if(state>0)
	

	
	switch(SystemParaMenu2.DetMode){
		default:break;
		case 0:memcpy(DetModeNameTab,"Auto",4);  break;
		case 1:memcpy(DetModeNameTab,"LowT",4);  break;
		case 2:memcpy(DetModeNameTab,"NormT",5);  break;
		case 3:memcpy(DetModeNameTab,"HighT",5);  break;
		case 4:memcpy(DetModeNameTab,"OFF",3);  break;
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], DetModeNameTab, strlen((ts_char*)DetModeNameTab)+1);
	//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1] = '\0';
	Display_MenuItemLabel(Pt_Temp, is_active);
	// vision_log_debug(" SystemParaMenu2.DetMode %d  Pt_Temp->Info.NameAdd %s \n", SystemParaMenu2.DetMode,(ts_char*)Pt_Temp->Info.NameAdd );
	// vision_log_debug("GetAndDisplayModeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, SystemParaMenu2.DetMode=%d\n", Pt_Temp, state, MenuNameLen, SystemParaMenu2.DetMode);
	// vision_log_debug("SystemParaMenu2.GAIN=%d\n",SystemParaMenu2.GAIN);
	// vision_log_debug("SystemParaMenu2.GOF=%d\n",SystemParaMenu2.GOF);
	// vision_log_debug("SystemParaMenu2.INT_T=%d\n",SystemParaMenu2.INT_T);
	// vision_log_debug("SystemParaMenu2.INT_C=%d\n",SystemParaMenu2.INT_C);
	// vision_log_debug("SystemParaMenu2.OOC_Par=%d\n",SystemParaMenu2.OOC_Par);

#endif
	
}


/*********************************常温参数确认之后 点击计算 推算出高温、低温的GOS值  ********************************************/
void GetAndDisplayGosCalcParameter(Point_MenuStructure Pt_Temp,u8 state)
{
	vision_log_debug(" SystemParaMenu2.DetMode %d  state %d \n",SystemParaMenu2.DetMode,state);

	vision_log_debug(" SystemParaMenu2.GAIN %d \n",SystemParaMenu2.GAIN);
	vision_log_debug(" SystemParaMenu2.OOC_Par gos %d \n",SystemParaMenu2.OOC_Par);
	vision_log_debug(" SystemParaMenu2.LowTemp.OOC_Par %d \n",SystemParaMenu2.LowTemp.OOC_Par);
	vision_log_debug(" SystemParaMenu2.HighTemp.OOC_Par %d \n",SystemParaMenu2.HighTemp.OOC_Par);

#if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
	//normal 和  off 模式下，可以调整gos参数
	if(state && (SystemParaMenu2.DetMode == DET_MODE_NORMAL ||  SystemParaMenu2.DetMode == DET_MODE_OFF))
	{
		// vot_auto_calc_gos_for_det(DET_MODE_NORMAL);
		dector_type_t dector_type = get_powerup_dector_name_type();
    	if(dector_type!=DECTOR_TYPE_RTD){
			SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6;
			SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2;

			vision_log_warn(" enter: dector_type_t %d \n", dector_type);
			vision_log_warn(" enter: SystemParaMenu2.OOC_Par %d \n", SystemParaMenu2.OOC_Par);
			if(dector_type==DECTOR_TYPE_VOT_WLP){
				SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6+2;
				SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
				SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2+10;
			}
		}
		else{
			SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
		}
	}
#endif
}

void GetAndDisplayLensParameter(Point_MenuStructure Pt_Temp,ts_u08 state){
#if WLP
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.WLP_Lens == 0){ //9mm
			DisPlayNewStr(Xline,Yline,"9mm",3,CharacterColor);
		}else if(SystemParaMenu2.WLP_Lens == 1){ //25mm
			DisPlayNewStr(Xline,Yline,"25mm",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		if(SystemParaMenu2.WLP_Lens < 1){
			SystemParaMenu2.WLP_Lens++;
		}
	}
	else
	{
		if(SystemParaMenu2.WLP_Lens > 0){
			SystemParaMenu2.WLP_Lens--;
		}
	}

	if(SystemParaMenu2.WLP_Lens == 0){ //9mm
		SystemParaMenu2.Ran_Limit = 150;
		DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"9mm",3,CharacterColor);
	}else if(SystemParaMenu2.WLP_Lens == 1){ //25mm
		SystemParaMenu2.Ran_Limit = 180;
		DisplayBackground(Xline,Yline,4,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"25mm",3,CharacterColor);
	}
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xff00ffff)|(SystemParaMenu2.Ran_Limit<<16)));
	if(SystemParaMenu2.AlgorithmSW ==0){ //YUV模式
		if(UsermenuParameter.ImageAdjust ==0){  //手动
			SystemParaMenu2.ImageManualPara.Ran_Limit = SystemParaMenu2.Ran_Limit;
		}else if(UsermenuParameter.ImageAdjust ==1) {//室内
			SystemParaMenu2.ImageIndoorPara.Ran_Limit = SystemParaMenu2.Ran_Limit;
		}else if(UsermenuParameter.ImageAdjust ==2){//室外
			if(SystemParaMenu2.OutdoorSel){
				SystemParaMenu2.ImageOutdoorM3Para.Ran_Limit = SystemParaMenu2.Ran_Limit;
			}else{
				SystemParaMenu2.ImageOutdoorPara.Ran_Limit = SystemParaMenu2.Ran_Limit;
			}
		}
	}
#endif
}

void GetAndDisplayAlphaParameter(Point_MenuStructure Pt_Temp,ts_u08 state){
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.Alpha,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.Alpha++;
		if(SystemParaMenu2.Alpha > 128)
		{
			SystemParaMenu2.Alpha = 128;
		}
//		APB3_REGW_ALPHA_PARA(SystemParaMenu2.Alpha);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.Alpha,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.Alpha > 0)
		{
			SystemParaMenu2.Alpha--;
		}
//		APB3_REGW_ALPHA_PARA(SystemParaMenu2.Alpha);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.Alpha,3,CharacterColor);
	}
}

void GetAndDisplayJLEPSParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,SystemParaMenu2.JLEPS,3,CharacterColor);
	}
	else if(state ==1)
	{
		SystemParaMenu2.JLEPS++;
		if(SystemParaMenu2.JLEPS > 32)
		{
			SystemParaMenu2.JLEPS = 32;
		}
//		APB3_REGW_JLEPS_SW(SystemParaMenu2.JLEPS);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.JLEPS,3,CharacterColor);
	}
	else
	{
		if(SystemParaMenu2.JLEPS > 0)
		{
			SystemParaMenu2.JLEPS--;
		}else		SystemParaMenu2.JLEPS = 0;
//		APB3_REGW_JLEPS_SW(SystemParaMenu2.JLEPS);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		display_u8number(Xline,Yline,SystemParaMenu2.JLEPS,3,CharacterColor);
	}
	if(state)
	{
		if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.JL_EPS = SystemParaMenu2.JLEPS;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.JL_EPS = SystemParaMenu2.JLEPS;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.JL_EPS = SystemParaMenu2.JLEPS;}
		}
	}
}


void fire_statis_info_show(void)
{
	ts_isp_fire_statis_s_t fire_statis;
	vot_get_fire_statis(&fire_statis);
	ts_u16 u16_min_val = fire_statis.st_frm_info.u16_min_val;                       /*最小值*/
	ts_u16 u16_max_val = fire_statis.st_frm_info.u16_max_val ;                       /*最大值*/  
	ts_u16 u16_mean  = fire_statis.st_frm_info.u16_mean  ;                          /*均值*/ 
	vision_log_warn(" u16_min_val %x  u16_max_val %x u16_mean %x \n",u16_min_val,u16_max_val,u16_mean);

}


/**
 * @description: 显示探测器AD值，温度，TES参数
 * @param {int} status，0：关闭，1：显示，2：保持
 * @return {void}
 */
void GetAndDisplayIspStatisInfo(int status)
{
	// ts_s32 s32_ret;
	// static  ts_u16 ad_avg_last=0,  ad_max_last=0;
	static ts_u16 tes_value;

    ts_char statis_info[91]="\0";
	ts_char statis_info_secondline[80]="\0";
    ts_u16 ad_avg=0,  ad_max=0;
    ts_u16 temp_ad_value;
	ts_u16 bak_ad_value_u16;
    ts_f32 temp_value;
	int is_active=0;
	ts_isp_fire_statis_s_t fire_statis;

	if(global_sys_ctrl_info.m_system_mode !=  SYSTEM_MODE_NORMAL){
		if(status==2){
			return;
		}
	}

	// vision_log_debug("GetAndDisplayIspStatisInfo SelectionMenu.MenuState %d ，status %d  \n",SelectionMenu.MenuState,status);

	if(status==0){
		is_active=0;
	}
	else if(status==1){
		is_active=1;
	}
	else {
		is_active=global_sys_ctrl_info.m_bottom_osd_info_visible_flag;
	}

	//菜单二显示，且没有明确说要关闭的时候，菜单二默认开启
	if(SelectionMenu.MenuState	==	0x26 && status!=0){
		is_active=1;
	}

	if(SelectionMenu.MenuState	==	0x25){
		is_active=0;
	}
	
	//debug
	// is_active=1;
	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	vot_read_det_ad();
	temp_ad_value = global_sys_ctrl_info.m_det_temp_ad_value;
	temp_value = global_sys_ctrl_info.m_det_temp_value;
	if(is_active==1){
				
		bak_ad_value_u16 = (ts_u16)global_sys_ctrl_info.m_cur_bak_ad_mean_u32;

		tes_value = vot_get_tes_info();
		vot_get_fire_statis(&fire_statis);
		ad_avg = fire_statis.st_frm_info.u16_mean;
		ad_max = fire_statis.st_frm_info.u16_max_val;
		
		// fire_statis_info_show();
		// vision_log_debug("ad_avg [%x],ad_max [%x] \n", ad_avg,ad_max);
	
		if (SystemParaMenu2.Usermenu_SW) {
			ts_f32 temp_Sensor = READ_TMP112_Temp();
			if(res_type==DECTOR_RESOLUTION_384X288){
				ts_cpl_snprintf(statis_info,91,"ADV:%04XH Bak:%04XH AD_Max:%04XH ", ad_avg,bak_ad_value_u16, ad_max);

				ts_cpl_snprintf(statis_info_secondline,80,"Det:%04XH %+.1f SEN:%+.1f TES:%04XH", temp_ad_value,temp_value,temp_Sensor,tes_value);
			}
			else{
				ts_cpl_snprintf(statis_info,91,"ADV:%04XH Bak:%04XH AD_Max:%04XH Det:%04XH Det_T:%+.1f Sen:%+.1f TES:%04XH", ad_avg,bak_ad_value_u16, ad_max,temp_ad_value,temp_value,temp_Sensor,tes_value);
			}
		}
		else {
			if(res_type==DECTOR_RESOLUTION_384X288){
				ts_cpl_snprintf(statis_info,91,"ADV:%04XH Bak:%04XH AD_Max:%04XH ", ad_avg,bak_ad_value_u16, ad_max);
				ts_cpl_snprintf(statis_info_secondline,80,"Det:%04XH Det_T:%+.1f TES:%04XH", temp_ad_value,temp_value,tes_value);
		
			}
			else{

	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		ts_f32 ZoomTempSen = ZoomMotorSen;
		ts_cpl_snprintf(statis_info,80,"ADV:%04XH Bak:%04XH AD_Max:%04XH Z_Sen:%+.1f Det_T:%+.1f TES:%04XH", ad_avg,bak_ad_value_u16, ad_max,ZoomTempSen,temp_value,tes_value);
	}else{
				ts_cpl_snprintf(statis_info,80,"ADV:%04XH Bak:%04XH AD_Max:%04XH Det:%04XH Det_T:%+.1f TES:%04XH", ad_avg,bak_ad_value_u16, ad_max,temp_ad_value,temp_value,tes_value);
	}
			}
		}
		// vision_log_debug("statis_info [%s]", statis_info);
		UpdateMenu2StatisInfo(statis_info, is_active,0);
		if(res_type==DECTOR_RESOLUTION_384X288){
			UpdateMenu2StatisInfo(statis_info_secondline, is_active,1);
		}
	
		global_sys_ctrl_info.m_bottom_osd_info_visible_flag = 1;
	}
	else  if(is_active==0){
		if(global_sys_ctrl_info.m_bottom_osd_info_visible_flag == 1){
			UpdateMenu2StatisInfo(statis_info, 0,0);
			if(res_type==DECTOR_RESOLUTION_384X288){
				UpdateMenu2StatisInfo(statis_info, 0,1);
			}
		}
		global_sys_ctrl_info.m_bottom_osd_info_visible_flag = 0;
	}
	
}

/**
 * @description: 刷新光标的位置
 * @param {ts_nodebuf_s_t} *pst_osd_node
 * @param {ts_u32} u32_width,最大是整个cvbs设备宽度
 * @param {ts_u32} u32_height，最大是整个cvbs设备高度
 * @return {ts_void}
 */
ts_void cursor_set_position(ts_nodebuf_s_t *pst_osd_node, ts_u16 pos_x, ts_u16 pox_y)
{
    if(pst_osd_node==NULL){
            return;
    }
    // vision_log_debug("cursor_set_position enter pos_x %u pox_y %u \n", pos_x,pox_y );
 
    ts_u16 u16_left, u16_top;
    ts_osd_attr_s_t st_attr;
    // ts_s32 ret;

    ts_tdk_osd_get_attr(shdl_osd_dev,pst_osd_node, (ts_osd_attr_s_t *)&st_attr);
	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

    u16_left = pos_x % max_width;
    u16_top = pox_y % max_heigth;
    
    st_attr.u16_left = (ts_u16)u16_left;
    st_attr.u16_top = (ts_u16)u16_top;
    //vision_log_debug(" max_width %u max_heigth %u \n", max_width,max_heigth );
    // vision_log_debug(" u16_left %u u16_top %u \n", u16_left,u16_top );

    ts_tdk_osd_set_attr(shdl_osd_dev,pst_osd_node, (ts_osd_attr_s_t *)&st_attr);
    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}


void EnterOrExitBPCMaualMode(ts_u08 exit_flag)
{
	vision_log_debug("m_cvbs_display_mode %d m_system_mode %d \n", SystemParaMenu2.m_cvbs_display_mode, global_sys_ctrl_info.m_system_mode);

	cvbs_cursor_refresh_display(0);
	
	if(exit_flag == 0){
		UpdateMenu2StatisInfo("", 0,0);
		UpdateMenu2StatisInfo("", 0,1);
	}

	if(SystemParaMenu2.m_bpc_ref_mode == BPC_MODE_ON_CVBS) {
		if (SystemParaMenu2.Usermenu_SW == 0) {	
			if(exit_flag == 0){
				if( SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG){	
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
				}
			}
			else{
				if( SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ENLARGE){	
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
				}
			}
	
		}
	}
	else if(SystemParaMenu2.m_bpc_ref_mode == BPC_MODE_ON_UVC){

		dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    	vision_log_warn("res_type %d \n",res_type);
		//如果是384模式，且当前是放大模式的话，就需要重置uvc视频大小为原始分辨率
		if(res_type==DECTOR_RESOLUTION_384X288 && SystemParaMenu2.m_uvc_enlarger_mode==OUTPUT_MODE_ENLARGE_ON){
			vot_uvc_usr_reset();
		}
		
	}

	update_cvbs_vpo_attr();

	
	// vision_log_debug("m_enter_BPC_manual_flag %d is_active %d \n", global_sys_ctrl_info.m_enter_BPC_manual_flag,is_active);
	if(exit_flag==0){
		vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
		IsHide_templut_osd_display(0);
		ResetDefaultMenuDisplay();
		global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_BPC;
		GetAndDisplayBPC_Maual();
		SelectionMenu.MenuState = 0x17;
		// ISP_clr_nuc_k();

	}
	else{
		// ISP_reset_nuc_k();
		vision_log_debug("before Exit_BPC_Maual_mode \n");
		Exit_BPC_Maual_mode();
		vision_log_debug("after Exit_BPC_Maual_mode \n");
		SelectionMenu.MenuState = 0x0;
		global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
		vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);

	}
}
/** @note 菜单一的显示中心光标 */
ts_void getDisplay_Crosshair_Switch(void) 
{
	if (UsermenuParameter.m_Crosshair_OnlyDisplay != OFF && UsermenuParameter.m_Crosshair_OnlyDisplay != ON) {
		UsermenuParameter.m_Crosshair_OnlyDisplay = OFF;
	}
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_Crosshair_info_osd_dot, 0);
	ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

    if (UsermenuParameter.m_Crosshair_OnlyDisplay == 0) {
        return;
    }

    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_Crosshair_info_osd_dot, 1);
    // 获取显示分辨率
    ts_u16 display_width, display_height;
    v_getDector_DisplayResolution(&display_width, &display_height);

	if (SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768) {
		set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 960/2, 768/2);
		set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 960/2, 768/2);
		return;
	}
	else if (SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024) {
		set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 1280/2, 1024/2);
		set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 1280/2, 1024/2);
		return;
	}
	else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512) {
		set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 640/2, 512/2);
		set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 640/2, 512/2);
		return;
	}
	else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576) {
		set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 768/2, 576/2);
		set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 768/2, 576/2);
		return;
	}
	
    // CVBS显示模式处理
    if (SystemParaMenu2.m_cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE) {
        set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, (720/2)-3, 576/2);
    } else {
        set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, display_width/2, display_height/2);
    }
    
    // UVC显示模式处理
    if (SystemParaMenu2.m_uvc_enlarger_mode == OUTPUT_MODE_ENLARGE_ON) {
        if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_NONE) {
            set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, 640/2, 512/2);
        } else {
            set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, display_width/2, display_height/2);
        }
    } else {
        set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_Crosshair_info_osd_dot, display_width/2, display_height/2);
    }
}

void EnterOrExitBPCMaualModeAuto()
{
	if(global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_NORMAL){
		EnterOrExitBPCMaualMode(0);
	}
	else if(global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_BPC){
		EnterOrExitBPCMaualMode(1);
	}
}
/** @note 获取并显示半自动校点 */
void vGetAndDisplay_SemiAutoBPC(void)
{
	ISP_set_det_calibrationing_flag(1);

	cvbs_osd_refresh_display(0);
	cvbs_cursor_refresh_display(1);	
	//进入矫正，清除临时参考图
	SystemParaMenu2.m_bpc_view = 0;
	ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3  , 0, 1);

    //定制客户DSW深圳鼎深微
    if (get_customer_type() == CUSTOMER_TYPE_DSW){
		global_sys_ctrl_info.m_backup_hst_mode = UsermenuParameter.m_hst_mode;
		vot_hst_mode_set(HOT_SPOT_TRACKING_MODE_NONE);
	}

	//隐藏提示信息
	ISP_set_bottom_statistic_force_flag(1);

	ts_u16 base_x = 0;// 测试
    ts_u16 base_y = 0;
	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	/* 获取中心点 */
	ts_u16 center_pos_x = (max_width>>1) + base_x;
	ts_u16 center_pox_y = (max_heigth>>1) + base_y; 
	vision_log_debug("center_pos_x %u center_pox_y %u \n", center_pos_x,center_pox_y);
	//记录当前坐标
	// global_sys_ctrl_info.m_BPC_cursor_pos_x = center_pos_x;
	// global_sys_ctrl_info.m_BPC_cursor_pos_y = center_pox_y;
	BadMes.Area_Ly = center_pox_y - 30;
	BadMes.Area_Ry = center_pox_y + 30;
	BadMes.Area_Lx = center_pos_x - 30;
	BadMes.Area_Rx = center_pos_x + 30;
	// 坏点区域显示
	BadArea_Display(1);

	//关闭锅盖功能,设置坏点坐标后，才能立即生效
	sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
	shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);

	//备份参数
	ts_isp_cfg_reg_s_t* pst_isp_reg = ISP_get_usr_cfg_ptr();
	memcpy(&global_sys_ctrl_info.m_isp_cfg,pst_isp_reg, sizeof(ts_isp_cfg_reg_s_t) );

	//配置bpc寄存器，用于标定模式
	enter_exit_isp_bpc_adjust(1);

	ts_cpl_msleep(1000);
}
void GetAndDisplayBPC_Maual(void)
{
    //定制客户DSW深圳鼎深微
    if (get_customer_type() == CUSTOMER_TYPE_DSW){
		global_sys_ctrl_info.m_backup_hst_mode = UsermenuParameter.m_hst_mode;
		vot_hst_mode_set(HOT_SPOT_TRACKING_MODE_NONE);
	}

	ISP_set_det_calibrationing_flag(1);
	
	//进入矫正，清除临时参考图
	SystemParaMenu2.m_bpc_view = 0;
	ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3  , 0, 1);

	// vision_log_debug("is_active %d\n", is_active);
	cvbs_osd_refresh_display(0);
	cvbs_cursor_refresh_display(1);

	//隐藏提示信息
	ISP_set_bottom_statistic_force_flag(1);
	ts_u16 base_x = 0; //10;
    ts_u16 base_y = 0; //6;
	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

	ts_u16 center_pos_x = (max_width>>1) + base_x;
	ts_u16 center_pox_y = (max_heigth>>1) + base_y;

	vision_log_debug("center_pos_x %u center_pox_y %u \n", center_pos_x,center_pox_y);
	
	//记录当前坐标
	global_sys_ctrl_info.m_BPC_cursor_pos_x = center_pos_x;
	global_sys_ctrl_info.m_BPC_cursor_pos_y = center_pox_y;


	//移动光标
	BPC_Maual_mode_move_cursor(0, 0);


	//备份参数
	// ts_isp_cfg_reg_s_t* pst_isp_reg = ISP_get_usr_cfg_ptr();
	// memcpy(&global_sys_ctrl_info.m_isp_cfg,pst_isp_reg, sizeof(ts_isp_cfg_reg_s_t) );

	//关闭锅盖功能,设置坏点坐标后，才能立即生效
	sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
	shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_BACKUP);
	// ISP_cut();

	if(SystemParaMenu2.m_bpc_cfg==BPC_CFG_MODE_ON){
		load_isp_config_for_bpc();	
	}
	else if(SystemParaMenu2.m_bpc_cfg==BPC_CFG_MODE_OFF){
		bpc_mode_isp_set(0,0);
	}
	else if(SystemParaMenu2.m_bpc_cfg==BPC_CFG_MODE_ON_EE_OFF_DDE){
		bpc_mode_isp_set(0,1);
	}
	else if(SystemParaMenu2.m_bpc_cfg==BPC_CFG_MODE_ON_EE_ON_DDE){
		bpc_mode_isp_set(1,1);
	}
	else if(SystemParaMenu2.m_bpc_cfg==BPC_CFG_MODE_OFF_EE_ON_DDE){
		bpc_mode_isp_set(1,0);
	}

	//配置bpc寄存器，用于标定模式
	enter_exit_isp_bpc_adjust(1);

	ts_cpl_msleep(1000);

}

void Exit_BPC_Maual_mode(void)
{

	 //定制客户DSW深圳鼎深微
    if (get_customer_type() == CUSTOMER_TYPE_DSW){
		vot_hst_mode_set(global_sys_ctrl_info.m_backup_hst_mode);
	}

	global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;


	cvbs_cursor_refresh_display(0);
	cvbs_osd_refresh_display(1);

	reset_usr_isp_cfg();	

	enter_exit_isp_bpc_adjust(0);

	//恢复参数
	// ts_isp_cfg_reg_s_t* pst_isp_reg = ISP_get_usr_cfg_ptr();
	// memcpy(pst_isp_reg, &global_sys_ctrl_info.m_isp_cfg,sizeof(ts_isp_cfg_reg_s_t) );
 	// //注意：修改完参数后，需要以下操作才能生效
    // ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	sffc_on_off(SFFC_MODE_MANUAl, OPERATION_MODE_NONE);
	shutter_cut_on_off(SHUTTER_MODE_TIMER, OPERATION_MODE_RESTORE);
	ISP_cut();
	ISP_write_sys_menu2_cfg(SystemParaMenu2);
	ISP_write_sys_cfg();
	vot_refresh_osd_bind();
	ISP_set_det_calibrationing_flag(0);
}


/**
 * @description: 调整实际的坐标中心点，跟光标的大小和起点有关
 * @param {*}：光标原来的起点
 * @return {*}
 */
void AdjustCusorCenterPositionXY(ts_u16 *base_x , ts_u16 *base_y)
{
	if(base_x){
		(*base_x) +=3;
	}

	if(base_y){
		(*base_y) +=3;
	}

}

void MoveCusorToCenter(void)
{

	ts_u16 base_x = 0; //10;
    ts_u16 base_y = 0; //6;
	ts_u16 max_width = 640;
    ts_u16 max_heigth = 512;
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		max_width = 1280;
    	max_heigth = 1024;
	}
	#endif
	if(SystemParaMenu2.m_bpc_ref_mode== BPC_MODE_ON_CVBS){
		max_width = global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_width;
    	max_heigth = global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_height;
	}
	ts_u16 center_pos_x = (max_width>>1) + base_x;
	ts_u16 center_pox_y = (max_heigth>>1) + base_y; 

	
	//记录当前坐标
	global_sys_ctrl_info.m_BPC_cursor_pos_x = center_pos_x;
	global_sys_ctrl_info.m_BPC_cursor_pos_y = center_pox_y;

	//移动光标
	cursor_set_position(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, center_pos_x, center_pox_y);

}

/**
 * @description: 移动光标位置
 * @param {ts_u08} pos_x_status:x坐标值 0:保持， 1：-- ； 2：++
 * @param {ts_u08} pos_y_status：y坐标值 0:保持， 1：-- ； 2：++
 * @return {void}
 */
void BPC_Maual_mode_move_cursor(ts_u08 pos_x_status, ts_u08 pos_y_status)
{

	static  ts_cpl_timeval_s_t  last_move_ms={0};
	static 	ts_u16 move_step=1;

	ts_cpl_timeval_s_t  now_move_ms={0};
	ts_cpl_gettimeofday(&now_move_ms);

	ts_u32 diff_ms = ts_cpl_get_timedif(&last_move_ms, &now_move_ms);
	last_move_ms = now_move_ms;
	// vision_log_debug("diff_ms %d \n",diff_ms);

	if(diff_ms<500000){
		move_step++;
	}
	else{
		move_step=1;
	}

	// ts_tdk_osd_set_visible(global_sys_ctrl_info.pst_framebuffer_osd, 0);
	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV) {
		global_sys_ctrl_info.m_BPC_cursor_pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x % max_width;
		global_sys_ctrl_info.m_BPC_cursor_pos_y =  global_sys_ctrl_info.m_BPC_cursor_pos_y % max_heigth;
		vision_log_debug("m_BPC_cursor_pos_x = %d, m_BPC_cursor_pos_y = %d\n", global_sys_ctrl_info.m_BPC_cursor_pos_x, global_sys_ctrl_info.m_BPC_cursor_pos_y);
	}

	if(pos_x_status==1){
		if(global_sys_ctrl_info.m_BPC_cursor_pos_x>=move_step){
			global_sys_ctrl_info.m_BPC_cursor_pos_x -= move_step;
		}
		else{
			global_sys_ctrl_info.m_BPC_cursor_pos_x=0;
		}
		
	}
	else if(pos_x_status==2){
		global_sys_ctrl_info.m_BPC_cursor_pos_x+=move_step;
		if(global_sys_ctrl_info.m_BPC_cursor_pos_x>=max_width){
			global_sys_ctrl_info.m_BPC_cursor_pos_x=max_width-1; //x坐标从0开始算，最大是max_width-1
		}
	}

	if(pos_y_status==2){
		global_sys_ctrl_info.m_BPC_cursor_pos_y+=move_step;
		if(global_sys_ctrl_info.m_BPC_cursor_pos_y>=max_heigth){
			global_sys_ctrl_info.m_BPC_cursor_pos_y=max_heigth-1; //y坐标从0开始算，最大是max_heigth-1
		}
	}
	else if(pos_y_status==1){
		if(global_sys_ctrl_info.m_BPC_cursor_pos_y>=move_step){
			global_sys_ctrl_info.m_BPC_cursor_pos_y-=move_step;
		}
		else{
			global_sys_ctrl_info.m_BPC_cursor_pos_y=0;
		}
	}

	ts_u16 pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x;
	ts_u16 pos_y = global_sys_ctrl_info.m_BPC_cursor_pos_y;

	vision_log_debug("move to x,y [%d,%d]",pos_x, pos_y);
	
	//移动光标
	cursor_set_position(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, pos_x, pos_y);

	ts_char info_tip[64];
	snprintf(info_tip,64 ,"[%d,%d]", pos_x, pos_y);
	ts_u16  dst_pos_x, dst_pos_y;

	ts_u16 half_width = max_width/2;
	ts_u16 half_height = max_heigth/2;

	vision_log_warn("max_width %u max_heigth %u \n",max_width,max_heigth );
	if(pos_x>half_width){
		dst_pos_x = half_width-100;
	}
	else{
		// ts_u32 pixels_width = fontl_get_fit_pixel_width(info_tip);
		dst_pos_x = half_width + 100 ;
	}

	if(pos_y>half_height){
		dst_pos_y = half_height - 60;
	}
	else{
		dst_pos_y = half_height + 60 ;
	}

	global_sys_ctrl_info.m_prompt_cursor_pos_x = dst_pos_x;
	global_sys_ctrl_info.m_prompt_cursor_pos_y = dst_pos_y;
	UpdateSpecialInfo(info_tip, dst_pos_x, dst_pos_y, 1);

}



void BPC_Maual_mode_set_bpc(void)
{

	// ts_u16 pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x;
	// ts_u16 pos_y = global_sys_ctrl_info.m_BPC_cursor_pos_y;

	// AdjustCusorCenterPositionXY(&pos_x, &pos_y);

	// ISP_set_bp_pos(pos_x, pos_y);
	
}

void UpdateSpecialInfoWithTimeMs(ts_char *info_tip, ts_u32 u32_msecs)
{
	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 1);
	ts_cpl_msleep(u32_msecs);
	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 0);

}



void BPC_Maual_mode_set_or_unset_bpc_with_square(void)
{
	static  ts_u16 last_pos_x=0, last_pos_y=0;
	static ts_u16 is_set=0;

	// ts_u16 square_side_len = 5;

	ts_u16 pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x;
	ts_u16 pos_y = global_sys_ctrl_info.m_BPC_cursor_pos_y;

		
	AdjustCusorCenterPositionXY(&pos_x, &pos_y);

	if(pos_x!=last_pos_x || pos_y!=last_pos_y){
		is_set = 1;
	}

	if(pos_x==last_pos_x && pos_y==last_pos_y)
	{
		if(is_set==1){
			is_set = 0;
		}
		else{
			is_set = 1;
		}
	}

	ts_u16 px = pos_x-2;
	ts_u16 py = pos_y-2;
	ts_char info_tip[64];
	if(is_set==0)
	{
		for(px=pos_x-2; px<pos_x+2; px++){
			for(py=pos_y-2; py<pos_y+2; py++){
				ISP_unset_bp_pos(px, py);
			}
		}
		// ISP_unset_bp_pos(pos_x, pos_y);
		snprintf(info_tip,64 ,"[%d,%d] unset", pos_x, pos_y);
	}
	else{
		// ISP_set_bp_pos(pos_x, pos_y);
		for(px=pos_x-2; px<pos_x+2; px++){
			for(py=pos_y-2; py<pos_y+2; py++){
				ISP_set_bp_pos(px, py);
			}
		}
		snprintf(info_tip,64 ,"[%d,%d] set", pos_x, pos_y);
	}
	last_pos_x = pos_x;
	last_pos_y = pos_y;

	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 0);
	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 1);

	// ts_char msg[64];
	// snprintf(msg,64,"bpc save point [%d,%d] done ", pos_x, pos_y);
	// UpdatePromptInfoWithTimeMs(msg,2000,VOT_LOG_TYPE_MENU2);

}

void BPC_Maual_mode_set_or_unset_bpc(void)
{
	static  ts_u16 last_pos_x=0, last_pos_y=0;
	static ts_u16 is_set=0;

	ts_u16 pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x;
	ts_u16 pos_y = global_sys_ctrl_info.m_BPC_cursor_pos_y;

	if(pos_x!=last_pos_x || pos_y!=last_pos_y){
		is_set = 1;
	}

	if(pos_x==last_pos_x && pos_y==last_pos_y)
	{
		if(is_set==1){
			is_set = 0;
		}
		else{
			is_set = 1;
		}
	}

	ts_char info_tip[64];
	if(is_set==0)
	{
		ISP_unset_bp_pos(pos_x, pos_y);
	if(SystemParaMenu2.Auto_Semi ==1){
		snprintf(info_tip,64 ,"Min[%d,%d] unset", pos_x, pos_y);}
	else if(SystemParaMenu2.Auto_Semi ==2){
		snprintf(info_tip,64 ,"Max[%d,%d] unset", pos_x, pos_y);}
	else
		snprintf(info_tip,64 ,"[%d,%d] unset", pos_x, pos_y);
	}
	else{
		ISP_set_bp_pos(pos_x, pos_y);
	if(SystemParaMenu2.Auto_Semi ==1){
		snprintf(info_tip,64 ,"Min[%d,%d] set", pos_x, pos_y);}
	else if(SystemParaMenu2.Auto_Semi ==2){
		snprintf(info_tip,64 ,"Max[%d,%d] set", pos_x, pos_y);}
	else
		snprintf(info_tip,64 ,"[%d,%d] set", pos_x, pos_y);
	}
	last_pos_x = pos_x;
	last_pos_y = pos_y;

	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 0);
	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 1);

	// ts_char msg[64];
	// snprintf(msg,64,"bpc save point [%d,%d] done ", pos_x, pos_y);
	// UpdatePromptInfoWithTimeMs(msg,2000,VOT_LOG_TYPE_MENU2);

}


void BPC_Maual_mode_set_or_unset_bpc_with_auto(void)
{
	static  ts_u16 last_pos_x=0, last_pos_y=0;
	static ts_u16 is_set=0;


	ts_u16 pos_x = global_sys_ctrl_info.m_BPC_cursor_pos_x;
	ts_u16 pos_y = global_sys_ctrl_info.m_BPC_cursor_pos_y;

	if(pos_x!=last_pos_x || pos_y!=last_pos_y){
		is_set = 1;
	}

	if(pos_x==last_pos_x && pos_y==last_pos_y)
	{
		if(is_set==1){
			is_set = 0;
		}
		else{
			is_set = 1;
		}
	}

	// ts_u16 px = pos_x-2;
	// ts_u16 py = pos_y-2;
	ts_char info_tip[64];
	if(is_set==0)
	{
		snprintf(info_tip,64 ,"[%d,%d] unset", pos_x, pos_y);
	}
	else{
	
		snprintf(info_tip,64 ,"[%d,%d] set", pos_x, pos_y);
	}
	last_pos_x = pos_x;
	last_pos_y = pos_y;

	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 0);
	UpdateSpecialInfo(info_tip, global_sys_ctrl_info.m_prompt_cursor_pos_x, global_sys_ctrl_info.m_prompt_cursor_pos_y, 1);
	
	vision_log_debug("pos_y [%d] pos_x [%d] \n", pos_y,pos_x );


	//中心点一定配置进去
	//当前点灰度值满足条件就配置进去，条件宽松点
	// if(y_cur_vaild>1.1f && y_cur_vaild<0.9f)
	{
		if(is_set==0)
		{
			ISP_unset_bp_pos(pos_x,pos_y);
		}
		else{
			ISP_set_bp_pos(pos_x,pos_y);
		}
		vision_log_debug("  x [%d] y [%d] \n",pos_x,pos_y);
	}


}




void BPC_Maual_mode_set_or_unset_bpc_with_xy(ts_u16 pos_x, ts_u16 pos_y)
{
	static  ts_u16 last_pos_x=0, last_pos_y=0;
	static ts_u16 is_set=0;

	global_sys_ctrl_info.m_BPC_cursor_pos_x = pos_x;
	global_sys_ctrl_info.m_BPC_cursor_pos_y = pos_y;

	if(pos_x!=last_pos_x || pos_y!=last_pos_y){
		is_set = 1;
	}

	if(pos_x==last_pos_x && pos_y==last_pos_y)
	{
		if(is_set==1){
			is_set = 0;
		}
		else{
			is_set = 1;
		}
	}

	ts_char info_tip[64];
	if(is_set==0)
	{
		ISP_unset_bp_pos(pos_x, pos_y);
		snprintf(info_tip,64 ,"[%d,%d] unset", pos_x, pos_y);
	}
	else{
		ISP_set_bp_pos(pos_x, pos_y);
		snprintf(info_tip,64 ,"[%d,%d] set", pos_x, pos_y);
	}
	last_pos_x = pos_x;
	last_pos_y = pos_y;

	UpdateSpecialInfoWithTimeMs(info_tip, 2000);

}


//写入vot_planck1280c的Voccm寄存器配置，调整ADV值为0X2000左右
void AutomaticCalcVoccm(void)
{
	ts_u16 TempADV1,TempADV2 = 0;
	ts_u08 ErrorFlag = 0;
	ts_u16 ad_avg=0;
	ts_u16 ad_max=0;
	ts_u16 ad_min=0; 
	//、将机芯对准25℃黑体，Sel_PJ置0，调节Voccm使ADV值在2000H附近
	SystemParaMenu2.Sel_PJ=0;
	planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
	ts_cpl_msleep(1000);
	vision_log_debug(" start Voccm   %d  \n", SystemParaMenu2.Voccm);
	ts_u16 try_cnt=3;
	while(ad_avg==0 && try_cnt>0){
		try_cnt--;
		isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
		if(ad_avg>0){
			break;
		}
	}
	UpdatePromptInfo("auto calc Voccm  ing...",1,VOT_LOG_TYPE_MENU2);
	ErrorFlag = 0;
	vision_log_debug(" ad_avg   0x%x  \n", ad_avg);
	if(ad_avg>=0x2000)
	{
		while(1)
		{
			ad_avg = 0x0;
			ad_max = 0x0;
			ad_min = 0x0;
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg<0x2000){
				vision_log_warn(" ad_avg   0x%x  \n", ad_avg);
				break;
			}

			if(SystemParaMenu2.Voccm > 0)
			{
				SystemParaMenu2.Voccm	 = SystemParaMenu2.Voccm - 1;
				planck1280c_write_Voccm(SystemParaMenu2.Voccm);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Voccm  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			// isp_dump_ad_info(1280,1024, TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, 0); 
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//<2000

		SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		ts_cpl_msleep(200);
		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;

		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//>2000

		if(abs(TempADV2 - 0x2000) > abs(0x2000 - TempADV1))
		{
			SystemParaMenu2.Voccm = SystemParaMenu2.Voccm - 1;
			planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		}

	}
	else
	{
		ErrorFlag = 0;
		while(1)
		{
			isp_dump_ad_info( TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>=0x2000){
				vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
				break;
			}

			if(SystemParaMenu2.Voccm <= 63)
			{
				SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
				planck1280c_write_Voccm(SystemParaMenu2.Voccm);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Voccm  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//>2000
		SystemParaMenu2.Voccm = SystemParaMenu2.Voccm - 1;
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		ts_cpl_msleep(200);
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//<2000

		if(abs(TempADV1 - 0x2000) < abs(0x2000 - TempADV2))
		{
			SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
			planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		}
	}

DetCrcERR:
	ISP_set_det_calibrationing_flag(0);

}



//一键DET自动调整流程：
void AutomaticDetParameterConfiguration(void)
{

	dector_type_t dector_type = get_powerup_dector_name_type();
	if(dector_type==DECTOR_TYPE_RTD){
		UpdatePromptInfoWithTimeMs("auto det don't support  for rtd sensor !", 2000,VOT_LOG_TYPE_MENU2);
		return;
	}

	if(get_sensor_status()!=SENSOR_STATUS_WORKING){
		UpdatePromptInfoWithTimeMs("sensor is not vaild !", 2000,VOT_LOG_TYPE_MENU2);
		return;
	}

	if(vot_check_det_all_para()!=0){
		UpdatePromptInfoWithTimeMs("det para is illegal !", 2000,VOT_LOG_TYPE_MENU2);
		return;
	}

	//自动切换为DET_MODE_NORMAL模式，做完DET后，会恢复detMode
	ts_u08 DetModeBackup =SystemParaMenu2.DetMode;
	//测温模式，det模式只能是OFF
	if(SystemParaMenu2.Usermenu_SW) {
		SystemParaMenu2.DetMode = DET_MODE_OFF;
		vision_log_debug(" Usermenu_SW %d detMode is OFF! \n", SystemParaMenu2.Usermenu_SW);
	}else{
		SystemParaMenu2.DetMode = DET_MODE_NORMAL;
	}
	// VotDetParInit(1);
	ISP_set_det_switch_force_flag(2);
	ts_cpl_msleep(1000);

	ts_u08 sun_pt_sw = SystemParaMenu2.SunPT_SW;
	SystemParaMenu2.SunPT_SW = 0;
	ts_cpl_msleep(500);
	// ISP_set_det_calibrationing_flag(1);

	ts_u08 ShutterMode_backup =  UsermenuParameter.ShutterMode;
	UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
	ts_cpl_msleep(1000);


	ISP_set_det_calibrationing_flag(1);
#ifdef SUPPORT_VOT_DETECTOR

	// u16 TempDETAD1,TempDETAD2 = 0;
	u8 TempGOF = 0;
	u16 TempADV1,TempADV2 = 0;
	u8 ErrorFlag = 0;
	u16 threshold = 0x3FFF;

	//测温模式也不做det自动调整
	// if(SystemParaMenu2.Usermenu_SW){
	// 	vision_log_debug(" Usermenu_SW %d is no vaild ! \n", SystemParaMenu2.Usermenu_SW);
	// 	return;
	// }

	//不是OFF模式，且不是normal模式，就不做det自动调整
// #if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
// 		if(SystemParaMenu2.DetMode != DET_MODE_OFF && SystemParaMenu2.DetMode != DET_MODE_NORMAL)
// 		{
// 			vision_log_debug(" SystemParaMenu2.DetMode %d is no vaild ! \n", SystemParaMenu2.DetMode);
// 			return;
// 		}
// #endif
	UpdatePromptInfo("auto det ing ...",0,VOT_LOG_TYPE_MENU2);
	UpdatePromptInfo("auto det ing ...",1,VOT_LOG_TYPE_MENU2);
	// displayReminderInfo(Do);

//	if(SystemParaMenu2.DetMode == 4){
//		threshold = 0x3E80;
//	}else if(SystemParaMenu2.DetMode == 2){
//		threshold = 0x3FFF;
//	}

	vision_log_debug(" Usermenu_SW %d ! \n", SystemParaMenu2.Usermenu_SW);

//如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置
#ifdef CONFIG_SNR_PLANCK1280C
{
//上电初始化及准备
// 1、机芯上电至少半小时，且必须待机芯稳定后才可进行调试，当DET_T温度未有变化时即为稳定状态（如图1）；
// 2、打开相机出图，再打开红外模组生产软件（V2.3.4），选择“端口号”，打开串口（如图2）；
// 3、在菜单⼆的System→Para_Init，点击“右”进⾏功能菜单参数初始化操作（如图3）；
// 4、确认OCC、NUC开关处于关闭状态（如图4）；
// 5、在DET→Init初始化，此操作是将探测器配置字导入机芯（如图5）；
// 6、将GOF+22，然后调节Gain将TES值调至接近3FFFH（如图6），记录Gain，再调回GOF，此时TES值在3980H附近（如图7）；
// 7、将机芯对准25℃黑体，Sel_PJ置0，调节Voccm使ADV值在2000H附近（如图8）。
	//确认OCC、NUC开关处于关闭状态
	//SystemParaMenu2.OCC_SW = 0;
	SystemParaMenu2.NUC_SW = 0;//NUC开关处于关闭状态
	// ISP_clr_nuc_k();//NUC开关处于关闭状态
	// ts_cpl_msleep(1000);
	// DetParInit(1);//DET→Init初始化
	// ts_cpl_msleep(1000);
	// ISP_cut();//打挡
	//将GOF+22，然后调节Gain将TES值调至接近3FFFH（如图6），记录Gain，再调回GOF，此时TES值在3980H附近（如图7）；

	SystemParaMenu2.Sel_PJ = 1;
	SystemParaMenu2.GOF1280 = SystemParaMenu2.GOF1280+22;//82+22=104
	SystemParaMenu2.GAIN1280 = 102; //[6:0],最大7位（63）配置大一点，默认情况下，tes就是接近0x3ffff
	SystemParaMenu2.Cint1280 = 6;//3;
	SystemParaMenu2.Nint= 1128;
	TempGOF = SystemParaMenu2.GOF1280;
	vision_log_debug(" TempGOF: %d ! \n", TempGOF);
	planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
	planck1280c_write_gof(TempGOF);//将GOF+22
	planck1280c_write_gof(SystemParaMenu2.GOF1280);//将GOF+22
	planck1280c_write_cint(SystemParaMenu2.Cint1280);
	planck1280c_write_Nint(SystemParaMenu2.Nint);
	planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);//SystemParaMenu2.Sel_PJ=1;
	planck1280c_write_NTCC(SystemParaMenu2.NTCC);
	planck1280c_write_Voccm(SystemParaMenu2.Voccm);
	planck1280c_write_detConfig(SystemParaMenu2.detConfig);
	planck1280c_write_Vfid(SystemParaMenu2.Vfid);

	//然后调节Gain将TES值调至接近3FFFH
	threshold = 0x3FBF;
	ts_cpl_msleep(1000);
	ISP_cut_new();
	ts_u16 ad_tes_value = vot_get_tes_info();
	vision_log_debug(" ad_tes_value %d ! \n", ad_tes_value);
	if(ad_tes_value < threshold){
		ErrorFlag = 0;
		while(1)
		{
			ad_tes_value = vot_get_tes_info();
			//vision_log_debug("1 ad_tes_value %d ! \n", ad_tes_value);
			if(ad_tes_value >= threshold){
				break;
			}
			if(SystemParaMenu2.GAIN1280 <0x7f)
			{
				SystemParaMenu2.GAIN1280 = SystemParaMenu2.GAIN1280 + 1;
				planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
				ts_cpl_msleep(500);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
	}
	if(ErrorFlag)
	{
		ErrorFlag = 0;
		UpdatePromptInfoWithTimeMs("auto calc gain  error",2000,VOT_LOG_TYPE_MENU2);
		goto DetCrcERR;
	}
// 再调回GOF，此时TES值在
// 3980H
	threshold = 0x3980;
	SystemParaMenu2.GOF1280 = SystemParaMenu2.GOF1280-22;//gof参数配置为82
	TempGOF = SystemParaMenu2.GOF1280;
    // ISP_pause();
	planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
	planck1280c_write_gof(SystemParaMenu2.GOF1280);
	planck1280c_write_cint(SystemParaMenu2.Cint1280);
	planck1280c_write_Nint(SystemParaMenu2.Nint);
	planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
	planck1280c_write_NTCC(SystemParaMenu2.NTCC);
	planck1280c_write_Voccm(SystemParaMenu2.Voccm);
	planck1280c_write_detConfig(SystemParaMenu2.detConfig);
	planck1280c_write_Vfid(SystemParaMenu2.Vfid);
 	// ISP_run();
	// ts_cpl_msleep(1000);
	ISP_cut_new();//打挡
	ErrorFlag = 0;
	while(1)
	{
		ad_tes_value = vot_get_tes_info();
		if(ad_tes_value < threshold){
			break;
		}
		if(SystemParaMenu2.GAIN1280 > 0)
		{
			SystemParaMenu2.GAIN1280 = SystemParaMenu2.GAIN1280 - 1;
			planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
			ts_cpl_msleep(500);
		}
		else
		{
			ErrorFlag = 1;
			break;
		}
	}

	if(ErrorFlag)
	{
		ErrorFlag = 0;
		UpdatePromptInfoWithTimeMs("auto calc gain  error",2000,VOT_LOG_TYPE_MENU2);
		goto DetCrcERR;
	}

	vision_log_debug(" finish gain %d TempGOF:%d \n", SystemParaMenu2.GAIN1280, TempGOF);
	UpdatePromptInfo("auto calc gain  done",1,VOT_LOG_TYPE_MENU2);
	//、将机芯对准25℃黑体，Sel_PJ置0，调节Voccm使ADV值在2000H附近
	SystemParaMenu2.Sel_PJ=0;
	planck1280c_write_cint(SystemParaMenu2.Cint1280);
	planck1280c_write_Nint(SystemParaMenu2.Nint);
	planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);//SystemParaMenu2.Sel_PJ=0;
	planck1280c_write_NTCC(SystemParaMenu2.NTCC);
	planck1280c_write_Voccm(SystemParaMenu2.Voccm);
	planck1280c_write_detConfig(SystemParaMenu2.detConfig);
	planck1280c_write_Vfid(SystemParaMenu2.Vfid);
	ts_cpl_msleep(2000);
	ts_u16 ad_avg;
	ts_u16 ad_max;
	ts_u16 ad_min; 
	// int temp_type=0;
	ad_avg = 0x0;
	ad_max = 0x0;
	ad_min = 0x0;
	vision_log_debug(" start Voccm   %d  \n", SystemParaMenu2.Voccm);
	ts_u16 try_cnt=3;
	while(ad_avg==0 && try_cnt>0){
		try_cnt--;
		isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
		if(ad_avg>0){
			break;
		}
	}
	UpdatePromptInfo("auto calc Voccm  ing...",1,VOT_LOG_TYPE_MENU2);
	ErrorFlag = 0;
	vision_log_debug(" ad_avg   0x%x  \n", ad_avg);
	if(ad_avg>=0x2000)
	{
		while(1)
		{
			ad_avg = 0x0;
			ad_max = 0x0;
			ad_min = 0x0;
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg<0x2000){
				vision_log_warn(" ad_avg   0x%x  \n", ad_avg);
				break;
			}

			if(SystemParaMenu2.Voccm > 0)
			{
				SystemParaMenu2.Voccm	 = SystemParaMenu2.Voccm - 1;
				planck1280c_write_Voccm(SystemParaMenu2.Voccm);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Voccm  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		

		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//<2000

		SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		ts_cpl_msleep(200);
		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;

		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//>2000

		if(abs(TempADV2 - 0x2000) > abs(0x2000 - TempADV1))
		{
			SystemParaMenu2.Voccm = SystemParaMenu2.Voccm - 1;
			planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		}

	}
	else
	{
		ErrorFlag = 0;
		while(1)
		{
			isp_dump_ad_info( TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>=0x2000){
				vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
				break;
			}

			if(SystemParaMenu2.Voccm <= 63)
			{
				SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
				planck1280c_write_Voccm(SystemParaMenu2.Voccm);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  Voccm   %d  ad_avg 0x%x \n", SystemParaMenu2.Voccm,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Voccm  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//>2000
		SystemParaMenu2.Voccm = SystemParaMenu2.Voccm - 1;
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		ts_cpl_msleep(200);
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//<2000

		if(abs(TempADV1 - 0x2000) < abs(0x2000 - TempADV2))
		{
			SystemParaMenu2.Voccm = SystemParaMenu2.Voccm + 1;
			planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		}
	}
	UpdatePromptInfo("auto calc Voccm  done ",1,VOT_LOG_TYPE_MENU2);
	vision_log_debug(" SystemParaMenu2.Voccm %d \n", SystemParaMenu2.Voccm);
	ts_cpl_msleep(1000);
	ISP_cut_new();
	UpdatePromptInfo("auto calc Voccm  done ",0,VOT_LOG_TYPE_MENU2);
	SystemParaMenu2.DetMode = DetModeBackup;
	//矫正期间，立即切换
	ISP_set_det_switch_force_flag(2);
	ts_cpl_msleep(1000);
	UpdatePromptInfoWithTimeMs("auto det completed",2000,VOT_LOG_TYPE_MENU2);
	return;//直接返回，不要测温
}
#else //如果是vot_planck640c，则写入vot_planck640c的det寄存器配置
{

	// ts_u16 ad_tes_value = vot_get_tes_info();
	// vision_log_debug(" ad_tes_value %d ! \n", ad_tes_value);
	// if(ad_tes_value < threshold)
	{
		SystemParaMenu2.GOF = 77;
		SystemParaMenu2.GAIN = 54; //[6:0],最大7位（63）配置大一点，默认情况下，tes就是接近0x3ffff
		if(SystemParaMenu2.Usermenu_SW == 0) {
			if(SystemParaMenu2.DetMode == DET_MODE_NORMAL){
				SystemParaMenu2.INT_C = 3;
				SystemParaMenu2.INT_T = 45;
			}
			else if(SystemParaMenu2.DetMode == DET_MODE_OFF){
				SystemParaMenu2.INT_C = 6;
				SystemParaMenu2.INT_T = 50;
			}
		}
		SystemParaMenu2.OOC_Par = 35;
	
	}

	ts_u32 protect_flag = 0x13;

	if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
		|| SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122	
		|| SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122	
	){
		protect_flag = planck640c_read_gain_protect_flag();
		planck640c_write_gain_protect_flag(0x13);
		vision_log_debug(" FORCE_DETECTOR_TYPE_VOT_640X512_9C1/C122  FORCE_DETECTOR_TYPE_VOT_384X288_9C1/C1122 ! \n");
		threshold = 0x3FFC;
	}


	ts_cpl_msleep(1000);
	ISP_cut();
	ts_u16 ad_tes_value = vot_get_tes_info();
	vision_log_debug(" ad_tes_value %d ! \n", ad_tes_value);
	if(ad_tes_value < threshold){
		ErrorFlag = 0;
		while(1)
		{
			ad_tes_value = vot_get_tes_info();
			if(ad_tes_value >= threshold){
				break;
			}

			if(SystemParaMenu2.GAIN <0x7f)
			{
				SystemParaMenu2.GAIN = SystemParaMenu2.GAIN + 1;
				planck640c_write_gain_adc(SystemParaMenu2.GAIN);
				ts_cpl_msleep(500);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
	}

	
	if(ErrorFlag)
	{
		ErrorFlag = 0;
		UpdatePromptInfoWithTimeMs("auto calc gain  error",2000,VOT_LOG_TYPE_MENU2);
		goto DetCrcERR;
	}


	TempGOF = SystemParaMenu2.GOF;
	SystemParaMenu2.GOF = 99;

    ISP_pause();
	planck640c_write_gain_adc(SystemParaMenu2.GAIN);
	planck640c_write_gof(SystemParaMenu2.GOF);
	planck640c_write_cint(SystemParaMenu2.INT_C);
	planck640c_write_tint(SystemParaMenu2.INT_T);
    planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
	planck640c_write_gos(SystemParaMenu2.OOC_Par);
 	ISP_run();
	
	ts_cpl_msleep(1000);
	ISP_cut_new();
	ErrorFlag = 0;
	while(1)
	{
		ad_tes_value = vot_get_tes_info();
		if(ad_tes_value < threshold){
			break;
		}

		if(SystemParaMenu2.GAIN > 0)
		{
			SystemParaMenu2.GAIN = SystemParaMenu2.GAIN - 1;
			planck640c_write_gain_adc(SystemParaMenu2.GAIN);
			ts_cpl_msleep(500);
		}
		else
		{
			ErrorFlag = 1;
			break;
		}
	}

	if(ErrorFlag)
	{
		ErrorFlag = 0;
		UpdatePromptInfoWithTimeMs("auto calc gain  error",2000,VOT_LOG_TYPE_MENU2);
		goto DetCrcERR;
	}

	//gof参数配置为77
	SystemParaMenu2.GOF = TempGOF;
	planck640c_write_gof(SystemParaMenu2.GOF);
	if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1){
		if(protect_flag!=0x4){
			protect_flag=0x4;
		}
		planck640c_write_gain_protect_flag(protect_flag);
	}
	else if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122){
		if(protect_flag!=0x1){
			protect_flag=0x1;
		}
		planck640c_write_gain_protect_flag(protect_flag);
	}

	vision_log_debug(" finish gain:%d TempGOF:%d \n", SystemParaMenu2.GAIN, TempGOF);
	UpdatePromptInfo("auto calc gain  done",1,VOT_LOG_TYPE_MENU2);

#if DET_Change_Para
	if(SystemParaMenu2.Usermenu_SW == 0) {
		if(SystemParaMenu2.DetMode == DET_MODE_OFF){
			SystemParaMenu2.INT_C = 6;
			SystemParaMenu2.INT_T = 50;
		}else if(SystemParaMenu2.DetMode == DET_MODE_NORMAL){
			SystemParaMenu2.INT_C = 3;
			SystemParaMenu2.INT_T = 45;
		}
	}
#else
	SystemParaMenu2.INT_C = 6;
	SystemParaMenu2.INT_T = 50;
#endif

	if(SystemParaMenu2.INT_T>=77){
		SystemParaMenu2.INT_T = 76;
	}
	planck640c_write_cint(SystemParaMenu2.INT_C);
	planck640c_write_tint(SystemParaMenu2.INT_T);
	planck640c_write_rst_data(77-SystemParaMenu2.INT_T);

	ts_cpl_msleep(500);
	ISP_cut_new();
	ts_cpl_msleep(1000);

	ts_u16 ad_avg;
	ts_u16 ad_max;
	ts_u16 ad_min; 
	// int temp_type=0;
	ad_avg = 0x0;
	ad_max = 0x0;
	ad_min = 0x0;

	vision_log_debug(" start gos   %d  \n", SystemParaMenu2.OOC_Par);
	ts_u16 try_cnt=3;
	while(ad_avg==0 && try_cnt>0){
		try_cnt--;
		isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
		if(ad_avg>0){
			break;
		}
	}

	UpdatePromptInfo("auto calc gos  ing...",1,VOT_LOG_TYPE_MENU2);
	ErrorFlag = 0;
	vision_log_debug(" ad_avg   0x%x  \n", ad_avg);

	if(ad_avg>=0x2000)
	{
		while(1)
		{
			ad_avg = 0x0;
			ad_max = 0x0;
			ad_min = 0x0;
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg<0x2000){
				vision_log_warn(" ad_avg   0x%x  \n", ad_avg);
				break;
			}

			if(SystemParaMenu2.OOC_Par > 0)
			{
				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
				planck640c_write_gos(SystemParaMenu2.OOC_Par);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  gos   %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc gos  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		

		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info( TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//<2000

		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		ts_cpl_msleep(200);
		ad_avg = 0x0;
		ad_max = 0x0;
		ad_min = 0x0;

		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//>2000

		if(abs(TempADV2 - 0x2000) > abs(0x2000 - TempADV1))
		{
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
			planck640c_write_gos(SystemParaMenu2.OOC_Par);
		}

	}
	else
	{
		ErrorFlag = 0;
		while(1)
		{

			isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>=0x2000){
				vision_log_debug("  gos   %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
				break;
			}

			if(SystemParaMenu2.OOC_Par <= 63)
			{
				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
				planck640c_write_gos(SystemParaMenu2.OOC_Par);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  gos   %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc gos  error",3000,VOT_LOG_TYPE_MENU2);
			goto DetCrcERR;
		}
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV1 = ad_avg;//>2000

		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		ts_cpl_msleep(200);
		try_cnt = 3;
		while(ad_avg==0 && try_cnt>0){
			try_cnt--;
			isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL);
			if(ad_avg>0){
				break;
			}
		}
		TempADV2 = ad_avg;//<2000

		if(abs(TempADV1 - 0x2000) < abs(0x2000 - TempADV2))
		{
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
			planck640c_write_gos(SystemParaMenu2.OOC_Par);
		}
	}


	UpdatePromptInfo("auto calc gos  done ",1,VOT_LOG_TYPE_MENU2);


#if DET_Change_Para
 	// vot_auto_calc_gos_for_det(DET_MODE_NORMAL);
    dector_type = get_powerup_dector_name_type();
    if(dector_type!=DECTOR_TYPE_RTD){
		if(SystemParaMenu2.DetMode == DET_MODE_OFF){
			SystemParaMenu2.offChange.INT_C = SystemParaMenu2.INT_C;
			SystemParaMenu2.offChange.INT_T = SystemParaMenu2.INT_T;
			SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
		}else if(SystemParaMenu2.DetMode == DET_MODE_NORMAL){
			SystemParaMenu2.NormalTemp.INT_C = SystemParaMenu2.INT_C;
			SystemParaMenu2.NormalTemp.INT_T = SystemParaMenu2.INT_T;
			SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6;
			SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2;
		}

		vision_log_warn(" enter: dector_type_t %d \n", dector_type);
		vision_log_warn(" enter: SystemParaMenu2.OOC_Par %d \n", SystemParaMenu2.OOC_Par);
		if(dector_type==DECTOR_TYPE_VOT_WLP){
			SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6+2;
			SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
			SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2+10;
		}
	}
  	else{ //rtd
        SystemParaMenu2.LowTemp.INT_C = SystemParaMenu2.INT_C;
        SystemParaMenu2.NormalTemp.INT_C = SystemParaMenu2.INT_C;
        SystemParaMenu2.HighTemp.INT_C = SystemParaMenu2.INT_C;
        SystemParaMenu2.offChange.INT_C = SystemParaMenu2.INT_C;

        SystemParaMenu2.LowTemp.INT_T = SystemParaMenu2.INT_T;
        SystemParaMenu2.NormalTemp.INT_T = SystemParaMenu2.INT_T;
        SystemParaMenu2.HighTemp.INT_T = SystemParaMenu2.INT_T;
        SystemParaMenu2.offChange.INT_T = SystemParaMenu2.INT_T;

        SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
    }

#endif

	ts_cpl_msleep(1000);
	ISP_cut_new();
	vision_log_debug(" SystemParaMenu2.OOC_Par %d \n",SystemParaMenu2.OOC_Par);
	vision_log_debug(" SystemParaMenu2.GAIN %d \n",SystemParaMenu2.GAIN);

	UpdatePromptInfo("auto calc gos  done ",0,VOT_LOG_TYPE_MENU2);
	SystemParaMenu2.DetMode = DetModeBackup;
		// VotDETParSelect();

	//矫正期间，立即切换
	ISP_set_det_switch_force_flag(2);
	ts_cpl_msleep(1000);

	// Int_Shutter_pro();  //test use
	// displayReminderInfo(Done);
	 UpdatePromptInfoWithTimeMs("auto det completed",2000,VOT_LOG_TYPE_MENU2);
}	
#endif
#endif

DetCrcERR:
	det_gos_switch_force_last = SystemParaMenu2.OOC_Par;
	ISP_set_det_calibrationing_flag(0);

	//恢复阳光保护的开关状态
	SystemParaMenu2.SunPT_SW = sun_pt_sw;
	UsermenuParameter.ShutterMode = ShutterMode_backup;

	if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
	{
		SystemParaMenu2.OOC_Par_Val[0] = SystemParaMenu2.OOC_Par ;
	}
	else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
	{
		SystemParaMenu2.OOC_Par_Val[1] = SystemParaMenu2.OOC_Par;
	}
	else if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
	{
		SystemParaMenu2.OOC_Par_Val[2] = SystemParaMenu2.OOC_Par;
	}
 	//保存配置
    ISP_write_sys_menu2_cfg(SystemParaMenu2);
}


//一键OCC自动调整流程：
void AutomaticOccParameterConfiguration(void)
{
	//关闭自动打档
	// ts_u08 ShutterMode_backup =  UsermenuParameter.ShutterMode;
	// UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
	// ts_cpl_msleep(1000);
	ISP_set_det_calibrationing_flag(1);
	ts_u08 sun_pt_sw = SystemParaMenu2.SunPT_SW;
	SystemParaMenu2.SunPT_SW = 0;
	ts_cpl_fix_msleep(300);
	UpdatePromptInfoWithTimeMs("OCC_function start",0,VOT_LOG_TYPE_MENU2);
	//如果是vot_planck1280c，走vot_planck1280c的OCC流程
	#ifdef CONFIG_SNR_PLANCK1280C
	{
//OCC校正
// 1、将机芯对准25℃黑体，OCC_Mode关闭状态下进行校正工作；
// 2、先进行OCC初始化，选择“Init”点击右键（如图1）；
// 3、选择“Calc”，点击右键，进行OCC校正计算（如图2）；
// 4、选择“Save”，点击右键，进行OCC校正数据保存（如图3）；
// 5、在DET菜单下Sel_PJ置1（如图4）
		OCC_Init(1);
		OCC_Calculate(1);
		OCC_Save(1);
		AutomaticCalcVoccm();
		SystemParaMenu2.Sel_PJ = 1;
		ISP_pause();
		planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);//SystemParaMenu2.Sel_PJ=1;
		ISP_run();
		ts_cpl_msleep(1000);
		ISP_cut();//打挡
	}
	#else //vot_planck640c，走原来的OCC流程
	 {
		OCC_Init(1);
		OCC_Calculate(1);
		OCC_Save(1);
	 }
  	#endif
	UpdatePromptInfoWithTimeMs("OCC_function completed",1000,VOT_LOG_TYPE_MENU2);

	// UsermenuParameter.ShutterMode = ShutterMode_backup;
	ISP_set_det_calibrationing_flag(0);
	//恢复阳光保护的开关状态
	SystemParaMenu2.SunPT_SW = sun_pt_sw;
}

void Automatic_Temp_GOS_Configuration(void)
{
	ts_u16 ad_avg;
	ts_u16 ad_max;
	ts_u16 ad_min; 
	u16 TempADV1,TempADV2 = 0;
	u8 ErrorFlag = 0;

	/* 阳光保护开关 */
	ts_u08 sun_pt_sw = SystemParaMenu2.SunPT_SW;
	SystemParaMenu2.SunPT_SW = 0;
	/* 设置机芯正在标定状态 */
	ISP_set_det_calibrationing_flag(1);
	/* 挡片模式 */
	ts_u08 Shutter_backup =  UsermenuParameter.ShutterMode;
	UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
	ts_cpl_msleep(500);
	
	vision_log_debug(" start Temp_gos:   %d  \n", SystemParaMenu2.OOC_Par);
	isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 

	UpdatePromptInfo("auto calc Temp_gos ing...",1,VOT_LOG_TYPE_MENU2);
	ErrorFlag = 0;
	vision_log_debug("Temp ad_avg   0x%x  \n", ad_avg);

	if(ad_avg>=0x1500)
	{
		while(1)
		{
			isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg<0x1500){
				vision_log_warn(" ad_avg   0x%x  \n", ad_avg);
				break;
			}

			if(SystemParaMenu2.OOC_Par > 0)
			{
				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
				planck640c_write_gos(SystemParaMenu2.OOC_Par);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("  Temp_gos   %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Temp_gos error",3000,VOT_LOG_TYPE_MENU2);
		}
		
		isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL); 
		TempADV1 = ad_avg;//<1500

		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		ts_cpl_msleep(200);
		isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
		TempADV2 = ad_avg;//>1500

		if(abs(TempADV2 - 0x1500) > abs(0x1500 - TempADV1))
		{
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
			planck640c_write_gos(SystemParaMenu2.OOC_Par);
		}

	}
	else
	{
		ErrorFlag = 0;
		while(1)
		{

			isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL); 
			if(ad_avg>=0x1500){
				vision_log_debug("Temp_gos   %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
				break;
			}

			if(SystemParaMenu2.OOC_Par <= 63)
			{
				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
				planck640c_write_gos(SystemParaMenu2.OOC_Par);
				ts_cpl_msleep(200);
			}
			else
			{
				ErrorFlag = 1;
				break;
			}
		}
		if(ErrorFlag)
		{
			ErrorFlag = 0;
			vision_log_debug("Temp_gos %d  ad_avg 0x%x \n", SystemParaMenu2.OOC_Par,ad_avg);
			UpdatePromptInfoWithTimeMs("auto calc Temp_gos error",3000,VOT_LOG_TYPE_MENU2);
		}
		isp_dump_ad_info(TS_UPL_POS_RAW ,  &ad_avg , &ad_max, &ad_min, NULL);
		TempADV1 = ad_avg;//>1500

		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		ts_cpl_msleep(200);
		isp_dump_ad_info(TS_UPL_POS_RAW,  &ad_avg , &ad_max, &ad_min, NULL);
		TempADV2 = ad_avg;//<1500

		if(abs(TempADV1 - 0x1500) < abs(0x1500 - TempADV2))
		{
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
			planck640c_write_gos(SystemParaMenu2.OOC_Par);
		}
	}
	ts_cpl_msleep(1000);
	ISP_cut();

	ISP_set_det_calibrationing_flag(0);
	//恢复阳光保护的开关状态
	SystemParaMenu2.SunPT_SW = sun_pt_sw;
	UsermenuParameter.ShutterMode = Shutter_backup;
	
	if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
	{
		SystemParaMenu2.OOC_Par_Val[0] = SystemParaMenu2.OOC_Par ;
	}
	else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
	{
		SystemParaMenu2.OOC_Par_Val[1] = SystemParaMenu2.OOC_Par;
	}
	else if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
	{
		SystemParaMenu2.OOC_Par_Val[2] = SystemParaMenu2.OOC_Par;
	}
	//保存配置
    ISP_write_sys_menu2_cfg(SystemParaMenu2);
	UpdatePromptInfo("auto calc Temp_gos done", 0,VOT_LOG_TYPE_MENU2);
	// vision_log_debug(" SystemParaMenu2.OOC_Par %d \n",SystemParaMenu2.OOC_Par);
}

//NUC校正
// 1、先进行NUC初始化，选择“Init”点击右键（如图1）；
// 2、将Sel置为Black，将机芯对准30℃黑体（如图2）；
// 3、选择“Clac”，点击左键，待计算完成后，将机芯对准60℃黑体，再“Calc”右键（如图3）；
// 4、选择“NUC_SW”，点击右键，打开NUC开关（如图4）；
// 5、选择“BPC”→“BAD_SW”，点击右键，打开坏点开关（如图5）；
// 6、选择“NUC”→“Save”，点击右键，进行NUC校正数据保存（如图6）。



//标定顺序
//1、上位机 All初始化 2、一键DET，3、一键OCC， 4、NUC校正校准，5、SFFC 校准，6、坏点矫正，7、SFFC刷新，8、All保存完成
