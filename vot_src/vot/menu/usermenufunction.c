/*
 * menufunction.c
 *
 *  Created on: 2024年3月14日
 *      Author: Administrator
 */

#include <base/ts_type.h>
#include"votmenu.h"
#include"usermenu.h"
#include"name.h"
#include "ispfunction.h"
#include "votmenu.h"
#include "usermenufunction.h"
#include <menu/testmenufunction.h>
#include <menu/usermenu.h>
#include <menu/votmain.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

extern SysCtrlInfo_s_t global_sys_ctrl_info;
extern ts_mdev_t shdl_osd_dev;
extern struct _Usermenu_Parameter UsermenuParameter;
ts_u08 CharacterColor,BackgroundColor;
float ShutterTime;
ts_u16 ShuttertTimeMIN;
const char *Focusslect[4] = {".FP",".ZP",".FE",".ZE"};
/********************************************************************************/
/*
 * 以下每一项函数对应每一个菜单的功能函数
 * 以下将每个功能函数单独列出来的原因：
 * 1、为了后面方便维护.
 * 2、增加代码的可读性.
 * 3、此种做法,无疑是增加了Flash的开销,增加了代码量,是比较冗余的写法,考虑到外部Flash容量够大,所以不考虑对Flash的影响.(牺牲空间换便利)
 * 4、此种做法有大量的重复的动作，后续可将部分菜单更新为统一的写法：当前9个测温点与5个测温区域已经融合到一起
 */
/********************************************************************************/
#if 0
/**
 * @brief:获得以及显示：图像模式参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayImageModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		DisPlayNewStr(Xline,Yline,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);
	}
	else if(state == 1)
	{
		if(UsermenuParameter.ImageAdjust == 0)	//手动调节：只能是线性模式
		{
			return;
		}
		else	//室内室外：四种模式切换
		{
			UsermenuParameter.ImageMode++;
			if(UsermenuParameter.ImageMode>3)
				UsermenuParameter.ImageMode = 1;
			//else
			{
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
				DisPlayNewStr(Xline,Yline,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);
			}
		}
	}
	else
	{
		if(UsermenuParameter.ImageAdjust == 0)	//手动调节：只能是线性模式
		{
			return;
		}
		else
		{
			if(UsermenuParameter.ImageMode > 1)
			{
				UsermenuParameter.ImageMode--;
			}
			else
			{
				UsermenuParameter.ImageMode = 3;
			}
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
			DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);
		}
	}
}
#endif
/**
 * @brief:获得以及显示：亮度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayBrightParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];

	ts_u08 MANUAL_Set_Bright = 0;//Manual模式下独立调节亮度
	ts_u08 auto_mode = 0; 

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		ts_u08 m_luma_percent_mode = UsermenuParameter.m_luma_percent_mode;
		if(m_luma_percent_mode>=LUMA_PERCENT_MODE_BUTT){
			m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
		}

		if(state){
			
			if(state==1){
				if(m_luma_percent_mode<(LUMA_PERCENT_MODE_BUTT-1)){
					
					#ifdef DSW_PRECENT_MODE_DEBUG
						m_luma_percent_mode++;
					#else
						m_luma_percent_mode+=25;
						if(m_luma_percent_mode>100){
							m_luma_percent_mode=100;
						}
					#endif
				}
				
			}
			else if(state==2){
				if(m_luma_percent_mode>0){
					#ifdef DSW_PRECENT_MODE_DEBUG
						m_luma_percent_mode--;
					#else
						if(m_luma_percent_mode>=25){
							m_luma_percent_mode-=25;
						}
						else{
							m_luma_percent_mode=0;
						}
						
					#endif
					
				}
			}
			
		}

		if(m_luma_percent_mode==LUMA_PERCENT_MODE_AUTO){
			sprintf((ts_char*)AddBuffer, "%s", "Auto");
		}
		else{
			// display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], m_luma_percent_mode);
			sprintf((ts_char*)AddBuffer, "%d%%",m_luma_percent_mode );
			
		}
		MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		Display_MenuItemLabel(Pt_Temp, is_active);

		if(m_luma_percent_mode!= UsermenuParameter.m_luma_percent_mode){
			UsermenuParameter.m_luma_percent_mode = m_luma_percent_mode;
		}
	}
	else{


		if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_INDOOR){
			auto_mode = 1;
			state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_RTD6){
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_K1){
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY_ENH){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL){
			MANUAL_Set_Bright = 1;
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}
		
		MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
		vision_log_debug("GetAndDisplayBrightParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.ImageAdjust=%d, UsermenuParameter.Bright=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.ImageAdjust, UsermenuParameter.Bright);
		if(state == 0)
		{
			if(UsermenuParameter.ImageAdjust)//0:手动,1：室内,2：室外
			{
				memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], BrightContrastTab[0][SelectionMenu.IsEnableEnligh], strlen((ts_char*)BrightContrastTab[0][SelectionMenu.IsEnableEnligh])+1);
			}
			else
			{
	
				if(MANUAL_Set_Bright == 1)
				{
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Bright);
				}
				else
				{
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Bright);
					
				}

			}
		}
		else if(state ==1)
		{
			if(MANUAL_Set_Bright==1)
			{
				UsermenuParameter.manual_ImagePara.Bright++;
				if(UsermenuParameter.manual_ImagePara.Bright > 127)
				{
					UsermenuParameter.manual_ImagePara.Bright = 127;
				}
				else
				{
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Bright);
				}
			}
			else
			{

				UsermenuParameter.Bright++;
				if(UsermenuParameter.Bright > 127)
				{
					UsermenuParameter.Bright = 127;
				}
				else
				{
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Bright);
				}

			}
		}
		else
		{
			if(MANUAL_Set_Bright==1)
			{
				if(UsermenuParameter.manual_ImagePara.Bright)
				{
					UsermenuParameter.manual_ImagePara.Bright--;
					
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Bright);

				}
				else
				{
					UsermenuParameter.manual_ImagePara.Bright  = 0;
				}
			}
			else
			{

				if(UsermenuParameter.Bright)
				{
					UsermenuParameter.Bright--;
				}
				else
				{
					UsermenuParameter.Bright  = 0;
					
				}

					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Bright );
			
			}
		}

		//针对indoor场景，特殊处理
		if(auto_mode==1 ){
			sprintf((ts_char*)AddBuffer, "%s", "Auto");
			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		}

	}

	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state)
	{
		vot_isp_luma_set();
	}
	
	vision_log_debug("GetAndDisplayBrightParameter UsermenuParameter.Bright %d \n", UsermenuParameter.Bright);
}

/**
 * @brief:获得以及显示：对比度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayContrastParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 MANUAL_Set_Contrast = 0;//Manual模式下独立调节对比度
	ts_u08 auto_mode = 0;

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		ts_u08 m_contrast_percent_mode = UsermenuParameter.m_contrast_percent_mode;
		if(m_contrast_percent_mode>=CONTRAST_PERCENT_MODE_BUTT){
			m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
		}
		#ifndef DSW_PRECENT_MODE_DEBUG
		if(m_contrast_percent_mode%25){
			m_contrast_percent_mode = ((m_contrast_percent_mode+12)/25)*25;
		}
		#endif

		if(state){
			
			if(state==1){
				if(m_contrast_percent_mode<(CONTRAST_PERCENT_MODE_BUTT-1)){
					
					#ifdef DSW_PRECENT_MODE_DEBUG
						m_contrast_percent_mode++;
					#else
						m_contrast_percent_mode+=25;
						if(m_contrast_percent_mode>100){
							m_contrast_percent_mode=100;
						}
					#endif
				}
			}
			else if(state==2){
				if(m_contrast_percent_mode>0){

					#ifdef DSW_PRECENT_MODE_DEBUG
						m_contrast_percent_mode--;
					#else
						if(m_contrast_percent_mode>=25){
							m_contrast_percent_mode-=25;
						}
						else{
							m_contrast_percent_mode=0;
						}
						
					#endif
				}
			}
			
		}

		if(m_contrast_percent_mode==CONTRAST_PERCENT_MODE_AUTO){
			sprintf((ts_char*)AddBuffer, "%s", "Auto");
		}
		else{
			// display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], m_contrast_percent_mode);
			sprintf((ts_char*)AddBuffer, "%d%%", m_contrast_percent_mode);
			
		}
		MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		Display_MenuItemLabel(Pt_Temp, is_active);

		if(m_contrast_percent_mode!= UsermenuParameter.m_contrast_percent_mode){
			UsermenuParameter.m_contrast_percent_mode = m_contrast_percent_mode;
		}
	}
	else{

		if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_INDOOR){
			state = 0;
			auto_mode = 1;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_RTD6){
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_K1){
			// state = 0;
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY_ENH){
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}
		else if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL){
			MANUAL_Set_Contrast = 1;
			vision_log_debug("m_dev_env_mode  %d \n " ,UsermenuParameter.m_dev_env_mode );
		}


		MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
		vision_log_debug("GetAndDisplayContrastParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.ImageAdjust=%d, UsermenuParameter.Contrast=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.ImageAdjust, UsermenuParameter.Contrast );
		if(state == 0)
		{
			if(UsermenuParameter.ImageAdjust)//0:手动,1：室内,2：室外
			{
				memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], BrightContrastTab[0][SelectionMenu.IsEnableEnligh], strlen((ts_char*)BrightContrastTab[0][SelectionMenu.IsEnableEnligh])+1);
			
			}
			else
			{

				if(MANUAL_Set_Contrast == 1)
				{
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Contrast);
				}
				else
				{
						display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Contrast);
				}

			}
		}
		else if(state ==1)
		{
			if(MANUAL_Set_Contrast==1)
			{	
				if(UsermenuParameter.manual_ImagePara.Contrast < 32)
				{
					UsermenuParameter.manual_ImagePara.Contrast++;
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Contrast);
				}
				else
				{
					UsermenuParameter.manual_ImagePara.Contrast = 32;
				}
			}
			else
			{
				if(UsermenuParameter.Contrast  < 32)
				{
					UsermenuParameter.Contrast ++;
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Contrast );
	
				}
				else
				{
					UsermenuParameter.Contrast  = 32;
				}
			}
		}
		else
		{
			if(MANUAL_Set_Contrast==1)
			{
				if(UsermenuParameter.manual_ImagePara.Contrast > 0)
				{
					UsermenuParameter.manual_ImagePara.Contrast--;
					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.manual_ImagePara.Contrast);
	
				}
				else
				{
					UsermenuParameter.manual_ImagePara.Contrast  = 0;
				}
			}
			else
			{
				if(UsermenuParameter.Contrast  > 0)
				{
					UsermenuParameter.Contrast --;

				}
				else
				{
					UsermenuParameter.Contrast  = 0;
				}

					display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.Contrast );
			
			}
		}

		//针对indoor场景，特殊处理
		if(auto_mode==1 ){
			sprintf((ts_char*)AddBuffer, "%s", "Auto");
			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		}
	}

	Display_MenuItemLabel(Pt_Temp, is_active);
	
	if(state)
	{

		vot_isp_coef_set();
		
	}
	vision_log_debug("GetAndDisplayContrastParameter UsermenuParameter.Contrast %d \n", UsermenuParameter.Contrast);
}
#if 0
/**
 * @brief:获得以及显示：场景参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaySenceParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		DisPlayNewStr(Xline,Yline,ImageAdjustNameTab[UsermenuParameter.ImageAdjust][SelectionMenu.IsEnableEnligh],3,CharacterColor);
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
		DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,ImageAdjustNameTab[UsermenuParameter.ImageAdjust][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		if(UsermenuParameter.ImageAdjust)
		{
			UsermenuParameter.ImageMode = 1;
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
			DisplayBackground(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,10,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);

			DisplayBackground(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,BrightContrastTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);//亮度切换成AUTO
			DisplayBackground(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,BrightContrastTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);//对比度切换成AUTO
		}
		else//手动：只能是模式0
		{
			UsermenuParameter.ImageMode = 0;
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));

			DisplayBackground(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,10,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);

			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(UsermenuParameter.Bright)));
			DisplayBackground(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,UsermenuParameter.Bright,3,CharacterColor);

			APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(UsermenuParameter.Contrast <<16)));
			DisplayBackground(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,UsermenuParameter.Contrast ,3,CharacterColor);
		}
		ImageAdjustSet(UsermenuParameter.ImageAdjust);  //图像设置
	}
	else
	{
		if(UsermenuParameter.ImageAdjust > 0)   //只显示自动（室外）、手动模式
		{
			UsermenuParameter.ImageAdjust--;
			if(UsermenuParameter.ImageAdjust > 0) UsermenuParameter.ImageAdjust = 0;
			DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,ImageAdjustNameTab[UsermenuParameter.ImageAdjust][SelectionMenu.IsEnableEnligh],3,CharacterColor);

			if(UsermenuParameter.ImageAdjust == 0)	//手动调节：只能是线性模式
			{
				UsermenuParameter.ImageMode = 0;
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
						Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,10,BACKGROUND_LAYER,BackgroundColor);
				DisPlayNewStr(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
						Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);

				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(UsermenuParameter.Bright)));
				DisplayBackground(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,UsermenuParameter.Bright,3,CharacterColor);

				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(UsermenuParameter.Contrast <<16)));
				DisplayBackground(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
				display_u8number(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,UsermenuParameter.Contrast ,3,CharacterColor);
			}
			else
			{
				UsermenuParameter.ImageMode = 1;
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				DisplayBackground(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
						Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,10,BACKGROUND_LAYER,BackgroundColor);
				DisPlayNewStr(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
						Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);
			}
		}
		else
		{
			UsermenuParameter.ImageAdjust = 2;
			DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,ImageAdjustNameTab[UsermenuParameter.ImageAdjust][SelectionMenu.IsEnableEnligh],3,CharacterColor);

			DisplayBackground(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[2].Info.line_X+8*GetStringSize(UsermenuTab2[2].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[2].Info.line_Y,BrightContrastTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);//亮度切换成AUTO
			DisplayBackground(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,4,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[3].Info.line_X+8*GetStringSize(UsermenuTab2[3].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,Y_START+Y_GAP*UsermenuTab2[3].Info.line_Y,BrightContrastTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);//对比度切换成AUTO

			UsermenuParameter.ImageMode = 1;
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
			DisplayBackground(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
					Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,10,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(UsermenuTab2[1].Info.line_X+8*GetStringSize(UsermenuTab2[1].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
					Y_START+Y_GAP*UsermenuTab2[1].Info.line_Y,ModeNameTab[UsermenuParameter.ImageMode][SelectionMenu.IsEnableEnligh],FONT_LAYER,CharacterColor);
		}
		ImageAdjustSet(UsermenuParameter.ImageAdjust);  //图像设置
	}
}
#endif

/**
 * @brief:获得以及显示：伪彩参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayPaletteParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{

	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayPaletteParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.Palette=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.Palette);

	if(state == 0)
	{
		// load_palette(UsermenuParameter.Palette);
		//DisPlayNewStr(Xline,Yline,PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh],4,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh], strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(state == 1)
	{
		
		// if(UsermenuParameter.Palette==5) UsermenuParameter.Palette = 6;
		if(UsermenuParameter.Palette <(VOT_COLOR_PALETTE_MODE_BUTT-1))
		{
			UsermenuParameter.Palette++;
		}
		else
		{
			UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_BUTT-1;
		}

		load_palette(UsermenuParameter.Palette);
		//APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
		//DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
		//DisPlayNewStr(Xline,Yline,PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh],4,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh], strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
		
	}
	else
	{

		
		if(UsermenuParameter.Palette>0){
			UsermenuParameter.Palette--;
		}
		else{
			UsermenuParameter.Palette=0;
		}
		
		// if(UsermenuParameter.Palette==5) UsermenuParameter.Palette = 4;
		
		load_palette(UsermenuParameter.Palette);
		//APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
		//DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
		//DisPlayNewStr(Xline,Yline,PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh],4,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh], strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
		

	}
}
#if 0
/**
 * @brief:获得以及显示：伪彩参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */

