/*
 * uart_vot.h
 *
 *  Created on: 2023年10月17日
 *      Author: Administrator
 */

#ifndef SRC_UART_UART_VOT_H_
#define SRC_UART_UART_VOT_H_
#include "../myheader.h"

#define N25Q032A_VOT_RES_DATA_ADDRESS 	0x4B4000
#define VOT_RESERVED_DATA_LENGTH	 	51
#define UPDATE_FIRMWARE_LENGTH     1280

enum VOT_COMMAND_STATUS
{
	ReceivedSuccess,
	UnframedHeader,
	CalibrationFailure,
	CommandNotSupported,
	Vot_Command_Status_Sum
};


typedef struct _COM_BUFF
{
	unsigned char buff[DATA_MAX_NUM];
	u16 count;
	u8 start_flag;
	u8 time;
//	unsigned char end_flag;

}COM_data;

extern  COM_data com_data;

typedef struct _SEND_BUFF
{
	unsigned char rx_buff[SEND_DATA_MAX_NUM];

}SEND_data;

extern  SEND_data send_data;

struct VOT_TOOLS
{
	u8 STATUS_FLAG;
	u16 DATA_LENGTH;
	u16 COMMAND_LENGTH;
	u16 RETURN_LENGTH;
//	u16 RESIDUAL_LENGTH;
	u16 HEAD_FLAG;
	u16 END_FLAG;
	u8 BIGDATA_FALG;
	u16 BIGDATA_TIME;
};
extern struct VOT_TOOLS VotTools;
extern struct VOT_TOOLS VotFPGAData;

struct VOT_Parameters
{
	u32 Update_Firmware_Len;
	u8 Version_Falg;
	u8 Test_Flag;
	u8 Execute_Flag;
};
extern struct VOT_Parameters VotParameters;

struct VOT_Arrays
{
	u8  Merchine_SN[14];
	u8  Lens_SN[10];
	u8  Produce_DATA[10];
	u8 	Rcvbuf[2];
};

extern u8 MaxTempAlarmFunSW;
extern u32 MaxTempLimit;
extern u32 OverPixNum;
extern u8 Pulse_Sig;

extern struct VOT_Arrays VotArrays;
extern u8 BADCorrectFlag;
extern u8 TShutFun;

typedef struct
{
	uint32_t    u32FrameLen;		//本项参数长度
	uint32_t    u32FrameOffset;		//参数缓存偏移
	uint16_t    u16FrameLen;		//payload报文长度
	uint16_t    u16FrameNum;		//总分帧数
	uint16_t    u16FrameIndex;		//帧序号
	uint16_t    u16VerifyCheck;		//payload校验
	uint8_t     u8OperStep;			//操作的参数
	uint8_t     u8RetryTime;		//重发次数
	//uint8_t     u8TailBuf_ARY[4U];	//尾巴报文缓存
	uint8_t     u8ParaBuf_ARY[1200U];	//出于要重传的考虑，把这个缓存做成全局变量
	ts_u32      m_core_data_buff ;		// 3M缓存
	uint32_t	File_size;		// 已导出数据大小
	uint32_t	Import_size;		// 已导入数据大小
}ParaMsgSwap_S;
extern ParaMsgSwap_S   g_stParaMsgSwap;

/*****************VOT_UART********************************/
void Data_Packets_Receive_And_Process(void);
//void Valid_Data_Handle(u8 *Valid_data);
void ASIC_Valid_Data_Handle(u8 *Valid_data);
void Asic_Update_Comm_Data(ts_u08* msg_ptr, ts_u16 msg_len);
void ASIC_Data_Packets_Receive_And_Process(void);
void Return_Message_Processing(void);
void Return_Message_Processing_with_pipeline_number(ts_u08 pipeline_number);
void MessageReturnState(uint8_t State);
void Data_Verification(const u8 *Array,u32 Len);
void Clear_Processed_Data(void);
void Vot_Tools_Init(void);
void Vot_Parameters_Init(void);
void Data_u16_Limit(u16* Data,u16 Max,u16 Min);
void Data_u8_Limit(u8* Data,u8 Max,u8 Min);
// u8 FPGAVersionTransmit(u8 *senddata);

/*****************Planck640*****************************/
void Get_VOTReservedAreaDataFromUart(u8 *data);
void Write_VOTReservedAreaDataToFlash(u8 *PointData);
void Write_VOTDetectorToFPGA(void);
void VOTDetectorInit(void);
void Reversal_VOTDetectorData(u8 *data);
u8 Change_NdacValue(u8 TempData);
// void AutomaticParameterConfiguration(void);
/*************Temp***************************/
// void ReturnTemptureFotTime(void);
// void ReturnFirePointTem(void);
// extern ts_void temperature_collection_thread(ts_void *pv_data);
// extern ts_s32 start_temperature_collection_thread(void);
// extern ts_s32 stop_temperature_collection_thread(void);

