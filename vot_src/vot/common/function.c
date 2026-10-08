/*
 * function.c
 *
 *  Created on: 2023年4月20日
 *      Author: yan
 */
#include "function.h"
#include <menu/testmenufunction.h>
#include <fontlib_vot.h>



extern SysCtrlInfo_s_t global_sys_ctrl_info;
extern AllMenuItem TestmenuItemTable[TestmenuTabNum];
extern ts_mdev_t shdl_osd_dev;

u8 ShutterState =0;  //OPR_ALARM
u8 RefreshData = 0;
#define CONST_FRACTION 0x4000

uint16_t BAK_SUM_AVG;
uint16_t DV_SUM_AVG;
int16_t delta_AVG;
int16_t delta_Pixel;

//uint16_t *ddr_DV_data = (uint16_t *)DDR_HBAK_ADDR;
uint16_t *ddr_DV_data = (uint16_t *)DDR_IR_Image_Bak;
uint16_t DV_value = {0};


u16 *ddr_NUC = (u16*)NUC_BASE_ADDR;
u16 NUC_value[] = {0};

//u16 *ddr_NUC_HDDR = (u16*)DDR_HBAK_ADDR;
u16 *ddr_NUC_IR_Image_Bak = (u16*)DDR_IR_Image_Bak;

uint8_t Calc_NUC;
uint16_t BD_L_avg;
uint16_t BD_H_avg;

void BAD_init(void)
{
	// u16 i,j;
	// for(j=0; j<IR_Y_pixel; j++)
	// {
	// 	for(i=0; i<IR_X_pixel; i++)
	// 	{
	// 		NUC_value[0]  = *(ddr_NUC+j*IR_X_pixel+i);
	// 		NUC_value[0] &= 0x7fff;
	// 		memcpy(ddr_NUC+j*IR_X_pixel+i,NUC_value,sizeof(NUC_value));
	// 	}
	// }
}

void BAD_Init_INC_DEC(void)
{
	// displayReminderInfo(Init);
	// BAD_init();
	// displayReminderInfo(Done);
}

void BAD_MarkOpr(u16 x,u16 y)
{
	// u16 NUC_value= 0;

	// NUC_value = *(ddr_NUC+y*IR_X_pixel+x);
	// if(NUC_value>>15)	NUC_value &= 0x7fff;
	// else			    NUC_value |= 0x8000;
	// memcpy(ddr_NUC+y*IR_X_pixel+x,&NUC_value,sizeof(NUC_value));
/*
	delay_1ms(10);
	DisplayBackground(X_START,Y_START+416,10,BACKGROUND_LAYER,SELECT_ITEM_COLOR);
	DisPlayNewStr(X_START,Y_START+416,"  Mark Down  ",4,FONT_COLOR);
	delay_1s(1);
	ClearSize16X16(X_START,10*16,Y_START+416);
*/
}

void BAD_CancelOpr(u16 x,u16 y)
{
/*
	u16 NUC_value= 0;

	NUC_value = *(ddr_NUC+y*IR_X_pixel+x);
	if(NUC_value>>15)
	{
		NUC_value &= 0x7fff;
		memcpy(ddr_NUC+y*IR_X_pixel+x,&NUC_value,sizeof(NUC_value));
	}
*/
	ISP_unset_bp_pos(x, y);
}

void BADMarkSemiAutoOpr(void)
{
	if((SystemParaMenu2.Auto_Semi)&&((SelectionMenu.MenuState == 0x0b)||BADCorrectFlag))
	{
//		BAD_MarkOpr(BadMes.Spot_x-1 ,BadMes.Spot_y-1);
		// BAD_MarkOpr(BadMes.Spot_x ,BadMes.Spot_y);
		vision_log_debug("BAD_MarkOpr(%d,%d)\n",BadMes.Spot_x,BadMes.Spot_y);
		global_sys_ctrl_info.m_BPC_cursor_pos_x = BadMes.Spot_x;
		global_sys_ctrl_info.m_BPC_cursor_pos_y = BadMes.Spot_y;
		BPC_Maual_mode_set_or_unset_bpc();
		// BPC_Maual_mode_set_or_unset_bpc_with_auto();
	}
}

void test_nuc(void)
{
	// u16 i,j;
	// u16 *K;

//	bsp_printf("test0 0x70:%x,0x74:%x,0x78:%x \n",read_u32(0xf81000B0),read_u32(0xf81000B4),read_u32(0xf81000B8));
//	APB3_REGW(0xf81000B0,0,0);
//	APB3_REGW(0xf81000B4,0,0);
//	APB3_REGW(0xf81000B8,0,0);
	// bsp_printf("0x70-74:%x,0x70:%x,0x72:%x \n",read_u32(0xf8100370),read_u16(0xf8100370),read_u16(0xf8100372));

	// Hyperram_Alarm1
//	BAD_MarkOpr(53,124);

/*
	for(j=419; j<421; j++)  //123  125
	{
		for(i=150; i<300; i++) //  50-55
		{
			BAD_MarkOpr(i,j);
		}
	}

	for(j=400; j<401; j++)
	{
		for(i=100; i<500; i++)
		{
			BAD_MarkOpr(i,j);
		}
	}

	for(j=200; j<400; j++)
	{
		for(i=100; i<500; i++)
		{
			BAD_MarkOpr(i,j);
		}
	}


	for(j=20; j<21; j++)
	{
		for(i=220; i<222; i++)
		{
			BAD_MarkOpr(i,j);
//			K = ddr_NUC + IR_X_pixel*j + i;
//			bsp_printf("test_bad Opr(%d,%d):%x\n",i,j,*K);
		}
	}
*/

}

void NUC_init(void)
{
// 	u16 i,j;
// 	for(j=0; j<IR_Y_pixel; j++)
// 	{
// 		for(i=0; i<IR_X_pixel; i++)
// 		{
// 			NUC_value[0] = *(ddr_NUC+j*IR_X_pixel+i);
// 			NUC_value[0] &= 0x8000;
// 			NUC_value[0] |= 0x4000;
// /*
// 			NUC_value[0]  = 0xA000;
// 			NUC_value[0]  += i ;
// */
// 			memcpy(ddr_NUC+j*IR_X_pixel+i,NUC_value,sizeof(NUC_value));
// 		}
// 	}
}

void Nuc_Init_INC_DEC(void)
{
	//displayReminderInfo(Init);
	// NUC_init_2();
	//displayReminderInfo(Done);
}

void Calc_NUC_INC(void)
{
	// Calc_NUC_INC_2(); //2023 06 25 test
/*
	DisplayBackground(X_START,Y_START+416,10,BACKGROUND_LAYER,SELECT_ITEM_COLOR);
	DisPlayNewStr(X_START,Y_START+416,"      计算中  ",4,FONT_COLOR);
	NUC_Calculate_2();
	DisplayBackground(X_START,Y_START+416,10,BACKGROUND_LAYER,SELECT_ITEM_COLOR);
	DisPlayNewStr(X_START,Y_START+416,"      计算完成  ",4,FONT_COLOR);
	delay_1s(1);
	ClearSize16X16(X_START,10*16,Y_START+416);
*/

//	DisplayMessage1("    计算中    ");
//	NUC_Calculate();		//NUC计算
//	IR_DeNoise_set(1);
//	DisplayMessage1("   计算完成   ");

//				NUC_Zone_Calc_BAD();	NUC之后以NUC数据为依据做换点判断，16*16的小区域均值做比较，图像的X像素点如果不是16的倍数会导致最右边区域出现异常判断
//	IR_DeNoise_set(1);

//	BAD_all_flash();
//	DisplayBadCount();

/******************************************************/
//	DisplayMessage1("    计算中    ");
//	mPrePara.BD_ID = 2;
//	IR_BD_id_set(mPrePara.BD_ID);
//	IR_Acc_bd_en();
//	delay_25ms(30);
//	BD_H_sum = FPGA_BD_sum;
//	BD_H_num = FPGA_BD_num;

//	delay_25ms(10);
//	 mPrePara.BD_ID = 0;
//	IR_BD_id_set(mPrePara.BD_ID);
//	NUC_Calculate();
//	DisplayMessage1("   计算完成   ");
}

void Calc_NUC_DEC(void)
{
	// Calc_NUC_DEC_2(); //2023 06 25 test


//	IR_DeNoise_set(3);
//	Ext_Shutter_pro();   //外校正

/******************************************************/
//	DisplayMessage1("    计算中    ");
//	mPrePara.BD_ID = 1;
//	IR_BD_id_set(mPrePara.BD_ID);
//	IR_Acc_bd_en();
//	delay_25ms(30);
//	BD_L_sum = FPGA_BD_sum;
//	BD_L_num = FPGA_BD_num;
//	DisplayMessage1("   计算完成   ");

}
void Get_DV(uint16_t addry,uint16_t addrx)
{
	// DV_value = *(ddr_DV_data + addry*IR_X_pixel + addrx);
}
void Write_ddr_NUC(uint16_t addry,uint16_t addrx,uint16_t data)		//
{
	// *(ddr_NUC + addry*IR_X_pixel + addrx) = data;
}
void Get_NUC(uint16_t addry,uint16_t addrx)
{
	// NUC_value[0] = *(ddr_NUC + addry*IR_X_pixel + addrx);
}