ts_u08 ZoomMegOffset(void)
{
	ts_u08 offset = 0;
	     if(UsermenuParameter.Enlarge <=20 ) offset =ZOOM_OFFSET +5;

	else if(UsermenuParameter.Enlarge <=25 ) offset =ZOOM_OFFSET +2;
	else if(UsermenuParameter.Enlarge <=30 ) offset =ZOOM_OFFSET +1;
	else if(UsermenuParameter.Enlarge <=35 ) offset =ZOOM_OFFSET +0;
	else if(UsermenuParameter.Enlarge <=40 ) offset =ZOOM_OFFSET -2;
	else if(UsermenuParameter.Enlarge <=45 ) offset =ZOOM_OFFSET -3;
	else if(UsermenuParameter.Enlarge <=50 ) offset =ZOOM_OFFSET -4;
	else if(UsermenuParameter.Enlarge <=55 ) offset =ZOOM_OFFSET -5;
	else if(UsermenuParameter.Enlarge <=60 ) offset =ZOOM_OFFSET -6;
	else if(UsermenuParameter.Enlarge <=65 ) offset =ZOOM_OFFSET -8;
	else if(UsermenuParameter.Enlarge <=70 ) offset =ZOOM_OFFSET -10;
	return offset;
}

void ShowZoom(ts_u32 x,ts_u32 y) //连续变倍适用
{
	if(!UsermenuParameter.ContinuousZoom) return;
	DisplayBackground(x-8*4-ZoomMegOffset(),y/2,4,BACKGROUND_LAYER,BackgroundColor);
	DisPlayNewStr     (x-8*4-ZoomMegOffset(),  y/2,"X",3,CharacterColor);
	Display_uint8_Dec2(x-8*4-ZoomMegOffset()+3,    y/2,UsermenuParameter.Enlarge/10,3,CharacterColor);
	DisPlayNewStr     (x-8*2-ZoomMegOffset()+3,    y/2,".",3,CharacterColor);
	Display_uint8_Dec1(x-8*1-ZoomMegOffset(),  y/2,UsermenuParameter.Enlarge%10,3,CharacterColor);
}
void ShowZoom2(ts_u32 x,ts_u32 y)//整数变倍适用
{
	ts_u08 data,pos;
	pos = 15;
	if(!UsermenuParameter.ContinuousZoom) return;
	DisplayBackground (x-8*2-pos,y,2,BACKGROUND_LAYER,BackgroundColor);
	DisPlayNewStr     (x-8*2-pos,y,"X",3,CharacterColor);
	switch(UsermenuParameter.Enlarge)
	{
		case 0:
			data = 1;
			Display_uint8_Dec2(x-8*2-pos,y,data,3,CharacterColor);
			break;
		case 1:
			data = 2;
			Display_uint8_Dec2(x-8*2-pos,y,data,3,CharacterColor);
			break;
		case 2:
			data = 4;
			Display_uint8_Dec2(x-8*2-pos,y,data,3,CharacterColor);
			break;
		case 3:
			data = 8;
			Display_uint8_Dec2(x-8*2-pos,y,data,3,CharacterColor);
			break;
	}
}
void EraseZoomMeg(void)
{
	DisplayBackground(IR_X_pixel-8*70-ZoomMegOffset(),IR_Y_pixel/2,70,BACKGROUND_LAYER,0);
}
void Clear_ZoomMenuInfo(void)
{
	RecallMenu.SelectItem = SelectionMenu.SelectItem;
	//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
	while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
	{
		SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
		Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
				SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
	}
	Clear_MenuInfo(SelectionMenu.SelectItem,UTab1_Length);//清除一级菜单，退出一级菜单
	ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息

	SelectionMenu.MenuState = 0x1f;//当前菜单状态为0x1f 变倍状态
}
void Show_ZoomMenuInfo(void)
{
	SelectionMenu.MenuState = 0x25;//当前菜单状态为0x25,进入菜单一
	SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
	Display_MenuItem(SelectionMenu.SelectItem->Parent,UTab1_Length, 1);//显示一级菜单
	SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
	Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
	Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
}

ts_u16 GetPos(ts_u08 type)
{
	ts_u16 pos=0;
	switch(type)
	{
		case 0:
			pos =IR_X_pixel-0;
			break;
		case 1:
			pos =IR_X_pixel-160;
			break;
		case 2:
			pos =IR_X_pixel-240;
			break;
		case 3:
			pos =IR_X_pixel-280;
			break;
	}
	return pos;
}
#endif
#ifndef SUPPORT_CONTINUOUS_ZOOM
//#if 0
void GetAndDisplayEnlargeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 flag = 0;
	ts_u16 pos =0;
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayEnlargeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.Enlarge=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.Enlarge);

	if(state == 1)
	{
		UsermenuParameter.Enlarge++;
		if(UsermenuParameter.Enlarge >10)
		{
			UsermenuParameter.Enlarge = 0;
			
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.Enlarge >0)
		{
			UsermenuParameter.Enlarge--;
		
		}
		else
			UsermenuParameter.Enlarge = 10;
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], EnlargeTab[UsermenuParameter.Enlarge], strlen((ts_char*)EnlargeTab[UsermenuParameter.Enlarge])+1);

	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		ISP_zoom_set(1<<UsermenuParameter.Enlarge);
	}
}
#else

void GetAndDisplayEnlargeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	// ts_u08 flag = 0;
	ts_u08 MenuNameLen;
	ts_u08 Enlarge_MSB,Enlarge_LSB;
	unsigned char Enlarge_str[5] = {0};
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayEnlargeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.Enlarge=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.Enlarge);
	ts_u08 Enlarge=UsermenuParameter.Enlarge;
	ts_u08 EnlargeValue = 10;

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		//放大倍数 1-8
		if(Enlarge>80 || Enlarge<10){
			Enlarge = 10;
		}

		if(state)
		{
			if(UsermenuParameter.m_pip_mode != PIP_MODE_NONE){
				state = 0;
				ts_char tip_info[64];
				snprintf(tip_info,64,"%s","pip mode  must be none!");
				UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU1);
			}

			if(state == 1)
			{
				
				if(Enlarge < 80 )
				{
					Enlarge+=10;
				}
				else{
					Enlarge = 80;
				}
			}
			else if(state == 2)
			{
				if(Enlarge > 10 )
				{
					Enlarge-=10;
				}
				else{
					Enlarge = 10;
				}
			}

			
			EnlargeValue = Enlarge;
		}
		snprintf((ts_char*)Enlarge_str, 5, "X%d", Enlarge/10);

	}
	else {

		//放大倍数 10-100 => 10为1.0倍数
		if(Enlarge<10 || Enlarge>100){
			Enlarge = 10;
		}
		if(state == 1)
		{
			Enlarge++;
			if(Enlarge >100 || Enlarge<10)
			{
				Enlarge = 10;
			}
		}
		else if(state == 2)
		{
			if(Enlarge > 10)
			{
				Enlarge--;
							}
			else
				Enlarge = 100;
		}

		Enlarge_str[0] = 'X';
		if(Enlarge < 100)		
		{	
			Enlarge_MSB = Enlarge/10;
			Enlarge_str[1] = (Enlarge_MSB%10) + 0x30;
			Enlarge_str[2] = '.';
			Enlarge_LSB = Enlarge%10;
			Enlarge_str[3] = (Enlarge_LSB%10) + 0x30;
			Enlarge_str[4] = '\0';
		}else {
			Enlarge_str[1] = '1';
			Enlarge_str[2] = '0';
			Enlarge_str[3] = 0x20;
			Enlarge_str[4] = '\0';
		}

		EnlargeValue = Enlarge;
   	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], Enlarge_str, strlen((ts_char*)Enlarge_str)+1);

	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		vision_log_warn("EnlargeValue %d UsermenuParameter.Enlarge %d \n",EnlargeValue, UsermenuParameter.Enlarge);
		if(Enlarge!=UsermenuParameter.Enlarge){
			UsermenuParameter.Enlarge = Enlarge; 
			ISP_vot_zoom_set(Enlarge);
		}
		
	}
}
#endif



void GetAndDisplaySunPtModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	

	vision_log_debug("GetAndDisplayHSTParameter state=%u UsermenuParameter.m_SunPT_mode %d \n", state,UsermenuParameter.m_SunPT_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_SunPT_mode = UsermenuParameter.m_SunPT_mode;
	if(m_SunPT_mode>=SUN_PT_MODE_BUTT){
		m_SunPT_mode = SUN_PT_MODE_AUTO;
	}

	vision_log_debug("GetAndDisplayHSTParameter state=%u m_hst_mode %d  %d\n", state,m_SunPT_mode,UsermenuParameter.m_SunPT_mode);

	if(state)
	{
		if(m_SunPT_mode==SUN_PT_MODE_AUTO)
		{
			m_SunPT_mode = SUN_PT_MODE_MANUAL;
		}
		else if(m_SunPT_mode==SUN_PT_MODE_MANUAL)
		{
			m_SunPT_mode = SUN_PT_MODE_AUTO;
		}
	}

	if(m_SunPT_mode == SUN_PT_MODE_AUTO){
		sprintf((ts_char*)AddBuffer, "%s", "Auto");
		
	}
	else if(m_SunPT_mode == SUN_PT_MODE_MANUAL){
		sprintf((ts_char*)AddBuffer, "%s", "Manual");
	}

	if(state){
		if(m_SunPT_mode!=UsermenuParameter.m_SunPT_mode){
			ISP_set_sun_pt_mode(m_SunPT_mode);
		}
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	vision_log_debug(" state=%u m_SunPT_mode %d  %d\n", state,m_SunPT_mode,UsermenuParameter.m_SunPT_mode);

}


void GetAndDisplaySunPtGapParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;

	vision_log_debug("GetAndDisplayHSTParameter state=%u UsermenuParameter.m_SunPT_gap %d \n", state,UsermenuParameter.m_SunPT_gap);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u16  m_SunPT_gap = UsermenuParameter.m_SunPT_gap;
	if(m_SunPT_gap>=MAX_SUN_PT_TIME_SECS){
		m_SunPT_gap = DEFAULT_SUN_PT_TIME_SECS;
	}

	vision_log_debug("GetAndDisplayHSTParameter state=%u m_hst_mode %d  %d\n", state,m_SunPT_gap,UsermenuParameter.m_SunPT_gap);

	if(state == 1)
	{
		if(m_SunPT_gap < MAX_SUN_PT_TIME_SECS)
		{
			m_SunPT_gap++;
		}

	}
	else if(state == 2)
	{
		if(m_SunPT_gap > 1)
		{
			m_SunPT_gap--;
		}
		else
		{
			m_SunPT_gap = 1;
		}
	}
	display_append_u16number((ts_u16*)&Pt_Temp->Info.NameAdd[MenuNameLen], m_SunPT_gap);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		if(m_SunPT_gap!=UsermenuParameter.m_SunPT_gap){
			ISP_set_sun_pt_gap(m_SunPT_gap);
		}
	}


}

void GetAndDisplayHSTParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	

	vision_log_debug("GetAndDisplayHSTParameter state=%u UsermenuParameter.m_hst_mode %d \n", state,UsermenuParameter.m_hst_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_hst_mode = UsermenuParameter.m_hst_mode;
	if(m_hst_mode>=HOT_SPOT_TRACKING_MODE_BUTT){
		m_hst_mode = HOT_SPOT_TRACKING_MODE_NONE;
	}

	vision_log_debug("GetAndDisplayHSTParameter state=%u m_hst_mode %d  %d\n", state,m_hst_mode,UsermenuParameter.m_hst_mode);

	if(state){	
	
		if(state==1){
	
			if(m_hst_mode<(HOT_SPOT_TRACKING_MODE_BUTT-1)) 
			{
				m_hst_mode++;
			}
		}
		else if(state==2){
			if(m_hst_mode>0){
				m_hst_mode--;
			}
		}
	}
	

	if(m_hst_mode==HOT_SPOT_TRACKING_MODE_NONE){
		sprintf((ts_char*)AddBuffer, "%s", "NONE");
	}
	else if(m_hst_mode==HOT_SPOT_TRACKING_MODE_MAX){
		sprintf((ts_char*)AddBuffer, "%s", "Max");
	}
	else if(m_hst_mode==HOT_SPOT_TRACKING_MODE_MIN){
		sprintf((ts_char*)AddBuffer, "%s", "Min");
	}
	else if(m_hst_mode==HOT_SPOT_TRACKING_MODE_MAX_MIN){
		sprintf((ts_char*)AddBuffer, "%s", "MaxMin");
	}

	
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(UsermenuParameter.m_hst_mode != m_hst_mode){
			vot_hst_mode_set(m_hst_mode);
			// ISP_write_sys_cfg();
		}

	}
	vision_log_debug(" state=%u m_hst_mode %d  %d\n", state,m_hst_mode,UsermenuParameter.m_hst_mode);

}
/** @note 中心光标显示/隐藏 */
void GetAndDisplay_Crosshair_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	ts_u08 MenuNameLen;
	ts_u08 addBuffer[32];

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayEnlargeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d\n", Pt_Temp, state, MenuNameLen);
	
	if (state == 0) {
		if (UsermenuParameter.m_Crosshair_OnlyDisplay)
		{
			UsermenuParameter.m_Crosshair_OnlyDisplay = ON;
			sprintf((ts_char*)addBuffer, "%s", "ON");
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "OFF");
		}
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], addBuffer, strlen((ts_char*)addBuffer)+1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1) {
		if (UsermenuParameter.m_Crosshair_OnlyDisplay == OFF) {
			UsermenuParameter.m_Crosshair_OnlyDisplay = ON;
			sprintf((ts_char*)addBuffer, "%s", "ON");
			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], addBuffer, strlen((ts_char*)addBuffer)+1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			getDisplay_Crosshair_Switch();
		}
	}
	else {
		if (UsermenuParameter.m_Crosshair_OnlyDisplay) {
			UsermenuParameter.m_Crosshair_OnlyDisplay = OFF;
			sprintf((ts_char*)addBuffer, "%s", "OFF");
			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], addBuffer, strlen((ts_char*)addBuffer)+1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			getDisplay_Crosshair_Switch();
		}	
	}
}
void GetAndDisplayPIPParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetAndDisplayPIPParameter state=%u UsermenuParameter.m_pip_mode %d \n", state,UsermenuParameter.m_pip_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  m_pip_mode = UsermenuParameter.m_pip_mode;
	if(m_pip_mode>=PIP_MODE_BUTT){
		m_pip_mode = PIP_MODE_NONE;
	}

	vision_log_debug("GetAndDisplayPIPParameter state=%u m_pip_mode %d  %d\n", state,m_pip_mode,UsermenuParameter.m_pip_mode);



	if(state){	

		if(UsermenuParameter.Enlarge != 10){
			state = 0;
			ts_char tip_info[64];
			snprintf(tip_info,64,"%s","zoom  must be X1 !");
			UpdatePromptInfoWithTimeMs(tip_info, 1000,VOT_LOG_TYPE_MENU1);
		}
		
		if(state==1){
	
			if(m_pip_mode<(PIP_MODE_ANY_WHERE-1)) 
			{
				m_pip_mode++;
			}
		}
		else if(state==2){
			if(m_pip_mode>0){
				m_pip_mode--;
			}
		}
	}

	if(m_pip_mode==PIP_MODE_NONE){
		sprintf((ts_char*)AddBuffer, "%s", "NONE");
	}
	else if(m_pip_mode==PIP_MODE_P1){
		sprintf((ts_char*)AddBuffer, "%s", "P1");
	}
	else if(m_pip_mode==PIP_MODE_P2){
		sprintf((ts_char*)AddBuffer, "%s", "P2");
	}
	else if(m_pip_mode==PIP_MODE_P3){
		sprintf((ts_char*)AddBuffer, "%s", "P3");
	}
	else if(m_pip_mode==PIP_MODE_P4){
		sprintf((ts_char*)AddBuffer, "%s", "P4");
	}
	else if(m_pip_mode==PIP_MODE_P5){
		sprintf((ts_char*)AddBuffer, "%s", "P5");
	}
	else if(m_pip_mode==PIP_MODE_P6){
		sprintf((ts_char*)AddBuffer, "%s", "P6");
	}
	else if(m_pip_mode==PIP_MODE_P7){
		sprintf((ts_char*)AddBuffer, "%s", "P7");
	}
	else if(m_pip_mode==PIP_MODE_P8){
		sprintf((ts_char*)AddBuffer, "%s", "P8");
	}
	else if(m_pip_mode==PIP_MODE_P9){
		sprintf((ts_char*)AddBuffer, "%s", "P9");
	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(UsermenuParameter.m_pip_mode != m_pip_mode){
			UsermenuParameter.m_pip_mode = m_pip_mode;
			// ISP_write_sys_cfg();

			vot_pip_mode_set(m_pip_mode, RZ_RATIO_MODE_2X, 1);
		}

	}
	vision_log_debug(" state=%u m_pip_mode %d  %d\n", state,m_pip_mode,UsermenuParameter.m_pip_mode);

}


/**
 * @brief:获得以及显示：语言参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayLanguageParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	//ts_u32 Xline = 0;
	//ts_u32 Yline = 0;
	//Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	//Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayEnlargeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.Enlarge=%d\n", Pt_Temp, state, MenuNameLen, SelectionMenu.IsEnableEnligh);
	if(state == 0)
	{
		//DisPlayNewStr(Xline,Yline,LanguageNameTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], LanguageNameTab[0][SelectionMenu.IsEnableEnligh], strlen((ts_char*)LanguageNameTab[0][SelectionMenu.IsEnableEnligh])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
		return;
	}
	else if(state == 1)
	{
		if(SelectionMenu.IsEnableEnligh == 0)
		{
			SelectionMenu.IsEnableEnligh = 1;
			CreatUsermenu();
			//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x01));

			//Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
			Pt_Temp = Pt_Temp->Parent;
			//Clear_MenuInfo(Pt_Temp,UTab1_Length);//清除一级菜单
			Display_MenuItem(Pt_Temp,UTab1_Length, 0);//显示一级菜单
			Pt_Temp = Pt_Temp->Son;
			Display_MenuItem(Pt_Temp,UTab4_Length, 0);//显示菜单一系统子链
			//DisplaySelectionMenuFrame(Pt_Temp->Info.line_X,Y_START+Y_GAP*Pt_Temp->Info.line_Y,140,BACKGROUND_LAYER,6);
		}
	}
	else
	{
		if(SelectionMenu.IsEnableEnligh)
		{
			SelectionMenu.IsEnableEnligh = 0;
			CreatUsermenu();
			//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));

			//Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
			Pt_Temp = Pt_Temp->Parent;
			//Clear_MenuInfo(Pt_Temp,UTab1_Length);//清除一级菜单
			Display_MenuItem(Pt_Temp,UTab1_Length, 0);//显示一级菜单
			Pt_Temp = Pt_Temp->Son;
			Display_MenuItem(Pt_Temp,UTab4_Length, 0);//显示菜单一系统子链

			//DisplaySelectionMenuFrame(Pt_Temp->Info.line_X,Y_START+Y_GAP*Pt_Temp->Info.line_Y,140,BACKGROUND_LAYER,6);
		}
	}
	UsermenuParameter.IsEnableEnligh = SelectionMenu.IsEnableEnligh;
}

/** @note 进入十字光标/半自动校点时，对默认菜单进行重置操作 */
void  ResetDefaultMenuDisplay(void)
{
	// GetAndDisplayIspStatisInfo(0);
	ISP_set_bottom_statistic_force_flag(1);
	Key_ExitUsermenu();
	Key_ExitUsermenu();
	Key_ExitUsermenu();
	// SelectionMenu.IsEnableEnligh = 0;
	// UsermenuParameter.IsEnableEnligh = SelectionMenu.IsEnableEnligh;
	// if(UsermenuParameter.UseUserMenuDefParam == 1)//如果标记使用用户菜单默认参数，则加载用户菜单默认参数
	// {
	// 	SelectionMenu.IsEnableEnligh =UsermenuParameter.IsEnableEnligh;
	// }
	CreatUsermenu();
}

void SelectFontColor(void)
{
	// display_set_fontcolor(UsermenuParameter.FontColor);
	switch(UsermenuParameter.FontColor)
	{
		case 0:
			BackgroundColor = 0;
			CharacterColor = 5;
			break;
		case 1:
		case 2:
			BackgroundColor = 0;
			CharacterColor = 6;
			break;

		case 3:
			BackgroundColor = 3;
			CharacterColor = 6;
			break;
	}
}

