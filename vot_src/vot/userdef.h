/*
 * userdef.h
 *
 *  Created on: 2023年3月21日
 *      Author: yan
 */

#pragma once

//#include "soc.h"
#include "myheader.h"
#include "config.h"

#define read_ts_u32(addr, value)
#define write_ts_u32(addr, value)

#define C_XSIZE       1080
#define C_YSIZE       1920
#define C_STRIDE      (C_XSIZE+15)/16*16*2

#ifdef SIM
    #define LOOP_UDELAY 100
#else
    #define LOOP_UDELAY 100000
#endif

#ifdef CONFIG_SNR_PLANCK1280C
#define IR_X_pixel 1280
#define IR_Y_pixel 1024
#else
#define IR_X_pixel 640
#define IR_Y_pixel 512
#endif

#define ZOOM_OFFSET 15

/************************** APB3 Slave Port *************************************/
#define APB3_SLV0_REG0_ID   	0  //Read ID
#define APB3_SLV_REG1_M0    	4  //m0_quan
#define APB3_SLV_REG2_M1    	8  //m1_quan
#define APB3_SLV_REG3_M2    	12 //m2_quan
#define APB3_SLV_REG4_M3    	16 //m3_quan
#define APB3_SLV_REG5_M4    	20 //m4_quan
#define APB3_SLV_REG6_M5    	24 //m5_quan
#define APB3_SLV_REG7_SLOW_CNT	28 //low_speed_error_cnt
#define OOB_APB_SLV				//IO_APB_SLAVE_0_INPUT
#define I2C_CTRL_HZ     		//SYSTEM_CLINT_HZ
#define SPI             		//SYSTEM_SPI_0_IO_CTRL
#define I2C_CTRL_MIPI   		//SYSTEM_I2C_0_IO_CTRL
#define SPI_CS 					0


#define ON  1
#define OFF 0

#define IO_APB_SLAVE_0_INPUT_function00     0xf8100000
#define IO_APB_SLAVE_0_INPUT_key            0xf810000D
#define IO_APB_SLAVE_0_INPUT_ScreenAD_Avg   0xf810000E
#define IO_APB_SLAVE_0_INPUT_ScreenAD_Max   0xf8100010
#define IO_APB_SLAVE_0_INPUT_ScreenAD_Min   0xf8100012
#define IO_APB_SLAVE_0_INPUT_Hyperram_Opr   0xf8100014
#define IO_APB_SLAVE_0_INPUT_Level 			0xf8100018
#define IO_APB_SLAVE_0_INPUT_Span  			0xf810001A
#define IO_APB_SLAVE_0_INPUT_ImgRaw_Avg     0xf8100028
#define IO_APB_SLAVE_0_INPUT_Bak_AVG        0xf810002A
//#define IO_APB_SLAVE_0_INPUT_Sensor         0xf8100070

/********************************20230614更新交互文档**********************************/
#define IO_APB_SLAVE_0_INPUT_function02     	0xf8100008
#define IO_APB_SLAVE_0_INPUT_COF     			0xf810000C
#define IO_APB_SLAVE_0_INPUT_INT_T     			0xf810001F
#define IO_APB_SLAVE_0_INPUT_OOC_PAR			0xf8100020
#define IO_APB_SLAVE_0_INPUT_function07     	0xf810001C
#define IO_APB_SLAVE_0_INPUT_SUPPOSE_CVBS   	0xf810002C
#define IO_APB_SLAVE_0_INPUT_FPGA_VERSION   	0xf8100024

#define Hyperram_Alarm1   						0xf8100070
#define Hypprram_Alarm2    						0xf8100074
#define Hypprram_Alarm3    						0xf8100090
#define Hypprram_Alarm4    						0xf8100094
#define Hypprram_Alarm5    						0xf81000AC
#define Hypprram_Alarm6    						0xf81000B0
#define IO_APB_SLAVE_0_INPUT_RH_THR   			0xf8100078

#define IO_APB_PAT_0X00                         0xf8100000
#define IO_APB_PAT_0X01                         0xf8100004