#if 0
void NUC_Calculate(void)		//NUC计算
{

//	uint16_t i;
//	uint16_t j;
	uint32_t i;
	uint16_t LBAK_val;
	uint16_t HBAK_val;
	uint16_t NUC;
	uint16_t * LBAK;
	uint16_t * HBAK;
	uint16_t * K;

	float avgL,avgH,avg;
	float tmp,pixel;

	uint32_t NUC_cal;
	uint16_t NUC_wr_value;

//	DDR_cannot_use();				//上电后或MSS持续占用DDR时，Fabric 不可使用DDR

	avgL = (BD_L_sum*1.0f)/BD_L_num;
	avgH = (BD_H_sum*1.0f)/BD_H_num;
	avg = (avgH-avgL);

	K = (uint16_t *) NUC_BASE_ADDR;
	LBAK = (uint16_t *) DDR_LBAK_ADDR;
	HBAK = (uint16_t *) DDR_HBAK_ADDR;
	for(i=0; i<(IR_X_pixel*IR_Y_pixel); i++){
		LBAK_val = *LBAK;
		HBAK_val = *HBAK;

		pixel = (HBAK_val-LBAK_val)>>2;
			if(pixel==0)
				tmp = 0;
			else
				tmp = avg/pixel;

		NUC_cal = tmp*(1<<13);
		NUC = *K;
		NUC_wr_value = (NUC & 0x8000) | (NUC_cal & 0x3fff);
			if((NUC_cal > 0x3fff) || (NUC_cal < 0x0fff))
				{
				NUC_wr_value &= 0x7fff;
				NUC_wr_value |=	0x8000;
				}
		*K = NUC_wr_value;

		K++;
		LBAK++;
		HBAK++;
		}
//	DDR_can_use();
}
#else
void NUC_Calculate(void)		//NUC计算
{
// // 	uint16_t i;
// // 	uint16_t j;

// // 	uint32_t NUC_cal;
// // 	uint16_t NUC_wr_value;

// // //	BAKG_ctl_off();		//本底温漂补偿关
// // //	NUC_ctl_off();		//增益校正关
// // //	BAD_ctl_off();		//盲元校正关
// // //	delay_25ms(2);

// // 	// SetFreeze_img(ON);		//图像冻结
// // 	delay_25ms(2);
// // //	Fabric_cannot_use_DDR();		//上电后或MSS持续占用DDR时，Fabric 不可使用DDR

// // 	delta_AVG = read_u16(IO_APB_SLAVE_0_INPUT_ScreenAD_Avg) - BAK_SUM_AVG;

// // 	for(j=0; j<IR_Y_pixel; j++){
// // 		for(i=0; i<IR_X_pixel; i++){
// // 			Get_DV(j,i);
// // 			delta_Pixel = DV_value - BAK_SUM_AVG;

// // 			//NUC_cal = (delta_AVG<<14)/delta_Pixel;	//4000H
// // 			NUC_cal = (delta_AVG<<13)/delta_Pixel;	    //2000H

// // 			//read NUC,bit15(bad) no change
// // 			Get_NUC(j,i);
// // 			NUC_wr_value = (NUC_value[0] & 0x8000) | (NUC_cal & 0x7fff);

// // 			//写NUC到DDR
// // 			Write_ddr_NUC(j,i,NUC_wr_value);
// // 	  }
// // 	}

// // //	Fabric_can_use_DDR();   //MSS释放DDR，Fabric 可使用DDR
// // 	delay_25ms(2);
// 	SetFreeze_img(OFF);		//图像冻结

// //	NUC_ctl_on();		//增益校正开
// //	BAD_ctl_on();		//盲元校正开
}

#endif

void Calc_NUC_INC_1(void)
{
	// NUC_Calculate();		//NUC计算
}
void Calc_NUC_DEC_1(void)
{
	// Ext_Shutter_pro();   //外校正
}
void NUC_Calculate_1(void)
{
// 	uint16_t i;
// 	uint16_t j;
// 	float _K =0.0;
// 	uint32_t NUC_cal;
// 	uint16_t NUC_wr_value;

// //	BAKG_ctl_off();		//本底温漂补偿关
// //	NUC_ctl_off();		//增益校正关
// //	BAD_ctl_off();		//盲元校正关
// //	delay_25ms(2);
// 	//SetIO_uatr(0);
// 	delay_1ms(15);

// 	BAK_SUM_AVG = 0x8000;
// 	SetFreeze_img(ON);		//图像冻结
// 	delay_25ms(2);
// //	Fabric_cannot_use_DDR();
// 	SetBaking_SW(ON);		//采集本底
// 	delay_25ms(2);
// //	memcpy(ddr_NUC_HDDR+j*IR_X_pixel+i,ddr_NUC_IR_Image_Bak,IR_X_pixel*IR_Y_pixel*2);
// 	delay_25ms(1);
// 	delta_AVG = read_u16(IO_APB_SLAVE_0_INPUT_ScreenAD_Avg) /4 - BAK_SUM_AVG;
// 	bsp_printf("delta_AVG is : 0x%x\n", delta_AVG);

// 	for(j=0; j<IR_Y_pixel; j++){
// 		for(i=0; i<IR_X_pixel; i++){
// 			Get_DV(j,i);
// 			NUC_cal =0;
// 			if(DV_value!=0x8000)
// 			{
// 				_K = (float)delta_AVG /(float)(DV_value - BAK_SUM_AVG);

// 				if(j==50 && i>500 && i<512)
// 				{
// //					bsp_printf("(%d,%d) _K is : %f,DV_value is: 0x%x\n", i,j,_K,DV_value);
// 					bsp_printf("-----(%d,%d) DV_value is: 0x%x\n", i,j,DV_value);
// 				}
// 				NUC_cal = _K *(1<<14);
// 				if(j==50 && i>500 && i<512)
// 				{
// 					bsp_printf("(%d,%d) NUC_cal is : 0x%x\n",i,j, NUC_cal);
// 				}
// /*
// //				if(_K>=0||_K<=0.2)
// //				if(_K>1.7||_K<0.4)
// 				if(_K>1.8||_K<0.2)
// 				{
// 					NUC_cal |= 1<<14;
// 				}
// 				else
// 				{
// 					NUC_cal =  _K *CONST_FRACTION;
// 				}
// */
// 			}
// 			else
// 			{
// 				NUC_cal |= 1<<14;
// 			}
// //			NUC_cal  = 0x813F;
// 			//read NUC,bit15(bad) no change
// 			Get_NUC(j,i);
// 			NUC_wr_value = (NUC_value[0] & 0x8000) | (NUC_cal & 0x7fff);
// 			//写NUC到DDR
// 			Write_ddr_NUC(j,i,NUC_wr_value);
// /*
// 			NUC_wr_value = j<<10 |i;
// 			Write_ddr_NUC(j,i,NUC_wr_value);
// */
// 	  }
// 	}
// /*
// 	for(j=0; j<IR_Y_pixel; j++)
// 	{
// 		for(i=0; i<IR_X_pixel; i++)
// 		{
// 			if(j==50 && i>500 && i<512)
// 			{
// 				Get_DV(j,i);
// 				bsp_printf("+++++(%d,%d) DV_value is: 0x%x\n", i,j,DV_value);
// 			}
// 		}
// 	}
// */
// 	delay_1ms(15);
// 	//SetIO_uatr(1);

// //	Fabric_can_use_DDR();   //MSS释放DDR，Fabric 可使用DDR
// 	delay_25ms(2);
// 	SetFreeze_img(OFF);		//图像冻结

//	NUC_ctl_on();		//增益校正开
//	BAD_ctl_on();		//盲元校正开

}

void NUC_init_2(void)
{
	// u16 i,j;
	// for(j=0; j<IR_Y_pixel; j++)
	// {
	// 	for(i=0; i<IR_X_pixel; i++)
	// 	{
	// 		NUC_value[0] = *(ddr_NUC+j*IR_X_pixel+i);
	// 		NUC_value[0] &= 0x8000;
	// 		NUC_value[0] |= 0x4000;
	// 		memcpy(ddr_NUC+j*IR_X_pixel+i,NUC_value,sizeof(NUC_value));
	// 	}
	// }
}

void Calc_NUC_INC_2(void)
{
#if 0
	displayReminderInfo(Calc);

	if(SystemParaMenu2.NUC_Flag)  //30,60度黑体
	{
		//IR_NUC_OPR(1);  //set mPrePara.BD_ID = 2;//地址待确认
		Ext_Shutter_pro();   //外校正
		delay_25ms(30);
		BD_H_avg = FPGA_Bak_AVG();
		delay_25ms(10);
		bsp_printf("BLACK_BD_H_avg_value is: 0x%x\n",BD_H_avg);
		//IR_NUC_OPR(0); //set mPrePara.BD_ID = 0;
		NUC_Calculate_2();
	}
	else
	{
		shutterOpr(OFF);
		delay_1ms(15);
		//IR_NUC_OPR(1);  //set mPrePara.BD_ID = 2;//地址待确认
		Ext_Shutter_pro();   //外校正
		delay_25ms(30);
		BD_H_avg = FPGA_Bak_AVG();
		delay_25ms(10);
		bsp_printf("SHUTTER_BD_H_avg_value is: 0x%x\n",BD_H_avg);

		//IR_NUC_OPR(0); //set mPrePara.BD_ID = 0;
		NUC_Calculate_2();
		shutterOpr(ON);
		delay_1ms(15);
	}

	displayReminderInfo(Done);

	systemPar.NoShutter =0;
#endif
}
void Calc_NUC_DEC_2(void)
{
#if 0
	displayReminderInfo(Calc);

	systemPar.NoShutter =1;
	//SetIO_uatr(0);
	delay_1ms(15);
	//IR_NUC_OPR(0);//set mPrePara.BD_ID = 1;  //地址待确认
 	Ext_Shutter_pro();   //外校正
	delay_25ms(30);
	BD_L_avg = FPGA_Bak_AVG();
//	bsp_printf("BD_L_avg_value is: 0x%x\n",BD_L_avg);
	delay_1ms(15);
    //SetIO_uatr(1);

    displayReminderInfo(Done);
#endif
}