/**
 * @brief:获得以及显示：字符颜色参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayFontColorParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
	// ts_u32 Xline = 0;
	// ts_u32 Yline = 0;
	// Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	// Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayFontColorParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.FontColor=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.FontColor);

	if(state == 0)
	{
		//DisPlayNewStr(Xline,Yline,FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh], strlen((ts_char*)FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(state == 1)
	{
		UsermenuParameter.FontColor++;
		if(UsermenuParameter.FontColor>3)
			UsermenuParameter.FontColor = 3;
		else
		{
			//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffffe7)|(UsermenuParameter.FontColor<<3)));
			SelectFontColor();
			
            menu_change_palette_vot(0, 1);
			Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
			Pt_Temp = Pt_Temp->Parent;
			Clear_MenuInfo(Pt_Temp,UTab1_Length);//清除一级菜单
			Display_MenuItem(Pt_Temp,UTab1_Length, 0);//显示一级菜单
			Pt_Temp = Pt_Temp->Son;
			Display_MenuItem(Pt_Temp,UTab4_Length, 1);//显示菜单一系统子链
			//DisplaySelectionMenuFrame(Pt_Temp->Next->Info.line_X,Y_START+Y_GAP*Pt_Temp->Next->Info.line_Y,140,BACKGROUND_LAYER,6);
		}
	}
	else
	{
		if(UsermenuParameter.FontColor>0)
		{
			UsermenuParameter.FontColor--;
			//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffffe7)|(UsermenuParameter.FontColor<<3)));
			SelectFontColor();
			
            menu_change_palette_vot(1, 1);

			Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
			Pt_Temp = Pt_Temp->Parent;
			Clear_MenuInfo(Pt_Temp,UTab1_Length);//清除一级菜单
			Display_MenuItem(Pt_Temp,UTab1_Length, 0);//显示一级菜单
			Pt_Temp = Pt_Temp->Son;
			Display_MenuItem(Pt_Temp,UTab4_Length, 1);//显示菜单一系统子链
			//DisplaySelectionMenuFrame(Pt_Temp->Next->Info.line_X,Y_START+Y_GAP*Pt_Temp->Next->Info.line_Y,140,BACKGROUND_LAYER,6);
		}
		else
			UsermenuParameter.FontColor = 0;
	}
}

/**
 * @brief:获得以及显示：镜像参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayMirrorParameter(Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active)
{
//	ts_u32 Xline = 0;
//	ts_u32 Yline = 0;
//	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
//	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayMirrorParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.Mirror=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.Mirror);

	if(state == 0)
	{
		//DisPlayNewStr(Xline,Yline,MirrorParameter[UsermenuParameter.Mirror],3,CharacterColor);
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], MirrorParameter[UsermenuParameter.Mirror], strlen((ts_char*)MirrorParameter[UsermenuParameter.Mirror])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if(state == 1)
	{
		UsermenuParameter.Mirror++;
		if(UsermenuParameter.Mirror>3)
			UsermenuParameter.Mirror = 3;
		else
		{
#if 0
			SetFreeze_img(ON);
			delay_1ms(25);
			MirrorBadSppot(1);
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffcff)|(UsermenuParameter.Mirror<<8)));
			//SetFreeze_img(OFF);

			APB3_REGW_Shutter_SW(ON);
			if(systemPar.NoShutter)  return;
			SetFreeze_img(ON);		//图像冻结
			delay_1ms(50);
			shutterOpr(OFF);
			if((SystemParaMenu2.Usermenu_SW)&&((SystemParaMenu2.TemTest_SW==1)||(SystemParaMenu2.TemTest_SW==3)))
			{
				Auto_INT();
				if((SelectionMenu.MenuState == 0x26)&&((SelectionMenu.SelectItem->Info.ID >= 0x61)&&(SelectionMenu.SelectItem->Info.ID <= 0x67)))//刷新显示
				{
					DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
					display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);
				}
			}

			delay_1ms(100);	   //delay_1ms(100);
			SetBaking_SW(ON);		//采集本底

			delay_1ms(500);   //预计200ms  多预留点时间
			SetBaking_SW(OFF);		//采集本底结束
			delay_1ms(50);   //150
			#ifdef SUPPORT_VOT_DETECTOR

				SystemParaMenu2.DTMY_Init = 1;
				APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));
				delay_1s(1);
				SystemParaMenu2.DTMY_Init = 0;
				APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));

			#endif
			shutterOpr(ON);
			delay_1ms(100);  //	delay_1ms(400);
			SetFreeze_img(OFF); //图像激活

			APB3_REGW_Shutter_SW(OFF);

			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,MirrorParameter[UsermenuParameter.Mirror],3,CharacterColor);
#else
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], MirrorParameter[UsermenuParameter.Mirror], strlen((ts_char*)MirrorParameter[UsermenuParameter.Mirror])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
#endif
		}
	}
	else
	{
		if(UsermenuParameter.Mirror>0)
		{
			UsermenuParameter.Mirror--;
#if 0
			SetFreeze_img(ON);
			delay_1ms(25);
			MirrorBadSppot(0);
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffcff)|(UsermenuParameter.Mirror<<8)));
			//SetFreeze_img(OFF);
			APB3_REGW_Shutter_SW(ON);
			if(systemPar.NoShutter)
				return;
			SetFreeze_img(ON);		//图像冻结
			delay_1ms(50);
			shutterOpr(OFF);
			if((SystemParaMenu2.Usermenu_SW)&&((SystemParaMenu2.TemTest_SW==1)||(SystemParaMenu2.TemTest_SW==3)))
			{
				Auto_INT();
				if((SelectionMenu.MenuState == 0x26)&&((SelectionMenu.SelectItem->Info.ID >= 0x61)&&(SelectionMenu.SelectItem->Info.ID <= 0x67)))//刷新显示
				{
					DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
					display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);
				}
			}

			delay_1ms(100);	 //delay_1ms(100);
			SetBaking_SW(ON);//采集本底

			delay_1ms(500); //预计200ms  多预留点时间
			SetBaking_SW(OFF);//采集本底结束
			delay_1ms(50);   //150
			#ifdef SUPPORT_VOT_DETECTOR
				SystemParaMenu2.DTMY_Init = 1;
				APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));
				delay_1s(1);
				SystemParaMenu2.DTMY_Init = 0;
				APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffff7ff)|(SystemParaMenu2.DTMY_Init<<11)));
			#endif
			shutterOpr(ON);
			delay_1ms(100);  //	delay_1ms(400);
			SetFreeze_img(OFF); //图像激活

			APB3_REGW_Shutter_SW(OFF);
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,MirrorParameter[UsermenuParameter.Mirror],3,CharacterColor);
#else
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], MirrorParameter[UsermenuParameter.Mirror], strlen((ts_char*)MirrorParameter[UsermenuParameter.Mirror])+1);
		//Pt_Temp->Info.NameAdd[MenuNameLen+strlen((ts_char*)FontColorNameTab[UsermenuParameter.FontColor][SelectionMenu.IsEnableEnligh])+1] = '\0';
		Display_MenuItemLabel(Pt_Temp, is_active);
#endif
		}
		else
			UsermenuParameter.Mirror = 0;
	}
	/* RAW+YUV 模式下不允许进行翻转 */
	if (SystemParaMenu2.Usermenu_SW && (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW)) {
		UsermenuParameter.Mirror = 0;
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], MirrorParameter[UsermenuParameter.Mirror], strlen((ts_char*)MirrorParameter[UsermenuParameter.Mirror])+1);
		Display_MenuItemLabel(Pt_Temp, is_active);
		UpdatePromptInfoWithTimeMs("mirror don't support in temp mode's raw_yuv !", 2000,VOT_LOG_TYPE_MENU1);
		
	}
	if(state){
		ISP_mirr_set(UsermenuParameter.Mirror);
	}
	

}

/**
 * @brief:获得以及显示：挡片模式参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayShutterModeParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayShutterModeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.ShutterMode=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.ShutterMode);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.ShutterMode<2)
		{
			UsermenuParameter.ShutterMode++;
		}
		else
		{
			UsermenuParameter.ShutterMode = 2;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.ShutterMode>0)
		{
			UsermenuParameter.ShutterMode--;
		}
		else
		{
			UsermenuParameter.ShutterMode = 0;
		}
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], ShutterModeNameTab[UsermenuParameter.ShutterMode][SelectionMenu.IsEnableEnligh], strlen((ts_char*)ShutterModeNameTab[UsermenuParameter.ShutterMode][SelectionMenu.IsEnableEnligh])+1);
	vision_log_debug("ShutterModeNameTab[UsermenuParameter.ShutterMode][SelectionMenu.IsEnableEnligh] = 0x%02X %02X %02X\n", Pt_Temp->Info.NameAdd[MenuNameLen], Pt_Temp->Info.NameAdd[MenuNameLen+1], Pt_Temp->Info.NameAdd[MenuNameLen+2]);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		ISP_shuttermode(UsermenuParameter.ShutterMode);
	}

}

/**
 * @brief:获得以及显示：自动挡片参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayAutoShutterParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayShutterModeParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.AutoShutter=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.AutoShutter);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.AutoShutter<40)
		{
			UsermenuParameter.AutoShutter++;
		}
		else
		{
			UsermenuParameter.AutoShutter = 40;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.AutoShutter>0)
		{
			UsermenuParameter.AutoShutter--;
		}
		else
		{
			UsermenuParameter.AutoShutter = 0;
		}
	}

	// if(UsermenuParameter.ShutterMode != SHUTTER_MODE_OFF)
	{
		if(UsermenuParameter.AutoShutter == 0)
		{
			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "OFF", 4);
		
			UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
		}
		else
		{
			UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
			if(UsermenuParameter.AutoShutter&0x1)
				sprintf((ts_char*)AddBuffer, "%d.5M", UsermenuParameter.AutoShutter/2);
			else
				sprintf((ts_char*)AddBuffer, "%d.0M", UsermenuParameter.AutoShutter/2);

			memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
		}
	}

	if(state){

		if(UsermenuParameter.ShutterMode==SHUTTER_MODE_OFF){
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
		}
		else{
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
		}
	}

	if(global_sys_ctrl_info.m_shutter_mode == SHUTTER_MODE_OFF){
		memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], "OFF", 4);
	}
	
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		ISP_set_auto_cut_flag_auto();
	}
#if 0
	if(state == 0)
	{
		if(UsermenuParameter.AutoShutter == 0)
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		else
		{
			ShutterTime = UsermenuParameter.AutoShutter/2.0;
			ShuttertTimeMIN = (uint16_t)(10 * ShutterTime);
			Display_uint8_Dec2(Xline,Yline,ShuttertTimeMIN/10,3,CharacterColor);
			DisPlayNewStr(Xline+16,Yline,".",3,CharacterColor);
			Display_uint8_Dec1(Xline+24,Yline,ShuttertTimeMIN%10,3,CharacterColor);
			DisPlayNewStr(Xline+32,Yline,"M",3,CharacterColor);
		}
	}
	else if(state == 1)
	{
		UsermenuParameter.AutoShutter++;
		ShutterTime = UsermenuParameter.AutoShutter/2.0;
		ShuttertTimeMIN = (uint16_t)(10 * ShutterTime);
		if(UsermenuParameter.AutoShutter>40)
			UsermenuParameter.AutoShutter = 40;
		else
		{
			ShutterTimer = 0;
			if(UsermenuParameter.AutoShutter>=0 && UsermenuParameter.AutoShutter<4)
			{
				//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffcffff)|(UsermenuParameter.AutoShutter<<16)));
			}
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			Display_uint8_Dec2(Xline,Yline,ShuttertTimeMIN/10,3,CharacterColor);
			DisPlayNewStr(Xline+16,Y_START+Y_GAP*Pt_Temp->Info.line_Y,".",3,CharacterColor);
			Display_uint8_Dec1(Xline+24,Y_START+Y_GAP*Pt_Temp->Info.line_Y,ShuttertTimeMIN%10,3,CharacterColor);
			DisPlayNewStr(Xline+32,Y_START+Y_GAP*Pt_Temp->Info.line_Y,"M",3,CharacterColor);
		}
		Int_Shutter_pro();
	}
	else
	{
		if(UsermenuParameter.AutoShutter>1)
		{
			ShutterTimer = 0;
			UsermenuParameter.AutoShutter--;
			ShutterTime = UsermenuParameter.AutoShutter/2.0;
			ShuttertTimeMIN = (uint16_t)(10 * ShutterTime);
			if(UsermenuParameter.AutoShutter>=0 && UsermenuParameter.AutoShutter<4)
			{
				//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffcffff)|(UsermenuParameter.AutoShutter<<16)));
			}
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			Display_uint8_Dec2(Xline,Yline,ShuttertTimeMIN/10,3,CharacterColor);
			DisPlayNewStr(Xline+16,Y_START+Y_GAP*Pt_Temp->Info.line_Y,".",3,CharacterColor);
			Display_uint8_Dec1(Xline+24,Y_START+Y_GAP*Pt_Temp->Info.line_Y,ShuttertTimeMIN%10,3,CharacterColor);
			DisPlayNewStr(Xline+32,Y_START+Y_GAP*Pt_Temp->Info.line_Y,"M",3,CharacterColor);
		}
		else
		{
			UsermenuParameter.AutoShutter = 0;
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffcffff)|(UsermenuParameter.AutoShutter<<16)));
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
		Int_Shutter_pro();
	}
#endif
}

/**
 * @brief:保存用户菜单的数据
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void SaveUsermenuParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	vision_log_debug("SaveUsermenuParameter Pt_Temp=%p, state=%d\n", Pt_Temp, state);
	if(state)
	{
		
		UpdatePromptInfo("Saving...", 1,VOT_LOG_TYPE_MENU1);
		ISP_write_sys_cfg();
		Write_TempParameter_cfg(&Temp_Parameter);
	
		UpdatePromptInfoWithTimeMs("save user menu para completed!", 2000,VOT_LOG_TYPE_MENU1);
		
		
		
	}
#if 0
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state)
	{
		DisPlayNewStr(Xline,Yline,SaveTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		Write_UsermenuParameterToFlash();
		ImagePartSave_flag = 1; //只保存当前图像模式下的参数值
		Write_CommonAreaParameterToFlash();
		ImagePartSave_flag = 0; //只保存当前图像模式下的参数值
		Write_TempParameterToFlash();//231205新增
		Write_QuadPosToFlash();//保存四边形的位置信息
		delay_1s(1);
		DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,SaveTab[1][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		delay_1s(1);
		DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
	}
#endif
}

/**
 * @brief:恢复出厂设置参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void ResetFactorySettingsParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	vision_log_debug("ResetFactorySettingsParameter Pt_Temp=%p, state=%d\n", Pt_Temp, state);

	if(state ){
		ts_u08 m_menu1_info_tip_visible = UsermenuParameter.m_menu1_info_tip_visible;
		UpdatePromptInfo("reset factory setting ing...", 1,VOT_LOG_TYPE_MENU1);
		
		//初始化用户菜单
		menu1_default_parameters_init();
		//初始化菜单二参数
		// menu2_default_parameters_init();
		Templut_Menu1_LoadDefault();

		//恢复isp出厂的默认配置
		reset_usr_isp_cfg();

		//参数生效
		userpara_reset();
		// testpara_reset();

		ISP_cut();
		ts_u08 m_menu2_info_tip_visible_tmp = SystemParaMenu2.m_menu2_info_tip_visible;
		UsermenuParameter.m_menu1_info_tip_visible = m_menu1_info_tip_visible;
		UpdatePromptInfoWithTimeMs("reset factory setting completed!", 2000,VOT_LOG_TYPE_MENU1);
		UsermenuParameter.m_menu1_info_tip_visible =  m_menu2_info_tip_visible_tmp; 
		
	}
	return;

#if 0
	if(state ==2)//只有左键可以初始化
	{

#if 0//lyb
		DisPlayNewStr(Xline,Yline,InitTab[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);
		SetFreeze_img(ON);		//图像冻结
		delay_1ms(50);
		ClearSportsInfo();//清除测温点信息
		ClearAreasInfo();//清除测温区域

		DisplayBackground(64,464,43,BACKGROUND_LAYER,0);//清除测温区域信息
#endif
		//MirrorBadRecover(UsermenuParameter.Mirror); //镜像复原
		UsermenuParameterInit();
#if 0//lyb
		Write_UsermenuParameterToFlash();
		Write_UserParameterToFPGA();
#endif
		CommonAreaParameterInit();
#if 0//lyb
		Write_CommonAreaParameterToFlash();
		Write_CommonAreaParameterToFPGA();
#endif
		vision_log_debug("UsermenuParameter.Bright=%d\n", UsermenuParameter.Bright);
		vision_log_debug("UsermenuParameter.Mirror=%d\n", UsermenuParameter.Mirror);
		vision_log_debug("UsermenuParameter.EE_Level=%d\n", UsermenuParameter.EE_Level);

		userpara_reset();//lyb
#ifdef	Electrolytic_copper
		if(UsermenuParameter.Quad_SW)//已经打开了
		{
			drawIrregularPolygonBox(vertices, 0);
			DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		}
		QuadPosInit();
		Write_QuadPosToFlash();
#endif
//		SysTimeAndSysShutInit();

//		ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息

#ifdef	Electrolytic_copper
		drawIrregularPolygonBox(vertices, 0);//清除四边形信息
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
#endif
#if 0
		Disp_Capture_Info();

		Templut_Menu2_LoadDefault();
		Write_TempParameterToFlash();
		Write_TempParameterToFPGA();
		DisplayCenterSpot_Init();//中心点光标显示

		Int_Shutter_pro();
		delay_1s(1);
#endif
		SelectionMenu.IsEnableEnligh = 0;	//中文
		SelectFontColor();
		//APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));

			Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
			Pt_Temp = Pt_Temp->Parent;
			Clear_MenuInfo(Pt_Temp,UTab1_Length);//清除一级菜单
			Display_MenuItem(Pt_Temp,UTab1_Length, 2);//显示一级菜单
			Pt_Temp = Pt_Temp->Son;
			Display_MenuItem(Pt_Temp,UTab4_Length, 6);//显示菜单一系统子链
#if 0
		Clear_MenuSonInfo(Pt_Temp,UTab4_Length,Pt_Temp->Parent->Son->Info.line_Y);//清除菜单一系统子链
		Clear_MenuInfo(Pt_Temp->Parent,UTab1_Length);//清除一级菜单

		Display_MenuItem(Pt_Temp->Parent,UTab1_Length, 2);//显示一级菜单
		Display_MenuItem(Pt_Temp,UTab4_Length, 6);//显示菜单一系统子链
		DisplaySelectionMenuFrame(Pt_Temp->Info.line_X,Y_START+Y_GAP*Pt_Temp->Info.line_Y,140,BACKGROUND_LAYER,6);
#endif
//		Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
//		DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
//		DisPlayNewStr(Xline,Yline,InitTab[1][SelectionMenu.IsEnableEnligh],3,CharacterColor);
//		delay_1s(1);
//		DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
#if(Mirror_DisplayType == 1)
		Menu1ParaInit_flag = 0;
#endif
	}

#endif
}

/**
 * @brief:将一个无符号8位整形转换成16进制并输出
 * @parameter:date:输入数据无符号8位整形
 * @parameter:output_date:16进制输出
 */
