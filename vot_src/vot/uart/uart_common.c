/*
 * uart_common.c
 *
 *  Created on: 2023年10月17日
 *      Author: yan
 */

#include "uart_common.h"

u16 DistanceMeasurementData;
u8 ShutterFlag;
u8 ZhongLinShutterFlag = 0;
u16 ZhongLinTime = 0;
u8 ZhongLinModeFlag = 0;
/*******************测距模块协议*****************************/

//控制指令55 AA 01 XX XX XX XX XX
//     55 AA 02 XX XX XX XX XX
//     55 AA 03 XX XX XX XX XX
void UartDistanceMeasurement(void){
#if SUPPORT_MEASURE_UART_PROTOCOL
	if((com_data.buff[0] == Measure_command_head1)&&(com_data.buff[1] == Measure_command_head2))
	{
		if(com_data.buff[2] == 0x01)
		{
	//55 AA 88 FF FF FF FF 84  单次测距
		send_data.rx_buff[0]=Measure_command_head1;
		send_data.rx_buff[1]=Measure_command_head2;
	    send_data.rx_buff[2]=0x88;
	    send_data.rx_buff[3]=0xFF;
		send_data.rx_buff[4]=0xFF;
		send_data.rx_buff[5]=0xFF;
		send_data.rx_buff[6]=0xFF;
		send_data.rx_buff[7]=0x84;
		}
		else if(com_data.buff[2] == 0x02)
		{
	//55 AA 89 FF FF FF FF 85  连续测距
		send_data.rx_buff[0]=Measure_command_head1;
		send_data.rx_buff[1]=Measure_command_head2;
		send_data.rx_buff[2]=0x89;
		send_data.rx_buff[3]=0xFF;
		send_data.rx_buff[4]=0xFF;
		send_data.rx_buff[5]=0xFF;
		send_data.rx_buff[6]=0xFF;
		send_data.rx_buff[7]=0x85;
		}
		else if(com_data.buff[2] == 0x03)
		{
	//55 AA 8E FF FF FF FF 8A  停止测距
		send_data.rx_buff[0]=Measure_command_head1;
		send_data.rx_buff[1]=Measure_command_head2;
		send_data.rx_buff[2]=0x8E;
		send_data.rx_buff[3]=0xFF;
		send_data.rx_buff[4]=0xFF;
		send_data.rx_buff[5]=0xFF;
		send_data.rx_buff[6]=0xFF;
		send_data.rx_buff[7]=0x8A;
		}
		else//测距模块回传信息  数据返回以 16 进制返回， 所有数据结果将真实数据乘 10 输出
		{

			if((com_data.buff[2] == 0x88)||(com_data.buff[2] == 0x89)){
				if(com_data.buff[3] == 0x01){
					DistanceMeasurementData = ((u16)com_data.buff[5])|com_data.buff[6];
				}
			}
//			for(int i = 0;i<8;i++)
//			{
//				bsp_printf("%c",com_data.buff[i]);
//			    display_u8number(X_START,Y_START+416,DistanceMeasurementData,4,(UsermenuParameter.FontColor+FONT_COLOR));
//				Display_Hex16bit(X_START,Y_START+416,DistanceMeasurementData,4,(UsermenuParameter.FontColor+FONT_COLOR));
//				delay_1s(5);
//				menu_clear_oneline(X_START,25*16,Y_START+416);
//			}
//				bsp_printf("\n");

		}
	}

	memset(com_data.buff,0,sizeof(com_data.buff));
	for(int i = 0;i<8;i++)
	{
	 bsp_printf("%c",send_data.rx_buff[i]);
	}
	 bsp_printf("\n");

	 memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

/*************************************************************************/

/*********************以下协议为客户要求，注释为客户要求*******************************/

//上电自检串口传输
void UartPoweronselfcheck(void){
	if((com_data.buff[0] == 0xBB)&&(com_data.buff[1] == 0xEE)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0x01)&&(com_data.buff[4] == 0x01))
	{
         send_data.rx_buff[0]=0xEE;
         send_data.rx_buff[1]=0xBB;
         send_data.rx_buff[2]=0x09;
         send_data.rx_buff[4]=0x0E;//软件配置项编码（待完善）
         send_data.rx_buff[5]=0x00;//0为初样机 1为正样机 2为定型试验 3为设计定型（待完善）
         send_data.rx_buff[6]=0x00;//初始值为0 每次更改增加1（软件更改版本号）（待完善）
         send_data.rx_buff[7]=0x00;//初始值为0 每次更改增加1（软件修订版本号）（待完善）
         send_data.rx_buff[8]=0x00;//年（两行）（待完善）
         send_data.rx_buff[9]=0x00;
         send_data.rx_buff[10]=0x00;//月（待完善）
         send_data.rx_buff[11]=0x00;//日（待完善）
         send_data.rx_buff[12]=0x00;
         send_data.rx_buff[3]=check1 + check2 + check3 +check4;//自检结果（已完成）
	}
	for(int i = 0;i<13;i++)
	{
	bsp_printf("%c",send_data.rx_buff[i]);
	}
	bsp_printf("\n");
}



//非制冷红外机芯串口协议

void UartXA338(void){
#ifdef XA338_UART_DATA
	if(com_data.buff[0] == 0xAA){
	  if((com_data.buff[com_data.buff[1]+2] == 0xEB) && (com_data.buff[com_data.buff[1]+3] == 0xAA)){
		if((com_data.buff[1] == 0x08)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0xAD)&&(com_data.buff[4] == 0x01))
		{
			if(com_data.buff[5] == 0x00)//黑热
			{
				UsermenuParameter.Palette = 1;
				load_palette(UsermenuParameter.Palette);
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function01,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			}
			else if(com_data.buff[5] == 0x01)//白热
			{
				UsermenuParameter.Palette = 1;
				load_palette(UsermenuParameter.Palette);
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function01,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			}
		}
		else if((com_data.buff[1] == 0x08)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0xAC)&&(com_data.buff[4] == 0x01))
		{
			if(com_data.buff[5] == 0x00)//1倍
			{
				UsermenuParameter.Enlarge = 1;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
			}
			else if(com_data.buff[5] == 0x01)//2倍
			{
				UsermenuParameter.Enlarge = 2;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
			}
		}
		else if((com_data.buff[1] == 0x05)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0x01)&&(com_data.buff[4] == 0x01))
		{
			if(com_data.buff[5] == 0x44)//手动校正（待完善）
			{

			}
			else if(com_data.buff[5] == 0x00)//禁止自动校正（待完善）
			{

			}
			else if(com_data.buff[5] == 0x01)//开启自动校正（待完善）
			{

			}
		}
		else if((com_data.buff[1] == 0x04)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0x11))
		{
			if(com_data.buff[4] == 0x01)//打开快门（待完善）
			{

			}
			else if(com_data.buff[4] == 0x00)//关闭快门（待完善）
			{

			}
		}
		else if((com_data.buff[1] == 0x08)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0xB3)&&(com_data.buff[4] == 0x01))
		{
			if(com_data.buff[5] == 0x00)//关棋盘格（待完善）
			{

		    }
			else if(com_data.buff[5] == 0x01)//开棋盘格（待完善）
			{

			}
		}
		else if((com_data.buff[1] == 0x08)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0xB4)&&(com_data.buff[4] == 0x01)&&(com_data.buff[5] == 0x01))//自检
		{

		}
		else if((com_data.buff[1] == 0x08)&&(com_data.buff[2] == 0x01)&&(com_data.buff[3] == 0xB5)&&(com_data.buff[4] == 0x01)&&(com_data.buff[5] == 0x01))//状态查询命令
		{
			send_data.rx_buff[0]=0xAA;
		    send_data.rx_buff[1]=0x0B;
			send_data.rx_buff[2]=0x00;//DDE档位 00手动 01档位1 02档位2 以此类推（待完善）
			send_data.rx_buff[3]=0x00;//亮度 0~255（待完善）
		    send_data.rx_buff[4]=0x0E;//对比度 0~255（待完善）
			send_data.rx_buff[5]=0x00;//变倍 01一倍 02二倍（待完善）
			send_data.rx_buff[6]=0x00;//极性状态 00黑热 01白热（待完善）
			send_data.rx_buff[7]=0x00;//校正状态 00关闭自动校正 01开启（待完善）
		    send_data.rx_buff[8]=0x00;//TEC状态 00禁止TEC状态 01允许TEC自动切换（待完善）
		    send_data.rx_buff[9]=0x00;//积分时间   先高后低（待完善）
			send_data.rx_buff[10]=0x00;
			send_data.rx_buff[11]=0x00;//阵列均值   先高后低（待完善）
		    send_data.rx_buff[12]=0x00;
		    send_data.rx_buff[13]=0x00;//FPA温度     先高后低 比例尺100（待完善）
		    send_data.rx_buff[14]=0x00;
		    send_data.rx_buff[15]=0x00;//SN码   以ASCII码形式表示（待完善）
		    send_data.rx_buff[16]=0x00;
		    send_data.rx_buff[17]=0x00;
		    send_data.rx_buff[18]=0x00;
		    send_data.rx_buff[19]=0x00;
		    send_data.rx_buff[20]=0x00;
		    send_data.rx_buff[21]=0x00;
		    send_data.rx_buff[22]=0x00;
		    send_data.rx_buff[23]=0x00;
		   	send_data.rx_buff[24]=0x00;//校验和
		   	send_data.rx_buff[25]=0xCB;
		   	for(int i = 0;i<26;i++)
		   	{
		   		 bsp_printf("%c",send_data.rx_buff[i]);
		   	}
		}
	  }
	}

	memset(com_data.buff,0,sizeof(com_data.buff));
	memset(send_data.rx_buff,0,sizeof(send_data.rx_buff));
#endif
}


