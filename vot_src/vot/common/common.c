/*
 * common.c
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

// #include "common.h"
#include "../myheader.h"

#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <menu/ispfunction.h>
#include <uapi/i2c_v100.h>

u8 check1 = 0B00001000;
u8 check2 = 0B00000100;
u8 check3 = 0B00000010;
u8 check4 = 0B00000001;

u8 SlaveAddressByte1;
u8 SlaveAddressByte2;
u8 SlaveAddressByte3;
u8 SlaveAddressByte4;

const uint32_t QUAN_BIT = 16;

u8 AutoTempFlag = 1;
u8 AutoGOF_Flag =1;

struct SystemPara      systemPar;
struct LeveleSpanPara  LeveleSpanPar;
LineASCIIBufStruct     LineASCIIBuf;
CharDispStruct Display;
DetPara detPara;

extern ts_u32 det_gos_switch_force_last;
extern SysCtrlInfo_s_t global_sys_ctrl_info;

void delay_25ms (uint32_t time_25ms)
{
	// bsp_uDelay(25*1000*time_25ms);
	ts_cpl_msleep(time_25ms*25);
}

void delay_1ms (uint32_t time_1ms)
{
	// bsp_uDelay(1000*time_1ms);
	ts_cpl_msleep(time_1ms);
}
void delay_1s (uint32_t time_1s)
{
	// bsp_uDelay(time_1s*1000000);
	ts_cpl_msleep(time_1s*1000);
}

uint16_t english_to_unit_temp(uint16_t tmp16)
{
	float	mfloat1;
	mfloat1 = (float)(tmp16);
	mfloat1 -= 1080;		//920
	mfloat1 /= 1.8;
	mfloat1 += 600.5;
	return (uint16_t)mfloat1;
}
// 公制－》英制 （温度）华氏温标与摄氏温标的关系为F=9×t÷5+32
uint16_t unit_to_english_temp(uint16_t tmp16)
{
	float	mfloat1;
	mfloat1 = (float)tmp16;
	mfloat1 -= 600;
	mfloat1 *= 1.8;
	mfloat1 += 1080.5;	  //920.5
	return (uint16_t)(mfloat1);
}

uint16_t chang_unit_For_CTemp(int16_t tmp16)
{
	float	mfloat1;
	mfloat1 = (float)tmp16;
	mfloat1*=1.8;
	mfloat1+=320.5;
	return (uint16_t)(mfloat1);
}
uint16_t chang_english_For_Span(uint16_t tmp16)
{
	float	mfloat1;

	mfloat1 = (float)(tmp16);
	mfloat1 /= 1.8;
	mfloat1 += 0.5;
    return (uint16_t)mfloat1;
}

uint16_t chang_unit_For_Span(uint16_t tmp16)
{
	float	mfloat1;

	mfloat1 = (float)(tmp16);
	mfloat1 *= 1.8;
	mfloat1 += 0.5;
    return (uint16_t)mfloat1;
}

int update_rotation_param(void)
{
/*
    int32_t m0_quan = m0_inv * ((1U<<QUAN_BIT) - 1U);
    int32_t m1_quan = m1_inv * ((1U<<QUAN_BIT) - 1U);
    int32_t m2_quan = m2_inv * ((1U<<QUAN_BIT) - 1U);
    int32_t m3_quan = m3_inv * ((1U<<QUAN_BIT) - 1U);
    int32_t m4_quan = m4_inv * ((1U<<QUAN_BIT) - 1U);
    int32_t m5_quan = m5_inv * ((1U<<QUAN_BIT) - 1U);

    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG1_M0, *(uint32_t*)&m0_quan);
    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG2_M1, *(uint32_t*)&m1_quan);
    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG3_M2, *(uint32_t*)&m2_quan);
    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG4_M3, *(uint32_t*)&m3_quan);
    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG5_M4, *(uint32_t*)&m4_quan);
    APB3_REGW(OOB_APB_SLV, APB3_SLV_REG6_M5, *(uint32_t*)&m5_quan);

*/
  return 0;
}

void menuset(u32 addr,u16 layer1,u16 layer2,u16 layer3,u16 layer4,u16 layer5)
{
	// menu_layer1(0,addr);
	// menu_layer2(0,addr);
	// menu_layer3(0,addr);
	// menu_layer4(0,addr);
	// menu_layer5(0,addr);
}

void InitHyram(void)
{
//    int i,j;
//    u32 addr;
//    for(j=0;j< Frame_y;j++)
//    {
// 		 int stride = Frame_x*2;  // Frame_x*2;  (Frame_x+15)/16*16*2;
// 		 addr = TXT_BASE_ADDR + stride*j;

// 		 for(i=0;i<640;i++)
// 		 {
// 			 write_u16(0,addr);
// 			addr = addr +2;
// 		 }
// 	}

}
// void InitSystemStart(void)
// {
//    int i,j,k;
//    u8 data;
//    u32 addr;
//    bsp_printf("test start \n");
//    for(j=300;j< Frame_y*Frame_x;j++)
//    {

// //		data = SystemStart[j*Frame_x+i];
// 		for(k=0;k<8;k--)
// 		{
// 			if(data&0x80)
// 			{
// 				menu_layer2(3,TXT_BASE_ADDR+j*2 +i*2);
// 			}
// 			data = data<<1;
// 		}
//    }
//    bsp_printf("test start finish\n");
// }


void Poweronselfcheck(void)
{
	/*测试自检用（更直观）*/
	if(check1 == 0)
	{bsp_printf("DDR Operation Success\n");}
	else
	{bsp_printf("DDR Operation False\n");}

	if(check2 == 0)
	{bsp_printf("SENSOR Operation Success\n");}
	else
	{bsp_printf("SENSOR Operation False\n");}

	if(check3 == 0)
	{bsp_printf("FLASH Operation Success\n");}
	else
	{bsp_printf("FLASH Operation False\n");}

	if(check4 == 0)
	{bsp_printf("TEMP Operation Success\n");}
	else
	{bsp_printf("TEMP Operation False\n");}

	delay_1s(1);
	for(int i = 10;i > 0 ;i--)
    {
	    com_data.buff[0] = 0xBB;
	    com_data.buff[1] = 0xEE;
	    com_data.buff[2] = 0x01;
	    com_data.buff[3] = 0x01;
	    com_data.buff[4] = 0x01;
	    UartPoweronselfcheck();
	    if((send_data.rx_buff[0]=0xEE)&&(send_data.rx_buff[1]=0xBB)&&(send_data.rx_buff[4] == 0x0E))
	    {i = 0;}
	    if(i > 0)
	    {delay_1s(1);}
	}
	if(!(send_data.rx_buff[3] == 0x00))
    {bsp_printf("Infrared sensor self test failure\n");}
	memset(send_data.rx_buff,0,sizeof(send_data.rx_buff));
}

void SystemInit(void)
{
#if 0
	bsp_printf("System Start!\n");
	write_u32(0x0,IO_APB_SLAVE_0_INPUT);
	write_u32(0x0,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);
	write_u32(0x00000001,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);	
	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)|0x2000));
//	write_u32(0x00000049,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);

	memset(&key,0,sizeof(key));
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff));
	APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
	APB3_REGW_WatchDogSW(1);  //逻辑看门狗功能
	i2c_init();
//	delay_1ms(5);
	Init_ADV7391();
	bsp_printf("Open Color Bar\n");
//	delay_1ms(5);
	InitHyram();

	write_u32(0x0000000F,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);
#if (BeiFangGuangXue_BootIcon == 1)
	/*****************************32*32的开机图标*****************************/
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[0],272,240,32,6);
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[2],304,240,32,6);
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[4],336,240,32,6);
	/*****************************32*32的开机图标*****************************/

	/*****************************64*64的开机图标*****************************/
	for(u8 u=0;u<6;u++)
	{
		Display_MatrixIconX2((uint8_t *)TurnOnIcon[u],224+u*32,224,64,6);
	}
	/*****************************64*64的开机图标*****************************/
#endif

	//IR_NUC_OPR(1);
    gpio_setOutputEnable(GPIO0, 0xff);
    gpio_setOutput(GPIO0, 0x0);

	spi_init();
#ifdef FunctionAutoFocus
    MotorInit();