void ConvertDecimalToHex(ts_u08 date,ts_u08 *output_date)
{
	ts_u08 i;
	i= date/16;
	if(i<10)		output_date[0] =  date/16 + 0x30;
	else			output_date[0] =  date/16 + 0x37;
	i = date%16;
	if(i<10)		output_date[1] =  date%16 + 0x30;
	else			output_date[1] =  date%16 + 0x37;
}


/**
 * @brief:显示版本号
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void DisPlaySOCAndFPGAVersion(ts_u08 state)
{
	// vision_log_debug("DisPlaySOCAndFPGAVersion state=%d\n", state);

	//太长的话，需要调整线程栈的大小
	ts_char ver_info[64];
	int is_active=1;
	const char *suffix = "";  // 默认无后缀
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER){
		suffix = Focusslect[0];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
		suffix = Focusslect[1];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER){
		suffix = Focusslect[2];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		suffix = Focusslect[3];
	}

	if(state == 0)
	{
		is_active=0;
	}
	else
	{
		is_active=1;
	}
	

	// ts_cpl_snprintf(ver_info, 32, "%s", "T10_250717_PLK6 C25_0527P1_PLK6" );
	// ts_cpl_snprintf(ver_info, 64, "%s %s",VOT_HARDWARE_PLATFORM,VOT_SOFTWARE_PLATFORM );
	if (SystemParaMenu2.Usermenu_SW == 0) {
		ts_cpl_snprintf(ver_info, 64, "%s%s",VOT_SOFTWARE_VER_FIXED_HEADER,suffix);
	}
	else if ((SystemParaMenu2.Usermenu_SW >= 1) && (SystemParaMenu2.Usermenu_SW <= 3)) {
		ts_cpl_snprintf(ver_info, 64, "%s.T%s",VOT_SOFTWARE_VER_FIXED_HEADER,suffix);
	}
	else {
		ts_cpl_snprintf(ver_info, 64, "%s.E%s",VOT_SOFTWARE_VER_FIXED_HEADER,suffix);	// 异常值
	}
	
	DisplaySocVersionInfo(ver_info, is_active);

  	// ts_tdk_osd_refresh_display(shdl_osd_dev, 0, 1);
}


/**
 * @description: 显示版本信息
 * @param {ts_u08} Status 0：不显示；其他取值，自动翻转显示版本信息
 * @return {void}
 */
void DisPlayDeviceInfoAndSoftVersionInfoAuto(ts_u08 Status)
{

	ts_u08 visible_flag = 1 - global_sys_ctrl_info.m_dev_soft_info_visible_flag ; 

	//强制不显示
	if(Status==0){
		visible_flag=0;
	}

	if(visible_flag==1){
		DisPlaySOCAndFPGAVersion(1);
	}
	else{
		DisPlaySOCAndFPGAVersion(0);
	}


	global_sys_ctrl_info.m_dev_soft_info_visible_flag  = visible_flag;


}





#if 0
void FPGAVersionCharacterDisplay(uint32_t XLINE,uint32_t YLINE,uint8_t *pData,ts_u08 layer,ts_u08 color)
{
	ts_u08 *PData1;
	PData1 = &pData[1];

	if(pData[0] == '0')
	{
		DisPlayNewStr(XLINE,YLINE,"T",layer,color);
		DisPlayNewStr(XLINE+8,YLINE,PData1,layer,color);
	}
	else if(pData[0] == 'A')
	{
		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
		DisPlayNewStr(XLINE+8,YLINE,PData1,layer,color);
		DisPlayNewStr(XLINE+16,YLINE,"_PLK6_T",layer,color);
	}
	else if(pData[0] == 'B')
	{
		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
		DisPlayNewStr(XLINE+8,YLINE,PData1,layer,color);
		DisPlayNewStr(XLINE+16,YLINE,"_PLK6",layer,color);
	}
	else if(pData[0] == 'C')
	{
		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
		DisPlayNewStr(XLINE+8,YLINE,PData1,layer,color);
		DisPlayNewStr(XLINE+16,YLINE,"_RTD6_T",layer,color);
	}
	else if(pData[0] == 'D')
	{
		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
		DisPlayNewStr(XLINE+8,YLINE,PData1,layer,color);
		DisPlayNewStr(XLINE+16,YLINE,"_RTD6",layer,color);
	}
}