// void NUC_Calculate_2(void)
// {
// //	uint16_t i;
// //	uint16_t j;
// 	uint32_t i;
// 	uint16_t LBAK_val;
// 	uint16_t HBAK_val;
// 	uint16_t NUC;
// 	uint16_t * LBAK;
// 	uint16_t * HBAK;
// 	uint16_t * K;

// 	float avgL,avgH,avg;
// 	float tmp,pixel;

// 	uint32_t NUC_cal;
// 	uint16_t NUC_wr_value;

// #ifdef  SUPPORT_CVBS
// 	avg = (float)(BD_H_avg-BD_L_avg)/2;
// #else
// 	avg = (float)(BD_H_avg-BD_L_avg)/2;
// //	avg = (float)(BD_H_avg-BD_L_avg)/4;//USB测试版本÷4
// #endif
// //	bsp_printf("(BD_L_avg:%d,BD_H_avg:%d) avg is: %f\n",BD_L_avg,BD_H_avg,avg);

// 	K = ddr_NUC;
// 	LBAK = (uint16_t *) DDR_LBAK_ADDR;
// 	HBAK = (uint16_t *) DDR_IR_Image_Bak;

// 	for(i=0; i<(IR_X_pixel*IR_Y_pixel); i++)
// 	{
// 		LBAK_val = *LBAK;
// 		HBAK_val = *HBAK;
// 		NUC = *K;
// 		NUC_cal = 0;
// 		if(NUC !=0x8000)
// 		{
// #ifdef  SUPPORT_CVBS
// 			pixel = (HBAK_val-LBAK_val)>>1;
// #else
// 			pixel = (HBAK_val-LBAK_val)>>1;
// 		//	pixel = (HBAK_val-LBAK_val)>>2;//USB测试版本>>2
// #endif
// 			if(pixel==0)		tmp = 0;
// 			else				tmp = avg/pixel;
// //			if(tmp>=0||tmp<=0.2)
// //			if(tmp>1.7||tmp<0.4)
// //			if(tmp>1.8||tmp<0.2)
// //			if(tmp>1.5||tmp<0.5)
// #ifdef WLP
// 			if(tmp>1.8||tmp<0.2)
// 			{
// #else
// 			if(tmp>1.3||tmp<0.8)  //241223修改
// 			{
// 				if(tmp>1.3)			{NUC_wr_value = 1.3*CONST_FRACTION;}
// 				else if(tmp<0.8)	{NUC_wr_value = 0.8*CONST_FRACTION;}
// #endif
// 	            NUC_wr_value &= 0x7fff;
// 	            NUC_wr_value |= 0x8000;
// //	            bsp_printf("+++ Bad:%d ;tmp :%f\n",i,tmp);
// 			}
// 			else
// 			{
// 				NUC_cal =  tmp *CONST_FRACTION;
// 				NUC_wr_value = (NUC & 0x8000) | (NUC_cal & 0x7fff);
// 			}
// 		}
// 		else
// 		{
// 			bsp_printf("*** other :%d\n",i);
// 			NUC_cal |= 1<<14;
// 			NUC_wr_value = (NUC & 0x8000) | (NUC_cal & 0x7fff);
// 		}

// 		*K = NUC_wr_value;
// 		K++;
// 		LBAK++;
// 		HBAK++;
// 	}
// }

void ReadAD(void)
{
// 	u16 ave = 0,max= 0,min = 0;

// 	// ave = ReadADAVERAGE();
// 	// max = ReadADMAX();
// 	// min = //ReadADMIN();
// 	if(systemPar.Ad_Auto_average!=ave)
// 	{
// 		systemPar.Ad_Auto_average = ave;
// 		systemPar.Ad_Auto_max =max;
// 		systemPar.Ad_Auto_min = min;
// 	//	check_max_min();
// 	//	bsp_printf("u16  test is : %04x\n", temp);
// /*
// 		bsp_printf("Ad_Auto_average is : 0x%04x\n", systemPar.Ad_Auto_average);
// 		bsp_printf("Ad_Auto_max     is : 0x%04x\n", systemPar.Ad_Auto_max);
// 		bsp_printf("Ad_Auto_min     is : 0x%04x\n", systemPar.Ad_Auto_min);
// */

// 		switch(UsermenuParameter.ImageMode)
// 		{
// 			case 0:
// //				//SetIO_uatr(0);
// //				delay_1ms(15);
// 				auto_widthLevel_cal(LEVEL | SPAN);
// //				delay_1ms(15);
// //				//SetIO_uatr(1);
// 				break;
// 			case 1:
// 				break;
// 		}

// 	}

}
//根据设定值计算level
void level_cal(void)
{
// 	unsigned int TempWIN_SETLevel;
// 	unsigned int TempWIN_SETWidth;
// 	unsigned int unint31;
// 	float mfloat1;
// 	float mfloat2;

// /*
// 	if(!b_LevelCal)	return;
// 	b_LevelCal=0;
// */

// 	//迭代计算level
// 	if(systemPar.Tempunit)
// 	{
// 		TempWIN_SETLevel=english_to_unit_temp(systemPar.winLevel);

// 		TempWIN_SETWidth=chang_english_For_Span(systemPar.winWidth);
// 	}
// 	else
// 	{
// 		TempWIN_SETLevel=systemPar.winLevel;
//    		TempWIN_SETWidth=systemPar.winWidth;
// 	}

// 	//LEVEL
// 	mfloat1=(float)TempWIN_SETLevel;
// 	mfloat1=mfloat1-600;

// 	// SPAM
// 	mfloat2=(float)TempWIN_SETWidth;  // 10*dT

// 	//LEVEL-SPAM/2
// 	mfloat1=mfloat1-mfloat2/2;
// 	mfloat1=mfloat1/10;

// 	mfloat1=mfloat1-systemPar.Temp_Base;    //dT
// 	mfloat1=mfloat1/systemPar.Temp_K;  //dAD

// 	mfloat1=mfloat1+systemPar.AD_Base;  //AD中值

// 	//把计算结果放如外部存储器
// 	unint31=(unsigned int) mfloat1;
// 	if(unint31>0x4000)
// 	{
// 	//写入xilinx
// 		systemPar.Temp_Level=unint31;
// //		//SetLevel(unint31);        //未添加
// 	}

}

void width_cal(void)
{
// 	float mfloat1;
// 	float mfloat2;
// 	unsigned int TempWIN_SETWidth;

// /*
// 	if(!b_SpanCal) return;
// 	b_SpanCal=0;

// 	system.IsoAnalysInfo.updateFlg = 1;
// */
// 	if(systemPar.Tempunit)
// 	{
// 		TempWIN_SETWidth=chang_english_For_Span(systemPar.winWidth);
// 	}
// 	else
// 	{
//    		TempWIN_SETWidth=systemPar.winWidth;
// 	}

// //迭代
// 	mfloat1=(float)TempWIN_SETWidth;  // 10*dT
// 	mfloat1=mfloat1/10;                 // dT

// 	mfloat1=mfloat1/systemPar.Temp_K;

// 	systemPar.AD_Width=(unsigned int)mfloat1;       // ADw

// 	mfloat2=1/mfloat1;
// 	mfloat2*=255;      //254;
// 	mfloat2*=4096;   //移位
// 	systemPar.AD_K=(unsigned int)mfloat2;   //Span

    //AD_K 写入 xilinx
//	//SetSpan(system.AD_K);   //未添加
}
//计算
void auto_cal_win(void)
{
// 	uint16_t Ad_Auto_max;
// 	uint16_t Ad_Auto_min;
// //	unsigned char unchar21;
// 	unsigned int  Temp_min_AD;
// 	unsigned int  Temp_max_AD;
// 	float mfloat2;
// 	float mfloat1;
// 	float AD_Base_cal;
// 	float Temp_Base_cal;
// 	float Temp_min_T;
// 	float Temp_max_T;

// 	Ad_Auto_max = systemPar.Ad_Auto_max;
// 	Ad_Auto_min = systemPar.Ad_Auto_min;

// 	if(Ad_Auto_max>Ad_Auto_min)
// 	{
// 		mfloat2=(float)Ad_Auto_max;	  //max
// 		mfloat1=(float)Ad_Auto_min;      //min

// 		mfloat2=(mfloat2+mfloat1)/2;
// 		AD_Base_cal=mfloat2;
// 		systemPar.in_tempAD=(unsigned int)mfloat2;
// //		temp_cal();            //暂时未添加
// 		mfloat2= (float)systemPar.out_temp;
// 		Temp_Base_cal=mfloat2/10;

// 		if((!LeveleSpanPar.cal_err)&&(LeveleSpanPar.cal_bigM))
// 		{
// 			Temp_min_AD=Ad_Auto_min;
// 			systemPar.in_tempAD=Ad_Auto_min;
// //			temp_cal();      //暂时未添加
// 			Temp_min_T=(float)systemPar.out_temp;
// 		}
// 		if((!LeveleSpanPar.cal_err)&&(LeveleSpanPar.cal_bigM))

// 		{
// 			Temp_max_AD=Ad_Auto_max;
// 			systemPar.in_tempAD=Ad_Auto_max;
// //			temp_cal();       //暂时未添加
// 			Temp_max_T=(float)systemPar.out_temp;
// 		}
// 		if((!LeveleSpanPar.cal_err)&&(LeveleSpanPar.cal_bigM))

// 		{
// 			systemPar.Temp_Base=Temp_Base_cal;  //T_Base  温度基
// 			systemPar.AD_Base=AD_Base_cal;  //AD_Base   AD基

// 			Temp_max_AD=Temp_max_AD-Temp_min_AD;
// 			Temp_max_T=Temp_max_T-Temp_min_T;
// 			mfloat1=(float)Temp_max_AD;
// 			systemPar.Temp_K=Temp_max_T/mfloat1;
// 			systemPar.Temp_K=systemPar.Temp_K/10;   //KT  斜率
// 		}
// 	}
// 	else
// 	{
// 		LeveleSpanPar.cal_err=1;
// 	};

	/*if((cal_err==1)||(!cal_bigM))
	{
		unchar21=(unsigned char)APT0TEMP[1];
		unchar21=unchar21/2+1;
		system.in_tempAD=APT0AD[unchar21];
		AD_Base_cal=(float)system.in_tempAD;
		temp_cal();
		mfloat2=(float)system.out_temp;
		Temp_Base_cal=mfloat2/10;

		if(cal_err)
		{
			system.in_tempAD=APT0AD[4];
			Temp_min_AD=system.in_tempAD;
			temp_cal();
			Temp_min_T=(float)system.out_temp;
		}
		if(cal_err)
		{
			unchar21=(unsigned char)APT0TEMP[1];
			unchar21-=1;
			system.in_tempAD=APT0AD[unchar21];
			Temp_max_AD=system.in_tempAD;
			temp_cal();
			Temp_max_T=(float)system.out_temp;
		}
		if(cal_err)
		{
			system.Temp_Base=Temp_Base_cal;  //T_Base  温度基
			system.AD_Base=AD_Base_cal;  //AD_Base   AD基

			Temp_max_AD=Temp_max_AD-Temp_min_AD;
			Temp_max_T=Temp_max_T-Temp_min_T;
			mfloat1=(float)Temp_max_AD;
			system.Temp_K=Temp_max_T/mfloat1;
			system.Temp_K=system.Temp_K/10;   //KT  斜率
		}
	}*/

}

