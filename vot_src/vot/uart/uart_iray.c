
/*
 * uart_iray.c
 *
 *  Created on: 2023年10月17日
 *      Author: yan
 */

#include "uart_iray.h"
#include "bsp.h"
#include "../menu/name.h"

struct IRAY_TOOLS IrayTools;

void UartDataHanding(void)    //串口数据处理
{
#ifdef SUPPORT_IRAY_UART_PROTOCOL
	if(com_data.buff[0] == Iray_command_head1)
	{
		if((com_data.buff[com_data.buff[1]+2] == Iray_command_tail1) && (com_data.buff[com_data.buff[1]+3] == Iray_command_tail2))
		{
			VerifyData(com_data.buff);
			if((com_data.buff[com_data.buff[1]+1] != IrayTools.CHECKSUM))
			{MessageReturnError(DRCCalibrationFailure);return;}
			switch(com_data.buff[1])
			{
				default: MessageReturnError(CommandNotExist); break;
	            case 4:receiveData_4byte();break;
				case 5:receiveData_5byte();break;
				case 6:receiveData_6byte();break;
				case 7:receiveData_7byte();break;
				case 8:receiveData_8byte();break;
				case 9:receiveData_9byte();break;
				case 12:receiveData_12byte();break;
				case 13:receiveData_13byte();break;
			}
		}
	}
	else
	{MessageReturnError(FrameHeaderError);}
#endif
}