/***************中波制冷项目*********************************/
void UartRefrigeration(void){
#if SUPPORT_Refrigeration_UART_PROTOCOL
	if(com_data.buff[3] == Refrigeration_command_LenL && com_data.buff[4] == Refrigeration_command_LenH &&\
		com_data.buff[5] ==	Refrigeration_command_write && com_data.buff[6] == Refrigeration_data_LenL && \
		com_data.buff[7] == Refrigeration_data_LenH && com_data.buff[9] == Refrigeration_command_addressH){
		if(Refrigeration_Data_Verify(&com_data.buff[3]) == com_data.buff[14]){
			switch(com_data.buff[8]){
			default:break;
			case 0x01://切换参数组1

				break;
			case 0x02://切换参数组2

				break;
			case 0x03://切换参数组3

				break;
			case 0x04://切换参数组4

				break;
			case 0x05://切换参数组5

				break;
			case 0x06://切换参数组6

				break;
			case 0x07://切换参数组7

				break;
			case 0x08://切换参数组8

				break;

			case 0x11://切换用户组1

				break;
			case 0x12://切换用户组2

				break;
			case 0x13://切换用户组3

				break;

			case 0x20://设置积分时间

				break;

			case 0x30://读取积分时间
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x00;
				send_data.rx_buff[13] = 0x00;
				break;

			case 0x40://读取均值
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x00;
				send_data.rx_buff[13] = 0x00;
				break;
			case 0x41://固定帧频模式

				break;
			case 0x42://设置帧频为25HZ

				break;
			case 0x43://设置帧频为50HZ

				break;
			case 0x44://设置帧频为100HZ

				break;
			case 0x45://外触发模式

				break;

			case 0x50://黑热模式
				UsermenuParameter.Palette = 0x01;
				load_palette(UsermenuParameter.Palette);
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
				break;
			case 0x51://白热模式
				UsermenuParameter.Palette = 0x00;
				load_palette(UsermenuParameter.Palette);
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
				break;
			case 0x52://图像翻转

				break;
			case 0x53://图像镜像

				break;
			case 0x54://ROI（兴趣区域左上）

				break;
			case 0x55://ROI（兴趣区域右下）

				break;

			case 0x60://输出原始图
				SystemParaMenu2.AlgorithmSW= 0x01;
				AlgorithmSW(SystemParaMenu2.AlgorithmSW);
				break;
			case 0x61://输出NUC图
				SystemParaMenu2.AlgorithmSW= 0x00;
				AlgorithmSW(SystemParaMenu2.AlgorithmSW);
				break;
			case 0x62://输出8位图
				SystemParaMenu2.AlgorithmSW= 0x00;
				AlgorithmSW(SystemParaMenu2.AlgorithmSW);
				break;

			case 0x70://单点校正

				break;
			case 0x71://两点校正（低温）

				break;
			case 0x72://两点校正（高温）

				break;
			case 0x73://坏点校正（设置阈值）

				break;
			case 0x74://坏点校正

				break;

			case 0x80://保存参数
				SaveTestmenuParameter(1);
				break;
			case 0x81://保存用户校准参数
				SaveTestmenuParameter(1);
				break;

			case 0x90://读取焦平面温度
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x00;
				send_data.rx_buff[13] = 0x00;
				break;
			}
			Return_Refrigeration_Message();
		}
	}
#endif
}


void Return_Refrigeration_Message(void){
#if SUPPORT_Refrigeration_UART_PROTOCOL
	send_data.rx_buff[0] = Refrigeration_command_head1;
	send_data.rx_buff[1] = Refrigeration_command_head2;
	send_data.rx_buff[2] = Refrigeration_command_head3;
	send_data.rx_buff[3] = Refrigeration_command_LenL;
	send_data.rx_buff[4] = Refrigeration_command_LenH;
	send_data.rx_buff[5] = Refrigeration_command_read;
	send_data.rx_buff[6] = Refrigeration_data_LenL;
	send_data.rx_buff[7] = Refrigeration_data_LenH;
	send_data.rx_buff[8] = com_data.buff[8];
	send_data.rx_buff[9] = Refrigeration_command_addressH;
	send_data.rx_buff[14] = Refrigeration_Data_Verify(&send_data.rx_buff[3]);
	for(u8 i = 0; i < send_data.rx_buff[3]+6; i++){
		bsp_printf("%c",send_data.rx_buff[i]);
	}
    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

u8 Refrigeration_Data_Verify(u8 *Databuf){
	u8 Verify_Sum = 0;
#if SUPPORT_Refrigeration_UART_PROTOCOL
	for(u8 i = 0 ;i < Databuf[0] + 2 ;i++){
		Verify_Sum += Databuf[i];
    }
#endif
	return Verify_Sum;
}

/******************************************************************/


/**************************寰宇项目*******************************/
void UartHuanYu(void){
#if SUPPORT_HuanYu_UART_PROTOCOL
	if(HuanYu_Data_Verify(&com_data.buff[2],com_data.buff[1]) == com_data.buff[com_data.buff[1]+2]){
		switch(com_data.buff[3]){
		default:break;
/*		case 0x00:
 *			send_data.rx_buff[1] = 0x15;

			break;*/
		case 0x00://状态查询
			send_data.rx_buff[1] = 0x15;

			send_data.rx_buff[2] = 0x00; //2,3,4,5 设备编号
			send_data.rx_buff[6] = 0x06; //探测器型号  热像分辨率 6-640, 3-384 ,12-1280
			send_data.rx_buff[7] = Year; //7,8,9 版本号 --xx年x月xx日 xx版本
			send_data.rx_buff[8] =((Month&0x0F)<<4)|((Day>>4)&0x0F);
			send_data.rx_buff[9] = ((Day&0x0F)<<4)|(Version_Num&0x0F);
			send_data.rx_buff[10] = UsermenuParameter.Palette; //10 bit[3:0] 色彩模式
			if(UsermenuParameter.Palette ==0x01)		send_data.rx_buff[11] = (SystemParaMenu2.SFilter&0x1F)|(0<<5);
			else 																send_data.rx_buff[11] = (SystemParaMenu2.SFilter&0x1F)|(1<<5); //11 bit[5]极性 bit[4:0]滤波等级
			send_data.rx_buff[12] = SystemParaMenu2.Ctrs_M; //12 对比度
			send_data.rx_buff[13] = SystemParaMenu2.Bri_M; //13 亮度
			send_data.rx_buff[14] = SystemParaMenu2.DDE_Level ; //DDE等级
			send_data.rx_buff[15] =  (UsermenuParameter.Mirror&0x07)|((SystemParaMenu2.Gamma&0x0F)<<3); //bit[6:3] gamma等级 bit[1:0]图像反转模式
			send_data.rx_buff[16] = SystemParaMenu2.TFilter; //3D降噪
			send_data.rx_buff[17] = UsermenuParameter.HY_ShutterTime1/2; //挡片时间 1
			send_data.rx_buff[18] = UsermenuParameter.HY_ShutterTime2/2; //挡片时间 2
			send_data.rx_buff[19] = UsermenuParameter.HY_ShutterTime3/2; //挡片时间 3
			send_data.rx_buff[20] = UsermenuParameter.Enlarge; //电子放大
			send_data.rx_buff[21] = 0x00; //合成图像

			Return_HuanYu_Message();
			break;

		case 0x05:// 极性模式设置
			if(com_data.buff[4] == 0x0F){
				UsermenuParameter.Palette = 0x01;
			}else if(com_data.buff[4] == 0x00){
				UsermenuParameter.Palette = 0x00;
			}
			load_palette(UsermenuParameter.Palette);
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			break;

		case 0x07://自动校正
			if(com_data.buff[4] == 0x0F){
				UsermenuParameter.AutoShutter = 0x03;
			}else if(com_data.buff[4] == 0x00){
				UsermenuParameter.AutoShutter = 0x00;
			}
			ShutterTimer = 0;
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffcffff)|(UsermenuParameter.AutoShutter<<16)));
//			Write_UsermenuParameterToFlash();
			Int_Shutter_pro();
			break;

		case 0x08://电子放大
#ifndef SUPPORT_CONTINUOUS_ZOOM
			if(com_data.buff[4] == 0x00){
				UsermenuParameter.Enlarge = 0x00;
			}else if(com_data.buff[4] == 0x0F){
				UsermenuParameter.Enlarge = 0x01;
			}else if(com_data.buff[4] == 0x03){
				UsermenuParameter.Enlarge = 0x02;
			}
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#else  //具体逻辑待测试调整
			if(com_data.buff[4] == 0x00){
				UsermenuParameter.Enlarge = 0x00;
			}else if(com_data.buff[4] == 0x0F){  //X2.0
				UsermenuParameter.Enlarge = 0x14;
			}else if(com_data.buff[4] == 0x03){  //X4.0
				UsermenuParameter.Enlarge = 0x28;
			}
			APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
		#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
		if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
		{
			APB3_REGW_ZOOM_Scaler_XY((u16)(1279-(UsermenuParameter.Enlarge-10)*8),(u16)(1023-(UsermenuParameter.Enlarge-10)*6));
			APB3_REGW_ZOOM_XY((u16)((1279-(UsermenuParameter.Enlarge-10)*8)*16384/1280),(u16)((1023-(UsermenuParameter.Enlarge-10)*6)*16384/1024));
		}
		else
		{
			APB3_REGW_ZOOM_Scaler_XY((u16)(639-(UsermenuParameter.Enlarge-10)*8),(u16)(511-(UsermenuParameter.Enlarge-10)*6));
			APB3_REGW_ZOOM_XY((u16)((639-(UsermenuParameter.Enlarge-10)*8)*16384/640),(u16)((511-(UsermenuParameter.Enlarge-10)*6)*16384/512));
		}
		
		#endif
#endif
//			Write_UsermenuParameterToFlash();
			break;

		case 0x09://视频对比度设置
			if(UsermenuParameter.ImageAdjust == 0){
				if(UsermenuParameter.ImageMode == 0){
					if(com_data.buff[4] > 127){
						com_data.buff[4] = 0x7f;
					}
					SystemParaMenu2.Ctrs_M = com_data.buff[4];
					APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
				}
			}
			break;

		case 0x0A://视频亮度设置
			if(UsermenuParameter.ImageAdjust == 0){
				if(UsermenuParameter.ImageMode == 0){
					if(com_data.buff[4] > 127){
						com_data.buff[4] = 127;
					}
					SystemParaMenu2.Bri_M = com_data.buff[4];
					APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
				}
			}
			break;

		case 0x0B://图像模式设置
			if(com_data.buff[4] == 0x00 || com_data.buff[4] == 0x01){
				if(com_data.buff[4] == 0x00) {       //手动模式
				UsermenuParameter.ImageAdjust = 0;
				UsermenuParameter.ImageMode = 0;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
				}
				else	{   //自动模式
					UsermenuParameter.ImageAdjust = 2;
					UsermenuParameter.ImageMode = 1;
				}
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				ImageAdjustSet(UsermenuParameter.ImageAdjust);  //图像设置
			}
			break;

		case 0x16:{//图像镜像
			u8 MirrorFlag_value;

			MirrorFlag_value = UsermenuParameter.Mirror;
			if(com_data.buff[4] == 0x00 || com_data.buff[4] == 0x03){
				UsermenuParameter.Mirror = com_data.buff[4];
			}else if(com_data.buff[4] == 0x01){
				UsermenuParameter.Mirror = 0x02;
			}else if(com_data.buff[4] == 0x02){
				UsermenuParameter.Mirror = 0x01;
			}
			SetFreeze_img(ON);
			delay_1ms(20);
//			if(MirrorFlag_value < UsermenuParameter.Mirror) 	    MirrorBadSppot(1);
//			else if(MirrorFlag_value > UsermenuParameter.Mirror)  	MirrorBadSppot(0);
			MirrorBadRecover(MirrorFlag_value); //镜像复原
			MirrorBadSppotInit(); //镜像翻转
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffcff)|(UsermenuParameter.Mirror<<8)));
			SetFreeze_img(OFF);

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
		//	bsp_printf("SetBaking_SW  :%d,addr_val:%x,addr_all:%x,addr_all2:%x\n",systemPar.TFilter,Readtest(),ReadtestALL(),ReadtestALL2());
		//	delay_1s(1);
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
		//	bsp_printf("Shutter Complete! time:(%d:%d) \n",systemPar.Time_M,systemPar.Time_S);
			APB3_REGW_Shutter_SW(OFF);
			break;
		}

		case 0x1B://自动打死点

			break;