//自动计算span
void auto_widthLevel_cal(uint8_t index) //auto_widthLevel_cal(LEVEL | SPAN);
{
	//uint8 i;
	// uint16_t tmp_MaxT,tmp_MinT;
// 	float tmp_Span;
// 	float Ad_Auto_average;
// 	float Ad_Auto_max;
// 	float Ad_Auto_min;
// 	float tmp32;

// 	Ad_Auto_average = (float)systemPar.Ad_Auto_average;
// 	Ad_Auto_max = (float)systemPar.Ad_Auto_max;
// 	Ad_Auto_min = (float)systemPar.Ad_Auto_min;

// //	bsp_printf("Ad_Auto_average :%f; max :%f; min :%f\n", Ad_Auto_average,Ad_Auto_max,Ad_Auto_min);
// 	tmp32 = (Ad_Auto_max + Ad_Auto_min) / 2;         //ADm

// 	//for(i = 3; i; i--)
// 	{
// 		Ad_Auto_average =(tmp32 + Ad_Auto_average) / 2;
// 	}

// 	if((uint16_t)tmp32 > (uint16_t)Ad_Auto_average)
// 	{
// 		tmp_Span = (Ad_Auto_average - Ad_Auto_min)*2;
// 		if(tmp_Span < 250)
// 		{
// 			tmp_Span = 250;
// 		}
// 		//tmp_Span *= 1.2;
// 		//Ad_Auto_min = Ad_Auto_min;
// 		//Ad_Auto_max = Ad_Auto_min + tmp_Span;
// 		tmp_Span *= 1.1;
// 		if(tmp_Span<3000)
// 		{
// 			//if(systemPar.WinTempRange == 0)
// 			{
// 				Ad_Auto_min = Ad_Auto_average - tmp_Span/2;
// 			}
// 		}
// 		tmp_Span *= 1.1;
// 		Ad_Auto_max = Ad_Auto_min + tmp_Span;
// 	}
// 	else
// 	{
// 		tmp_Span = (Ad_Auto_max - Ad_Auto_average)*2;
// //		bsp_printf("tmp_Span1 is : %f\n", tmp_Span);
// 		if(tmp_Span < 250)
// 		{
// 			tmp_Span = 250;
// 		}
// 		tmp_Span *= 1.1;
// //		bsp_printf("tmp_Span2 is : %d\n", (uint16_t)tmp_Span);
// 		if(tmp_Span<3000)
// 		{
// 			Ad_Auto_max = Ad_Auto_average + tmp_Span/2;
// 		}
// 		tmp_Span *= 1.1;
// //		bsp_printf("tmp_Span3 is : %d\n", (uint16_t)tmp_Span);
// 		tmp32 = Ad_Auto_max - tmp_Span;
// 		if(tmp32 > Ad_Auto_min)
// 		{
// 			Ad_Auto_min = tmp32;
// 		}
// //		bsp_printf("tmp32_1 is : %d\n", (uint16_t)tmp32);
// 	}

// 	if(tmp_Span < 640)//spank=1633 =0x661
// 	{
// 		tmp_Span = 640;
// 	}

// 	Ad_Auto_average =(Ad_Auto_min + Ad_Auto_max) / 2;

// 	systemPar.Ad_Auto_max = (uint16_t)Ad_Auto_max;
// 	systemPar.Ad_Auto_min = (uint16_t)Ad_Auto_min;

// 	if(index&SPAN)
// 	{
// 		systemPar.AD_Width = (uint16_t)tmp_Span;


// 		tmp32 = (255.0)*(4096.0);
// 		tmp32 = tmp32 / tmp_Span;
// //		bsp_printf("tmp32_2 is : %d\n", (uint16_t)tmp32);

// 		if((systemPar.Time_S>10 ||systemPar.Time_M>0||systemPar.Time_H>0)&& systemPar.AD_K!=0)
// 		{
// //			systemPar.AD_K = (u16)(tmp32/8) +systemPar.AD_K*(7/8);
// //			systemPar.AD_K = (u16)((tmp32 + 15*systemPar.AD_K)/16);
// 			tmp32 = (tmp32 + 15*systemPar.AD_K)/16;

// //			bsp_printf("Time_%d:%d:%d\n", systemPar.Time_H,systemPar.Time_M,systemPar.Time_S);

// 		}
// 		tmp32 *= 1+ (float)UsermenuParameter.Contrast /8;
// //		tmp32 = tmp32 + tmp32 * menu_para.Contrast /8;
// 		systemPar.AD_K = (u16)tmp32;
// 		//SetSpan(systemPar.AD_K);
// 	}

// 	if(index&LEVEL)
// 	{
// //		if(Ad_Auto_min<AD_data[0])
// //			Ad_Auto_min=AD_data[0]+256;

// 		if((systemPar.Time_S>10 ||systemPar.Time_M>0||systemPar.Time_H>0)&& systemPar.Temp_Level!=0)
// 		{
// //			systemPar.Temp_Level = (u16)(Ad_Auto_min/8) +systemPar.Temp_Level*(7/8);
// 			Ad_Auto_min =(Ad_Auto_min + 15*systemPar.Temp_Level)/16;
// 		}
// 		Ad_Auto_min = Ad_Auto_min * (160-(float)UsermenuParameter.Bright)/128;
// 		systemPar.Temp_Level=(uint16_t)Ad_Auto_min;
// /*
// //		bsp_printf("//SetLevel before is : %f\n", Ad_Auto_min);
// 		Ad_Auto_min = Ad_Auto_min * (160-menu_para.Bright)/128;
// //		bsp_printf("//SetLevel after is : %f\n", Ad_Auto_min);
// 		systemPar.Temp_Level=(uint16_t)Ad_Auto_min;
// */
// 		//SetLevel(systemPar.Temp_Level);
// 	}
}

// void manual_operation_widthLevel_cal(uint8_t index)  //手动
// {
// 	u16 AD_K = 0,Temp_Level = 0;
// 	float tmp32 = 0;

// 	if(systemPar.LevelSpanFlag)
// 	{
// 		auto_widthLevel_cal(LEVEL | SPAN);
// 		systemPar.LevelSpanFlag = 0;
// 	}
// 	if(index&SPAN)
// 	{
// 		tmp32 = (float)systemPar.AD_K;
// 		tmp32 *= 1+ (float)UsermenuParameter.Contrast /8;
// 		AD_K = (u16)tmp32;
// 		//SetSpan(AD_K);
// 	}
// 	if(index&LEVEL)
// 	{
// 		tmp32 = (float)systemPar.Temp_Level;
// 		tmp32 *= (160- (float)UsermenuParameter.Bright)/128;
// 		Temp_Level=(uint16_t)tmp32;
// 		//SetLevel(Temp_Level);
// 	}
// }

/****************************************** OOC OPR ******************************************************/