#define IO_APB_PAT_0X22   			            0xf8100088
#define IO_APB_PAT_0X23   			            0xf810008C
#define IO_APB_PAT_0X24   			            0xf8100090
#define IO_APB_PAT_0X25   			            0xf8100094
#define IO_APB_PAT_0X26   			            0xf8100098
#define IO_APB_PAT_0X27   			            0xf810009C
#define IO_APB_PAT_0X33   			            0xf81000CC
#define IO_APB_PAT_0X34   			            0xf81000D0
#define IO_APB_PAT_0X35   			            0xf81000D4
#define IO_APB_PAT_0X36   			            0xf81000D8
#define IO_APB_PAT_0X39   			            0xf81000E4

#define IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS   	0xf8100100
#define IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX   0xf8100104
#define IO_APB_SLAVE_0_INPUT_SPOT_MIN_POS  		0xf8100108
#define IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MIN   0xf810010C


#define IO_APB_SLAVE_0_INPUT_SPOTN_SET  		0xf8100110
//#define IO_APB_SLAVE_0_INPUT_SPOTN_X_Y_POS_SET  0xf8100114
#define IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_START 0xf8100118

//#define IO_APB_SLAVE_0_INPUT_AREAN_SET  		0xf810013C
#define IO_APB_SLAVE_0_INPUT_AREAN_SET_LEFT_XY  0xf8100140
#define IO_APB_SLAVE_0_INPUT_AREAN_SET_RIGHT_XY 0xf8100144
#define IO_APB_SLAVE_0_INPUT_Area5_MinAD  		0xf8100158
#define IO_APB_SLAVE_0_INPUT_Area5_MaxAD 		0xf810015A

#define IO_APB_SLAVE_0_INPUT_Area5_MinAD_X  	0xf8100170
#define IO_APB_SLAVE_0_INPUT_Area5_MinAD_Y  	0xf8100172
#define IO_APB_SLAVE_0_INPUT_Area5_MaxAD_X		0xf8100174
#define IO_APB_SLAVE_0_INPUT_Area5_MaxAD_Y		0xf8100176
//*******************************12.15 9点4区域新地址*********************/
#define IO_APB_SLAVE_0_INPUT_SPOT2_AD_TEMP_START 0xf810011C
#define IO_APB_SLAVE_0_INPUT_SPOT3_AD_TEMP_START 0xf8100120
#define IO_APB_SLAVE_0_INPUT_SPOT4_AD_TEMP_START 0xf8100124
#define IO_APB_SLAVE_0_INPUT_SPOT5_AD_TEMP_START 0xf8100128
#define IO_APB_SLAVE_0_INPUT_SPOT6_AD_TEMP_START 0xf810012C
#define IO_APB_SLAVE_0_INPUT_SPOT7_AD_TEMP_START 0xf8100130
#define IO_APB_SLAVE_0_INPUT_SPOT8_AD_TEMP_START 0xf8100134
#define IO_APB_SLAVE_0_INPUT_SPOT9_AD_TEMP_START 0xf8100138

#define IO_APB_SLAVE_0_INPUT_SPOT1_X_Y__SET  0xf8100178
#define IO_APB_SLAVE_0_INPUT_SPOT2_X_Y__SET  0xf810017C
#define IO_APB_SLAVE_0_INPUT_SPOT3_X_Y__SET  0xf8100180
#define IO_APB_SLAVE_0_INPUT_SPOT4_X_Y__SET  0xf8100184
#define IO_APB_SLAVE_0_INPUT_SPOT5_X_Y__SET  0xf8100188
#define IO_APB_SLAVE_0_INPUT_SPOT6_X_Y__SET  0xf810018C
#define IO_APB_SLAVE_0_INPUT_SPOT7_X_Y__SET  0xf8100190
#define IO_APB_SLAVE_0_INPUT_SPOT8_X_Y__SET  0xf8100194
#define IO_APB_SLAVE_0_INPUT_SPOT9_X_Y__SET  0xf8100198
#define IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET   0xf810019C
#define IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET   0xf81001A0
#define IO_APB_SLAVE_0_INPUT_Area2_Left_XY__SET   0xf81001A4
#define IO_APB_SLAVE_0_INPUT_Area2_Right_XY__SET   0xf81001A8
#define IO_APB_SLAVE_0_INPUT_Area3_Left_XY__SET   0xf81001AC
#define IO_APB_SLAVE_0_INPUT_Area3_Right_XY__SET   0xf81001B0
#define IO_APB_SLAVE_0_INPUT_Area4_Left_XY__SET   0xf81001B4
#define IO_APB_SLAVE_0_INPUT_Area4_Right_XY__SET   0xf81001B8
#define IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START   0xf810014C
#define IO_APB_SLAVE_0_INPUT_AREAN3_MIN_MAX_TEMP_START   0xf8100150
#define IO_APB_SLAVE_0_INPUT_AREAN4_MIN_MAX_TEMP_START   0xf8100154
#define IO_APB_SLAVE_0_INPUT_AREAN2_TEMPAVG_START        0xf8100160
#define IO_APB_SLAVE_0_INPUT_AREAN3_TEMPAVG_START        0xf8100164
#define IO_APB_SLAVE_0_INPUT_AREAN4_TEMPAVG_START        0xf8100168
/*************************************************************************************/