#ifdef SUPPORT_IRAY_UART_PROTOCOL
void MessageReturnError(u8 ErrorMes)
{
	u8 i = 0;

	send_data.rx_buff[0] = Iray_command_head2;
	send_data.rx_buff[1] = 0x05;
	send_data.rx_buff[2] = 0xFF;
	send_data.rx_buff[3] = 0xFF;
	send_data.rx_buff[4] = Iray_command_operation_word;
	send_data.rx_buff[5] = ErrorMes;
	send_data.rx_buff[7] = Iray_command_tail1;
	send_data.rx_buff[8] = Iray_command_tail2;
	VerifyData(send_data.rx_buff);
	send_data.rx_buff[6] = IrayTools.CHECKSUM;
	for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
	{
		bsp_printf("%c",send_data.rx_buff[i]);
	}
    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void receiveData_4byte(void)
{
  if(com_data.buff[2] == 0x01)
	{

		u8 Data_8bit;
	    u16 Temp,Temp1;
		u16 Data_16bit;

		send_data.rx_buff[0] = Iray_command_head2;
		send_data.rx_buff[3] = Iray_command_operation_word;
		send_data.rx_buff[7] = Iray_command_tail1;
		send_data.rx_buff[8] = Iray_command_tail2;
/************************设置菜单栏（部分待添加）***********************/
		if((com_data.buff[3] == 0xC3) && (com_data.buff[4] == 0x00)) //读取探测器DET温度  DET的AD值 y = -33.8x +7543
		{
			float K,TEMP;
			uint16_t TEMP_16;
			u8 TEMP_int,TEMP_point;

			send_data.rx_buff[1] = 0x05;
			send_data.rx_buff[2] = 0xC3;

			DispSysMenu2.Det_PAR = ReadDETAD();
			if(DispSysMenu2.Det_PAR < 7543) //正
			{
				K = 33.8;
				TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
				TEMP_16 = (uint16_t)(TEMP *100);
				TEMP_int = (TEMP_16 >>8)& 0x7F;
				TEMP_int = (TEMP_16 >>8);
				TEMP_point = TEMP_16;
			}
			else
			{
				K = 33.8;
				TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
				TEMP_16 = (uint16_t)(TEMP *100);
				TEMP_16 = (~TEMP_16) + 1;
				TEMP_int = TEMP_16 >> 8;
				TEMP_int = TEMP_int | 0x80;
				TEMP_point = TEMP_16;
			}
				send_data.rx_buff[4] = TEMP_point;
				send_data.rx_buff[5] = TEMP_int;
			}
		  else if((com_data.buff[3] == 0x7C) && (com_data.buff[4] == 0x00)) //读取sensor温度
		  {
			  	  	float temp;
			  	  	u16 temp1;

					send_data.rx_buff[1] = 0x05;
					send_data.rx_buff[2] = 0x7C;

					temp = READ_TMP112_Temp();
					if(temp < 0)  //负 补码形式，需转换
					{
						temp1 = (u16)abs((int)(temp*100));
						temp1 = (~temp1)+1;

						send_data.rx_buff[4] = temp1;    //数据低位在前，高位在后
						send_data.rx_buff[5] = temp1 >>8;
					}
					else
					{
						temp1 = (u16)abs((int)(temp*100));

						send_data.rx_buff[4] = temp1;
						send_data.rx_buff[5] = temp1 >>8;
					}
		   }
			else if((com_data.buff[3] == 0x7F) && (com_data.buff[4] == 0x02)) //保存设置
			{
			    ClearSize16X16(32,48,400);
			    DisPlayNewStr(32,400,"Saving",4,CharacterColor);
			    Write_UsermenuParameterToFlash();//用户菜单数据保存
			    Write_TestmenuParameterToFlash();
			    Write_CommonAreaParameterToFlash();
			    delay_1s(1);
			    ClearSize16X16(32,48,400);
			    DisPlayNewStr(32,400,"Done",4,CharacterColor);
			    delay_1s(1);
			    ClearSize16X16(32,48,400);
				send_data.rx_buff[1] = 0x04;
				send_data.rx_buff[2] = 0x7F;
				send_data.rx_buff[4] = 0x01;
				send_data.rx_buff[6] = Iray_command_tail1;
			    send_data.rx_buff[7] = Iray_command_tail2;
			}
			else if((com_data.buff[3] == 0x44) && (com_data.buff[4] == 0x00)) //光标位置(位置读取)
			{
				send_data.rx_buff[1] = 0x07;
				send_data.rx_buff[2] = 0x44;
//				send_data.rx_buff[4] = mPostPara.Cross0_x;
//			    send_data.rx_buff[5] = mPostPara.Cross0_x>>8;
//				send_data.rx_buff[6] = mPostPara.Cross0_y;
//			    send_data.rx_buff[7] = mPostPara.Cross0_x>>8;
				send_data.rx_buff[9] = Iray_command_tail1;
				send_data.rx_buff[10] = Iray_command_tail2;

			}
/************************增益控制菜单栏（部分待添加）***********************/
			else if((com_data.buff[3] == 0x2B) && (com_data.buff[4] == 0x00)) //感兴趣区域读取
			{
				send_data.rx_buff[13] = Iray_command_tail1;
				send_data.rx_buff[14] = Iray_command_tail2;

			    send_data.rx_buff[1] = 0x0B;
			    send_data.rx_buff[2] = 0x2B;
//			  send_data.rx_buff[4] = mPostPara.Area1_Lx;
//			  send_data.rx_buff[5] = mPostPara.Area1_Lx>>8;
//			  send_data.rx_buff[6] = mPostPara.Area1_Ly;
//			  send_data.rx_buff[7] = mPostPara.Area1_Ly>>8;
//			  send_data.rx_buff[8] = mPostPara.Area1_Rx;
//			  send_data.rx_buff[9] = mPostPara.Area1_Rx>>8;
//			  send_data.rx_buff[10] = mPostPara.Area1_Ry;
//			  send_data.rx_buff[11] = mPostPara.Area1_Ry>>8;
			}
 /************************盲元功能菜单栏(部分待添加)***********************/
			else if((com_data.buff[3] == 0x93) && (com_data.buff[4] == 0x02)) //盲元扫描
			{
//			  Calc_BAD_INC();
				send_data.rx_buff[6] = Iray_command_tail1;
				send_data.rx_buff[7] = Iray_command_tail2;
				send_data.rx_buff[1] = 0x04;
				send_data.rx_buff[2] = 0x93;
				send_data.rx_buff[4] = 0x01;
			}
   /************************其他功能设置(PN\SN码查询)***********************/

			else if((com_data.buff[3] == 0x70) && (com_data.buff[4] == 0x00)) //PN查询 //改为序列号查询
			{
				send_data.rx_buff[25] = Iray_command_tail1;
				send_data.rx_buff[26] = Iray_command_tail2;

				send_data.rx_buff[1] = 0x17;
				send_data.rx_buff[2] = 0x70;

				send_data.rx_buff[4] = SystemParaMenu2.MerchineSN[0];
				send_data.rx_buff[5] = SystemParaMenu2.MerchineSN[1];
				send_data.rx_buff[6] = SystemParaMenu2.MerchineSN[2];
				send_data.rx_buff[7] = SystemParaMenu2.MerchineSN[3];
				send_data.rx_buff[8] = SystemParaMenu2.MerchineSN[4];
				send_data.rx_buff[9] = SystemParaMenu2.MerchineSN[5];
				send_data.rx_buff[10] = SystemParaMenu2.MerchineSN[6];
				send_data.rx_buff[11] = SystemParaMenu2.MerchineSN[7];
				send_data.rx_buff[12] = SystemParaMenu2.MerchineSN[8];
				send_data.rx_buff[13] = SystemParaMenu2.MerchineSN[9];
				send_data.rx_buff[14] = SystemParaMenu2.MerchineSN[10];
				send_data.rx_buff[15] = SystemParaMenu2.MerchineSN[11];
				send_data.rx_buff[16] = SystemParaMenu2.MerchineSN[12];
				send_data.rx_buff[17] = SystemParaMenu2.MerchineSN[13];
				send_data.rx_buff[18] = 0x00;
				send_data.rx_buff[19] = 0x00;
				send_data.rx_buff[20] = 0x00;
				send_data.rx_buff[21] = 0x00;
				send_data.rx_buff[22] = 0x00;
			  	send_data.rx_buff[23] = 0x00;
			}
			else if((com_data.buff[3] == 0x71) && (com_data.buff[4] == 0x00)) //SN查询  //改为镜头信息查询
			{
//				KEY_F4_PRO();
				send_data.rx_buff[25] = Iray_command_tail1;
				send_data.rx_buff[26] = Iray_command_tail2;

				send_data.rx_buff[1] = 0x17;
				send_data.rx_buff[2] = 0x71;

				send_data.rx_buff[4] = SystemParaMenu2.LensSN[0];
				send_data.rx_buff[5] = SystemParaMenu2.LensSN[1];
				send_data.rx_buff[6] = SystemParaMenu2.LensSN[2];
				send_data.rx_buff[7] = SystemParaMenu2.LensSN[3];
				send_data.rx_buff[8] = SystemParaMenu2.LensSN[4];
				send_data.rx_buff[9] = SystemParaMenu2.LensSN[5];
				send_data.rx_buff[10] = SystemParaMenu2.LensSN[6];
				send_data.rx_buff[11] = SystemParaMenu2.LensSN[7];
				send_data.rx_buff[12] = SystemParaMenu2.LensSN[8];
				send_data.rx_buff[13] = SystemParaMenu2.LensSN[9];
				send_data.rx_buff[14] = 0x00;
				send_data.rx_buff[15] = 0x00;
				send_data.rx_buff[16] = 0x00;
				send_data.rx_buff[17] = 0x00;
				send_data.rx_buff[18] = 0x00;
				send_data.rx_buff[19] = 0x00;
				send_data.rx_buff[20] = 0x00;
				send_data.rx_buff[21] = 0x00;
				send_data.rx_buff[22] = 0x00;
				send_data.rx_buff[23] = 0x00;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
		VerifyData(send_data.rx_buff);
		send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		  	{
		  		bsp_printf("%c",send_data.rx_buff[i]);
		  	}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
	}
}

void receiveData_5byte(void)
{
  if(com_data.buff[2] == 0x01)
	{
		send_data.rx_buff[0] = Iray_command_head2;
		send_data.rx_buff[1] = 0x04;
		send_data.rx_buff[3] = Iray_command_operation_word;
		send_data.rx_buff[6] = Iray_command_tail1;
		send_data.rx_buff[7] = Iray_command_tail2;
	/************************设置菜单栏（部分待添加）***********************/
		  if((com_data.buff[3] == 0x11) && (com_data.buff[4] == 0x02))	 //NUC校正
		{
				send_data.rx_buff[2] = 0x11;
//			 	成像型
				if(com_data.buff[5] == 0x00)  //背景校正-打档外参考
				{
					Ext_Shutter_pro();
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) //快门校正-打档内参考
				{
					Int_Shutter_pro();
					send_data.rx_buff[4] = 0x01;
				}

//              测温型
				else if(com_data.buff[5] == 0x80)  //背景校正-打档外参考
				{
					Ext_Shutter_pro();
					send_data.rx_buff[4] = 0x80;
				}
				else if(com_data.buff[5] == 0x81) //快门校正-打档内参考
				{
					Int_Shutter_pro();
					send_data.rx_buff[4] = 0x81;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		}
	   else if((com_data.buff[3] == 0x01) && (com_data.buff[4] == 0x01)) //自动NUC校正
		{
			  send_data.rx_buff[2] = 0x01;
			  if(com_data.buff[5] == 0x00)  // 关
				{
					systemPar.shutter_flag = 0;
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) // 开
				{
					systemPar.shutter_flag = 3;
					send_data.rx_buff[4] = 0x01;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		 }
		 else if((com_data.buff[3] == 0x03) && (com_data.buff[4] == 0x01)) //设置自动校正时间间隔
		{
			send_data.rx_buff[2] = 0x03;

			if(com_data.buff[5] >= 0 && com_data.buff[5] <= 0x14)
			{
				ShutterTimer = 0;
				UsermenuParameter.AutoShutter = com_data.buff[5] * 2;
				Write_UsermenuParameterToFlash();
				Int_Shutter_pro();

				send_data.rx_buff[4] = UsermenuParameter.AutoShutter / 2;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
//			else
//			{
//				send_data.rx_buff[4] = 0xFF;  //设置失败
//			}
		}
		else if((com_data.buff[3] == 0x04) && (com_data.buff[4] == 0x01)) //设置自动校正温度间隔
		{
				send_data.rx_buff[2] = 0x04;

				if(com_data.buff[5] >= 0 && com_data.buff[5] <= 0x14)
				{
					AutoTempFlag = 1;
					SystemParaMenu2.TempChangeValue = com_data.buff[5];
					Write_TestmenuParameterToFlash();

					send_data.rx_buff[4] = SystemParaMenu2.TempChangeValue;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		}
		else if((com_data.buff[3] == 0x82) && (com_data.buff[4] == 0x02)&& (com_data.buff[5] == 0x00)) //恢复出厂-初始化
		{
			ResetFactorySettingsParameter(&UsermenuItemTable[3].pt[5],2);

			send_data.rx_buff[2] = 0x82;
	        send_data.rx_buff[4] = 0x01;
		}
	/************************视频菜单栏（部分待添加）***********************/
			else if((com_data.buff[3] == 0x43) && (com_data.buff[4] == 0x02)) //十字光标样式（待添加）
			{
				send_data.rx_buff[2] = 0x43;
			  if(com_data.buff[5] == 0x81)  //样式1
				{

					send_data.rx_buff[4] = 0x81;
				}
				else if(com_data.buff[5] == 0x82)  //样式2
				{

					send_data.rx_buff[4] = 0x82;
				}
				else if(com_data.buff[5] == 0x83)  //样式3
				{

					send_data.rx_buff[4] = 0x83;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

			}
		  else if((com_data.buff[3] == 0x5C) && (com_data.buff[4] == 0x01))	 //数字视频源选择
		 {
		        send_data.rx_buff[2] = 0x5C;

				if(com_data.buff[5] == 0x00)  //ORG(实际原始AD图像)
				{
//					YUVorAD = 1;
//					ADV_set1();
//				    TF_ctl_off();
//				    Bd_ctl_off(); //本底校正
//				    K_ctl_off(); //NUC
//				    BAD_ctl_off();
//				    delay_1ms(20);
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01)  //NUC(实际非均匀校正后的AD图像)
				{
//					YUVorAD = 1;
//					ADV_set1();
//
//				    Bd_ctl_on();
//				    K_ctl_on(); //NUC
//				    BAD_ctl_on();
//
//				    TF_ctl_off();
//				    delay_1ms(20);
					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02)  //DRC（实际正常YUV图像）
				{
//					 YUVorAD = 0;
//			         ADV_set0();
//
//			         TF_ctl_on();
//			         Bd_ctl_on();
//			         K_ctl_on(); //NUC
//			         BAD_ctl_on();
					 send_data.rx_buff[4] = 0x02;
				}
				else if(com_data.buff[5] == 0x04)  //TEMP(待添加)
				{

					send_data.rx_buff[4] = 0x04;
				}
				else if(com_data.buff[5] == 0x05)  //DNS（待添加）
				{

					send_data.rx_buff[4] = 0x05;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
			}
		 else if((com_data.buff[3] == 0x3F) && (com_data.buff[4] == 0x02))	 //CVBS制式
		{
			  send_data.rx_buff[2] = 0x3F;
			  if(com_data.buff[5] == 0x01)  //PLA
				{
//					video_format = 0;
					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x00)  //NTSC（待添加）
				{
//					video_format = 0x01;
					send_data.rx_buff[4] = 0x00;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

		}
		else if((com_data.buff[3] == 0x4C) && (com_data.buff[4] == 0x01)) //图像翻转
		{
				u8 MirrorFlag_value;

				send_data.rx_buff[2] = 0x4C;

				MirrorFlag_value = UsermenuParameter.Mirror;
				if(com_data.buff[5] == 0x01) //不翻转
				{
					UsermenuParameter.Mirror = 0;
					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02) //左右翻转
				{
					UsermenuParameter.Mirror = 1;
					send_data.rx_buff[4] = 0x02;
				}
				else if(com_data.buff[5] == 0x04) //上下翻转
				{
					UsermenuParameter.Mirror = 2;
					send_data.rx_buff[4] = 0x04;
				}
				else if(com_data.buff[5] == 0x08) //镜像反转
				{
					UsermenuParameter.Mirror = 3;
					send_data.rx_buff[4] = 0x08;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
				SetFreeze_img(ON);
				delay_1ms(20);
//				if(MirrorFlag_value < UsermenuParameter.Mirror) 	    MirrorBadSppot(1);
//				else if(MirrorFlag_value > UsermenuParameter.Mirror)  	MirrorBadSppot(0);
				MirrorBadRecover(MirrorFlag_value); //镜像复原
				MirrorBadSppotInit(); //镜像翻转
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xfffffcff)|(UsermenuParameter.Mirror<<8)));
				SetFreeze_img(OFF);
//				Int_Shutter_pro();
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
		}
		else if((com_data.buff[3] == 0x3D) && (com_data.buff[4] == 0x02)) //CVBS视频开关
		{
		        send_data.rx_buff[2] = 0x3D;

				if(com_data.buff[5] == 0x00)  // 关
				{
					SystemParaMenu2.CVBS_flag = 0;
					APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xfffffff7)|(SystemParaMenu2.CVBS_flag<<3)));
//					WriteADV7391(0x00,1);
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) // 开
				{
					SystemParaMenu2.CVBS_flag = 1;
					APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xfffffff7)|(SystemParaMenu2.CVBS_flag<<3)));
					//初始化7391，给出彩条
//					Init_ADV7391();
//					delay_1ms(15);
////					Int_Shutter_pro;
//					delay_1ms(15);
//					WriteADV7391(0x84,0x00);	//关闭测试彩条，正常输出图像
//				    delay_1ms(15);
////				    Int_Shutter_pro;

					send_data.rx_buff[4] = 0x01;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		}
		else if((com_data.buff[3] == 0x3E) && (com_data.buff[4] == 0x02)) //图像冻结
	 {
		        send_data.rx_buff[2] = 0x3E;
				if(com_data.buff[5] == 0x00)  // 关
				{
					SetFreeze_img(OFF); //图像激活
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) // 开
				{
                    SetFreeze_img(ON);		//图像冻结
					send_data.rx_buff[4] = 0x01;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

	 }
	  else if((com_data.buff[3] == 0x42) && (com_data.buff[4] == 0x02)) //设置调色板（后续还有添加）
		{
				send_data.rx_buff[2] = 0x42;
				if((com_data.buff[5]>=0x00)&&(com_data.buff[5]<=0x0E))
				{
					UsermenuParameter.Palette = com_data.buff[5];
					load_palette(UsermenuParameter.Palette);
					APB3_REGW(IO_APB_PAT_0X01,0,((APB3_REGR(IO_APB_PAT_0X01,0)&0xffffff0f)|(UsermenuParameter.Palette<<4)));
					if(SelectionMenu.SelectItem != NULL)
					{
						if((SelectionMenu.SelectItem->Parent == &UsermenuItemTable[0].pt[0])&&(SelectionMenu.SelectItem->Parent !=SelectionMenu.SelectItem))
						{
							DisplayBackground(UsermenuItemTable[1].pt[4].Info.line_X+8*GetStringSize(UsermenuItemTable[1].pt[4].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
									Y_START+Y_GAP*UsermenuItemTable[1].pt[4].Info.line_Y,8,BACKGROUND_LAYER,BackgroundColor);
							DisPlayNewStr(UsermenuItemTable[1].pt[4].Info.line_X+8*GetStringSize(UsermenuItemTable[1].pt[4].Info.Name[SelectionMenu.IsEnableEnligh])+16-8*SelectionMenu.IsEnableEnligh,\
									Y_START+Y_GAP*UsermenuItemTable[1].pt[4].Info.line_Y,PaletteNameTab[UsermenuParameter.Palette][SelectionMenu.IsEnableEnligh],4,CharacterColor);
						}
					}
					send_data.rx_buff[4] = com_data.buff[5];
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		}
	 /************************增益控制菜单栏（部分待添加）***********************/
			else if((com_data.buff[3] == 0x1F) && (com_data.buff[4] == 0x01)) //图像模式（AGC模式）
			{
				send_data.rx_buff[2] = 0x1F;

				if(com_data.buff[5] == 0x00)  // 手动
				{
					UsermenuParameter.ImageAdjust = 0;
					UsermenuParameter.ImageMode = 0;
					Write_CommonAreaParameterToFPGA();
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) // 室内
				{
					UsermenuParameter.ImageAdjust = 1;
					Write_CommonAreaParameterToFPGA();

					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02) // 室外
				{
					UsermenuParameter.ImageAdjust = 2;
					UsermenuParameter.ImageMode = 1;
					Write_CommonAreaParameterToFPGA();
					send_data.rx_buff[4] = 0x02;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

			}
			else if((com_data.buff[3] == 0x22) && (com_data.buff[4] == 0x01)) //对比度设置
			{
				send_data.rx_buff[2] = 0x22;

				if(UsermenuParameter.ImageAdjust == 0)
				{
					if(UsermenuParameter.ImageMode == 0)
					{
						SystemParaMenu2.Ctrs_M = com_data.buff[5];
						if(SystemParaMenu2.Ctrs_M > 127)
						{
							SystemParaMenu2.Ctrs_M = 127;
						}
						APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0x0000ffff)|(SystemParaMenu2.Ctrs_M<<16)));
						send_data.rx_buff[4] = SystemParaMenu2.Ctrs_M;
					}
				}
				else
				{
					return;
				}

			}
			else if((com_data.buff[3] == 0x1A) && (com_data.buff[4] == 0x02)) //设置图像增强开关
			{
				send_data.rx_buff[2] = 0x1A;

				if(com_data.buff[5] == 0x00) //关
				{
					SystemParaMenu2.DDE_Level = 0;
					APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level);
					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) //开
				{
					SystemParaMenu2.DDE_Level = 1;
					APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level);
					send_data.rx_buff[4] = 0x01;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

			}
			else if((com_data.buff[3] == 0x19) && (com_data.buff[4] == 0x01)) //设置图像增强档位
			{
				send_data.rx_buff[2] = 0x19;

				if(com_data.buff[5] >= 0x00 && com_data.buff[5] <= 0x10)
				{
					SystemParaMenu2.DDE_Level = com_data.buff[5] - 1;
					APB3_REGW_DDE_LV(SystemParaMenu2.DDE_Level);
					send_data.rx_buff[4] = SystemParaMenu2.DDE_Level + 1;
				}
    			else
    			{
    				MessageReturnError(CommandNotExist);
    				return;
    			}
//                else
//				{
//				  send_data.rx_buff[4] = 0xFF;
//				}
			}
			else if((com_data.buff[3] == 0x1B) && (com_data.buff[4] == 0x02)) //设置图像滤波开关（空间滤波）
			{
				send_data.rx_buff[2] = 0x1B;

//				if(com_data.buff[5] == 0x00) //关
//				{
//					SystemParaMenu2.SFilter = 0;
//					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
//					send_data.rx_buff[4] = 0x00;
//				}
//				else if(com_data.buff[5] == 0x01) //开（一档）
//				{
//					SystemParaMenu2.SFilter = 1;
//					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
//					send_data.rx_buff[4] = 0x01;
//				}
//				else if(com_data.buff[5] == 0x02) //开（二档）
//				{
//					SystemParaMenu2.SFilter = 2;
//					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
//					send_data.rx_buff[4] = 0x02;
//				}
//				else if(com_data.buff[5] == 0x03) //开（三档）
//				{
//					SystemParaMenu2.SFilter = 3;
//					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
//					send_data.rx_buff[4] = 0x03;
//				}
				if(com_data.buff[5] >= 0x01 && com_data.buff[5] <= 0x07)
				{
					SystemParaMenu2.SFilter = com_data.buff[5];
					APB3_REGW_SF_LV(SystemParaMenu2.SFilter);
					send_data.rx_buff[4] = SystemParaMenu2.SFilter;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}

			}
 /************************盲元功能菜单栏(部分待添加)***********************/
			else if((com_data.buff[3] == 0x90) && (com_data.buff[4] == 0x01)) //盲元添加/撤销
			{

			  send_data.rx_buff[2] = 0x90;

				if(com_data.buff[5] == 0x01) //添加
				{
					//NUC_ctl_on();		//增益校正开
//				  BAD_ctl_on();		//盲元校正开
				  //BAD8_ctl_on();		//8点盲元替换开
				/*  DisPlayMenu2(Menu2State);*/
					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02) //撤销
				{
//					BAD_ctl_off();		//盲元校正关
				/*  DisPlayMenu2(Menu2State);*/
					send_data.rx_buff[4] = 0x02;
				}
				else if(com_data.buff[5] == 0x05) //盲元表保存
				{
					ClearSize16X16(32,48,400);
					DisPlayNewStr(32,400,"Saving",4,CharacterColor);
					Write_UsermenuParameterToFlash();
					Write_TestmenuParameterToFlash();
					Write_CommonAreaParameterToFlash();
					Write_TempParameterToFlash();//231205新增
					delay_1s(1);
					ClearSize16X16(32,48,400);
					DisPlayNewStr(32,400,"Done",4,CharacterColor);
					delay_1s(1);
					ClearSize16X16(32,48,400);
					send_data.rx_buff[4] = 0x05;
				}
				else if(com_data.buff[5] == 0x06) //盲元表恢复
				{
					BAD_Init_INC_DEC();
					send_data.rx_buff[4] = 0x06;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
			}
			else if((com_data.buff[3] == 0xA0) && (com_data.buff[4] == 0x01)) //标镜头K
			{

			   send_data.rx_buff[2] = 0xA0;

				if(com_data.buff[5] == 0x0A) //获取低温数据
				{
					Calc_NUC_DEC();
					send_data.rx_buff[4] = 0x0A;
				}
				else if(com_data.buff[5] == 0x0B) //获取高温数据
				{
					Calc_NUC_INC();
					send_data.rx_buff[4] = 0x0B;
				}
				else if(com_data.buff[5] == 0x0C) //计算  //改为开关
				{
					if(SystemParaMenu2.NUC_SW == 1)
					{
						SystemParaMenu2.NUC_SW = 0;
						APB3_REGW(IO_APB_SLAVE_0_INPUT_function00,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function00,0)&0xf7ffffff)|(SystemParaMenu2.NUC_SW<<27)));
						DisplayBackground(32,400,7,BACKGROUND_LAYER,3);
						DisPlayNewStr(32,400,"NUC_OFF",4,6);
						delay_1s(1);
						ClearSize16X16(32,56,400);
					}
					else
					{
						SystemParaMenu2.NUC_SW = 1;
						APB3_REGW(IO_APB_SLAVE_0_INPUT_function00,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function00,0)&0xf7ffffff)|(SystemParaMenu2.NUC_SW<<27)));
						delay_1ms(30);
						Int_Shutter_pro();
						DisplayBackground(32,400,7,BACKGROUND_LAYER,3);
						DisPlayNewStr(32,400,"NUC_ON",4,6);
						delay_1s(1);
						ClearSize16X16(32,56,400);
					}
					send_data.rx_buff[4] = 0x0C;
				}
				else if(com_data.buff[5] == 0x0D) //保存
				{
					DisplayBackground(32,400,6,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Saving",4,6);
					Write_Flash_NUC();
					DisplayBackground(32,400,6,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Done",4,6);
					delay_1s(1);
					ClearSize16X16(32,48,400);
					send_data.rx_buff[4] = 0x0D;
				}
				else if(com_data.buff[5] == 0x0E) //清除
				{
					Nuc_Init_INC_DEC();
					delay_1ms(30);
					BAD_Init_INC_DEC();
					delay_1ms(30);
					Int_Shutter_pro();
					DisplayBackground(32,400,6,BACKGROUND_LAYER,3);
					DisPlayNewStr(32,400,"Done",4,6);
					delay_1s(1);
					ClearSize16X16(32,48,400);
					send_data.rx_buff[4] = 0x0E;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
			}
			else if((com_data.buff[3] == 0xA1) && (com_data.buff[4] == 0x01)) //标定锅盖
			{

			  send_data.rx_buff[2] = 0xA1;

				if(com_data.buff[5] == 0x00) //获取
				{

					send_data.rx_buff[4] = 0x00;
				}
				else if(com_data.buff[5] == 0x01) //保存
				{

					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02) //清除
				{

					send_data.rx_buff[4] = 0x02;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
			}
     else if((com_data.buff[3] == 0x44) && (com_data.buff[4] == 0x02)) //盲元光标移动
			{
//			 if((mPostPara.BAD_Cross0_Flag == 1)&&(mPostPara.Cross0_Flag == 0))
//			 {
			   send_data.rx_buff[2] = 0x44;

//			   Spot1_INC();

				if(com_data.buff[5] == 0x01) //上移1
				{
//					Cross0_Y_INC();
					send_data.rx_buff[4] = 0x01;
				}
				else if(com_data.buff[5] == 0x02) //下移1
				{
//					Cross0_Y_DEC();
					send_data.rx_buff[4] = 0x02;
				}
				else if(com_data.buff[5] == 0x03) //左移1
				{
//					Cross0_X_DEC();
					send_data.rx_buff[4] = 0x03;
				}
				else if(com_data.buff[5] == 0x04) //右移1
				{
//					Cross0_X_INC();
					send_data.rx_buff[4] = 0x04;
				}
				else if(com_data.buff[5] == 0x81) //上移20
				{
//					Cross0_Y_INC_more();
					send_data.rx_buff[4] = 0x81;
				}
				else if(com_data.buff[5] == 0x82) //下移20
				{
//					Cross0_Y_DEC_more();
					send_data.rx_buff[4] = 0x82;
				}
				else if(com_data.buff[5] == 0x83) //左移20
				{
//					Cross0_X_DEC_more();
					send_data.rx_buff[4] = 0x83;
				}
				else if(com_data.buff[5] == 0x84) //右移20
				{
//					Cross0_X_INC_more();
					send_data.rx_buff[4] = 0x84;
				}
//			}
		}
	}

  else if(com_data.buff[2] == 0x07)
  {
      send_data.rx_buff[0] = Iray_command_head2;
	  send_data.rx_buff[1] = 0x05;
	  send_data.rx_buff[2] = 0x07;
	  send_data.rx_buff[4] = Iray_command_operation_word;
	  send_data.rx_buff[7] = Iray_command_tail1;
	  send_data.rx_buff[8] = Iray_command_tail2;
		 /************************测温参数功能菜单栏(待添加)***********************/
		 if((com_data.buff[3] == 0x00) && (com_data.buff[4] == 0x01)) //测温OSD开/关
		 {
		      send_data.rx_buff[3] = 0x00;
		      if(com_data.buff[5] == 0x00)//关
			 {

			   send_data.rx_buff[5] = 0x00;
			 }
			 else if(com_data.buff[5] == 0x01)//开
			 {

			   send_data.rx_buff[5] = 0x01;
			 }
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		 }
		 else if((com_data.buff[3] == 0x02) && (com_data.buff[4] == 0x01)) //温度单位选择
		 {
		         send_data.rx_buff[3] = 0x02;
			  if(com_data.buff[5] == 0x00)//摄氏度
				{

				  send_data.rx_buff[5] = 0x00;
				}
				else if(com_data.buff[5] == 0x01)//开尔文
				{

				  send_data.rx_buff[5] = 0x01;
				}
				else if(com_data.buff[5] == 0x02)//华氏度
				{

				  send_data.rx_buff[5] = 0x02;
				}
				else
				{
					MessageReturnError(CommandNotExist);
					return;
				}
		 }
		else if((com_data.buff[3] == 0x05) && (com_data.buff[4] == 0x00)) //低—高增益阈值读取
		{
			send_data.rx_buff[8] = Iray_command_tail1;
		    send_data.rx_buff[9] = Iray_command_tail2;
			send_data.rx_buff[1] = 0x06;
		    send_data.rx_buff[3] = 0x05;
		  if(com_data.buff[5] == 0x00)
			{

			 send_data.rx_buff[5] = 0xB0;
		     send_data.rx_buff[6] = 0x04;
			}
		}
		else if((com_data.buff[3] == 0x06) && (com_data.buff[4] == 0x00)) //低—高增益百分比读取
		{
		   send_data.rx_buff[1] = 0x07;
		   send_data.rx_buff[3] = 0x06;
		   send_data.rx_buff[9] = Iray_command_tail1;
		   send_data.rx_buff[10] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//百分比95%
			{

			  send_data.rx_buff[5] = 0x5F;
		      send_data.rx_buff[6] = 0x00;
			  send_data.rx_buff[7] = 0x00;
			}
		}
		else if((com_data.buff[3] == 0x07) && (com_data.buff[4] == 0x00)) //高—低增益阈值读取
		{
		   send_data.rx_buff[1] = 0x06;
		   send_data.rx_buff[3] = 0x07;
		   send_data.rx_buff[8] = Iray_command_tail1;
		   send_data.rx_buff[9] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//阈值为140.0
			{

			  send_data.rx_buff[5] = 0x78;
		      send_data.rx_buff[6] = 0x05;
			}
		}
		else if((com_data.buff[3] == 0x08) && (com_data.buff[4] == 0x00)) //高—低增益百分比读取
		{
		   send_data.rx_buff[1] = 0x07;
		   send_data.rx_buff[3] = 0x08;
		   send_data.rx_buff[9] = Iray_command_tail1;
		   send_data.rx_buff[10] = Iray_command_tail2;
		   if(com_data.buff[5] == 0x00)//百分比为15%
			 {

			   send_data.rx_buff[5] = 0x0F;
		       send_data.rx_buff[6] = 0x00;
			   send_data.rx_buff[7] = 0x00;
			 }
		}
		else if((com_data.buff[3] == 0x01) && (com_data.buff[4] == 0x01)) //测温范围选择
		{
		     send_data.rx_buff[3] = 0x01;
		   if(com_data.buff[5] == 0x00)//高增益
			 {

			  send_data.rx_buff[5] = 0x00;
			 }
			 else if(com_data.buff[5] == 0x01)//低增益
			 {

			  send_data.rx_buff[5] = 0x01;
			 }
			 else if(com_data.buff[5] == 0x03)//自动选择
			 {

			  send_data.rx_buff[5] = 0x03;
			 }
		}
		else if((com_data.buff[3] == 0x0F) && (com_data.buff[4] == 0x00)) //反射温度读取
		{
			int32_t temp;
			 send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x0F;
		     send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		    if(com_data.buff[5] == 0x00)
			{
			  temp = (Temp_Parameter.Trefl/10-60)*10000;

			  send_data.rx_buff[5] = temp;
			  send_data.rx_buff[6] = temp>>8;
			  send_data.rx_buff[7] = temp>>16;
			  send_data.rx_buff[8] = temp>>24;

			}
		}
		else if((com_data.buff[3] == 0x10) && (com_data.buff[4] == 0x00)) //大气环境温度读取
		{
			int32_t temp;
			 send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x10;
		     send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{
			  temp = (Temp_Parameter.Tamb/10-60)*10000;

			  send_data.rx_buff[5] = temp;
		  	  send_data.rx_buff[6] = temp>>8;
		  	  send_data.rx_buff[7] = temp>>16;
		  	  send_data.rx_buff[8] = temp>>24;
			}
		}
		else if((com_data.buff[3] == 0x11) && (com_data.buff[4] == 0x00)) //大气透过率读取
		{
			u32 temp;
			 send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x11;
		     send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		   if(com_data.buff[5] == 0x00)//透过率0.45
			 {
			   temp = Temp_Parameter.RelHum*100;
			   send_data.rx_buff[5] = temp;
			   send_data.rx_buff[6] = temp>>8;
			   send_data.rx_buff[7] = temp>>16;
			   send_data.rx_buff[8] = temp>>24;
			 }
		}
		else if((com_data.buff[3] == 0x12) && (com_data.buff[4] == 0x00)) //发射率读取
		{
			u32 temp;
		     send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x12;
		     send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		   if(com_data.buff[5] == 0x00)//发射率0.98
			 {
			   temp = Temp_Parameter.Emiss*100; //*10000
			   send_data.rx_buff[5] = temp;
			   send_data.rx_buff[6] = temp>>8;
			   send_data.rx_buff[7] = temp>>16;
			   send_data.rx_buff[8] = temp>>24;
			 }
		}
		else if((com_data.buff[3] == 0x13) && (com_data.buff[4] == 0x00)) //距离读取
		{
		    u32 distance;
		     send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x13;
		     send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{
			    distance = Temp_Parameter.distance *1000;
			    send_data.rx_buff[5] = distance;
		        send_data.rx_buff[6] = distance>>8;
				send_data.rx_buff[7] = distance>>16;
				send_data.rx_buff[8] = distance>>24;
			}
		}
		else if((com_data.buff[3] == 0x18) && (com_data.buff[4] == 0x01)) //环境变量生效
		{
		     send_data.rx_buff[3] = 0x18;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x01;
			}
		}
	 /************************点测温功能菜单栏(待添加)***********************/
		else if((com_data.buff[3] == 0x82) && (com_data.buff[4] == 0x00)) //点坐标读取
		{
		     send_data.rx_buff[1] = 0x09;
		     send_data.rx_buff[3] = 0x82;
		     send_data.rx_buff[11] = Iray_command_tail1;
		     send_data.rx_buff[12] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x00;
//		      send_data.rx_buff[6] = mPostPara.Cross0_x;
//			  send_data.rx_buff[7] = mPostPara.Cross0_x>>8;
//		      send_data.rx_buff[8] = mPostPara.Cross0_y;
//			  send_data.rx_buff[9] = mPostPara.Cross0_x>>8;
			}
		}
		else if((com_data.buff[3] == 0x83) && (com_data.buff[4] == 0x00)) //点温度读取
		{
		     send_data.rx_buff[1] = 0x09;
		     send_data.rx_buff[3] = 0x83;
			 send_data.rx_buff[11] = Iray_command_tail1;
		     send_data.rx_buff[12] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			    send_data.rx_buff[5] = 0x00;
		        send_data.rx_buff[6] = 0x65;
				send_data.rx_buff[7] = 0x01;
				send_data.rx_buff[8] = 0x00;
				send_data.rx_buff[9] = 0x00;
			}
		}
	 /************************区域、线测温功能菜单栏(线模块功能待添加)***********************/
	 else if((com_data.buff[3] == 0x42) && (com_data.buff[4] == 0x00)) //区域、线模块坐标读取
	 {
	         send_data.rx_buff[1] = 0x0D;
		     send_data.rx_buff[3] = 0x42;
			 send_data.rx_buff[15] = Iray_command_tail1;
		     send_data.rx_buff[16] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x00;
//		      send_data.rx_buff[6] = mPostPara.Area1_Lx;
//			  send_data.rx_buff[7] = mPostPara.Area1_Lx>>8;
//			  send_data.rx_buff[8] = mPostPara.Area1_Ly;
//			  send_data.rx_buff[9] = mPostPara.Area1_Ly>>8;
//			  send_data.rx_buff[10] = mPostPara.Area1_Rx;
//			  send_data.rx_buff[11] = mPostPara.Area1_Rx>>8;
//		      send_data.rx_buff[12] = mPostPara.Area1_Ry;
//			  send_data.rx_buff[13] = mPostPara.Area1_Ry>>8;
			}

	 }
	 else if((com_data.buff[3] == 0x45) && (com_data.buff[4] == 0x00)) //区域、线最高温度值及坐标读取
	 {
	         send_data.rx_buff[1] = 0x0D;
		     send_data.rx_buff[3] = 0x45;
			 send_data.rx_buff[15] = Iray_command_tail1;
		     send_data.rx_buff[16] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{
			  send_data.rx_buff[5] = 0x00;
		      send_data.rx_buff[6] = 0x4E;
			  send_data.rx_buff[7] = 0x01;
			  send_data.rx_buff[8] = 0x00;
			  send_data.rx_buff[9] = 0x00;
			  send_data.rx_buff[10] = 0x10;
		      send_data.rx_buff[11] = 0x00;
			  send_data.rx_buff[12] = 0x0A;
		      send_data.rx_buff[13] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x48) && (com_data.buff[4] == 0x00)) //区域、线最低温度值及坐标读取
	 {
	         send_data.rx_buff[1] = 0x0D;
		     send_data.rx_buff[3] = 0x48;
			 send_data.rx_buff[15] = Iray_command_tail1;
		     send_data.rx_buff[16] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			    send_data.rx_buff[5] = 0x00;
		        send_data.rx_buff[6] = 0x42;
				send_data.rx_buff[7] = 0x01;
				send_data.rx_buff[8] = 0x00;
				send_data.rx_buff[9] = 0x00;
				send_data.rx_buff[10] = 0x2B;
		        send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x15;
				send_data.rx_buff[13] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x4B) && (com_data.buff[4] == 0x00)) //区域、线中心温度值及坐标读取
	 {
	         send_data.rx_buff[1] = 0x0D;
		     send_data.rx_buff[3] = 0x4B;
			 send_data.rx_buff[15] = Iray_command_tail1;
		     send_data.rx_buff[16] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			    send_data.rx_buff[5] = 0x00;
		        send_data.rx_buff[6] = 0x33;
				send_data.rx_buff[7] = 0x01;
				send_data.rx_buff[8] = 0x00;
				send_data.rx_buff[9] = 0x00;
				send_data.rx_buff[10] = 0x96;
		        send_data.rx_buff[11] = 0x00;
				send_data.rx_buff[12] = 0x96;
				send_data.rx_buff[13] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x4C) && (com_data.buff[4] == 0x00)) //区域、线平均温度读取
	 {
	         send_data.rx_buff[1] = 0x09;
		     send_data.rx_buff[3] = 0x4C;
			 send_data.rx_buff[11] = Iray_command_tail1;
		     send_data.rx_buff[12] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			    send_data.rx_buff[5] = 0x00;
		        send_data.rx_buff[6] = 0x33;
				send_data.rx_buff[7] = 0x01;
				send_data.rx_buff[8] = 0x00;
				send_data.rx_buff[9] = 0x00;
			}
	 }
	 	 /************************整帧测温功能(部分待添加)***********************/
	 else if((com_data.buff[3] == 0x20) && (com_data.buff[4] == 0x01)) //等温线开/关
	 {
		     send_data.rx_buff[3] = 0x20;

		  if(com_data.buff[5] == 0x01)//开
			{

			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关
			{

			    send_data.rx_buff[5] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x24) && (com_data.buff[4] == 0x01)) //整帧测温开/关
	 {
		     send_data.rx_buff[3] = 0x24;

		  if(com_data.buff[5] == 0x01)//开
			{

			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关
			{

			    send_data.rx_buff[5] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x26) && (com_data.buff[4] == 0x01)) //最高温度显示开/关
	 {
		     send_data.rx_buff[3] = 0x26;

		  if(com_data.buff[5] == 0x01)//开
			{
			    UsermenuParameter.CaptureFlag = 2;
				Disp_Capture_Info();
			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关
			{
				UsermenuParameter.CaptureFlag = 0;
				Disp_Capture_Info();
			    send_data.rx_buff[5] = 0x00;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }
	 else if((com_data.buff[3] == 0x28) && (com_data.buff[4] == 0x01)) //最低温度显示开/关
	 {
		     send_data.rx_buff[3] = 0x28;

		  if(com_data.buff[5] == 0x01)//开
			{
			  	UsermenuParameter.CaptureFlag = 1;
			  	Disp_Capture_Info();
			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关
			{
				UsermenuParameter.CaptureFlag = 0;
				Disp_Capture_Info();
			    send_data.rx_buff[5] = 0x00;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }
	 else if((com_data.buff[3] == 0x2B) && (com_data.buff[4] == 0x01)) //中心点温度显示开/关
	 {
		     send_data.rx_buff[3] = 0x2B;

		  if(com_data.buff[5] == 0x01)//开
			{

			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关
			{

			    send_data.rx_buff[5] = 0x00;
			}
	 }
	 else if((com_data.buff[3] == 0x2D) && (com_data.buff[4] == 0x01)) //温度报警类型
	 {
		     send_data.rx_buff[3] = 0x2D;

		  if(com_data.buff[5] == 0x00)//关
			{

			    send_data.rx_buff[5] = 0x00;

			}
			else if(com_data.buff[5] == 0x01)//低温报警
			{

			    send_data.rx_buff[5] = 0x01;
			}
			else if(com_data.buff[5] == 0x02)//高温报警
			{

			    send_data.rx_buff[5] = 0x02;
			}
			else if(com_data.buff[5] == 0x03)//高低温报警
			{

			    send_data.rx_buff[5] = 0x03;
			}
	 }
	 else if((com_data.buff[3] == 0x2E) && (com_data.buff[4] == 0x00)) //报警低阈值读取
	 {
	       send_data.rx_buff[1] = 0x08;
		   send_data.rx_buff[3] = 0x2E;
		   send_data.rx_buff[10] = Iray_command_tail1;
		   send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//20.0度
			{

			  send_data.rx_buff[5] = 0xC8;
		      send_data.rx_buff[6] = 0x00;
			  send_data.rx_buff[7] = 0x00;
			  send_data.rx_buff[8] = 0x00;

			}
	 }
	 else if((com_data.buff[3] == 0x2F) && (com_data.buff[4] == 0x00)) //报警高阈值读取
	 {
	         send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x2F;
			 send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//40.0度
			{

			  send_data.rx_buff[5] = 0x90;
		      send_data.rx_buff[6] = 0x01;
			  send_data.rx_buff[7] = 0x00;
		      send_data.rx_buff[8] = 0x00;

			}
	 }
/*
 * 同电子放大要求的客户要求
 * */
	 else if((com_data.buff[3] == 0x27) && (com_data.buff[4] == 0x00)) //获取整帧最高温度
	 {
		 u16 spot_Y_Pos,spot_X_Pos;
	       send_data.rx_buff[1] = 0x0C;
		   send_data.rx_buff[3] = 0x27;
		   send_data.rx_buff[14] = Iray_command_tail1;
		   send_data.rx_buff[15] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//40.0度
			{
			  temp_convert_display_H((u32 *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX,&capture_temp_int);     //capture实际显示的最大温度值
			  spot_Y_Pos= ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)>>16)&0xFFFF);
			  spot_X_Pos = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)&0xFFFF);

			    send_data.rx_buff[5] = capture_temp_int;//返回结果为八位，前四位是实际温度*10，后四位为坐标点，低位前高位后
		        send_data.rx_buff[6] = capture_temp_int>>8;
				send_data.rx_buff[7] = 0x00;
				send_data.rx_buff[8] = 0x00;

			    send_data.rx_buff[9] = spot_X_Pos;
		        send_data.rx_buff[10] = spot_X_Pos>>8;
				send_data.rx_buff[11] = spot_Y_Pos;
				send_data.rx_buff[12] = spot_Y_Pos>>8;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }
	 else if((com_data.buff[3] == 0x29) && (com_data.buff[4] == 0x00)) //获取整帧最低温度
	 {
		 u16 spot_Y_Pos,spot_X_Pos;
	       send_data.rx_buff[1] = 0x0C;
		   send_data.rx_buff[3] = 0x29;
		   send_data.rx_buff[14] = Iray_command_tail1;
		   send_data.rx_buff[15] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//40.0度
			{

				temp_convert_display_H((u32 *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MIN,&capture_temp_min);     //capture实际显示的最小温度值
				spot_Y_Pos = ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MIN_POS,0))>>16)&0xFFFF;
				spot_X_Pos = ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MIN_POS,0))&0xFFFF);

			    send_data.rx_buff[5] = capture_temp_min;//返回结果为八位，前四位是实际温度*10，后四位为坐标点，低位前高位后
		        send_data.rx_buff[6] = capture_temp_min>>8;
				send_data.rx_buff[7] = 0x00;
				send_data.rx_buff[8] = 0x00;

			    send_data.rx_buff[9] = spot_X_Pos;
		        send_data.rx_buff[10] = spot_X_Pos>>8;
				send_data.rx_buff[11] = spot_Y_Pos;
				send_data.rx_buff[12] = spot_Y_Pos>>8;

			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }
	 else if((com_data.buff[3] == 0x2C) && (com_data.buff[4] == 0x00)) //获取整帧中心温度
	 {
		 u16 spot_X_Pos,spot_Y_Pos;
	       send_data.rx_buff[1] = 0x0C;
		   send_data.rx_buff[3] = 0x2C;
		   send_data.rx_buff[14] = Iray_command_tail1;
		   send_data.rx_buff[15] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//40.0度
			{
			  DisPlaySpot(SpotInit[0].spot_X_Pos,SpotInit[0].spot_Y_Pos,2,0,0);
			  DisplayBackground(SpotInit[0].spot_X_Pos+5,SpotInit[0].spot_Y_Pos+2,10,BACKGROUND_LAYER,0);
			  DisplayBackground(SpotInit[0].spot_X_Pos-58,SpotInit[0].spot_Y_Pos-18,7,BACKGROUND_LAYER,0);

			  SpotInit[0].spot_X_Pos = 320;
			  SpotInit[0].spot_Y_Pos = 256;
			  APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT1_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT1_X_Y__SET,0)&0x00000000)|(( SpotInit[0].spot_Y_Pos<<16)+( SpotInit[0].spot_X_Pos)));//设置测温点的X位置,设置测温点的Y位置
			  temp_convert_display_H(( uint32_t *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_START,&spot_temp_int);     //spot实际显示的温度值

			    send_data.rx_buff[5] = spot_temp_int;//返回结果为八位，前四位是实际温度*10，后四位为坐标点，低位前高位后
		        send_data.rx_buff[6] = spot_temp_int>>8;
				send_data.rx_buff[7] = 0x00;
				send_data.rx_buff[8] = 0x00;

			    send_data.rx_buff[9] =  SpotInit[0].spot_X_Pos;
		        send_data.rx_buff[10] =  SpotInit[0].spot_X_Pos>>8;
				send_data.rx_buff[11] =  SpotInit[0].spot_Y_Pos;
				send_data.rx_buff[12] =  SpotInit[0].spot_Y_Pos>>8;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }

	 else if((com_data.buff[3] == 0x2A) && (com_data.buff[4] == 0x00)) //整帧平均温度读取
	 {
		 u16 Area_Rx,Area_Ry,Area_Lx,Area_Ly,temp1_point;
		 int16_t Area_temp_AVG,temp1_int,temp0_avg;
		 uint8_t Data_8bit= 0;
			int16_t test = 0,temp;
	       send_data.rx_buff[1] = 0x08;
		   send_data.rx_buff[3] = 0x2A;
		   send_data.rx_buff[10] = Iray_command_tail1;
		   send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//40.0度
			{
			  Areainit[0].Area_Lx = 0;
			  Areainit[0].Area_Ly = 0;
			  Areainit[0].Area_Rx = 639;
			  Areainit[0].Area_Ry = 511;
			  APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0)&0x00000000)|(( Areainit[0].Area_Ly<<16)+( Areainit[0].Area_Lx)));//设置测温区域左上X位置，设置测温区域左上Y位置
			  APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0)&0x00000000)|(( Areainit[0].Area_Ry<<16)+ Areainit[0].Area_Rx));//设置测温区域右下X位置，设置测温区域右下Y位置

			Area_temp_AVG	 = read_u32(IO_APB_SLAVE_0_INPUT_AREAN_TEMPAVG_START)/(( Areainit[0].Area_Rx -  Areainit[0].Area_Lx)*( Areainit[0].Area_Ry- Areainit[0].Area_Ly));
			//bsp_printf("Area_temp_AVG  == %x ,SUM = %x \n",Area_temp_AVG,read_u32(IO_APB_SLAVE_0_INPUT_AREAN_TEMPAVG_START));
			temp1_int = Area_temp_AVG>>4;
			Data_8bit = Area_temp_AVG & 0x0f;		     //温度小数部分
			temp1_point = Data_8bit*625;   //小数部分统一乘以10^4 ,保留1位小数
			temp1_point /= 1000;
			//bsp_printf("temp1_point  == %x \n",temp1_point);
			if(temp1_int<0)
			{
				temp0_avg = temp1_int*10 - temp1_point-400;
			}
			else
			{
				temp0_avg = temp1_int*10 + temp1_point-400;
			}   // 温度在写入之前有加40写入寄存器，实际显示时将40减去
			//bsp_printf("temp0_avg  == %x \n",temp0_avg);

			    send_data.rx_buff[5] = temp0_avg;//返回结果为4位，返回值实际温度*10
		        send_data.rx_buff[6] = temp0_avg>>8;
				send_data.rx_buff[7] = 0x00;
				send_data.rx_buff[8] = 0x00;

			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
	 }


