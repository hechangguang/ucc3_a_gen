/*
 * Zoom_Lens.h
 *
 *  Created on: 2024Âπ¥5Êúà11Êó•
 *      Author: Administrator
 */

#ifndef SRC_COMMON_ZOOM_LENS_H_
#define SRC_COMMON_ZOOM_LENS_H_

#include "../myheader.h"

/*********MotorControl**************/
#define N25Q032A_Motor_Address 0x4B6000

#define FLASH_MOTOR_FLAG1 0xA3
#define FLASH_MOTOR_FLAG2 0xA4
#define MOTOR_LENGTH 1202
#define MOTORPRESET_NUM 16
#define MOTORPOS_NUM 30

#define  FPGAMotorSet      	   	0xf8100080
#define  FPGAMotor2Set      	0xf81000A0
#define  FocusAreaSetLeft      	0xf81000A4
#define  FocusAreaSetRight     	0xf81000A8

#define  ReadImageSum()    	  	read_u32(0xf810007C)
#define  MotorPolarity(c)		//write_u32((read_u32(FPGAMotorSet)&0xFFEFFFFF)|((c&0x01) <<20),FPGAMotorSet)
#define  MotorState(c)			//write_u32((read_u32(FPGAMotorSet)&0xFFF0FFFF)| ((c &0x0F)<<16),FPGAMotorSet)
#define  MotorStepNum(c)		//write_u32((read_u32(FPGAMotorSet)&0xFFFF00FF)| ((c &0xFF)<<8),FPGAMotorSet)
#define  MotorStepLenth(c)		//write_u32((read_u32(FPGAMotorSet)&0xFFFFFF00)| ((c &0xFF)<<0),FPGAMotorSet)

#define  Motor2Polarity(c)		//write_u32((read_u32(FPGAMotor2Set)&0xFFEFFFFF)|((c&0x01) <<20),FPGAMotor2Set)
#define  Motor2State(c)			//write_u32((read_u32(FPGAMotor2Set)&0xFFF0FFFF)| ((c &0x0F)<<16),FPGAMotor2Set)
#define  Motor2StepNum(c)		//write_u32((read_u32(FPGAMotor2Set)&0xFFFF00FF)| ((c &0xFF)<<8),FPGAMotor2Set)
#define  Motor2StepLenth(c)		//write_u32((read_u32(FPGAMotor2Set)&0xFFFFFF00)| ((c &0xFF)<<0),FPGAMotor2Set)

#define  FocusAreaSetLx(c)     // write_u32((read_u32(FocusAreaSetLeft)&0xFFFF0000)| ((c &0xFFFF)<<0),FocusAreaSetLeft)
#define  FocusAreaSetLy(c)     // write_u32((read_u32(FocusAreaSetLeft)&0x0000FFFF)| ((c &0xFFFF)<<16),FocusAreaSetLeft)
#define  FocusAreaSetRx(c)      //write_u32((read_u32(FocusAreaSetRight)&0xFFFF0000)| ((c &0xFFFF)<<0),FocusAreaSetRight)
#define  FocusAreaSetRy(c)      //write_u32((read_u32(FocusAreaSetRight)&0x0000FFFF)| ((c &0xFFFF)<<16),FocusAreaSetRight)

#define  ReadMachineStatus()    read_u32(IO_APB_PAT_0X34)&(~0xDFFFFFFF)

/**************MotorControlParameters***********************/
typedef enum MotorSpeed
{
	StepSpeed = 1,
	NormalSpeed = 10,
	InitSpeed = 15,
	FullSpeed = 100,
	Speedset1 = 85,
	Speedsum
}MSpeed;

typedef enum MotorState
{
	StopState,
	NormalState,
	SpeedState,
	FixState,
	ForceStopState,
	Statesum
}MState;

typedef enum MotorDirection
{
	MotorReversal,
	MotorForward,
	Directionsum
}MDirection;

typedef enum MotorPresetState
{
	FocusState = 1,
	ZoomState,
	BothState,
	MotorPresetStatesum
}MPresetState;

typedef enum MotorSwitchState
{
	OFF_State,
	ON_State,
}MSwitchState;

/**************MotorFocusParameters**********************************************/
typedef struct _motorFlag{
	u8 Flag;
} motorFlag_t;
extern motorFlag_t MotorFlag;

typedef struct MotorMeasure
{
	u8 MeasureFocusFlag;
	u32 MeasureFocusSum;
	u8 MeasureFocusNum;
	u32 LimitStepMax;
	u32 LimitStepMin;

	u8 MeasureZoomFlag;
	u32 MeasureZoomSum;
	u8 MeasureZoomNum;
	u32 LimitZoomMax;
	u32 LimitZoomMin;

	MDirection focusForward;
	MDirection focusReversal;
	MDirection zoomForward;
	MDirection zoomReversal;
}MotorMeasureParameters;
extern MotorMeasureParameters MeasureParameters;

typedef struct MotorFocus
{
	u8 FocusFlag;
	u8 ValidSteps;
	u8 BackSteps;

	u32 NowSite;
	u32 MaxValue;
	u32 FocusSite;
	//u32 ImageValue[3];
	u32 ImageValue[9];

	u32 FocusStepMax;
	u32 FocusStepMin;

	u8 UpNum;
	u8 DownNum;
	u32 LastSite;

	u32 PresetPos;
	u8 PresetRightFlag;
	u8 PresetLeftFlag;

	u8 FocusSpeed;
	u32 DownImage[2];
}MotorFocusParameters;
extern MotorFocusParameters  FocusParameters;

