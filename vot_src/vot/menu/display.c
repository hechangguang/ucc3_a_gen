/*
 * display.c
 *
 *  Created on: 2024年3月12日
 *      Author: Administrator
 */

#include"display.h"
#include "fontlib.h"
#include <menu/testmenufunction.h>
#include <snr_comm_vot.h>
#include <fontlib_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>


extern SysCtrlInfo_s_t global_sys_ctrl_info;

#define TXT_BASE_ADDR     0x1100000

#ifdef CONFIG_SNR_PLANCK1280C
#define IR_X_pixel 1280
#define IR_Y_pixel 1024
#else
#define IR_X_pixel 640
#define IR_Y_pixel 512
#endif
ts_u08 SOC_WR_Hyram_Flag = 1;
/**
 * @brief:SOC访问Hyram,并向FPGA传输信号
 * @parameter:Switch:0:SOC访问访问Hyram时,数据量小可以同步刷新字符与图像数据,
 * 					>0:SOC访问访问Hyram时,数据量大需要分别刷新字符与图像数据,
 */
void  SOC_ReadAndWrite_Hyram(ts_u08 Switch)
{
#if (SOC_WriteAndRead_Hyram == 1)
	if(SOC_WR_Hyram_Flag)
	{
		if(Switch)
		{
			SystemParaMenu2.test_state = 1;
//			APB3_REGW_TEST_SET(SystemParaMenu2.test_state);
		}
		else
		{
			SystemParaMenu2.test_state = 0;
//			APB3_REGW_TEST_SET(SystemParaMenu2.test_state);
		}
	}
#endif
}

/**************************在内存交互模块中显示字符数据的功能***************************************************/
/**
 * @brief:字符数据传输到内存交互中
 * @parameter:buf[]:字符数据数组，需传输的数据
 * @parameter:fontaddr：内存交互地址
 */
void Tran_FontDataToRAM(ts_u08 buf[],ts_u32 fontaddr)
{
// 	ts_u32 fontdata = 0;
// 	for(ts_u08 num = 0;num<4;num++)
// 	{
// 		fontdata = (buf[6+num*4]<<24)+(buf[5+num*4]<<16)+(buf[4+num*4]<<8)+buf[3+num*4];
// //		write_u32(fontdata,fontaddr);
// 		fontaddr = fontaddr + 0x04;
// 	}
}

/**
 * @brief:根据ASCII码从储存的打点数据中找到对应的字符数据
 * @parameter:identify_1：字符辨识ID
 * @parameter:identify_2：字符辨识ID
 * @parameter:identify_3：字符辨识ID
 */
ts_u16 FindCharactersID(ts_u08 identify_1, ts_u08 identify_2,ts_u08 identify_3)
{
	ts_u16 i;
	ts_u08 tmpBit[3];

	i=0;
	while(i<400)
	{
		tmpBit[0]=Font_TAB[i][0];
		tmpBit[1]=Font_TAB[i][1];
		tmpBit[2]=Font_TAB[i][2];
		if((tmpBit[0]==255) && (tmpBit[1]==255)&&(tmpBit[2]==255)) break;
		if((tmpBit[0]==identify_1) && (tmpBit[1]==identify_2)&&(tmpBit[2]==identify_3)) return i;
		i++;
	}
	return 0;
}

/**
 * @brief:在内存交互模块中显示字符数据
 * @parameter:*pData：指向字符数据的指针
 * @parameter:offset：字符显示在内存交互地址中的偏移量
 */
void DisPlayNewFont(ts_u08 *pData,ts_u08 offset)
{
	ts_u16 i,FontNumber;
	i=0;
	ts_u32 addrbase = 0xf8100400 + offset*16;
	while(pData[i]!=0)
	{
		if(pData[i]<128)//128
		{
			FontNumber=FindCharactersID(0,0,pData[i]);
			i++;
		}
		else
		{
			break;
		}
		Tran_FontDataToRAM((ts_u08 *)Font_TAB[FontNumber],addrbase);
		addrbase = addrbase + 0x10;
	}
}

/**
 * @brief:在内存交互模块中清除字符数据
 * @parameter:xstart：需要清除的字符的起始地址偏移量
 * @parameter:xend：需要清除的字符的结束地址偏移量
 */
void ClearFont(ts_u08 xstart,ts_u08 xend)
{
	return;
	ts_u32 fontaddress = 0xf8100400 + xstart*16;
	for(ts_u16 i=xstart*16;i<xend*16;i++)
	{
//		write_ts_u08(0,fontaddress);
		fontaddress = fontaddress+0x01;
	}
}