void OOC_init(void)
{
// 	uint16_t i,j;
// 	uint8_t * dout;
// 	// uint8_t dout_val;
// 	dout = (uint8_t *)OOC_BASE_ADDR;
// 	displayReminderInfo(Init);

// 	for(j=0; j<IR_Y_pixel; j++)
// 	{
// 		for(i=0; i<IR_X_pixel; i++)
// 		{
// 			*dout = 0x80 ;
// 			dout++;
// 		}
// 	}

// /*	for(j=0; j<IR_Y_pixel; j++)
// 	{
// 		for(i=0; i<IR_X_pixel; i++)
// 		{
// 			if(i%2 ==0)
// 			{
// 				*dout = j ;
// 			}
// 			else
// 			{
// 				*dout = i ;
// 			}
// 			dout++;
// 		}
// 	} */
// 	displayReminderInfo(Done);
}

void OOC_Cal(void)
{
// 	uint16_t K_val,BAK_val;
// 	uint32_t i,j;
// 	uint8_t  OCC_val;
// 	uint16_t * BAK;
// 	uint8_t  * OCC;
// 	u8 step=16;

// 	float BAK1,BAK2;
// 	float Bd_val;

// 	float avgBAK32;
// 	float avgBAK35;
// 	float AD;
// 	float avgBAK30;

// 	displayReminderInfo(Calc);

// 	//IR_NUC_OPR(1);//set mPrePara.BD_ID = 1;  //地址待确认

// 	SystemParaMenu2.OOC_Mode = 0;  //使用固定值
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffbf)|(SystemParaMenu2.OOC_Mode<<6)));

// 	SystemParaMenu2.OOC_Fix = 35;
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
// 	Ext_Shutter_pro();
// 	delay_25ms(30);
// //	avgBAK38 = (FPGA_BD_sum*1.0f)/FPGA_BD_num;
// 	// avgBAK35 = FPGA_Bak_AVG()*1.0f/2;
// //	bsp_printf("The avgBAK35 = %f",avgBAK35);

// 	SystemParaMenu2.OOC_Fix = 30;//32
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
// 	Ext_Shutter_pro();
// 	delay_25ms(30);
// //	avgBAK28 = (FPGA_BD_sum*1.0f)/FPGA_BD_num;
// 	// avgBAK30 = FPGA_Bak_AVG()*1.0f/2;
// //	bsp_printf("The avgBAK28 = %f",avgBAK30);

// 	AD = avgBAK30- avgBAK35;
// 	K_val = (uint16_t)(AD/5);
// 	bsp_printf("The K_val == %x",K_val);

// 	SystemParaMenu2.OOC_Fix = 32;//32
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix)));
// 	Ext_Shutter_pro();
// 	delay_25ms(30);
// //	avgBAK32 = (FPGA_BD_sum*1.0f)/FPGA_BD_num;
// 	// avgBAK32 = FPGA_Bak_AVG()*1.0f/2;
// //	bsp_printf("The avgBAK32 = %f",avgBAK32);

// 	BAK1 = avgBAK32 + (K_val/2);
// 	BAK2 = avgBAK32 - (K_val/2);
// //	bsp_printf("The BAK1 = %f  ;The BAK2 = %f  ",BAK1,BAK2);

// 	//			BAK1 = 8000 + 400;  //avgBAK32 + (K_val/2);
// 	//			BAK2 = 8000 - 400;   //avgBAK32 - (K_val/2);
// 	//
// 	SystemParaMenu2.OOC_Mode = 1; //使用计算值
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffbf)|(SystemParaMenu2.OOC_Mode<<6)));
// //刷新菜单显示
// 	if(SelectionMenu.SelectItem != NULL)
// 	{
// 		if((SelectionMenu.SelectItem->Parent == &TestmenuItemTable[0].pt[1])&&
// 				(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem))
// 		{
// 			DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1+80,TEST_Y_START+Y_GAP*1,2,BACKGROUND_LAYER,BackgroundColor);
// 			display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*1+80,Y_START+Y_GAP*1,SystemParaMenu2.OOC_Mode,3,CharacterColor);
// 		}
// 	}

// 	delay_25ms(10);
// 	Ext_Shutter_pro();
// 	delay_25ms(10);

// 	if(!systemPar.OOC_Way)//逐次逼近
// 	{
// 		for(j=0;j<32;j++) //16
// 		{
// 			OCC = (uint8_t *) OOC_BASE_ADDR;
// 			BAK = (uint16_t *) DDR_IR_Image_Bak;
// 	//		DDR_cannot_use();				//上电后或MSS持续占用DDR时，Fabric 不可使用DDR

// #ifdef Detector384
// 			for(i=0; i<IR_Y_pixel; i++)	 //一帧数据
// 			{
// 				for(k = 0; k < IR_X_pixel; k++)
// 				{
// #else
// 			for(i=0; i<(IR_X_pixel*IR_Y_pixel); i++)	 //一帧数据
// 			{
// #endif
// 				BAK_val = *BAK;
// 				Bd_val = (BAK_val*1.0f)/2;
// 				OCC_val = *OCC;
// 				OCC_val = (OCC_val>>2) & 0x3F;


// 			if(Bd_val > BAK1)
// 			{
// 		//		OCC_val += 1;
// 				OCC_val ++;
// 			}
// 			else if(Bd_val < BAK2)
// 			{
// 				//OCC_val -= 1;
// 				OCC_val --;
// 			}
// 			else
// 			{
// 				OCC_val = OCC_val+0;
// 			}

// 			OCC_val = (OCC_val& 0x3F)<<2;

// #ifdef Detector384
// 			if(j==31)
// 			{
// //
// 				if(k % 2 == 0)
// 				{
// 					OCC = OCC - (k/2);
// 					*OCC  = OCC_val;
// 					if(k != 0)
// 					{
// 						OCC = OCC + (k/2);
// 						*OCC = 0;
// 					}
// 				}
// 				else
// 				{
// 					*OCC = 0;
// 				}
// 			}
// 			else
// 			{
// 				*OCC = OCC_val;
// 			}
// #else
// 			*OCC = OCC_val;
// #endif

// 		/*	if((i>250*IR_X_pixel)&&(i<250*IR_X_pixel+200))
// 			{
// 			bsp_printf("The Count is %d ; The Bd_val = %f  ;The OCC_val = %d \n",j,Bd_val,OCC_val);
// 				//bsp_printf("The Bd_val = %f \n",Bd_val);
// 			} */
// 		//	if(j==31)	bsp_printf("%c",OCC_val>>2);

// 				BAK++;
// 			OCC++;
// #ifdef Detector384
// 			}
// #endif
// 			}
// 			delay_25ms(10);
// 			Ext_Shutter_pro();
// 			delay_25ms(10);
// 	/*		DDR_can_use();
// 			delay_25ms(10);
// 			IR_Acc_bd_en();
// 			delay_25ms(10); */
// 		}
// 	}
// 	else //二分法
// 	{
// 		for(step=16;step>0;step/=2)
// 		{
// 			OCC = (uint8_t *) OOC_BASE_ADDR;
// 			BAK = (uint16_t *) DDR_IR_Image_Bak;

// 			for(i=0; i<IR_X_pixel*IR_Y_pixel; i++)	 //一帧数据
// 			{
// 				BAK_val = *BAK;
// 				Bd_val = (BAK_val*1.0f)/2;
// 				OCC_val = *OCC;
// 				OCC_val = (OCC_val>>2) & 0x3F;
// 				if	   (Bd_val > BAK1) OCC_val+=step;
// 				else if(Bd_val < BAK2) OCC_val-=step;

// 				OCC_val = (OCC_val& 0x3F)<<2;
// 				*OCC = OCC_val;
// 				BAK++;
// 				OCC++;
// 		}
// 			delay_25ms(10);
// 			Ext_Shutter_pro();
// 			delay_25ms(10);

// 		}
// 	}
// 	displayReminderInfo(Done);
}

void OOCSetConstantValue (u8 val)
{
	// u8 data;
	// uint16_t i,j;
	// uint8_t * dout;
	// dout = (uint8_t *)OOC_BASE_ADDR;
	// data = val<<2;
	// for(j=0; j<IR_Y_pixel; j++)
	// {
	// 	for(i=0; i<IR_X_pixel; i++)
	// 	{
	// 		*dout = data;
	// 		dout++;
	// 	}
	// }
}

void OOCSysetemInit(void)
{
// 	if(SystemParaMenu2.OOC_Mode==0)
// 	{
// 		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffffC0)|(SystemParaMenu2.OOC_Fix));
// 	}
// 	else
// 	{
// //		OOC_init();
// 	}
}