/******************************************/
	 else if((com_data.buff[3] == 0xF0) && (com_data.buff[4] == 0x01)) //温宽拉伸开/关
	 {
		     send_data.rx_buff[3] = 0xF0;

		  if(com_data.buff[5] == 0x01)//打开
			{

			    send_data.rx_buff[5] = 0x01;

			}
			else if(com_data.buff[5] == 0x00)//关闭
			{

			    send_data.rx_buff[5] = 0x00;
			}
	 }
		else if((com_data.buff[3] == 0x1D) && (com_data.buff[4] == 0x00)) //温度拉伸低温阈值读
	 {
	         send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x1D;
			 send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//20.0度
			{

			    send_data.rx_buff[5] = 0x40;
		        send_data.rx_buff[6] = 0x0D;
				send_data.rx_buff[7] = 0x03;
				send_data.rx_buff[8] = 0x00;

			}
	 }
	 else if((com_data.buff[3] == 0x1E) && (com_data.buff[4] == 0x00)) //温度拉伸高温阈值读
	 {
	         send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x1E;
			 send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)//20.0度
			{

			    send_data.rx_buff[5] = 0x80;
		        send_data.rx_buff[6] = 0x1A;
				send_data.rx_buff[7] = 0x06;
				send_data.rx_buff[8] = 0x00;

			}
	 }
	/************************校正功能菜单栏(待添加)***********************/
	  else if((com_data.buff[3] == 0x6A) && (com_data.buff[4] == 0x02)) //二次标定生效并保存
	 {
		     send_data.rx_buff[3] = 0x6A;
		  if(com_data.buff[5] == 0x00)//20.0度
			{

			    send_data.rx_buff[5] = 0x01;

			}
	 }
	 else if((com_data.buff[3] == 0x6B) && (com_data.buff[4] == 0x02)) //二次标定清除
	 {
		     send_data.rx_buff[3] = 0x6B;
		  if(com_data.buff[5] == 0x00)//20.0度
			{

			    send_data.rx_buff[5] = 0x01;

			}
	 }