/****************************************************************************************************/


/********************************在DDR中显示字符或汉字数据的功能********************************************/
/**
 * @brief:根据汉字识别码从储存的打点数据中找到对应的汉字数据,UTF-8编码格式，汉字占三个字节,需要三位识别码
 * @parameter:identify_1：汉字辨识ID
 * @parameter:identify_2：汉字辨识ID
 * @parameter:identify_3：汉字辨识ID
 */
ts_u16 FindFontNumber(ts_u08 identify_1, ts_u08 identify_2,ts_u08 identify_3)
{
	ts_u16 i;
	ts_u08 tmpBit[3];

	i=0;
	while(i<400)
	{
		tmpBit[0]=HZ_TAB[i][0];
		tmpBit[1]=HZ_TAB[i][1];
		tmpBit[2]=HZ_TAB[i][2];
		if((tmpBit[0]==255) && (tmpBit[1]==255)&&(tmpBit[2]==255)) break;
		if((tmpBit[0]==identify_1) && (tmpBit[1]==identify_2)&&(tmpBit[2]==identify_3)) return i;
		i++;
	}
	return 0;
}

/**
 * @brief:显示大小为16*16的字符或图标数据
 * @parameter:buf：字符或图标数据
 * @parameter:X_LINE：显示的X轴位置
 * @parameter:Y_LINE：显示的Y轴位置
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisPlaySize16X16(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 layer,ts_u08 color)
{
	ts_u08 i,j;
	ts_u32 addrc;
	ts_u08 data1,data2;

	#ifdef CONFIG_SNR_PLANCK1280C
	{
		for(i=0;i<32;i++)
		{
			data1 = buf[i*2+3];	//去掉前三位识别码开始算
			data2 = buf[i*2+4];
			for(j=0;j<32;j++)
			{
				
				addrc = TXT_BASE_ADDR + 1280*(j+Y_LINE);
				if(j<16)
				{
					if(data1&0x80)
					{
						Select_CharacterLayer(layer,color,addrc+(i+X_LINE)*2);
					}
					data1 = data1<<1;
				}
				else
				{
					if(data2&0x80)
					{
						Select_CharacterLayer(layer,color,addrc+(i+X_LINE)*2);
					}
					data2 = data2<<1;
				}
			}
		}
	}
	#else
	{
		for(i=0;i<16;i++)
		{
			data1 = buf[i*2+3];	//去掉前三位识别码开始算
			data2 = buf[i*2+4];
			for(j=0;j<16;j++)
			{
				addrc = TXT_BASE_ADDR + 640*(j+Y_LINE);
				if(j<8)
				{
					if(data1&0x80)
					{
						Select_CharacterLayer(layer,color,addrc+(i+X_LINE)*2);
					}
					data1 = data1<<1;
				}
				else
				{
					if(data2&0x80)
					{
						Select_CharacterLayer(layer,color,addrc+(i+X_LINE)*2);
					}
					data2 = data2<<1;
				}
			}
		}
	}
	#endif
}

/**
 * @brief:显示大小为16*8的字符或图标数据
 * @parameter:buf：字符或图标数据
 * @parameter:X_LINE：显示的X轴位置
 * @parameter:Y_LINE：显示的Y轴位置
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisPlaySize16X8(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 layer,ts_u08 color)
{
	ts_u08 i,j;
	ts_u32 addrc;
	ts_u08 data1;
	#ifdef CONFIG_SNR_PLANCK1280C 
	{
		for(i=0;i<32;i++)//列
		{
			data1 = buf[i+3];
			addrc = TXT_BASE_ADDR + 1280*(i+Y_LINE)+X_LINE*2;
			for(j=0;j<16;j++)//行
			{
				if(data1&0x80)
				{
					Select_CharacterLayer(layer,color,addrc+j*2);
				}
				data1 = data1<<1;
			}
		}
	}
	#else
	{
		for(i=0;i<16;i++)//列
		{
			data1 = buf[i+3];
			addrc = TXT_BASE_ADDR + 640*(i+Y_LINE)+X_LINE*2;
			for(j=0;j<8;j++)//行
			{
				if(data1&0x80)
				{
					Select_CharacterLayer(layer,color,addrc+j*2);
				}
				data1 = data1<<1;
			}
		}
	}
	#endif
}

/**
 * @brief:显示字符串数据
 * @parameter:X_LINE：显示的X轴位置
 * @parameter:Y_LINE：显示的Y轴位置
 * @parameter:*pData：字符串数据
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisPlayNewStr(ts_u32 X_LINE,ts_u32 Y_LINE,const ts_char *pData_str,ts_u08 layer,ts_u08 color)
{
	// ts_u08 *pData = (ts_u08*)pData_str;
#if 0
	ts_u08 isENorCHflag;
	ts_u16 i,FontNumber;
	i=0;

	SOC_ReadAndWrite_Hyram(1);

	while(pData[i]!=0)
	{
		if(pData[i]<128)//128
		{
			FontNumber=FindCharactersID(0,0,pData[i]);//utf-8编码格式，字符占一个字节，使用三个字节表示为了与汉字对齐
			i++;
			isENorCHflag = 0;
		}
		else
		{
			if(pData[i+1]!=0)
			{

				FontNumber=FindFontNumber(pData[i],pData[i+1],pData[i+2]);//utf-8编码格式，汉字占三个字节
				i=i+3;
				isENorCHflag = 1;
			}
			else
				break;
		}
		if(isENorCHflag)
		{
			DisPlaySize16X16((ts_u08 *)HZ_TAB[FontNumber],X_LINE,Y_LINE,layer,color);
			X_LINE = X_LINE+16;			//X轴前进16（地址偏移量+16）
		}
		else
		{
			DisPlaySize16X8((ts_u08 *)Font_TAB[FontNumber],X_LINE,Y_LINE,layer,color);
			X_LINE = X_LINE+8;			//X轴前进8（地址偏移量+8）
		}
	}

	SOC_ReadAndWrite_Hyram(0);
#endif
}

/**
 * @brief:获得当前字符串的长度，输出的长度 = 汉字数量*2 + 字符数量
 * @parameter:*Str：当前字符串
 * @Return:(ChineseNum*2+CharacterNum):字符串的长度
 */