#endif

	vot_menu_init();
	UART_Init();
	Init_TMP112();

#ifdef FunctionAutoFocus
	MotorModeSelect();
#endif


	Read_Flash_NUC_init();
	Read_Flash_OOC_init();
    OOCSysetemInit();

#if(Mirror_DisplayType == 0)
    MirrorBadSppotInit();
#endif
	Int_Shutter_pro();  //test use
//    Poweronselfcheck();
	delay_1ms(15);
	WriteADV7391(0x84,0x00);	//关闭测试彩条，正常输出图像
	bsp_printf("Close Color Bar\n");
    delay_1ms(5);
    TimeInit();
    memset(com_data.buff,0,sizeof(com_data.buff));

#if DET_Change_Para
    detParaInit();
    DETParSelect();
#endif

	Int_Shutter_pro();  //test use
	delay_25ms(8);

#if (BeiFangGuangXue_BootIcon == 1)

	/*****************************32*32的开机图标*****************************/
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[0],272,240,32,0);
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[2],304,240,32,0);
//	Display_MatrixIcon((uint8_t *)TurnOnIcon[4],336,240,32,0);
	/*****************************32*32的开机图标*****************************/

	/*****************************64*64的开机图标*****************************/
	for(u8 u=0;u<6;u++)
	{
		Display_MatrixIconX2((uint8_t *)TurnOnIcon[u],224+u*32,224,64,0);
	}
	/*****************************64*64的开机图标*****************************/
#endif

	write_u32(0x0000001F,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);
	//write_u32(0x0000004F,IO_APB_SLAVE_0_INPUT_Hyperram_Opr);
#ifdef   SUPPORT_FPGA_USB_UART_PROTOCOL
     WriteCommandFlag(1,VerifyVal); //FPGA串口传输标志位
#endif


#if ((SUPPORT_HuanYu_UART_PROTOCOL==1) || (SUPPORT_ZhongLin_UART_PROTOCOL==1))
    FirmwareTransferInit();///在线升级
#endif
 	delay_25ms(20);
    Int_Shutter_pro();  //test use
    DisplayCenterSpot_Init();  //中心点光标
    bsp_printf("System Init end\n");
#endif
}

#if 0
void ShutterTest (u16 timeset)
{
	gpio_setOutput(GPIO0, 0x01);
	delay_1ms(timeset);
//	gpio_setOutput(GPIO0, 0x0);

	gpio_setOutput(GPIO0, 0x02);
	delay_1ms(timeset);
	gpio_setOutput(GPIO0, 0x0);
}
void shutterOpr(u8 sw)
{
	if(sw)
	{
//		gpio_setOutput(GPIO0, 0x02);
		//SetIO_Shutter(2);
//		bsp_printf("Shutter ON \n");
	}
	else
	{
//		gpio_setOutput(GPIO0, 0x01);
		//SetIO_Shutter(1);
//		bsp_printf("Shutter OFF \n");
	}
	delay_1ms(60);  //15
//	gpio_setOutput(GPIO0, 0x0);
	//SetIO_Shutter(0);
}
void keyscan (void)
{
	key.old = ReadKey();
	delay_1ms(25);
	key.new = ReadKey();
	if(key.old == key.new && key.new !=0x03)
	{
		key.one = key.new;
//		bsp_printf("NEW Press:%x\n",key.one);
/*
		while(ReadKey()!=0x0F)
		{
		}
		key.valid = key.new;
*/
	}
	if(key.one != 0 && ReadKey()==0x03)
	{
		key.valid = key.one;
	//	bsp_printf("valid Press:%x \n",key.valid);
	}
/*
	else
	{
		memset(&key,0,sizeof(key));
	}
*/
//	bsp_printf("key  %x\n",key.new);
}

void KeyOpr(void)
{
	keyscan();
	if(key.valid)
	{
		switch(key.valid)
		{
#if 0
			case Key_Shutter:
//				shutterOpr(ON);
				Int_Shutter_pro();

//				ShutterTest(20);
				bsp_printf("Shutter 20ms \n");
				bsp_printf("Key_Shutter Press addr_val:%x,addr_all:%x,addr_all2:%x\n",Readtest(),ReadtestALL(),ReadtestALL2());
				break;

			case Key_IR_SW:
//				ShutterTest(50);
				bsp_printf("Shutter 40ms \n");

				systemPar.IR_SW =!systemPar.IR_SW;
				SetIR_SW(systemPar.IR_SW);
				SetIR_SW_(systemPar.IR_SW);

//				bsp_printf("Key_IR_SW Press:%d ,addr_val:%x,addr_all:%x,addr_all2:%x\n",systemPar.IR_SW,Readtest(),ReadtestALL(),ReadtestALL2());
				break;

			case Key_TFilter:
//				ShutterTest(10);
				bsp_printf("Shutter 50ms \n");
/************************* test ***********************/
//				BNUC_init();
//				NUC_init();
//				Calc_NUC_DEC();
//				Write_Flash_NUC();
//				test_nuc();
/*****************************************************/

				NUC_init();
				NUC_Calculate_1();
				test_nuc();
/*
				systemPar.TFilter++;
				if(systemPar.TFilter>3)
				{
					systemPar.TFilter = 0;
				}
//			bsp_printf("Key_TFilter Press :%d,addr_val:%x,addr_all:%x,addr_all2:%x\n",systemPar.TFilter,Readtest(),ReadtestALL(),ReadtestALL2());
				SetTFilter(systemPar.TFilter);
*/
				break;
			case Key_Rerver:
				bsp_printf("Shutter 60ms \n");
				systemPar.NUC_SW =!systemPar.NUC_SW;
				SetNUC_SW(systemPar.NUC_SW);
//				 SetFreeze_img(systemPar.Test_SW);
//				systemPar.Test_SW =!systemPar.Test_SW;
//				SetTset_SW(systemPar.Test_SW);
//				Calc_NUC_INC();
//				Read_Flash_NUC();
//				test_nuc();
//				Display_test();
//				ShutterTest(40);

//				bsp_printf("Key_Rerver addr_val:%x,addr_all:%x,addr_all2:%x\n",Readtest(),ReadtestALL(),ReadtestALL2());
				break;
#endif
			case key_Color:
				UsermenuParameter.Palette++;
				if(UsermenuParameter.Palette==5) 	UsermenuParameter.Palette = 6;
				if(UsermenuParameter.Palette >14)	UsermenuParameter.Palette = 0;
				load_palette(UsermenuParameter.Palette);
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			break;
			case key_Scaler:
				UsermenuParameter.Enlarge+=20;
#ifdef SUPPORT_CONTINUOUS_ZOOM
				if(UsermenuParameter.Enlarge > 70)	UsermenuParameter.Enlarge=10;
				APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
	#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
				APB3_REGW_ZOOM_Scaler_XY((u16)(639-(UsermenuParameter.Enlarge-10)*8),(u16)(511-(UsermenuParameter.Enlarge-10)*6));
				APB3_REGW_ZOOM_XY((u16)((639-(UsermenuParameter.Enlarge-10)*8)*16384/640),(u16)((511-(UsermenuParameter.Enlarge-10)*6)*16384/512));
	#endif
#else
				if(UsermenuParameter.Enlarge > 3)	UsermenuParameter.Enlarge=0;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#endif
				break;

			default:
				break;
		}
//		delay_1ms(800);
		key.valid = 0;
		key.one = 0;
	}
}
#endif
void Int_Shutter_pro(void)		//内校正
{
	// // ISP_cut();
	//if(VotTools.BIGDATA_FALG) return;
#if 0
#if SUPPORT_ZhongLin_UART_PROTOCOL
	if(ShutterFlag) return;
#endif

	if(MotorFlag.Flag) return;

	if(!ProtectFlag)
	{
#ifdef OPR_ALARM
	APB3_REGW_Shutter_SW(ON);
#endif

	if(systemPar.NoShutter)  return;
	if(SystemParaMenu2.ShutFun) return;

	SetFreeze_img(ON);		//图像冻结
	delay_1ms(50);
	shutterOpr(OFF);
	if((SystemParaMenu2.Usermenu_SW)&&((SystemParaMenu2.TemTest_SW==1)||(SystemParaMenu2.TemTest_SW==3)))
	{
		Auto_INT();
		if((SelectionMenu.MenuState == 0x26)&&((SelectionMenu.SelectItem->Info.ID >= 0x61)&&(SelectionMenu.SelectItem->Info.ID <= 0x67)))//刷新显示
		{
#ifdef SUPPORT_VOT_DETECTOR
			DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+48,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);
#else
			DisplayBackground(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,TEST_Y_START+Y_GAP*4,4,BACKGROUND_LAYER,BackgroundColor);
			display_u8number(TEST_X_START+(TEST_X_INCREMENT+X_GAP)*3+56,Y_START+Y_GAP*4,SystemParaMenu2.INT_T,3,CharacterColor);
#endif
		}
	}
	delay_1ms(100);	   //delay_1ms(100);
	SetBaking_SW(ON);		//采集本底
//	bsp_printf("SetBaking_SW  :%d,addr_val:%x,addr_all:%x,addr_all2:%x\n",systemPar.TFilter,Readtest(),ReadtestALL(),ReadtestALL2());
//	delay_1s(1);
	delay_1ms(200);   //预计200ms  多预留点时间
	SetBaking_SW(OFF);		//采集本底结束
	delay_1ms(50);   //150
	shutterOpr(ON);
	delay_1ms(100);  //	delay_1ms(400);
	SetFreeze_img(OFF); //图像激活
	UsermenuParameter.ShutterCounter++;
//	bsp_printf("Shutter Complete! time:(%d:%d) \n",systemPar.Time_M,systemPar.Time_S);
#ifdef OPR_ALARM
	APB3_REGW_Shutter_SW(OFF);
#endif
	if(!SystemParaMenu2.Usermenu_SW)	{APB3_REGW_S_Range(SystemParaMenu2.S_Range - FPGA_Bak_AVG()/2);}  //防干烧功能（观测版）
	}