void TimeInit(void)
{
	// memset(&systemPar,0,sizeof(systemPar));
	// if(UsermenuParameter.ImageMode)
	// {
	// 	systemPar.LevelSpanFlag =1;
	// }
}
void PowerOnTime(void)
{
// //	systemPar.Time_Count++;
// 	if(systemPar.Time_Count>=10)
// 	{
// 		systemPar.Time_Count =0;
// 		RefreshData = 1;
// 		systemPar.Time_S++;
// 		if(systemPar.Time_S>=60)
// 		{
// 			systemPar.Time_S =0;
// 			systemPar.Time_M++;
// 			UsermenuParameter.SystemTimeCounter++;
// 			if(systemPar.Time_M>=60)
// 			{
// 				systemPar.Time_M = 0;
// 				systemPar.Time_H++;
// 			}
// 		}
// #ifdef Network_Machine
// 		systemPar.shutter_flag = 3;
// #else

// 		if(systemPar.Time_H == 0)//刚上电,hour == 0
// 		{

// 		#ifdef WLP
// 			if(systemPar.Time_M <2)
// 			{
// 				systemPar.shutter_flag = 1;
// 			}
// 			else if(systemPar.Time_M <4)
// 			{
// 				systemPar.shutter_flag = 2;
// 			}
// 			else if(systemPar.Time_M <8)
// 			{
// 				systemPar.shutter_flag = 3;
// 			}
// 			else if(systemPar.Time_M < 11)
// 			{
// 				systemPar.shutter_flag = 4;
// 			}
// 			else if(systemPar.Time_M < 15)
// 			{
// 				systemPar.shutter_flag = 5;
// 			}
// 			else if(systemPar.Time_M < 20)
// 			{
// 				systemPar.shutter_flag = 6;
// 			}
// 			else
// 			{
// 				#if(No_Shutter == 1)
// 					systemPar.shutter_flag = 0;
// 					APB3_REGW(0xf8100074,0,((APB3_REGR(0xf8100074,0)&0xffffbfff)|(1<<14)));
// 				#else
// 					systemPar.shutter_flag = 7;
// 				#endif
// 			}

// 		#else

// 		#ifdef SUPPORT_VOT_DETECTOR
// 			if(systemPar.Time_M <3)
// 			{
// 				systemPar.shutter_flag = 1;
// 			}
// 			else if(systemPar.Time_M <10)
// 			{
// 				systemPar.shutter_flag = 2;
// 			}
// 			else
// 			{
// 				#if(No_Shutter == 1)
// 					systemPar.shutter_flag = 0;
// 					APB3_REGW(0xf8100074,0,((APB3_REGR(0xf8100074,0)&0xffffbfff)|(1<<14)));
// 				#else
// 					systemPar.shutter_flag = 3;
// 				#endif
// 			}
// 		#else
// 			if(systemPar.Time_M <10)
// 			{
// 				systemPar.shutter_flag = 2;
// 				//if(VotParameters.Test_Flag)		systemPar.shutter_flag = 4; //数据自动采集时间
// 				//else 		systemPar.shutter_flag = 2;
// 			}
// 			#ifndef	SUPPORT_HuanYu_UART_PROTOCOL
// 			else
// 			{
// 				#if(No_Shutter == 1)
// 					if(SystemParaMenu2.NoShutterFlag)  //读取存储的挡片数据
// 					{
// 						systemPar.shutter_flag = 0;
// 						APB3_REGW(0xf8100074,0,((APB3_REGR(0xf8100074,0)&0xffffbfff)|(1<<14)));
// 					}
// 					else   //可以打档操作
// 					{	systemPar.shutter_flag = 3;APB3_REGW(0xf8100074,0,(APB3_REGR(0xf8100074,0)&0xffffbfff));}
// 				#else
// 					systemPar.shutter_flag = 3;
// 				#endif
// 				//if(VotParameters.Test_Flag)		systemPar.shutter_flag = 4;//数据自动采集时间
// 				//else 		systemPar.shutter_flag = 3;
// 			}
// 			#else
// 			else if((systemPar.Time_M >=10)&&(systemPar.Time_M <60))	    	{systemPar.shutter_flag = 3;}
// 			#endif
// 		#endif

// 	#endif
// 		}
// #ifdef	SUPPORT_HuanYu_UART_PROTOCOL
// 		else {systemPar.shutter_flag = 4;}
// #endif
// #endif
// 	}
}

void X_Operation(void)
{
	// u8 *p_ooc;
	// u8 ooc_temp;
	// uint16_t i,j,temp;
	// uint16_t *bad;
	// bad = (uint16_t *)NUC_BASE_ADDR ;
	// for(j=0;j<IR_Y_pixel;j++)
	// {
	// 	for(i=0;i<IR_X_pixel/2;i++)
	// 	{
	// 		temp = *(bad+j*IR_X_pixel + i);
	// 		*(bad+j*IR_X_pixel + i) = *(bad+j*IR_X_pixel + IR_X_pixel-1-i);
	// 		*(bad+j*IR_X_pixel + IR_X_pixel-1-i)= temp;
	// 	}
	// }
	// p_ooc =(u8*)OOC_BASE_ADDR;
	// for(j=0;j<IR_Y_pixel;j++)
	// {
	// 	for(i=0;i<IR_X_pixel/2;i++)
	// 	{
	// 		ooc_temp = *(p_ooc+j*IR_X_pixel + i);
	// 		*(p_ooc+j*IR_X_pixel + i) = *(p_ooc+j*IR_X_pixel + IR_X_pixel-1-i);
	// 		*(p_ooc+j*IR_X_pixel + IR_X_pixel-1-i)= ooc_temp;
	// 	}
	// }
}
void Y_Operation(void)
{
	// u8 *p_ooc;
	// u8 ooc_temp;
	// uint16_t i,j,temp;
	// uint16_t *bad;
	// bad = (uint16_t *)NUC_BASE_ADDR;
	// for(j=0;j<IR_Y_pixel/2;j++)
	// {
	// 	for(i=0;i<IR_X_pixel;i++)
	// 	{
	// 		temp = *(bad+j*IR_X_pixel + i);
	// 		*(bad+j*IR_X_pixel + i) = *(bad+(IR_Y_pixel-1-j)*IR_X_pixel +i);
	// 		*(bad+(IR_Y_pixel-1-j)*IR_X_pixel+i)= temp;
	// 	}
	// }
	// p_ooc =(u8*)OOC_BASE_ADDR;
	// for(j=0;j<IR_Y_pixel/2;j++)
	// {
	// 	for(i=0;i<IR_X_pixel;i++)
	// 	{
	// 		ooc_temp = *(p_ooc+j*IR_X_pixel + i);
	// 		*(p_ooc+j*IR_X_pixel + i) = *(p_ooc+(IR_Y_pixel-1-j)*IR_X_pixel +i);
	// 		*(p_ooc+(IR_Y_pixel-1-j)*IR_X_pixel+i)= ooc_temp;
	// 	}
	// }
}

void MirrorBadSppotInit(void)
{
	// switch(UsermenuParameter.Mirror)
	// {
	// 	case 1://X
	// 		X_Operation();
	// 		break;
	// 	case 2://Y
	// 		Y_Operation();
	// 		break;
	// 	case 3://X_Y
	// 		X_Operation();
	// 		Y_Operation();
	// 		break;
	// }
}


void MirrorBadSppot(uint8_t pos)
{
//	SetFreeze_img(ON);
	// switch(UsermenuParameter.Mirror)
	// {
	// 	case 0://OFF
	// 			if(pos)
	// 			{
	// 					X_Operation();
	// 					Y_Operation();
	// 			}
	// 			else
	// 			{
	// 					X_Operation();
	// 			}
	// 		break;
	// 	case 1://X
	// 			if(pos)
	// 			{
	// 					X_Operation();
	// 			}
	// 			else
	// 			{
	// 					Y_Operation();
	// 					X_Operation();
	// 			}
	// 		break;
	// 	case 2://Y
	// 			if(pos)
	// 			{
	// 					X_Operation();
	// 					Y_Operation();
	// 			}
	// 			else
	// 			{
	// 					X_Operation();
	// 			}
	// 		break;
	// 	case 3 ://X_Y
	// 			if(pos)
	// 			{
	// 					X_Operation();
	// 			}
	// 			else
	// 			{
	// 					X_Operation();
	// 					Y_Operation();
	// 			}
	// 		break;
	// }
	//	SetFreeze_img(OFF);
	//	Int_Shutter_pro();
	//	Write_Flash_NUC();
}
void MirrorBadRecover(u8 BeforeVal)
{
	 	// if(BeforeVal==1)  //X镜像
		// {
	 	// 	X_Operation();
		// }
		// else if(BeforeVal==2) //Y镜像
		// {
		// 	Y_Operation();
		// }
		// else if(BeforeVal==3) //XY镜像
		// {
		// 	X_Operation();Y_Operation();
		// }

/*	if(BeforeVal==0) //不镜像
	{
		if(NowVal==1)	{X_Operation();}
		else if(NowVal==2)	{Y_Operation();}
		else if(NowVal==3) 	{X_Operation();Y_Operation();}
	}
	else 	if(BeforeVal==1)  //X镜像
	{
		if(NowVal==0)	{X_Operation();}
		else if(NowVal==2)	{X_Operation();Y_Operation();}
		else if(NowVal==3) 	{Y_Operation();}
	}
	else if(BeforeVal==2) //Y镜像
	{
		if(NowVal==0)	{Y_Operation();}
		else if(NowVal==1)	{Y_Operation();X_Operation();}
		else if(NowVal==3) 	{X_Operation();}
	}
	else if(BeforeVal==3) //XY镜像
	{
		if(NowVal==0)	{X_Operation();Y_Operation();}
		else if(NowVal==1)	{Y_Operation();}
		else if(NowVal==2) 	{X_Operation();}
	}*/
}

void FPGAShutterOpr(void)   //OPR_ALARM
{

// 	uint32_t x,y;
// 	x=300;
// 	y= 240;

// 	if(read_u8(IO_APB_SLAVE_0_INPUT_COF)&0x80)
// 	{
// //		Int_Shutter_pro();
// //		APB3_REGW_Shutter_SW(OFF);
// 		ShutterState =1;
// 		DisPlayWarnIcon((u8*)Aralm_Icon,x,y,4,6,0);
// //		DisplayBackground(x,y,15,BACKGROUND_LAYER,0);
// //		DisPlayNewStr(x,y,"  Alarm   ",4,FONT_COLOR);
// 	}
// 	else
// 	{
// 		if(ShutterState)
// 		{
// 			ShutterState =0;
// 			DisPlayWarnIcon((u8*)Aralm_Icon,x,y,BACKGROUND_LAYER,6,1);
// //			ClearSize16X16(x,18*16,y);
// 		}

// 	}
}