#define IO_APB_SLAVE_0_INPUT_AREAN_TEMPAVG_START        0xf810015C
#define IO_APB_SLAVE_0_INPUT_AREAN_MIN_MAX_TEMP_START   0xf8100148

#define IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY   	0xf810002C
#define IO_APB_SLAVE_0_INPUT_LEVEL_GAIN_ADJ  			0xf8100030
#define IO_APB_SLAVE_0_INPUT_LEVEL2_GAIN2   			0xf8100034

#define IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START   		0xf8100038

#define IO_APB_SLAVE_0_INPUT_DET_VTEMP   				0xf8100068
#define IO_APB_SLAVE_0_INPUT_DET_TEST   				0xf810006C
/**********************************************************************************/
#define ADTTAB    0xf8100200
/**********************************************************************************/
#define IO_APB_SLAVE_0_INPUT_ALPHA			0xf8100084
#define IO_APB_SLAVE_0_INPUT_IMAGE_PARA			0xf8100088

/************************************FPGA_Uart_DDR***********************************/
#define IO_APB_SLAVE_0_INPUT_UartCommand1		0xf81000C0
#define IO_APB_SLAVE_0_INPUT_UartCommand2		0xf81000C4
#define IO_APB_SLAVE_0_INPUT_UartCommand3		0xf8100380//0xf81000C8

/**************************  HyperRAM  addr   *************************************/
#if No_Shutter  //新的24帧无档位地址更新
	#define OOC_BASE_ADDR     	0x03C0000
	#define DDR_IR_Image_Bak  	0x0470000
	#define NUC_BASE_ADDR    	0x0520000
	#define DDR_LBAK_ADDR     	0x05D0000

	#define PALETTE_BASE_ADDR 					0x680000
	#define TXT_BASE_ADDR     						0x690000
	#define Update_Firmware_BASE_ADDR  	0x740000

	/*********帧图像导入功能测试 *****************/
	#define DDR_IR_Image_T0     0x1400000
	#define DDR_IR_Image_T1     0x1500000
	#define DDR_IR_Image_T2     0x1600000
	#define DDR_IR_Image_T3     0x1700000

	/*********存储的4帧数据 *****************/
	#define DDR_IR_Image_T00     0x100000
	#define DDR_IR_Image_T10     0x1B0000
	#define DDR_IR_Image_T20     0x260000
	#define DDR_IR_Image_T30     0x310000

	/*********无挡片功能测试 *****************/
	#define DDR_IR_Image_BakP1  0x0A00000
	#define DDR_IR_Image_BakP2  0x0AB0000
	#define DDR_IR_Image_BakP3  0x0B60000
	#define DDR_IR_Image_BakP4  0x0C10000
	#define DDR_IR_Image_BakP5  0x0CC0000
	#define DDR_IR_Image_BakP6  0x0D70000
	#define DDR_IR_Image_BakP7  0x0E20000
	#define DDR_IR_Image_BakP8  0x0ED0000
	#define DDR_IR_Image_BakP9  0x1100000
	#define DDR_IR_Image_BakP10  0x11B0000
	#define DDR_IR_Image_BakP11  0x1260000
	#define DDR_IR_Image_BakP12  0x1310000
	#define DDR_IR_Image_BakP13  0x13C0000
	#define DDR_IR_Image_BakP14  0x1470000
	#define DDR_IR_Image_BakP15  0x1520000
	#define DDR_IR_Image_BakP16  0x15D0000
	#define DDR_IR_Image_BakP17  0x1680000
	#define DDR_IR_Image_BakP18  0x1730000
	#define DDR_IR_Image_BakP19  0x17E0000
	#define DDR_IR_Image_BakP20  0x1890000
	#define DDR_IR_Image_BakP21  0x1940000