#endif
}
#if 0
void Ext_Shutter_pro(void)		//外校正
{
/*
	SetFreeze_img(ON);		//图像冻结
//	Shutter_dsp();
	delay_1ms(100);
	SetBaking_SW(ON);		//采集本底
	delay_1ms(100);
	SetFreeze_img(OFF); //图像激活
//	delay_1ms(100);
//	BAK_SUM_AVG = *(APB3_BASE + 64)>>16 & 0xffff;
//	BAK_SUM_AVG = read_u16(IO_APB_SLAVE_0_INPUT_ScreenAD_Avg);

*/
	SetFreeze_img(ON);		//图像冻结
	delay_1ms(100);	   //delay_1ms(300);
	SetBaking_SW(ON);		//采集本底
	delay_1s(1);
	SetBaking_SW(OFF);		//采集本底结束
	delay_1ms(400);
	SetFreeze_img(OFF); 	//图像激活
}


void ColorBarOut(int y_size,int x_size, int stride,u32 frameAdr){
	   int i,j;
	   u32 wdata;
	   u32 wdata_lo,wdata_hi;
	   u32 addr;
	   bsp_print("colorbar begin...\n\r");
	//   int color_width = x_size/8;
	   int color_width = 134;
	   u8  color_value[8];
	   for(i=0;i<8;i++){
	     color_value[i] = i*32;
	   }
	   for(j=0;j<y_size;j++){
		 addr = frameAdr + stride*j;
	     for(i=0;i<x_size;i=i+2){
		    if(i<color_width){
				wdata_lo = 0xff00ffff|(color_value[0]<<16);
			    wdata_hi = 0xffffff00|(color_value[0]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*2){
				wdata_lo = 0xff00ffff|(color_value[1]<<16);
			    wdata_hi = 0xffffff00|(color_value[1]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*3){
				wdata_lo = 0xff00ffff|(color_value[2]<<16);
			    wdata_hi = 0xffffff00|(color_value[2]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*4){
				wdata_lo = 0xff00ffff|(color_value[3]<<16);
			    wdata_hi = 0xffffff00|(color_value[3]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*5){
				wdata_lo = 0xff00ffff|(color_value[4]<<16);
			    wdata_hi = 0xffffff00|(color_value[4]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*6){
				wdata_lo = 0xff00ffff|(color_value[5]<<16);
			    wdata_hi = 0xffffff00|(color_value[5]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else if(i<color_width*7){
				wdata_lo = 0xff00ffff|(color_value[6]<<16);
			    wdata_hi = 0xffffff00|(color_value[6]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
		    else{
				wdata_lo = 0xff00ffff|(color_value[7]<<16);
			    wdata_hi = 0xffffff00|(color_value[7]);
				wdata    = 0x80ff80ff&wdata_lo&wdata_hi;
			}
	        write_u32(wdata,addr);
	        addr = addr +4;
		 }
	   }
}
#endif

void Auto_INT(void)  //自动调节INT值，新结构
{
	uint16_t DET_X;
	uint16_t DET_AD;      //T0
	// uint16_t Auto_k;
	float x,y,temp;
	ts_u08 last_int_t=0;

#ifdef 	SUPPORT_VOT_DETECTOR
	if(SystemParaMenu2.Usermenu_SW ==1) //一档测温规律公式
	{
		//*************最新版测温规律（适用环境-20-60度）*****************//
		vot_read_det_ad();
		DET_AD = global_sys_ctrl_info.m_det_temp_ad_value;
		if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_GENERAL || SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC)
		{
			if(DET_AD>=6058)
			{
				DET_X=DET_AD-6058;
				x  = (float)DET_X;
				y = x*x;
				y *= 0.0079;
				x *= 36.9;
				y -= x;
				y += 101051;
			}
			else
			{
				DET_X=6058-DET_AD;
				x  = (float)DET_X;
				y = x*x;
				y *= 0.0079;
				x *= 36.9;
				y += x;
				y += 101051;
			}

			y = y/100000;
			temp = y * 36+4;//  四舍五入
			last_int_t = SystemParaMenu2.INT_T;
			SystemParaMenu2.INT_T = (uint8_t)temp;
			ts_f32 temp_f = READ_TMP112_Temp();

			if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC)
			{
				if(temp_f <= 15.0f) {
					SystemParaMenu2.INT_T = SystemParaMenu2.INT_T-2;
				}
					if((15.0f < temp_f) && (temp_f <= 25.0f)) {
					SystemParaMenu2.INT_T=SystemParaMenu2.INT_T-1;
				}
			}else if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_GENERAL){
				if(temp_f <= -12.0f) {
					if(SystemParaMenu2.INT_T ==25) {
						SystemParaMenu2.INT_T = 24;
					}
				}
			}
		}else if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_HAND384){
			if(DET_AD>=6007)
			{
				DET_X=DET_AD-6007;
				x  = (float)DET_X;
				y = x*x;
				y *= 0.0094;
				x *= 41.652;
				y -= x;
				y += 100804;
			}
			else
			{
				DET_X=6007-DET_AD;
				x  = (float)DET_X;
				y = x*x;
				y *= 0.0094;
				x *= 41.652;
				y += x;
				y += 100804;
			}

			y = y/100000;
			temp = y * 38 + 2;// 小数点计算 
			last_int_t = SystemParaMenu2.INT_T;
			ts_f32 tempf = READ_TMP112_Temp();
			if(tempf < -8.0f){
				temp = 22.5;
			}
			SystemParaMenu2.INT_T = (uint8_t)temp;

			ts_f32 INT_Float = temp - SystemParaMenu2.INT_T; 
			global_sys_ctrl_info.SetValue_INTtoMK =(ts_u16)((INT_Float*0.0264 + 1)*8192);//得到小数部分，传给MK
			// vision_log_notice("----[1]Auto_INT: INT_T=%d, int_Float=%f, INTtoMK=%d---- \n", SystemParaMenu2.INT_T, int_Float, global_sys_ctrl_info.SetValue_INTtoMK);
		}
	}
	else if((SystemParaMenu2.Usermenu_SW ==2)||(SystemParaMenu2.Usermenu_SW ==3)) //二档测温规律公式 04.01    三档测温与二档测温暂时共用
	{
		vot_read_det_ad();
		DET_AD = global_sys_ctrl_info.m_det_temp_ad_value;
		if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_GENERAL || SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC)
		{
		if(DET_AD>=6161)
		{
			DET_X=DET_AD-6161;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0059;
			x *= 39.272;
			y -= x;
			y += 100121;
		}
		else
		{
			DET_X=6161-DET_AD;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0059;
			x *= 39.272;
			y += x;
			y += 100121;
		}

		y = y/100000;
		temp = y * 29.7+18.3;//  四舍五入
		last_int_t = SystemParaMenu2.INT_T;
		SystemParaMenu2.INT_T = (uint8_t)temp;

		ts_f32 temp_f = READ_TMP112_Temp();
		if(temp_f <= -19.5f) {
			if(SystemParaMenu2.INT_T ==32)	{
				SystemParaMenu2.INT_T = 31;
			}
		}
		if(SystemParaMenu2.INT_T >=53) {

			if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_GENERAL){
				SystemParaMenu2.INT_T = SystemParaMenu2.INT_T+3;
			}else if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
				SystemParaMenu2.INT_T = SystemParaMenu2.INT_T+2; //26.07.19修改，由原本的+3改为+2
			}
		}
	 }else if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_HAND384) {
		if(DET_AD>=6037)
		{
			DET_X=DET_AD-6037;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0081;
			x *= 41.459;
			y -= x;
			y += 100816;
		}
		else
		{
			DET_X=6037-DET_AD;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0081;
			x *= 41.459;
			y += x;
			y += 100816;
		}

		y = y/100000;
		temp = y * 29.7+16.3;// 
		last_int_t = SystemParaMenu2.INT_T;
		SystemParaMenu2.INT_T = (uint8_t)temp;

		ts_f32 int_Float = temp - SystemParaMenu2.INT_T; 
		global_sys_ctrl_info.SetValue_INTtoMK =(ts_u16)((int_Float*0.0336 + 1)*8192);//得到小数部分，传给MK
		// vision_log_notice("----Auto_INT: INT_T=%d, int_Float=%f, INTtoMK=%d---- \n", SystemParaMenu2.INT_T, int_Float, global_sys_ctrl_info.SetValue_INTtoMK);
	 }
	}
#else 		 //IRay测温
	if(SystemParaMenu2.Usermenu_SW ==1)
	{
	//新的一档测温规律公式
		DET_AD=ReadDETAD();
		if(DET_AD>=5987)
		{
			DET_X=DET_AD-5987;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0067;
			x *= 32.986;
			y -= x;
			y += 100578;
		}
		else
		{
			DET_X=5987-DET_AD;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0067;
			x *= 32.986;
			y += x;
			y += 100578;
		}

		y = y/100000;
		temp = y * 42+5.5;//3.7+0.5
		SystemParaMenu2.INT_T = (uint8_t)temp;

		//Auto_k=0x2000;
	}
	else if((SystemParaMenu2.Usermenu_SW ==2)||(SystemParaMenu2.Usermenu_SW ==3)) //三档测温与二档测温暂时共用
	{
		//新的二档测温规律公式  //2025.04.30更新到最新公式
		DET_AD=ReadDETAD();
		if(DET_AD>=6022)
		{
			DET_X=DET_AD-6022;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0053;   
			x *= 34.49;
			y -= x;
			y += 100284;
		}
		else
		{
			DET_X=6022-DET_AD;
			x  = (float)DET_X;
			y = x*x;
			y *= 0.0053;
			x *= 34.49;
			y += x;
			y += 100284;
		}

		y = y/100000;
//				temp = y * 37+20.5;//3.7+0.5
		temp = y * 29.5+21;//20.5+0.5
		SystemParaMenu2.INT_T = (uint8_t)temp;
		//Auto_k=0x2000;
	}
#endif
	if (SystemParaMenu2.INT_T > 76) {
		SystemParaMenu2.INT_T = 76;
	}
	if (last_int_t != SystemParaMenu2.INT_T) {
		planck640c_write_tint(SystemParaMenu2.INT_T);	//INT_T
		planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
		ts_cpl_fix_msleep(100);
	}
}
#if 0
/*******************成都浊弥_TEST******************/
void AutoTempCorrectionOverTime(void)
{
        float K,TEMP;
        u16 TEMP_16,count;
        static u16 Last_TEMP_16;

        if(UsermenuParameter.AutoShutter == 0)          {ShutterTimer = 0;count = 30;}
        else  count = (u16)(30 * UsermenuParameter.AutoShutter);

        DispSysMenu2.Det_PAR = ReadDETAD();

#ifdef SUPPORT_VOT_DETECTOR
                if(DispSysMenu2.Det_PAR < 7416) //正
                {
                        K = 33;
                        TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
                }
                else  //负数
                {
                        K = 33;
                        TEMP = (DispSysMenu2.Det_PAR - 7416)/K;
                //        TEMP_16 = (~TEMP_16) + 1;    //补码表示
                }
#else
                if(DispSysMenu2.Det_PAR < 7543) //正
                {
                        K = 33.8;
                        TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
                }
                else  //负数
                {
                        K = 33.8;
                        TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
                //        TEMP_16 = (~TEMP_16) + 1;    //补码表示
                }
#endif

        switch(AutoTempFlag)
        {
                default:
                        break;
                case 1:
                        Last_TEMP_16 = (uint16_t)(TEMP *10);
                        AutoTempFlag = 2;
                        break;
                case 2:
                        TEMP_16 = (uint16_t)(TEMP *10);
#ifdef WLP
                        if(SystemParaMenu2.TempChangeValue && (systemPar.shutter_flag > 3))
#else
                        if(SystemParaMenu2.TempChangeValue && (systemPar.shutter_flag == 3))
#endif
                        {
#ifdef WLP
                        		if(abs(TEMP_16 - Last_TEMP_16) >= SystemParaMenu2.TempChangeValue)
#else
                                if(abs(TEMP_16 - Last_TEMP_16) >= 10 * SystemParaMenu2.TempChangeValue)
#endif
                                {
                                        if(count - ShutterTimer >= 5)//和挡片时间错开5秒以上
                                        {
                                                Int_Shutter_pro();
                                        }
                                        Last_TEMP_16 = TEMP_16;
                                }
                        }
                        break;
        }

}

void Auto_GOF(void)
{
#ifndef		SUPPORT_VOT_DETECTOR
	int16_t Data_16bit;
	float  temp;

	if((SystemParaMenu2.TemTest_SW != 4)&&AutoGOF_Flag)
	{
	temp = READ_TMP112_Temp();
	Data_16bit = (int16_t)(temp*10);
	Data_16bit = Data_16bit/10;


	if(Data_16bit>45)		{	SystemParaMenu2.GOF = 81;	}
	else					{	SystemParaMenu2.GOF = 72;	}

	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF
	}
#endif
}

void testOpr(void)  //承恩测试参数
{
/*	u16 i,j;
	u8* ptr;
	switch(systemPar.ImgNum)
	{
		case 0:
			ptr = (u8*)(DDR_IR_Image_T0+640*2); //+640*4
			break;
		case 1:
			ptr = (u8*)DDR_IR_Image_T1;
			break;
		case 2:
			ptr = (u8*)DDR_IR_Image_T2;
			break;
		case 3:
			ptr = (u8*)DDR_IR_Image_T3;
			break;
	}
	for(j=0; j<480;j++) // IR_Y_pixel
	{
		for(i=0; i<IR_X_pixel;i++)
		{
//			if(j&0xFF ==255) *ptr = 254;
//			else	   *ptr = (u8)j;
			*ptr = (u8)j;
			ptr++;

//			if(i&0xFF==255) *ptr = 254;
//			else       *ptr = (u8)i;
			*ptr = (u8)i;
			ptr++;
		 }
	}*/
#ifdef VOT
	SystemParaMenu2.DDE_Level =4;

#else
	SystemParaMenu2.DDE_Level =9;
#endif

	SystemParaMenu2.Gamma   = 3;
	SystemParaMenu2.Plat_NX = 12;
	SystemParaMenu2.FK8_SW = 1;
	SystemParaMenu2.DDE_SW =1;
	SystemParaMenu2.TFilter =1;
	SystemParaMenu2.SFilter = 1;
	//SystemParaMenu2.MedFilt_SW =1;
	SystemParaMenu2.DEST_SW = 1;
	SystemParaMenu2.HOST_SW = 1;
	SystemParaMenu2.Gama_TabVal = 3;

	//SystemParaMenu2.BF_SW = 1;
	//APB3_REGW_BF_SW(SystemParaMenu2.BF_SW);

	UsermenuParameter.ImageAdjust = 2;
	APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));

	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffe3ffff)|(SystemParaMenu2.Gamma<<18)));
	APB3_REGW_Plat_NX(SystemParaMenu2.Plat_NX);
	APB3_REGW_FK8_SW(SystemParaMenu2.FK8_SW);
	APB3_REGW_DDE_SW(SystemParaMenu2.DDE_SW);
	APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level*16-1);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff)|(SystemParaMenu2.TFilter<<10)));

	APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
	//APB3_REGW_MedFilt_SW(SystemParaMenu2.MedFilt_SW);

	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffdfff)|(SystemParaMenu2.DEST_SW<<13)));
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffbfff)|(SystemParaMenu2.HOST_SW<<14)));

	load_palette(UsermenuParameter.Palette);//加载伪彩

	SystemParaMenu2.Output_mode = 0x0f;//默认CVBS版本
	SystemParaMenu2.CVBS_flag = 1;
	APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
	APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xfffffff7)|(SystemParaMenu2.CVBS_flag<<3)));

	SystemParaMenu2.ImageIndoorPara.Ran_Limit = 160; //2024.05.04 暂时给160
	SystemParaMenu2.ImageOutdoorPara.Ran_Limit = 100;
	switch(UsermenuParameter.ImageAdjust)
	{
		case 1:
		SystemParaMenu2.Ran_Limit = SystemParaMenu2.ImageIndoorPara.Ran_Limit;
		break;
		case 2:
		SystemParaMenu2.Ran_Limit = SystemParaMenu2.ImageOutdoorPara.Ran_Limit;
		break;
	}
	APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xff00ffff)|(SystemParaMenu2.Ran_Limit<<16)));
	UpdataImagePara();
}
void OneFramePrintf(void)
{
	u16 i,j;
	u8* ptr;
	ptr = (u8*)DDR_IR_Image_T00;   //	DDR_IR_Image_T0
	for(j=0; j<481;j++)   //DDR_IR_Image_T00
	{
		for(i=0; i<IR_X_pixel*2;i++)
		{
			bsp_printf("%c",*ptr);
			ptr++;
		 }
/*		bsp_printf("%c",0xDD);
		bsp_printf("%c",0xEE);*/
	}
}