void SensorOpr(void)
{

// #if 0  //传输Sensor数据
// 	float temp = 0.0;
// 	u16 Setval = 0;
// 	temp = READ_TMP112_Temp();
// 	temp *=10;
// //	temp = -108;
// 	Setval = abs((int)temp);
// 	if(temp<0)
// 	{
// 		Setval |= 1<<15;
// //		if(systemPar.Time_Count %50== 0) bsp_printf("Setval: %x\n",Setval);
// 	}
// 	APB3_REGW_SENSOR_SET(Setval);

// #else  //传输DET温度
// 	float temp = 0.0;
// 	u16 Setval = 0;
// 	float K,TEMP;

// 	DispSysMenu2.Det_PAR = ReadDETAD();
// 	#ifdef SUPPORT_VOT_DETECTOR
// 		if(DispSysMenu2.Det_PAR < 7416) //正
// 		{
// 			K = 33;
// 			TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
// 		}
// 		else  //负数
// 		{
// 			K = 33;
// 			TEMP = (DispSysMenu2.Det_PAR - 7416)/K;
// 		//	TEMP_16 = (~TEMP_16) + 1;    //补码表示
// 		}
// 	#else
// 		if(DispSysMenu2.Det_PAR < 7543) //正
// 		{
// 			K = 33.8;
// 			TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
// 		}
// 		else  //负数
// 		{
// 			K = 33.8;
// 			TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
// 		//	TEMP_16 = (~TEMP_16) + 1;    //补码表示
// 		}
// 	#endif

// 		temp = (float)(TEMP *10);
// 	//	temp = -108;
// 		Setval = abs((int)temp);
// #ifdef SUPPORT_VOT_DETECTOR
// 		if(DispSysMenu2.Det_PAR >= 7416)
// #else
// 		if(DispSysMenu2.Det_PAR >= 7543)
// #endif
// 		{
// 			Setval |= 1<<15;
// 	//		if(systemPar.Time_Count %50== 0) bsp_printf("Setval: %x\n",Setval);
// 		}else		{Setval &=0x7FFF;}
// 		APB3_REGW_DETTemp(Setval);
// #ifdef No_Shutter
// 		APB3_REGW_SENSOR_SET(Setval);
// #endif
// #endif

// //	if(systemPar.Time_Count %50== 0) bsp_printf("READ_TMP112: %x\n",read_u32(IO_APB_SLAVE_0_INPUT_Hyperram_Opr));

// //	if(read_u16(Hypprram_Alarm2)&0x8000)  //bak_start
// //	{
// //		Int_Shutter_pro();
// 		//write_u16(read_u16(Hypprram_Alarm2)|0x8000,Hypprram_Alarm2); //bak_end = 1;
// //	}
// //	else
// //	{
// 		//write_u16(read_u16(Hypprram_Alarm2)&0x7FFF,Hypprram_Alarm2); //bak_end = 0;
// //	}
}



static u32 PixelsNumbers = 0;

/* 校正传入坐标的像素 一处调用 void PC_CorrectBadPixels(void) */
void BadCorrectOpration(u16 x,u16 y)
{
	ISP_set_bp_pos(x, y);
}
/* 刷新记录坏点信息的 串口指令一处调用 开启PC坏点标定模式 */

u8 PC_MarkBadPixels(u16 Pixels_X,u16 Pixels_Y)
{
	if((Pixels_X == 0xFFFF)&&(Pixels_Y == 0xFFFF))
	{
		vision_log_debug("BadCorrectOpration: Pixels_X = 0xFFFF,Pixels_Y = 0xFFFF\r\n");
		return 0;
	}
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV) {
		Pixels_X = Pixels_X % max_width;
		Pixels_Y = Pixels_Y % max_heigth;
	}
	vision_log_debug("XTempPos %d,YTempPos %d \n",Pixels_X,Pixels_Y);
	BadCorrectOpration(Pixels_X,Pixels_Y);//只有这里用
	PCMoveCursorBPC(Pixels_X,Pixels_Y);
	return 0;
}


/* 刷新记录坏点信息的 串口指令一处调用 开启/取消坏点校正 */
void PC_CorrectBadPixels(void)
{
	// u32 *BadPositionPoint;
// 	u16 XTempPos,YTempPos = 0;
// 	u32 *BadPositionP;

// 	BadPositionP = (ts_u32 *)m_high_temp_ad_buff;
// 	for(u32 a = 0;a<PixelsNumbers;a++)
// 	{
// 		XTempPos = (*BadPositionP)>>16;
// 		YTempPos = (*BadPositionP)&0xFFFF;
// 		if((XTempPos == 0xFFFF)&&(YTempPos == 0xFFFF))
// 		{
// 			BadPositionP = BadPositionP+1;
// 			continue;
// 		}
// 		else
// 		{
// 			BadCorrectOpration(XTempPos,YTempPos);//只有这里用

// //			DisPlaySpot(XTempPos,YTempPos,3,0,1);//暂时无光标
// 			PCMoveCursorBPC(XTempPos,YTempPos);
// 		}
		
// 		BadPositionP = BadPositionP+1;
// 	}
}


/* 串口指令一处调用 取消上一次标记的一个坏点 */
u8 PC_CancalOneMarkBadPixels(u16 Pixels_X,u16 Pixels_Y)
{
	// u32 *BadPositionPoint;
	u16 XTempPos,YTempPos = 0;
	// u32 *BadPositionP;

	XTempPos = Pixels_X;
	YTempPos = Pixels_Y;
	if((XTempPos == 0xFFFF)&&(YTempPos == 0xFFFF))
	{
		vision_log_debug("XTempPos = 0xFFFF,YTempPos = 0xFFFF\r\n");
		return 0;
	}
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV) {
		XTempPos = XTempPos % max_width;
		YTempPos = YTempPos % max_heigth;
	}
	vision_log_debug("XTempPos %d,YTempPos %d \n",XTempPos,YTempPos);
	BAD_CancelOpr(XTempPos,YTempPos);
	PCMoveCursorBPC(XTempPos,YTempPos);

	return 0;
}


void PC_CancalAllMarkBadPixels(void)
{
	// u32 *BadPositionPoint;
// 	u16 XTempPos,YTempPos = 0;
// 	u32 *BadPositionP;

// 	BadPositionP = (ts_u32 *)m_high_temp_ad_buff;
// 	for(u32 a = 0;a<PixelsNumbers;a++)
// 	{
// 		XTempPos = (*BadPositionP)>>16;
// 		YTempPos = (*BadPositionP)&0xFFFF;
// 		if((XTempPos == 0xFFFF)&&(YTempPos == 0xFFFF))
// 		{
// 			BadPositionP = BadPositionP+1;
// 			continue;
// 		}

// 		vision_log_debug("XTempPos %d,YTempPos %d \n",XTempPos,YTempPos);
// 		BAD_CancelOpr(XTempPos,YTempPos);
// //		DisPlaySpot(XTempPos,YTempPos,3,0,1);
// 		PCMoveCursorBPC(XTempPos,YTempPos);
// 		*BadPositionP = 0xFFFFFFFF;
// 		BadPositionP = BadPositionP+1;
// 	}
// 	PixelsNumbers = 0;
}

/*
PC 上位机UVC 校正坏点
*/

void Enter_PC_BPCMaualMode(void) 
{
	PixelsNumbers = 0;
	// global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_BPC;
	// GetAndDisplayBPC_Maual();
	EnterOrExitBPCMaualMode(0);
	
}
void EXIT_PC_BPCMaualMode(void) 
{
	PixelsNumbers = 0;
	// Exit_BPC_Maual_mode();
	// global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
	EnterOrExitBPCMaualMode(1);
}

void PCMoveCursorBPC(ts_u16 pos_x,ts_u16 pos_y) 
{
	//移动光标
	cursor_set_position(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, pos_x, pos_y);

	ts_char info_tip[64];
	if(SystemParaMenu2.Auto_Semi ==1){
		snprintf(info_tip,64 ,"Min[%d,%d]", pos_x, pos_y);}
	else if(SystemParaMenu2.Auto_Semi ==2){
		snprintf(info_tip,64 ,"Max[%d,%d]", pos_x, pos_y);}
	else	snprintf(info_tip,64 ,"[%d,%d]", pos_x, pos_y);	
	
	ts_u16  dst_pos_x, dst_pos_y;
	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	ts_u16 half_width = (max_width)/2;
	ts_u16 half_height = (max_heigth)/2;

	if(pos_x > half_width) {
		dst_pos_x = 20;
	}
	else{
		ts_u32 pixels_width = fontl_get_fit_pixel_width(info_tip);
		dst_pos_x = max_width - pixels_width - 60;
	}

	if(pos_y > half_height) {
		dst_pos_y = 30;
	}
	else{
		dst_pos_y = max_heigth - 30;
	}
	/* 半自动校点时，提示词显示位置坐标点 */
	global_sys_ctrl_info.m_prompt_cursor_pos_x = dst_pos_x;
	global_sys_ctrl_info.m_prompt_cursor_pos_y = dst_pos_y;
	UpdateSpecialInfo(info_tip, dst_pos_x, dst_pos_y, 1);
}