/************************盲元功能菜单栏(部分待添加)***********************/

	 /************************黑体校正功能***********************/
	  else if((com_data.buff[3] == 0x7C) && (com_data.buff[4] == 0x00)) //黑体校正开关状态读取
		{
		     send_data.rx_buff[3] = 0x7C;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x00;
			}
		}
		 else if((com_data.buff[3] == 0x7C) && (com_data.buff[4] == 0x01)) //黑体校正开关设置
		{
		     send_data.rx_buff[3] = 0x7C;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x01;
			}
		}
		else if((com_data.buff[3] == 0x7D) && (com_data.buff[4] == 0x00)) //黑体温度读取
		{
		     send_data.rx_buff[1] = 0x08;
		     send_data.rx_buff[3] = 0x7D;
			 send_data.rx_buff[10] = Iray_command_tail1;
		     send_data.rx_buff[11] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			    send_data.rx_buff[5] = 0x90;
		        send_data.rx_buff[6] = 0xD0;
				send_data.rx_buff[7] = 0x03;
				send_data.rx_buff[8] = 0x00;
			}
		}
		else if((com_data.buff[3] == 0x7E) && (com_data.buff[4] == 0x00)) //黑体坐标读取
		{
		     send_data.rx_buff[1] = 0x0C;
		     send_data.rx_buff[3] = 0x7E;
			 send_data.rx_buff[14] = Iray_command_tail1;
		     send_data.rx_buff[15] = Iray_command_tail2;
		  if(com_data.buff[5] == 0x00)
			{

			  send_data.rx_buff[5] = 0x3E;
		      send_data.rx_buff[6] = 0x01;
			  send_data.rx_buff[7] = 0xFE;
			  send_data.rx_buff[8] = 0x00;
			  send_data.rx_buff[9] = 0x42;
		      send_data.rx_buff[10] = 0x01;
			  send_data.rx_buff[11] = 0x02;
			  send_data.rx_buff[12] = 0x01;
			}
		}
	}
  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
	    {
			bsp_printf("%c",send_data.rx_buff[i]);
		}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void receiveData_6byte(void)
{
  if(com_data.buff[2] == 0x01)
	{
	  send_data.rx_buff[0] = Iray_command_head2;
	  send_data.rx_buff[1] = 0x04;
	  send_data.rx_buff[3] = Iray_command_operation_word;
	  send_data.rx_buff[6] = Iray_command_tail1;
	  send_data.rx_buff[7] = Iray_command_tail2;
 /************************视频菜单栏（部分待添加）***********************/
		if((com_data.buff[3] == 0x4B) && (com_data.buff[4] == 0x01))//警示色域值设置（待添加）
		{
		  send_data.rx_buff[2] = 0x4B;
		 if((com_data.buff[5] == 0xF0) && (com_data.buff[6] == 0x00)) //警示红
		 {

			 send_data.rx_buff[4] = 0x00;
		 }
		 else if((com_data.buff[5] == 0xF0) && (com_data.buff[6] == 0x01)) //警示绿
		 {

			 send_data.rx_buff[4] = 0x01;
		 }
		 else if((com_data.buff[5] == 0x14) && (com_data.buff[6] == 0x02)) //警示蓝
		 {

			 send_data.rx_buff[4] = 0x02;
		 }
	  }
  /************************增益控制菜单栏（部分待添加）***********************/
		else if((com_data.buff[3] == 0x23)&&(com_data.buff[4] == 0x01)) //亮度设置
		{
			send_data.rx_buff[2] = 0x23;
            if(UsermenuParameter.ImageAdjust == 0)
            {
            	if(UsermenuParameter.ImageMode == 0)
                {
            		SystemParaMenu2.Bri_M = com_data.buff[5];
            		if(SystemParaMenu2.Bri_M > 127)
            		{
            			SystemParaMenu2.Bri_M = 127;
            		}
                    APB3_REGW(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ,0)&0xffff0000)|(SystemParaMenu2.Bri_M)));
           		    send_data.rx_buff[4] = SystemParaMenu2.Bri_M ;
                }
             }
             else
             {
                return;
             }
		}
	 /************************高级菜单栏（部分待添加）***********************/
		else if((com_data.buff[3] == 0x77)&&(com_data.buff[4] == 0x02))//波特率设置（待添加）
		{
			send_data.rx_buff[2] = 0x77;
			if((com_data.buff[5] == 0x02) && (com_data.buff[6] == 0x00)) //9600bps
			{
				SystemParaMenu2.BaudLevel = 1;
				Write_TestmenuParameterToFlash();
				send_data.rx_buff[4] = 0x02;
			}
			else if((com_data.buff[5] == 0x04) && (com_data.buff[6] == 0x00)) //19200bps
			{
				SystemParaMenu2.BaudLevel = 2;
				Write_TestmenuParameterToFlash();
				send_data.rx_buff[4] = 0x04;
			}
			else if((com_data.buff[5] == 0x08) && (com_data.buff[6] == 0x00)) //38400bps
			{
				SystemParaMenu2.BaudLevel = 3;
				Write_TestmenuParameterToFlash();
				send_data.rx_buff[4] = 0x08;
			}
			else if((com_data.buff[5] == 0x40) && (com_data.buff[6] == 0x00)) //57600bps
			{
				SystemParaMenu2.BaudLevel = 4;
				Write_TestmenuParameterToFlash();
				send_data.rx_buff[4] = 0x40;
			}
			else if((com_data.buff[5] == 0x10) && (com_data.buff[6] == 0x00)) //115200bps
			{
				SystemParaMenu2.BaudLevel = 5;
				Write_TestmenuParameterToFlash();
				send_data.rx_buff[4] = 0x10;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
			VerifyData(send_data.rx_buff);
			for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
			{
				bsp_printf("%c",send_data.rx_buff[i]);
			}
			memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
			delay_1ms(30);
			uart0_init();
			return;
		}
   	/************************数字视频功能菜单栏(部分待添加)***********************/
		else if((com_data.buff[3] == 0x5D)&&(com_data.buff[4] == 0x02))//数字视频数据接口设置
		{
			send_data.rx_buff[2] = 0x5D;
			if((com_data.buff[5] == 0x03) && (com_data.buff[6] == 0x00)) //LVDS //改为USB2.0
			{
				SystemParaMenu2.Output_mode = 0;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
				send_data.rx_buff[4] = 0x03;
			}
			else if((com_data.buff[5] == 0x02) && (com_data.buff[6] == 0x00)) //LVCOMS  //改为USB3.0
			{
				SystemParaMenu2.Output_mode = 1;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
				send_data.rx_buff[4] = 0x02;
			}
			else if((com_data.buff[5] == 0x04) && (com_data.buff[6] == 0x00)) //BT.656
			{
				SystemParaMenu2.Output_mode = 8;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
				send_data.rx_buff[4] = 0x04;
			}
			else if((com_data.buff[5] == 0x05) && (com_data.buff[6] == 0x00)) //BT.1120  //改为VOT_T
			{
				SystemParaMenu2.Output_mode = 9;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
				send_data.rx_buff[4] = 0x05;
			}
			else if((com_data.buff[5] == 0x05) && (com_data.buff[6] == 0x80)) //CDS_2
			{

				send_data.rx_buff[4] = 0x80;
			}
			else if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x80)) //关闭
			{
				SystemParaMenu2.Output_mode = 15;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS,0)&0xf0ffffff)|(SystemParaMenu2.Output_mode<<24)));
				send_data.rx_buff[4] = 0x00;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}
		}

	}

	else if(com_data.buff[2] == 0x07)
	{
		  send_data.rx_buff[0] = Iray_command_head2;
		  send_data.rx_buff[1] = 0x05;
		  send_data.rx_buff[2] = 0x07;
		  send_data.rx_buff[4] = Iray_command_operation_word;
		  send_data.rx_buff[7] = Iray_command_tail1;
		  send_data.rx_buff[8] = Iray_command_tail2;
		/************************测温参数功能菜单栏(待添加)***********************/
		 if((com_data.buff[3] == 0x05)&&(com_data.buff[4] == 0x01))//低—高增益阈值设置
		 {
		   send_data.rx_buff[3] = 0x05;
		   if((com_data.buff[5] == 0xB0) && (com_data.buff[6] == 0x04))//120
			 {

				 send_data.rx_buff[5] = 0x01;
			 }
		 }
		 else if((com_data.buff[3] == 0x07)&&(com_data.buff[4] == 0x01))//高—低增益阈值设置
		{
		     send_data.rx_buff[3] = 0x07;
			if((com_data.buff[5] == 0x2C) && (com_data.buff[6] == 0x01))//阈值为30.0
			{

			   send_data.rx_buff[5] = 0x01;
			}
		}
		 /************************点测温功能菜单栏(待添加)***********************/
	  else if((com_data.buff[3] == 0x80)&&(com_data.buff[4] == 0x01))//点测温开/关
		{
		    send_data.rx_buff[3] = 0x80;
//		  if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x01))//开
//			{
////			  mPostPara.Cross0_Flag = 1;
////	          Spot1_Set();
//			  send_data.rx_buff[5] = 0x01;
//			}
//			else if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x00))//关
//			{
////			  mPostPara.Cross0_Flag = 0;
////	          Spot1_Set();
//			  send_data.rx_buff[5] = 0x00;
//			}

		    if(com_data.buff[6] == 0x01)//
		    {

		    }
		    else if(com_data.buff[6] == 0x00)//关
		    {

		    }
		}
   /************************区域、线测温功能菜单栏(线模块功能待添加)***********************/
    else if((com_data.buff[3] == 0x40)&&(com_data.buff[4] == 0x01))//区域/线模块打开
		{
		    send_data.rx_buff[3] = 0x40;
		  if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x01))//开
			{

			  send_data.rx_buff[5] = 0x01;
			}
			else if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x00))//关
			{

			  send_data.rx_buff[5] = 0x00;
			}

		}
		 else if((com_data.buff[3] == 0x41)&&(com_data.buff[4] == 0x01))//区域/线类型选择
		 {
		    send_data.rx_buff[3] = 0x41;
		   if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x00))//区域
			{

			  send_data.rx_buff[5] = 0x00;
			}
			else if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x01))//线
			{

			  send_data.rx_buff[5] = 0x01;
			}
		 }
		/************************校正功能菜单栏(待添加)***********************/
		else if((com_data.buff[3] == 0x6F)&&(com_data.buff[4] == 0x02))//二次标定（两点校正）
		{
		    send_data.rx_buff[1] = 0x05;
			send_data.rx_buff[2] = 0x07;
			send_data.rx_buff[3] = 0x6F;
			send_data.rx_buff[4] = Iray_command_operation_word;
			send_data.rx_buff[7] = Iray_command_tail1;
		    send_data.rx_buff[8] = Iray_command_tail2;
			if((com_data.buff[5] == 0x19) && (com_data.buff[6] == 0x00))//25度
			{

				send_data.rx_buff[5] = 0x01;
			}
		}
	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		{
			bsp_printf("%c",send_data.rx_buff[i]);
		}
	    memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void receiveData_7byte(void)
{
  if(com_data.buff[2] == 0x07)
	{
	  send_data.rx_buff[0] = Iray_command_head2;
	  send_data.rx_buff[1] = 0x05;
	  send_data.rx_buff[2] = 0x07;
	  send_data.rx_buff[4] = Iray_command_operation_word;
	  send_data.rx_buff[7] = Iray_command_tail1;
	  send_data.rx_buff[8] = Iray_command_tail2;
	 /************************测温参数功能菜单栏(待添加)***********************/
		  if((com_data.buff[3] == 0x06) && (com_data.buff[4] == 0x01))//低—高增益百分比设置
			{

		    send_data.rx_buff[3] = 0x06;
			  if((com_data.buff[5] == 0x5F) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00))//百分比为95%
				{

				  send_data.rx_buff[5] = 0x01;
				}
			}
		 else if((com_data.buff[3] == 0x08) && (com_data.buff[4] == 0x01))//高—低增益百分比设置
		 {
		      send_data.rx_buff[3] = 0x08;
		   if((com_data.buff[5] == 0x0F) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00))//百分比为15%
			 {

			   send_data.rx_buff[5] = 0x01;
			 }
		 }
	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		{
			bsp_printf("%c",send_data.rx_buff[i]);
		}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