void ShieldProtect(void)
{
	u16 Over_Num;
//	if(ProtectTime >= UsermenuParameter.P_Time*60)
//	{
//		UsermenuParameter.SunPT_SW = 0;
//		APB3_REGW_SunPT_SW(UsermenuParameter.SunPT_SW);
//		if((SelectionMenu.MenuState == 0x25)&&((SelectionMenu.SelectItem->Info.ID >= 0x31)&&(SelectionMenu.SelectItem->Info.ID <= 0x39)))
//		{
//			if(SelectionMenu.IsEnableEnligh == 0)
//			{
//				DisplayBackground((TEST_X_START+TEST_X_INCREMENT*3)+104,TEST_Y_START+Y_GAP*8,3,BACKGROUND_LAYER,BackgroundColor);
//				DisPlayNewStr((TEST_X_START+TEST_X_INCREMENT*3)+104,Y_START+Y_GAP*8,"OFF",3,CharacterColor);
//			}
//			else
//			{
//				DisplayBackground((TEST_X_START+TEST_X_INCREMENT*3)+112,TEST_Y_START+Y_GAP*8,3,BACKGROUND_LAYER,BackgroundColor);
//				DisPlayNewStr((TEST_X_START+TEST_X_INCREMENT*3)+112,Y_START+Y_GAP*8,"OFF",3,CharacterColor);
//			}
//
//		}
//		else if((SelectionMenu.MenuState == 0x26)&&((SelectionMenu.SelectItem->Info.ID >= 0x81)&&(SelectionMenu.SelectItem->Info.ID <= 0x8C)))
//		{
//				DisplayBackground((TEST_X_START+TEST_X_INCREMENT*2)+60,TEST_Y_START+Y_GAP*9,2,BACKGROUND_LAYER,BackgroundColor);
//				display_u8number((TEST_X_START+TEST_X_INCREMENT*2)+60,Y_START+Y_GAP*9,0,3,CharacterColor);
//		}
//	}
	if(UsermenuParameter.SunPT_SW)
	{
		if(!ProtectFlag)
		{
			if(SystemParaMenu2.Usermenu_SW)
			{
				if(SystemParaMenu2.Usermenu_SW==1)				{SystemParaMenu2.S_Val = SystemParaMenu2.S_Val1;}
				else if(SystemParaMenu2.Usermenu_SW==2)		{SystemParaMenu2.S_Val = SystemParaMenu2.S_Val2;}
			}
			else	{SystemParaMenu2.S_Val = SystemParaMenu2.S_Val0;}

			Over_Num = (read_u32(Hypprram_Alarm3)>>16)&0x3FFF;
			if(Over_Num >= SystemParaMenu2.S_Val)
			{
				APB3_REGW_Shutter_SW(ON);
				SetFreeze_img(ON);
				delay_1ms(50);
				shutterOpr(OFF);
				delay_1ms(100);
				SetFreeze_img(OFF);
				APB3_REGW_Shutter_SW(OFF);
				ProtectFlag = 1;
			}
		}
		else
		{
			if(ProtectTime >= UsermenuParameter.P_Time*60)
			{
				APB3_REGW_Shutter_SW(ON);
				//SetFreeze_img(ON);
				delay_1ms(50);
				shutterOpr(ON);
				//SetFreeze_img(OFF);
				APB3_REGW_Shutter_SW(OFF);

				ShutterTimer=0;
				ProtectFlag = 0;
				delay_1ms(200);
				Int_Shutter_pro();
				ProtectTime = 0;
			}
		}
	}
	else
	{
		//if(!ProtectTime)return;
		if(ProtectFlag)
		{
			APB3_REGW_Shutter_SW(ON);
			//SetFreeze_img(ON);
			delay_1ms(50);
			shutterOpr(ON);
			//SetFreeze_img(OFF);
			APB3_REGW_Shutter_SW(OFF);

			ShutterTimer=0;
			ProtectFlag = 0;
			delay_1ms(200);
			Int_Shutter_pro();
			ProtectTime = 0;
		}
	}
}