/**
 * @brief:获得以及显示：图像增强开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayDDE_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	switch(SelectionMenu.MenuState)
	{
		case 0x25:
			Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
			break;
		case 0x26:
			Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
			break;
	}
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.DDE_SW == 0)		{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else 															{DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);  }
	}
	else if(state ==1)
	{
		SystemParaMenu2.DDE_SW =1;
		APB3_REGW_DDE_SW(SystemParaMenu2.DDE_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
	}
	else
	{
		SystemParaMenu2.DDE_SW =0;
		APB3_REGW_DDE_SW(SystemParaMenu2.DDE_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.DDE_SW = SystemParaMenu2.DDE_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.DDE_SW = SystemParaMenu2.DDE_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.DDE_SW = SystemParaMenu2.DDE_SW;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.DDE_SW = SystemParaMenu2.DDE_SW;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.DDE_SW = SystemParaMenu2.DDE_SW;}
		}
	}
}
#endif

/**
 * @brief:获得以及显示：图像细节增强参数，DDE
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayDDELeverParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayDDELeverParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.DDE_Level=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.DDE_Level);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.DDE_Level<64)
		{
			UsermenuParameter.DDE_Level++;
		}
		else
		{
			UsermenuParameter.DDE_Level = 63;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.DDE_Level>0)
		{
			UsermenuParameter.DDE_Level--;
		}
		else
		{
			UsermenuParameter.DDE_Level = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.DDE_Level);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		ISP_dde_level(UsermenuParameter.DDE_Level*4);
	}


}

void GetAndDisplayEnhanceLeverParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayDDELeverParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.enhance_level=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.enhance_level);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.enhance_level<16)
		{
			UsermenuParameter.enhance_level++;
		}
		else
		{
			UsermenuParameter.enhance_level = 16;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.enhance_level>0)
		{
			UsermenuParameter.enhance_level--;
		}
		else
		{
			UsermenuParameter.enhance_level = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.enhance_level);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		ISP_enhance_set(UsermenuParameter.enhance_level*16);
	}


}


/**
 * @brief:获得以及显示：图像边缘增强参数，锐化
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayEELeverParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplayEELeverParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.EE_Level=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.EE_Level);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.EE_Level<16)
		{
			UsermenuParameter.EE_Level++;
		}
		else
		{
			UsermenuParameter.EE_Level = 16;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.EE_Level>0)
		{
			UsermenuParameter.EE_Level--;
		}
		else
		{
			UsermenuParameter.EE_Level = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.EE_Level);
	Display_MenuItemLabel(Pt_Temp, is_active);
	if(state){
		ISP_ee_set(UsermenuParameter.EE_Level*16);
	}



}

/**
 * @brief:获得以及显示：空间滤波开关
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaySFilter_SWParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#if 0
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	switch(SelectionMenu.MenuState)
	{
		case 0x25:
			Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
			break;
		case 0x26:
			Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+8;
			break;
	}
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.MedFilt_SW == 0)		{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else 															{DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);  }
	}
	else if(state ==1)
	{
		SystemParaMenu2.MedFilt_SW =1;
		APB3_REGW_MedFilt_SW(SystemParaMenu2.MedFilt_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
	}
	else
	{
		SystemParaMenu2.MedFilt_SW =0;
		APB3_REGW_MedFilt_SW(SystemParaMenu2.MedFilt_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
	}

	if(SystemParaMenu2.AlgorithmSW ==0) //YUV模式
	{
		if(UsermenuParameter.ImageAdjust ==0)  //手动
		{
			SystemParaMenu2.ImageManualPara.MedFilt_SW = SystemParaMenu2.MedFilt_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==1) //室内
		{
			SystemParaMenu2.ImageIndoorPara.MedFilt_SW = SystemParaMenu2.MedFilt_SW;
		}
		else if(UsermenuParameter.ImageAdjust ==2)//室外
		{
			if(SystemParaMenu2.OutdoorSel ==1)				{SystemParaMenu2.ImageOutdoorM3Para.MedFilt_SW = SystemParaMenu2.MedFilt_SW;}
			else if(SystemParaMenu2.OutdoorSel ==2)		{SystemParaMenu2.ImageOutdoorJLPara.MedFilt_SW = SystemParaMenu2.MedFilt_SW;}
			else	if(SystemParaMenu2.OutdoorSel ==0)		{SystemParaMenu2.ImageOutdoorPara.MedFilt_SW = SystemParaMenu2.MedFilt_SW;}
		}
	}
#endif
}

/**
 * @brief:获得以及显示：空间滤波参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaySFilterParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	// ts_u08 AddBuffer[32];
	
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("GetAndDisplaySFilterParameter Pt_Temp=%p, state=%d, MenuNameLen=%d, UsermenuParameter.SFilter=%d\n", Pt_Temp, state, MenuNameLen, UsermenuParameter.SFilter);

	if(state == 0)
	{
	}
	else if(state == 1)
	{
		if(UsermenuParameter.SFilter<32)
		{
			UsermenuParameter.SFilter++;
		}
		else
		{
			UsermenuParameter.SFilter = 32;
		}
	}
	else if(state == 2)
	{
		if(UsermenuParameter.SFilter>0)
		{
			UsermenuParameter.SFilter--;
		}
		else
		{
			UsermenuParameter.SFilter = 0;
		}
	}
	display_append_u8number(&Pt_Temp->Info.NameAdd[MenuNameLen], UsermenuParameter.SFilter);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		ISP_nr_set(UsermenuParameter.SFilter*8);
	}
	

}
#ifdef CONFIG_SNR_PLANCK1280C
void GetDisplayDeviceSenceMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayDeviceSenceMode state=%u UsermenuParameter.m_dev_env_mode %d \n", state,UsermenuParameter.m_dev_env_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  dev_env_mode = UsermenuParameter.m_dev_env_mode;
		if(dev_env_mode>=DEVICE_MODE_1280_BUTT){
			dev_env_mode = DEVICE_MODE_1280_R1;
		}

		vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

		if(state){	
		
			if(state==1){
				if(dev_env_mode<(DEVICE_MODE_1280_BUTT-1)){
					dev_env_mode++;
				}
			}
			else if(state==2){
				if(dev_env_mode>0){
					dev_env_mode--;			
				}
			}
		}

		if(dev_env_mode == DEVICE_MODE_1280_R1){
			sprintf((ts_char*)AddBuffer, "%s", ":R1");
		}
		else if(dev_env_mode == DEVICE_MODE_1280_R2){
			sprintf((ts_char*)AddBuffer, "%s", ":R2");
		}
		else if(dev_env_mode == DEVICE_MODE_1280_R2_1){
			sprintf((ts_char*)AddBuffer, "%s", ":R2-1");
		}
		else if(dev_env_mode == DEVICE_MODE_1280_R2_2){
			sprintf((ts_char*)AddBuffer, "%s", ":R2-2");
		}
			else if(dev_env_mode == DEVICE_MODE_1280_R2_3){
			sprintf((ts_char*)AddBuffer, "%s", ":R2-3");
		}

	

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(UsermenuParameter.m_dev_env_mode != dev_env_mode){
			UsermenuParameter.m_dev_env_mode = dev_env_mode;
			ISP_write_sys_cfg();
			reset_usr_isp_cfg();
			//只保存菜单一
        	ISP_write_sys_cfg();
		}
	}
	vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);
}

#else
	
void GetDisplayDeviceSenceMode(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	
	vision_log_debug("GetDisplayDeviceSenceMode state=%u UsermenuParameter.m_dev_env_mode %d \n", state,UsermenuParameter.m_dev_env_mode);

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	ts_u08  dev_env_mode = UsermenuParameter.m_dev_env_mode;

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    // vision_log_warn("res_type %d \n",res_type);
    
    if(res_type==DECTOR_RESOLUTION_384X288){
		if(!(dev_env_mode>=DEVICE_MODE_384_LOW_ATD && dev_env_mode<DEVICE_MODE_384_END)){
			dev_env_mode = DEVICE_MODE_384_LOW_ATD;
		}

		vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

		if(state){	
		
			if(state==1){
				if(dev_env_mode<(DEVICE_MODE_384_END-1)){
					dev_env_mode++;
				}
			}
			else if(state==2){
				if(dev_env_mode>DEVICE_MODE_384_LOW_ATD){
					dev_env_mode--;
				}
			}
		}

		if(dev_env_mode == DEVICE_MODE_384_LOW_ATD){
			sprintf((ts_char*)AddBuffer, "%s", "Low Atd");
		}
		else{
			sprintf((ts_char*)AddBuffer, "%s", "Low Atdrb");
		}
	
	}
	else {

		ts_u08 customer_type = get_customer_type();

		if(dev_env_mode>=DEVICE_MODE_OUTDOOR_BPC){
			dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
		}

		if (customer_type != CUSTOMER_TYPE_DSW)
		{
			if(dev_env_mode>=DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT &&
				dev_env_mode<=DEVICE_MODE_OUTDOOR_CITY_RED_HOT
			){
				dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
			}
		}

		vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

		if(state){	
		
			if(state==1){
				if(dev_env_mode<(DEVICE_MODE_OUTDOOR_BPC-1)){
					dev_env_mode++;

					//如果dev_env_mode等于DEVICE_MODE_NORMAL_END，则需要调整到定制的配置
					if (customer_type == CUSTOMER_TYPE_DSW)
					{
						if(dev_env_mode==DEVICE_MODE_NORMAL_END){
							dev_env_mode = DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT;
						}
						else{
							if(dev_env_mode>DEVICE_MODE_OUTDOOR_CITY_RED_HOT){
								dev_env_mode = DEVICE_MODE_OUTDOOR_CITY_RED_HOT;
							}
						}
					}
					else{
						if(dev_env_mode==DEVICE_MODE_NORMAL_END){
							dev_env_mode = DEVICE_MODE_NORMAL_END-1;
						}
					}

				}
			}
			else if(state==2){
				if(dev_env_mode>0){
					dev_env_mode--;
					
					if (customer_type == CUSTOMER_TYPE_DSW)
					{
						//如果dev_env_mode等于DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT，则需要调整到通用配置
						if(dev_env_mode==DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT-1){
							dev_env_mode = DEVICE_MODE_NORMAL_END-1;
						}
					}
					
				}
			}
		}

		if(dev_env_mode == DEVICE_MODE_OUTDOOR){
			sprintf((ts_char*)AddBuffer, "%s", "outdoor");
		}
		else if(dev_env_mode == DEVICE_MODE_INDOOR){
			sprintf((ts_char*)AddBuffer, "%s", "indoor");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_K1){
			sprintf((ts_char*)AddBuffer, "%s", "Low Atd");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_FLY){
			sprintf((ts_char*)AddBuffer, "%s", "Pod View");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_FLY_ENH){
			sprintf((ts_char*)AddBuffer, "%s", "Pod View2");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL){
			sprintf((ts_char*)AddBuffer, "%s", "Manual");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_RTD6){
			sprintf((ts_char*)AddBuffer, "%s", "RTD6");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_GUAN_NIAO){
			sprintf((ts_char*)AddBuffer, "%s", "BirdSight");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv1");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv2");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv3");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv4");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv5");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6){
			sprintf((ts_char*)AddBuffer, "%s", "LowAtdv6");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT){
			sprintf((ts_char*)AddBuffer, "%s", "CITY_HL");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT){
			sprintf((ts_char*)AddBuffer, "%s", "CITY_WH");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT){
			sprintf((ts_char*)AddBuffer, "%s", "CITY_BH");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_RED_HOT){
			sprintf((ts_char*)AddBuffer, "%s", "CITY_RH");
		}
		else if(dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_IRON_HOT){
			sprintf((ts_char*)AddBuffer, "%s", "CITY_IRH");
		}

	}

	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer)+1);
	Display_MenuItemLabel(Pt_Temp, is_active);

	if(state){
		
		if(UsermenuParameter.m_dev_env_mode != dev_env_mode){
			UsermenuParameter.m_dev_env_mode = dev_env_mode;
			ISP_write_sys_cfg();
			reset_usr_isp_cfg();
		
			//只保存菜单一
        	ISP_write_sys_cfg();
    
		}
	}
	vision_log_debug("GetDisplayDeviceEnvMode state=%u dev_env_mode %d  %d\n", state,dev_env_mode,UsermenuParameter.m_dev_env_mode);

}
#endif


static ts_u08 obtain_open_SpotorArea_num(ts_u16 state)
{
	ts_u08 count = 0,i;

	for(i = 0;i<16;i++)
	{
		if(state&0x01)
		{
			count++;
		}
		state = state>>1;
	}
	return count;
}

ts_u08 modify_Spot_sequence(struct Spot tempspot,ts_u08 state)
{
	ts_u08 i = 0;
	// ts_u08 result = 0;
	if(state == 1)
	{
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(SpotInit[i].spot_sequence >= tempspot.spot_sequence)
				{
					SpotInit[i].spot_sequence--;
				}
			}
		}
	}
	else
	{
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(SpotInit[i].spot_sequence >= tempspot.spot_sequence)
				{
					SpotInit[i].spot_sequence--;
				}
			}
		}
	}
	return 0;
}

ts_u08 serach_SpotArea_maxsequence(ts_u08 kind)
{
	ts_u08 i,index= 0;
	ts_u08 max_sequence = 0;
	if(kind == 0)
	{
		max_sequence = SpotInit[0].spot_sequence;
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(max_sequence <= SpotInit[i].spot_sequence)
				{
					max_sequence =  SpotInit[i].spot_sequence;
					index = i;
				}
			}
		}
		return index;
	}
	
	return 0;
}
/** @note 获取与显示测试点的测温参数与测温信息 */
ts_void getAndDisplay_Spots_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
	// ts_u16 tempValue = 0;

	if((Pt_Temp->Info.ID<0x210)||(Pt_Temp->Info.ID>0x218)) {
		return;
	}

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d\n", Pt_Temp, state, menuName_Len);
	
	ts_u08 Serial = (ts_u08)(Pt_Temp->Info.ID - 0x210);
	ts_u16 Order = 1<<Serial;

	if (state == 0) {
		if(Spot_state & Order) {
			sprintf((ts_char*)addBuffer, "%s", "ON");
			DisPlayIcon(SpotInit[Serial].spot_ID,SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,1);
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "OFF");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1)
	{
		if((Spot_state&Order) == 0)//第一次启动 
		{
			Spot_state = (Spot_state&(0xffff-Order))|Order;
			SpotInit[Serial].spot_sequence = obtain_open_SpotorArea_num(Spot_state);
			select_Spot = &SpotInit[Serial];
			sprintf((ts_char*)addBuffer, "%s", "ON");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			DisPlayIcon(SpotInit[Serial].spot_ID,SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,1);
			Disp_SpotN_Messages(&SpotInit[Serial],0);
		}
		else {
			if(SpotInit[Serial].spot_sequence != select_Spot->spot_sequence)//再次启动一次
			{
				modify_Spot_sequence(SpotInit[Serial],1);
				SpotInit[Serial].spot_sequence = obtain_open_SpotorArea_num(Spot_state);
				select_Spot = &SpotInit[Serial];
				Disp_SpotN_Messages(&SpotInit[Serial],0);
			}
		}
	}
	else if(state == 2)
	{
		if(Spot_state&Order)
		{
			Spot_state = (Spot_state&(0xffff-Order))|0x0000;
			modify_Spot_sequence(SpotInit[Serial],0);
			SpotInit[Serial].spot_sequence = 0;	
			if(SpotInit[serach_SpotArea_maxsequence(0)].spot_sequence == 0)
			{
				select_Spot = NULL;
			}
			else
			{
				select_Spot = &SpotInit[serach_SpotArea_maxsequence(0)];
				Disp_SpotN_Messages(&SpotInit[Serial], 0);
			}	
			sprintf((ts_char*)addBuffer, "%s", "OFF");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			
			DisPlayIcon(SpotInit[Serial].spot_ID,SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,0);
			// UpdateSpecialInfo("info_tip", SpotInit[Serial].spot_X_Pos, SpotInit[Serial].spot_Y_Pos+10, 0);
			Refresh_templutSpot_Captions("", &SpotInit[Serial], 0);
		}
	}
}
/** @note 获取与显示测试区域的测温参数与测温信息 */
ts_void getAndDisplay_Areas_Parameter (Point_MenuStructure Pt_Temp,ts_u08 state, ts_u08 is_active) {
	ts_u08 menuName_Len;
	ts_u08 addBuffer[32];
	// ts_char special_Mess[64] ={0};

	if((Pt_Temp->Info.ID < 0x220) || (Pt_Temp->Info.ID > 0x223))
	{
		return;
	}
	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, is_active = %d, menuName_Len = %d Area_state = 0x%x\n", Pt_Temp, state, is_active, menuName_Len, Area_state);

	ts_u08 Serial = (ts_u08)(Pt_Temp->Info.ID - 0x220);
	ts_u16 Order = 1 << Serial;
	if (state == 0) {
		if(Area_state&Order) {
			sprintf((ts_char*)addBuffer, "%s", "ON");
			select_Area = &Areainit[Serial];
			DisPlay_AREA_Icon(select_Area, 0, 1);
		}
		else {
			sprintf((ts_char*)addBuffer, "%s", "OFF");
		}
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
		Display_MenuItemLabel(Pt_Temp, is_active);
	}
	else if (state == 1) {
		if((Area_state&Order) == 0) {
			Area_state = (Area_state&(0xffff-Order))|Order;
			select_Area = &Areainit[Serial];
			sprintf((ts_char*)addBuffer, "%s", "ON");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			DisPlay_AREA_Icon(select_Area, 0, 1);
		}
	}
	else {
		if(Area_state&Order) {
			Area_state = (Area_state&(0xffff-Order))|0x0000;
			ClearUncheckAreaIcon(&Areainit[Serial]);
			Refresh_templut_AreaCaptions("", &Areainit[Serial], 0);
			if(Area_state == 0) {
				select_Area = NULL;
			}
			sprintf((ts_char*)addBuffer, "%s", "OFF");
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
			Display_MenuItemLabel(Pt_Temp, is_active);
		}
	}
}
/** @note 捕获参数 */
void getAndDisplay_lutCapture_Parameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 menuName_Len;
	// ts_u08 addBuffer[32];

	menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	vision_log_debug("Pt_Temp = %p, state = %d, menuName_Len = %d, SystemParaMenu2.Usermenu_SW = %d\n", Pt_Temp, state, menuName_Len, SystemParaMenu2.Usermenu_SW);

	if (state == 0) {
		memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], CaptureParameter[UsermenuParameter.CaptureFlag], strlen((ts_char*)CaptureParameter[UsermenuParameter.CaptureFlag])+1);
		Display_MenuItemLabel(Pt_Temp, is_active);
		Disp_Capture_Info();
	}
	else if (state == 1) {
		if(UsermenuParameter.CaptureFlag == 0) {
			Disp_Capture_Spot_MAX_info(OFF);
		}
		else if(UsermenuParameter.CaptureFlag == 1) {
			Disp_Capture_Spot_MIN_info(OFF);
		}
		UsermenuParameter.CaptureFlag++;
		if(UsermenuParameter.CaptureFlag > 3) {
			UsermenuParameter.CaptureFlag = 3;
		}
		else {
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], CaptureParameter[UsermenuParameter.CaptureFlag], strlen((ts_char*)CaptureParameter[UsermenuParameter.CaptureFlag])+1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			Disp_Capture_Info();
		}
	}
	else {
		if(UsermenuParameter.CaptureFlag > 0) {
			if(UsermenuParameter.CaptureFlag == 3) {
				Disp_Capture_Spot_MIN_info(OFF);
			}
			else if(UsermenuParameter.CaptureFlag == 2) {
				Disp_Capture_Spot_MAX_info(OFF);
			}
			UsermenuParameter.CaptureFlag --;
			memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], CaptureParameter[UsermenuParameter.CaptureFlag], strlen((ts_char*)CaptureParameter[UsermenuParameter.CaptureFlag])+1);
			Display_MenuItemLabel(Pt_Temp, is_active);
			Disp_Capture_Info();
		}
		else {
			UsermenuParameter.CaptureFlag = 0;
		}
	}
}
/** @note 菜单一测温档位切换(仅在测温模式下有效，Usermenu_SW >= 1) */
void getAndDisplay_lutLevel_Parameter(Point_MenuStructure Pt_Temp, ts_u08 state, ts_u08 is_active)
{
    static const char* const gear_str[4] = {"OFF", "ONE", "TWO", "THR"};
    ts_u08 menuName_Len;
    ts_u08 addBuffer[32];

    if (SystemParaMenu2.m_templutMode_Limit < 1 || SystemParaMenu2.m_templutMode_Limit > 3) {
        SystemParaMenu2.m_templutMode_Limit = 1;
    }
    /* 防御性检查 */
    if (SystemParaMenu2.Usermenu_SW < 1) {
        vision_log_debug("WARN: getAndDisplay called but Usermenu_SW=%d (OFF)\n", SystemParaMenu2.Usermenu_SW);
        UpdatePromptInfoWithTimeMs("Enable Temp mode first", 1000, VOT_LOG_TYPE_MENU1);
        return;
    }
    menuName_Len = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
    vision_log_debug("Pt_Temp=%p, state=%d, Usermenu_SW=%d, Limit=%d\n", Pt_Temp, state, SystemParaMenu2.Usermenu_SW, SystemParaMenu2.m_templutMode_Limit);

    if (state == 0) {
        sprintf((ts_char*)addBuffer, "%s", gear_str[SystemParaMenu2.Usermenu_SW]);
        memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
        Display_MenuItemLabel(Pt_Temp, is_active);
        return;
    }

    if (state == 1) {
        /* 加挡 */
        if (SystemParaMenu2.Usermenu_SW >= SystemParaMenu2.m_templutMode_Limit) {
            UpdatePromptInfoWithTimeMs("Max Level Reached", 1000, VOT_LOG_TYPE_MENU1);
            return;
        }
        SystemParaMenu2.Usermenu_SW++;
    } else {
        /* 减挡 */
        if (SystemParaMenu2.Usermenu_SW <= 1) {
            UpdatePromptInfoWithTimeMs("Min Level Reached", 1000, VOT_LOG_TYPE_MENU1);
            return;
        }
        SystemParaMenu2.Usermenu_SW--;
    }
    global_sys_ctrl_info.Usermenu_SW_Switching = 1;
    UpdatePromptInfo("Temperature SW Change...", 1, VOT_LOG_TYPE_MENU1);

    sprintf((ts_char*)addBuffer, "%s", gear_str[SystemParaMenu2.Usermenu_SW]);
    memcpy(&Pt_Temp->Info.NameAdd[menuName_Len], addBuffer, strlen((ts_char*)addBuffer) + 1);
    Display_MenuItemLabel(Pt_Temp, is_active);

    global_sys_ctrl_info.Shifting_Gears_Flag = 1;
    TemLevel_convert();
    global_sys_ctrl_info.Shifting_Gears_Flag = 0;

    UpdatePromptInfo("Temperature SW Change...", 0, VOT_LOG_TYPE_MENU1);
    global_sys_ctrl_info.Usermenu_SW_Switching = 0;
	//
    ISP_write_sys_menu2_cfg(SystemParaMenu2);
    if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG) {
        SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
        cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
        cvbs_osd_refresh_display(1);
    }
}
#if 0
/**
 * @brief:获得以及显示：中心点光标显示开关（无温度）
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayCenterSpotParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;

	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;

	if(state == 0)
	{
		if(UsermenuParameter.CenterSpotSW == 0)				{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else if(UsermenuParameter.CenterSpotSW == 1)	 	{DisPlayNewStr(Xline,Yline,"Little",3,CharacterColor);  }
		else if(UsermenuParameter.CenterSpotSW == 2)	 	{DisPlayNewStr(Xline,Yline,"Large",3,CharacterColor);  }
	}
	else if(state ==1)
	{
		if(SystemParaMenu2.Usermenu_SW)	return;
		UsermenuParameter.CenterSpotSW++;
		if(UsermenuParameter.CenterSpotSW > 2)
		{
			UsermenuParameter.CenterSpotSW = 2;
		}
		else
		{
		DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
		if(UsermenuParameter.CenterSpotSW == 0)				{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else if(UsermenuParameter.CenterSpotSW == 1)	 	{DisPlayNewStr(Xline,Yline,"Little",3,CharacterColor);  }
		else if(UsermenuParameter.CenterSpotSW == 2)	 	{DisPlayNewStr(Xline,Yline,"Large",3,CharacterColor);  }
		Display_CenterSpot(UsermenuParameter.CenterSpotSW);
		}
	}
	else
	{
		if(SystemParaMenu2.Usermenu_SW)	return;
		if(UsermenuParameter.CenterSpotSW > 0)
		{
			UsermenuParameter.CenterSpotSW --;
		}
		else
		{
			UsermenuParameter.CenterSpotSW = 0;
		}
		DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
		if(UsermenuParameter.CenterSpotSW == 0)				{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else if(UsermenuParameter.CenterSpotSW == 1)	 	{DisPlayNewStr(Xline,Yline,"Little",3,CharacterColor);  }
		else if(UsermenuParameter.CenterSpotSW == 2)	 	{DisPlayNewStr(Xline,Yline,"Large",3,CharacterColor);  }
		Display_CenterSpot(UsermenuParameter.CenterSpotSW);
	}
}


/**
 * @brief:获取与显示测试点的测温参数与测温信息
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplaySpotsParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if((Pt_Temp->Info.ID<0x210)||(Pt_Temp->Info.ID>0x218))
	{
		return;
	}
	ts_u08 Serial = (ts_u08)(Pt_Temp->Info.ID - 0x210);
	ts_u16 Order = 1<<Serial;
	if(state == 0)
	{
		if(Spot_state&Order)
		{
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
			DisPlayIcon(0,SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,2,CharacterColor);
		}
		else
		{
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
	else if(state == 1)//右启动
	{
		if((Spot_state&Order) == 0)//第一次启动
		{
			Spot_state = (Spot_state&(0xffff-Order))|Order;
			SpotInit[Serial].spot_sequence = obtain_open_SpotorArea_num(Spot_state);
			select_Spot = &SpotInit[Serial];
			Disp_SpotN_Messages(&SpotInit[Serial],0);
			DisplayBackground(Xline,Yline,6,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON ",3,CharacterColor);
			DisPlayIcon(0,SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,2,CharacterColor);
		}
		else
		{
			if(SpotInit[Serial].spot_sequence != select_Spot->spot_sequence)//再次启动一次
			{
				modify_Spot_sequence(SpotInit[Serial],1);
				SpotInit[Serial].spot_sequence = obtain_open_SpotorArea_num(Spot_state);
				select_Spot = &SpotInit[Serial];
				Disp_SpotN_Messages(&SpotInit[Serial],0);
			}
		}
	}
	else if(state == 2)//左关
	{
		if(Spot_state&Order)
		{
			Spot_state = (Spot_state&(0xffff-Order))|0x0000;

			modify_Spot_sequence(SpotInit[Serial],0);
			SpotInit[Serial].spot_sequence = 0;
			if(SpotInit[serach_SpotArea_maxsequence(0)].spot_sequence == 0)
			{
				select_Spot = NULL;
			}
			else
			{
				select_Spot = &SpotInit[serach_SpotArea_maxsequence(0)];
				Disp_SpotN_Messages(select_Spot,0);
			}
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
			DisPlaySpot(SpotInit[Serial].spot_X_Pos,SpotInit[Serial].spot_Y_Pos,2,0,0);
			DisplayBackground(SpotInit[Serial].spot_X_Pos+5,SpotInit[Serial].spot_Y_Pos+2,10,BACKGROUND_LAYER,0);
			DisplayBackground(SpotInit[Serial].spot_X_Pos-58,SpotInit[Serial].spot_Y_Pos-18,7,BACKGROUND_LAYER,0);
		}
	}
}

/**
 * @brief:获取与显示测试区域的测温参数与测温信息
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayAreasParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if((Pt_Temp->Info.ID<0x220)||(Pt_Temp->Info.ID>0x223))
	{
		return;
	}
	ts_u08 Serial = (ts_u08)(Pt_Temp->Info.ID - 0x220);
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
			select_Area = &Areainit[Serial];
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
		}
	}
	else//左关
	{
		if(Area_state&Order)
		{
			Area_state = (Area_state&(0xffff-Order))|0x0000;
			ClearUncheckAreaIcon(&Areainit[Serial]);
			if(Area_state == 0)
			{
				select_Area = NULL;
				DisplayBackground(64,464,43,BACKGROUND_LAYER,0);
			}
			DisplayBackground(Xline,Yline,2,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
		}
	}
}

/**
 * @brief:获得以及显示：捕获参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayCaptureParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		DisPlayNewStr(Xline,Yline,CaptureParameter[UsermenuParameter.CaptureFlag],3,CharacterColor);
		Disp_Capture_Info();
	}
	else if(state == 1)
	{
		UsermenuParameter.CaptureFlag++;
		if(UsermenuParameter.CaptureFlag > 3)
		{
			UsermenuParameter.CaptureFlag = 3;
		}
		else
		{
			DisplayBackground(Xline,Yline,7,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,CaptureParameter[UsermenuParameter.CaptureFlag],3,CharacterColor);
			Disp_Capture_Info();
		}
	}
	else
	{
		if(UsermenuParameter.CaptureFlag > 0)
		{
			UsermenuParameter.CaptureFlag --;
			DisplayBackground(Xline,Yline,8,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,CaptureParameter[UsermenuParameter.CaptureFlag],3,CharacterColor);
			Disp_Capture_Info();
		}
		else
			UsermenuParameter.CaptureFlag = 0;
	}
}

/**
 * @brief:获得以及显示：测温挡位参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayTemperatureLevelParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(SystemParaMenu2.Usermenu_SW == 1)
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
		if(SystemParaMenu2.Usermenu_SW ==1)
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"TWO",3,CharacterColor);
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Change...",4,6);

			SetFreeze_img(ON);		//图像冻结
			delay_25ms(20);
			SystemParaMenu2.Usermenu_SW = 2;//二档测温  0
			APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xffffdfff));
//		Write_TestmenuParameterToFPGA();
			TemLevel_convert();
//			delay_25ms(5);
			delay_25ms(20);
			Int_Shutter_pro();
			delay_25ms(30);
			Int_Shutter_pro();
			SetFreeze_img(OFF); //图像激活

			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Done",4,6);
			delay_25ms(25);
			DisplayBackground(32,400,12,BACKGROUND_LAYER,0);
		}
	}
	else
	{
		if(SystemParaMenu2.Usermenu_SW ==2)
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,"ONE",3,CharacterColor);
			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Change...",4,6);

			SetFreeze_img(ON);		//图像冻结
			delay_25ms(20);
			SystemParaMenu2.Usermenu_SW = 1;
			APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)|0x2000));
//		Write_TestmenuParameterToFPGA();
			TemLevel_convert();
			delay_25ms(20);
			Int_Shutter_pro();
			delay_25ms(20);
			Int_Shutter_pro();
			SetFreeze_img(OFF); //图像激活

			DisplayBackground(32,400,10,BACKGROUND_LAYER,3);
			DisPlayNewStr(32,400,"Done",4,6);
			delay_25ms(25);
			DisplayBackground(32,400,12,BACKGROUND_LAYER,0);
		}
	}
}

/**
 * @brief:打开四边形区域显示
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayQuadSwitchParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef	Electrolytic_copper
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		DisPlayNewStr(Xline,Yline,SwitchTab[UsermenuParameter.Quad_SW],3,CharacterColor);
	}
	else if(state ==1)
	{
		if(UsermenuParameter.Quad_SW == 0)
		{
			UsermenuParameter.Quad_SW = 1;
			APB3_REGW(IO_APB_PAT_0X25,0,((APB3_REGR(IO_APB_PAT_0X25,0)&0xfffffffef)|(1<<4)));//开启计算四边形区域温度
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,SwitchTab[UsermenuParameter.Quad_SW],3,CharacterColor);
			drawIrregularPolygonBox(vertices, CharacterColor);
		}
	}
	else
	{
		if(UsermenuParameter.Quad_SW)
		{
			UsermenuParameter.Quad_SW = 0;
			APB3_REGW(IO_APB_PAT_0X25,0,((APB3_REGR(IO_APB_PAT_0X25,0)&0xfffffffef)|(0<<4)));//关闭计算四边形区域温度
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			DisPlayNewStr(Xline,Yline,SwitchTab[UsermenuParameter.Quad_SW],3,CharacterColor);
			drawIrregularPolygonBox(vertices, 0);
			DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		}
	}
#endif
}


/**
 * @brief:设置四边形温度报警阈值
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayQuadAlarmSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state)
{
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
		Display_uint8_Dec2(Xline+6,Yline,SystemParaMenu2.Quad_TemAlarm/10,3,CharacterColor);
		DisPlayNewStr(Xline+22,Yline,".",3,CharacterColor);
		Display_uint8_Dec1(Xline+27,Yline,SystemParaMenu2.Quad_TemAlarm%10,3,CharacterColor);
		DisPlayNewStr(Xline+36,Yline,"℃",3,CharacterColor);
	}
	else if(state ==1)
	{
		if(UsermenuParameter.Quad_SW == 1)
		{
			if(SystemParaMenu2.Quad_TemAlarm < 1000)
				SystemParaMenu2.Quad_TemAlarm += 5;
			else
				SystemParaMenu2.Quad_TemAlarm = 1000;
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			Display_uint8_Dec2(Xline+6,Yline,SystemParaMenu2.Quad_TemAlarm/10,3,CharacterColor);
			DisPlayNewStr(Xline+22,Yline,".",3,CharacterColor);
			Display_uint8_Dec1(Xline+27,Yline,SystemParaMenu2.Quad_TemAlarm%10,3,CharacterColor);
			DisPlayNewStr(Xline+36,Yline,"℃",3,CharacterColor);
		}
	}
	else
	{
		if(UsermenuParameter.Quad_SW == 1)
		{
			if(SystemParaMenu2.Quad_TemAlarm > 0)
				SystemParaMenu2.Quad_TemAlarm -= 5;
			else
				SystemParaMenu2.Quad_TemAlarm = 0;
			DisplayBackground(Xline,Yline,5,BACKGROUND_LAYER,BackgroundColor);
			Display_uint8_Dec2(Xline+6,Yline,SystemParaMenu2.Quad_TemAlarm/10,3,CharacterColor);
			DisPlayNewStr(Xline+22,Yline,".",3,CharacterColor);
			Display_uint8_Dec1(Xline+27,Yline,SystemParaMenu2.Quad_TemAlarm%10,3,CharacterColor);
			DisPlayNewStr(Xline+36,Yline,"℃",3,CharacterColor);
		}
	}
}
#endif

/**
 * @brief:获得以及显示：辐射率参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayRadianceParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_char AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);

	if(state == 0){

	}
	else if(state ==1) {
		if(Temp_Parameter.Emiss < 0x64)
			Temp_Parameter.Emiss++;
		else
			Temp_Parameter.Emiss = 100;
	}
	else {
		if(Temp_Parameter.Emiss > 0)
			Temp_Parameter.Emiss--;
		else
			Temp_Parameter.Emiss = 0;
	}
	if(Temp_Parameter.Emiss == 100) {
		ts_cpl_snprintf((ts_char*)AddBuffer, sizeof(AddBuffer), "%s","1.00");
	}
	else if(Temp_Parameter.Emiss < 10) {
		ts_cpl_snprintf((ts_char*)AddBuffer, sizeof(AddBuffer), "0.0%d",Temp_Parameter.Emiss);
	}
	else {
		ts_cpl_snprintf((ts_char*)AddBuffer, sizeof(AddBuffer), "0.%d",Temp_Parameter.Emiss);
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
}

/**
 * @brief:获得以及显示：反射温度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayReflectionTemperatureParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(state == 0) {

	}
	else if(state ==1) {
		if(Temp_Parameter.Trefl < 0x0640)
			Temp_Parameter.Trefl+= 10;
		else
			Temp_Parameter.Trefl = 0x0640;
	}
	else {
		if(Temp_Parameter.Trefl > 0)
			Temp_Parameter.Trefl-=10;
		else
			Temp_Parameter.Trefl = 0;
	}
	sprintf((ts_char*)AddBuffer, "%+d℃", Temp_Parameter.Trefl/10-60);
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
}
/**
 * @brief:获得以及显示：环境温度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayEnvironmentTemperatureParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(state == 0)
	{

	}
	else if(state ==1)
	{
		if(Temp_Parameter.Tamb < 0x0640)
			Temp_Parameter.Tamb+= 10;
		else
			Temp_Parameter.Tamb = 0x0640;
	}
	else
	{
		if(Temp_Parameter.Tamb > 0)
			Temp_Parameter.Tamb-=10;
		else
			Temp_Parameter.Tamb = 0;
	}
	sprintf((ts_char*)AddBuffer, "%+d℃", Temp_Parameter.Tamb/10-60);
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
}

/**
 * @brief:获得以及显示：相对湿度参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayRelativeHumidityParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(state == 0) {

	}
	else if(state ==1) {
		if(Temp_Parameter.RelHum < 100)
			Temp_Parameter.RelHum+= 1;
		else
			Temp_Parameter.RelHum = 100;
	}
	else {
		if(Temp_Parameter.RelHum > 0)
			Temp_Parameter.RelHum-=1;
		else
			Temp_Parameter.RelHum = 0;
	}
	sprintf((ts_char*)AddBuffer, "%d%%", Temp_Parameter.RelHum);
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
}

/**
 * @brief:获得以及显示：距离参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayDistanceParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(state == 0)
	{

	}
	else if(state ==1)
	{
		if(Temp_Parameter.distance < 1000)
			Temp_Parameter.distance+= 10;
		else
			Temp_Parameter.distance = 1000;
	}
	else
	{
		if(Temp_Parameter.distance > 0)
			Temp_Parameter.distance-=10;
		else
			Temp_Parameter.distance = 0;
	}
	if(Temp_Parameter.distance == 0) {
		sprintf((ts_char*)AddBuffer, "%s","None");
	}
	else {
		sprintf((ts_char*)AddBuffer, "%dM", Temp_Parameter.distance/10);
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
}
/**
 * @brief:获得以及显示：镜头设置参数
 * @parameter:Pt_Temp:指向当前选中菜单列表项的指针
 * @parameter:state:0:初始化显示此参数,1：参数++,2:参数--
 */