//
void receiveData_8byte(void)
{
  if(com_data.buff[2] == 0x01)
	{
		send_data.rx_buff[0] = Iray_command_head2;
		send_data.rx_buff[3] = Iray_command_operation_word;
		send_data.rx_buff[6] = Iray_command_tail1;
		send_data.rx_buff[7] = Iray_command_tail2;
 /************************视频菜单栏（部分待添加）***********************/
		if((com_data.buff[3] == 0x4F) && (com_data.buff[4] == 0x02))//视频大小设置（待添加）
		{
			send_data.rx_buff[1] = 0x04;
		    send_data.rx_buff[2] = 0x4F;
		 if((com_data.buff[5] == 0xD0) && (com_data.buff[6] == 0x02) && (com_data.buff[7] == 0x40)&& (com_data.buff[8] == 0x02) )//720*576
		 {

			 send_data.rx_buff[4] = 0xD0;
		 }
		 else if((com_data.buff[5] == 0x80) && (com_data.buff[6] == 0x02) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x02) )//640*512
		 {

			 send_data.rx_buff[4] = 0x80;
		 }
	 }

	}

	else if(com_data.buff[2] == 0x07)
	{
		   send_data.rx_buff[0] = Iray_command_head2;
		   send_data.rx_buff[1] = 0x05;
		   send_data.rx_buff[2] = 0x07;
		   send_data.rx_buff[4] = Iray_command_operation_word;
		   send_data.rx_buff[7] = Iray_command_tail1;
		   send_data.rx_buff[8] = Iray_command_tail2;
		 /************************测温参数功能菜单栏(待添加)***********************/
		 if((com_data.buff[3] == 0x0F) && (com_data.buff[4] == 0x01))//反射温度设置
		 {
			 int32_t temp;
		    send_data.rx_buff[3] = 0x0F;
		    send_data.rx_buff[5] = 0x01;
//		   if((com_data.buff[5] == 0xE0) && (com_data.buff[6] == 0x93) && (com_data.buff[7] == 0x04)&& (com_data.buff[8] == 0x00) )//反射温度为30度
//			 {
		      temp =  (com_data.buff[8]<<24)|(com_data.buff[7]<<16)|(com_data.buff[6]<<8)|(com_data.buff[5]<<0);
		      if(temp>>31)
		      {
		    	  temp= ~(temp-1);
		    	  Temp_Parameter.Trefl = 600-(float)(temp/10000)*10; //
		      }
		      else
		      {
		    	  Temp_Parameter.Trefl =(float)(temp/10000)*10+600;
		      }
//				 temp = (u32)com_data.buff[5];
//				 temp|= (u32)com_data.buff[6]<<8;
//				 temp|= (u32)com_data.buff[7]<<16;
//				 temp|= (u32)com_data.buff[8]<<24;
//			 }
		 }
		else if((com_data.buff[3] == 0x10) && (com_data.buff[4] == 0x01))//大气环境温度设置
		{
			int32_t temp;
		    send_data.rx_buff[3] = 0x10;
		    send_data.rx_buff[5] = 0x01;
//		  if((com_data.buff[5] == 0x90) && (com_data.buff[6] == 0xD0) && (com_data.buff[7] == 0x03)&& (com_data.buff[8] == 0x00) )//环境温度为25度
//			{
		      temp =  (com_data.buff[8]<<24)|(com_data.buff[7]<<16)|(com_data.buff[6]<<8)|(com_data.buff[5]<<0);
		      if(temp>>31)
		      {
		    	  temp= ~(temp-1);
		    	  Temp_Parameter.Tamb = 600-(float)(temp/10000)*10; //
		      }
		      else
		      {
		    	  Temp_Parameter.Tamb = (float)(temp/10000)*10+600;
		      }
//				 temp = (u32)com_data.buff[5];
//				 temp|= (u32)com_data.buff[6]<<8;
//				 temp|= (u32)com_data.buff[7]<<16;
//				 temp|= (u32)com_data.buff[8]<<24;
				/* mPostPara.Tamb = ((temp/10000)+60)*10;*/

//			}
		}
		else if((com_data.buff[3] == 0x11) && (com_data.buff[4] == 0x01))//大气透过率设置   ---相对湿度？！
		{
			u32 temp;
		    send_data.rx_buff[3] = 0x11;
	//	  if((com_data.buff[5] == 0x94) && (com_data.buff[6] == 0x11) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x00) )//透过率为0.45
			{
		    	temp = (com_data.buff[8]<<24)|(com_data.buff[7]<<16)|(com_data.buff[6]<<8)|(com_data.buff[5]<<0);
		    	Temp_Parameter.RelHum = temp/100;
			  send_data.rx_buff[5] = 0x01;
			}
		}
		else if((com_data.buff[3] == 0x12) && (com_data.buff[4] == 0x01))//发射率设置 ---辐射率
		{
			u32 temp;
		    send_data.rx_buff[3] = 0x12;
		  //if((com_data.buff[5] == 0x48) && (com_data.buff[6] == 0x26) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x00) )
			{
		    	temp = (com_data.buff[8]<<24)|(com_data.buff[7]<<16)|(com_data.buff[6]<<8)|(com_data.buff[5]<<0);
		    	Temp_Parameter.Emiss = temp/100;
		    	send_data.rx_buff[5] = 0x01;
			}
		}
		else if((com_data.buff[3] == 0x13) && (com_data.buff[4] == 0x01))//距离设置
		{
			  u32 distance;
		      send_data.rx_buff[3] = 0x13;
//		  if((com_data.buff[5] == 0x60) && (com_data.buff[6] == 0xEA) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x00) )
//			{
		      distance = (com_data.buff[8]<<24)|(com_data.buff[7]<<16)|(com_data.buff[6]<<8)|(com_data.buff[5]<<0);
		      Temp_Parameter.distance = distance/1000;
//				 distance = (u32)com_data.buff[5];
//				 distance|= (u32)com_data.buff[6]<<8;
//				 distance|= (u32)com_data.buff[7]<<16;
//				 distance|= (u32)com_data.buff[8]<<24;
//				 mPostPara.distance = ((distance/10000))*10;
			     send_data.rx_buff[5] = 0x01;
//			}
		}
 	 /************************整帧测温功能(部分待添加)***********************/
		else if((com_data.buff[3] == 0x2E) && (com_data.buff[4] == 0x01))//报警低阈值设置
		{
		    send_data.rx_buff[3] = 0x2E;
		  if((com_data.buff[5] == 0xC8) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x00) )//20.0度
			{

			  send_data.rx_buff[5] = 0x01;
			}

		}
		else if((com_data.buff[3] == 0x2F) && (com_data.buff[4] == 0x01))//报警高阈值设置
		{
		    send_data.rx_buff[3] = 0x2F;
		  if((com_data.buff[5] == 0x90) && (com_data.buff[6] == 0x01) && (com_data.buff[7] == 0x00)&& (com_data.buff[8] == 0x00) )//40.0度
			{

			  send_data.rx_buff[5] = 0x01;
			}

		}
		else if((com_data.buff[3] == 0x1D) && (com_data.buff[4] == 0x01))//温度拉伸低温阈值写
		{
		    send_data.rx_buff[3] = 0x1D;
		  if((com_data.buff[5] == 0x40) && (com_data.buff[6] == 0x0D) && (com_data.buff[7] == 0x03)&& (com_data.buff[8] == 0x00) )//20.0度
			{

			  send_data.rx_buff[5] = 0x01;
			}

		}
		else if((com_data.buff[3] == 0x1E) && (com_data.buff[4] == 0x01))//温度拉伸高温阈值写
		{
		    send_data.rx_buff[3] = 0x1E;
		  if((com_data.buff[5] == 0x80) && (com_data.buff[6] == 0x1A) && (com_data.buff[7] == 0x06)&& (com_data.buff[8] == 0x00) )//40.0度
			{

			  send_data.rx_buff[5] = 0x01;
			}

		}