void CameralinkSW(void)
{
	systemPar.CameralinkSW = !systemPar.CameralinkSW;
	APB3_REGW_CameralinkSW(systemPar.CameralinkSW);
	bsp_printf("%c",systemPar.CameralinkSW);
}
void AlgorithmSW(u8 state)
{
	if(!state) //YUV
	{
		SystemParaMenu2.AGC_SW =1;
//		SystemParaMenu2.DEST_SW = 1;
//		SystemParaMenu2.HOST_SW = 1;
//		SystemParaMenu2.AGC_Filter = 1;
	}
	else //AD
	{
		SystemParaMenu2.AGC_SW =0;

//		SystemParaMenu2.FK8_SW = 0;
//		SystemParaMenu2.DDE_SW =0;
//		//SystemParaMenu2.MedFilt_SW =0;
//		SystemParaMenu2.DEST_SW = 0;
//		SystemParaMenu2.HOST_SW = 0;
//		SystemParaMenu2.Gama_TabVal = 2;
//
//		SystemParaMenu2.AGC_Filter = 0;
//		//SystemParaMenu2.SFDDE = 0;
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//		SystemParaMenu2.Gamma = 3;
//		SystemParaMenu2.TFilter =2;
//		//SystemParaMenu2.BF_SW = 1;
//#else
//		SystemParaMenu2.Gamma = 0;
//		SystemParaMenu2.TFilter =0;
//		//SystemParaMenu2.BF_SW = 0;
//#endif
//
//#ifdef VOT
//		SystemParaMenu2.DTMY_SW = 0;
//		APB3_REGW(Hypprram_Alarm2,0,((APB3_REGR(Hypprram_Alarm2,0)&0xfffffbff)|(SystemParaMenu2.DTMY_SW<<10)));
//#endif
	}
	APB3_REGW_AGC_SW(SystemParaMenu2.AGC_SW);
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffe3ffff)|(SystemParaMenu2.Gamma<<18)));
//	APB3_REGW_FK8_SW(SystemParaMenu2.FK8_SW);
//	APB3_REGW_DDE_SW(SystemParaMenu2.DDE_SW);
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff)|(SystemParaMenu2.TFilter<<10)));
//	//APB3_REGW_MedFilt_SW(SystemParaMenu2.MedFilt_SW);
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffdfff)|(SystemParaMenu2.DEST_SW<<13)));
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffbfff)|(SystemParaMenu2.HOST_SW<<14)));
//	//APB3_REGW_BF_SW(SystemParaMenu2.BF_SW);