#ifdef  FunctionAutoFocus
		case 0x34://自动聚焦
			FocusParaInit();
			FocusParameters.FocusFlag = 1;
			break;

		case 0x35://聚焦+ 单步
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,5);
			delay_1ms(50);
			break;

		case 0x36://聚焦- 单步
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
			delay_1ms(50);
			break;

		case 0x37://聚焦++ 大步
			FocusMotorOperate(NormalState,15,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,5);
			delay_1ms(50);
			break;

		case 0x38://聚焦-- 大步
			FocusMotorOperate(NormalState,15,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
			delay_1ms(50);
			break;

		case 0x39://变倍+ 单步
			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;

		case 0x3A://变倍- 单步
			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;

		case 0x3B://变倍++ 大步
			ZoomMotorOperate(NormalState,15,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;

		case 0x3C://变倍-- 大步
			ZoomMotorOperate(NormalState,15,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;
#endif

		case 0x32://色彩模式设置
			if(com_data.buff[4]<5){
				UsermenuParameter.Palette = com_data.buff[4];}
			else	{
				UsermenuParameter.Palette = com_data.buff[4]+1;}
			load_palette(UsermenuParameter.Palette);
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			break;

		case 0x4A://3D降噪
			if(com_data.buff[4] >= 0x00 || com_data.buff[4] <= 0x03){
			SystemParaMenu2.TFilter = com_data.buff[4];
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xffffe3ff)|(SystemParaMenu2.TFilter<<10)));
			}
			break;

		case 0x4B://虚拟按键
			if(com_data.buff[4] == 0x01){
				KEY_FE_PRO();
			}else if(com_data.buff[4] == 0x02){
				KEY_FD_PRO();
			}else if(com_data.buff[4] == 0x03){
				KEY_FB_PRO();
			}else if(com_data.buff[4] == 0x04){
				KEY_F7_PRO();
			}else if(com_data.buff[4] == 0x05){
				KEY_EF_PRO();
			}else if(com_data.buff[4] == 0x06){
				Key_ExitUsermenu();
			}
			break;

		case 0x50://挡片切换时间 1   前10分钟，设置1-9
			if(com_data.buff[4] >= 1 || com_data.buff[4] <= 9){
				UsermenuParameter.HY_ShutterTime1 = com_data.buff[4]*2;
//				UsermenuParameter.AutoShutter = UsermenuParameter.HY_ShutterTime1;
			}
			break;

		case 0x51://挡片切换时间 2  前60分钟，设置1-29
			if(com_data.buff[4] >= 1 || com_data.buff[4] <= 29){
				UsermenuParameter.HY_ShutterTime2 = com_data.buff[4]*2;
//				UsermenuParameter.AutoShutter = UsermenuParameter.HY_ShutterTime2;
			}
			break;

		case 0x52://挡片切换时间 3  60分钟后，设置1-255
			if(com_data.buff[4] >= 1 || com_data.buff[4] <= 255){
				UsermenuParameter.HY_ShutterTime3 = com_data.buff[4]*2;
			}
			break;

		case 0x63://机芯序列号

			break;

		case 0x68://增益极值
			if(com_data.buff[4] >= 0x00 && com_data.buff[4] <= 0x7f)
			{
				SystemParaMenu2.GAIN = com_data.buff[4];
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));
			}
			break;

		case 0x6A://伽马
			if(com_data.buff[4] >= 0x00 && com_data.buff[4] <= 0x07)
			{
				SystemParaMenu2.Gamma = com_data.buff[4];
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffe3ffff)|(SystemParaMenu2.Gamma<<18)));
			}
			break;

		case 0x6B://合成图像的比例

			break;

		case 0x6C://背景校正设置
			Ext_Shutter_pro();
			break;

		case 0x72://滤波等级
			if(com_data.buff[4] >= 0x01 && com_data.buff[4] <= 0x07)
			{
				SystemParaMenu2.SFilter = com_data.buff[4];
				APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
			}
			break;

		case 0x77://图像增强等级
			if(com_data.buff[4]>=0x00 && com_data.buff[4]<=0x10)
			{
				SystemParaMenu2.DDE_Level = com_data.buff[4];
				APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level*16-1);
			}
			break;

		case 0x78://手动挡片校正
			if(!Flag_Calibration)
			{
				TShutFun = SystemParaMenu2.ShutFun;
				SystemParaMenu2.ShutFun = 0;
				Int_Shutter_pro();
				SystemParaMenu2.ShutFun = TShutFun;
			}
			else
				Ext_Shutter_pro();
			break;

		case 0x80://系统复位
		ClearSize16X16(32,48,400);
		DisPlayNewStr(32,400,"Initing",4,CharacterColor);

#if(Mirror_DisplayType == 1)
		Menu1ParaInit_flag = 1;
#endif
		UsermenuParameterInit();
		Write_UsermenuParameterToFlash();
		Write_UserParameterToFPGA();

		CommonAreaParameterInit();
		Write_CommonAreaParameterToFlash();
		Write_CommonAreaParameterToFPGA();

		if(UsermenuParameter.Quad_SW)//已经打开了
		{
			drawIrregularPolygonBox(vertices, 0);
			DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		}
		QuadPosInit();
		Write_QuadPosToFlash();

		ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
		ClearSportsInfo();//清除测温点信息
		ClearAreasInfo();//清除测温区域信息
		drawIrregularPolygonBox(vertices, 0);//清除四边形信息
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		Disp_Capture_Info();
		APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,\
							(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
		ClearFont(0,64);
		Templut_Menu2_LoadDefault();
		Write_TempParameterToFlash();
		Write_TempParameterToFPGA();

		Int_Shutter_pro();
		delay_1s(1);

		SelectFontColor();
		APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));
#if(Mirror_DisplayType == 1)
		Menu1ParaInit_flag = 0;
#endif
		delay_1s(1);
		ClearSize16X16(32,48,400);
		DisPlayNewStr(32,400,"Done",4,CharacterColor);
		delay_1s(1);
		ClearSize16X16(32,32,400);
			break;

		case 0x81://参数保存
			ClearSize16X16(32,48,400);
			DisPlayNewStr(32,400,"Saving",4,CharacterColor);
			Write_UsermenuParameterToFlash();
			ImagePartSave_flag = 1; //只保存当前图像模式下的参数值
			Write_CommonAreaParameterToFlash();
			ImagePartSave_flag = 0; //只保存当前图像模式下的参数值
			Write_TempParameterToFlash();//231205新增
			Write_QuadPosToFlash();//保存四边形的位置信息
			delay_1s(1);
			ClearSize16X16(32,48,400);
			DisPlayNewStr(32,400,"Done",4,CharacterColor);
			delay_1s(1);
			ClearSize16X16(32,32,400);
			break;

#ifdef  FunctionAutoFocus
		case 0x90://变倍+ 持续运动开始
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
			break;

		case 0x92://聚焦+ 持续运动开始
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusForward,2);
			break;

		case 0x94://变倍- 持续运动开始
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
			break;

		case 0x96://聚焦- 持续运动开始
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
			break;

		case 0x91://变倍+ 持续运动停止
		case 0x95://变倍- 持续运动停止
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;

		case 0x93://聚焦+ 持续运动停止
		case 0x97://聚焦- 持续运动停止
			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
			delay_1ms(50);
			break;

		case 0xB0://zoom最小边界值

			break;

		case 0xB1://zoom最大边界值

			break;

		case 0xB2://Focus最小边界值

			break;

		case 0xB3://Focus最大边界值

			break;

		case 0xB4://zoom绝对位置

			break;

		case 0xB5://Focus绝对位置

			break;

		case 0xC0://聚焦行程差

			break;

		case 0x10://变倍模式
			SystemParaMenu2.Zoom_Foc_SW = com_data.buff[4];//变倍模式为00时，触发自动聚焦

			break;
#endif

		case 0xE0://镜头状态查询

			break;
		}
	}
#endif