void UART_OPR_Correct_Bad_Pix(u8 *Valid_data); //机芯内部一键校正坏点
void UART_OPR_KEY(u8 *Valid_data);  //按键传输协议
void UART_OPR_SET_HOST_SW(u8 *Valid_data);//去横纹开关
void UART_OPR_SET_DEST_SW(u8 *Valid_data);//去竖纹开关
void UART_OPR_SET_Gmax(u8 *Valid_data);
void UART_OPR_SET_Palette(u8 *Valid_data);
void UART_OPR_SET_DDE_Level(u8 *Valid_data);
void UART_OPR_SET_Bright(u8 *Valid_data);
void UART_OPR_SET_Contrast(u8 *Valid_data);
void UART_OPR_SET_AutoShutter(u8 *Valid_data);
void UART_OPR_SET_SFilter(u8 *Valid_data);
void UART_OPR_SET_Enlarge(u8 *Valid_data);
void UART_OPR_SET_Enlarge_ShowMes(u8 *Valid_data);
void UART_OPR_SET_TFilter(u8 *Valid_data);
void UART_OPR_Save_Menu1Par(u8 *Valid_data);//用户菜单数据保存
void UART_OPR_Set_Read_SmartData(u8 *Valid_data);
void UART_OPR_Set_Write_SmartData(u8 *Valid_data);
void UART_OPR_Set_AutoShutterByMin(u8 *Valid_data);
void UART_OPR_Save_AllParameter(u8 *Valid_data);
void UART_OPR_Set_Mirror(u8 *Valid_data);
void UART_OPR_TEST(u8 *Valid_data);
void UART_OPR_GetDeviceInfo(u8 *Valid_data);
void UART_OPR_SET_NUC(u8 *Valid_data);
void UART_OPR_SET_OCC(u8 *Valid_data);
void UART_OPR_Semi_Automatic(u8 *Valid_data);
void UART_OPR_Correct_One_Badpixel(u8 *Valid_data);
void UART_OPR_OneClick(u8 *Valid_data);
void UART_OPR_ReadParMenu1(u8 *Valid_data);
void UART_OPR_Get_Firmware_Version(u8 *Valid_data);
void UART_OPR_OnlineUupgrade(u8 *Valid_data);
void UART_OPR_PC_Correct_Badpixel(u8 *Valid_data);
void UART_OPR_SET_Env_Mode(u8 *Valid_data);
void UART_OPR_Sunlight_Protection(u8 *Valid_data);
void UART_OPR_ShutterControl(u8 *Valid_data);
void UART_OPR_SET_BPC(u8 *Valid_data);
void Menu_Two_Protocol(u8 *databuff);
void UART_OPR_AllData_Backup(u8 *Valid_data);
void UART_OPR_ImportJsonParameters(u8 *Valid_data);
void UART_OPR_ExportJsonParameters(u8 *Valid_data);
void UART_OPR_AutoFocus(u8 *Valid_data);
void UART_OPR_FollowFocus(u8 *Valid_data);
ts_u08 ParseJsonParameters(const char* json_str);//解析JSON字符串，提取参数值并进行相应处理
extern ts_void create_cursor_display(ts_void);
/* 增加 */
void UART_OPR_TemCalibration_T_Set(u8 *Valid_data);
void UART_OPR_TemCalibration_AD_Set(u8 *Valid_data);
void UART_OPR_TemCalibration_Read(u8 *Valid_data);
void UART_OPR_Temp_correct_Set(u8 *Valid_data);
void UART_OPR_Temp_correct_Read(u8 *Valid_data);
void UART_OPR_Set_TemptestCollect(u8 *Valid_data);
void UART_OPR_Temp_Meas_Range(u8 *Valid_data);
void UART_OPR_ReadPointTemp_Multi(u8 *Valid_data);
void UART_OPR_ReadAreaTemp_Multi(u8 *Valid_data);
void UART_OPR_ReadCaptureTemp_Multi(u8 *Valid_data);
void UART_OPR_Temp_Drift(u8 *Valid_data);
void UART_OPR_TemplutCollect_Data(u8 *Valid_data) ;
void UART_OPR_Set_ISP_AGC02_CFG(u8 *Valid_data) ;
void UART_OPR_UserTempFrame_Mode(u8 *Valid_data) ;
void UART_OPR_Set_TempGear_MaxLimit(u8 *Valid_data);
void UART_OPR_ReadWriteTempParam(u8 *Valid_data);
void UART_OPR_TEMP_AD_TO_RAW_AD(u8 *Valid_data);
void UART_OPR_OutputFormat_Ctrl(u8 *Valid_data);
void UART_OPR_TempCalib_Second_Function(u8 *Valid_data);
#endif /* SRC_UART_UART_VOT_H_ */