//	APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xf8ffffff)|(SystemParaMenu2.AGC_Filter<<24)));
//	APB3_REGW(IO_APB_PAT_0X23,0,((APB3_REGR(IO_APB_PAT_0X23,0)&0xfffffdff)|(SystemParaMenu2.SFDDE<<9)));
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffdfff)|(SystemParaMenu2.DEST_SW<<13)));
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffbfff)|(SystemParaMenu2.HOST_SW<<14)));
//	load_palette(UsermenuParameter.Palette);//加载伪彩
//	ImageAdjustSet(UsermenuParameter.ImageAdjust); //加载对应的图像模式参数
}


void detParaInit(void){
#if DET_Change_Para
	detPara.lastFlag = 0;
	detPara.statusFlag = 0;
	detPara.count = 0;
	detPara.count1 = 0;
	detPara.count2 = 0;
	detPara.value = 128;
#endif
}

void dynamicDETParChange(void) {
#if DET_Change_Para
	float K,TEMP;
	uint16_t TEMP_16;
	u16	DDE_LevelVal;

	if(SystemParaMenu2.Usermenu_SW) return;
#ifdef SUPPORT_VOT_DETECTOR
	if(SystemParaMenu2.DetMode)	return;
#endif

	DispSysMenu2.Det_PAR = ReadDETAD();

#ifdef SUPPORT_VOT_DETECTOR
	if(DispSysMenu2.Det_PAR < 7416) //正
	{
		K = 33;
		TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
		TEMP_16 = (uint16_t)(TEMP *10);

		if(TEMP_16 < 0) {
			detPara.count1 = 0;
			detPara.count2 = 0;
			detPara.count++;
			if(detPara.count >= 60){
				detPara.statusFlag = 1;
				detPara.count = 0;
			}
		}else if(TEMP_16 >= 0 && TEMP_16 <= 500){
			detPara.count = 0;
			detPara.count2 = 0;
			detPara.count1++;
			if(detPara.count1 >= 60){
				detPara.statusFlag = 2;
				detPara.count1 = 0;
			}
		}else{
			detPara.count = 0;
			detPara.count1 = 0;
			detPara.count2++;
			if(detPara.count2 >= 60){
				detPara.statusFlag = 3;
				detPara.count2 = 0;
			}
		}
	}
	else  //负数
	{
		detPara.count1 = 0;
		detPara.count2 = 0;
		detPara.count++;
		if(detPara.count >= 60){
			detPara.statusFlag = 1;
			detPara.count = 0;
		}
	}
#else
	if(DispSysMenu2.Det_PAR < 7543) //正
	{
		//DisPlayNewFont("+",36);
		K = 33.8;
		TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
		TEMP_16 = (uint16_t)(TEMP *10);

		if(TEMP_16 < 100) {
			detPara.count1 = 0;
			detPara.count2 = 0;
			detPara.count++;
			if(detPara.count >= 60){
				detPara.statusFlag = 1;
				detPara.count = 0;
			}
		}else if(TEMP_16 >= 100 && TEMP_16 <= 580){
			detPara.count = 0;
			detPara.count2 = 0;
			detPara.count1++;
			if(detPara.count1 >= 60){
				detPara.statusFlag = 2;
				detPara.count1 = 0;
			}
		}else{
			detPara.count = 0;
			detPara.count1 = 0;
			detPara.count2++;
			if(detPara.count2 >= 60){
				detPara.statusFlag = 3;
				detPara.count2 = 0;
			}
		}
	}
	else  //负数
	{
		detPara.count1 = 0;
		detPara.count2 = 0;
		detPara.count++;
		if(detPara.count >= 60){
			detPara.statusFlag = 1;
			detPara.count = 0;
		}
	}
#endif

	if(detPara.statusFlag != detPara.lastFlag)  {
//#ifdef SUPPORT_VOT_DETECTOR
		switch(detPara.statusFlag) {
		default: break;
		case 1:
			SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;
#ifdef SUPPORT_VOT_DETECTOR
			SystemParaMenu2.SFDDE = 0;
			SystemParaMenu2.DDE_Level = 8;
#endif
			//statusFlag = 0;
			break;
		case 2:
			SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;
#ifdef SUPPORT_VOT_DETECTOR
			SystemParaMenu2.SFDDE = 1;
			switch(UsermenuParameter.ImageAdjust){
			case 0:  //手动
				SystemParaMenu2.DDE_Level =SystemParaMenu2.ImageManualPara.DDELevel; break;
			case 1: //室内
				SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageIndoorPara.DDELevel; break;
			case 2: //室外
				switch(SystemParaMenu2.OutdoorSel)
				{
					case 0:  //GD
						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorPara.DDELevel; break;
					case 1:  //M3
						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorM3Para.DDELevel;break;
					case 2:  //JL
						SystemParaMenu2.DDE_Level = SystemParaMenu2.ImageOutdoorJLPara.DDELevel;break;
				}
				break;
			}
#endif
			//statusFlag = 0;
			break;
		case 3:
			SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;
#ifdef SUPPORT_VOT_DETECTOR
			SystemParaMenu2.SFDDE = 0;
			SystemParaMenu2.DDE_Level = 8;
#endif
			//statusFlag = 0;
			break;
		}
//#else
//		switch(detPara.statusFlag) {
//		default: break;
//		case 1:
//			SystemParaMenu2.INT_C = 5;
//			SystemParaMenu2.INT_T = 45;
//			SystemParaMenu2.OOC_Par = 35;
//			//statusFlag = 0;
//			break;
//		case 2:
//			SystemParaMenu2.INT_C = 3;
//			SystemParaMenu2.INT_T = 65;
//			SystemParaMenu2.OOC_Par = 37;
//			//statusFlag = 0;
//			break;
//		case 3:
//			SystemParaMenu2.INT_C = 4;
//			SystemParaMenu2.INT_T = 60;
//			SystemParaMenu2.OOC_Par = 42;
//			//statusFlag = 0;
//			break;
//		}
//#endif

		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));
		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));
		write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
#ifdef SUPPORT_VOT_DETECTOR
		APB3_REGW(IO_APB_PAT_0X23,0,((APB3_REGR(IO_APB_PAT_0X23,0)&0xfffffdff)|(SystemParaMenu2.SFDDE<<9)));
		if(SystemParaMenu2.DDE_Level == 0)		{DDE_LevelVal = 0;}
		else									{DDE_LevelVal= SystemParaMenu2.DDE_Level*16-1;}
		APB3_REGW_DDE_LV(DDE_LevelVal);
#endif

//#if ((SUPPORT_IRAY_DETECTOR==1)||(Temperature_Version==1))//艾睿版本或者光智测温
//		if(SystemParaMenu2.Usermenu_SW <= 1) {
//			SystemParaMenu2.INT_C_Val[0] = SystemParaMenu2.INT_C ;
//			SystemParaMenu2.INT_T_Val[0] = SystemParaMenu2.INT_T ;
//			SystemParaMenu2.OOC_Par_Val[0] = SystemParaMenu2.OOC_Par ;
//		}else if(SystemParaMenu2.Usermenu_SW == 2) {
//			SystemParaMenu2.INT_C_Val[1] = SystemParaMenu2.INT_C;
//			SystemParaMenu2.INT_T_Val[1] = SystemParaMenu2.INT_T;
//			SystemParaMenu2.OOC_Par_Val[1] = SystemParaMenu2.OOC_Par;
//		}
//#endif
		//u8 value = detPara.statusFlag - 1;
		//APB3_REGW_NUC_PARA(detPara.value);
		Int_Shutter_pro();
		refreshDetMenu();
	}
	//APB3_REGW_NUC_PARA(detPara.value);
	detPara.lastFlag = detPara.statusFlag;


