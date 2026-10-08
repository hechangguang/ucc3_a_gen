/*
 * function.h
 *
 *  Created on: 2023年4月20日
 *      Author: yan
 */

#ifndef SRC_COMMON_FUNCTION_H_
#define SRC_COMMON_FUNCTION_H_

#include "../myheader.h"
#include <menu/votmenu.h>
#include <common/common.h>

/*
static inline void write_u32(u32 data, u32 address){
    *((volatile u32*) address) = data;
}

*/
// #define APB3_W_U32(addr, data)  

// #define ReadADAVERAGE()		 1
// #define ReadImgRaw_Avg()	 1  
// #define //ReadADMAX()			 1
// #define ReadADMIN()			  1
// #define SetLevel(val)		 1
// #define SetSpan(val)		  1

// #define FPGA_Bak_AVG()	     1
// #define IR_NUC_OPR(val)		  1
#define ReadDETAD()		1
#define ReadDETAD_TEST()		1 

#define LEVEL						1
#define SPAN						2

void BAD_init(void);
void BAD_Init_INC_DEC(void);
void BAD_MarkOpr(u16 x,u16 y);
void BAD_CancelOpr(u16 x,u16 y);
void BADMarkSemiAutoOpr(void);

void NUC_init(void);
void Nuc_Init_INC_DEC(void);
void Calc_NUC_INC(void);
void Calc_NUC_DEC(void);
void NUC_Calculate(void);
void NUC_Calculate_1(void);

void NUC_init_2(void);
void Calc_NUC_INC_2(void);
void Calc_NUC_DEC_2(void);
// void NUC_Calculate_2(void);

void auto_widthLevel_cal(uint8_t index);
// void manual_operation_widthLevel_cal(uint8_t index);
void Write_ddr_NUC(uint16_t addry,uint16_t addrx,uint16_t data);
void test_nuc(void);
void ReadAD(void);


void OOC_init(void);
void OOCSetConstantValue(u8 val);
void OOCSysetemInit(void);
void OOC_Cal(void);

void TimeInit(void);
void PowerOnTime(void);

void MirrorBadSppot(uint8_t pos);
void MirrorBadSppotInit(void);
void MirrorBadRecover(u8 BeforeVal);
void FPGAShutterOpr(void);//TEST_1
void SensorOpr(void);//TEST_1


void BadCorrectOpration(u16 x,u16 y);
u8 PC_MarkBadPixels(u16 Pixels_X,u16 Pixels_Y);
u8 PC_CancalOneMarkBadPixels(u16 Pixels_X,u16 Pixels_Y);
void PC_CorrectBadPixels(void);
void PC_CancalAllMarkBadPixels(void);
void Enter_PC_BPCMaualMode(void) ;
void EXIT_PC_BPCMaualMode(void);
void PCMoveCursorBPC(ts_u16 pos_x,ts_u16 pos_y);

extern uint16_t *ddr_NUC;
extern uint16_t *ddr_NUC_HDDR;
extern u16 *ddr_NUC_IR_Image_Bak;
extern uint16_t NUC_value[];
extern uint16_t BAK_SUM_AVG;
extern uint16_t DV_SUM_AVG;
extern u8 RefreshData;


//2025-11-27 半自动校正坏点框的绘制
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
ts_s32 ts_lib_set_pixel(ts_nodebuf_s_t *pst_osd, ts_u16 x, ts_u16 y, ts_u08 color);
ts_s32 ts_lib_fill_screen_black(ts_nodebuf_s_t *pst_osd, ts_u08 color);
ts_s32 ts_lib_draw_corner_rect(ts_nodebuf_s_t *pst_osd, ts_u16 x, ts_u16 y, ts_u16 width, ts_u16 height, ts_u08 line_thickness, ts_u08 corner_length, ts_u08 color);
void semiAutoMode_clearScreen(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h);
void semiAutoMode_DrawRect(ts_nodebuf_s_t *pst_osd, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u16 top_left_x, ts_u16 top_left_y, ts_u16 bottom_right_x, ts_u16 bottom_right_y);
void semi_auto_point_cap(ts_u16 point_x, ts_u16 point_y);
ts_s08 ts_lib_selfAdaptive_AreasScreen_black(ts_nodebuf_s_t *pst_osd, ts_u16 Area_Lx, ts_u16 Area_Ly, ts_u16 Area_Rx, ts_u16 Area_Ry);
#endif /* SRC_COMMON_FUNCTION_H_ */
