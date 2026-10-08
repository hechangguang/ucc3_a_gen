#include <base/ts_type.h>
#include"menu_save_flash.h"
#include <menu/usermenufunction.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

void UsermenuParameterInit(void)
{
// 	SystemParaMenu2.Bri_M = 74;
// 	SystemParaMenu2.Ctrs_M = 64;

// 	UsermenuParameter.Bright = 64;
// 	UsermenuParameter.Contrast = 64;
// 	UsermenuParameter.Palette = 0;
// 	UsermenuParameter.CaptureFlag = 0;
// 	UsermenuParameter.FontColor = 1; //2是字符颜色动态 1是白色 3是背景色
// #if(Mirror_DisplayType == 1)
// 	if(Menu1ParaInit_flag == 0)//菜单一下初始化镜像不处理
// 	{
// 		UsermenuParameter.Mirror = 0;
// 	}
// #else
// 	UsermenuParameter.Mirror = 0;
// #endif

// #ifdef Network_Machine
// 	UsermenuParameter.AutoShutter = 0;
// 	UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF;
// #else
// 	UsermenuParameter.AutoShutter = 1;
// 	UsermenuParameter.ShutterMode == SHUTTER_MODE_TIMER;
// #endif

// #ifdef  SUPPORT_CONTINUOUS_ZOOM
// 	UsermenuParameter.Enlarge = 10;
// #else
// 	UsermenuParameter.Enlarge = 0;
// #endif

// 	UsermenuParameter.pause_ctl = 0;
// 	SelectionMenu.IsEnableEnligh = 0;

// 	UsermenuParameter.SunPT_SW = 1;
// 	UsermenuParameter.P_Time = 1;

// //	for(ts_u08 i=0;i<5;i++)
// //	{
// //		Areainit[i].Area_Lx =200 +i*50;
// //		Areainit[i].Area_Ly = 240;
// //		Areainit[i].Area_Rx = 232 +i*50 -AreaOffset;
// //		Areainit[i].Area_Ry = 272 -AreaOffset;
// //	}
// 	// Areainit[0].Area_Lx =IR_X_pixel/2-23;
// 	// Areainit[0].Area_Ly = IR_Y_pixel/2-23;
// 	// Areainit[0].Area_Rx = IR_X_pixel/2+9;
// 	// Areainit[0].Area_Ry = IR_Y_pixel/2+9;

// 	// Areainit[1].Area_Lx =IR_X_pixel/2-100-23;
// 	// Areainit[1].Area_Ly = IR_Y_pixel/2-23;
// 	// Areainit[1].Area_Rx = IR_X_pixel/2-100+9;
// 	// Areainit[1].Area_Ry = IR_Y_pixel/2+9;

// 	// Areainit[2].Area_Lx =IR_X_pixel/2-50-23;
// 	// Areainit[2].Area_Ly = IR_Y_pixel/2-23;
// 	// Areainit[2].Area_Rx = IR_X_pixel/2-50+9;
// 	// Areainit[2].Area_Ry = IR_Y_pixel/2+9;

// 	// Areainit[3].Area_Lx =IR_X_pixel/2+50-23;
// 	// Areainit[3].Area_Ly = IR_Y_pixel/2-23;
// 	// Areainit[3].Area_Rx = IR_X_pixel/2+50+9;
// 	// Areainit[3].Area_Ry = IR_Y_pixel/2+9;

// 	// Areainit[4].Area_Lx =IR_X_pixel/2+100-23;
// 	// Areainit[4].Area_Ly = IR_Y_pixel/2-23;
// 	// Areainit[4].Area_Rx = IR_X_pixel/2+100+9;
// 	// Areainit[4].Area_Ry = IR_Y_pixel/2+9;

// 	// SpotInit[0].spot_X_Pos = IR_X_pixel/2-1;
// 	// SpotInit[0].spot_Y_Pos = IR_Y_pixel/2-1;
// 	// //九宫格
// 	// SpotInit[1].spot_X_Pos = IR_X_pixel/3/2-1;
// 	// SpotInit[1].spot_Y_Pos =  IR_Y_pixel/3/2-1;
// 	// SpotInit[2].spot_X_Pos = (IR_X_pixel/3)*1+(IR_X_pixel/3/2-1);
// 	// SpotInit[2].spot_Y_Pos =  IR_Y_pixel/3/2-1;
// 	// SpotInit[3].spot_X_Pos = (IR_X_pixel/3)*2+(IR_X_pixel/3/2-1);
// 	// SpotInit[3].spot_Y_Pos =  IR_Y_pixel/3/2-1;

// 	// SpotInit[4].spot_X_Pos = IR_X_pixel/3/2-1;
// 	// SpotInit[4].spot_Y_Pos =  (IR_Y_pixel/3)*1+(IR_Y_pixel/3/2-1);
// 	// SpotInit[5].spot_X_Pos = (IR_X_pixel/3)*2+(IR_X_pixel/3/2-1);
// 	// SpotInit[5].spot_Y_Pos =  (IR_Y_pixel/3)*1+(IR_Y_pixel/3/2-1);

// 	// SpotInit[6].spot_X_Pos = IR_X_pixel/3/2-1;
// 	// SpotInit[6].spot_Y_Pos = (IR_Y_pixel/3)*2+(IR_Y_pixel/3/2-1);
// 	// SpotInit[7].spot_X_Pos = (IR_X_pixel/3)*1+(IR_X_pixel/3/2-1);
// 	// SpotInit[7].spot_Y_Pos =  (IR_Y_pixel/3)*2+(IR_Y_pixel/3/2-1);
// 	// SpotInit[8].spot_X_Pos = (IR_X_pixel/3)*2+(IR_X_pixel/3/2-1);
// 	// SpotInit[8].spot_Y_Pos =  (IR_Y_pixel/3)*2+(IR_Y_pixel/3/2-1);
// 	UsermenuParameter.ContinuousZoom =1;
// #ifdef SUPPORT_HuanYu_UART_PROTOCOL
// 	UsermenuParameter.HY_ShutterTime1 = 2; // 1
// 	UsermenuParameter.HY_ShutterTime2 = 6;//  3
// 	UsermenuParameter.HY_ShutterTime3 = 10;//  5
// #endif
// 	UsermenuParameter.CenterSpotSW = 0;

// 	SystemParaMenu2.DDE_Level = 0;
// 	SystemParaMenu2.SFilter = 0;
//  	CommonAreaParameterInit();
// 	userpara_reset();
}