/************************盲元功能菜单栏(部分待添加)***********************/

	 /************************黑体校正功能***********************/
		else if((com_data.buff[3] == 0x7D) && (com_data.buff[4] == 0x01))//黑体温度设置
		{
		    send_data.rx_buff[3] = 0x7D;
		  if((com_data.buff[5] == 0x80) && (com_data.buff[6] == 0x1A) && (com_data.buff[7] == 0x06)&& (com_data.buff[8] == 0x00) )
			{

			  send_data.rx_buff[5] = 0x01;
			}

		}
	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		{
			bsp_printf("%c",send_data.rx_buff[i]);
		}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void receiveData_9byte(void)
{
  if(com_data.buff[2] == 0x01)
	{
	   send_data.rx_buff[0] = Iray_command_head2;
	   send_data.rx_buff[3] = Iray_command_operation_word;
	   send_data.rx_buff[6] = Iray_command_tail1;
	   send_data.rx_buff[7] = Iray_command_tail2;
		 	/************************视频菜单栏***********************/
//		if(((mPostPara.BAD_Cross0_Flag == 1)&&(mPostPara.Cross0_Flag == 0))||((Menu1State == 11)&&(mPostPara.Cross0_Flag == 1)))
//		{

		  if((com_data.buff[3] == 0x44) && (com_data.buff[4] == 0x02)) //十字光标移动
		  {
			  if(Spot_state)
			  {
				 send_data.rx_buff[1] = 0x04;
		         send_data.rx_buff[2] = 0x44;
//				 Spot1_INC();
			  if((com_data.buff[5] == 0x06) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//上移小步长
				{
//				    Cross0_Y_INC();
					send_data.rx_buff[4] = 0x06;
				}
				 else if((com_data.buff[5] == 0x86)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//上移大步长
				{
//					Cross0_Y_INC_more();
					send_data.rx_buff[4] = 0x86;
				}
				else if((com_data.buff[5] == 0x07)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//下移小步长
				{
//				    Cross0_Y_DEC();
					send_data.rx_buff[4] = 0x07;
				}
				else if((com_data.buff[5] == 0x87)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//下移大步长
				{
//				    Cross0_Y_DEC_more();
					send_data.rx_buff[4] = 0x87;
				}
				else if((com_data.buff[5] == 0x08)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//左移小步长
				{
//				    Cross0_X_DEC();
					send_data.rx_buff[4] = 0x08;
				}
				else if((com_data.buff[5] == 0x88)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//左移大步长
				{
//				    Cross0_X_DEC_more();
					send_data.rx_buff[4] = 0x88;
				}
				else if((com_data.buff[5] == 0x09)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//右移小步长
				{
//				    Cross0_X_INC();
					send_data.rx_buff[4] = 0x09;
				}
				else if((com_data.buff[5] == 0x89)&& (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x00))//右移大步长
				{
//				    Cross0_X_INC_more();
					send_data.rx_buff[4] = 0x89;
				}
				else if(com_data.buff[5] == 0x05)
				{
					if((com_data.buff[6] == 0x40) && (com_data.buff[7] == 0x01) && (com_data.buff[8] == 0x00) && (com_data.buff[9] == 0x01))//移动到中心（640*512）
					{
//				     Set_Position_Area_Spot_Center();
					 send_data.rx_buff[4] = 0x40;
					}
					else if((com_data.buff[6] == 0x68) && (com_data.buff[7] == 0x01) && (com_data.buff[8] == 0xF0) && (com_data.buff[9] == 0x00))//移动到中心（720*576）(待添加)
					{
						send_data.rx_buff[4] = 0x68;
					}
					else//十字光标位置（位置设置）
					{
//                   	mPostPara.Cross0_x = ((uint16_t)com_data.buff[7]<<8)|com_data.buff[6];
//					 	mPostPara.Cross0_y = ((uint16_t)com_data.buff[9]<<8)|com_data.buff[8];
//					 	Spot1_Set();

						send_data.rx_buff[4] = 0x01;
					}
				}
				else
				{
					return;
				}
			  }
		 }
//	 }
	}

	else if(com_data.buff[2] == 0x07)
  {
		   send_data.rx_buff[0] = Iray_command_head2;
		   send_data.rx_buff[4] = Iray_command_operation_word;
		   send_data.rx_buff[7] = Iray_command_tail1;
		   send_data.rx_buff[8] = Iray_command_tail2;
		/************************点测温功能菜单栏***********************/
//		if((Menu1State == 11)&&(mPostPara.Cross0_Flag == 1))
//		{
	  if((com_data.buff[3] == 0x82) && (com_data.buff[4] == 0x01)) //点坐标设置
		{
		    send_data.rx_buff[1] = 0x05;
		    send_data.rx_buff[2] = 0x07;
		    send_data.rx_buff[3] = 0x82;
		  if(com_data.buff[5] == 0x00)
			{
//			    mPostPara.Cross0_x = ((uint16_t)com_data.buff[7]<<8)|com_data.buff[6];
//				mPostPara.Cross0_y = ((uint16_t)com_data.buff[9]<<8)|com_data.buff[8];
//				Spot1_Set();
			    send_data.rx_buff[5] = 0x01;
			}
		}
//	 }
	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		{
			bsp_printf("%c",send_data.rx_buff[i]);
		}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}


void receiveData_12byte(void)
{
  if(com_data.buff[2] == 0x01)
	{
		send_data.rx_buff[0] = Iray_command_head2;
		send_data.rx_buff[3] = Iray_command_operation_word;
		send_data.rx_buff[6] = Iray_command_tail1;
		send_data.rx_buff[7] = Iray_command_tail2;
		/************************视频菜单栏（部分待添加）***********************/
		if((com_data.buff[3] == 0x40)&& (com_data.buff[4] == 0x02))//电子放大功能
		{
			send_data.rx_buff[1] = 0x04;
			send_data.rx_buff[2] = 0x40;
			if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x00)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x7F) &&(com_data.buff[10] == 0x02)
				&& (com_data.buff[11] == 0xFF)&&(com_data.buff[12] == 0x01))   //1.0倍
			{
#ifndef SUPPORT_CONTINUOUS_ZOOM
				UsermenuParameter.Enlarge = 0;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#else
				UsermenuParameter.Enlarge = 10;
				APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
#endif
				send_data.rx_buff[4] = 0x01;
			}
			else if((com_data.buff[5] == 0xA0) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x80)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0xDF) &&(com_data.buff[10] == 0x01)
				&& (com_data.buff[11] == 0x7F)&&(com_data.buff[12] == 0x01))   //2.0倍
			{
#ifndef SUPPORT_CONTINUOUS_ZOOM
				UsermenuParameter.Enlarge = 1;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#else
				UsermenuParameter.Enlarge = 20;
				APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
#endif
				send_data.rx_buff[4] = 0x02;
			}
/*
 * 客户定制，3倍指令为放大4倍，4倍指令为放大8倍
 * */
			else if((com_data.buff[5] == 0xD5) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0xAB)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0xA9) &&(com_data.buff[10] == 0x01)
				&& (com_data.buff[11] == 0x54)&&(com_data.buff[12] == 0x01))   //4.0倍
			{
#ifndef SUPPORT_CONTINUOUS_ZOOM
				UsermenuParameter.Enlarge = 2;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#else
				UsermenuParameter.Enlarge = 40;
				APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
#endif
				send_data.rx_buff[4] = 0x03;
			}
			else if((com_data.buff[5] == 0xF0) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0xC0)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x8F) &&(com_data.buff[10] == 0x01)
					&& (com_data.buff[11] == 0x3F)&&(com_data.buff[12] == 0x01))   //8.0倍
			{
#ifndef SUPPORT_CONTINUOUS_ZOOM
				UsermenuParameter.Enlarge = 3;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
#else
				UsermenuParameter.Enlarge = 80;
				APB3_REGW_ZOOM(UsermenuParameter.Enlarge);
#endif
				send_data.rx_buff[4] = 0x04;
			}
			else
			{
				MessageReturnError(CommandNotExist);
				return;
			}

/*
 * 原正常指令，暂时屏蔽
 * */
#if 0

//			else if((com_data.buff[5] == 0xD5) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0xAB)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0xA9) &&(com_data.buff[10] == 0x01)
//				&& (com_data.buff[11] == 0x54)&&(com_data.buff[12] == 0x01))   //3.0倍
//			{
//				mTXTPara.Ir_zoom = 3;
//				IrZoom_set();
//				send_data.rx_buff[4] = 0x03;
//			}
			else if((com_data.buff[5] == 0xF0) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0xC0)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x8F) &&(com_data.buff[10] == 0x01)
				&& (com_data.buff[11] == 0x3F)&&(com_data.buff[12] == 0x01))   //4.0倍
			{
				UsermenuParameter.Enlarge = 2;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
				send_data.rx_buff[4] = 0x04;
			}