ts_u08 GetStringSize(const  char *Str)
{
	ts_u08 ChineseNum = 0;
	ts_u08 CharacterNum = 0;
	ts_u16 Lengths = 0;
	while(Str[Lengths]!=0)
	{
		if(Str[Lengths]<128)//128
		{
			if(FindCharactersID(0,0,Str[Lengths]))
			{
				CharacterNum++;
			}
			Lengths = Lengths+1;
		}
		else
		{
			if(Str[Lengths+1]!=0)
			{
				if(FindFontNumber(Str[Lengths],Str[Lengths+1],Str[Lengths+2]))
				{
					ChineseNum++;
				}
				Lengths=Lengths+3;
			}
			else
				break;
		}
	}

	return (ChineseNum*2+CharacterNum);
}

/******************************************************************************************************/

/**
 * @brief:清除显示字符数据,高度：22px,长度：自定义
 * @parameter:X_start：X轴起始位置
 * @parameter:X_offset：X轴偏移量
 * @parameter:Y_start：Y轴起始位置
 */
void ClearSize16X22(ts_u16 X_start,ts_u16 X_offset,ts_u16 Y_start)
{
// 	int i,j;
// 	ts_u32 addr;

// //	SOC_ReadAndWrite_Hyram(1);

// 	for(j=Y_start-3;j<Y_start+22;j++)
// 	{
// 		for(i=X_start;i<X_start+X_offset;i++)
// 		{
// 			addr = TXT_BASE_ADDR + 1280*j + i*2;
// //			write_ts_u16(0,addr);
// 		}
// 	}
//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:清除显示字符数据,高度：16px,长度：自定义
 * @parameter:X_start：X轴起始位置
 * @parameter:X_offset：X轴偏移量
 * @parameter:Y_start：Y轴起始位置
 */
void ClearSize16X16(ts_u16 X_start,ts_u16 X_offset,ts_u16 Y_start)
{
	return;
// 	int i,j;
// 	ts_u32 addr;

// //	SOC_ReadAndWrite_Hyram(1);
// 	for(j=Y_start;j<Y_start+17;j++)
// 	{
// 		for(i=X_start;i<X_start+X_offset;i++)
// 		{
// 			addr = TXT_BASE_ADDR + 1280*j + i*2;
// //			write_ts_u16(0,addr);
// 		}
// 	}
//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:X轴画一条线，长度自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisplayXLine(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color)
{
	// ts_u16 i;
	// ts_u32 addr;

	// addr = TXT_BASE_ADDR + 640*2*y;
	// for(i=0;i<len;i++)
	// {
	// 	Select_CharacterLayer(layer,color,addr+(i+x)*2);
	// }
}

/**
 * @brief:Y轴画一条线，长度自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisplayYLine(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color)
{
	// ts_u16 i;
	// ts_u32 addr;
	// addr = TXT_BASE_ADDR + 640*2*y+x*2;
	// for(i=0;i<len;i++)
	// {
	// 	Select_CharacterLayer(layer,color,addr+i*1280);
	// }
}


/**
 * @brief:显示选中的菜单项框架：高度：22px,长度：自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisplayMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color)
{

//	SOC_ReadAndWrite_Hyram(1);

	DisplayXLine(x,y-3,len,layer,color);
	DisplayXLine(x,y+19,len,layer,color);
	DisplayYLine(x,y-3,23,layer,color);
	DisplayYLine(x+len,y-3,23,layer,color);

//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:显示三角形与外框：高度：22px,长度：自定义,用于384一级菜单选择
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:Type：显示的三角形类型
 * @parameter:color：字符颜色
 */
void DisplayTriangleFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 Type,ts_u08 color)
{
	DisplayMenuFrame(x,y,len,3,color);
//	SOC_ReadAndWrite_Hyram(1);
	if(Type)
	{
		DisPlay16((ts_u08 *)IR_TAB[8+Type],x+2,y,3,color);
	}
	else
	{
		DisPlay16((ts_u08 *)IR_TAB[8+Type],x+4,y,3,color);
	}
//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:显示选中的菜单项框架：高度：22px,长度：自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisplaySelectionMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color)
{
//	SOC_ReadAndWrite_Hyram(1);

	DisplayXLine(x,y-3,len,layer,color);
	DisplayXLine(x,y-2,len,layer,color);
	DisplayXLine(x,y+19,len,layer,color);
	DisplayXLine(x,y+18,len,layer,color);

	DisplayYLine(x,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+1,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+2,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+3,y-3,Y_GAP+1,layer,color);

	DisplayYLine(x+len-3,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+len-2,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+len-1,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+len,y-3,Y_GAP+1,layer,color);

//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:清除显示选中的菜单项框架：高度：22px,长度：自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void ClearSelectionMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color)
{
//	SOC_ReadAndWrite_Hyram(1);

	DisplayXLine(x,y-2,len,layer,0);
	DisplayXLine(x,y+18,len,layer,0);

	DisplayYLine(x+1,y-3,Y_GAP+1,layer,0);
	DisplayYLine(x+2,y-3,Y_GAP+1,layer,0);
	DisplayYLine(x+3,y-3,Y_GAP+1,layer,0);
	DisplayYLine(x+len-1,y-3,Y_GAP+1,layer,0);
	DisplayYLine(x+len-2,y-3,Y_GAP+1,layer,0);
	DisplayYLine(x+len-3,y-3,Y_GAP+1,layer,0);

	DisplayXLine(x,y-3,len,layer,color);
	DisplayXLine(x,y+19,len,layer,color);
	DisplayYLine(x,y-3,Y_GAP+1,layer,color);
	DisplayYLine(x+len,y-3,Y_GAP+1,layer,color);

//	SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:显示菜单背景颜色：高度：16px,长度：自定义
 * @parameter:x：显示的X轴位置
 * @parameter:y：显示的Y轴位置
 * @parameter:len：长度
 * @parameter:layer：显示的层级
 * @parameter:color：字符颜色
 */
void DisplayBackgroundColor(ts_u16 x, ts_u16 y, ts_u16 len, ts_u08 layer,ts_u08 color)
{
   ts_u16 i,j;
   ts_u32 addr;

//   SOC_ReadAndWrite_Hyram(1);
	#ifdef CONFIG_SNR_PLANCK1280C
	{
	 	for(j=0;j<32;j++)
		{
			addr = TXT_BASE_ADDR + 1280*2*(j+y);	
			for(i=0;i<len;i++)
			{
				Select_CharacterLayer(layer,color,addr+(i+x)*2);
			}
		}
	}
	#else
	{
		for(j=0;j<16;j++)
		{
			addr = TXT_BASE_ADDR + 640*2*(j+y);	
			for(i=0;i<len;i++)
			{
				Select_CharacterLayer(layer,color,addr+(i+x)*2);
			}
		}
	}
	#endif
//   SOC_ReadAndWrite_Hyram(0);
}

/**
 * @brief:将Hex数据转换为ASCII码，并传到HyperRAM中显示
 * @parameter:pData：无符号16位整形数据
 * @parameter:start：显示的起始位置
 */
void Display_HyperRAMHexData(ts_u16 pData,ts_u08 start)
{
	unsigned char buf[6] = {0x00};

	Hex16bit_ascii4(pData,buf);
	buf[4] = 0x48;//"H";
	DisPlayNewFont(buf,start);
}

/**
 * @brief:将8位无符号整型数据转换为ASCII码，并传到HyperRAM中显示
 * @parameter:pData：无符号8位整形数据
 * @parameter:start：显示的起始位置
 */
void Display_HyperRAMDataUint(ts_u08 pData,ts_u08 start)
{
	unsigned char Temp_ts_u08NumberTab[5] = {0x20,0x20,0x20,0x20,0};
	ts_u08_ascii(pData,Temp_ts_u08NumberTab);
	DisPlayNewFont(Temp_ts_u08NumberTab,start);
}

/**
 * @brief:将16位无符号整型数据转换为ASCII码，并传到HyperRAM中显示
 * @parameter:pData：无符号16位整形数据
 * @parameter:start：显示的起始位置
 */
void Display_HyperRAMDataUShort(ts_u16 pData,ts_u08 start)
{
	unsigned char	buf[5] = {0x00};
	uint16_ascii4(pData,buf);
	DisPlayNewFont(buf,start);
}

/**
 * @brief:在内存交互模块中实时显示探测器数据，传感器数据
 * @parameter:void
 */
void DisplayBottomPromptInformation(void)
{
// 	float temp = 0;
// 	ts_u08 temp1 = 0;
// 	float K,TEMP;
// 	ts_u16 TEMP_16,Data16bit;
// 	ts_u08 TEMP_int,TEMP_point;

// 	if(SelectionMenu.MenuState == 0x26)//当前进入菜单二显示探测器参数，传感器参数
// 	{
// 		DisPlayNewFont("ADV:",0);
// 		// DispSysMenu2.ADV_PAR = ReadImgRaw_Avg();
// 		Display_HyperRAMHexData(DispSysMenu2.ADV_PAR,4);

// 		DisPlayNewFont("Bak:",10);
// 		DispSysMenu2.Bak_PAR = FPGA_Bak_AVG()/2;
// 		Display_HyperRAMHexData(DispSysMenu2.Bak_PAR,14);

// 		DisPlayNewFont("Det:",20);
// 		DispSysMenu2.Det_PAR = ReadDETAD();
// 		Display_HyperRAMHexData(DispSysMenu2.Det_PAR,24);

// 		DisPlayNewFont("Det_T:",30);
// 		DispSysMenu2.Det_PAR = ReadDETAD();
// #ifdef SUPPORT_VOT_DETECTOR
// 		if(DispSysMenu2.Det_PAR < 7416) //正
// 		{
// 			DisPlayNewFont("+",36);
// 			K = 33;
// 			TEMP = (7416 - DispSysMenu2.Det_PAR)/K;
// 		}
// 		else  //负数
// 		{
// 			DisPlayNewFont("-",36);
// 			K = 33;
// 			TEMP = (DispSysMenu2.Det_PAR - 7416)/K;
// 		//	TEMP_16 = (~TEMP_16) + 1;    //补码表示
// 		}
// #else
// 		if(DispSysMenu2.Det_PAR < 7543) //正
// 		{
// 			DisPlayNewFont("+",36);
// 			K = 33.8;
// 			TEMP = (7543 - DispSysMenu2.Det_PAR)/K;
// 		}
// 		else  //负数
// 		{
// 			DisPlayNewFont("-",36);
// 			K = 33.8;
// 			TEMP = (DispSysMenu2.Det_PAR - 7543)/K;
// 		//	TEMP_16 = (~TEMP_16) + 1;    //补码表示
// 		}
// #endif
// 		TEMP_16 = (ts_u16)(TEMP *10);
// 		TEMP_int = TEMP_16 /10;
// 		TEMP_point = TEMP_16 %10;
// 		if(TEMP_int >= 100)
// 		{
// 			Display_HyperRAMDataUint(TEMP_int,37);
// 			DisPlayNewFont(".",40);
// 		}
// 		else if((TEMP_int < 100)&&(TEMP_int >= 10))
// 		{
// 			Display_HyperRAMDataUint(TEMP_int,37);
// 			DisPlayNewFont(".",39);
// 			Display_HyperRAMDataUint(TEMP_point,40);
// 		}
// 		else if(TEMP_int < 10)
// 		{
// 			Display_HyperRAMDataUint(0,37);
// 			Display_HyperRAMDataUint(TEMP_int,38);
// 			DisPlayNewFont(".",39);
// 			Display_HyperRAMDataUint(TEMP_point,40);
// 		}

// 		DisPlayNewFont("Sensor:",42);
// 		temp = READ_TMP112_Temp();
// 		if(temp<0)
// 		{
// 			temp1 = (ts_u08) abs((int)temp);
// 			DisPlayNewFont("-",49);

// 			Display_HyperRAMDataUint(temp1,50);

// 			DisPlayNewFont(".",52);
// 			temp1 = ((ts_u16)abs((int)(temp*10)))%10;

// 			Display_HyperRAMDataUint(temp1,53);
// 		}
// 		else
// 		{
// 			temp1 = (ts_u08)temp;

// 			DisPlayNewFont("+",49);

// 			Display_HyperRAMDataUint(temp1,50);

// 			DisPlayNewFont(".",52);

// 			temp1 = ((ts_u16)(temp*10))%10;

// 			Display_HyperRAMDataUint(temp1,53);
// 		}
// 		DisPlayNewFont("TES:",55);

// 		// Data16bit = ReadDETAD_TEST();//输出 test 列
// 		Display_HyperRAMHexData(Data16bit,59);
// 	}
}
/** @note 刷新测温功能之光标位置 
 * 1、由于测温功能光标有其他颜色显示问题，需要偶数对齐
*/
ts_void templutCursor_Set_position(ts_nodebuf_s_t *pst_osd_node, ts_u16 pos_x, ts_u16 pox_y) {
	ts_u16 cRet;
    ts_u16 u16_left, u16_top;
    ts_osd_attr_s_t st_attr;

    if(pst_osd_node == NULL) {
            return;
    }

	/* 增加384兼容 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

	u16_left = pos_x;
	u16_top = pox_y;
    // 边界检查
    if (u16_left >= max_width) {
        u16_left = max_width - 2;
    }
    if (u16_top >= max_heigth) {
        u16_top = max_heigth - 2;
    }

    // 偶数对齐（向下取整到最近的偶数）
    u16_left = u16_left & 0xFFFE;  // 清除最低位，相当于 x = x - (x % 2)
    u16_top = u16_top & 0xFFFE;
    
    if (u16_left < 0) {
		u16_left = 0;
	}

    if (u16_top < 0) {
		u16_top = 0;
	}
    
	ts_tdk_osd_get_attr(shdl_osd_dev, pst_osd_node, (ts_osd_attr_s_t *)&st_attr);
    st_attr.u16_left = (ts_u16)u16_left;
    st_attr.u16_top = (ts_u16)u16_top;
    cRet = ts_tdk_osd_set_attr(shdl_osd_dev, pst_osd_node, (ts_osd_attr_s_t *)&st_attr);
    if(cRet != TS_OK){
        vision_log_warn(" ts_tdk_osd_set_attr failed \n");
    }
    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}
/**
 * @brief:清除当前已打开的测温点的数据
 * @parameter:void
 */
void ClearSportsInfo(void)
{
	return;
	for(ts_u08 i=0;i<9;i++)
	{
		if(Spot_state&(1<<i))
		{
			DisPlaySpot(SpotInit[i].spot_X_Pos,SpotInit[i].spot_Y_Pos,2,0,0);
			DisplayBackground(SpotInit[i].spot_X_Pos+5,SpotInit[i].spot_Y_Pos+2,10,BACKGROUND_LAYER,0);
			DisplayBackground(SpotInit[i].spot_X_Pos-58,SpotInit[i].spot_Y_Pos-18,7,BACKGROUND_LAYER,0);
		}
	}
}

/**
 * @brief:清除当前已打开的测温区域的数据
 * @parameter:void
 */
void ClearAreasInfo(void)
{
	return;
	for(ts_u08 i=0;i<5;i++)
	{
		if(Area_state&(1<<i))
		{
			ClearUncheckAreaIcon(&Areainit[i]);
		}
	}
}

/** @note 测温功能：移动测温点光标 */
void DisPlayIcon(ts_u16 Sport_id,ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 visible)
{
	// DisPlaySpot(X_LINE,Y_LINE,layer,color,Icon);
	templut_captions_refresh_display(1);
	switch(Sport_id)
	{
		case 1:
			ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, visible);
			cursor_set_position(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, X_LINE, Y_LINE);
			break;
		case 2:
			ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, visible);
			cursor_set_position(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, X_LINE, Y_LINE);
			break;
		case 4:
			ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, visible);
			cursor_set_position(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, X_LINE, Y_LINE);
			break;
	}
}
/** @note 清除测温区域节点的字幕 */
void Clear_templut_osdNode_Info(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y) {
    ts_fontl_lbl_atts_s_t st_attr;
	ts_osd_attr_s_t st_osd_attr;

	ts_tdk_osd_get_attr(shdl_osd_dev, global_sys_ctrl_info.m_templut_info_osd_node, &st_osd_attr);

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
	ts_u32 pixels_width = fontl_get_fit_pixel_width(special_info);
	//左右预留八个
	pixels_width +=8;

    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
    // if(SelectionMenu.MenuState==0x26)//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    // else
    // 	st_attr.u16_width = 144;
    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

	ts_u08 osd_type  =  vot_menu_osd_type();
    if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
	}

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;
	st_attr.u08_visiable = 0;
	// st_attr.u16_width =  st_osd_attr.u16_width - st_attr.u16_left;
	//全黑是隐藏
	st_attr.u08_font_color = OSD_COLOR_BLACK;
	st_attr.u08_bg_color = OSD_COLOR_BLACK;
	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    st_attr.u08_rim_width = 3;

    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_templut_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
}

/** @note */
void Update_templut_osdNode_Info(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y)
{
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = fontl_get_fit_pixel_width(special_info);
	//左右预留八个
	pixels_width +=8;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
    // if(SelectionMenu.MenuState==0x26)//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    // else
    // 	st_attr.u16_width = 144;
    st_attr.u16_height = 24; //16（16x16的font）+4(上预留)+4（下预留）
    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;
	st_attr.u08_visiable = 1;
	st_attr.u08_font_color = OSD_COLOR_WHITE;
	st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    st_attr.u08_rim_width = 3;

	ts_u08 osd_type  =  vot_menu_osd_type();
    if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
	}

    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_templut_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
}
/** @note 区域字幕显示 */
void Refresh_templut_AreaCaptions(ts_char *special_Mess, struct Area* vSelect_Areas, ts_u08 visible) 
{
	Clear_templut_osdNode_Info(vSelect_Areas->last_osd_Msg, vSelect_Areas->last_osdMsg_X_pos, vSelect_Areas->last_osdMsg_Y_pos);
	if (visible == 0) {
		return;
	}
	Update_templut_osdNode_Info(special_Mess, vSelect_Areas->Area_X_Pos, vSelect_Areas->Area_Y_Pos);

	ts_cpl_memset(vSelect_Areas->last_osd_Msg, 0, 64);
	ts_cpl_memcpy(vSelect_Areas->last_osd_Msg, special_Mess, 64);
	vSelect_Areas->last_osdMsg_X_pos = vSelect_Areas->Area_X_Pos;
	vSelect_Areas->last_osdMsg_Y_pos = vSelect_Areas->Area_Y_Pos;
}
/** @note 点字幕显示 */
void Refresh_templutSpot_Captions(ts_char *special_Mess, struct Spot* vSelect_Spots, ts_u08 visible) {
	ts_u16 spot_x, spot_y;

	Clear_templut_osdNode_Info(vSelect_Spots->last_SpotMsg, vSelect_Spots->last_Spot_X_pos, vSelect_Spots->last_Spot_Y_pos);
	if (visible == 0) {
		return;
	}
	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);

	// 计算缩放后的阈值（使用整数运算，避免浮点）
	int x_threshold     = new_width - (640 - 580);	// 60
	int y_bottom_limit  = new_height - (512 - 480);	// 32
	int y_special       = new_height - (512 - 508);	// 4
	int y_offset        = 23;
	int y_const         = new_height - (512 - 485);	// 27

	// 使用缩放后的阈值进行判断
	if (vSelect_Spots->spot_Y_Pos >= y_bottom_limit && vSelect_Spots->spot_X_Pos < x_threshold) {
		spot_x = vSelect_Spots->spot_X_Pos;
		if (vSelect_Spots->spot_Y_Pos >= y_special) {
			spot_y = y_const;
		} 
		else {
			spot_y = vSelect_Spots->spot_Y_Pos - y_offset;
		}
	}
	else if (vSelect_Spots->spot_X_Pos >= x_threshold && vSelect_Spots->spot_Y_Pos < y_bottom_limit) {
		spot_x = x_threshold;
		spot_y = vSelect_Spots->spot_Y_Pos;
	}
	else if (vSelect_Spots->spot_X_Pos >= x_threshold && vSelect_Spots->spot_Y_Pos >= y_bottom_limit) {
		spot_x = x_threshold;
		if (vSelect_Spots->spot_Y_Pos >= y_special) {
			spot_y = y_const;
		} else {
			spot_y = vSelect_Spots->spot_Y_Pos - y_offset;
		}
	} else {
		spot_x = vSelect_Spots->spot_X_Pos;
		spot_y = vSelect_Spots->spot_Y_Pos;
	}
	Update_templut_osdNode_Info(special_Mess, spot_x, spot_y);

	ts_cpl_memset(vSelect_Spots->last_SpotMsg, 0, 64);
	ts_cpl_memcpy(vSelect_Spots->last_SpotMsg, special_Mess, 64);
	vSelect_Spots->last_Spot_X_pos = spot_x;
	vSelect_Spots->last_Spot_Y_pos = spot_y;
}

