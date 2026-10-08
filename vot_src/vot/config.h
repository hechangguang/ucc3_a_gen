/*
 * config.h
 *
 *  Created on: 2024年3月8日
 *      Author: GZ
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/*
 * 测试功能放置在TestFunction区域，正式版本使用后移至OptionalFunction
 * */

/*************Detector************/
#define VOT		1 //打开此宏定义表示使用VOT探测器,否则使用艾瑞探测器

/************UartProtocol************/
#define SUPPORT_IRAY_UART_PROTOCOL 	1//打开此宏定义表示支持艾瑞协议

//#define SUPPORT_MEASURE_UART_PROTOCOL  1 //打开此宏定义表示支持测距协议

//#define SUPPORT_Refrigeration_UART_PROTOCOL 1 //打开此宏定义表示支持中波制冷协议

//#define SUPPORT_HuanYu_UART_PROTOCOL  1 //打开此宏定义表示支持寰宇协议
//#define SUPPORT_HuanYu_UART_PROTOCOL_NET  1 //打开此宏定义表示支持寰宇协议网络板对接光智机芯功能

//#define SUPPORT_ZhongLin_UART_PROTOCOL 1 //打开此宏定义表示支持中林协议 ***注意 使用时关闭上述其他协议**
//#define SUPPORT_ZhongLin_Multi_Firmware 1 //打开表示中林在线升级支持 多固件识别，相应的机芯固件要做特殊处理**

//#define SUPPORT_BoYu_UART_PROTOCOL  1 //打开此宏定义表示支持博宇协议(调焦变倍相关)

//#define SUPPORT_FPGA_USB_UART_PROTOCOL 1 //打开表示USB2.0通过FPGA传输串口协议

/*************Version***************/
/* 固件版本后缀说明
 *  _RTD6 艾睿640
 *  _RTD3 艾睿384
 *  _PLK6 光智640
 *  _PLK3 光智384
 *
 *	_F 电调版本
 *	_Z 变倍版本
 *
 *	other(包含定制项目)
 *	_FX2 赛普拉斯2.0版本
 *	_NO 无挡片版本
 *	_XXXX (XXXX为定制项目缩写，例如中林项目为 _ZL)
 *
 *	后缀可根据项目需求叠加，例如C241104P1_RTD3_F
 */

/*
 * 测试版使用T  正式版使用P
 * */

#ifdef VOT
//	#define  CPU_VERSION	"   C241115T1_PLK6_D    "
	#define  CPU_VERSION	"   C250430T1_PLK6    "   //通用版本
//	#define  CPU_VERSION	"   C250108T1_PLK6_FX2    "   //XX定制版本
	#define SUPPORT_VOT_DETECTOR   1
	#define FLASH_SAVE_FLAG1  0x55
	#define FLASH_SAVE_FLAG2  0xAA

#else
//	#define  CPU_VERSION	"   C241115T1_RTD6_C  "   //IRay测温版本号  需要打开宏定义Temperature_Version参数
	#define  CPU_VERSION	"   C250430T1_RTD6  "      //通用版本
//	#define  CPU_VERSION	"   C250102P1_RTD6_FX2  "	  //IRay XX定制版本
	#define FLASH_SAVE_FLAG1  0x12
	#define FLASH_SAVE_FLAG2  0x08
	#define SUPPORT_IRAY_DETECTOR  1
#endif


#ifdef SUPPORT_HuanYu_UART_PROTOCOL   //寰宇版本号信息
enum HuanYuVersion{
	Year=0x24,   //24年
	Month=0x0A,   //0x00-0x0C月
	Day=0x12,     //25号
	Version_Num=0x01   //第X版
};
#endif


/****************CommonFunction**************************/
#define DEFAULT_VIDEO_MODE  //切换默认启动CVBS/USB  开启表示使用CVBS

#define SUPPORT_CONTINUOUS_ZOOM   1 // 打开表示使用连续变倍功能1.0~7.0变倍   否则使用原先逻辑X2 X4 X8 默认使用连续变倍

#define SOC_WriteAndRead_Hyram	1 //SOC访问Hyram:0:表示SOC访问Hyram时，不需要向FPGA传输标志位,
								  //			1:表示SOC访问Hyram时，需要向FPGA传输的标志位,
								  //此种情况主要是因为图像缓存资源不够时需要开启此功能，默认开启此功能.

#define Mirror_DisplayType 	0   //镜像的显示类型:1：每次重新上电后镜像OFF,镜像参数不保存,菜单一初始化时，不初始化当前镜像.
								//镜像的显示类型:0：重新上电后使用上一次保存的镜像，菜单一初始化时，初始化镜像为OFF

#define DET_Change_Para 1  		//DET动态调整参数

/****************OptionalFunction***********************/
//#define OPR_ALARM   //区域告警相关功能 OPR_ALARM

//#define MingYangProject_FunctionSwitch 	0//名扬时代640转384版本项目定制需求：电子放大只需要OFF,X2俩中选项，
											 //0:关闭专属于名扬时代项目的功能函数,1:打开专属于名扬时代项目的功能函数，
											 //注意：名扬时代有俩个版本：名扬时代640版本/名扬时代640转384版本，
											 //此宏定义只有在“名扬时代640转384版本”方可置1,其他版本包括“名扬时代640版本”置0
											 //此宏定义置1时：SOC的版本号后缀需要由“RTD6”修改为“RTD3”
// #define  FunctionAutoFocus	//打开此宏定义表示使用自动调焦功能（调焦版本开启FunctionAutoFocus 和 FunctuinMotorPreset）--已去掉

// #define FunctionAutoZoom //打开此宏定义表示使用变倍功能（变焦版本开启FunctionAutoFocus、FunctionAutoZoom 和 FunctuinMotorPreset） --已去掉

#define FunctuinMotorPreset // 打开此宏定义表示使用预置位，温补，跟焦相关功能

// #define FunctionMotor_encoder 1 	//打开此宏定义表示使用电机编码器功能，否则使用电机电位器功能

//#define SS1    //打开此宏定义表示调焦变焦功能，使用SS1网络板配置

//#define AnFangProject      //安防报警区域和测温区域使用的是相同的4个区域位  如需正常使用测温功能，需屏蔽此宏定义

//#define Network_Machine     //网络型整机设备，soc端默认设置为关闭自动打档，打档功能由ARM端控制

//#define Electrolytic_copper  		//点解铜项目，使用四边形区域控制继电器报警，大于报警阈值发送对应指令

//#define Fire_Alarm  		//连云港项目，火点报警,不可和区域告警相关功能同时启用

//#define brakeMode  //打开此宏定义表示使用制动模式，否则使用滑动模式

//#define BeiFangGuangXue_BootIcon   1//北方光学添加开机图标

//#define WLP   1//WLP项目

//#define MYFirePoint_Alarm	1   //名扬项目火点报警，与连云港项目火点报警Fire_Alarm不可共用

// #define QC_PROJECT 1//千从项目，使用3m起标定

/*****************TestFunction**********************/
//#define TEST_CC     //春春AGC测试

//#define No_Shutter		1	//无挡片功能测试,打开此宏定义则支持无挡片功能,默认不打开，此功能暂且属于测试功能

//#define TEST_SAVE_Image   //测试 4帧数据可存机芯 暂时无挡片功能的共用flash地址测试

//#define IRAY_AutoGOFTest	1   //自动GOF功能测试

//#define  Area2TestDisp  1  //测温区域2单独显示，用于测温采集数据使用

#endif /* SRC_CONFIG_H_ */