//			else if((com_data.buff[5] == 0x00) && (com_data.buff[6] == 0x01) && (com_data.buff[7] == 0xCD)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x7F) &&(com_data.buff[10] == 0x01)
//				&& (com_data.buff[11] == 0x32)&&(com_data.buff[12] == 0x01))   //5.0倍
//			{
//				mTXTPara.Ir_zoom = 5;
//				IrZoom_set();
//				send_data.rx_buff[4] = 0x05;
//			}
//			else if((com_data.buff[5] == 0x0B) && (com_data.buff[6] == 0x01) && (com_data.buff[7] == 0xD5)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x74) &&(com_data.buff[10] == 0x01)
//				&& (com_data.buff[11] == 0x29)&&(com_data.buff[12] == 0x01))   //6.0倍
//			{
//				mTXTPara.Ir_zoom = 6;
//				IrZoom_set();
//				send_data.rx_buff[4] = 0x06;
//			}
//			else if((com_data.buff[5] == 0x12) && (com_data.buff[6] == 0x01) && (com_data.buff[7] == 0xDB)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x6C) &&(com_data.buff[10] == 0x01)
//				&& (com_data.buff[11] == 0x23)&&(com_data.buff[12] == 0x01))   //7.0倍
//			{
//				mTXTPara.Ir_zoom = 7;
//				IrZoom_set();
//				send_data.rx_buff[4] = 0x07;
//			}
			else if((com_data.buff[5] == 0x18) && (com_data.buff[6] == 0x01) && (com_data.buff[7] == 0xE0)  && (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0x67) &&(com_data.buff[10] == 0x01)
				&& (com_data.buff[11] == 0x1F)&&(com_data.buff[12] == 0x01))   //8.0倍
			{
				UsermenuParameter.Enlarge = 3;
				APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0xffffe7ff)|(UsermenuParameter.Enlarge<<11)));
				send_data.rx_buff[4] = 0x08;
			}
