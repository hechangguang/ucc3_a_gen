/*
 * TempCal.h
 *
 *  Created on: 2023年11月14日
 *      Author: WU
 */

#ifndef SRC_COMMON_TEMPCAL_H_
#define SRC_COMMON_TEMPCAL_H_

#include "../myheader.h"
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_media.h>

#define TEMP_LENGTH	120
#define	FLASH_TEMP_FLAG1	0xAA
#define	FLASH_TEMP_FLAG2	0xA2
#define N25Q032A_TEMPCAL_ADDRESS		0x4B3000

#define TEMPERATURE_POINTS_NUMBER		0X0003
#define TEMPERATURE_AREAS_NUMBER		0X0002

typedef enum {
	TEMP_NORMAL_DISP_ORIAD = 0,		// 正常模式：显示原始AD
	TEMP_NORMAL_DISP_LUT_AD,		// 正常模式：显示测温AD
	TEMP_NORMAL_DISP_TEMP,			// 正常模式：显示温度
	TEMP_CALIBRATION_DISP_AD,		// RUN 模式：显示 AD

	TEMP_DISP_MODE_MAX      // 边界检查
} temp_disp_tyep_e;

typedef struct SaveSpot {
	ts_u16 spot_X_Save;
	ts_u16 spot_Y_Save;
}*PT_SaveSpot;

typedef struct SaveArea {
	ts_u16 Area_Lx;
	ts_u16 Area_Ly;
	ts_u16 Area_Rx;
	ts_u16 Area_Ry;
}*PT_SaveArea;

struct _Temp_Parameter
{
	uint16_t	AD_sensor;
	uint8_t 	Emiss;					//辐射率
	uint16_t 	distance;			//距离
	uint16_t 	Tamb;						//环境温度
	uint8_t 	RelHum; 					//相对湿度
	uint16_t 	Trefl;						//反射温度
	uint16_t	SpotDisplay;
	uint16_t	AreaDisplay;
	uint8_t		Lens_Set;
	int16_t  	TEMP_data1[12];
	int16_t  	TEMP_data2[12];
	int16_t  	TEMP_data3[12];
	uint16_t 	AD_data1[12];
	uint16_t 	AD_data2[12];
	uint16_t 	AD_data3[12];
		
	int16_t 	Temp_correct1;
	int16_t 	Temp_correct2;
	int16_t 	Temp_correct3;

	struct SaveSpot SaveSpot[TEMPERATURE_POINTS_NUMBER];
	ts_u16 reserved_1[(9-TEMPERATURE_POINTS_NUMBER)*2];
	struct SaveArea SaveArea[TEMPERATURE_AREAS_NUMBER];
	ts_u16 reserved_2[(4-TEMPERATURE_AREAS_NUMBER)*2];

	uint8_t 	Second_CalibFlag;		//二次校准标志位，0-关闭，1-开启
	int16_t  	Second_TEMP1[12];
	uint16_t 	Second_AD1[12];
	int16_t  	Second_TEMP2[12];
	uint16_t 	Second_AD2[12];
	int16_t 	Second_correct1;
	int16_t 	Second_correct2;
	uint8_t 	IncreaseNum[1];  //4字节对齐，字节补充 目前332
};

struct _Second_Calibration_Parameter
{
	uint8_t CalibFlag; //是否进入标定模式
	uint8_t CalibMode; //标定模式的状态
	uint8_t Global_offsetFlag; //标定全局偏移量
	uint16_t CalibAD; //标定AD值
	int16_t CalibTemp; //标定温度
};
extern struct _Second_Calibration_Parameter SecondCalib_Parameter;

extern u16 TempXpos;
extern u16 TempYpos;
extern u8 PointTraceFun;


extern uint16_t AD_data[12];
// extern uint16_t AD_data1[12];
// extern uint16_t AD_data2[12];
//extern uint16_t AD_data3[12];
//extern uint16_t K_value[11];
extern int16_t  TEMP_data[12];
// extern int16_t  TEMP_data1[12];
// extern int16_t  TEMP_data2[12];
//extern int16_t  TEMP_data3[12];

extern uint16_t in_tempAD;
extern int16_t SensorTemp;
extern int16_t Temp_correct;
extern int16_t  spot_temp_int;
extern uint16_t spot_point;

// extern int16_t Temp_correct1;
// extern int16_t Temp_correct2;
// extern int16_t Temp_correct3;

extern uint16_t AD_correct;

extern float TiTatm_H4;
extern float TiEmiss_H4;
extern float TiEmiss;

extern int16_t  capture_temp_int;
extern int16_t capture_temp_min;