#endif
}
#endif


ts_u08 GetDetTempStatus(ts_u08 is_inited_flag)
{
	ts_u08 det_temp_mode = DET_TEMP_MODE_NORMAL;
	// static ts_u08 cnt = 0;
	vot_snr_update_det_ad();
	vot_read_det_ad();
	ts_f32 m_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
	ts_s32 m_det_temp_value_ten = (ts_s32)(m_det_temp_value*10.0f);

	// ts_f32 m_det_temp_value_f[12]={-0.7f, -0.6f, -0.1f, 0.3f,10.0f, 30.0f, 49.4f, 49.7f, 50.0f,50.3f,50.5f,51.0f};
	// cnt++;
	// m_det_temp_value = m_det_temp_value_f[12-(cnt%12)];
	// m_det_temp_value = m_det_temp_value_f[(cnt%12)];
	
	// m_det_temp_value_ten = (ts_s32)(m_det_temp_value*10.0f);
	// vision_log_debug("m_det_temp_value %f  is_inited_flag %d m_det_temp_value_ten %d \n",m_det_temp_value,is_inited_flag,m_det_temp_value_ten);
	
	// if(is_inited_flag==1)
	{

		if(m_det_temp_value_ten<=-5){
			det_temp_mode = DET_TEMP_MODE_LOW;
			// vision_log_debug("DET_TEMP_MODE_LOW \n");
		}
		else if((m_det_temp_value_ten>(ts_s32)5) && (m_det_temp_value_ten<=(ts_s32)495)){
			det_temp_mode = DET_TEMP_MODE_NORMAL;
			// vision_log_debug("DET_TEMP_MODE_NORMAL \n");
		}
		else if(m_det_temp_value_ten>=(ts_s32)505){
			det_temp_mode = DET_TEMP_MODE_HIGH;
			// vision_log_debug("DET_TEMP_MODE_HIGH \n");
		}
		// else{
		// 	// vision_log_debug("keep mode \n");
			
		// }
	}


	return det_temp_mode;
}

void VotDETSetTempParameter(ts_u08 det_mode)
{

	vision_log_warn(" det_mode %d \n",det_mode);
	ISP_set_det_calibrationing_flag(1);

	ISP_set_det_switching_flag(1);
	ts_u08 sun_pt_sw = SystemParaMenu2.SunPT_SW;
	SystemParaMenu2.SunPT_SW = 0;

	vot_vpo_freeze();

	ISP_pause();
	ts_cpl_fix_msleep(200);
	switch(det_mode) {
		default: break;
		case DET_TEMP_MODE_LOW:
			SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_LOW %d \n",det_mode);
			break;
		case DET_TEMP_MODE_NORMAL:
			SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_NORMAL %d \n",det_mode);
			break;
		case DET_TEMP_MODE_HIGH:
			SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_HIGH %d \n",det_mode);
			break;
		case DET_TEMP_MODE_OFF:
			SystemParaMenu2.INT_C = SystemParaMenu2.offChange.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.offChange.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.offChange.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_OFF %d \n",det_mode);
			break;
	}
	
	if(SystemParaMenu2.INT_T>=77){
		SystemParaMenu2.INT_T = 76;
	}


	
	 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
		planck1280c_write_gof(SystemParaMenu2.GOF1280);
		planck1280c_write_cint(SystemParaMenu2.Cint1280);
		planck1280c_write_Nint(SystemParaMenu2.Nint);
		planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
		planck1280c_write_NTCC(SystemParaMenu2.NTCC);
		planck1280c_write_Voccm(SystemParaMenu2.Voccm);
		planck1280c_write_detConfig(SystemParaMenu2.detConfig);
		planck1280c_write_Vfid(SystemParaMenu2.Vfid);
	}
	#else
	{
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		planck640c_write_cint(SystemParaMenu2.INT_C);
		planck640c_write_tint(SystemParaMenu2.INT_T);
		planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
    }
	#endif
	//避免重复配置gos
	det_gos_switch_force_last = SystemParaMenu2.OOC_Par;
	ts_cpl_fix_msleep(200);

	ISP_run();

	vot_isp_cut_with_flag(0,1,0) ;

	vision_log_debug("leave : after ISP_cut \n");
	
	vot_vpo_unfreeze();

	global_sys_ctrl_info.m_det_cur_mode = det_mode;
	SystemParaMenu2.SunPT_SW = sun_pt_sw;

	ISP_set_det_switching_flag(0);
	ISP_set_det_calibrationing_flag(0);
	vision_log_warn("leave :  \n");

}

void VotDETParSelect(void) 
{
	static DetPara det_para={DET_TEMP_MODE_BUTT,DET_TEMP_MODE_BUTT,0,0,0,0};
	static ts_u08 is_inited_flag=0;
	// ts_u08 m_det_temp_mode;

#if DET_Change_Para

	if(SystemParaMenu2.Usermenu_SW) return;
	//if(SystemParaMenu2.DetMode)	return;

	det_para.lastFlag = global_sys_ctrl_info.m_det_cur_mode;
	if(SystemParaMenu2.DetMode==DET_TEMP_MODE_AUTO){
		
		det_para.statusFlag = GetDetTempStatus(is_inited_flag);
		if(det_para.lastFlag!=det_para.statusFlag){

			vision_log_warn("old %d , new %d \n",det_para.lastFlag , det_para.statusFlag );
			VotDETSetTempParameter(det_para.statusFlag);
			det_para.lastFlag = det_para.statusFlag;
			
		}
		if(is_inited_flag==0){
			is_inited_flag=1;
		}
	}
	else{
		
		if(det_para.lastFlag!=SystemParaMenu2.DetMode){
			vision_log_warn(" %d SystemParaMenu2.DetMode %d \n",SystemParaMenu2.DetMode,SystemParaMenu2.DetMode);
			VotDETSetTempParameter(SystemParaMenu2.DetMode);
			det_para.lastFlag = SystemParaMenu2.DetMode;
		}
		
	}
	// vision_log_debug(" %d \n",SystemParaMenu2.DetMode);

#endif
	
}


void DETParSelect(void) 
{
	return ;
/*
#if DET_Change_Para
	float K,TEMP;
	uint16_t TEMP_16;
	// u16	DDE_LevelVal;

	if(SystemParaMenu2.Usermenu_SW) return;
	//if(SystemParaMenu2.DetMode)	return;

	DispSysMenu2.Det_PAR = vot_read_det_ad();

#ifdef SUPPORT_VOT_DETECTOR
	if(DispSysMenu2.Det_PAR < 7416) //正
	{
		K = 33;
		TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
		TEMP_16 = (uint16_t)(TEMP *10);

		if(TEMP_16 < 0) {
			detPara.statusFlag = 1;
		}else if(TEMP_16 >= 0 && TEMP_16 <= 500){
			detPara.statusFlag = 2;
		}else{
			detPara.statusFlag = 3;
		}
	}
	else  //负数
	{
		detPara.statusFlag = 1;
	}
#else
	//7543H
	if(DispSysMenu2.Det_PAR < 7543) //正
	{
		//DisPlayNewFont("+",36);
		K = 33.8;
		TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
		TEMP_16 = (uint16_t)(TEMP *10);

		if(TEMP_16 < 100) {
			detPara.statusFlag = 1;
		}else if(TEMP_16 >= 100 && TEMP_16 <= 580){
			detPara.statusFlag = 2;
		}else{
			detPara.statusFlag = 3;
		}
	}
	else  //负数
	{
		detPara.statusFlag = 1;
	}
#endif

//#ifdef SUPPORT_VOT_DETECTOR
	switch(SystemParaMenu2.DetMode){
	default:break;
	case 0:
		switch(detPara.statusFlag) {
		default: break;
		case 1:
			SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;

			//statusFlag = 0;
			break;
		case 2:
			SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;

			SystemParaMenu2.SFDDE = 1; //M3/GD//JL
			//statusFlag = 0;
			break;
		case 3:
			SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;

			//statusFlag = 0;
			break;
		}
		break;

	case 1:
		SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
		SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
		SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;

		break;
	case 2:
		SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
		SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
		SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;

		// SystemParaMenu2.SFDDE = 1; //M3/GD//JL
		break;
	case 3:
		SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
		SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
		SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;
		break;
	case 4:
		SystemParaMenu2.INT_C = SystemParaMenu2.offChange.INT_C;
		SystemParaMenu2.INT_T = SystemParaMenu2.offChange.INT_T;
		SystemParaMenu2.OOC_Par = SystemParaMenu2.offChange.OOC_Par;
		break;
	}

	if(SystemParaMenu2.INT_T>=77){
		SystemParaMenu2.INT_T = 76;
	}

	//配置生效
	planck640c_write_gos(SystemParaMenu2.OOC_Par);
 	planck640c_write_cint(SystemParaMenu2.INT_C);
	planck640c_write_tint(SystemParaMenu2.INT_T);
	planck640c_write_rst_data(77-SystemParaMenu2.INT_T);

	detPara.lastFlag = detPara.statusFlag;
	ts_cpl_fix_msleep(500);
	ISP_cut();
#endif
*/	
}



