/*
 * @Author: shixiaobo xiaobo.shi@votinfrared.com
 * @Date: 2025-10-28 10:45:09
 * @LastEditors: shixiaobo xiaobo.shi@votinfrared.com
 * @LastEditTime: 2025-10-28 12:33:07
 * @FilePath: /ucc3_a_gen/sample/vision/src/vot/common/IIC_OPR.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * IIC_OPR.h
 *
 *  Created on: 2023年5月25日
 *      Author: yan
 */

#ifndef SRC_COMMON_IIC_OPR_H_
#define SRC_COMMON_IIC_OPR_H_

#include "../myheader.h"


#define mode_select					0x0100
#define I2C_CTRL                    SYSTEM_I2C_0_IO_CTRL

/***********MCP3221_addr*************/
//2026.05.28 地址选择改用菜单切换
#define FocusMCP_MYAddresss     (0x99 >>1) //5T  0x9b 4T 0x99  7T 0x9F
#define ZoomMCP_MYAddresss      (0x9B >>1)

#define FocusMCP_SS1Addresss    (0x9B >>1) //5T  0x9b 4T 0x99  7T 0x9F
#define ZoomMCP_SS1Addresss     (0x9F >>1)
// #ifdef SS1
// 	#define Focus_MCP3221_addr    0x9F //5T  0x9b 4T 0x99  7T 0x9F
// 	#define Zoom_MCP3221_addr    0x9b
// #else
// 	#define Focus_MCP3221_addr    0x99 //5T  0x9b 4T 0x99  7T 0x9F
// 	#define Zoom_MCP3221_addr    0x9b
// #endif

/***********Sensor_addr*************/
//#ifdef  SensorTMP112
//	#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
//    	#define  SlaveAddressByte1  0x90
//        #define  SlaveAddressByte2  0x91
//
//        #define  SlaveAddressByte3  0x92
//        #define  SlaveAddressByte4  0x93
//
//	#else
//        #define  SlaveAddressByte1  0x90
//        #define  SlaveAddressByte2  0x91
//	#endif
//
//#else
//        #define  SlaveAddressByte1  0x92
//        #define  SlaveAddressByte2  0x93
//#endif
extern u8 SlaveAddressByte1;
extern u8 SlaveAddressByte2;
extern u8 SlaveAddressByte3;
extern u8 SlaveAddressByte4;

/* I2C设备地址类型 */
typedef enum I2c_AddrType
 {
    SensorAddr = 0,
    MCP3221_Addr,
	MotorSen_Addr,
    OtherAddr,
} I2c_AddrType_t;

void bsp_printf_c(int c);
void bsp_printf_s(char *p);
void bsp_printf_d(int val);
void bsp_printf(const char *format, ...);
int bsp_puts(char *s);
int putchar(int c);
void print_hex(uint32_t val, uint32_t digits);

/************************** Function File ***************************/
u16 TMP112_Read2ByteFromSlave_IIC(uint8_t addr,uint8_t reg,uint8_t value);
u8 TMP112_Write2ByteToSlave_IIC(u8 addr,u8 reg,u8 value1,u8 value2);
void Reg_Out32(u32 addr,u32 data);
u32 Reg_In32(u32 addr);
// int assert(int cond);
// void i2c_init(void);
void Init_TMP112(void);
float READ_TMP112_Temp(void);

int I2c_WriteRegData(u16 reg,u8 data);


/************************** ADV7391 File ***************************/
extern uint8_t 	 ADV7391_rxbuffer[1];

void Init_ADV7391(void);
void Init_ADV7391_2(void);
void WriteADV7391(uint8_t ADV7391_reg,uint8_t ADV7391_value);
void TestDV7391(uint8_t ADV7391_reg,uint8_t ADV7391_value);


/************************** MCP3221 File ***************************/
// ts_u16 Read2ByteFromMCP3221_IIC(uint8_t addr ) ;
float READ_TMP117_Temp_ZL(void);//中林项目测温小板
#endif /* SRC_COMMON_IIC_OPR_H_ */