#if SUPPORT_HuanYu_UART_PROTOCOL_NET
	if(HuanYu_Data_Verify(&com_data.buff[2],com_data.buff[1]) == com_data.buff[com_data.buff[1]+2]){
		switch(com_data.buff[3]){
		default:break;
		case 0x0B://场景选择
			if(com_data.buff[4] == 0x00 || com_data.buff[4] == 0x01){
				if(com_data.buff[4] == 0x00) {       //手动模式
				UsermenuParameter.ImageAdjust = 0;
				UsermenuParameter.ImageMode = 0;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
				APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
				}else{   //自动模式
					UsermenuParameter.ImageAdjust = 2;
					UsermenuParameter.ImageMode = 1;
				}
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
				ImageAdjustSet(UsermenuParameter.ImageAdjust);  //图像设置
			}
			break;
		case 0x0C://模式选择（自动场景可调）
			if(UsermenuParameter.ImageAdjust == 0)	//手动调节：只能是线性模式
			{
				return;
			}
			if(com_data.buff[4] >= 0x01 && com_data.buff[4] <= 0x03){
				UsermenuParameter.ImageMode = com_data.buff[4];
				APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
			}
			break;
		case 0x0A://亮度调节（手动场景可调）
			if(UsermenuParameter.ImageAdjust == 0){
				if(UsermenuParameter.ImageMode == 0){
					if(com_data.buff[4] > 127){
						com_data.buff[4] = 127;
					}
					SystemParaMenu2.Bri_M = com_data.buff[4];
					APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
				}
			}
			break;
		case 0x09://对比度调节（手动场景可调）
			if(UsermenuParameter.ImageAdjust == 0){
				if(UsermenuParameter.ImageMode == 0){
					if(com_data.buff[4] > 127){
						com_data.buff[4] = 0x7f;
					}
					SystemParaMenu2.Ctrs_M = com_data.buff[4];
					APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
				}
			}
			break;
		case 0x32://伪彩选择
			Data_u8_Limit(&com_data.buff[4], 0x0D, 0);
			if(com_data.buff[4] <= 4){
				UsermenuParameter.Palette = com_data.buff[4];
			}else{
				UsermenuParameter.Palette = com_data.buff[4] + 1;
			}
			load_palette(UsermenuParameter.Palette);
			APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
			break;
		case 0x08://电子放大
#ifdef SUPPORT_CONTINUOUS_ZOOM
			Data_u8_Limit(&com_data.buff[4], 70, 10);
			UsermenuParameter.Enlarge = com_data.buff[4];
			APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
		if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
		{
			APB3_REGW_ZOOM_Scaler_XY((u16)(1279-(UsermenuParameter.Enlarge-10)*8),(u16)(1023-(UsermenuParameter.Enlarge-10)*6));
			APB3_REGW_ZOOM_XY((u16)((1279-(UsermenuParameter.Enlarge-10)*8)*16384/1280),(u16)((1023-(UsermenuParameter.Enlarge-10)*6)*16384/1024));
		}
		else
		{
			APB3_REGW_ZOOM_Scaler_XY((u16)(639-(UsermenuParameter.Enlarge-10)*8),(u16)(511-(UsermenuParameter.Enlarge-10)*6));
			APB3_REGW_ZOOM_XY((u16)((639-(UsermenuParameter.Enlarge-10)*8)*16384/640),(u16)((511-(UsermenuParameter.Enlarge-10)*6)*16384/512));
		}
			
#endif

			if(UsermenuParameter.Enlarge>10)
			{
				EraseZoomMeg();
				ShowZoom(IR_X_pixel-(UsermenuParameter.Enlarge-10)*4,IR_Y_pixel);
			}else{
				EraseZoomMeg();
			}
#endif
			break;
		case 0xC1://语言选择
			Data_u8_Limit(&com_data.buff[4], 1, 0);
			SelectionMenu.IsEnableEnligh = com_data.buff[4];
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));
			break;
		case 0xC2://字符颜色
			Data_u8_Limit(&com_data.buff[4], 3, 0);
			UsermenuParameter.FontColor = com_data.buff[4];
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffffe7)|(UsermenuParameter.FontColor<<3)));
			SelectFontColor();
			break;
		case 0x16:{//图像镜像
			u8 MirrorFlag_value;

			Data_u8_Limit(&com_data.buff[4], 3, 0);
			MirrorFlag_value = UsermenuParameter.Mirror;
			if(com_data.buff[4] == 0x00 || com_data.buff[4] == 0x03){
				UsermenuParameter.Mirror = com_data.buff[4];
			}else if(com_data.buff[4] == 0x01){
				UsermenuParameter.Mirror = 0x02;
			}else if(com_data.buff[4] == 0x02){
				UsermenuParameter.Mirror = 0x01;
			}
			SetFreeze_img(ON);
			delay_1ms(20);
//			if(MirrorFlag_value < UsermenuParameter.Mirror) 	    MirrorBadSppot(1);
//			else if(MirrorFlag_value > UsermenuParameter.Mirror)  	MirrorBadSppot(0);
			MirrorBadRecover(MirrorFlag_value); //镜像复原
			MirrorBadSppotInit(); //镜像翻转
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffcff)|(UsermenuParameter.Mirror<<8)));
			SetFreeze_img(OFF);

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
		//	bsp_printf("SetBaking_SW  :%d,addr_val:%x,addr_all:%x,addr_all2:%x\n",systemPar.TFilter,Readtest(),ReadtestALL(),ReadtestALL2());
		//	delay_1s(1);
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
		//	bsp_printf("Shutter Complete! time:(%d:%d) \n",systemPar.Time_M,systemPar.Time_S);
			APB3_REGW_Shutter_SW(OFF);
			break;
		}
		case 0x06://手动打档
			if(!Flag_Calibration)
			{
				TShutFun = SystemParaMenu2.ShutFun;
				SystemParaMenu2.ShutFun = 0;
				Int_Shutter_pro();
				SystemParaMenu2.ShutFun = TShutFun;
			}
			else
				Ext_Shutter_pro();
			break;
		case 0x07://自动打档间隔
			Data_u8_Limit(&com_data.buff[4], 0x28, 0);
			ShutterTimer = 0;
			UsermenuParameter.AutoShutter = com_data.buff[4];
			Write_UsermenuParameterToFlash();
			Int_Shutter_pro();
			break;
		case 0x81://保存功能
			ClearSize16X16(32,48,400);
			displayReminderInfo(Save);
			Write_UsermenuParameterToFlash();
			ImagePartSave_flag = 1; //只保存当前图像模式下的参数值
			Write_CommonAreaParameterToFlash();
			ImagePartSave_flag = 0; //只保存当前图像模式下的参数值
			Write_TempParameterToFlash();//231205新增
			Write_QuadPosToFlash();//保存四边形的位置信息
			delay_1s(1);
			ClearSize16X16(32,48,400);
			displayReminderInfo(Done);
			delay_1s(1);
			ClearSize16X16(32,32,400);
			break;
		case 0x80://初始化功能
			ClearSize16X16(32,48,400);
			displayReminderInfo(Init);

	#if(Mirror_DisplayType == 1)
			Menu1ParaInit_flag = 1;
	#endif
			UsermenuParameterInit();
			Write_UsermenuParameterToFlash();
			Write_UserParameterToFPGA();

			CommonAreaParameterInit();
			Write_CommonAreaParameterToFlash();
			Write_CommonAreaParameterToFPGA();

			if(UsermenuParameter.Quad_SW)//已经打开了
			{
				drawIrregularPolygonBox(vertices, 0);
				DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
			}
			QuadPosInit();
			Write_QuadPosToFlash();

			ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
			ClearSportsInfo();//清除测温点信息
			ClearAreasInfo();//清除测温区域信息
			drawIrregularPolygonBox(vertices, 0);//清除四边形信息
			DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
			Disp_Capture_Info();
			APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,\
								(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
			ClearFont(0,64);
			Templut_Menu2_LoadDefault();
			Write_TempParameterToFlash();
			Write_TempParameterToFPGA();

			Int_Shutter_pro();
			delay_1s(1);

			SelectFontColor();
			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));
	#if(Mirror_DisplayType == 1)
			Menu1ParaInit_flag = 0;
	#endif
			delay_1s(1);
			ClearSize16X16(32,48,400);
			displayReminderInfo(Done);
			delay_1s(1);
			ClearSize16X16(32,32,400);
			break;
		case 0x63://查看版本号
			switch(VotParameters.Version_Falg)
			{
				default:break;
				// case 0:	VotParameters.Version_Falg = 1;DisPlaySOCAndFPGAVersion(1);break;
				// case 1:	VotParameters.Version_Falg = 0;DisPlaySOCAndFPGAVersion(2);break;
			}
			break;
		case 0x77://图像增强
			Data_u8_Limit(&com_data.buff[4], 0x10, 0);
			SystemParaMenu2.DDE_Level = com_data.buff[4];
			APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level*16-1);
			break;
		case 0x72://空间滤波
			Data_u8_Limit(&com_data.buff[4], 0x10, 0);
			SystemParaMenu2.SFilter = com_data.buff[4];
			APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
			break;
/*************调焦电机相关***************/
#ifdef  FunctionAutoFocus
		case 0x34://自动聚焦
			if(!SystemParaMenu2.Zoom_Lens){
				FocusParaInit();
				FocusParameters.FocusStepMax = MeasureParameters.LimitStepMax;
				FocusParameters.FocusStepMin = MeasureParameters.LimitStepMin;
				FocusParameters.FocusFlag = 1;
			}else{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;
		case 0x35://单步前进 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			FocusMotorOperate(NormalState,com_data.buff[4],MeasureParameters.focusForward,5);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.focusForward,5);
			delay_1ms(50);
			break;
		case 0x36://单步后退 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			FocusMotorOperate(NormalState,com_data.buff[4],MeasureParameters.focusReversal,5);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.focusReversal,5);
			delay_1ms(50);
			break;
		case 0x92://持续前进 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			FocusMotorOperate(NormalState,com_data.buff[4],MeasureParameters.focusForward,5);
			break;
		case 0x96://持续后退 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			FocusMotorOperate(NormalState,com_data.buff[4],MeasureParameters.focusReversal,5);
			break;
		case 0x93://持续运行停止 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			FocusMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.focusForward,5);
			delay_1ms(50);
			break;
#endif
/*************调焦电机相关***************/
#ifdef FunctionAutoZoom
		case 0x10://变焦后 触发自动聚焦
			Data_u8_Limit(&com_data.buff[4], 1, 0);
			SystemParaMenu2.Zoom_Foc_SW = com_data.buff[4];//变倍模式为00时，触发自动聚焦
			break;
		case 0x39://单步前进 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			ZoomMotorOperate(NormalState,com_data.buff[4],MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.zoomForward,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;
		case 0x3A://单步后退 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			ZoomMotorOperate(NormalState,com_data.buff[4],MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.zoomReversal,5);
			delay_1ms(50);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;
		case 0x90://持续前进 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			ZoomMotorOperate(NormalState,com_data.buff[4],MeasureParameters.zoomForward,2);
			break;
		case 0x94://持续后退 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			ZoomMotorOperate(NormalState,com_data.buff[4],MeasureParameters.zoomReversal,2);
			break;
		case 0x91://持续运行停止 使用步长xx
			Data_u8_Limit(&com_data.buff[4], 0x30, 0);
			ZoomMotorOperate(ForceStopState,com_data.buff[4],MeasureParameters.zoomReversal,5);
			if(!SystemParaMenu2.Zoom_Foc_SW)	 //变倍模式为00时，触发自动聚焦
			{
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}
			break;
#endif
		}
	}
#endif
}