typedef struct MotorZoom
{
	u8 ZoomFlag;
	u32 ZoomSite;
	u32 PresetPos;
	u8 PresetRightFlag;
	u8 PresetLeftFlag;

	u8 ZoomSpeed;

//	u8 FlowerFocusFlag;
}MotorZoomParameters;
extern MotorZoomParameters  ZoomParameters;

typedef struct Voltagesample
{
	u8 SampleFlag;
	u8 StartFlag;
	u8 Temperature;
	u32 VoltagePos;

	float SensorTemp;
	float Position;
}VoltageSample;
extern VoltageSample VoltageSamplePara;

typedef struct motorFlashPara{
	ts_s32 *FocusMotorPos;
	ts_s32 *ZoomMotorPos;

//	MotorFlashPara *next;
}MotorFlashPara;
extern MotorFlashPara MotorPresetParemeters[MOTORPRESET_NUM];

typedef struct motorPresetPara{
	u8 CalibrationFlag;
	u8 PresetNum;
	u32 CalibrationPos;
	u8 CalibrationNum;

//	u8 SearchFlag;
	u8 PresetSwitch;
	u8 FollowFlag;
	u8 FollowSwitch;

	u8 ZoomPresetSpeed;
	u8 FocusPresetSpeed;

	u8 ZoomOperateFlag;
//	u8 CompensateOperateFlag;
}MotorPresetPara;
extern MotorPresetPara MotorPresetParameters;

typedef struct motorStoragePara{
	ts_s32 Focus_MotorSite[MOTORPRESET_NUM][MOTORPOS_NUM+1];
	ts_s32 Zoom_MotorSite[MOTORPRESET_NUM][MOTORPOS_NUM+1];

}MotorStoragePara;
extern MotorStoragePara MotorStorageParameters;
extern u8 FocusTable_AorM;
extern ts_f32 ZoomMotorSen;
extern ts_u08 MCP3221ReadFlag;
extern ts_u08 Focus_MCP3221_addr;    
extern ts_u08 Zoom_MCP3221_addr; 
/************MotorFunction**************************************/
ts_void MoterFun_Init(ts_void);
// ts_s32 MotorOperate(ts_void *pv_param);
void FocusRelated(void);
void ZoomRelated(void);
void PresetRelated(void);
void MotorFlagRelated(void);
ts_s32 AFS_EvaluationFuc_Cal(ts_void);
ts_void AFS_Focus_init(ts_void);
ts_void AFS_Focus_Adjust(ts_void);

u32  GetVol(u8 MCP3221_addr);
ts_s32 Get_Encoder_Vol(u8 MCP3221_addr);  //编码器计数读取
void Encoder_Clear(ts_u32 setval,ts_u08 Type);
void FocusMotorOperate(u8 state,u8 speed,u8 direction,u8 stepnum);
void ZoomMotorOperate(u8 state,u8 speed,u8 direction,u8 stepnum);
void BubbleSort(u32 a[],u8 len);
void FocusAreaSet(u16 Lx,u16 Ly,u16 Rx,u16 Ry);
ts_void Encoder_irq_cb(ts_u32 u32_channel, ts_void * pv_priv);
ts_void pwm_bind_encoder_irq_set(ts_s32 s32_tp_pwm, ts_u32 u32_qdr_ch);
void MotorEncoder_Init(void);

void FocusParaInit(void);
void MeasureParaInit(void);
void MotorInit(void);
void MotorModeSelect(void);
void Motor_AddressType_Select(void);

void MotorAutoMeasure(void);
void FocusRangeMeasure(void);
void ZoomRangeMeasure(void);
void MotorAutoFocus(void);
void FocusRangeMeasure_encoder(void);
void ZoomRangeMeasure_encoder(void);

void ManualZoom(void);
u16 Pos_Calibration_extend(float DVal);
/**********Test**************/
void AutoFocusVoltageSample(void);
void VoltageSamplePareInit(void);
ts_f32 READ_TMP117_Temp(void);
float GetSensor(void);
u8 CompareTemperature(float temp);

void Write_MotorParameterToFlash(void);
void MotorFlashParaInit(void);
void Read_MotorParameterFromFlash(void);

void PresetPositionParaInit(void);
void PresetPositionCalibration(void);

void FocusPresetPositionRightOperate(void);
void FocusPresetPositionLeftOperate(void);
void FocusPresetPositionOperate(void);
void ZoomPresetPositionRightOperate(void);
void ZoomPresetPositionLeftOperate(void);
void ZoomPresetPositionOperate(void);

u8 JudgeMotorState(u8 state,u32 FocusPresetPos,u32 ZoomPresetPos);
void SelectPresetFlag(u8 state,u32 FocusPresetPos,u32 ZoomPresetPos);
void PresetPosSearch(u8 *SW);
void FollowFocus(u8 SW,MotorFlashPara *MotorPos,ts_s32 NowSite);
void MotorSensor_Read(void);

#endif /* SRC_COMMON_ZOOM_LENS_H_ */