extern	int16_t  spot_temp1,spot_temp2,spot_temp3,spot_temp4,spot_temp5,spot_temp6,spot_temp7,spot_temp8,spot_temp9;
extern	int16_t	Temp0_max,Temp0_min,Temp0_avg,Temp1_max,Temp1_min,Temp1_avg,Temp2_max,Temp2_min,Temp2_avg,Temp3_max,Temp3_min,Temp3_avg;
extern	uint16_t	spot_AD1,spot_AD2,spot_AD3,spot_AD4,spot_AD5,spot_AD6,spot_AD7,spot_AD8,spot_AD9;

extern struct _Temp_Parameter	Temp_Parameter;
void  AD_calculate(void);
void  temp_calculate(void);
void temp_display(void);
extern void temp_convert_display_H(uint32_t * addr,int16_t * temp_int) ;
void temp_convert_display_L(uint32_t * addr,int16_t * temp_int);         //低16位温度值显示
s16  AreaTemp_convert_AVG(uint16_t temp_int);        //区域平均温计算
void READ_Sensor_Temp(void)	;		//读取温度传感器数据
void Test_Value_Send(void);

void Tamb_TiEmiss_cal(int16_t TEMP);

ts_void TEMP_Parameters_init(void);
void Read_TempParameterFromFlash(void);
ts_void Templut_Menu1_LoadDefault(void);
ts_void Templut_Menu2_LoadDefault(void);
ts_s08 Read_TempParameter_cfg(struct _Temp_Parameter* Temp_Parameter_cfg);
ts_s08 Write_TempParameter_cfg(struct _Temp_Parameter* Temp_Parameter_cfg);

void load_AD2Temp(void);
void ReDispSpot1Message(void);
void ReDispArea1Message(void);
void ReDispCaptureMessage(void);
// void SpotMessage(char *Sportname,ts_u16 SportX,ts_u16 SportY,ts_u16 Sportid);
void ReDispQuadMessage(void);
void TemLevel_convert(void);
int16_t Distance_Correct(int16_t Temp);
int16_t Tamb_TiEmiss_Correct(int16_t temp0);
int16_t	Temp_AllCorrect(int16_t Temp0);
uint16_t LookUP_TemtoAD(int16_t TemVal,uint8_t TemLevel);
void MaxTempPointTraceFunction(u8 SwitchFun);
void MaxTempAlarmFunction(void);

ts_f32 LookUP_ADtoTemp(uint16_t ADVal,uint8_t TemLevel);
void SecondCalib_Cal(uint8_t TemLevel);
ts_u16 SecondCalib_TempToAD(ts_u16 StartNum,ts_f32 Kx,uint8_t TemLevel);
ts_u16 Calib_TempToAD_300to600(ts_u16 StartNum);
void Sport_Display_Process(struct Spot* temp_spot_t);
void Area_Display_Process(struct Area* temp_area_t);
void Disp_Capture_Spot_MIN_info(ts_u08 visible);
void Disp_Capture_Spot_MAX_info(ts_u08 visible);
/* 启用/关停温度矩阵表 */
void vot_update_templut_table(ts_u08 vEnable_type, ts_u32 ad_mean_u32);
/* upl下提取帧数据 */
ts_s08 votMalloc_FrmAddr(ts_s32 s32_nPos);
ts_s08 votDumpTemplut_SingleFrmData(ts_s32 s32_nPos);
ts_s08 votCalcul_memory_TempOrAd(ts_s32 s32_nPos);
ts_u16 UART_ConvertRawAd_To_CalibAd(ts_u16 raw_ad);
ts_u16 UART_ConvertRawAd_To_tempAd(ts_u16 raw_ad);
int16_t UART_ConvertRawAdToTemp(ts_u16 raw_ad);
ts_u16 TP_GetTempFromMem(ts_u16 spot_X_Pos, ts_u16 spot_Y_Pos);
ts_s08 TZ_GetTempFromMem(struct Area* vAreainit);
void CAP_GetTempFromMem(struct Spot* vSpot_max, struct Spot* sVpot_min);
void Auto_GOFSet_HAND(void);
/* FIRE模块调用 */
ts_s32 votSafe_execute_FireModule(void);
/* PC机进入/退出测温标定 */
ts_void EnterORExit_PC_templut_Calib(void);
ts_void vot_Set_imgParam_row_data(ts_u08 mode, ts_u08 lens);
ts_void vot_Set_DistCorr_Fact(void);
void SecondCalib_Parameter_Init(void);
void SecondTemp_Autocalibration(void);
void SecondCalib_Offset_Function(void);
ts_s08 votLocal_TempAD_TO_RawAD(ts_u16 temp_ad, ts_u16* t_rawAD);
uint16_t temp_to_hex_with_sign(float temp);
ts_u16 FUN_Return_MK(ts_u08 vEnable_type);
#endif /* SRC_COMMON_TEMPCAL_H_ */