void Display_MatrixIcon(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 size,ts_u08 color)
{
	// ts_u08 i,j,num;
	// ts_u32 addrc;
	// ts_u08 tempdata[10] = {0};
	// for(i=0;i<size;i++)
	// {
	// 	for(num = 0;num<(size/8);num++)
	// 	{
	// 		tempdata[num] = buf[(size/8)*i+num];
	// 	}
	// 	for(j=0;j<size;j++)
	// 	 {
	// 		addrc = TXT_BASE_ADDR + 1280*(j+Y_LINE);
	// 		if(tempdata[(j/8)]&0x80)
	// 		{
	// 			menu_layer4(color,addrc+(i+X_LINE)*2);
	// 		}
	// 		tempdata[(j/8)] = tempdata[(j/8)]<<1;
	// 	 }
	// }
}

void Display_MatrixIconX2(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 size,ts_u08 color)
{
	// // ts_u08 i,j,num;
	// ts_u32 addrc,addrcc;
	// ts_u08 tempdata[15] = {0};
	// for(ts_u08 i=0;i<32;i+=2)
	// {
	// 	for(ts_u08 m = 0;m<2;m++)
	// 	{
	// 		for(ts_u08 num = 0;num<4;num++)
	// 		{
	// 			tempdata[num] = buf[i*2+num];
	// 		}
	// 		for(ts_u08 j=0;j<64;j+=2)
	// 		{
	// 			addrc = TXT_BASE_ADDR + 1280*(j+Y_LINE);
	// 			addrcc = TXT_BASE_ADDR + 1280*(j+1+Y_LINE);
	// 			if(tempdata[(j/16)]&0x80)
	// 			{
	// 				menu_layer4(color,addrc+(i+m+X_LINE)*2);
	// 				menu_layer4(color,addrcc+(i+m+X_LINE)*2);
	// 			}
	// 			tempdata[(j/16)] = tempdata[(j/16)]<<1;
	// 		}
	// 	}
	// }
}
void DisplayCenterSpot_Init(void)
{
    if(!SystemParaMenu2.Usermenu_SW) 		{Display_CenterSpot(UsermenuParameter.CenterSpotSW);} //显示中心点光标
    else															{UsermenuParameter.CenterSpotSW = 0;Display_CenterSpot(UsermenuParameter.CenterSpotSW);} //测温版不显示中心光标
}