//	#define DDR_IR_Image_BakP22  0x19F0000
	#define DDR_IR_Image_BakP22  0x1C10000
	#define DDR_IR_Image_BakP23  0x1AA0000
	#define DDR_IR_Image_BakP24  0x1B50000
	/*************************************/

#else
#define OOC_BASE_ADDR     	0x0600000
#define DDR_IR_Image_Bak  	0x0700000
#define NUC_BASE_ADDR    	0x0800000
#define DDR_LBAK_ADDR     	0x0900000

/*********无挡片功能测试 *****************/
#define DDR_IR_Image_BakP1  0x0A00000
#define DDR_IR_Image_BakP2  0x0B00000
#define DDR_IR_Image_BakP3  0x0C00000
#define DDR_IR_Image_BakP4  0x0D00000
#define DDR_IR_Image_BakP5  0x0E00000
#define DDR_IR_Image_BakP6  0x0F00000
/*************************************/

/*********帧图像导入功能测试 *****************/
#define DDR_IR_Image_T0     0x1400000
#define DDR_IR_Image_T1     0x1500000
#define DDR_IR_Image_T2     0x1600000
#define DDR_IR_Image_T3     0x1700000


#define DDR_IR_Image_T00     0x100000
#define DDR_IR_Image_T10     0x200000
#define DDR_IR_Image_T20     0x300000
#define DDR_IR_Image_T30     0x400000

/*************************************/
#define PALETTE_BASE_ADDR 0x1C00000
#define TXT_BASE_ADDR     0x1100000
#define Update_Firmware_BASE_ADDR  0x1200000

#endif

#define SYSTEM_GPIO_0_IO_CTRL 0
#define GPIO0       SYSTEM_GPIO_0_IO_CTRL

/**************************  test   *************************************/
#define		IR_DET			0x30000000
#define     FPGA_DDR_USE    (*((volatile uint32_t *) (IR_DET + 0x000)))
/**************************  test   *************************************/

struct SystemPara{

	ts_u08  TFilter;
	ts_u08  IR_SW;
	ts_u08  Test_SW;
	ts_u08  NUC_SW;
	ts_u08  LevelSpanFlag;
	ts_u08  shutter_flag;
	uint8_t WinTempRange;		//测温档,0-2档  //26
	uint8_t Tempunit;         	//温度单位      0=Celsius(攝氏度)，1=Fahrenheit(华氏度)

	ts_u08 Time_S;
	ts_u08 Time_M;
	ts_u32 Time_H;
	ts_u32 Time_Count;

	uint16_t winWidth;		 	//窗口最大值
	uint16_t winLevel;	     	//窗口最小值

	//温度计算
	uint16_t Ad_Auto_average;
	uint16_t Ad_Auto_max;
	uint16_t Ad_Auto_min;

	uint16_t in_tempAD;
	int16_t out_temp;
	int16_t Dsp_out_temp;

	uint16_t Temp_Level;        //2  写Xlinx的level值（2字节）
	uint16_t AD_K;  		    //4  Span 用于xlinx显示
	uint16_t AD_Width;
	float Temp_Base;         	//1c  基准温度
	float AD_Base;           	//20  基准AD值
	float Temp_K;            	//24  温度 斜率

	ts_u08 NoShutter;
	ts_u08 OOC_Way;
	ts_u08 ImgNum;
	ts_u08 CameralinkSW;


	ts_u08 FirmwareUpdate;  // 1SOC升级 2FPGA升级
	ts_u08 FirmwareUpdateStatus;
	ts_u08* PtrUpdateDDR;
	ts_u32 FirmwareUpdateLen,FirmwareLenCount,FirmwareOverTime;
	ts_u08 FirmwareUpdateFlag;
};

struct LeveleSpanPara{

	ts_u08	cal_small;
	ts_u08	cal_big;
	ts_u08	cal_bigM;
	ts_u08	cal_err;
};

typedef struct
{
	//uint8 code *p_code;
	ts_u08  *p_xdata;
	ts_u32 ASCAddr;
	ts_u08 index;
	ts_u08 arrayNum;
	ts_u08 linesNum;
} LineASCIIBufStruct;

typedef struct
{
	//X为无符号整数，最高位表示字符大小，
	//x.15=1为ASCII16X8，x.15=0为ASCII8X8
	//x.14=1为字节倒向（位右移），x.14=0为字节正向（位左移），
	//X坐标只占底8位，从0到511
	ts_u16 x;
	ts_u08 y;
	ts_u08 back;
	ts_u08 front;
	ts_u08 sel;
} CharDispStruct;