void Return_HuanYu_Message(void){
#if (SUPPORT_HuanYu_UART_PROTOCOL || SUPPORT_HuanYu_UART_PROTOCOL_NET)
	send_data.rx_buff[0] = HuanYu_Command_Head;
	send_data.rx_buff[2] = HuanYu_Command_Address;
	send_data.rx_buff[send_data.rx_buff[1]+3] = HuanYu_Command_End;
	send_data.rx_buff[send_data.rx_buff[1]+2] = HuanYu_Data_Verify(&send_data.rx_buff[2],com_data.buff[1]);
	for(u8 i = 0; i < send_data.rx_buff[1]+4; i++){
		bsp_printf("%c",send_data.rx_buff[i]);
	}
    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

u8 HuanYu_Data_Verify(u8 *Databuf,u8 Len){
	u8 Verify_Sum = 0;
#if (SUPPORT_HuanYu_UART_PROTOCOL || SUPPORT_HuanYu_UART_PROTOCOL_NET)
	for(u8 i = 0 ;i < Len ;i++){
		Verify_Sum += Databuf[i];
    }
#endif
	return Verify_Sum;
}

/*************************************************************/

/****************************中林项目******************************/
//void ConvertDecimalToHex(u8 date,u8 *output_date)
//{
//	u8 i;
//	i= date/16;
//	if(i<10)		output_date[0] =  date/16 + 0x30;
//	else			output_date[0] =  date/16 + 0x37;
//	i = date%16;
//	if(i<10)		output_date[1] =  date%16 + 0x30;
//	else			output_date[1] =  date%16 + 0x37;
//}

void UartZhongLin(void){
#if SUPPORT_ZhongLin_UART_PROTOCOL
	if(ZhongLin_Data_Verify(&com_data.buff[2]) == com_data.buff[com_data.buff[2]+3]){
		Return_ZhongLin_Status(ZhongLin_Status_ReceivedSuccess);
		switch(com_data.buff[3]){
		default:break;
		case ZhongLin_Function_STATUS://0x00
			switch(com_data.buff[4]){
			default:break;
			case ZhongLin_Page_First:{//0x00 //查询程序版本、探测器温度，累计工作时间，快门次数，极限温度
				float K,TEMP;
				u16 TEMP_16;
				u16 temp;
				short ResultTmp;
				if(com_data.buff[5] == 0 && com_data.buff[6] == 0 && com_data.buff[7] == 0){
					send_data.rx_buff[2] = 0x19;//长度
//					send_data.rx_buff[3] = ZhongLin_Function_STATUS;
//					send_data.rx_buff[4] = ZhongLin_Page_First;

					send_data.rx_buff[5] = 0x02;//机芯名称

					send_data.rx_buff[6] = 0x00;//通信对象


					send_data.rx_buff[7] = (APB3_REGR(IO_APB_SLAVE_0_INPUT_FPGA_VERSION,0)>>24)&0xff;   //程序版本号  //年
					send_data.rx_buff[7] = (send_data.rx_buff[7]>>4)*10 + (send_data.rx_buff[7] & 0x0F);
					send_data.rx_buff[8] = (APB3_REGR(IO_APB_SLAVE_0_INPUT_FPGA_VERSION,0)>>16)&0xff;  //月
					send_data.rx_buff[8] = (send_data.rx_buff[8]>>4)*10 + (send_data.rx_buff[8] & 0x0F);
					send_data.rx_buff[9] = (APB3_REGR(IO_APB_SLAVE_0_INPUT_FPGA_VERSION,0)>>8)&0xff;   //日
					send_data.rx_buff[9] = (send_data.rx_buff[9]>>4)*10 + (send_data.rx_buff[9] & 0x0F);

#ifdef SUPPORT_VOT_DETECTOR

					if((u16)READ_TMP112_Temp() != 0x8000)
					{
						ResultTmp = (short)(READ_TMP112_Temp()*100);
					}
					else
					{
						DispSysMenu2.Det_PAR = ReadDETAD();
						if(DispSysMenu2.Det_PAR < 7416) //正
						{
							K = 33;
							TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
							TEMP = TEMP * 100;
							ResultTmp = (short)TEMP;
						}
						else  //负数
						{
							K = 33;
							TEMP = (DispSysMenu2.Det_PAR - 7416)/K;
							TEMP = TEMP * (-100);
							ResultTmp = (short)TEMP;
						}
					}

#else
					if((u16)READ_TMP112_Temp() != 0x8000)
					{
						ResultTmp = (short)(READ_TMP112_Temp()*100);
					}
					else
					{
						DispSysMenu2.Det_PAR = ReadDETAD();
						if(DispSysMenu2.Det_PAR < 7543)
						{ //正
							K = 33.8;
							TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
							TEMP = TEMP * 100;
							ResultTmp = (short)TEMP;
						}else{  //负数
							K = 33.8;
							TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
							TEMP = TEMP * (-100);
							ResultTmp = (short)TEMP;
						}
					}

#endif
					send_data.rx_buff[10] = ResultTmp>>8;      //探测器温度  高8位
					send_data.rx_buff[11] = ResultTmp;         //低8位

					send_data.rx_buff[12] = (u8)UsermenuParameter.SystemTimeCounter>>24;  //累计工作时间 (视频制式)
					send_data.rx_buff[13] = (u8)UsermenuParameter.SystemTimeCounter>>16;
					send_data.rx_buff[14] = (u8)UsermenuParameter.SystemTimeCounter>>8;
					send_data.rx_buff[15] = (u8)UsermenuParameter.SystemTimeCounter&0x000000FF;

					send_data.rx_buff[16] = (u8)UsermenuParameter.ShutterCounter>>24;  //快门次数
					send_data.rx_buff[17] = (u8)UsermenuParameter.ShutterCounter>>16;
					send_data.rx_buff[18] = (u8)UsermenuParameter.ShutterCounter>>8;
					send_data.rx_buff[19] = (u8)UsermenuParameter.ShutterCounter&0x000000FF;

					temp = -40 * 100;
					send_data.rx_buff[20] = temp>>8;  //极限低温 高8位
					send_data.rx_buff[21] = temp;

					temp = 60 * 100;
					send_data.rx_buff[22] = temp>>8;	//极限高温 高8位
					send_data.rx_buff[23] = temp;

					//S/N序列号 年
					send_data.rx_buff[24] = (SystemParaMenu2.MerchineSN[0]-0x30)*10+(SystemParaMenu2.MerchineSN[1]-0x30);
					//月
					send_data.rx_buff[25] = (SystemParaMenu2.MerchineSN[2]-0x30)*10+(SystemParaMenu2.MerchineSN[3]-0x30);
					//编号高8位
					temp =	(SystemParaMenu2.MerchineSN[4]-0x30)*1000+(SystemParaMenu2.MerchineSN[5]-0x30)*100+\
							(SystemParaMenu2.MerchineSN[6]-0x30)*10+(SystemParaMenu2.MerchineSN[7]-0x30);
					send_data.rx_buff[26] = temp >> 8;
					//编号低8位
					send_data.rx_buff[27] = temp;
				}
				break;
			}
			case ZhongLin_Page_Second://0x01

				break;
			case ZhongLin_Page_Third://0x02

				break;
			}
			break;

		case ZhongLin_Function_SETUP://0x01
			switch(com_data.buff[4]){
			default:break;
			case ZhongLin_Page_First://0x00
				switch(com_data.buff[5]){
				default:break;
				case 0x00:          //查询非均匀性校正间隔时间
					send_data.rx_buff[2] = 0x13;//长度
//					send_data.rx_buff[3] = ZhongLin_Function_SETUP;
//					send_data.rx_buff[4] = ZhongLin_Page_First;

					send_data.rx_buff[5] = 0x00;//

					send_data.rx_buff[6] = 0x00;//

					send_data.rx_buff[7] = UsermenuParameter.AutoShutter/2;  //自动补偿时间

					send_data.rx_buff[8] = 0x00;

					send_data.rx_buff[9] = 0x00;  //镜头选择

					send_data.rx_buff[10] = 0x05;      //波特率

					send_data.rx_buff[11] = 0x00;  //预留命令选项
					send_data.rx_buff[12] = 0x00;
					send_data.rx_buff[13] = 0x00;
					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = 0x00;
					send_data.rx_buff[21] = 0x00;
					break;

				case 0x03://自动补偿时间
//					if(com_data.buff[7] >= 0 && com_data.buff[7] <= 0x14)
//					{
						Data_u8_Limit(&com_data.buff[7],0x14,0);
						ShutterTimer = 0;
						UsermenuParameter.AutoShutter = com_data.buff[7] * 2;
						APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffcffff)|(UsermenuParameter.AutoShutter<<16)));
//						Write_UsermenuParameterToFlash();
						ZhongLinShutterFlag = 1;
//						Int_Shutter_pro();
//					}
					break;
				case 0x05://保存设置
					DisplayBackground(32,400,6,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Saving",4,6);
					Write_UsermenuParameterToFlash();
					ImagePartSave_flag = 1; //只保存当前图像模式下的参数值
					Write_CommonAreaParameterToFlash();
					ImagePartSave_flag = 0; //只保存当前图像模式下的参数值
					Write_TempParameterToFlash();//231205新增
					Write_QuadPosToFlash();//保存四边形的位置信息
					Write_TestmenuParameterToFlash();
					delay_1s(1);
					DisplayBackground(32,400,6,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Done",4,6);
					delay_1s(1);
					DisplayBackground(32,400,6,BACKGROUND_LAYER,0);
					break;
				case 0x06://恢复出厂设置
					DisplayBackground(32,400,7,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Init...",4,6);

#if(Mirror_DisplayType == 1)
					Menu1ParaInit_flag = 1;
#endif
					UsermenuParameterInit();
					Write_UsermenuParameterToFlash();
					Write_UserParameterToFPGA();

					CommonAreaParameterInit();
					Write_CommonAreaParameterToFlash();
					Write_CommonAreaParameterToFPGA();

					if(UsermenuParameter.Quad_SW)//已经打开了
					{
						drawIrregularPolygonBox(vertices, 0);
						DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
					}
					QuadPosInit();
					Write_QuadPosToFlash();

					ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
					ClearSportsInfo();//清除测温点信息
					ClearAreasInfo();//清除测温区域信息
					drawIrregularPolygonBox(vertices, 0);//清除四边形信息
					DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
					Disp_Capture_Info();
					APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,\
							(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
					ClearFont(0,64);
					Templut_Menu2_LoadDefault();
					Write_TempParameterToFlash();
					Write_TempParameterToFPGA();

					Int_Shutter_pro();
					delay_1s(1);

					SelectionMenu.IsEnableEnligh = 0;	//中文
					SelectFontColor();
					APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffff8)|0x00));

#if(Mirror_DisplayType == 1)
					Menu1ParaInit_flag = 0;
#endif
					DisplayBackground(32,400,7,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Done",4,6);
					delay_1s(1);
					DisplayBackground(32,400,7,BACKGROUND_LAYER,0);
					break;
				case 0x0A://增益选择
//					Data_u8_Limit(&com_data.buff[7],0x07,0);
//					SystemParaMenu2.INT_C = com_data.buff[7];
//					APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));
//#ifndef	 SUPPORT_VOT_DETECTOR
//					if(SystemParaMenu2.Usermenu_SW <= 1){ //一档测温 1
//						SystemParaMenu2.INT_C_Val[0] = SystemParaMenu2.INT_C ;
//					}else if(SystemParaMenu2.Usermenu_SW == 2){ //二档测温
//						SystemParaMenu2.INT_C_Val[1] = SystemParaMenu2.INT_C;
//					}
//#endif
					break;
				case 0x15://串口透传伺服板

					break;
				}
				break;
			case ZhongLin_Page_Second://0x01

				break;
			case ZhongLin_Page_Third://0x02

				break;
			}
			break;

		case ZhongLin_Function_VIDEO://0x02
			switch(com_data.buff[4]){
			default:break;
			case ZhongLin_Page_First://0x00
				switch(com_data.buff[5]){
				default:break;
				case 0x00:  //查询伪彩
					send_data.rx_buff[2] = 0x13;//长度

					send_data.rx_buff[5] = UsermenuParameter.Mirror;// 镜像

					send_data.rx_buff[6] = UsermenuParameter.Palette;// 伪彩

					send_data.rx_buff[7] = UsermenuParameter.Enlarge;//电子放大

					send_data.rx_buff[8] = SystemParaMenu2.ImgT;//测试图像

					send_data.rx_buff[9] = 0x00;  //预留命令选项
					send_data.rx_buff[10] = 0x00;
					send_data.rx_buff[11] = 0x00;
					send_data.rx_buff[12] = 0x00;
					send_data.rx_buff[13] = 0x00;
					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = 0x00;
					send_data.rx_buff[21] = 0x00;
					break;

				case 0x02://伪彩设置
//					if((com_data.buff[7] >= 0x00)&&(com_data.buff[7] <= 0x0e)){
					Data_u8_Limit(&com_data.buff[7],0x0e,0);
					if(com_data.buff[7] == 0x03){
						com_data.buff[7] = 0x02;
					}else if(com_data.buff[7] == 0x02){
						com_data.buff[7] = 0x03;
					}else if(com_data.buff[7] == 0x05){
						com_data.buff[7] = 0x04;
					}else if(com_data.buff[7] == 0x04){
						com_data.buff[7] = 0x05;
					}
					UsermenuParameter.Palette = com_data.buff[7];
					load_palette(UsermenuParameter.Palette);
					APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
//					}
					break;
				case 0x07://测试图像
					Data_u8_Limit(&com_data.buff[7],1,0);
					SystemParaMenu2.ImgT = com_data.buff[7];
					APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xfffffffb)|(SystemParaMenu2.ImgT<<2)));
					break;
				case 0x11://快门校正
					if(com_data.buff[7] == 1 && com_data.buff[6] == 0){
						ZhongLinShutterFlag = 1;
//						Int_Shutter_pro();
					}
					break;
				}
				break;
			case ZhongLin_Page_Second://0x01
				switch(com_data.buff[5]){
				default:break;
				case 0x00:{//亮度、对比度查询
					u16 temp;
					send_data.rx_buff[2] = 0x13;//长度

					send_data.rx_buff[5] = 0x00;

					if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
						if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
							temp = (u16)((float)SystemParaMenu2.Ctrs_M / 1.27);
							send_data.rx_buff[6] = temp << 8;// 对比度 高8位
							send_data.rx_buff[7] = temp;
						}
					}else{
						temp = (u16)((float)SystemParaMenu2.Ctrs_A / 1.27);
						send_data.rx_buff[6] = temp << 8;// 对比度 高8位
						send_data.rx_buff[7] = temp;
					}

					send_data.rx_buff[8] = 0x00;
					send_data.rx_buff[9] = 0x00;

					send_data.rx_buff[10] = 0x00; //映射范围

					if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
						if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
							temp = (u16)((float)SystemParaMenu2.Bri_M / 1.27);
							send_data.rx_buff[11] = temp << 8;// 亮度 高8位
							send_data.rx_buff[12] = temp;
						}
					}else{
						temp = (u16)((float)SystemParaMenu2.Bri_A / 2.55);
						send_data.rx_buff[11] = temp << 8;// 亮度 高8位
						send_data.rx_buff[12] = temp;
					}

					send_data.rx_buff[13] = 0x00;//预留命令选项
					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = 0x00;
					send_data.rx_buff[21] = 0x00;
					break;
				}


				case 0x02://对比度
					if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
						if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
