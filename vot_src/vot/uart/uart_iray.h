/*
 * uart_iray.h
 *
 *  Created on: 2023年10月17日
 *      Author: yan
 */

#ifndef SRC_UART_UART_IRAY_H_
#define SRC_UART_UART_IRAY_H_


#include "../myheader.h"

enum IRAY_COMMAND_STATUS
{
	CommandSendingTimeout = 0xF1,
	CommandNotExist = 0xFB,
	DRCCalibrationFailure = 0xFD,
	FrameHeaderError = 0xFF,
	IRAY_Command_Status_Sum
};

struct IRAY_TOOLS
{
	u8 COUNT;
	u8 CHECKSUM;
};
extern struct IRAY_TOOLS IrayTools;


void UartDataHanding(void);
void MessageReturnError(u8 ErrorMes);
void receiveData_4byte(void);
void receiveData_5byte(void);
void receiveData_6byte(void);
void receiveData_7byte(void);
void receiveData_8byte(void);
void receiveData_9byte(void);
void receiveData_12byte(void);
void receiveData_13byte(void);
void VerifyData(u8 *DRCBUFF);
//void Send_Check_Digit(void);

#endif /* SRC_UART_UART_IRAY_H_ */