void GetAndDisplayLensSetParameter(Point_MenuStructure Pt_Temp,ts_u08 state,ts_u08 is_active)
{
	ts_u08 MenuNameLen;
	ts_u08 AddBuffer[32];
	ts_u08 tmp_lens_Set = Temp_Parameter.Lens_Set;

	MenuNameLen = strlen((ts_char*)Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh]);
	if(state == 0) {	

	}
	else if(state ==1) {
		if(tmp_lens_Set < 14)
			tmp_lens_Set++;
		else
			tmp_lens_Set = 14;
	}
	else {
		if(tmp_lens_Set > 0)
			tmp_lens_Set--;
		else
			tmp_lens_Set = 0;
	}
	/*************************************************************************************
		镜头选择，A---定焦镜头/手调镜头，B---电调镜头
	*************************************************************************************/
	if(tmp_lens_Set == 0)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_4.6");
	}
	else if(tmp_lens_Set == 1)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_9.1");
	}
	else if(tmp_lens_Set == 2)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_13");
	}
	else if(tmp_lens_Set == 3)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_15");
	}
	else if(tmp_lens_Set == 4)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_19");
	}
	else if(tmp_lens_Set == 5)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_25");
	}
	else if(tmp_lens_Set == 6)
	{
		sprintf((ts_char*)AddBuffer, "%s","B_25");
	}
	else if(tmp_lens_Set == 7)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_35");
	}
	else if(tmp_lens_Set == 8)
	{
		sprintf((ts_char*)AddBuffer, "%s","B_35");
	}
	else if(tmp_lens_Set == 9)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_50");
	}
	else if(tmp_lens_Set == 10)
	{
		sprintf((ts_char*)AddBuffer, "%s","B_50");
	}
	else if(tmp_lens_Set == 11)
	{
		sprintf((ts_char*)AddBuffer, "%s","B_75");
	}
	else if(tmp_lens_Set == 12)
	{
		sprintf((ts_char*)AddBuffer, "%s","B_100");
	}
	else if(tmp_lens_Set == 13)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_6.6");
	}
	else if(tmp_lens_Set == 14)
	{
		sprintf((ts_char*)AddBuffer, "%s","A_5.7");
	}
	memcpy(&Pt_Temp->Info.NameAdd[MenuNameLen], AddBuffer, strlen((ts_char*)AddBuffer) + 1);
	Display_MenuItemLabel(Pt_Temp, is_active);
	Temp_Parameter.Lens_Set = tmp_lens_Set;
}