//							if(com_data.buff[7] > 100){
//								com_data.buff[7] = 100;
//							}
							Data_u8_Limit(&com_data.buff[7],100,0);
							SystemParaMenu2.Ctrs_M = (u8)((float)com_data.buff[7]*1.27);
							APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
						}
					}else{
//						if(com_data.buff[7] > 100){
//							com_data.buff[7] = 100;
//						}
						Data_u8_Limit(&com_data.buff[7],100,0);
						SystemParaMenu2.Ctrs_A = (u8)((float)com_data.buff[7]*1.27);
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_A<<16)));
						if(SystemParaMenu2.OutdoorSel == 1)				SystemParaMenu2.ImageOutdoorM3Para.Ctrs_A = SystemParaMenu2.Ctrs_A;
						else	if(SystemParaMenu2.OutdoorSel == 0)		SystemParaMenu2.ImageOutdoorPara.Ctrs_A = SystemParaMenu2.Ctrs_A;
						else if(SystemParaMenu2.OutdoorSel == 2)		SystemParaMenu2.ImageOutdoorJLPara.Ctrs_A = SystemParaMenu2.Ctrs_A;
					}
					break;
				case 0x03://亮度
					if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
						if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
//							if(com_data.buff[7] > 100){
//								com_data.buff[7] = 100;
//							}
							Data_u8_Limit(&com_data.buff[7],100,0);
							SystemParaMenu2.Bri_M = (u8)((float)com_data.buff[7]*1.27);
							APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
						}
					}else{
//						if(com_data.buff[7] > 100){
//							com_data.buff[7] = 100;
//						}
						Data_u8_Limit(&com_data.buff[7],100,0);
						SystemParaMenu2.Bri_A = (u8)((float)com_data.buff[7]*2.55);
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2,0)&0xffff0000)|(SystemParaMenu2.Bri_A)));
						if(SystemParaMenu2.OutdoorSel == 1)				SystemParaMenu2.ImageOutdoorM3Para.Bri_A = SystemParaMenu2.Bri_A;
						else	if(SystemParaMenu2.OutdoorSel == 0)		SystemParaMenu2.ImageOutdoorPara.Bri_A = SystemParaMenu2.Bri_A;
						else if(SystemParaMenu2.OutdoorSel == 2)		SystemParaMenu2.ImageOutdoorJLPara.Bri_A = SystemParaMenu2.Bri_A;
					}
					break;
				case 0x01://调光模式  //手动 室内  室外
					Data_u8_Limit(&com_data.buff[7],2,0);
					UsermenuParameter.ImageAdjust = com_data.buff[7];
					if(UsermenuParameter.ImageAdjust)
					{
						UsermenuParameter.ImageMode = 1;
						APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
					}
					else//手动：只能是模式0
					{
						UsermenuParameter.ImageMode = 0;
						APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
					}
					ImageAdjustSet(UsermenuParameter.ImageAdjust);  //图像设置
					break;
				case 0x04://调光算法  //线性  室外M3  室外JL
					Data_u8_Limit(&com_data.buff[7],2,0);
					if(com_data.buff[7] == 0x00){
						UsermenuParameter.ImageAdjust = 0;
						UsermenuParameter.ImageMode = 0;
						APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
						ImageAdjustSet(UsermenuParameter.ImageAdjust);
					}else if(com_data.buff[7] == 0x01){
						SystemParaMenu2.OutdoorSel = 1;
						UsermenuParameter.ImageAdjust = 2;
						UsermenuParameter.ImageMode = 1;
						APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
						ImageAdjustSet(UsermenuParameter.ImageAdjust);
					}else if(com_data.buff[7] == 0x02){
						SystemParaMenu2.OutdoorSel = 0;
						UsermenuParameter.ImageAdjust = 2;
						UsermenuParameter.ImageMode = 1;
						APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xfff8ffff)|(UsermenuParameter.ImageMode<<16)));
						ImageAdjustSet(UsermenuParameter.ImageAdjust);
					}
					break;
				case 0x05://抛弃比例

					break;
				}
				break;
			case ZhongLin_Page_Third://0x02
				switch(com_data.buff[5]){
				default:break;
				case 0x00:  //查询增强开关、锐化系数
					send_data.rx_buff[2] = 0x13;//长度

					send_data.rx_buff[5] = 0x00;
					send_data.rx_buff[6] = 0x00;
					send_data.rx_buff[7] = 0x00;

					send_data.rx_buff[8] = SystemParaMenu2.DDE_SW;//细节增益开关

					send_data.rx_buff[9] = 0x00;

					send_data.rx_buff[10] = SystemParaMenu2.DDE_Level; //锐化系数

					send_data.rx_buff[11] = SystemParaMenu2.SFilter;//平滑系数（空间滤波）

					send_data.rx_buff[12] = SystemParaMenu2.DEST_SW;//去竖条系数 (去竖纹)

					send_data.rx_buff[13] = 0x00;//预留命令选项
					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = 0x00;
					send_data.rx_buff[21] = 0x00;
					break;


				case 0x03://细节增益
//					if(com_data.buff[7]>=0x00 && com_data.buff[7]<=0x01){
					Data_u8_Limit(&com_data.buff[7],1,0);
					SystemParaMenu2.DDE_SW = com_data.buff[7];
					APB3_REGW_DDE_SW(SystemParaMenu2.DDE_SW);
//					}
					break;
				case 0x04:{//锐化系数
					u16	DDE_LevelVal;
//					if(com_data.buff[7]>=0x00 && com_data.buff[7]<=0x10){
					Data_u8_Limit(&com_data.buff[7],80,0);
					SystemParaMenu2.DDE_Level = com_data.buff[7]/5;
//					APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level);
//					}
					if(SystemParaMenu2.DDE_Level == 0){
						DDE_LevelVal = 0;
					}else{
						DDE_LevelVal= SystemParaMenu2.DDE_Level*16-1;
					}
					APB3_REGW_DDE_LV(DDE_LevelVal); //传值范围0-255
					if(UsermenuParameter.ImageAdjust ==0){  //手动
						SystemParaMenu2.ImageManualPara.DDELevel = SystemParaMenu2.DDE_Level;
					}else if(UsermenuParameter.ImageAdjust ==1){ //室内
						SystemParaMenu2.ImageIndoorPara.DDELevel = SystemParaMenu2.DDE_Level;
					}else if(UsermenuParameter.ImageAdjust ==2){//室外
						if(SystemParaMenu2.OutdoorSel){
							SystemParaMenu2.ImageOutdoorM3Para.DDELevel = SystemParaMenu2.DDE_Level;
						}else{
							SystemParaMenu2.ImageOutdoorPara.DDELevel = SystemParaMenu2.DDE_Level;
						}
					}
					break;
				}
				case 0x05://去噪等级
//					if(com_data.buff[7]>=0x01 && com_data.buff[7]<=0x07){
					Data_u8_Limit(&com_data.buff[7],1,0);
					SystemParaMenu2.SFilter = 1 + com_data.buff[7];
					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
//					}
					if(UsermenuParameter.ImageAdjust ==0){  //手动
						SystemParaMenu2.ImageManualPara.SFilter = SystemParaMenu2.SFilter;
					}
					else if(UsermenuParameter.ImageAdjust ==1){ //室内
					   SystemParaMenu2.ImageIndoorPara.SFilter = SystemParaMenu2.SFilter;
					}
					else if(UsermenuParameter.ImageAdjust ==2){//室外
						if(SystemParaMenu2.OutdoorSel){
							SystemParaMenu2.ImageOutdoorM3Para.SFilter = SystemParaMenu2.SFilter;
						}else{
							SystemParaMenu2.ImageOutdoorPara.SFilter = SystemParaMenu2.SFilter;
						}
					}
					break;
				}
				break;
			}
			break;

		case ZhongLin_Function_ADVANCE://0xA0
			switch(com_data.buff[4]){
			default:break;
			case ZhongLin_Page_First://0x00
				switch(com_data.buff[5]){
				default:break;
				case 0x00://查询手动控制快门开关状态、防太阳辐射开关状态、防太阳辐射阈值、自动校正坏点开关状态
					send_data.rx_buff[2] = 0x13;//长度

					send_data.rx_buff[5] = 0x00;//光标命令

					send_data.rx_buff[6] = 0x00;
					send_data.rx_buff[7] = 0x00;
					send_data.rx_buff[8] = 0x00;
					send_data.rx_buff[9] = 0x00;

					if(SystemParaMenu2.SFilter == 1){
						send_data.rx_buff[10] = 0x00; //去噪算法开关
					}else{
						send_data.rx_buff[10] = 0x01; //去噪算法开关
					}

					send_data.rx_buff[11] = ShutterFlag;//手动快门开关

					send_data.rx_buff[12] = UsermenuParameter.SunPT_SW;//防太阳辐射算法开关
					send_data.rx_buff[13] = 0x00;//自动校坏点算法开关
					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;//坐标AD值
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = SystemParaMenu2.S_Range>>8;//防太阳辐射阈值
					send_data.rx_buff[21] = SystemParaMenu2.S_Range;
					break;


				case 0x06:{//去噪开关
//					if(com_data.buff[7]>=0x00 && com_data.buff[7]<=0x01){
//					SystemParaMenu2.MedFilt_SW = com_data.buff[7];
//					APB3_REGW_MedFilt_SW(SystemParaMenu2.MedFilt_SW);
//					}
					static u8 temp = 5;
					Data_u8_Limit(&com_data.buff[7],1,0);
					if(com_data.buff[7] == 0x00){
						temp = SystemParaMenu2.SFilter;
						SystemParaMenu2.SFilter = 1;
					}else if(com_data.buff[7] == 0x01){
						SystemParaMenu2.SFilter = temp;
					}
//					SystemParaMenu2.SFilter = com_data.buff[7];
					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
					if(UsermenuParameter.ImageAdjust ==0){  //手动
						SystemParaMenu2.ImageManualPara.SFilter = SystemParaMenu2.SFilter;
					}
					else if(UsermenuParameter.ImageAdjust ==1){ //室内
					   SystemParaMenu2.ImageIndoorPara.SFilter = SystemParaMenu2.SFilter;
					}
					else if(UsermenuParameter.ImageAdjust ==2){//室外
						if(SystemParaMenu2.OutdoorSel){
							SystemParaMenu2.ImageOutdoorM3Para.SFilter = SystemParaMenu2.SFilter;
						}else{
							SystemParaMenu2.ImageOutdoorPara.SFilter = SystemParaMenu2.SFilter;
						}
					}
					break;
				}
				case 0x09://自动校坏点

					break;
				case 0x0A://坐标X值

					break;
				case 0x0B://坐标Y值

					break;
				case 0x0C://手动坏点

					break;
				case 0x0D://保存坏点
					NUC_Save(1);
					break;
				case 0x0E://手动闭合快门
					Data_u8_Limit(&com_data.buff[7],1,0);
					if(!com_data.buff[7]){//取消
						APB3_REGW_Shutter_SW(ON);
						SetFreeze_img(ON);
						delay_1ms(50);
						shutterOpr(OFF);
						delay_1ms(100);
						SetFreeze_img(OFF);
						APB3_REGW_Shutter_SW(OFF);

						ShutterFlag = 1;
					}else{//激活
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

						ShutterFlag = 0;

//						delay_1ms(200);
//						Int_Shutter_pro();
					}
					break;
				case 0x0F://防止太阳辐射算法
					Data_u8_Limit(&com_data.buff[7],1,0);
					UsermenuParameter.SunPT_SW = com_data.buff[7];
					APB3_REGW_SunPT_SW(UsermenuParameter.SunPT_SW);
					break;
				case 0x10:{//防太阳辐射阈值
					u16 temp;
					temp = com_data.buff[6]<<8|com_data.buff[7];
					Data_u16_Limit(&temp,0x3CAF,0);
					SystemParaMenu2.S_Range = temp;
					// APB3_REGW_S_Range(SystemParaMenu2.S_Range - FPGA_Bak_AVG()/2);
					SystemParaMenu2.S_Range0 = SystemParaMenu2.S_Range;
					break;
				}
				}
				break;
			case ZhongLin_Page_Second://0x01

				break;
			case ZhongLin_Page_Third://0x02

				break;
			}
			break;

		case ZhongLin_Function_EXPERT://0xA1
			switch(com_data.buff[4]){
			default:break;
			case ZhongLin_Page_First://0x00
				switch(com_data.buff[5]){
				default:break;
				case 0x00:{ //查询环境温度
					u16 temp;
					float K,TEMP;
					u16 TEMP_16;
					short ResultTmp;
					send_data.rx_buff[2] = 0x13;//长度

#ifdef SUPPORT_VOT_DETECTOR
					DispSysMenu2.Det_PAR = ReadDETAD();
					if(DispSysMenu2.Det_PAR < 7416) //正
					{
						K = 33;
						TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
						TEMP = TEMP * 100;
						ResultTmp = (short)TEMP;
					}
					else  //负数
					{
						K = 33;
						TEMP = (DispSysMenu2.Det_PAR - 7416)/K;
						TEMP = TEMP * (-100);
						ResultTmp = (short)TEMP;
					}
#else
					DispSysMenu2.Det_PAR = ReadDETAD();
					if(DispSysMenu2.Det_PAR < 7543){ //正
						K = 33.8;
						TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
						TEMP = TEMP * 100;
						ResultTmp = (short)TEMP;
					}else{  //负数
						K = 33.8;
						TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
						TEMP = TEMP * (-100);
						ResultTmp = (short)TEMP;
					}
#endif

					send_data.rx_buff[5] = ResultTmp>>8;//焦温值
					send_data.rx_buff[6] = ResultTmp;

					temp = ReadDETAD();
					send_data.rx_buff[7] = temp>>8;//AD值显示  X16
					send_data.rx_buff[8] = temp;

					send_data.rx_buff[9] = 0x00;//VSK档位

					send_data.rx_buff[10] = 0x00;//VSK值
					send_data.rx_buff[11] = 0x00;

					if((u16)READ_TMP117_Temp_ZL() != 0x8000)
					{
						ResultTmp = (short)(READ_TMP117_Temp_ZL()*100);
					}
					else
					{
						ResultTmp = 0;
					}

					send_data.rx_buff[12] = ResultTmp>>8;//环境温度值
					send_data.rx_buff[13] = (u8)(ResultTmp&0x00ff);

					send_data.rx_buff[14] = 0x00;
					send_data.rx_buff[15] = 0x00;
					send_data.rx_buff[16] = 0x00;
					send_data.rx_buff[17] = 0x00;
					send_data.rx_buff[18] = 0x00;
					send_data.rx_buff[19] = 0x00;
					send_data.rx_buff[20] = 0x00;
					send_data.rx_buff[21] = 0x00;
					break;
				}


				case 0x0D://升级程序
					if(com_data.buff[7] == 0x0D){//升级FPGA程序

					}else if(com_data.buff[7] == 0x10){//升级伺服板ARM程序
						send_data.rx_buff[0] = ZhongLin_Servo_Head;
						send_data.rx_buff[1] = 0x10;
						send_data.rx_buff[2] = 0x01;
						send_data.rx_buff[3] = 0x01;
						send_data.rx_buff[4] = 0x04;
						send_data.rx_buff[5] = 0xA3;
						send_data.rx_buff[6] = ZhongLin_Servo_Data_Verify(&send_data.rx_buff[1]);
						for(u8 i = 0; i < 7; i++){
							bsp_printf("%c",send_data.rx_buff[i]);
						}
					    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
					    return;
					}
					break;
				}
				break;
			case ZhongLin_Page_Second:{//0x01 //查询探测器配置参数积分时间
				u16 temp;
				send_data.rx_buff[2] = 0x13;//长度

				send_data.rx_buff[5] = 0x00;
				send_data.rx_buff[6] = 0x00;
				send_data.rx_buff[7] = 0x00;
				send_data.rx_buff[8] = 0x00;
				send_data.rx_buff[9] = 0x00;
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x00;

				send_data.rx_buff[13] = 0x00;//INT_T(0~1024)
				send_data.rx_buff[14] = SystemParaMenu2.INT_T;

				send_data.rx_buff[15] = SystemParaMenu2.INT_C;//GAIN(1~7)

				send_data.rx_buff[16] = 0x00;
				send_data.rx_buff[17] = 0x00;
				send_data.rx_buff[18] = 0x00;

				temp = ReadDETAD();
				send_data.rx_buff[19] = temp>>8;//焦温偏移校正值
				send_data.rx_buff[20] = temp;

				send_data.rx_buff[21] = 0x00;
				break;
			}
			case ZhongLin_Page_Third://0x02

				break;
			}
			break;
		}
		Return_ZhongLin_Message();
	}else{
		Return_ZhongLin_Status(ZhongLin_Status_CalibrationFailure);
	}