static ts_s32 i2C0_CH = -1;
/* Sensor I2C 容错控制 */
static ts_u08 sensor_retry_cnt = 0;
static ts_u08 sensor_last_sel  = 0xFE;   /* 记录上一次的 sensor 选择 */
static ts_u08 MotorSen_retry_cnt = 0; 
#define SENSOR_RETRY_MAX   15

/** @note I2C0初始化 */
ts_void Sensor_Init(void)
{
	ts_u16 TemVal;
	ts_s32 s32_ret;
	ts_u08 write_addr;
	ts_u08 read_addr;

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
	/* sensor 选择发生变化，重置重试计数 */
	if (sensor_last_sel != SystemParaMenu2.sensor) {
		sensor_retry_cnt = 0;
		sensor_last_sel  = SystemParaMenu2.sensor;
	}

	if(i2C0_CH == -1) {
		i2C0_CH = ts_lib_i2c_open("/dev/i2c0");
		vision_log_debug("Open IIC file Success! (%d)\n", i2C0_CH);
	}
	if(i2C0_CH >= 0){
		if(SystemParaMenu2.sensor) {
			SlaveAddressByte1 = 0x49;
			/* 7位地址转换成8位 */
			write_addr = (SlaveAddressByte1 << 1) | 0;
			read_addr = (SlaveAddressByte1 << 1) | 1;
			s32_ret = ts_lib_i2c_ra08d16_writew(i2C0_CH,write_addr, 0x01, 0xC7);	// 0x49 是国产
		}
		else{
			SlaveAddressByte1 = 0x48;
			/* 7位地址转换成8位 */
			write_addr = (SlaveAddressByte1 << 1) | 0;
			read_addr = (SlaveAddressByte1 << 1) | 1;
			s32_ret = ts_lib_i2c_ra08d16_writew(i2C0_CH,write_addr, 0x01, 0x2134);	// 0x48 进口tmp117
			ts_cpl_fix_msleep(30);
			ts_lib_i2c_ra08d16_writew(i2C0_CH,write_addr, 0x00, 0x0);
		}

		if (s32_ret != TS_OK) {
			vision_log_debug("i2c0_Sensor write error\n");
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
        	return;
		}
		ts_cpl_fix_msleep(20);
		s32_ret = ts_lib_i2c_ra08d16_readw(i2C0_CH,read_addr, 0x00, (ts_u16 *)&TemVal);
		if (s32_ret != TS_OK) {
			vision_log_debug("i2c0_Sensor read error\n");
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
			return;
		}
		sensor_retry_cnt = 0;
        vision_log_debug("i2c0_SensorInit Success! Addr[%#x] : %#x\n", SlaveAddressByte1, TemVal);
		
	}
	else {
		vision_log_debug(" Open i2C0 file Fail! \n");		
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
}

ts_void MotorSen_Init(void)
{
	ts_s32 s32_ret;
	ts_u08 write_addr;
	ts_u08 read_addr;
	ts_u16 TemVal;

	if(SystemParaMenu2.sensor){
		vision_log_notice("Motor sensor address conflict and return!\n");
		return;
	}
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
	// if(i2C0_CH == -1) {
	// 	i2C0_CH = ts_lib_i2c_open("/dev/i2c0");
	// 	vision_log_debug("Open IIC file Success! (%d)\n", i2C0_CH);
	// }
	// if(i2C0_CH >= 0){
		SlaveAddressByte4 = 0x49;
		write_addr = (SlaveAddressByte4 << 1) | 0;
		read_addr = (SlaveAddressByte4 << 1) | 1;
		s32_ret = ts_lib_i2c_ra08d16_writew(i2C0_CH,write_addr, 0x01, 0xC7); //	
		if (s32_ret != TS_OK) {
			vision_log_debug("i2c0_MotorSen write error\n");
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
			return;
		}
		ts_cpl_fix_msleep(20);
		s32_ret = ts_lib_i2c_ra08d16_readw(i2C0_CH,read_addr,0x00,(ts_u16 *)&TemVal);
		if (s32_ret != TS_OK){
			vision_log_debug("i2c0_MotorSensor read error!\n");
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
			return;
		}
		vision_log_debug("i2c0_MotorSenInit Success! Addr[%#x] \n", SlaveAddressByte3);
		
	// }
	// else {
	// 	vision_log_debug(" Open i2C0 file Fail! \n");		
	// }
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
}

ts_u16 Device_I2c0_Data_Get(ts_u08 addr_type, ts_u08 addr7)
{
	ts_s32 s32ret;
	ts_u16 Data = 0;
	ts_u08 read_addr = (addr7 << 1) | 1;

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
	/* sensor 选择发生变化，重置重试次数 */
	if (sensor_last_sel != SystemParaMenu2.sensor) {
		sensor_retry_cnt = 0;
		sensor_last_sel  = SystemParaMenu2.sensor;
	}

	/* 超过容错次数，直接返回，不打 log（解决卡顿） */
	if (sensor_retry_cnt >= SENSOR_RETRY_MAX) {
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
		return 0xFFFF;
	}

	if(i2C0_CH == -1){
		i2C0_CH = ts_lib_i2c_open("/dev/i2c0");
		vision_log_debug("Open IIC file Success! (%d)\n", i2C0_CH);
	}
	if(i2C0_CH >= 0){
		switch (addr_type)
		{
			case SensorAddr:
				s32ret = ts_lib_i2c_ra08d16_readw(i2C0_CH,read_addr,0x00,(ts_u16 *)&Data);
				if (s32ret != TS_OK) {
					sensor_retry_cnt++;
					vision_log_debug("i2c0_Sensor read error (%d), retry=%d/%d\n", s32ret, sensor_retry_cnt, SENSOR_RETRY_MAX);
					ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
					return 0xFFFF;				
				}
				sensor_retry_cnt = 0;
				// vision_log_debug("i2c0_Sensor read_addr[%#x] : %#x\n", read_addr, Data);
				break;
			case MCP3221_Addr:
				s32ret = ts_lib_i2c_ra08d16_readw(i2C0_CH,read_addr,0x00,(ts_u16 *)&Data);
				if (s32ret != TS_OK) {
					vision_log_debug("i2c0_MCP3221 read error\n");
					ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
					return 0xFFFF;
				}
				// vision_log_debug("i2c0_MCP3221 read_addr[%#x] : %#x\n", read_addr, Data);
				break;
			case MotorSen_Addr:
				/* 超过容错次数，直接返回，不打 log（解决卡顿） */
				if (MotorSen_retry_cnt >= SENSOR_RETRY_MAX) {
					ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
					return 0xFFFF;
				}
				s32ret = ts_lib_i2c_ra08d16_readw(i2C0_CH,read_addr,0x00,(ts_u16 *)&Data);
				if (s32ret != TS_OK){
					MotorSen_retry_cnt++;
					vision_log_debug("i2c0_MotorSen read error cnt=%d\n",MotorSen_retry_cnt);
					ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
					return 0xFFFF;}
					MotorSen_retry_cnt = 0;
				break;
			default:ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);return 0xFFFF;
		}
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
		return Data;
	}
	sensor_retry_cnt++;
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_tmp117_i2c_mutex);
	return 0xFFFF;
}