#endif
		}
//	 /************************增益控制菜单栏（部分待添加）***********************/
//		else if((com_data.buff[3] == 0x2B)&& (com_data.buff[4] == 0x01))//感兴趣区域设置
//		{
//			 send_data.rx_buff[1] = 0x04;
//			 send_data.rx_buff[2] = 0x2B;
//
//		    /*mPostPara.Area1_Flag = 1;
//			mPostPara.Area1_Lx = ((uint16_t)com_data.buff[6]<<8)|com_data.buff[5];
//		    mPostPara.Area1_Ly = ((uint16_t)com_data.buff[8]<<8)|com_data.buff[7];
//		    mPostPara.Area1_Rx = ((uint16_t)com_data.buff[10]<<8)|com_data.buff[9];
//		    mPostPara.Area1_Ry = ((uint16_t)com_data.buff[12]<<8)|com_data.buff[11];*/
////			Area1_Set();
//			send_data.rx_buff[4] = 0x01;
//
//
//		}
	}
//
//	else if(com_data.buff[2] == 0x07)
//  {
//
//		   send_data.rx_buff[0] = 0x55;
//		   send_data.rx_buff[4] = 0x33;
//		   send_data.rx_buff[7] = 0xEB;
//		   send_data.rx_buff[8] = 0xAA;
//
///************************盲元功能菜单栏(部分待添加)***********************/
//
//	 /************************黑体校正功能***********************/
//	  if((com_data.buff[3] == 0x7E) && (com_data.buff[4] == 0x01)) //黑体坐标设置
//		{
//		    send_data.rx_buff[1] = 0x05;
//		    send_data.rx_buff[2] = 0x07;
//		    send_data.rx_buff[3] = 0x7E;
//		  if((com_data.buff[5] == 0xBE) && (com_data.buff[6] == 0x00) && (com_data.buff[7] == 0x8C)&& (com_data.buff[8] == 0x00)&& (com_data.buff[9] == 0xC8)&&(com_data.buff[10] == 0x00)
//				&& (com_data.buff[11] == 0x96)&&(com_data.buff[12] == 0x00))
//			{
//
//			  send_data.rx_buff[5] = 0x01;
//			}
//		}
//	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
	    for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
	    {
	       bsp_printf("%c",send_data.rx_buff[i]);
	    }
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));

}


void receiveData_13byte(void)
{
	uint8_t i;
  if(com_data.buff[2] == 0x07)
	{
	   send_data.rx_buff[0] = Iray_command_head2;
	   send_data.rx_buff[4] = Iray_command_operation_word;
	   send_data.rx_buff[7] = Iray_command_tail1;
	   send_data.rx_buff[8] = Iray_command_tail2;
   /************************区域、线测温功能菜单栏(线模块功能待添加)***********************/
		if((com_data.buff[3] == 0x42)&& (com_data.buff[4] == 0x01))//区域、线模块坐标设置
	 {
		    send_data.rx_buff[1] = 0x05;
			send_data.rx_buff[2] = 0x07;
			send_data.rx_buff[3] = 0x42;

		    if(com_data.buff[5]<=0x0B)
		   {
//		      mPostPara.Area1_Flag = 1;
//
//			  mPostPara.Area1_Lx = ((uint16_t)com_data.buff[7]<<8)|com_data.buff[6];
//		      mPostPara.Area1_Ly = ((uint16_t)com_data.buff[9]<<8)|com_data.buff[8];
//		      mPostPara.Area1_Rx = ((uint16_t)com_data.buff[11]<<8)|com_data.buff[10];
//		      mPostPara.Area1_Ry = ((uint16_t)com_data.buff[13]<<8)|com_data.buff[12];
//			  Area1_Set();
		      send_data.rx_buff[5] = com_data.buff[5];
		   }
	 }
	}

  	VerifyData(send_data.rx_buff);
	send_data.rx_buff[send_data.rx_buff[1]+1] = IrayTools.CHECKSUM;
		for(uint8_t i = 0; i < send_data.rx_buff[1]+4; i++)
		{
			bsp_printf("%c",send_data.rx_buff[i]);
		}
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void VerifyData(u8 *DRCBUFF)
{
	u8 LENGTH;
	LENGTH = DRCBUFF[1];
	IrayTools.CHECKSUM = 0;
	for(IrayTools.COUNT = 0 ;IrayTools.COUNT <= LENGTH ;IrayTools.COUNT++)
	{
		IrayTools.CHECKSUM += DRCBUFF[IrayTools.COUNT];  //
    }
	IrayTools.CHECKSUM = IrayTools.CHECKSUM % 0x100;
}
#endif
