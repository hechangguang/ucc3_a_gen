/*
 * uart_common.h
 *
 *  Created on: 2023年10月17日
 *      Author: yan
 */

#ifndef SRC_UART_UART_COMMON_H_
#define SRC_UART_UART_COMMON_H_

#include "../myheader.h"


extern u8 ZhongLinShutterFlag;
extern u16 ZhongLinTime;
extern u8 ShutterFlag;

/*
 * Parameter
 * */

/***************中林项目************************/
enum ZHONGLINGSTATUS{
	ZhongLin_Status_ReceivedSuccess = 0x00,
	ZhongLin_Status_CalibrationFailure = 0x06,
	ZhongLin_Status_sum
};

enum ZHONGLINFUNCTION{
	ZhongLin_Function_STATUS,
	ZhongLin_Function_SETUP,
	ZhongLin_Function_VIDEO,
	ZhongLin_Function_ADVANCE = 0xA0,
	ZhongLin_Function_EXPERT = 0xA1,
	ZhongLin_Function_sum
};

enum ZHONGLINPAGE{
	ZhongLin_Page_First,
	ZhongLin_Page_Second,
	ZhongLin_Page_Third,
	ZhongLin_Page_sum
};


/*
 * Function
 * */

/********************测距模块协议******************/
void UartPoweronselfcheck(void);

/*****************非制冷红外机芯串口协议**************/
void UartDistanceMeasurement(void);
void UartXA338(void);

/*****************中波制冷项目*************************/
void UartRefrigeration(void);
void Return_Refrigeration_Message(void);
u8 Refrigeration_Data_Verify(u8 *Databuf);

/****************寰宇项目***********************/
void UartHuanYu(void);
void Return_HuanYu_Message(void);
u8 HuanYu_Data_Verify(u8 *Databuf,u8 Len);

/***************中林项目************************/
void UartZhongLin(void);
void UartZhongLinServo(void);
void Return_ZhongLin_Status(u8 Status);
void Return_ZhongLin_Message(void);
void Return_ZhongLin_Servo_Message(void);
u8 ZhongLin_Data_Verify(u8 *Databuf);
u8 ZhongLin_Servo_Data_Verify(u8 *Databuf);

#endif /* SRC_UART_UART_COMMON_H_ */
