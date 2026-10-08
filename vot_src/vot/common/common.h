/*
 * common.h
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

#ifndef SRC_COMMON_COMMON_H_
#define SRC_COMMON_COMMON_H_

#include "../myheader.h"
#include "snr_comm_vot.h"

#define Frame_x    640
#define Frame_y    512
#define Frame_C_STRIDE   (Frame_x+15)/16*16*2


#define color1  0x2000
#define color2  0x0400
#define color3  0x00C0
#define color4  0x0020
#define color5  0x0005

#define write_u8(value, addr)  (1)
#define write_u16(value, addr) (1)
#define write_u32(value, addr) (1)
#define read_u8(addr) (1)
#define read_u16(addr) (1)
#define read_u32(addr) (1)

//#define SetIO_0(val)	  	gpio_setOutput(GPIO0, (read_u32(GPIO0)&0xFFFFFFFE)|(0x00000001&val<<0))
//#define SetIO_1(val)	  	gpio_setOutput(GPIO0, (read_u32(GPIO0)&0xFFFFFFFD)|(0x00000002&val<<1))
// #define SetIO_Shutter(val)	1
// #define SetIO_uatr(val)	  	1

//#define ReadKey()		  read_u8(IO_APB_SLAVE_0_INPUT_key)
#define ReadKey()		   (read_u8(0xf8100095)&0x03)
#define Readtest()		  read_u8(0xf8100003)
#define ReadtestALL()	  read_u32(0xf8100000)
#define ReadtestALL2()	  read_u32(0xf8100004)

#define Key_Shutter  0x0E
#define Key_IR_SW    0x0D
#define Key_Rerver   0x0B
#define Key_TFilter  0x07
#define key_Color	0x02
#define key_Scaler	0x01

#define SetIR_SW_(val)		  
#define SetBaking_SW_(val)	  
#define SetIR_SW(val)		
#define SetBaking_SW(val)	  
#define SetFreeze_img(val)	 
#define SetNUC_SW(val)	      
#define SetTFilter(val)		  
#define SetTset_SW(val)	      



#if 0
#define menu_layer1(data,addr)  write_u16((read_u16(addr)&0xFFF8)|(0x0007&data<<0),addr);
#define menu_layer2(data,addr)  write_u16((read_u16(addr)&0xFFC7)|(0x0038&data<<3),addr);
#define menu_layer3(data,addr)  write_u16((read_u16(addr)&0xFC7F)|(0x0380&data<<6),addr);
#define menu_layer4(data,addr)  write_u16((read_u16(addr)&0xE1FF)|(0x1E00&data<<9),addr);
#define menu_layer5(data,addr)  write_u16((read_u16(addr)&0x1FFF)|(0xE000&data<<13),addr);

#else

#define menu_layer1(data,addr)  1
#define menu_layer2(data,addr)  1
#define menu_layer3(data,addr)  1
#define menu_layer4(data,addr)  1
#define menu_layer5(data,addr)  1

#endif

typedef enum type_Func{
	Done,
	Do,
	Init,
	Save,
	Calc,
	Type_Sum
}Type_Func;

int update_rotation_param(void);
void test_txt(void);
void test0(void);

/*********************************************************************************/
void InitHyram(void);
void SystemInit(void);
// void InitSystemStart(void);
void delay_25ms (uint32_t time_25ms);
void delay_1ms (uint32_t time_1ms);
void delay_1s (uint32_t time_1s);
uint16_t english_to_unit_temp(uint16_t tmp16);
uint16_t unit_to_english_temp(uint16_t tmp16);
uint16_t chang_unit_For_CTemp(int16_t tmp16);
uint16_t chang_english_For_Span(uint16_t tmp16);
uint16_t chang_unit_For_Span(uint16_t tmp16);


void KeyOpr(void);
void Int_Shutter_pro(void);		//内校正
void Ext_Shutter_pro(void);		//外校正
void Auto_INT(void);            //自动调节INT值
void shutterOpr(u8 sw);
void AutoTempCorrectionOverTime(void);
void Auto_GOF(void);
void ShieldProtect(void);

void ColorBarOut(int y_size,int x_size, int stride,u32 frameAdr);
void testOpr(void);
void OneFramePrintf(void);
void CameralinkSW(void);
void  AlgorithmSW(u8 state); // 1为开算法 0为关闭算法

void detParaInit(void);
void DETParSelect(void);
void VotDETParSelect(void);
void dynamicDETParChange(void);
void displayReminderInfo(Type_Func func);

ts_void Sensor_Init(void);
ts_u16 Device_I2c0_Data_Get(ts_u08 addr_type, ts_u08 addr7);
ts_void MotorSen_Init(void);

ts_u08 GetDetTempStatus(ts_u08 is_inited_flag);

extern struct SystemPara      systemPar;
extern struct LeveleSpanPara  LeveleSpanPar;
extern LineASCIIBufStruct     LineASCIIBuf;
extern CharDispStruct         Display;
/*********************************************************************************/

extern u8 check1;
extern u8 check2;
extern u8 check3;
extern u8 check4;
extern u8 AutoTempFlag;
extern u8 AutoGOF_Flag;

typedef struct _DetPara {
	u8 lastFlag;
	u8 statusFlag;
	u8 count;
	u8 count1;
	u8 count2;
	u8 value;
}DetPara;

typedef struct libup_ctx_s {
    /* 节点动态申请，此处保存节点首地址 */
    ts_nodebuf_s_t *past_node;

    /* 保存当前的 upload 节点 */
    ts_u32 u32_pos;

    /* 当前图像宽高信息 */
    ts_u16 u16_width;
    ts_u16 u16_height;

    /* 节点数 */
    ts_u32 u32_node_cnt;

    /* 指定获取的帧数 */
    ts_u32 u32_frm_cnt;

    /* 获取计数，不能大于 u32_frm_cnt */
    ts_u32 u32_get_cnt;

    /* 节点链表 */
    ts_cpl_list_head_s_t st_ready_head;

    /* isp 设备句柄 */
    ts_void *pv_isp;

    /* 同步信号 */
    ts_cpl_semphr_s_t st_sema;

    /* 是否为 DMA 内存 */
    ts_s32 s32_is_dma;
} libup_ctx_s_t;


ts_void vot_CloseTempLut_attr(void);
ts_void vot_SetTempLut_attr(ts_u32 ad_mean_u32);

#endif /* SRC_COMMON_COMMON_H_ */