#define APB3_REGW(addr, offset, data) 

#define APB3_REGR(addr, offset) 

#define APB3_REGW_TEST_SET(data) 

#define APB3_REGW_SENSOR_SET(data) 

#define APB3_REGW_Shutter_SW(data) 
#define APB3_REGW_AD_Ssensor_Set(data) 


//#define ReadCommandFlag()		(read_ts_u32(IO_APB_SLAVE_0_INPUT_UartCommand3)>>24)&0x01
#define WriteCommandFlag(data,data1) 

/***************Alarm TEST_1  0x70-73 **********************/
#define APB3_REGW_Alarm_AutoVal(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(0x3FF<<0)))| ((data << 0)&0x3FF<<0),Hyperram_Alarm1) //Bit[9-0]

#define APB3_REGW_S_Range(data) \
write_ts_u32((read_ts_u32(Hypprram_Alarm3)&(~(0xFFFF<<0)))| ((data << 0)&0xFFFF<<0),Hypprram_Alarm3) //Bit[15-0]

#define APB3_REGW_Alarm_Val(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(0x3FF<<10)))| ((data << 10)&0x3FF<<10),Hyperram_Alarm1) //Bit[19-10]

#define APB3_REGW_Alarm_SW(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(3<<20)))| ((data << 20)&3<<20),Hyperram_Alarm1) //Bit[21-20]

#define APB3_REGW_Alarm_Range(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(3<<22)))| ((data << 22)&3<<22),Hyperram_Alarm1) //Bit[23-22]

#define APB3_REGW_AF_Val_Range(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(0x7F<<25)))| ((data << 25)&0x7F<<25),Hyperram_Alarm1) //Bit[31-25]

#define APB3_REGW_CameralinkSW(data) \
write_ts_u32((read_ts_u32(Hyperram_Alarm1)&(~(0x01<<24)))| ((data << 24)&0x01<<24),Hyperram_Alarm1) //Bit[24] 河南中光学定制

#define APB3_REGW_AlarmAD(data) \
write_ts_u32((read_ts_u32(Hypprram_Alarm5)&(~(0xFFFF<<0)))| ((data << 0)&0xFFFF<<0),Hypprram_Alarm5) //Bit[15-0] 连云港

#define APB3_REGW_AlarmNum(data) \
write_ts_u32((read_ts_u32(Hypprram_Alarm6)&(~(0xFFFFFFFF<<0)))| ((data << 0)&0xFFFFFFFF<<0),Hypprram_Alarm6) //Bit[31-0] 连云港
/***********************************************************/

#define APB3_REGW_SunPT_SW(data) \
write_ts_u32((read_ts_u32(Hypprram_Alarm3)&(~(3<<30)))| ((data << 30)&3<<30),Hypprram_Alarm3) //Bit[31-30]

/***************0X22   0X88~0X8B   寄存器交互地址**********************/
//#define APB3_REGW_SF_LV(data) 

#define APB3_REGW_SF_LV(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X35)&(~(0xFF<<0)))| ((data << 0)&0xFF<<0),IO_APB_PAT_0X35)    //Bit[7-0]

#define APB3_REGW_GMAX(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(0x0F<<12)))| ((data << 12)&0x0F<<12),IO_APB_PAT_0X22) //Bit[15-12]

#define APB3_REGW_GMIN(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(0x0F<<16)))| ((data << 16)&0x0F<<16),IO_APB_PAT_0X22) //Bit[19-16]

#define APB3_REGW_DDE_LV(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(0xFF<<20)))| ((data << 20)&0xFF<<20),IO_APB_PAT_0X22) //Bit[27-20]

#define APB3_REGW_DDE_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(1<<28)))| ((data << 28)&0x01<<28),IO_APB_PAT_0X22) //Bit[28]

#define APB3_REGW_MedFilt_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(1<<29)))| ((data << 29)&0x01<<29),IO_APB_PAT_0X22) //Bit[29]

#define APB3_REGW_AGC_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(1<<30)))| ((data << 30)&0x01<<30),IO_APB_PAT_0X22) //Bit[30]

#define APB3_REGW_BF_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X22)&(~(1<<31)))| ((data << 31)&0x01<<31),IO_APB_PAT_0X22) //Bit[31]
/*****************************************/
#define APB3_REGW_JLEPS_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X39)&0xFFFFFF00)| ((data << 0)&0xFF),IO_APB_PAT_0X39) //Bit[31]