#endif
}

void UartZhongLinServo(void){
	char ResTmp;
#if SUPPORT_ZhongLin_UART_PROTOCOL
		switch(com_data.buff[2]<<8|com_data.buff[3]){
		default:break;
		case 0x0040://变倍+
			if(MotorPresetParameters.FollowSwitch){
				MotorPresetParameters.ZoomOperateFlag = 1;
				MotorPresetParameters.FollowFlag = 1;
//				ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomForward,2);
				ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomReversal,2);
			}else{
//				ZoomMotorOperate(NormalState,ZoomParameters.ZoomSpeed,MeasureParameters.zoomForward,2);
				ZoomMotorOperate(NormalState,ZoomParameters.ZoomSpeed,MeasureParameters.zoomReversal,2);
			}
//			delay_1ms(50);
//			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
//			delay_1ms(50);
//			Return_ZhongLin_Servo_Message();
			break;
		case 0x0020://变倍-
			if(MotorPresetParameters.FollowSwitch){
				MotorPresetParameters.ZoomOperateFlag = 1;
				MotorPresetParameters.FollowFlag = 1;
//				ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomReversal,2);
				ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomForward,2);
			}else{
//				ZoomMotorOperate(NormalState,ZoomParameters.ZoomSpeed,MeasureParameters.zoomReversal,2);
				ZoomMotorOperate(NormalState,ZoomParameters.ZoomSpeed,MeasureParameters.zoomForward,2);
			}