void Display_CenterSpot(ts_u08 Spot_way)
{
	return;
#if 0
	ts_u08 i;
	ts_u16 Y_LINE,X_LINE;

	X_LINE = IR_X_pixel/2;
	Y_LINE = IR_Y_pixel/2;
//	if(way)
//	{DisPlaySpot(IR_X_pixel/2,IR_Y_pixel/2,layer,color,1);}
//	else
//	{DisPlaySpot(IR_X_pixel/2,IR_Y_pixel/2,2,0,1);}
	if(!way)		color= 0;

	SOC_ReadAndWrite_Hyram(1);
	for(i=0;i<88;i++)
	{
		if((i>=40&&i<=46)||(Y_LINE+i-43)<0) continue;
		Select_CharacterLayer(layer,color,TXT_BASE_ADDR+IR_X_pixel*2*(Y_LINE+i-43) +X_LINE*2);
	}
	for(i=0;i<88;i++)
	{
		if((i>=40&&i<=46)||(X_LINE-43+i)<0) continue;
		Select_CharacterLayer(layer,color,TXT_BASE_ADDR+IR_X_pixel*2*Y_LINE +(X_LINE-43+i)*2);
	}
	Select_CharacterLayer(layer,color,TXT_BASE_ADDR+IR_X_pixel*2*Y_LINE +X_LINE*2);

	SOC_ReadAndWrite_Hyram(0);
#endif
//	APB3_REGW_DisplaySpot_Set(Spot_way);
}