/*************** 0X23 0X8C~0X8F   寄存器交互地址 **********************/
#define APB3_REGW_Plat_NX(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0xFF<<0)))| ((data << 0)&0xFF<<0),IO_APB_PAT_0X23) //Bit[7-0]

#define APB3_REGW_FK8_SW(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0x01<<8)))| ((data << 8)&0x01<<8),IO_APB_PAT_0X23) //Bit[8]
#define APB3_REGW_AGC_AD_Set(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0x03<<10)))| ((data << 10)&0x03<<10),IO_APB_PAT_0X23) //Bit[11-10]

#define APB3_REGW_DisplaySpot_Set(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0x03<<12)))| ((data&0x03) << 12),IO_APB_PAT_0X23) //Bit[13-12]

#define APB3_REGW_DETTemp(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0xFFFF<<16)))| ((data&0xFFFF) << 16),IO_APB_PAT_0X23) //Bit[31-16]
/***************************************************************************************************/

#define APB3_REGW_ImageAdjust(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X01)&(~(0x03<<22)))| ((data <<22)&0x03<<22),IO_APB_PAT_0X01)    //Bit[23-22]

/***************0X23 0X24 0X25 0x26   测试数据地址**********************/
#define APB3_REGW_IMG_1(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X23)&(~(0xFFFFFFFF<<0)))| ((data << 0)&0xFFFFFFFF<<0),IO_APB_PAT_0X23)
#define APB3_REGW_IMG_2(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X24)&(~(0xFFFFFFFF<<0)))| ((data << 0)&0xFFFFFFFF<<0),IO_APB_PAT_0X24)
#define APB3_REGW_IMG_3(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X25)&(~(0xFFFFFFFF<<0)))| ((data << 0)&0xFFFFFFFF<<0),IO_APB_PAT_0X25)
#define APB3_REGW_IMG_4(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X26)&(~(0xFFFFFFFF<<0)))| ((data << 0)&0xFFFFFFFF<<0),IO_APB_PAT_0X26)

/*****************************************/


/*************** 0X23 0XCC~0XCF   寄存器交互地址 **********************/
#define APB3_REGW_ZOOM(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X33)&(~(0xFF<<0)))| ((data << 0)&0xFF<<0),IO_APB_PAT_0X33) //Bit[7-0]

#define APB3_REGW_ZOOM_Scaler_XY(scalerX,scalerY) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X33)&0x000000FF)| (((scalerX &0x3FF)<<8)|((scalerY&0x3FF) << 18)),IO_APB_PAT_0X33) //Bit[27-8]
#define APB3_REGW_ZOOM_XY(dataX,dataY) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X34)&(~(0xFFFFFFFF<<0)))| ((dataX &0x3FFF<<0)|((dataY&0x3FFF) << 14)),IO_APB_PAT_0X34) //Bit[27-0]
/******************************************************************/
#define APB3_REGW_PULSE_SET(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X34)&(~(0x01<<28)))| ((data &0x01) << 28),IO_APB_PAT_0X34) //Bit[28]
#define APB3_REGW_WatchDogSW(data) 	write_ts_u32((read_ts_u32(IO_APB_PAT_0X34)&0xBFFFFFFF)| ((data &0x01)<<30),IO_APB_PAT_0X34) //Bit[30]


/***************SPI_CS*******************/
#define APB3_REGW_SPI_CS(data) \
write_ts_u32((read_ts_u32(IO_APB_SLAVE_0_INPUT_ALPHA)&(~(0x01<<0)))| ((data << 0)&0x01<<0),IO_APB_SLAVE_0_INPUT_ALPHA) //Bit[0]
/*******************************/

/*--------------NUC_PARA_SELECT-----------------*/
#define APB3_REGW_NUC_PARA(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X35)&(~(0x03<<8)))| ((data << 8)&0x03<<8),IO_APB_PAT_0X35) //Bit[9-8]

/*--------------Alpha_PARA_SELECT-----------------*/
#define APB3_REGW_ALPHA_PARA(data) \
write_ts_u32((read_ts_u32(IO_APB_PAT_0X35)&(~(0xFF<<24)))| ((data << 24)&0xFF<<24),IO_APB_PAT_0X35) //Bit[31-24]
/*---------------------------------------------*/