void GetAndDisplaySunProtect(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef OPR_ALARM
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;

	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		if(UsermenuParameter.SunPT_SW == 0)		{DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);}
		else 									{DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);  }
	}
	else if(state ==1)
	{
		UsermenuParameter.SunPT_SW =1;
		APB3_REGW_SunPT_SW(UsermenuParameter.SunPT_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"ON",3,CharacterColor);
	}
	else
	{
		UsermenuParameter.SunPT_SW =0;
		APB3_REGW_SunPT_SW(UsermenuParameter.SunPT_SW);
		DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
		DisPlayNewStr(Xline,Yline,"OFF",3,CharacterColor);
	}
#endif
}

void GetAndDisplayProtectTime(Point_MenuStructure Pt_Temp,ts_u08 state)
{
#ifdef OPR_ALARM
	ts_u32 Xline = 0;
	ts_u32 Yline = 0;
	Xline = Pt_Temp->Info.line_X+8*GetStringSize(Pt_Temp->Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh;
	Yline = Y_START+Y_GAP*Pt_Temp->Info.line_Y;
	if(state == 0)
	{
		display_u8number(Xline,Yline,UsermenuParameter.P_Time,3,CharacterColor);
		DisPlayNewStr(Xline+16,Yline,"M",3,CharacterColor);
	}
	else if(state == 1)
	{
		UsermenuParameter.P_Time++;
		if(UsermenuParameter.P_Time > 20)
			UsermenuParameter.P_Time = 20;
		else
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,UsermenuParameter.P_Time,3,CharacterColor);
			DisPlayNewStr(Xline+16,Yline,"M",3,CharacterColor);
		}
	}
	else
	{
		UsermenuParameter.P_Time--;
		if(UsermenuParameter.P_Time < 1)
			UsermenuParameter.P_Time = 1;
		else
		{
			DisplayBackground(Xline,Yline,3,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(Xline,Yline,UsermenuParameter.P_Time,3,CharacterColor);
			DisPlayNewStr(Xline+16,Yline,"M",3,CharacterColor);
		}
	}
#endif
}
/**
 * @description: 配置提示词，用于初始化，保存等操作，给用户提示
 * @param {ts_char} *prompt_info：具体提示词字符串内容
 * @param {ts_s08} keep_seconds ：keep_seconds小于0：一直显示；keep_seconds大于零：等待keep_seconds秒
 * @return {void}
 */
void SetDisplayUserPrompt(ts_char *prompt_info, ts_s08 keep_seconds)
{
	if(prompt_info==NULL){
		return;
	}
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

	global_sys_ctrl_info.m_prompt_keep_seconds = keep_seconds;
	snprintf(global_sys_ctrl_info.m_prompt_info, 64, "%s",prompt_info );
	global_sys_ctrl_info.m_prompt_refresh_flag = 1;
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
}

/**
 * @description: 获取提示词信息
 * @param {ts_char} *prompt_info：目的存储地址
 * @param {int} max_bytes：       目的地址的长度
 * @param {ts_s08} *s08_keep_seconds  目的秒数
 * @return {*}
 */
void GetDisplayUserPrompt(ts_char *prompt_info, int max_bytes, ts_s08 *s08_keep_seconds)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

	if(prompt_info){
		snprintf(prompt_info, max_bytes, "%s",global_sys_ctrl_info.m_prompt_info );
	}

	if(s08_keep_seconds){
		*s08_keep_seconds = global_sys_ctrl_info.m_prompt_keep_seconds;
	}

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
}

/**
 * @description: 获取刷新标志
 * @return {ts_u08} 
 */
ts_u08 GetPromptRefreshFlag(void)
{
	ts_u08  refresh_flag = 0;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

	refresh_flag = global_sys_ctrl_info.m_prompt_refresh_flag;
	global_sys_ctrl_info.m_prompt_refresh_flag = 0;
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

	return refresh_flag;
}


/**
 * @description: 主动关闭提示信息
 * @param {ts_u08} is_visible, 0:关闭提示信息，其他值：保持现状
 * @return {void}
 */
void SetPromptVisibleFlag(ts_u08 is_visible)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
	global_sys_ctrl_info.m_prompt_visible_flag  = is_visible;
	if(is_visible==0){
		global_sys_ctrl_info.m_prompt_keep_seconds = 0; //时间配置为0,关闭显示
		global_sys_ctrl_info.m_prompt_refresh_flag = 1;
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

}

/**
 * @description: 刷新提示词显示
 * @param {ts_char} *user_info：提示词内容
 * @param {ts_u08} is_active： 是否显示，0：关闭， 1：显示
 * @return {void}
 */
void UpdatePromptInfo(ts_char *user_info, ts_u08 is_active, ts_u08 log_type)
{
	//判断用户菜单过程提示信息是否显示
	if(log_type==VOT_LOG_TYPE_MENU1 && UsermenuParameter.m_menu1_info_tip_visible==VOT_SWITCH_OFF_LIKE1){
		is_active = 0;
	}
	else if(log_type==VOT_LOG_TYPE_MENU2 && SystemParaMenu2.m_menu2_info_tip_visible==VOT_SWITCH_OFF_LIKE1){
		is_active = 0;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);

	if(is_active==1){
		DisplayUserInfo(user_info, 1);
		global_sys_ctrl_info.m_prompt_visible_flag=1;
	}
	else{
		if(global_sys_ctrl_info.m_prompt_visible_flag==1){
			DisplayUserInfo(user_info, 0);
		}
		global_sys_ctrl_info.m_prompt_visible_flag=0;

	}

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
	
}

void UpdatePromptInfoWithTimeMs(ts_char *user_info, ts_u32 u32_msecs,ts_u08 log_type)
{
	UpdatePromptInfo(user_info, 1,log_type);
	ts_cpl_msleep(u32_msecs);
	UpdatePromptInfo(user_info, 0,log_type);

}