//			delay_1ms(50);
//			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
//			delay_1ms(50);
//			Return_ZhongLin_Servo_Message();
			break;
		case 0x0100://调焦+
			FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.focusForward,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,5);
//			delay_1ms(50);
//			Return_ZhongLin_Servo_Message();
			break;
		case 0x0080://调焦-
			FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.zoomReversal,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
//			delay_1ms(50);
//			Return_ZhongLin_Servo_Message();
			break;
		case 0x0000://停止调焦和变倍
			if(MotorPresetParameters.FollowSwitch){
				ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
				delay_1ms(50);
				MotorPresetParameters.ZoomOperateFlag = 0;
			}else{
				FocusParameters.FocusFlag = 0;
				ZoomParameters.ZoomFlag = 0;
	//			ZoomParameters.PresetRightFlag = 0;
	//			ZoomParameters.PresetLeftFlag = 0;
				ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
				delay_1ms(50);
			}

			VoltageSamplePara.SensorTemp = GetSensor();
			VoltageSamplePara.VoltagePos = GetVol(Focus_MCP3221_addr);
			VoltageSamplePara.Position = VoltageSamplePara.VoltagePos;
			break;
		case 0x2500://设置变倍速度
			Data_u8_Limit(&com_data.buff[5],3,0);
			com_data.buff[5] = com_data.buff[5] * 2 + 3;
			ZoomParameters.ZoomSpeed = com_data.buff[5];
			break;
		case 0x2700://设置调焦速度
			Data_u8_Limit(&com_data.buff[5],3,0);
			com_data.buff[5] = com_data.buff[5] * 2 + 5;
			FocusParameters.FocusSpeed = com_data.buff[5];
			break;
		case 0x4F00://跳转变倍位
			ZoomParameters.PresetPos = com_data.buff[4]<<8 | com_data.buff[5];
			ZoomParameters.PresetPos = 4096 - ZoomParameters.PresetPos;
			if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//						ZoomParameters.PresetLeftFlag = 1;
				ZoomParameters.PresetRightFlag = 1;
			}else{
//						ZoomParameters.PresetRightFlag = 1;
				ZoomParameters.PresetLeftFlag = 1;
			}
			break;
		case 0x8700://跳转调焦位
			FocusParameters.PresetPos = com_data.buff[4]<<8 | com_data.buff[5];
			if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
				FocusParameters.PresetRightFlag = 1;
			}else{
				FocusParameters.PresetLeftFlag = 1;
			}
			break;
		case 0x1010://跟焦  设置地址
			if(com_data.buff[4] == 0x30){
				Data_u8_Limit(&com_data.buff[5],2,1);
				if(com_data.buff[5] == 1){
					MotorPresetParameters.FollowSwitch = ON_State;
				}else if(com_data.buff[5] == 2){
					MotorPresetParameters.FollowSwitch = OFF_State;
				}
			}else if(com_data.buff[4] == 0x10){

			}
			break;
		case 0x0F0F:{//位置查询
			u16 Voltage,temp;
			Return_ZhongLin_Servo_Message();
			delay_1ms(50);
			send_data.rx_buff[0] = ZhongLin_Servo_Head;
			send_data.rx_buff[1] = 0x01;
			temp = GetVol(Zoom_MCP3221_addr);
			temp = 4096 - temp;
			send_data.rx_buff[2] = temp>>8;
			send_data.rx_buff[3] = temp;

			temp = GetVol(Focus_MCP3221_addr);
//			Voltage = temp;
//			temp = temp - 113;
//			temp = (u16)((float)temp * 0.093);
//			Voltage = Voltage + temp + 68;
			send_data.rx_buff[4] = temp>>8;
			send_data.rx_buff[5] = temp;

			send_data.rx_buff[6] = ZhongLin_Servo_Data_Verify(&send_data.rx_buff[1]);
			for(u8 i = 0; i < 7; i++){
				bsp_printf("%c",send_data.rx_buff[i]);
			}
		    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
			return;
		}
		case 0x0201://一键聚焦
			if(!ZhongLinModeFlag){
				FocusParaInit();
				FocusParameters.FocusFlag = 1;
			}else{
				PresetPositionParaInit();
				MotorPresetParameters.CalibrationFlag = 1;
				MotorPresetParameters.PresetNum = 0;
			}

//			Return_ZhongLin_Servo_Message();
			break;
		case 0x0101:{
			u16 temp;
			temp = com_data.buff[4]<<8|com_data.buff[5];
			if(temp == 0x0101){//透明通道

			}else if(temp == 0x0102){//ARM升级

			}else if(temp == 0x0103){//升级指令

			}

			break;
		}
		case 0x0400://版本号查询

			break;
		case 0x0F10://温度查询
			Return_ZhongLin_Servo_Message();
			delay_1ms(50);
			send_data.rx_buff[0] = ZhongLin_Servo_Head;
			send_data.rx_buff[1] = 0x00;
			send_data.rx_buff[2] = 0x00;
			if((u16)READ_TMP117_Temp_ZL() != 0x8000)
			{
				ResTmp = (char)(READ_TMP117_Temp_ZL());
			}
			else
			{
				ResTmp = 0;
			}
			send_data.rx_buff[3] = ResTmp;
			send_data.rx_buff[4] = 0;
			send_data.rx_buff[5] = 1;
			send_data.rx_buff[6] = ZhongLin_Servo_Data_Verify(&send_data.rx_buff[1]);

			for(u8 i = 0; i < 7; i++){
				bsp_printf("%c",send_data.rx_buff[i]);
			}
		    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
		    return;
			break;
		case 0x0009://预调模式
			if(com_data.buff[4] == 0 && com_data.buff[5] == 0)
			ZhongLinModeFlag = 1;
			break;
		case 0x000B://正常模式
			if(com_data.buff[4] == 0 && com_data.buff[5] == 0)
			ZhongLinModeFlag = 0;
			break;
		case 0x000E://跟焦+聚焦模式

			break;
		}
		if(com_data.buff[1] == 0){//清晰度评价值
			u32 temp;
			temp = ReadImageSum();
			com_data.buff[2] = temp>>24;
			com_data.buff[3] = temp>>16;
			com_data.buff[4] = temp>>8;
			com_data.buff[5] = temp;
		}else if(com_data.buff[1] == 0xF0){//跳转任意调焦变倍位
			ZoomParameters.PresetPos = com_data.buff[2]<<8 | com_data.buff[3];
			ZoomParameters.PresetPos = 4096 - ZoomParameters.PresetPos;
			if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//						ZoomParameters.PresetLeftFlag = 1;
				ZoomParameters.PresetRightFlag = 1;
			}else{
//						ZoomParameters.PresetRightFlag = 1;
				ZoomParameters.PresetLeftFlag = 1;
			}
			FocusParameters.PresetPos = com_data.buff[4]<<8 | com_data.buff[5];
			if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
				FocusParameters.PresetRightFlag = 1;
			}else{
				FocusParameters.PresetLeftFlag = 1;
			}
		}
		Return_ZhongLin_Servo_Message();
#endif
}

void Return_ZhongLin_Status(u8 Status){
#if SUPPORT_ZhongLin_UART_PROTOCOL
	send_data.rx_buff[0] = ZhongLin_Command_Head1;
	send_data.rx_buff[1] = ZhongLin_Command_Head2;
	send_data.rx_buff[2] = 0x01;
	send_data.rx_buff[3] = Status;
	send_data.rx_buff[send_data.rx_buff[2]+4] = ZhongLin_Command_Tail;
	send_data.rx_buff[send_data.rx_buff[2]+3] = ZhongLin_Data_Verify(&send_data.rx_buff[2]);
	for(u8 i = 0; i < send_data.rx_buff[2]+5; i++){
		bsp_printf("%c",send_data.rx_buff[i]);
	}
    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

void Return_ZhongLin_Message(void){
#if SUPPORT_ZhongLin_UART_PROTOCOL
	if(!send_data.rx_buff[2]) return;
	send_data.rx_buff[0] = ZhongLin_Command_Head1;
	send_data.rx_buff[1] = ZhongLin_Command_Head2;
	send_data.rx_buff[3] = com_data.buff[3];
	send_data.rx_buff[4] = com_data.buff[4];
	send_data.rx_buff[send_data.rx_buff[2]+4] = ZhongLin_Command_Tail;
	send_data.rx_buff[send_data.rx_buff[2]+3] = ZhongLin_Data_Verify(&send_data.rx_buff[2]);
	for(u8 i = 0; i < send_data.rx_buff[2]+5; i++){
		bsp_printf("%c",send_data.rx_buff[i]);
	}
    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

void Return_ZhongLin_Servo_Message(void){
#if SUPPORT_ZhongLin_UART_PROTOCOL
//	u16 temp;
//	send_data.rx_buff[0] = ZhongLin_Servo_Head;
//	send_data.rx_buff[1] = 0x01;
//	temp = GetVol(Focus_MCP3221_addr);
//	send_data.rx_buff[2] = temp>>8;
//	send_data.rx_buff[3] = temp;
//	temp = GetVol(Zoom_MCP3221_addr);
//	send_data.rx_buff[4] = temp>>8;
//	send_data.rx_buff[5] = temp;
//	send_data.rx_buff[6] = ZhongLin_Servo_Data_Verify(&send_data.rx_buff[1]);
	for(u8 i = 0; i < 7; i++){
		bsp_printf("%c",com_data.buff[i]);
	}
//    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
#endif
}

u8 ZhongLin_Data_Verify(u8 *Databuf){
	u8 Verify_Sum = 0;
#if SUPPORT_ZhongLin_UART_PROTOCOL
	for(u8 i = 0 ;i <= Databuf[0] ;i++){
		Verify_Sum ^= Databuf[i];
    }
#endif
	return Verify_Sum;
}

u8 ZhongLin_Servo_Data_Verify(u8 *Databuf){
	u8 Verify_Sum = 0;
#if SUPPORT_ZhongLin_UART_PROTOCOL
	for(u8 i = 0 ;i < 5 ;i++){
		Verify_Sum += Databuf[i];
    }
#endif
	return Verify_Sum;
}
/****************************************************************/