void CommonAreaParameterInit(void)
{
	// SystemParaMenu2.Ctrs_M = 64;
	// SystemParaMenu2.Bri_M = 74;
	// SystemParaMenu2.DDE_Level = 0;
	// SystemParaMenu2.SFilter = 0;
}

void VotDetParInit(ts_u08 state)
{

	vision_log_debug(" enter: \n");
	if(state)
	{

		dector_type_t dector_type = get_powerup_dector_name_type();
		if(dector_type!=DECTOR_TYPE_RTD){

				SystemParaMenu2.NDAC_L = 0x10;
				SystemParaMenu2.NDAC_H = 128; //0xf0;

				SystemParaMenu2.GOF = 77;
				SystemParaMenu2.GAIN = 54;
				SystemParaMenu2.INT_C = 5;
				SystemParaMenu2.INT_T = 50;
				SystemParaMenu2.OOC_Par = 35;
				//光智探测器用户参数配置
				SystemParaMenu2.GAIN = 20;
				//*******************测温版参数设置*******************//
				SystemParaMenu2.GOF_Val[0] = 77;
				SystemParaMenu2.GOF_Val[1] = 77;
				SystemParaMenu2.GOF_Val[2] = 77;
				SystemParaMenu2.INT_C_Val[0] = 7;
				SystemParaMenu2.INT_C_Val[1] = 7;
				SystemParaMenu2.INT_C_Val[2] = 7;
				SystemParaMenu2.INT_T_Val[0] = 40;
				SystemParaMenu2.INT_T_Val[1] = 40;
				SystemParaMenu2.INT_T_Val[2] = 40;
				//**********************GOS***************//
				SystemParaMenu2.OOC_Par_Val[0] = 32;
				SystemParaMenu2.OOC_Par_Val[1] = 32;
				SystemParaMenu2.OOC_Par_Val[2] = 32;
			//*******************观测版参数设置*******************//
				SystemParaMenu2.OOC_Par = 40;
				SystemParaMenu2.GOF = 77;
				SystemParaMenu2.INT_C = 7;
				SystemParaMenu2.INT_T = 42;

				SystemParaMenu2.NDAC_L = 0x10;
				SystemParaMenu2.NDAC_H = 128; //0xf0;

				// VOTDetectorInit();

		#ifdef WLP
				SystemParaMenu2.GAIN = 36;
				SystemParaMenu2.GOF = 77;
				SystemParaMenu2.INT_C = 3;
				SystemParaMenu2.INT_T = 66;
				SystemParaMenu2.OOC_Par = 31;

				SystemParaMenu2.NDAC_L = 0x10;
				SystemParaMenu2.NDAC_H = 0xf0;
		#endif
			/*-------DETChange---------*/
		#if DET_Change_Para
				if(SystemParaMenu2.Usermenu_SW){ //测温
					SystemParaMenu2.DetMode = DET_MODE_OFF;
				}else{
					SystemParaMenu2.DetMode = DET_MODE_AUTO;
				}

				SystemParaMenu2.LowTemp.INT_C = 6;
				SystemParaMenu2.NormalTemp.INT_C = 3;
				SystemParaMenu2.HighTemp.INT_C = 5;
				SystemParaMenu2.offChange.INT_C = 6;

				SystemParaMenu2.LowTemp.INT_T = 34;
				SystemParaMenu2.NormalTemp.INT_T = 45;
				SystemParaMenu2.HighTemp.INT_T = 40;
				SystemParaMenu2.offChange.INT_T = 50;

				SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6;
				SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
				SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2;
				SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;


				vision_log_warn(" enter: dector_type_t %d \n", dector_type);
				vision_log_warn(" enter: SystemParaMenu2.OOC_Par %d \n", SystemParaMenu2.OOC_Par);
				if(dector_type==DECTOR_TYPE_VOT_WLP){
					SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6+2;
					SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
					SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2+10;
				}

		#endif

		}
		else {

				SystemParaMenu2.GAIN = 54;
				//*******************测温版参数设置*******************//
				SystemParaMenu2.GOF_Val[0] = 78;
				SystemParaMenu2.GOF_Val[1] = 70;
				SystemParaMenu2.GOF_Val[2] = 70;

				SystemParaMenu2.INT_C_Val[0] = 5;
				SystemParaMenu2.INT_T_Val[0] = 50;

				SystemParaMenu2.INT_C_Val[1] = 7;
				SystemParaMenu2.INT_C_Val[2] = 7;

				SystemParaMenu2.INT_T_Val[1] = 50;
				SystemParaMenu2.INT_T_Val[2] = 50;
				SystemParaMenu2.OOC_Par_Val[0] = 35;
				SystemParaMenu2.OOC_Par_Val[1] = 32;
				SystemParaMenu2.OOC_Par_Val[2] = 32;
				//*******************观测版参数设置*******************//
				//SystemParaMenu2.OOC_Par = 35;
				SystemParaMenu2.OOC_Par = 37;
				SystemParaMenu2.GOF = 78;
				SystemParaMenu2.INT_C = 3;
				SystemParaMenu2.INT_T = 65;

		#if DET_Change_Para
				// if(SystemParaMenu2.Usermenu_SW){ //测温
				// 	SystemParaMenu2.DetMode = DET_MODE_OFF;
				// }else{
				// 	SystemParaMenu2.DetMode = DET_MODE_AUTO;
				// }

				// SystemParaMenu2.LowTemp.INT_C = 6;
				// SystemParaMenu2.NormalTemp.INT_C = 3;
				// SystemParaMenu2.HighTemp.INT_C = 5;
				// SystemParaMenu2.offChange.INT_C = 6;

				// SystemParaMenu2.LowTemp.INT_T = 35;
				// SystemParaMenu2.NormalTemp.INT_T = 45;
				// SystemParaMenu2.HighTemp.INT_T = 40;
				// SystemParaMenu2.offChange.INT_T = 50;

			
				// SystemParaMenu2.LowTemp.OOC_Par = 35;
				// SystemParaMenu2.NormalTemp.OOC_Par = 40;
				// SystemParaMenu2.HighTemp.OOC_Par = 36;
				// SystemParaMenu2.offChange.OOC_Par = 36;
		#endif

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
		if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
		{
			SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[0];
			SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[0];
			SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[0];
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[0];
		}
		else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
		{
			SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[1];
			SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[1];
			SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[1];
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[1];
		}
		else  if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
		{
			SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[2];
			SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[2];
			SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[2];
			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[2];
		}
#ifdef CONFIG_SNR_PLANCK1280C
//如果是vot_planck1280c
	{
		SystemParaMenu2.GAIN1280 = 54;
		SystemParaMenu2.GOF1280 = 82;
		SystemParaMenu2.Cint1280 = 6;//3;
		SystemParaMenu2.Nint=1128;
		SystemParaMenu2.Sel_PJ=1;
		SystemParaMenu2.NTCC=10;
		SystemParaMenu2.Voccm=8;
		SystemParaMenu2.detConfig=1;
		SystemParaMenu2.Vfid=92;
	}
#endif
	}
	vision_log_debug(" leave: \n");
}

void DetParInit(ts_u08 state)
{

	vision_log_debug(" enter: \n");
	if(state==0){
		return;
	}
	ISP_set_det_calibrationing_flag(1);
	UpdatePromptInfoWithTimeMs("DET para init start",500,VOT_LOG_TYPE_MENU2);
	VotDetParInit(state);
	
	testpara_reset_det_parameter();

#if DET_Change_Para

		if(SystemParaMenu2.DetMode == DET_TEMP_MODE_AUTO){

			//立即切换
			ISP_set_det_switch_force_flag(2);
			ts_cpl_msleep(800);
		}
#endif


	UpdatePromptInfoWithTimeMs("DET para init completed",1000,VOT_LOG_TYPE_MENU2);

	ISP_set_det_calibrationing_flag(0);
	vision_log_debug(" leave: \n");
}


