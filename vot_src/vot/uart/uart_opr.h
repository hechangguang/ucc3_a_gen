/*
 * uart_opr.h
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

#ifndef SRC_UART_UART_OPR_H_
#define SRC_UART_UART_OPR_H_
#include "../myheader.h"

#define DATA_MAX_NUM  2048
#define SEND_DATA_MAX_NUM  1300//100

//#ifdef SUPPORT_BoYu_UART_PROTOCOL
/*
 * ZOOM:变倍电机 FOC:聚焦电机 IRIS:光圈电机
 */
//设置命令
#define _CLIENT_EFL_SET     		(unsigned short)0x1001 //指定到固定的焦距位置EFL，聚焦电机跟随
#define _CLIENT_FOC_POS_SET 		(unsigned short)0x1002 //指定到固定的聚焦电机位置
#define _CLIENT_ZOOM_POS_SET 		(unsigned short)0x1003 //指定到固定的变倍位置，聚焦电机不跟随
#define _CLIENT_IRIS_POS_SET 		(unsigned short)0x1004 //指定到固定的光圈电机位置
#define _CLIENT_IRIS_ANGLE_SET 		(unsigned short)0x1005 //指定到固定的光圈电机角度
#define _CLIENT_FOC_GOTO_CAL_POS 	(unsigned short)0x1006 //聚焦电机回到标定位置
#define _CLIENT_FOC_SPD_STEP_ADD 	(unsigned short)0x2001 //单步聚焦+
#define _CLIENT_FOC_SPD_STEP_SUB 	(unsigned short)0x2002 //单步聚焦-
#define _CLIENT_FOC_SPD_CONT_ADD 	(unsigned short)0x2003 //连续聚焦+
#define _CLIENT_FOC_SPD_CONT_SUB 	(unsigned short)0x2004 //连续聚焦-
#define _CLIENT_FOC_SPD_CONT_STOP 	(unsigned short)0x2005 //连续聚焦停
#define _CLIENT_ZOOM_SPD_STEP_ADD 	(unsigned short)0x3001 //单步变倍+
#define _CLIENT_ZOOM_SPD_STEP_SUB 	(unsigned short)0x3002 //单步变倍-
#define _CLIENT_ZOOM_SPD_CONT_ADD 	(unsigned short)0x3003 //连续变倍+
#define _CLIENT_ZOOM_SPD_CONT_SUB 	(unsigned short)0x3004 //连续变倍-
#define _CLIENT_ZOOM_SPD_CONT_STOP 	(unsigned short)0x3005 //连续变倍停

#define _CLIENT_IRIS_SPD_STEP_ADD 	(unsigned short)0x3011 //单步变倍+
#define _CLIENT_IRIS_SPD_STEP_SUB 	(unsigned short)0x3012 //单步变倍-
#define _CLIENT_IRIS_SPD_CONT_ADD 	(unsigned short)0x3013 //连续变倍+
#define _CLIENT_IRIS_SPD_CONT_SUB 	(unsigned short)0x3014 //连续变倍-
#define _CLIENT_IRIS_SPD_CONT_STOP 	(unsigned short)0x3015 //连续变倍停


//回传命令
#define _CLIENT_RELY_T_ 			(unsigned short)0x5001 //温度回传
#define _CLIENT_RELY_EFL_ 			(unsigned short)0x5002 //焦距回传
#define _CLIENT_RELY_ZOOM_POS 		(unsigned short)0x5009 //变倍电机位置
#define _CLIENT_RELY_FOC_POS_ 		(unsigned short)0x5003 //调焦电机位置
#define _CLIENT_RELY_FOC_CRITER 	(unsigned short)0x5008 //当前焦距标定值
#define _CLIENT_RELY_IRIS_ANGLE_ 	(unsigned short)0x500C //光圈电机角度
#define _CLIENT_RELY_IRIS_POS_ 		(unsigned short)0x500D //光圈电机位置

//回传开关（默认开）
#define _CLIENT_DATA_RETURN (unsigned short)0x6003 //数据回传

void BoYuUartOPR(u16 command,u32 data);

//#endif






typedef enum
{
	Baud_9600bps = 9600,
	Baud_19200bps = 19200,
	Baud_38400bps = 38400,
	Baud_57600bps = 57600,
	Baud_115200bps = 115200,
	Baud_sum
}BAUD;

extern BAUD baud;
extern u16 VerifyVal;

enum IRAYCOMMAND
{
	Iray_command_head1 = 0xAA,
	Iray_command_head2 = 0x55,
	Iray_command_operation_word = 0x33,
	Iray_command_tail1 = 0xEB,
	Iray_command_tail2 = 0xAA,
	Iray_command_sum
};

enum VOTCOMMAND
{
	Vot_command_head1 = 0x56,
	Vot_command_head2 = 0x4F,
	Vot_command_head3 = 0x54,
	Vot_command_sum
};

enum MEASURECOMMAND
{
	Measure_command_head1 = 0x55,
	Measure_command_head2 = 0xAA,
	Measure_command_sum
};

enum REFRIGERATIONCOMMAND
{
	Refrigeration_command_head1 = 0x55,
	Refrigeration_command_head2 = 0xAA,
	Refrigeration_command_head3 = 0x10,
	Refrigeration_command_LenH = 0x00,
	Refrigeration_command_LenL = 0x09,
	Refrigeration_data_LenH = 0x00,
	Refrigeration_data_LenL = 0x04,
	Refrigeration_command_addressH = 0x00,
	Refrigeration_command_write = 0x01,
	Refrigeration_command_read = 0x00,
	Refrigeration_command_sum
};

enum HUANYUCOMMAND
{
	HuanYu_Command_Head = 0xF0,
	HuanYu_Command_Address = 0x26,
	HuanYu_Command_End = 0xFF,
	HuanYu_command_sum
};

enum ZONGLINCOMMAND{
	ZhongLin_Command_Head1 = 0x55,
	ZhongLin_Command_Head2 = 0xAA,
	ZhongLin_Command_Tail = 0xF0,
	ZhongLin_Servo_Head = 0xFF,
	ZhongLin_Command_sum
};

void UART_Init(void);
void uart0_init(void);
void UartInterrupt_Sub(void);
void uart_command(void);
void Uart_Receive(void);
void FirmwareTransferInit(void);
void ClearUartData(void);

void FPGACommand_Init(void);
void FPGACommandData_Packets(void);
void FPGACommand_Tem(void);

void BoYuUartOPR(u16 command,u32 data);

#endif /* SRC_UART_UART_OPR_H_ */