ts_s32 ts_lib_set_pixel(ts_nodebuf_s_t *pst_osd, ts_u16 x, ts_u16 y, ts_u08 color) {
   
	
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		if (pst_osd == NULL || x >= 1280 || y >= 1024) {
			return -TS_EINVAL;
		}
		if (color > 0x03) {
			return -TS_EINVAL;
		}
		//tsp_log_notice("pos x y: %d %d\n", x, y);
		ts_u32 u32_row_bytes = 1280 >> 2;
		ts_u32 byte_offset = y * u32_row_bytes + (x >> 2);  
		ts_u08 bit_offset = (x & 0x03) << 1;
		ts_u08 *pu08_pixel = (ts_u08 *)pst_osd->u32_virt_addr + byte_offset;
		*pu08_pixel &= ~(0x03 << bit_offset);
		*pu08_pixel |= (color & 0x03) << bit_offset;
	}
	#else
	{
		if (pst_osd == NULL || x >= 640 || y >= 512) {
			return -TS_EINVAL;
		}
		if (color > 0x03) {
			return -TS_EINVAL;
		}
		//tsp_log_notice("pos x y: %d %d\n", x, y);
		ts_u32 u32_row_bytes = 640 >> 2;
		ts_u32 byte_offset = y * u32_row_bytes + (x >> 2);  
		ts_u08 bit_offset = (x & 0x03) << 1;
		ts_u08 *pu08_pixel = (ts_u08 *)pst_osd->u32_virt_addr + byte_offset;
		*pu08_pixel &= ~(0x03 << bit_offset);
		*pu08_pixel |= (color & 0x03) << bit_offset;
	}
	#endif
    return 0;
}

//全屏填充0x55 透明色
ts_s32 ts_lib_fill_screen_black(ts_nodebuf_s_t *pst_osd, ts_u08 color) {
    if (pst_osd == NULL) {
        return -TS_EINVAL; 
    }
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		ts_u08 fill_byte = color;
		ts_u32 u32_row_bytes = 1280 >> 2;
		ts_u32 total_bytes = u32_row_bytes * 1024;
		ts_cpl_memset((ts_void *)pst_osd->u32_virt_addr, fill_byte, total_bytes);
	}
	#else
	{
		ts_u08 fill_byte = color;
		ts_u32 u32_row_bytes = 640 >> 2;
		ts_u32 total_bytes = u32_row_bytes * 512;
		ts_cpl_memset((ts_void *)pst_osd->u32_virt_addr, fill_byte, total_bytes);
	}
	#endif
    return 0;
}

ts_s32 ts_lib_draw_corner_rect(ts_nodebuf_s_t *pst_osd, ts_u16 x, ts_u16 y, ts_u16 right, ts_u16 bottom, ts_u08 line_thickness, ts_u08 corner_length, ts_u08 color) {
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		if (pst_osd == NULL || x >= 1280 || y >= 1024) return -TS_EINVAL;
		if (right >= 1280 || bottom >= 1024) return -TS_EINVAL;
	}
	#else
	{
		if (pst_osd == NULL || x >= 640 || y >= 512) return -TS_EINVAL;
		if (right >= 640 || bottom >= 512) return -TS_EINVAL;
	}
	#endif
    if (line_thickness == 0 || corner_length == 0 || color > 0x03) return -TS_EINVAL;

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_set_pixel(pst_osd, x + dx, y + dy, color);
            ts_lib_set_pixel(pst_osd, x + dy, y + dx, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_set_pixel(pst_osd, right - dx - 1, y + dy, color);
            ts_lib_set_pixel(pst_osd, right - dy - 1, y + dx, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_set_pixel(pst_osd, x + dx, bottom - dy - 1, color);
            ts_lib_set_pixel(pst_osd, x + dy, bottom - dx - 1, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_set_pixel(pst_osd, right - dx - 1, bottom - dy - 1, color);
            ts_lib_set_pixel(pst_osd, right - dy - 1, bottom - dx - 1, color);
        }
    }

    return 0;
}

/** @note 自适应绘制像素点亮灭 */
static ts_s32 ts_lib_selfAdaptive_setPixel(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u16 x, ts_u16 y, ts_u08 color) {
    if (pst_osd == NULL || x >= s32_img_w || y >= s32_img_h) {
        return -TS_EINVAL;
    }
    if (color > 0x03) {
        return -TS_EINVAL;
    }
    //tsp_log_notice("pos x y: %d %d\n", x, y);
    ts_u32 u32_row_bytes = s32_img_w >> 2;
    ts_u32 byte_offset = y * u32_row_bytes + (x >> 2);  
    ts_u08 bit_offset = (x & 0x03) << 1;
    ts_u08 *pu08_pixel = (ts_u08 *)pst_osd->u32_virt_addr + byte_offset;
    *pu08_pixel &= ~(0x03 << bit_offset);
    *pu08_pixel |= (color & 0x03) << bit_offset;
    return 0;
}
/** @note 全屏填充0x55 透明色 */
static ts_s32 ts_lib_selfAdaptive_fillScreen_black(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u08 color) {
    if (pst_osd == NULL) {
        return -TS_EINVAL; 
    }
    ts_u08 fill_byte = color;
    ts_u32 u32_row_bytes = s32_img_w >> 2;
    ts_u32 total_bytes = u32_row_bytes * s32_img_h;
    ts_cpl_memset((ts_void *)pst_osd->u32_virt_addr, fill_byte, total_bytes);

    return 0;
}
/** @note 根据宽高，自适应绘制圆角矩形 */
static ts_s32 ts_lib_selfAdaptive_draw_corner_rect(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u16 x, ts_u16 y, ts_u16 right, ts_u16 bottom, 
	ts_u08 line_thickness, ts_u08 corner_length, ts_u08 color) 
{
    if (pst_osd == NULL || x >= s32_img_w || y >= s32_img_h) return -TS_EINVAL;
    if (right >= s32_img_w || bottom >= s32_img_h) return -TS_EINVAL;
    if (line_thickness == 0 || corner_length == 0 || color > 0x03) return -TS_EINVAL;

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, x + dx, y + dy, color);
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, x + dy, y + dx, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, right - dx - 1, y + dy, color);
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, right - dy - 1, y + dx, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, x + dx, bottom - dy - 1, color);
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, x + dy, bottom - dx - 1, color);
        }
    }

    for (ts_u08 dy = 0; dy < line_thickness; dy++) {
        for (ts_u16 dx = 0; dx < corner_length; dx++) {
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, right - dx - 1, bottom - dy - 1, color);
            ts_lib_selfAdaptive_setPixel(pst_osd, s32_img_w, s32_img_h, right - dy - 1, bottom - dx - 1, color);
        }
    }

    return 0;
}
/** @note 指定区域填充0x55 透明色，目前测温功能在使用 */
ts_s08 ts_lib_selfAdaptive_AreasScreen_black(ts_nodebuf_s_t *pst_osd, ts_u16 Area_Lx, ts_u16 Area_Ly, ts_u16 Area_Rx, ts_u16 Area_Ry)
{
	ts_osd_attr_s_t st_attr = {0};

    if (pst_osd == NULL) {
        return -TS_EINVAL; 
    }

	ts_tdk_osd_get_attr(shdl_osd_dev, pst_osd, &st_attr);
	if (Area_Lx >= Area_Rx || Area_Ly >= Area_Ry || Area_Rx > st_attr.u16_width || Area_Ry > st_attr.u16_height) {
		return -TS_EINVAL;
	}
	
    //每个像素2bit 为 b 01,所以为1，sast_mnu_rgbs对应 /* 黑色， 透明度 0, 即完全不透明 */
	// 计算每行字节数（右移2位 = 除以4，每像素2bit）
	ts_u32 u32_width_bytes = st_attr.u16_width >> 2;

	ts_u16 byte_start = Area_Lx >> 2;
	ts_u16 byte_end = (Area_Rx + 3) >> 2;

    if (byte_end > u32_width_bytes) {
        byte_end = u32_width_bytes;
    }
	ts_u16 fill_bytes = byte_end - byte_start;
    if (fill_bytes == 0) {
        return 0;
    }

	/* 获取缓冲区起始地址 */
	ts_u08 * pu08_base_addr   = (ts_u08 *)pst_osd->u32_virt_addr;
    ts_u32 row_offset = (ts_u32)Area_Ly * u32_width_bytes;
    ts_u16 height = Area_Ry - Area_Ly;

    for (ts_u16 y = 0; y < height; y++) {
        ts_u08 *pu08_row_ptr = pu08_base_addr + row_offset + byte_start;
        ts_cpl_memset(pu08_row_ptr, 0x55, fill_bytes);
        row_offset += u32_width_bytes;
    }
	return 0;
}
/* 半自动校点所需画点函数 */
void semiAutoMode_clearScreen(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h)
{
	ts_lib_selfAdaptive_fillScreen_black(pst_osd, s32_img_w, s32_img_h, 0x55);
}
void semiAutoMode_DrawRect(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u16 top_left_x, ts_u16 top_left_y, ts_u16 bottom_right_x, ts_u16 bottom_right_y) 
{
	ts_lib_selfAdaptive_draw_corner_rect(pst_osd, s32_img_w, s32_img_h, top_left_x, top_left_y, bottom_right_x, bottom_right_y, 2, 8, 0x00);
	ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}

