/*
 * TempCal.c
 *
 *  Created on: 2023年11月14日
 *      Author: WU
 */
#include "../myheader.h"
#include "sffs_files_vot.h"
#include "sffs_files.h"
#include <uapi/isp_comm.h>
#include <uapi/sfc_v100.h>
#include <planck640c_vot.h>
#include <snr_comm_vot.h>
#include <menu/ispfunction.h>
#include <fontlib_vot.h>
#include <menu/ispfunction.h>
#include <snr_comm.h>
#include <libupload.h>
#include <menu/usermenufunction.h>
#include "./common/DistanceCoeffs.h"

#define CAPTUREBOUNDARY		2		/* 测温中捕获的边界限制 */
unsigned char TempParameterBuff[TEMP_LENGTH] = {0};

extern SysCtrlInfo_s_t global_sys_ctrl_info;
extern ts_u32 det_gos_switch_force_last;
struct _Temp_Parameter	Temp_Parameter = {
	.AD_data1 = {0x801,0x956,0xAAB,0xC00,0x1000,0x11BE,0x17CF,0x1F5E,0x2F26,0x324E,0x3576,0x389E},
	.AD_data2 = {0x760,0xB90,0xC30,0xD80,0x1000,0x112C,0x1258,0x1380,0x17B0,0x1C80,0x2050,0x28C0},
	.AD_data3 = {0x760,0xB90,0xC30,0xD80,0x1000,0x112C,0x1258,0x1380,0x17B0,0x1C80,0x2050,0x28C0},
	.TEMP_data1 = {-30,-20,-10,0,30, 40,70,100,150,160,170,180}, //第一档温度段
	.TEMP_data2 = {-30,-20,-10,0,30, 70,140,300,600,650,700,750}, //第二档温度段
	.TEMP_data3 = {-30,-20,-10,0,30, 70,600,1000,1500,1550,1600,1650}, //第三档温度段
	.Second_AD1 = {0x801,0x956,0xAAB,0xC00,0x1000,0x11BE,0x17CF,0x1F5E,0x2F26,0x389E,0x3576,0x3890},
	.Second_AD2 = {0x760,0xB90,0xC30,0xD80,0x1000,0x112C,0x1258,0x1380,0x17B0,0x1C80,0x2050,0x28C0},
	.Second_TEMP1 = {-30,-20,-10,0,30, 40,70,100,150,160,170,180}, //第一档温度段
	.Second_TEMP2 = {-30,-20,-10,0,30, 70,140,300,600,650,700,750}, //第二档温度段
};

struct _Second_Calibration_Parameter SecondCalib_Parameter;
uint16_t AD_data[12]={0};

uint16_t K_value[11]={0};

int16_t  TEMP_data[12]={0};


uint16_t in_tempAD;                    // in_tempAD 计算返回的AD值，
int16_t SensorTemp;                  //SensorTemp 最后经过各种校准的温度值
int16_t SensorTempC1 = 0;           //全局变量温度补偿
int16_t Temp_correct;                //菜单界面进行温度校正使用

int16_t Sensor7Temp;              //传感器的实测温度值
int16_t SensorAD = 0;           //全局变量AD补偿

uint16_t AD_correct;

int16_t test_temp_AD = 0x0FF8;  //0x0E78
int16_t  spot_temp_int;            //spot的温度部分   个位是小数
uint16_t spot_point;							 //spot的温度小数部分
int16_t  capture_temp_int;         //capture的最大值温度部分   个位是小数
int16_t capture_temp_min;         //capture的最小值温度部分
/******************************点和区域****************************/
int16_t  spot_temp1,spot_temp2,spot_temp3,spot_temp4,spot_temp5,spot_temp6,spot_temp7,spot_temp8,spot_temp9;
int16_t	Temp0_max,Temp0_min,Temp0_avg,Temp1_max,Temp1_min,Temp1_avg,Temp2_max,Temp2_min,Temp2_avg,Temp3_max,Temp3_min,Temp3_avg;
uint16_t spot_AD1,spot_AD2,spot_AD3,spot_AD4,spot_AD5,spot_AD6,spot_AD7,spot_AD8,spot_AD9;
u8 LineOffestFlag = 0;
/************************不同溫度环境不同温度点进行修正*************************/
//int16_t temp_correction_range[6][2]={3,0, -40,40, 40,100, 100,140}; //数组a[0][0] 用来记录环境温度修正的个数 a[1][0]- a[1][1] 记录第一个范围 以后类推
//int8_t range1[17]={0,10,20,30,40,-55,60,70,80,90,100};  //6

//int16_t temp_correction_range[6][2]={0}; //数组a[0][0] 用来记录环境温度修正的个数 a[1][0]- a[1][1] 记录第一个范围 以后类推
//int8_t range1[17]={0};
int8_t range2[17]={0};
int8_t range3[17]={0};
int8_t range4[17]={0};
int8_t range5[17]={0};
/*****************************************************************************/
//uint8_t message0[] = "SensorTemp:   \n";
//uint8_t message11[] = "T1:   \n";
//uint8_t message22[] = "T2:   \n";
//uint8_t message33[] = "in_tempAD:   \n";
/******************************************************************************/
float TiTatm_H4;
float TiEmiss_H4;
float TiEmiss;

// u8 SlaveAddressByte1;
// u8 SlaveAddressByte2;
// u8 SlaveAddressByte3;
// u8 SlaveAddressByte4;

u16 TempXpos = 0;
u16 TempYpos = 0;
u8 PointTraceFun = 0;

/* 设置距离修正系数 */
ts_void vot_Set_DistCorr_Fact(void) {
	ts_u08 x;
	ts_u08 mode = SystemParaMenu2.Usermenu_SW - 1;
	ts_u08 lens = Temp_Parameter.Lens_Set;

    if (lens >= 15) {
		return;
	}
	// vision_log_debug("Temp_Parameter.Lens_Set = 0x%x\n", Temp_Parameter.Lens_Set);
	switch (lens) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 1:
			for (x = 0; x < DIST_CORR_MAX_DIST; x++) {
				if (mode == 0) {
					if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
						global_sys_ctrl_info.g_dist_corr_tbl[0][lens][x].factor = tempMode1_lens_9p1mm_QC[x];	/* 一挡9.1mm镜头 */
					}else{
					global_sys_ctrl_info.g_dist_corr_tbl[0][lens][x].factor = tempMode1_lens_9p1mm[x];	/* 一挡9.1mm镜头 */
					}
				}
				else if (mode == 1) {
					if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
						global_sys_ctrl_info.g_dist_corr_tbl[1][lens][x].factor = tempMode2_lens_9p1mm_QC[x];	/* 二挡9.1mm镜头 高低温 */
					}else{
					global_sys_ctrl_info.g_dist_corr_tbl[1][lens][x].factor = tempMode2_lens_9p1mm[x];	/* 二挡9.1mm镜头 高低温 */
					}
				}
				else {
					/* 暂时使用 二挡9.1mm镜头 高低温 */
					if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
						global_sys_ctrl_info.g_dist_corr_tbl[1][lens][x].factor = tempMode2_lens_9p1mm_QC[x];	/* 二挡9.1mm镜头 高低温 */
					}else{
					global_sys_ctrl_info.g_dist_corr_tbl[2][lens][x].factor = tempMode2_lens_9p1mm[x];
					}
				}
			}
		break;
		default:break;
	}
}
/** @note 读取温度传感器数据 */
void READ_Sensor_Temp(void)
{
	// READ_TMP112_Temp();     //读TMP112温度
	temp_calculate();         //计算温度值

	//temp_convert_display_H(&FPGA_Cross0_Temp,&spot_temp_int);     //spot实际显示的温度值
	// temp_convert_display_H((u32 *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX,&capture_temp_int);     //capture实际显示的最大温度值
	// temp_convert_display_H((u32 *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MIN,&capture_temp_min);     //capture实际显示的最小温度值

}

float READ_TMP112_Temp(void)//读TMP117温度
{
	uint16_t Data_16bit;
	float temp = 0;
	Data_16bit = Device_I2c0_Data_Get(SensorAddr,SlaveAddressByte1);
    // vision_log_debug("TMP112_Data:%#x\n",Data_16bit);
	delay_1ms(2);
	if(Data_16bit == 0x8000)
		return Data_16bit;
	if(!(Data_16bit>>15))
	{
		temp  = (float)(Data_16bit*0.0078125);
	}
	else
	{
		Data_16bit = ~((Data_16bit)-1);
		temp  = 0 -(float)(Data_16bit)*0.0078125;
	}
    // vision_log_debug("TMP112_Temp:%f\n",temp);
	return temp;
}
/** @note 将活跃坐标储存在指定区域内 */
static ts_void SpotInif_to_SaveSpot(void) {
	int mCnt;
	
	Temp_Parameter.SpotDisplay = Spot_state;
	Temp_Parameter.AreaDisplay = Area_state;
	for (mCnt = 0; mCnt < TEMPERATURE_POINTS_NUMBER; mCnt++) {
		Temp_Parameter.SaveSpot[mCnt].spot_X_Save = SpotInit[mCnt].spot_X_Pos;
		Temp_Parameter.SaveSpot[mCnt].spot_Y_Save = SpotInit[mCnt].spot_Y_Pos;
	}
	for (mCnt = 0; mCnt < TEMPERATURE_AREAS_NUMBER; mCnt++) {
		Temp_Parameter.SaveArea[mCnt].Area_Lx = Areainit[mCnt].Area_Lx;
		Temp_Parameter.SaveArea[mCnt].Area_Ly = Areainit[mCnt].Area_Ly;
		Temp_Parameter.SaveArea[mCnt].Area_Rx = Areainit[mCnt].Area_Rx;
		Temp_Parameter.SaveArea[mCnt].Area_Ry = Areainit[mCnt].Area_Ry;
	}
}
ts_void Templut_Menu1_LoadDefault(void) {
	int vx;

	Temp_Parameter.AD_sensor = 0;
	Temp_Parameter.SpotDisplay = 0;
	Temp_Parameter.AreaDisplay = 0;

	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);

	ts_u16 center_x = new_width / 2;
	ts_u16 center_y = new_height / 2;
	/* 更新到原始值 */
	for (vx = 0; vx < TEMPERATURE_POINTS_NUMBER; vx++) {
		SpotInit[vx].spot_X_Pos = center_x + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx%3);
		SpotInit[vx].spot_Y_Pos = center_y + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx/3);
	}
	/* 区域1 */
	for (vx = 0; vx < TEMPERATURE_AREAS_NUMBER; vx++) {
		Areainit[vx].Area_Lx = (center_x - 30) + 40 * vx;
		Areainit[vx].Area_Ly = (center_y + 24);
		Areainit[vx].Area_Rx = (center_x - 30) + 32 + 40 * vx;
		Areainit[vx].Area_Ry = (center_y + 24) + 32;
	}
	if(Spot_state || Area_state || UsermenuParameter.CaptureFlag) {
		Clear_Templut_Information();
	}
}
ts_void Templut_Menu2_LoadDefault(void)
{
	int vx;
	Temp_Parameter.AD_sensor = 0;
	Temp_Parameter.RelHum = 50; //0%
	Temp_Parameter.Emiss = 100; //1*100
	Temp_Parameter.Tamb = 850; //环境温度 (25+60)*10
	Temp_Parameter.Trefl = 850;  //(25+60)*10
	Temp_Parameter.distance = 10; //1m*10

	Temp_Parameter.SpotDisplay = 0;
	Temp_Parameter.AreaDisplay = 0;
	Temp_Parameter.Lens_Set = 1;  //默认是定焦9.1mm镜头

	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);

	ts_u16 center_x = new_width / 2;
	ts_u16 center_y = new_height / 2;
	/* 更新到原始值 */
	for (vx = 0; vx < TEMPERATURE_POINTS_NUMBER; vx++) {
		SpotInit[vx].spot_X_Pos = center_x + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx%3);
		SpotInit[vx].spot_Y_Pos = center_y + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * (vx/3);
	}
	/* 区域1 */
	for (vx = 0; vx < TEMPERATURE_AREAS_NUMBER; vx++) {
		Areainit[vx].Area_Lx = (center_x - 30) + 40 * vx;
		Areainit[vx].Area_Ly = (center_y + 24);
		Areainit[vx].Area_Rx = (center_x - 30) + 32 + 40 * vx;
		Areainit[vx].Area_Ry = (center_y + 24) + 32;
	}
	if(Spot_state || Area_state || UsermenuParameter.CaptureFlag)	{
		Clear_Templut_Information();
	}
}

ts_s08 Write_TempParameter_cfg(struct _Temp_Parameter* Temp_Parameter_cfg)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;

	SpotInif_to_SaveSpot();

	s32_hfile_r = ts_lib_sfc_open(FN_VOT_TEMPPARA_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)Temp_Parameter_cfg, sizeof(struct _Temp_Parameter));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return TS_OK;
	}
	return TS_ERR;
}

ts_s08 Read_TempParameter_cfg( struct _Temp_Parameter *Temp_Parameter_cfg )
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	s32_hfile_r = ts_lib_sfc_open(FN_VOT_TEMPPARA_CFG);
	if (s32_hfile_r > 0) {
		memset(Temp_Parameter_cfg, 0, sizeof(struct _Temp_Parameter));
		
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(Temp_Parameter_cfg), sizeof(struct _Temp_Parameter));
			// vision_log_notice("struct _Temp_Parameter = %d\n", sizeof(struct _Temp_Parameter));
			return TS_OK;
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	return TS_ERR;
}
/** @note 测温功能相关参数初始化 */
ts_void TEMP_Parameters_init(void)
{
	int vx;
    struct _Temp_Parameter  sys_Tempara_cfg={0};

    if(Read_TempParameter_cfg(&sys_Tempara_cfg)==0){
        
        memcpy(&Temp_Parameter,&sys_Tempara_cfg,sizeof(struct _Temp_Parameter));
		
		Spot_state = Temp_Parameter.SpotDisplay;
		Area_state = Temp_Parameter.AreaDisplay;
		for (vx = 0; vx < TEMPERATURE_POINTS_NUMBER; vx++) {
			SpotInit[vx].spot_X_Pos = Temp_Parameter.SaveSpot[vx].spot_X_Save;
			SpotInit[vx].spot_Y_Pos = Temp_Parameter.SaveSpot[vx].spot_Y_Save;
		}
		/* 不建议使用memcpy，避免中间被插入 */
		for (vx = 0; vx < TEMPERATURE_AREAS_NUMBER; vx++) {
			Areainit[vx].Area_Lx = Temp_Parameter.SaveArea[vx].Area_Lx;
			Areainit[vx].Area_Ly = Temp_Parameter.SaveArea[vx].Area_Ly;
			Areainit[vx].Area_Rx = Temp_Parameter.SaveArea[vx].Area_Rx;
			Areainit[vx].Area_Ry = Temp_Parameter.SaveArea[vx].Area_Ry;
		}
		if(Temp_Parameter.Second_CalibFlag > 1)	{Temp_Parameter.Second_CalibFlag = 0;}
    }
    else {
        Templut_Menu2_LoadDefault();	//如果用户没有保存配置，加载用户菜单一的默认排至
		Temp_Parameter.Second_CalibFlag = 0;
		Write_TempParameter_cfg(&Temp_Parameter);	//保存配置
    }
	SecondCalib_Parameter_Init();
}

void Write_TempParameterToFPGA(void)
{

	load_AD2Temp();
	if(Spot_state)	
		select_Spot = &SpotInit[0];
}

void load_AD2Temp()
{
#if 0
	uint8_t i;
	int16_t t1,temp1,temp2;
	uint16_t t2;
	volatile uint32_t *ptr;
	ptr = (volatile uint32_t *)ADTTAB;

	if(SystemParaMenu2.Usermenu_SW == 1)  //一档
	{
		 for(i=0;i<12;i++)
		 {
			TEMP_data[i] = TEMP_data1[i];
			 if(i==0) 				{AD_data[i]=0x1000;}
			 else if(i==11)		{AD_data[i]=0xFFFF;}
			 else						{AD_data[i] = AD_data1[i];}
		 }
	}
	else if(SystemParaMenu2.Usermenu_SW == 2) //二档
	{
		 for(i=0;i<12;i++)
		 {
			TEMP_data[i] = TEMP_data2[i];
			//			AD_data[i] = AD_data2[i];
			if(i==0) 				{AD_data[i]=0x1000;}
			else if(i==11)		{AD_data[i]=0xFFFF;}
			else						{AD_data[i] = AD_data2[i];}
		 }
	}
	else if(SystemParaMenu2.Usermenu_SW == 3) //三档
	{
		 for(i=0;i<12;i++)
		 {
			TEMP_data[i] = TEMP_data3[i];
			if(i==0) 				{AD_data[i]=0x1000;}
			else if(i==11)		{AD_data[i]=0xFFFF;}
			else						{AD_data[i] = AD_data3[i];}
		 }
	}
	 for(i=0;i<11;i++)
	 {
		 temp1 = TEMP_data[i];
		 temp2 = TEMP_data[i+1];

		 if(temp2 & (1<<15))
		 {
			temp1	-=1;
			temp1 = ~temp1;
			temp2	-=1;
			temp2 = ~temp2;
			t1 = temp1 - temp2;
		 }
		 else
		 {
			t1 = temp2 - temp1;
		 }
//			rx_buff[0] = t1>>8;
//			rx_buff[1] = t1>>0;
		t2 =AD_data[i+1]-AD_data[i];
		K_value[i] = (uint16_t)(t1*16384/t2);// 2的14次
//			rx_buff[0] = K_value[i]>>8;
//			rx_buff[1] = K_value[i]>>0;
//      MSS_UART_polled_tx(&g_mss_uart1,rx_buff, sizeof(rx_buff));

		*(ptr+i) = AD_data[i];
		*(ptr+i+1) = AD_data[i+1];
		*(ptr+i+12) = TEMP_data[i] + 40;
		*(ptr+i+12+1) = TEMP_data[i+1] + 40;
		*(ptr+i+24) = K_value[i];

	 }
#endif
}

//输入in_tempAD，输出 in_tempAD
void  AD_calculate(void)
{
	uint8_t	i;
	uint16_t AD30_Data=0;       //

	int16_t t2=0;
	int16_t t0=0;
	int16_t t1=0;

	uint16_t temp1=0;
	uint16_t temp2=0;
	float mfloat1=0.0f;
	float mfloat2=0.0f;
	

//	Data_16bit = SensorTemp & 0xffff;
//	//Data_16bit = 0x140;
//	t2 = Data_16bit;
//
//	mfloat1=(float)t2;
////		mfloat1=SensorTemp;
//	mfloat1=mfloat1/10;
//	t0 = (int32_t)mfloat1;	//修正后的环境温度  暂时留着   t0 = t2

	t0 = SensorTemp;

	if(SystemParaMenu2.Usermenu_SW == 1)  //一档
	{
		if(Temp_Parameter.Second_CalibFlag == 1){
			AD30_Data = Temp_Parameter.Second_AD1[4];  //AD30的AD值
			for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
			{
				t1    = Temp_Parameter.Second_TEMP1[i] *10;        //温度表
				temp1 = Temp_Parameter.Second_AD1[i];         //AD表
				i++;
				t2    = Temp_Parameter.Second_TEMP1[i] *10;
				temp2 = Temp_Parameter.Second_AD1[i];
				if((t0>=t1)&&(t0<=t2))
				{
					i=88;
				}
			}
		}else{
			AD30_Data = Temp_Parameter.AD_data1[4];  //AD30的AD值
			for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
			{
				t1    = Temp_Parameter.TEMP_data1[i] *10;        //温度表
				temp1 = Temp_Parameter.AD_data1[i];         //AD表
				i++;
				t2    = Temp_Parameter.TEMP_data1[i] *10;
				temp2 = Temp_Parameter.AD_data1[i];
				if((t0>=t1)&&(t0<=t2))
				{
					i=88;
				}
			}
		}
	}
	else if(SystemParaMenu2.Usermenu_SW == 2) //二档
	{
		if(Temp_Parameter.Second_CalibFlag == 1){
			AD30_Data = Temp_Parameter.Second_AD2[4];  //AD30的AD值
			for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
			{
				t1    = Temp_Parameter.Second_TEMP2[i] *10;        //温度表
				temp1 = Temp_Parameter.Second_AD2[i];         //AD表
				i++;
				t2    = Temp_Parameter.Second_TEMP2[i] *10;
				temp2 = Temp_Parameter.Second_AD2[i];
				if((t0>=t1)&&(t0<=t2))
				{
					i=88;
				}
			}
		}else{
			AD30_Data = Temp_Parameter.AD_data2[4];  //AD30的AD值
			for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
			{
				t1    = Temp_Parameter.TEMP_data2[i] *10;        //温度表
				temp1 = Temp_Parameter.AD_data2[i];         //AD表
				i++;
				t2    = Temp_Parameter.TEMP_data2[i] *10;
				temp2 = Temp_Parameter.AD_data2[i];
				if((t0>=t1)&&(t0<=t2))
				{
					i=88;
				}
			}
		}
	}
	else if(SystemParaMenu2.Usermenu_SW == 3)
	{
		AD30_Data = Temp_Parameter.AD_data3[4];  //AD30的AD值
		for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		{
			t1    = Temp_Parameter.TEMP_data3[i] *10;        //温度表
			temp1 = Temp_Parameter.AD_data3[i];         //AD表
			i++;
			t2    = Temp_Parameter.TEMP_data3[i] *10;
			temp2 = Temp_Parameter.AD_data3[i];
			if((t0>=t1)&&(t0<=t2))
			{
				i=88;
			}
		}
	}

	temp2=temp2-temp1;						   //ad2-ad1
	mfloat1=(float)temp2;
	temp2=t0-t1;      							 //t-t1
	mfloat2=(float)temp2;
	mfloat1=mfloat2*mfloat1;         //(t-t1)*(ad2-ad1)

	temp2=t2-t1;   		               //t2-t1
	mfloat2=(float)temp2;
	mfloat2=mfloat1/mfloat2;        //(t-t1)*((ad2-ad1)/(t2-t1))

	mfloat1=(float)temp1;
	mfloat2=mfloat2+mfloat1;
//	mfloat2=mfloat2-32768;          //ADamb-8000H    dAD
	in_tempAD = (uint16_t)mfloat2;
//	AD_correct = in_tempAD;

	if(in_tempAD < AD30_Data)  //in_tempAD < AD_data[4]  减去AD30的8000H
	{
		in_tempAD = AD30_Data - in_tempAD;
		in_tempAD |= 1<<15;
	}
	else
	{
		in_tempAD -= AD30_Data;
	}

	Temp_Parameter.AD_sensor = in_tempAD;
	// APB3_REGW_AD_Ssensor_Set(Temp_Parameter.AD_sensor);


}

void temp_calculate(void)    //AD补偿修正公式
{
	// uint16_t Temp,Temp_int;
	// uint8_t	Temp_point;
	int16_t Data_16bit;
	float  DeltaTemp,temp;
/*
		Temp = TMP112_Read2ByteFromSlave_IIC(SlaveAddressByte1,0x00,SlaveAddressByte2);
//		Temp = 0xF3F0; //0C90
	if(Temp>>15)  //负
		{
			Temp_int = Temp & 0xFF80;
			Temp_int = Temp_int-1;
			Temp_int = (~Temp_int) & 0x7F80; //保留整数部分，第8-15位，最高位符号位 0xxx xxxx x000 0000

			Temp_point = Temp;
			Temp_point = Temp_point-1;
			Temp_point = (~Temp_point) & 0x7F; //保留7位小数部分 0000 0000 0xxx xxxx

			Data_16bit = Temp_int|Temp_point;    //整数+小数
			Data_16bit =(Data_16bit * 78125)/1000000;
			Data_16bit = 0 - (Data_16bit &~(1<<15));
		}
		else  //正
		{
			Data_16bit =(Temp * 78125)/1000000;
	//	Data_16bit =Data_16bit &0x7FFF;
		}
*/
	temp = READ_TMP112_Temp();
	Data_16bit = (int16_t)(temp*10);
	SensorTemp = Data_16bit;
//	SensorTemp = 0x012C;

#ifdef 	SUPPORT_VOT_DETECTOR
	if(SystemParaMenu2.Usermenu_SW==1)  //一档测温
	{

		DeltaTemp = SensorTemp/10;

		if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_HAND384)
		{
			if(DeltaTemp<= 0.6) //新版手持机384规律-A版机芯260912
			{
				SensorTemp=SensorTemp -70;
			}
			else if((DeltaTemp> 0.6)&&(DeltaTemp<=5))
			{
				DeltaTemp=DeltaTemp-0.6;
				DeltaTemp=DeltaTemp*0.5/4.4;
				DeltaTemp=-7+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>5)&&(DeltaTemp<=25))
			{
				DeltaTemp=DeltaTemp-5;
				DeltaTemp=DeltaTemp*2/20;
				DeltaTemp=-6.5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>25)&&(DeltaTemp<=30))
			{
				DeltaTemp=DeltaTemp-25;
				DeltaTemp=DeltaTemp*2.2/5;
				DeltaTemp=-4.5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>30)&&(DeltaTemp<=37))
			{
				DeltaTemp=DeltaTemp-30;
				DeltaTemp=DeltaTemp*1.5/7;
				DeltaTemp=-2.3+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>37)&&(DeltaTemp<=39))
			{
				DeltaTemp=DeltaTemp-37;
				DeltaTemp=DeltaTemp*0.8/2;
				DeltaTemp=-0.8+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>39)&&(DeltaTemp<=46))
			{
				SensorTemp=SensorTemp +0;
			}
			else if((DeltaTemp>46)&&(DeltaTemp<=55))
			{
				DeltaTemp=DeltaTemp-46;
				DeltaTemp=DeltaTemp*0.5/9;
				DeltaTemp=0+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>55)&&(DeltaTemp<=57))
			{
				DeltaTemp=DeltaTemp-55;
				DeltaTemp=DeltaTemp*1.5/2;
				DeltaTemp=0.5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if(DeltaTemp>57)
			{
				SensorTemp=SensorTemp + 20;
			}
		}else if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
			if(DeltaTemp<= -15) //最新版规律-A版(千从项目？)
			{
//				SensorTemp=SensorTemp -44;
				SensorTemp=SensorTemp -70;
			}
			else if((DeltaTemp> -15)&&(DeltaTemp<=0.6))
			{
				DeltaTemp=DeltaTemp+15;
//				DeltaTemp=DeltaTemp*0.7/15;
//				DeltaTemp=-4.4+DeltaTemp;
				DeltaTemp=DeltaTemp*2/15.6;
				DeltaTemp=-7+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp> 0.6)&&(DeltaTemp<=5))
			{
				SensorTemp=SensorTemp -50;
			}
			else if((DeltaTemp>5)&&(DeltaTemp<=25))
			{
				DeltaTemp=DeltaTemp-5;
				DeltaTemp=DeltaTemp*5/20;
				DeltaTemp=-5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>25)&&(DeltaTemp<=46))
			{
				DeltaTemp=DeltaTemp-25;
				DeltaTemp=DeltaTemp*2/21;
				DeltaTemp=0+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;				
			}
			else if((DeltaTemp>46)&&(DeltaTemp<=50))
			{
				DeltaTemp=DeltaTemp-46;
				DeltaTemp=DeltaTemp*2.5/4;
				DeltaTemp=2+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if(DeltaTemp>50)
			{

				SensorTemp=SensorTemp+45;
			}

		}else{   //通用版
			if(DeltaTemp<= -15) //最新版规律-A版机芯260320
			{
//				SensorTemp=SensorTemp -44;
				SensorTemp=SensorTemp -70;
			}
			else if((DeltaTemp> -15)&&(DeltaTemp<=0.6))
			{
				DeltaTemp=DeltaTemp+15;
//				DeltaTemp=DeltaTemp*0.7/15;
//				DeltaTemp=-4.4+DeltaTemp;
				DeltaTemp=DeltaTemp*2/15.6;
				DeltaTemp=-7+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp> 0.6)&&(DeltaTemp<=5))
			{
				SensorTemp=SensorTemp -50;
			}
			else if((DeltaTemp>5)&&(DeltaTemp<=25))
			{
				DeltaTemp=DeltaTemp-5;
				DeltaTemp=DeltaTemp*5/20;
				DeltaTemp=-5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>25)&&(DeltaTemp<=46))
			{
				SensorTemp=SensorTemp+0;
			}
			else if((DeltaTemp>46)&&(DeltaTemp<=60))
			{
				DeltaTemp=DeltaTemp-46;
				DeltaTemp=DeltaTemp*1/14;
				DeltaTemp=0+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>60)&&(DeltaTemp<=65))
			{
				DeltaTemp=DeltaTemp-60;
				DeltaTemp=DeltaTemp*1/5;
				DeltaTemp=1+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if(DeltaTemp>65)
			{
				SensorTemp=SensorTemp +20;
			}
		}
		// else  //其余镜头规律
		// {
		// 	if(DeltaTemp<= -10) //最新版规律-1113
		// 	{
		// 		SensorTemp=SensorTemp -8;
		// 	}
		// 	else if((DeltaTemp> -10)&&(DeltaTemp<=5))
		// 	{
		// 		DeltaTemp=DeltaTemp+10;
		// 		DeltaTemp=DeltaTemp*1.2/15;
		// 		DeltaTemp=-0.8-DeltaTemp;
		// 		DeltaTemp=10*DeltaTemp;
		// 		SensorTemp=SensorTemp+(int16_t)DeltaTemp;
		// 	}
		// 	else if((DeltaTemp> 5)&&(DeltaTemp<=10))
		// 	{
		// 		SensorTemp=SensorTemp -20;
		// 	}
		// 	else if((DeltaTemp>10)&&(DeltaTemp<=35))
		// 	{
		// 		DeltaTemp=DeltaTemp-10;
		// 		DeltaTemp=DeltaTemp*2/25;
		// 		DeltaTemp=-2+DeltaTemp;
		// 		DeltaTemp=10*DeltaTemp;
		// 		SensorTemp=SensorTemp+(int16_t)DeltaTemp;
		// 	}
		// 	else if((DeltaTemp>35)&&(DeltaTemp<=45))
		// 	{
		// 		SensorTemp=SensorTemp+0;
		// 	}
		// 	else if((DeltaTemp>45)&&(DeltaTemp<=65))
		// 	{
		// 		DeltaTemp=DeltaTemp-45;
		// 		DeltaTemp=DeltaTemp*2/20;
		// 		DeltaTemp=0+DeltaTemp;
		// 		DeltaTemp=10*DeltaTemp;
		// 		SensorTemp=SensorTemp+(int16_t)DeltaTemp;
		// 	}
		// 	else if(DeltaTemp>65)
		// 	{
		// 		SensorTemp=SensorTemp +20;
		// 	}
		// }

		Temp_correct = Temp_Parameter.Temp_correct1;
	}
	else if(SystemParaMenu2.Usermenu_SW==2)
	{
		DeltaTemp = SensorTemp/10;
		// if(Temp_Parameter.Lens_Set==5)  //25mm镜头
		{
			if(DeltaTemp<= -15) //最新版规律-A版机芯260320
			{
				SensorTemp=SensorTemp +120;
			}
			else if((DeltaTemp> -15)&&(DeltaTemp<=4.5))
			{
				DeltaTemp=DeltaTemp+15;
				DeltaTemp=DeltaTemp*(-12)/19.5;
				DeltaTemp=12+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}

			else if((DeltaTemp> 4.5)&&(DeltaTemp<=60))
			{
				SensorTemp=SensorTemp+0;
			}
			else if(DeltaTemp> 60)
			{
				SensorTemp=SensorTemp+40;
			}
		}
		Temp_correct = Temp_Parameter.Temp_correct2;
	}
	else if(SystemParaMenu2.Usermenu_SW==3)
	{
		Temp_correct = Temp_Parameter.Temp_correct3;
	}
#else  //IRay艾睿测温
	if(SystemParaMenu2.Usermenu_SW==1)  //一档测温
	{
			DeltaTemp = SensorTemp/10;
			/*	if(DeltaTemp<=-11)
			{
				SensorTemp = SensorTemp -50;
			}
			else if((DeltaTemp>-11)&&(DeltaTemp<=7))
			{
				DeltaTemp=DeltaTemp+11;
				DeltaTemp=DeltaTemp*1/18;
				DeltaTemp=-5+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}*/
			if(DeltaTemp<=7)
			{
				SensorTemp = SensorTemp -50;
			}
			else if((DeltaTemp>7)&&(DeltaTemp<=33))
			{
				DeltaTemp=DeltaTemp-7;
				DeltaTemp=DeltaTemp*4/26;
				DeltaTemp=-5 +DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>33)&&(DeltaTemp<=35))
			{
				DeltaTemp=DeltaTemp-33;
				DeltaTemp=DeltaTemp*1/2;
				DeltaTemp=-1+DeltaTemp;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if((DeltaTemp>35)&&(DeltaTemp<=45))  //标定状态区间，不需要进行补偿
			{
				SensorTemp = SensorTemp+0;
			}
			else if((DeltaTemp>45)&&(DeltaTemp<=66))
			{
				DeltaTemp=DeltaTemp-45;
				DeltaTemp=DeltaTemp*2/21;
				DeltaTemp=10*DeltaTemp;
				SensorTemp=SensorTemp+(int16_t)DeltaTemp;
			}
			else if(DeltaTemp>66)
			{
				SensorTemp=SensorTemp+20;
			}

		Temp_correct = Temp_Parameter.Temp_correct1;
	}
	else if(SystemParaMenu2.Usermenu_SW==2) //二档测温
	{
		DeltaTemp = SensorTemp/10;
		if(DeltaTemp<=-11)
		{
			SensorTemp = SensorTemp +360;
		}
		else if((DeltaTemp>-11)&&(DeltaTemp<=10))
		{
			DeltaTemp=DeltaTemp+11;
			DeltaTemp=DeltaTemp*29.5/21;
			DeltaTemp=36-DeltaTemp;
			DeltaTemp=10*DeltaTemp;
			SensorTemp=SensorTemp+(int16_t)DeltaTemp;
		}

		else if((DeltaTemp>10)&&(DeltaTemp<=30))
		{
			DeltaTemp=DeltaTemp-10;
			DeltaTemp=DeltaTemp*6.5/20;
			DeltaTemp= 6.5 -DeltaTemp;
			DeltaTemp=10*DeltaTemp;
			SensorTemp=SensorTemp+(int16_t)DeltaTemp;
		}
		else if(DeltaTemp>30)
		{
			SensorTemp = SensorTemp+0;
		}

		Temp_correct = Temp_Parameter.Temp_correct2;

	}
	else if(SystemParaMenu2.Usermenu_SW==3)  //三档测温
	{

		Temp_correct = Temp_Parameter.Temp_correct3;
	}
#endif

	if(SystemParaMenu2.TemTest_SW<2)	SensorTemp = Data_16bit+Temp_correct; //不做Sensor修正
	else{
		if(Temp_Parameter.Second_CalibFlag == 1){		
			if(SystemParaMenu2.Usermenu_SW==1)			Temp_correct = Temp_Parameter.Second_correct1;
			else if(SystemParaMenu2.Usermenu_SW==2)		Temp_correct = Temp_Parameter.Second_correct2;
		}
		SensorTemp = SensorTemp + Temp_correct ;      //获取最终的计算温度值 = sensor+输入偏移值+不同环境下温度补偿表
	}
	AD_calculate();

}

ts_f32 LookUP_ADtoTemp(uint16_t ADVal,uint8_t TemLevel)  //AD转温度  TemLevel用来选择不同的温度档位
{
	uint8_t	i;
	int16_t temp1=0,temp2=0;
	uint16_t ad1=0,ad2=0;
	// float Temp=0.0f;
	// float mfloat1=0.0f;
	// float mfloat2=0.0f;

	if(TemLevel == 1)  //一档
	{
		 for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		 {
			if(Temp_Parameter.Second_CalibFlag == 1){   //二次标定
				temp1 = Temp_Parameter.Second_TEMP1[i];        //温度表
				ad1    = Temp_Parameter.Second_AD1[i];         //AD表
				i++;
				temp2 = Temp_Parameter.Second_TEMP1[i];
				ad2    = Temp_Parameter.Second_AD1[i];
			}else{
			temp1 = Temp_Parameter.TEMP_data1[i];        //温度表
			ad1    = Temp_Parameter.AD_data1[i];         //AD表
			i++;
			temp2 = Temp_Parameter.TEMP_data1[i];
			ad2    = Temp_Parameter.AD_data1[i];
			}
			if((ADVal>=ad1)&&(ADVal<=ad2))
			{
				i=88;
			}
		}
	}
	else if(TemLevel == 2) //二档
	{
		 for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		 {
			if(Temp_Parameter.Second_CalibFlag == 1){ //二次标定
				temp1 = Temp_Parameter.Second_TEMP2[i];        //温度表
				ad1    = Temp_Parameter.Second_AD2[i];         //AD表
				i++;
				temp2 = Temp_Parameter.Second_TEMP2[i];
				ad2    = Temp_Parameter.Second_AD2[i];
			}else{
			temp1 = Temp_Parameter.TEMP_data2[i];        //温度表
			ad1    = Temp_Parameter.AD_data2[i];         //AD表
			i++;
			temp2 = Temp_Parameter.TEMP_data2[i];
			ad2    = Temp_Parameter.AD_data2[i];
			}
			if((ADVal>=ad1)&&(ADVal<=ad2))
			{
				i=88;
			}
		}
	}
	else if(TemLevel == 3)
	{
		 for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		 {
			temp1 = Temp_Parameter.TEMP_data3[i];        //温度表
			ad1    = Temp_Parameter.AD_data3[i];         //AD表
			i++;
			temp2 = Temp_Parameter.TEMP_data3[i];
			ad2    = Temp_Parameter.AD_data3[i];
			if((ADVal>=ad1)&&(ADVal<=ad2))
			{
				i=88;
			}
		}
	}
/*(ad - ad1)/(t-t1) = (ad2-da1)/(t2-t1) */
	ad2=ad2-ad1;						//AD表中两点之间的差值 AD2-AD1
	if(ad2 == 0) {
		vision_log_debug("Error: ad1=ad2 in LookUP_ADtoTemp function. \n");
		return -40.0f;
	}
	temp2=temp2-temp1;				   //温度表中两点之间的差值 T2-T1
	float mfloat1=(float)temp2;
	float mfloat2= (float)ad2;

	float Temp = (float)(ADVal-ad1)*mfloat1/mfloat2;	//(AD-AD1)*((T2-T1)/(AD2-AD1)) 据AD值和温度表进行线性插值计算温度值
	Temp = (float)temp1 + Temp;					//加上温度表中较小T1的温度值

	return Temp;
}

void SecondCalib_Cal(uint8_t TemLevel)  //二次校准计算
{
	float mfloat1;
	float mfloat2;
	float mfloat3;
	if(Temp_Parameter.Second_CalibFlag == 0){  //二次校准未开启
		return;}

	if(TemLevel == 1)  //一档
	{
		//计算AD40
		mfloat1 = (float)(Temp_Parameter.AD_data1[5] - Temp_Parameter.AD_data1[4]) / (float)(Temp_Parameter.TEMP_data1[5] - Temp_Parameter.TEMP_data1[4]);  //原AD30、40的k值
		mfloat2 = (float)(Temp_Parameter.AD_data1[6] - Temp_Parameter.AD_data1[5]) / (float)(Temp_Parameter.TEMP_data1[6] - Temp_Parameter.TEMP_data1[5]);  //原AD40、70的k值
		mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		Temp_Parameter.Second_AD1[5] = SecondCalib_TempToAD(4,mfloat1,TemLevel);	//根据二次校准斜率计算出新的AD40的AD值	
		vision_log_debug("111Second_Calib AD40 = 0x%04x \n", Temp_Parameter.Second_AD1[5]);
		//计算AD100
		mfloat1 = (float)(Temp_Parameter.AD_data1[7] - Temp_Parameter.AD_data1[6]) / (float)(Temp_Parameter.TEMP_data1[7] - Temp_Parameter.TEMP_data1[6]);  //原AD70、100的k值
		mfloat2 = (float)(Temp_Parameter.AD_data1[8] - Temp_Parameter.AD_data1[7]) / (float)(Temp_Parameter.TEMP_data1[8] - Temp_Parameter.TEMP_data1[7]);  //原AD100、150的k值
		mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		Temp_Parameter.Second_AD1[7] = SecondCalib_TempToAD(6,mfloat1,TemLevel);	//根据二次校准斜率计算出新的AD100的AD值	
		vision_log_debug("111Second_Calib AD100 = 0x%04x \n", Temp_Parameter.Second_AD1[7]);
		//计算AD0
		mfloat1 = (float)(Temp_Parameter.AD_data1[4] - Temp_Parameter.AD_data1[3]) / (float)(Temp_Parameter.TEMP_data1[4] - Temp_Parameter.TEMP_data1[3]);  //原AD0、30的k值
		mfloat2 = (float)(Temp_Parameter.AD_data1[5] - Temp_Parameter.AD_data1[4]) / (float)(Temp_Parameter.TEMP_data1[5] - Temp_Parameter.TEMP_data1[4]);  //原AD30、40的k值
		mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		mfloat3 = (float)(Temp_Parameter.Second_AD1[5] - Temp_Parameter.Second_AD1[4]) / (float)(Temp_Parameter.Second_TEMP1[5] - Temp_Parameter.Second_TEMP1[4]);
		mfloat3 = mfloat3*mfloat1;
		mfloat2 = (float)(Temp_Parameter.Second_TEMP1[4] - Temp_Parameter.Second_TEMP1[3]) *mfloat3;
		Temp_Parameter.Second_AD1[3] = (ts_u16)((float)Temp_Parameter.Second_AD1[4] - mfloat2);	//根据二次校准斜率计算出新的AD0的AD值
		vision_log_debug("111Second_Calib AD0 = 0x%04x \n", Temp_Parameter.Second_AD1[3]);
		//推算-30，-20，-10,160,170,180 对应AD值
		mfloat1 = (float)(Temp_Parameter.Second_AD1[4] - Temp_Parameter.Second_AD1[3]) / (float)(Temp_Parameter.Second_TEMP1[4] - Temp_Parameter.Second_TEMP1[3]); 
		Temp_Parameter.Second_AD1[2] = (ts_u16)((float)Temp_Parameter.Second_AD1[3] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-10)的AD值
		vision_log_debug("111Second_Calib AD-10 = 0x%04x \n", Temp_Parameter.Second_AD1[2]);
		Temp_Parameter.Second_AD1[1] = (ts_u16)((float)Temp_Parameter.Second_AD1[2] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-20)的AD值
		vision_log_debug("111Second_Calib AD-20 = 0x%04x \n", Temp_Parameter.Second_AD1[1]);
		Temp_Parameter.Second_AD1[0] = (ts_u16)((float)Temp_Parameter.Second_AD1[1] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-30)的AD值
		vision_log_debug("111Second_Calib AD-30 = 0x%04x \n", Temp_Parameter.Second_AD1[0]);

		mfloat1 = (float)(Temp_Parameter.Second_AD1[8] - Temp_Parameter.Second_AD1[7]) / (float)(Temp_Parameter.Second_TEMP1[8] - Temp_Parameter.Second_TEMP1[7]); 
		Temp_Parameter.Second_AD1[9] = (ts_u16)((float)Temp_Parameter.Second_AD1[8] + 10*mfloat1);	//根据二次校准斜率计算出新的AD160的AD值
		vision_log_debug("111Second_Calib AD160 = 0x%04x \n", Temp_Parameter.Second_AD1[9]);
		Temp_Parameter.Second_AD1[10] = (ts_u16)((float)Temp_Parameter.Second_AD1[9] + 10*mfloat1);	//根据二次校准斜率计算出新的AD170的AD值
		vision_log_debug("111Second_Calib AD170 = 0x%04x \n", Temp_Parameter.Second_AD1[10]);
		Temp_Parameter.Second_AD1[11] = (ts_u16)((float)Temp_Parameter.Second_AD1[10] + 10*mfloat1);	//根据二次校准斜率计算出新的AD180的AD值
		vision_log_debug("111Second_Calib AD180 = 0x%04x \n", Temp_Parameter.Second_AD1[11]);

	}
	else if(TemLevel == 2) //二档
	{
		//计算AD70
		mfloat1 = (float)(Temp_Parameter.AD_data2[5] - Temp_Parameter.AD_data2[4]) / (float)(Temp_Parameter.TEMP_data2[5] - Temp_Parameter.TEMP_data2[4]);  //原AD30、70的k值
		mfloat2 = (float)(Temp_Parameter.AD_data2[6] - Temp_Parameter.AD_data2[5]) / (float)(Temp_Parameter.TEMP_data2[6] - Temp_Parameter.TEMP_data2[5]);  //原AD70、140的k值
		mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		Temp_Parameter.Second_AD2[5] = SecondCalib_TempToAD(4,mfloat1,TemLevel);	//根据二次校准斜率计算出新的AD70的AD值	
		vision_log_debug("222Second_Calib AD70 = 0x%04x \n", Temp_Parameter.Second_AD2[5]);
		//计算AD600
		Temp_Parameter.Second_AD2[8] = Calib_TempToAD_300to600(6); //根据300度和140度的AD值，计算出600度的AD值
		vision_log_debug("222Second_Calib AD600 = 0x%04x \n", Temp_Parameter.Second_AD2[8]);
		// mfloat1 = (float)(Temp_Parameter.AD_data2[7] - Temp_Parameter.AD_data2[6]) / (float)(Temp_Parameter.TEMP_data2[7] - Temp_Parameter.TEMP_data2[6]);  //原AD140、300的k值
		// mfloat2 = (float)(Temp_Parameter.AD_data2[8] - Temp_Parameter.AD_data2[7]) / (float)(Temp_Parameter.TEMP_data2[8] - Temp_Parameter.TEMP_data2[7]);  //原AD300、600的k值
		// mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		// Temp_Parameter.Second_AD2[7] = SecondCalib_TempToAD(6,mfloat1,TemLevel);	//根据二次校准斜率计算出新的AD300的AD值	
		/*因客户采集的黑体不是600度，而是500度，这里做600度的转换计算*/
		// ts_f32 floatK = (float)(Temp_Parameter.Second_AD2[8] - Temp_Parameter.Second_AD2[7]) / (float)(Temp_Parameter.Second_TEMP2[8] - Temp_Parameter.Second_TEMP2[7]);  //AD300、500的k值
		// Temp_Parameter.Second_AD2[8] = (ts_u16)((float)Temp_Parameter.Second_AD2[7] + (600-300)*floatK);	//重新计算出新的AD600的AD值
		// vision_log_debug("222Second_Calib AD300 = 0x%04x,New AD600 = 0x%04x \n", Temp_Parameter.Second_AD2[7],Temp_Parameter.Second_AD2[8]);
		
		//计算AD0
		mfloat1 = (float)(Temp_Parameter.AD_data2[4] - Temp_Parameter.AD_data2[3]) / (float)(Temp_Parameter.TEMP_data2[4] - Temp_Parameter.TEMP_data2[3]);  //原AD0、30的k值
		mfloat2 = (float)(Temp_Parameter.AD_data2[5] - Temp_Parameter.AD_data2[4]) / (float)(Temp_Parameter.TEMP_data2[5] - Temp_Parameter.TEMP_data2[4]);  //原AD30、70的k值
		mfloat1 = mfloat1/mfloat2;	//二次校准斜率
		mfloat3 = (float)(Temp_Parameter.Second_AD2[5] - Temp_Parameter.Second_AD2[4]) / (float)(Temp_Parameter.Second_TEMP2[5] - Temp_Parameter.Second_TEMP2[4]);
		mfloat3 = mfloat3*mfloat1;
		mfloat2 = (float)(Temp_Parameter.Second_TEMP2[4] - Temp_Parameter.Second_TEMP2[3]) *mfloat3;
		Temp_Parameter.Second_AD2[3] = (ts_u16)((float)Temp_Parameter.Second_AD2[4] - mfloat2);	//根据二次校准斜率计算出新的AD0的AD值
		vision_log_debug("222Second_Calib AD0 = 0x%04x \n", Temp_Parameter.Second_AD2[3]);
		//推算-30，-20，-10,650,700,750 对应AD值
		mfloat1 = (float)(Temp_Parameter.Second_AD2[4] - Temp_Parameter.Second_AD2[3]) / (float)(Temp_Parameter.Second_TEMP2[4] - Temp_Parameter.Second_TEMP2[3]); 
		Temp_Parameter.Second_AD2[2] = (ts_u16)((float)Temp_Parameter.Second_AD2[3] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-10)的AD值
		vision_log_debug("222Second_Calib AD-10 = 0x%04x \n", Temp_Parameter.Second_AD2[2]);
		Temp_Parameter.Second_AD2[1] = (ts_u16)((float)Temp_Parameter.Second_AD2[2] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-20)的AD值
		vision_log_debug("222Second_Calib AD-20 = 0x%04x \n", Temp_Parameter.Second_AD2[1]);
		Temp_Parameter.Second_AD2[0] = (ts_u16)((float)Temp_Parameter.Second_AD2[1] - 10*mfloat1);	//根据二次校准斜率计算出新的AD(-30)的AD值
		vision_log_debug("222Second_Calib AD-30 = 0x%04x \n", Temp_Parameter.Second_AD2[0]);

		mfloat1 = (float)(Temp_Parameter.Second_AD2[8] - Temp_Parameter.Second_AD2[7]) / (float)(Temp_Parameter.Second_TEMP2[8] - Temp_Parameter.Second_TEMP2[7]); 
		Temp_Parameter.Second_AD2[9] = (ts_u16)((float)Temp_Parameter.Second_AD2[8] + 50*mfloat1);	//根据二次校准斜率计算出新的AD650的AD值
		vision_log_debug("222Second_Calib AD650 = 0x%04x, Kx =%f \n", Temp_Parameter.Second_AD2[9],mfloat1);
		Temp_Parameter.Second_AD2[10] = (ts_u16)((float)Temp_Parameter.Second_AD2[9] + 50*mfloat1);	//根据二次校准斜率计算出新的AD700的AD值
		vision_log_debug("222Second_Calib AD700 = 0x%04x \n", Temp_Parameter.Second_AD2[10]);
		Temp_Parameter.Second_AD2[11] = (ts_u16)((float)Temp_Parameter.Second_AD2[10] + 50*mfloat1);	//根据二次校准斜率计算出新的AD750的AD值
		vision_log_debug("222Second_Calib AD750 = 0x%04x \n", Temp_Parameter.Second_AD2[11]);
	}
	else if(TemLevel == 3)
	{

	}

}

ts_u16 SecondCalib_TempToAD(ts_u16 StartNum,ts_f32 Kx,uint8_t TemLevel)  //二次校准计算
{
	ts_f32 K1,AD_x;
	ts_u16 ADx = 0;
	int16_t  Sec_TEMP[3] = {0};
	uint16_t Sec_AD[3] = {0};
	/********************
	 * AD1,ADx,AD2  三个AD点
	 * T1,Tx,T2  	三个温度点
	 * Kx = K1/K2  二次校准斜率比例
	 * Kx = {(ADx-AD1)/(Tx-T1)} / {(AD2-ADx)/(T2-Tx)}
	 * Kx*(Tx-T1)/(T2-Tx) = (ADx-AD1)/(AD2-ADx)
	 * K = Kx*(Tx-T1)/(T2-Tx)
	 * k*(AD2-ADx) = (ADx-AD1)
	 * (K*AD2 + AD1) = (1+K)*ADx
	 * ADx = (K*AD2 + AD1)/(1+K)
	********************/
	if(TemLevel == 1)  //一档
	{
		Sec_TEMP[0] = Temp_Parameter.Second_TEMP1[StartNum];
		Sec_TEMP[1] = Temp_Parameter.Second_TEMP1[StartNum+1];
		Sec_TEMP[2] = Temp_Parameter.Second_TEMP1[StartNum+2];
		Sec_AD[0] = Temp_Parameter.Second_AD1[StartNum];
		Sec_AD[1] = Temp_Parameter.Second_AD1[StartNum+1];
		Sec_AD[2] = Temp_Parameter.Second_AD1[StartNum+2];
	}else if(TemLevel == 2) //二档
	{
		Sec_TEMP[0] = Temp_Parameter.Second_TEMP2[StartNum];
		Sec_TEMP[1] = Temp_Parameter.Second_TEMP2[StartNum+1];
		Sec_TEMP[2] = Temp_Parameter.Second_TEMP2[StartNum+2];
		Sec_AD[0] = Temp_Parameter.Second_AD2[StartNum];
		Sec_AD[1] = Temp_Parameter.Second_AD2[StartNum+1];
		Sec_AD[2] = Temp_Parameter.Second_AD2[StartNum+2];
	}
	// K1 = (float)(Temp_Parameter.Second_TEMP1[StartNum+1] - Temp_Parameter.Second_TEMP1[StartNum]) / (float)(Temp_Parameter.Second_TEMP1[StartNum+2] - Temp_Parameter.Second_TEMP1[StartNum+1]);
	K1 = (float)(Sec_TEMP[1] - Sec_TEMP[0]) / (float)(Sec_TEMP[2] - Sec_TEMP[1]);
	vision_log_debug("---K1=[%f] \n", K1);
	K1 = Kx * K1;

	// AD_x = (float)Temp_Parameter.Second_AD1[StartNum+2] * K1 + (float)Temp_Parameter.Second_AD1[StartNum];
	AD_x = (float)Sec_AD[2] * K1 + (float)Sec_AD[0];
	AD_x = AD_x / (1+K1);
	vision_log_debug("---FinalKx=[%f], AD_x=[%f] \n", K1, AD_x);
	ADx = (ts_u16)AD_x;
	
	return ADx;
}

ts_u16 Calib_TempToAD_300to600(ts_u16 StartNum)  //二次校准二档推算600度
{
	//根据140和300度推算600度对应的AD值，
	/******************
	* 原始标定表140和300的K1/二次标定表140和300的K2 =原始标定表300和600的K/二次标定表300和600的x ,斜率之比相等，
	* Kx = K1/K2  二次校准斜率比例
	*Kx = ((AD600-AD300)/(T600-T300)) /((ADx-ad300)/(t600-t300)); //斜率
	*	= K*(t600-t300)/(ADx-ad300)
	*ADx = K*(t600-t300)/Kx + ad300
	*****************/
	ts_u16 retAD = 0;
	ts_f32 K1,K2,Kx;

	K1 = (float)(Temp_Parameter.AD_data2[StartNum+1] - Temp_Parameter.AD_data2[StartNum]) / (float)(Temp_Parameter.TEMP_data2[StartNum+1] - Temp_Parameter.TEMP_data2[StartNum]);
	K2 = (float)(Temp_Parameter.Second_AD2[StartNum+1] - Temp_Parameter.Second_AD2[StartNum]) / (float)(Temp_Parameter.Second_TEMP2[StartNum+1] - Temp_Parameter.Second_TEMP2[StartNum]);
	Kx = K1/K2;
	// vision_log_debug("---Kx=[%f] \n", Kx);
	K1 =  (float)(Temp_Parameter.AD_data2[StartNum+2] - Temp_Parameter.AD_data2[StartNum+1]) / (float)(Temp_Parameter.TEMP_data2[StartNum+2] - Temp_Parameter.TEMP_data2[StartNum+1]);
	K2 = K1/Kx;
	// vision_log_debug("---K1=[%f], K2=[%f] \n", K1,K2);
	ts_f32 ret = (float)(Temp_Parameter.Second_TEMP2[StartNum+2] - Temp_Parameter.Second_TEMP2[StartNum+1])*K2 + (float)Temp_Parameter.Second_AD2[StartNum+1];
	// vision_log_debug("---ret=[%f] \n", ret);
	retAD = (ts_u16)ret;
	return retAD;
}

#if 0		//暂时不用
void temp_display(void)
{
	uint8_t t1= 0;
	int16_t test,int1;
	uint16_t point;
	uint8_t rx_buff[6]={0};

//	test = *APB3_Temp_cross_ctl>>16;         		 //温度整数部分
//	test = test_temp_AD;
	test = 0x8000;
	rx_buff[0] = test>>8;
	rx_buff[1] = test;

	int1 = test>>7;
	rx_buff[2] = int1>>8;
	rx_buff[3] = int1;
	int1 -=40;                                   // 温度在写入之前有加40写入寄存器，实际显示时将40减去
	rx_buff[4] = int1>>8;
	rx_buff[5] = int1;

	t1 = (test & ~(1<<7));		 //温度小数部分

//	temp_int = test_temp_AD>>7;         		 //测试部分  写入31.93°
//	t1 = (test_temp_AD & ~(1<<7));

	t1 = t1>>3;
	point = t1 * 625/1000;             	//小数部分统一乘以10^4 ,取模保留1位小数
}

void temp_convert_display_H(uint32_t * addr,int16_t * temp_int)         //高16位温度值显示   转换的温度整体*10 小数位为个数位
{
	uint8_t t1= 0;
	int16_t test = 0,temp;

	test = *addr>>16;
//	temp = test>>7;             		//温度整数部分
//	t1 = (test & ~(1<<7));		      //温度小数部分
//	t1 = t1>>3;
	temp = test>>4;             		//温度整数部分
	t1 = (test & 0x0f);		      //温度小数部分
	t1 = (t1 * 625)/1000;                  //小数部分统一乘以10^4    //保留1位小数
	if(temp<0)
	{
			*temp_int = temp*10 - t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去  这里温度一般不会低于0  FPGA是+40传送过来的

	}
	else
	{
			*temp_int = temp*10 + t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去
	}
//**************************************************************//
//	bsp_printf("SpotTemp0000= %x\n",*temp_int );
	*temp_int = Temp_AllCorrect(*temp_int); //温度最后显示修正部分
//**************************************************************//
}

void temp_convert_display_L(uint32_t * addr,int16_t * temp_int)         //低16位温度值显示
{
	uint8_t t1= 0;
	int16_t test = 0,temp;

	test = *addr>>0;         		     //温度整数部分
//	temp = test>>7;
//	t1 = (test & ~(1<<7));		       //温度小数部分
//	t1 = t1>>3;
	temp = test>>4;             		//温度整数部分
	t1 = (test & 0x0f);		      //温度小数部分
	t1 = (t1 * 625)/1000;                  //小数部分统一乘以10^4  保留1位小数
	if(temp<0)
	{
			*temp_int = temp*10 - t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去     这里温度一般不会低于0  FPGA是+40传送过来的
	}
	else
	{
			*temp_int = temp*10 + t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去
	}
//**************************************************************//
	*temp_int = Temp_AllCorrect(*temp_int); //温度最后显示修正部分
//**************************************************************//
}
s16  AreaTemp_convert_AVG(uint16_t temp_int)         //区域平均温计算
{
	uint8_t t1= 0;
	int16_t temp;

	temp = temp_int>>4;             		//温度整数部分
	t1 = (temp_int & 0x0f);		      //温度小数部分
	t1 = (t1 * 625)/1000;                  //小数部分统一乘以10^4  保留1位小数
	if(temp<0)
	{
		temp = temp*10 - t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去     这里温度一般不会低于0  FPGA是+40传送过来的
	}
	else
	{
		temp = temp*10 + t1 -400;      // 温度在写入之前有加40写入寄存器，实际显示时将40减去
	}
	temp = Temp_AllCorrect(temp); //温度最后显示修正部分
	return	temp;
}


#if 0
// 公制－》英制 （温度）华氏温标与摄氏温标的关系为F=9×t÷5+32
uint16_t unit_to_english_temp(uint16_t tmp16)
{
	float	mfloat1;
	mfloat1 = (float)tmp16;
	mfloat1 -= 600;
	mfloat1 *= 1.8;
	mfloat1 += 1080.5;	  //920.5
	return (uint16_t)(mfloat1);
}

uint16_t english_to_unit_temp(uint16_t tmp16)
{
	float	mfloat1;
	mfloat1 = (float)(tmp16);
	mfloat1 -= 1080;		//920
	mfloat1 /= 1.8;
	mfloat1 += 600.5;
	return (uint16_t)mfloat1;
}
#endif
#endif
uint16_t Ti_Temp_Tab[]=
{   									//;-39
        2,3,3,3,4,4,5,7,10,15,   					//-30
        25,34,41,48,56,61,67,74,81,89,   				//-20
        98,108,118,128,140,152,166,181,198,215,				//-10
        234,254,275,299,324,353,381,413,447,484,				//0
        518,554,592,633,676,722,770,822,876, 				//9
        933,994,1057,1125,1196,1271,1350,1434,1522,1614, 		//19
        1722,1814,1922,2036,2155,2280,2411,2549,2700,2845, 		//29
        3004,3170,3345,3528,3719,3919,4122,4337,4560,4791,		//39
        5031,5280,5537,5802,6076,6359,6649,6947,7253,7567,		//49
        7887,8214,8547,8885,9229,9577,9928,10282,10639,10996,	//59
        11353,11709,12062,12411,12756,13093,13422,			//66
};


//输入参数：距离、环境温度、
void Tamb_TiEmiss_cal(int16_t TEMP)
{
	uint16_t  TempDist;
	uint16_t  TempTamb;
	uint16_t  TempRefl;
	uint16_t  sint1;
	uint16_t  sint2;
	float 	mfloat1;
	float 	mfloat2;
	float 	mfloat3;
	float 	Ti;

//	uint16_t Val;
//	uint8_t rx_buff[5]={0};

//    if(mTXTPara.Distunit)  //距离单位  英尺
//	{
//		mfloat1=(float)mPostPara.distance;
//		mfloat1*=0.3048;
//		mfloat1+=0.5;
//		TempDist=(unsigned int)mfloat1;
//	}
//	else
	if(TEMP>250)  //大于25度进行公式修正
	{
		TempDist=Temp_Parameter.distance;
		// 没有特别说明，TempDist全部改为 <=1
		if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
			if(TempDist <= 3) //千从项目3m起标定	
			{
				TempDist=10;
			}
		}else{
			if(TempDist <= 1) //通用版本	
			{
				TempDist=10;
			}
		}

	}
	else
	{
		TempDist=10;
	}
//	if(mTXTPara.Tempunit)  //温度单位
//	{
//		TempTamb=(int)english_to_unit_temp(mPostPara.Tamb);
//		TempRefl=(int)english_to_unit_temp(mPostPara.Trefl);

//	}
//	else
	{
		TempTamb=Temp_Parameter.Tamb; //环境温度
		TempRefl=Temp_Parameter.Trefl; //反射温度
   }


	sint1=TempTamb+2130;    //转换为绝对温度 calScreen_para.screen_para.cal_data.Tamb

//环境温度^4
	mfloat1=(float)sint1;
	mfloat1=mfloat1*mfloat1;
	mfloat1=mfloat1*mfloat1;
	TiTatm_H4=mfloat1;

//查表求 可凝水率b
	if(sint1<2350)     //绝对温度×10
	{
		sint2=2;
	}
	else
	{
		if(sint1>3370)
		{
			sint2=13422;
		}
		else
		{
			sint1=sint1-2340;
			sint1=sint1/10;
			sint2=Ti_Temp_Tab[sint1];
		}
	}

//	W_Cor=(float)(b_cor*(float)distval*relhum/100000+(float)distval*1.1106/1000)/100;
//可凝水量w
	mfloat1=(float)Temp_Parameter.RelHum; //相对湿度R
	mfloat2=(float)sint2;  //b
	mfloat2=mfloat2*mfloat1; //(L*(b*R+1.1106) )/100
	mfloat2=mfloat2+11106;
	mfloat2=mfloat2/1000000000;
//		mfloat2=mfloat2/10000000000;
//		mfloat2=mfloat2/10000000;
	mfloat1=(float)TempDist;
	mfloat2=mfloat2*mfloat1;

//	{
//		Val = (uint16_t)(mfloat2*1000);
////	Val1 = (uint16_t)mfloat2;
//	rx_buff[0] = TempDist>>8;
//	rx_buff[1] = TempDist>>0;
//	rx_buff[2] = mPostPara.RelHum>>0;
//	rx_buff[3] = Val>>8;
//		rx_buff[4] = Val>>0;
//
//	MSS_UART_polled_tx(&g_mss_uart1,rx_buff, sizeof(rx_buff));
//	}

// tao_i*Emiss
	if(mfloat2<0.165)
	{
		mfloat2=sqrt(mfloat2);
		mfloat3=mfloat2*(-0.598);
		Ti=exp(mfloat3);
        mfloat2=(float)Temp_Parameter.Emiss;
		mfloat2=mfloat2/100;
		mfloat2=mfloat2*Ti;
		TiEmiss=mfloat2;
	}
	else
	{
		mfloat3=0.165/mfloat2;
//		pow_cal();  //	mfloat2=pow_cal(mfloat2,0.122);
		mfloat3=pow(mfloat2,0.122);
		Ti=mfloat3*0.784;
				mfloat2=(float)Temp_Parameter.Emiss;
		mfloat2=mfloat2/100;
		mfloat2=mfloat2*Ti;
		TiEmiss=mfloat2;  //ε*τ_i
	};

// (1-Ti)*环境温度^4
	mfloat3=TiTatm_H4;  //环境温度^4
	mfloat3=mfloat3*(1-Ti);
	TiTatm_H4=mfloat3;

	//反射温度^4
	sint1=TempRefl+2130;    //转换为绝对温度 calScreen_para.screen_para.cal_data.Tamb
	mfloat3=(float)sint1;
	mfloat3=mfloat3*mfloat3;
	mfloat3=mfloat3*mfloat3;
	//system.TiEmiss_H4=mfloat1;
// (1-Emiss)Ti*环境温度^4
    mfloat2=(float)Temp_Parameter.Emiss;  //辐射率
	mfloat2=(1.0-mfloat2/100.0)*Ti;
	mfloat3=mfloat3*mfloat2;
	TiEmiss_H4 = mfloat3 + TiTatm_H4;  //反射温度+环境温度
	// vision_log_notice("TiEmiss_H4 = %d, mfloat3 = %d, TiTatm_H4 = %d\n", (int)TiEmiss_H4, (int)mfloat3, (int)TiTatm_H4);
}

int16_t Tamb_TiEmiss_Correct(int16_t temp0)
{
	float mfloat1;
	float mfloat2;
	Tamb_TiEmiss_cal(temp0); //衰减系数计算
	mfloat1 = (float)temp0+2730; //绝对温度
	mfloat1 = mfloat1*mfloat1;
	mfloat1 = mfloat1*mfloat1;
	mfloat2 = (mfloat1 -TiEmiss_H4) / TiEmiss;
	//	mfloat1 = TiEmiss*mfloat1;
	//	mfloat2 = mfloat1 + TiEmiss_H4;   //T^4=ε*τ_i*T_d^4+(1-ε)*τ_i*T_r^4+(1-τ_i)*T_a^4
	mfloat2=sqrt(mfloat2);
	mfloat2=sqrt(mfloat2);
	temp0 = (int16_t)(mfloat2-2730);
	// vision_log_notice("temp0= 0x%04x,TiEmiss_H4= [%.5f],TiEmiss= [%.5f]\n",temp0,TiEmiss_H4,TiEmiss);
	return temp0;
}
/** @note 测温点循环扫描 */
void ReDispSpot1Message()
{
	if((SelectionMenu.MenuState == 0x00)||(SelectionMenu.MenuState == 0x25)||((SelectionMenu.MenuState&0x03) == 0x03)) {
		if(Spot_state) {
			Spot_SimultaneousDisplay(Spot_state);
		}
	}
}
/** @note 递归获取当前的选中的区域 */
// void FindSelectArea(void) {
// 	static u8 UpdataFlag = 0;			//更新显示测温区域的标志位

// 	if(Area_state&(1<<UpdataFlag))
// 	{
// 		select_Area = &Areainit[UpdataFlag];
// 		UpdataFlag++;
// 		if(UpdataFlag>3) {
// 			UpdataFlag = 0;
// 		}
// 		return;
// 	}
// 	else
// 	{
// 		UpdataFlag++;
// 		if(UpdataFlag>3) {
// 			UpdataFlag = 0;
// 		}
// 		FindSelectArea();
// 	}
// }
// 获取下一个应该执行的位（在Area_state中为1的位中轮流选择）
static ts_s08 get_next_bit_to_execute(int area_state, int total_bits)
{
	static int last_executed_bit = -1;
    int valid_bits = area_state & ((1 << total_bits) - 1);

    if (valid_bits == 0 || total_bits <= 0) {
        return -1;
    }

    for (int offset = 1; offset <= total_bits; offset++) {
        int current_bit = (last_executed_bit + offset) % total_bits;

        if (valid_bits & (1 << current_bit)) {
            last_executed_bit = current_bit;
            return current_bit;
        }
    }

    return -1;
}
/** @note 测温功能--区域定时刷新 */
void ReDispArea1Message(void) {
	if((SelectionMenu.MenuState == 0x00)||(SelectionMenu.MenuState == 0x25)||((SelectionMenu.MenuState&0x07) == 0x07)) {
		if((Area_state&0x0f) && (SystemParaMenu2.Auto_Semi == 0)) {
			// Area_SimultaneousDisplay(Area_state & 0x000F);		//显示选中的测温区域
			// if(SelectionMenu.MenuState != 0x07) {
			// 	FindSelectArea();
			// }
			int bit_to_execute = get_next_bit_to_execute(Area_state, 4);
			switch (bit_to_execute)
			{	
				// 0：只显示温度，不移动坐标
				case 0: AreaN_Scale(&Areainit[0], 0, AreaBad_XY_Normal);break;
				case 1: AreaN_Scale(&Areainit[1], 0, AreaBad_XY_Normal);break;
				case 2: AreaN_Scale(&Areainit[2], 0, AreaBad_XY_Normal);break;
				case 3: AreaN_Scale(&Areainit[3], 0, AreaBad_XY_Normal);break;
				case -1:  // 没有需要执行的位
				default:break;
			}
		}
	}
}
/** @note 捕获自动刷新功能 */
void ReDispCaptureMessage(void)
{
	if((SelectionMenu.MenuState == 0x00)||(SelectionMenu.MenuState == 0x25)) {
		if((UsermenuParameter.CaptureFlag)) {
			Disp_Capture_Info();
		}
	}
}
#if 0
// void ReDispArea1Message(void)
{
	if((SelectionMenu.MenuState == 0x00)||(SelectionMenu.MenuState == 0x25)||((SelectionMenu.MenuState&0x03) == 0x03))
	{
		if((Area_state)&&(SystemParaMenu2.Auto_Semi == 0))
		{
			Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域

			if(!Alarm_Flag)
			{
#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
				Area_Val_Simultaneous();
#endif
				if(SelectionMenu.MenuState != 0x07)
				{
					// FindSelectArea();//递归获取当前的选中的Area
				}
				AreaN_Scale(select_Area,0,AreaBad_XY_Normal);
#ifdef Area2TestDisp
	//***************************Test*********************************//
		if(UsermenuParameter.CaptureFlag == 0)
		{
			if(Area_state&0x0002)	TestArea2Message(&Areainit[1]);
			else  	ClearSize16X16(X_START,28*16+100,Y_START+392);
		}
#endif
			}
			if(Alarm_Flag) {ClearFont(0,64);}
		}
	}
}

void ReDispQuadMessage(void)
{
#ifdef	Electrolytic_copper
	if((SelectionMenu.MenuState == 0x00)||(SelectionMenu.MenuState == 0x25)||(SelectionMenu.MenuState&0x03 == 0x03))
	{
		if(UsermenuParameter.Quad_SW)
		{
			drawIrregularPolygonBox(vertices, CharacterColor);
			DisplayQuadTemp();

			Quad_TemAlarmSet();

		}
	}
#endif
}
#endif

/** @note 已弃用 */
void SpotMessage(char *Sportname,ts_u16 SportX,ts_u16 SportY,ts_u16 Sportid)
{
}

/** @note 测温档位切换DET参数更新 */
void TemLevel_convert(void) {
	ISP_set_det_calibrationing_flag(1);
	ts_u08 sun_pt_sw = SystemParaMenu2.SunPT_SW;
	SystemParaMenu2.SunPT_SW = 0;

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	vot_vpo_freeze();
	ISP_pause();

	if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
	{
		SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[0];
		SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[0];
		SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[0];
		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[0];
	}
	else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
	{
		SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[1];
		SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[1];
		SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[1];
		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[1];
	}
	else  if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
	{
		SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[2];
		SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[2];
		SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[2];
		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[2];
	}
            
	board_type_t board_name =  get_powerup_board_port_type();
	if(SystemParaMenu2.Usermenu_SW <= 1) //一档测温 1
	{
		if (board_name == BOARD_PORT_UAD3_LITE) {
			snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);
		}
		else if (board_name == BOARD_PORT_UAD3) {
			snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);
		}
	}
	else if((SystemParaMenu2.Usermenu_SW == 2) ||(SystemParaMenu2.Usermenu_SW == 3))//二档、三档测温  0
	{
		if (board_name == BOARD_PORT_UAD3_LITE) {
			snr_gpio_cfg2out(46, TS_ELEC_LVL_LO);
		}
		else if (board_name == BOARD_PORT_UAD3) {
			snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);
		}
	}
	ts_cpl_msleep(1000);
	planck640c_write_gain_adc(SystemParaMenu2.GAIN);
	planck640c_write_gof(SystemParaMenu2.GOF);
	planck640c_write_gos(SystemParaMenu2.OOC_Par);
	planck640c_write_cint(SystemParaMenu2.INT_C);
	planck640c_write_tint(SystemParaMenu2.INT_T);
    planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
	//避免重复配置gos
	det_gos_switch_force_last = SystemParaMenu2.OOC_Par;
	ISP_run();
    ts_cpl_fix_msleep(1500);
	vot_isp_temp_cut_with_flag(0,1);
    ts_cpl_fix_msleep(1500);
	vot_isp_temp_cut_with_flag(0,1);
	ts_cpl_msleep(500);
	vot_vpo_unfreeze();
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	//恢复阳光保护的开关状态
	SystemParaMenu2.SunPT_SW = sun_pt_sw;
	ISP_set_det_calibrationing_flag(0);
	vision_log_debug("leave : after ISP_cut \n");
	ts_cpl_msleep(100);
	ISP_cut();
}
/** @note AD基础处理 */
static ts_u16 temp_lut_ad_normalize(ts_u16 raw_ad){
	return raw_ad & 0x3FFF;
}
/** @note AD_sensor处理 */
static  ts_u16 temp_lut_ad_with_offset(ts_u16 origin_ad, ts_u16 sensor_offset) {
	ts_u16 offset = sensor_offset & 0x7FFF;
	if (sensor_offset >> 15) {
		return origin_ad - offset;
	} else {
		return origin_ad + offset;
	}
}
/** @note 温度修正 */
static ts_f32 temp_lut_value_correct(ts_f32 raw_temp) {
	int16_t temp_cal = (int16_t)(raw_temp * 10);
	temp_cal = Temp_AllCorrect(temp_cal);
	return (ts_f32)temp_cal / 10.0f;
}
/** @note 核心显示类型判断函数 */
static const temp_disp_tyep_e temp_disp_lut[2][4] = {
    /* mode = 0 */
    {
        TEMP_NORMAL_DISP_ORIAD,
        TEMP_NORMAL_DISP_LUT_AD,
		TEMP_NORMAL_DISP_TEMP,
        TEMP_CALIBRATION_DISP_AD
    },

    /* mode = 1 */
    {
        TEMP_NORMAL_DISP_ORIAD,
        TEMP_NORMAL_DISP_LUT_AD,
		TEMP_NORMAL_DISP_TEMP,
        TEMP_CALIBRATION_DISP_AD
    }
};
/** @note 查表(唯一数据源) */
static temp_disp_tyep_e temp_lut_disp_mode_get(ts_u08 mode, ts_u08 disp)
{
    if (mode >= 2 || disp >= TEMP_DISP_MODE_MAX) {
        return TEMP_DISP_MODE_MAX;
    }

    return temp_disp_lut[mode][disp];
}
/** @note 测温点数值显示管理 */
void Sport_Display_Process(struct Spot* temp_spot_t) {
	ts_char info_tip[64] = {0};
	ts_u16 tmp_ad = 0;
	ts_u16 raw_ad = temp_spot_t->spot_AD;
	ts_u16 ad_val = temp_lut_ad_normalize(raw_ad);
	temp_disp_tyep_e cur_mode = temp_lut_disp_mode_get(SystemParaMenu2.m_temperCalibra_Flag, SystemParaMenu2.AD_T);
	switch (cur_mode) {
		case TEMP_CALIBRATION_DISP_AD: 
			tmp_ad = ad_val - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			if((Temp_Parameter.Second_CalibFlag == 1)&&(SecondCalib_Parameter.CalibFlag == 1)) {SecondCalib_Parameter.CalibAD = tmp_ad;}  //记录二次标定AD值
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_ORIAD: 
			ts_cpl_snprintf(info_tip, 64, "0x%04X", ad_val);
			break;
		case TEMP_NORMAL_DISP_LUT_AD: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_TEMP: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_f32 reTemp = LookUP_ADtoTemp(tmp_ad, SystemParaMenu2.Usermenu_SW);
			reTemp = temp_lut_value_correct(reTemp);
			if((Temp_Parameter.Second_CalibFlag == 1)&&(SecondCalib_Parameter.Global_offsetFlag)) {
				ts_s16 tmp_temp = (ts_s16)(reTemp*10);
				SecondCalib_Parameter.CalibTemp = tmp_temp;}  //记录二次标定的温度值
			ts_cpl_snprintf(info_tip, 64, "%+.1f", reTemp);
			break;
		default: 
			ts_cpl_snprintf(info_tip, sizeof(info_tip), "");
			break;
	}
	Refresh_templutSpot_Captions(info_tip, temp_spot_t, 1);
}
/** @note 测温区域数值显示管理 */
void Area_Display_Process(struct Area* temp_area_t) {
	ts_char special_Mess[64] = {0};
	ts_u16 tmp_ad_Max, tmp_ad_Min, tmp_ad_Avg;

	ts_u16 raw_ad_Max = temp_area_t->Area_temp_MAX;
	ts_u16 raw_ad_Min = temp_area_t->Area_temp_MIN;
	ts_u16 raw_ad_Avg = temp_area_t->Area_temp_AVG;
	ts_u16 ad_val_Max = temp_lut_ad_normalize(raw_ad_Max);
	ts_u16 ad_val_Min = temp_lut_ad_normalize(raw_ad_Min);
	ts_u16 ad_val_Avg = temp_lut_ad_normalize(raw_ad_Avg);

	temp_disp_tyep_e cur_mode = temp_lut_disp_mode_get(SystemParaMenu2.m_temperCalibra_Flag, SystemParaMenu2.AD_T);

	switch (cur_mode) {
		case TEMP_CALIBRATION_DISP_AD: 
			tmp_ad_Max = ad_val_Max - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			tmp_ad_Min = ad_val_Min - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			tmp_ad_Avg = ad_val_Avg - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			ts_cpl_snprintf(special_Mess, 64, "%s Max: %04X Min: %04X Avg: %04X", temp_area_t->name, tmp_ad_Max, tmp_ad_Min, tmp_ad_Avg);
			break;
		case TEMP_NORMAL_DISP_ORIAD: 
			ts_cpl_snprintf(special_Mess, 64, "%s Max: %04X Min: %04X Avg: %04X", temp_area_t->name, ad_val_Max, ad_val_Min, ad_val_Avg);
			break;
		case TEMP_NORMAL_DISP_LUT_AD: 
			if(VotParameters.Test_Flag) {	//采集的时候输出平均AD值
				tmp_ad_Max = ad_val_Max + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				tmp_ad_Min = ad_val_Min + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				tmp_ad_Avg = ad_val_Avg + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				temp_area_t->Area_temp_MAX = tmp_ad_Max;
				temp_area_t->Area_temp_MIN = tmp_ad_Min;
				temp_area_t->Area_temp_AVG = tmp_ad_Avg;
				ts_cpl_snprintf(special_Mess, 64, "%s Max: %04X Min: %04X Avg: %04X", temp_area_t->name, temp_area_t->Area_temp_MAX, temp_area_t->Area_temp_MIN, temp_area_t->Area_temp_AVG);
			}
			else {
				tmp_ad_Max = ad_val_Max + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				tmp_ad_Min = ad_val_Min + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				tmp_ad_Avg = ad_val_Avg + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
				tmp_ad_Max = temp_lut_ad_with_offset(tmp_ad_Max, Temp_Parameter.AD_sensor);
				tmp_ad_Min = temp_lut_ad_with_offset(tmp_ad_Min, Temp_Parameter.AD_sensor);
				tmp_ad_Avg = temp_lut_ad_with_offset(tmp_ad_Avg, Temp_Parameter.AD_sensor);
				ts_cpl_snprintf(special_Mess, 64, "%s Max: %04X Min: %04X Avg: %04X", temp_area_t->name, tmp_ad_Max, tmp_ad_Min, tmp_ad_Avg);
			}
			break;
		case TEMP_NORMAL_DISP_TEMP: 
			tmp_ad_Max = ad_val_Max + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad_Min = ad_val_Min + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad_Avg = ad_val_Avg + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad_Max = temp_lut_ad_with_offset(tmp_ad_Max, Temp_Parameter.AD_sensor);
			tmp_ad_Min = temp_lut_ad_with_offset(tmp_ad_Min, Temp_Parameter.AD_sensor);
			tmp_ad_Avg = temp_lut_ad_with_offset(tmp_ad_Avg, Temp_Parameter.AD_sensor);

			ts_f32 reTmp_Max = LookUP_ADtoTemp(tmp_ad_Max, SystemParaMenu2.Usermenu_SW);
			ts_f32 reTmp_Min = LookUP_ADtoTemp(tmp_ad_Min, SystemParaMenu2.Usermenu_SW);
			ts_f32 reTmp_avg = LookUP_ADtoTemp(tmp_ad_Avg, SystemParaMenu2.Usermenu_SW);

			reTmp_Max = temp_lut_value_correct(reTmp_Max);
			reTmp_Min = temp_lut_value_correct(reTmp_Min);
			reTmp_avg = temp_lut_value_correct(reTmp_avg);

			ts_cpl_snprintf(special_Mess, 64, "%s Max: %+.1f Min: %+.1f Avg: %+.1f", temp_area_t->name, reTmp_Max, reTmp_Min, reTmp_avg);
			break;
		default: 
			ts_cpl_snprintf(special_Mess, sizeof(special_Mess), "");
			break;
	}
	/* OSD显示位置 */
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (max_width > 384 && max_heigth > 288) {
		temp_area_t->Area_X_Pos = 20;
		temp_area_t->Area_Y_Pos = 470;
	}
	else {
		temp_area_t->Area_X_Pos = 20;
		temp_area_t->Area_Y_Pos = 245;
	}
	Refresh_templut_AreaCaptions(special_Mess, temp_area_t, 1);
	// vision_log_notice("name:%s, Lx=%d Ly=%d Rx=%d Ry=%d\n", temp_area_t->name, temp_area_t->Area_Lx, temp_area_t->Area_Ly, temp_area_t->Area_Rx, temp_area_t->Area_Ry);
}
/** @note 测温捕获点--最小值 */
void Disp_Capture_Spot_MIN_info(ts_u08 visible) {
	ts_char info_tip[64] = {0};
	ts_u16 tmp_ad = 0;
	// 显示光标
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, visible);
	templutCursor_Set_position(global_sys_ctrl_info.m_lutCapture_minCursor_dot, spot_min.spot_X_Pos, spot_min.spot_Y_Pos);

	ts_u16 raw_ad = spot_min.spot_AD;
	ts_u16 ad_val = temp_lut_ad_normalize(raw_ad);

	temp_disp_tyep_e cur_mode = temp_lut_disp_mode_get(SystemParaMenu2.m_temperCalibra_Flag, SystemParaMenu2.AD_T);
	switch (cur_mode) {
		case TEMP_CALIBRATION_DISP_AD: 
			tmp_ad = ad_val - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_ORIAD: 
			ts_cpl_snprintf(info_tip, 64, "0x%04X", ad_val);
			break;
		case TEMP_NORMAL_DISP_LUT_AD: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_TEMP: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_f32 reTemp = LookUP_ADtoTemp(tmp_ad, SystemParaMenu2.Usermenu_SW);
			reTemp = temp_lut_value_correct(reTemp);
			ts_cpl_snprintf(info_tip, 64, "%+.1f", reTemp);
			break;
		default: 
			ts_cpl_snprintf(info_tip, sizeof(info_tip), "");
			break;
	}
	Refresh_templutSpot_Captions(info_tip, &spot_min, visible);
}
/** @note 测温捕获点--最大值 */
void Disp_Capture_Spot_MAX_info(ts_u08 visible) {
	ts_char info_tip[64] = {0};
	ts_u16 tmp_ad = 0;

	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, visible);
	templutCursor_Set_position(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, spot_max.spot_X_Pos, spot_max.spot_Y_Pos);

	ts_u16 raw_ad = spot_max.spot_AD;
	ts_u16 ad_val = temp_lut_ad_normalize(raw_ad);

	temp_disp_tyep_e cur_mode = temp_lut_disp_mode_get(SystemParaMenu2.m_temperCalibra_Flag, SystemParaMenu2.AD_T);
	switch (cur_mode) {
		case TEMP_CALIBRATION_DISP_AD: 
			tmp_ad = ad_val - global_sys_ctrl_info.m_cur_bak_ad_mean_u32 + 0x1000;
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_ORIAD: 
			ts_cpl_snprintf(info_tip, 64, "0x%04X", ad_val);
			break;
		case TEMP_NORMAL_DISP_LUT_AD: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_cpl_snprintf(info_tip, 64, "0x%04X", tmp_ad);
			break;
		case TEMP_NORMAL_DISP_TEMP: 
			tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
			ts_f32 reTemp = LookUP_ADtoTemp(tmp_ad, SystemParaMenu2.Usermenu_SW);
			reTemp = temp_lut_value_correct(reTemp);
			ts_cpl_snprintf(info_tip, 64, "%+.1f", reTemp);
			break;
		default: 
			ts_cpl_snprintf(info_tip, sizeof(info_tip), "");
			break;
	}
	Refresh_templutSpot_Captions(info_tip, &spot_max, visible);

	if((UsermenuParameter.CaptureFlag == 3)||(UsermenuParameter.CaptureFlag == 7)) { /*TODO: */ }
	else { /*TODO: */ }
}
/********************************************************
 * 通过 ts_lib_up_start 方式提取 UPL 枚举任一节点数据
 * 需要快取快出，不允许过长时间滞留，特别注意事项如下：
 * 1、TS_UPL_POS_VPO_YUV 节点在UVC硬件接口数据流会存在64K包头
 * 2、TS_UPL_POS_BEFORE_AGC 需要右移2位
********************************************************/
ts_u32 u32_dump_upl_Frm_Size = 0;
libup_ctx_s_t vot_sst_ctx = {0};
static ts_u32 u32_frame_staging_buf = 0;

/** @note 获取帧信息 */
static ts_void votGet_SingleFrm_info(ts_u32 u32_pos, libup_ctx_s_t* sst_ctx, ts_u32 *pu32_frm_size, ts_u32 *pu32_strid_size)
{
    ts_u32 u32_frm_size, u32_width, u32_height, u32_strid;

    if (u32_pos >= TS_UPL_POS_VPO_YUV)
        tdl_mupl_get_frm_info((ts_u32 *)&u32_width, (ts_u32 *)&u32_height, ts_null);
    else
        tdl_isp_get_frm_info(TS_ISP_MDEV0, (ts_u32 *)&u32_width, (ts_u32 *)&u32_height);

	
    memset(sst_ctx, 0, sizeof(libup_ctx_s_t));
    sst_ctx->u16_width = (ts_u16)u32_width;
    sst_ctx->u16_height = (ts_u16)u32_height;
    sst_ctx->u32_pos = u32_pos;
    // vision_log_debug("u32_pos = %d, u32_width = %d, u32_height = %d\n", u32_pos, sst_ctx->u16_width, sst_ctx->u16_height);
    u32_frm_size = u32_width * u32_height;
    if (TS_UPL_POS_VPO_YUV == u32_pos) {
        u32_frm_size = u32_frm_size << 1;
        u32_strid = (u32_width * (u32_height + 4)) << 1;
    } else {
        if (u32_pos < TS_UPL_POS_ISP)
            u32_frm_size = u32_frm_size << 1;

        u32_strid = u32_frm_size;
    }

    if (pu32_frm_size != ts_null)
        *pu32_frm_size = u32_frm_size;

    if (pu32_strid_size != ts_null)
        *pu32_strid_size = u32_strid;
}
/** @note 申请帧数据内存转移 */
ts_s08 votMalloc_FrmAddr(ts_s32 s32_nPos) {
	ts_u32 u32_frm_size;
	
    ts_cpl_memset(&vot_sst_ctx, 0, sizeof(libup_ctx_s_t));
    votGet_SingleFrm_info(s32_nPos, &vot_sst_ctx, (ts_u32 *)&u32_frm_size, (ts_u32 *)&u32_dump_upl_Frm_Size);
    u32_dump_upl_Frm_Size = u32_dump_upl_Frm_Size * 2;
	vision_log_debug("u32_pos = %d, u32_width = %d, u32_height = %d, u32_dump_upl_Frm_Size = 0x%x\n", s32_nPos, vot_sst_ctx.u16_width, vot_sst_ctx.u16_height, u32_dump_upl_Frm_Size);
	/* 帧数据转存缓冲 */
    if (u32_frame_staging_buf == 0) {
        u32_frame_staging_buf = (ts_u32)ts_cpl_tmm_malloc(ts_null, u32_dump_upl_Frm_Size, 32, "storage_dumpFrm");
        if (u32_frame_staging_buf == ts_null) {
            vision_log_warn("[votMalloc]: u32_frame_staging_buf is ts_null ...\n");
        }
		else {
			ts_cpl_memset((void *)u32_frame_staging_buf, 0, u32_dump_upl_Frm_Size);
			return TS_OK;
		}
    }
	return TS_ERR;
}
/** @note 获取UPL节点视频流 */
ts_s08 votDumpTemplut_SingleFrmData(ts_s32 s32_nPos) {
	ts_u32 i;
    ts_u32 s32_is_dma = 0;
    ts_nodebuf_s_t *pst_node;
    ts_s32 u32_frm_cnt = 1;
    ts_u32 u32_node_cnt = u32_frm_cnt + 1;

	/* 菜单二下不刷新，进入半自动，十字光标校正情况下均不更新 */
	if(SelectionMenu.MenuState == 0x26 || SystemParaMenu2.Auto_Semi != 0 || global_sys_ctrl_info.m_system_mode != SYSTEM_MODE_NORMAL) {
		return TS_ERR;	//
	}
	if ((global_sys_ctrl_info.m_allowGetFrameStream_flag != 1) && Spot_state == 0 && (Area_state & 0x0f) == 0 && UsermenuParameter.CaptureFlag == 0) {
		return TS_ERR;
	}
    /* */
    ts_void *pv_tmm = ts_cpl_tmm_malloc(ts_null, u32_dump_upl_Frm_Size, 32, "upl_dataFrm_buff");
    if (ts_null == pv_tmm) {
		return TS_ERR;
	}
	
	ts_cpl_memset(pv_tmm, 0, u32_dump_upl_Frm_Size);
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
	global_sys_ctrl_info.m_temp_get_streaming_flag = 1;
    tdl_isp_claim_s_chn();
    ts_lib_up_start(s32_nPos, (ts_u32 *)pv_tmm, u32_frm_cnt, u32_node_cnt, s32_is_dma);
    for (i = 0; i < u32_frm_cnt; i++) {
        pst_node = ts_lib_up_get_node(100);
        if (ts_null == pst_node) {
            break;
        }
        // ts_u16 *pu16_ptr = (ts_u16 *)pst_node->u32_virt_addr;
		ts_cpl_memset((ts_void*)u32_frame_staging_buf, 0, u32_dump_upl_Frm_Size);
        ts_cpl_memcpy((ts_void*)u32_frame_staging_buf, (ts_void*)pst_node->u32_virt_addr, u32_dump_upl_Frm_Size);
        ts_lib_up_release_node(pst_node);
    }
    ts_lib_up_stop();
    tdl_isp_release_s_chn();
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
	global_sys_ctrl_info.m_temp_get_streaming_flag = 0;

    ts_cpl_tmm_free(pv_tmm);
	if (global_sys_ctrl_info.m_allowGetFrameStream_flag == 1) {
		global_sys_ctrl_info.m_allowGetFrameStream_flag = 0;
	}
	if (global_sys_ctrl_info.m_sync_latest_frame_id++ >= 60000) {
		global_sys_ctrl_info.m_sync_latest_frame_id = 0;
	}
	return TS_OK;
}

/*********************************************************************/
/** @note 大端转换成小端 */
static uint16_t swap_16(uint16_t value) {
    return (value << 8) | (value >> 8);
}
/** @note 根据行号返回需要去掉的元素个数
 * 行号范围：0 到 511。
 * 如果行号无效，返回 -1。
 */
static ts_s32 get_skip_count(ts_s32 line) {
    // 每个范围起始行号，额外添加一个虚拟值 512 便于比较
    static const ts_s32 start_640[] = {0, 26, 52, 77, 103, 128, 154, 180, 205, 231, 256, 282, 308, 333, 359, 384, 410, 436, 461, 487, 512};
	static const ts_s32 start_384[] = {0, 43, 86, 128, 171, 214, 256, 288};
	ts_s32 num_ranges;
	const ts_s32 *startOffest;

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	if(res_type==DECTOR_RESOLUTION_640X512){
		if (line < 0 || line > 511) {
			return -1;
		}
        startOffest = start_640;
        num_ranges = (sizeof(start_640) / sizeof(start_640[0])) - 1;
	}
	else {
		if (line < 0 || line > 287) {
			return -1;
		}
        startOffest = start_384;
        num_ranges = (sizeof(start_384) / sizeof(start_384[0])) - 1;
	}
    

    for (ts_s32 i = 0; i < num_ranges; i++) {
        if (line < startOffest[i + 1]) {
            return i + 1;
        }
    }

    return -1;
}
ts_u16 UART_ConvertRawAd_To_CalibAd(ts_u16 raw_ad)
{
	ts_u16 ad_val = temp_lut_ad_normalize(raw_ad);
	return ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
}
/** @note 原始AD内部公式转换成测温AD */
ts_u16 UART_ConvertRawAd_To_tempAd(ts_u16 raw_ad)
{
	ts_u16 tmp_ad = 0;
	ts_u16 ad_val = temp_lut_ad_normalize(raw_ad);
	tmp_ad = ad_val + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
	tmp_ad = temp_lut_ad_with_offset(tmp_ad, Temp_Parameter.AD_sensor);
	return tmp_ad;
}
/** @note 原始AD通过自定义查表方式转换成温度 */
int16_t UART_ConvertRawAdToTemp(ts_u16 raw_ad) 
{
	ts_u16 tmp_ad = UART_ConvertRawAd_To_tempAd(raw_ad);
	ts_f32 reTemp = LookUP_ADtoTemp(tmp_ad, SystemParaMenu2.Usermenu_SW);
	reTemp = temp_lut_value_correct(reTemp);

	return (int16_t)(reTemp*10.0f);
}

/* AGC节点：从内存中获取原始 AD → 计算 → 得到用于测温的 AD 值 */
ts_u16 TP_GetTempFromMem(ts_u16 spot_X_Pos, ts_u16 spot_Y_Pos) {
	ts_u32 index = 0;
	ts_u16 tmp_OriginalAD = 0;
	ts_u16 max_width = 0, max_heigth = 0;

	v_getDector_DisplayResolution(&max_width, &max_heigth);	
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_u16 *pu16_ptr = (ts_u16 *)u32_frame_staging_buf;

	if ((spot_X_Pos <= max_width && spot_X_Pos >= 0) && (spot_Y_Pos <= max_heigth && spot_Y_Pos >= 0)) {
		if (UsermenuParameter.Mirror == 1) {	// X镜像
			spot_X_Pos = (max_width - 1) - spot_X_Pos;
		}
		else if(UsermenuParameter.Mirror == 2) { // Y镜像
			spot_Y_Pos = (max_heigth - 1) - spot_Y_Pos;
		}
		else if(UsermenuParameter.Mirror == 3) { // XY镜像
			spot_X_Pos = (max_width - 1) - spot_X_Pos;
			spot_Y_Pos = (max_heigth - 1) - spot_Y_Pos;
		}
		index = spot_Y_Pos * vot_sst_ctx.u16_width + spot_X_Pos;
		tmp_OriginalAD = (pu16_ptr[index]) >> 2;
	}
	else {
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
		return 0xFFFF;
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

	return tmp_OriginalAD;
}
/* AGC节点：从内存中获取原始 AD → 计算 → 得到用于测温的 AD 值 */
ts_s08 TZ_GetTempFromMem(struct Area* vAreainit) {
	ts_u32 index;
	ts_u64 sumTemp = 0;
	ts_u64 avgTemp = 0;
	ts_u32 Area_valid_pixels = 0;
	ts_u16 maxTemp = 0, minTemp = 0x3fff;
	ts_u16 max_width = 0, max_heigth = 0;
	ts_u16 Mir_Area_Lx = 0, Mir_Area_Ly = 0, Mir_Area_Rx = 0, Mir_Area_Ry = 0;

	v_getDector_DisplayResolution(&max_width, &max_heigth);	
	if(UsermenuParameter.Mirror == 1){ //X镜像
		Mir_Area_Lx = (max_width - 1) - vAreainit->Area_Rx;
		Mir_Area_Rx = (max_width - 1) - vAreainit->Area_Lx;
		Mir_Area_Ly = vAreainit->Area_Ly;
		Mir_Area_Ry = vAreainit->Area_Ry;
	}else if(UsermenuParameter.Mirror == 2){ //Y镜像
		Mir_Area_Ly = (max_heigth - 1) - vAreainit->Area_Ry;
		Mir_Area_Ry = (max_heigth - 1) - vAreainit->Area_Ly;
		Mir_Area_Lx = vAreainit->Area_Lx;
		Mir_Area_Rx = vAreainit->Area_Rx;
	}
	else if(UsermenuParameter.Mirror == 3){ //XY镜像
		Mir_Area_Lx = (max_width - 1) - vAreainit->Area_Rx;
		Mir_Area_Ly = (max_heigth - 1) - vAreainit->Area_Ry;
		Mir_Area_Rx = (max_width - 1) - vAreainit->Area_Lx;
		Mir_Area_Ry = (max_heigth - 1) - vAreainit->Area_Ly;
	}else{
		Mir_Area_Lx = vAreainit->Area_Lx;
		Mir_Area_Ly = vAreainit->Area_Ly;
		Mir_Area_Rx = vAreainit->Area_Rx;
		Mir_Area_Ry = vAreainit->Area_Ry;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_u16 *pu16_ptr = (ts_u16 *)u32_frame_staging_buf;

	for (ts_u32 y = Mir_Area_Ly; y < Mir_Area_Ry; y++) {
		for (ts_u32 x = Mir_Area_Lx; x < Mir_Area_Rx; x++) {
			index =  (y * vot_sst_ctx.u16_width) + x;
			ts_u16 tmp_value = (pu16_ptr[index]) >> 2;
			if (tmp_value == 0x0000) {
				continue;
			}
			Area_valid_pixels++;
			if (maxTemp < tmp_value) {
				maxTemp = tmp_value;
			}
			if (minTemp > tmp_value) {
				minTemp = tmp_value;
			}
			sumTemp += tmp_value;
		}
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

	if (Area_valid_pixels > 0) {
		avgTemp = sumTemp / Area_valid_pixels;
	}
	else {
		avgTemp = 0;
		minTemp = 0;
		maxTemp = 0;
		vision_log_warn("TZ_GetTempFromMem: Area_valid_pixels is 0, cannot calculate average temp\n");
		return TS_ERR;
	}
	
	vAreainit->Area_temp_AVG = (ts_u16)avgTemp;
	vAreainit->Area_temp_MAX = maxTemp;
	vAreainit->Area_temp_MIN = minTemp;
	return TS_OK;
}

void CAP_GetTempFromMem(struct Spot* vSpot_max, struct Spot* sVpot_min) {
	ts_u32 index;
	ts_u16 maxTemp = 0, minTemp = 0x3fff;
	ts_u16 max_x = 0, max_y = 0;
	ts_u16 min_x = 0, min_y = 0;
	ts_u16 max_width = 0, max_heigth = 0;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_u16 *pu16_ptr = (ts_u16 *)u32_frame_staging_buf;
	for (ts_s32 y = CAPTUREBOUNDARY; y < max_heigth - CAPTUREBOUNDARY; y++) {
		for (ts_s32 x = CAPTUREBOUNDARY; x < max_width - CAPTUREBOUNDARY; x++) {
			index =  (y * vot_sst_ctx.u16_width) + x;
			ts_u16 tmp_value = (pu16_ptr[index]) >> 2;
			if (tmp_value == 0x0000) {
				continue;
			}
			if(maxTemp < tmp_value) {
				maxTemp = tmp_value;
				max_x = x;
				max_y = y;
			}
			if(minTemp > tmp_value) {
				minTemp = tmp_value;
				min_x = x;
				min_y = y;
			}
		}
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
	vision_log_notice("CAP_GetTempFromMem: maxTemp=0x%04X at (%d, %d), minTemp=0x%04X at (%d, %d)\n", maxTemp, max_x, max_y, minTemp, min_x, min_y);
	if(UsermenuParameter.Mirror == 1){ //X镜像
		max_x = (max_width - 1) - max_x;
		min_x = (max_width - 1) - min_x;
	}else if(UsermenuParameter.Mirror == 2){ //Y镜像
		max_y = (max_heigth - 1) - max_y;
		min_y = (max_heigth - 1) - min_y;
	}else if(UsermenuParameter.Mirror == 3){ //XY镜像
		max_x = (max_width - 1) - max_x;
		min_x = (max_width - 1) - min_x;
		max_y = (max_heigth - 1) - max_y;
		min_y = (max_heigth - 1) - min_y;
	}	
	vSpot_max->spot_X_Pos = max_x;
	vSpot_max->spot_Y_Pos = max_y;
	vSpot_max->spot_AD = maxTemp;

	sVpot_min->spot_X_Pos = min_x;
	sVpot_min->spot_Y_Pos = min_y;
	sVpot_min->spot_AD = minTemp;
}
/** @note 同一帧中，同时计算测温点，测温区域，捕获等AD/temp */
ts_s08 votCalcul_memory_TempOrAd(ts_s32 s32_nPos) {
	ts_u32 index;
	ts_u16 Mirrorx = 0, Mirrory = 0;
	ts_u16 Mirr_AreaLx = 0, Mirr_AreaRx = 0, Mirr_AreaLy = 0, Mirr_AreaRy = 0;

	if (Spot_state == 0 && (Area_state&0x0f) == 0 && UsermenuParameter.CaptureFlag == 0) {
		return TS_ERR;
	}

	ts_u16 *pu16_ptr = (ts_u16 *)u32_frame_staging_buf;
	
	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);
	/*------------- 测温点相关计算 -------------*/
	for (ts_u08 vIndex = 0; vIndex < 3; vIndex++) {
		if (Spot_state & (1 << vIndex)) {
			if ((SpotInit[vIndex].spot_X_Pos <= new_width && SpotInit[vIndex].spot_X_Pos >= 0) && (SpotInit[vIndex].spot_Y_Pos <= new_height && SpotInit[vIndex].spot_Y_Pos >= 0)) {
				if (UsermenuParameter.Mirror == 0) {
					if (s32_nPos >= TS_UPL_POS_VPO_YUV) {
						ts_u16 vOffest = get_skip_count(SpotInit[vIndex].spot_Y_Pos);
						if (vOffest > 0) {
							index =  SpotInit[vIndex].spot_Y_Pos * vot_sst_ctx.u16_width + SpotInit[vIndex].spot_X_Pos + vOffest;
							SpotInit[vIndex].spot_AD = swap_16(pu16_ptr[index]);;
							if (SpotInit[vIndex].spot_AD == 0x0000) {
								SpotInit[vIndex].spot_AD = swap_16(pu16_ptr[index+1]);
							}
						}
					}
					else {
						index =  (SpotInit[vIndex].spot_Y_Pos * vot_sst_ctx.u16_width) + SpotInit[vIndex].spot_X_Pos;
						SpotInit[vIndex].spot_AD = (pu16_ptr[index]) >> 2;
					}
				}
				else {
					if(UsermenuParameter.Mirror == 1){ //X镜像
						 Mirrorx = (new_width - 1) - SpotInit[vIndex].spot_X_Pos;
						 Mirrory = SpotInit[vIndex].spot_Y_Pos;
					}else if(UsermenuParameter.Mirror == 2){ //Y镜像
						 Mirrory = (new_height - 1) - SpotInit[vIndex].spot_Y_Pos;
						 Mirrorx = SpotInit[vIndex].spot_X_Pos;
					}else if(UsermenuParameter.Mirror == 3){ //XY镜像
						 Mirrorx = (new_width - 1) - SpotInit[vIndex].spot_X_Pos;
						 Mirrory = (new_height - 1) - SpotInit[vIndex].spot_Y_Pos;
					}

					if (s32_nPos >= TS_UPL_POS_VPO_YUV) {
						ts_u16 vOffest = get_skip_count(Mirrory);
						if (vOffest > 0) {
							index =  Mirrory * vot_sst_ctx.u16_width + Mirrorx + vOffest;
							SpotInit[vIndex].spot_AD = swap_16(pu16_ptr[index]);;
							if (SpotInit[vIndex].spot_AD == 0x0000) {
								SpotInit[vIndex].spot_AD = swap_16(pu16_ptr[index+1]);
							}
						}
					}
					else {
						index =  (Mirrory * vot_sst_ctx.u16_width) + Mirrorx;
						SpotInit[vIndex].spot_AD = (pu16_ptr[index]) >> 2;
					}
				}
			}
		}
	}
	/*------------- 测温区域相关计算 -------------*/
	for (ts_u08 vi = 0; vi < 2; vi++) {
		ts_u64 sumTemp = 0;
		ts_u64 avg_value = 0;
		ts_u16 maxTemp = 0, minTemp = 0x3fff;
		ts_u32 Area_valid_pixels = 0;

		int bit_to_execute = get_next_bit_to_execute(Area_state, 4) + 1;
		if (bit_to_execute > 0) {
			if(UsermenuParameter.Mirror == 0){ //无镜像
				if (s32_nPos >= TS_UPL_POS_VPO_YUV) {
					for(ts_s32 y = Areainit[bit_to_execute - 1].Area_Ly; y < Areainit[bit_to_execute - 1].Area_Ry; y++){
						ts_u16 vOffest = get_skip_count(y);
						if (vOffest > 0) {
							for (ts_s32 j = Areainit[bit_to_execute - 1].Area_Lx; j < Areainit[bit_to_execute - 1].Area_Rx; j++) {
								index =  (y * vot_sst_ctx.u16_width) + j + vOffest;
								ts_u16 tmp_value = swap_16(pu16_ptr[index]);
								/* */
								if(tmp_value == 0x0000) {
									continue;
								}
								Area_valid_pixels++;
								if(maxTemp < tmp_value) {
									maxTemp = tmp_value;
								}
								if(minTemp > tmp_value) {
									minTemp = tmp_value;
								}
								sumTemp += tmp_value;
							}
						}
						else {
							vision_log_warn("row Value is ERROR! y=%d\n", y);
						}
					}
				}
				else {
					for(ts_s32 y = Areainit[bit_to_execute - 1].Area_Ly; y < Areainit[bit_to_execute - 1].Area_Ry; y++){
						for (ts_s32 j = Areainit[bit_to_execute - 1].Area_Lx; j < Areainit[bit_to_execute - 1].Area_Rx; j++) {
							index =  (y * vot_sst_ctx.u16_width) + j;
							ts_u16 tmp_value = (pu16_ptr[index]) >> 2;
							
							if(tmp_value == 0x0000) {
								continue;
							}
							Area_valid_pixels++;
							if(maxTemp < tmp_value) {
								maxTemp = tmp_value;
							}
							if(minTemp > tmp_value) {
								minTemp = tmp_value;
							}
							sumTemp += tmp_value;
						}
					}
				}
			}
			else {
				if(UsermenuParameter.Mirror == 1){ //X镜像
					 Mirr_AreaLx = (new_width - 1) - Areainit[bit_to_execute - 1].Area_Rx;
					 Mirr_AreaRx = (new_width - 1) - Areainit[bit_to_execute - 1].Area_Lx;
					 Mirr_AreaLy = Areainit[bit_to_execute - 1].Area_Ly;
					 Mirr_AreaRy = Areainit[bit_to_execute - 1].Area_Ry;
				}else if(UsermenuParameter.Mirror == 2){ //Y镜像
					 Mirr_AreaLy = (new_height - 1) - Areainit[bit_to_execute - 1].Area_Ry;
					 Mirr_AreaRy = (new_height - 1) - Areainit[bit_to_execute - 1].Area_Ly;
					 Mirr_AreaLx = Areainit[bit_to_execute - 1].Area_Lx;
					 Mirr_AreaRx = Areainit[bit_to_execute - 1].Area_Rx;
				}else if(UsermenuParameter.Mirror == 3){ //XY镜像
					 Mirr_AreaLx = (new_width - 1) - Areainit[bit_to_execute - 1].Area_Rx;
					 Mirr_AreaLy = (new_height - 1) - Areainit[bit_to_execute - 1].Area_Ry;
					 Mirr_AreaRx = (new_width - 1) - Areainit[bit_to_execute - 1].Area_Lx;
					 Mirr_AreaRy = (new_height - 1) - Areainit[bit_to_execute - 1].Area_Ly;
				}
				if (s32_nPos >= TS_UPL_POS_VPO_YUV) {
					for(ts_s32 y = Mirr_AreaLy; y < Mirr_AreaRy; y++){
						ts_u16 vOffest = get_skip_count(y);
						if (vOffest > 0) {
							for (ts_s32 j = Mirr_AreaLx; j < Mirr_AreaRx; j++) {
								index =  (y * vot_sst_ctx.u16_width) + j + vOffest;
								ts_u16 tmp_value = swap_16(pu16_ptr[index]);
								/* */
								if(tmp_value == 0x0000) {
									continue;
								}
								Area_valid_pixels++;
								if(maxTemp < tmp_value) {
									maxTemp = tmp_value;
								}
								if(minTemp > tmp_value) {
									minTemp = tmp_value;
								}
								sumTemp += tmp_value;
							}
						}
						else {
							vision_log_warn("row Value is ERROR! y=%d\n", y);
						}
					}
				}
				else {
					for(ts_s32 y = Mirr_AreaLy; y < Mirr_AreaRy; y++){
						for (ts_s32 j = Mirr_AreaLx; j < Mirr_AreaRx; j++) {
							index =  (y * vot_sst_ctx.u16_width) + j;
							ts_u16 tmp_value = (pu16_ptr[index]) >> 2;
							
							if(tmp_value == 0x0000) {
								continue;
							}
							Area_valid_pixels++;
							if(maxTemp < tmp_value) {
								maxTemp = tmp_value;
							}
							if(minTemp > tmp_value) {
								minTemp = tmp_value;
							}
							sumTemp += tmp_value;
						}
					}
				}
			}
			if (Area_valid_pixels > 0) {
				avg_value = sumTemp / Area_valid_pixels;
			}
			else {
				avg_value = 0;
				minTemp = 0;
				maxTemp = 0;
				vision_log_warn("No valid pixels in Area %d\n", bit_to_execute-1);
			}
			Areainit[bit_to_execute-1].Area_temp_AVG = (ts_u16)avg_value;
			Areainit[bit_to_execute-1].Area_temp_MAX = maxTemp;
			Areainit[bit_to_execute-1].Area_temp_MIN = minTemp;
			// vision_log_notice("npos=%d avg=0x%llx min=0x%x max=0x%x valid=0x%x\n", s32_nPos, (unsigned long long)avg_value, minTemp, maxTemp, Area_valid_pixels);
		}
	}
	/*------------- 全屏捕捉点相关计算 -------------*/
	if (UsermenuParameter.CaptureFlag) {
		ts_u16 maxTemp = 0, minTemp = 0x3fff;
		ts_u16 max_x = 0, max_y = 0;
		ts_u16 min_x = 0, min_y = 0;
		
		if (s32_nPos >= TS_UPL_POS_VPO_YUV) {
			for(ts_s32 y = CAPTUREBOUNDARY; y < new_height-CAPTUREBOUNDARY; y++){
				ts_u16 vOffest = get_skip_count(y);
				if (vOffest > 0) {
					for (ts_s32 j = CAPTUREBOUNDARY; j < new_width-CAPTUREBOUNDARY; j++) {
						index =  (y * vot_sst_ctx.u16_width) + j + vOffest;
						ts_u16 tmp_value = swap_16(pu16_ptr[index]);
						if(tmp_value == 0x0000) {
							continue;
						}
						if(maxTemp < tmp_value) {
							maxTemp = tmp_value;
							max_x = j;
							max_y = y;
						}
						if(minTemp > tmp_value) {
							minTemp = tmp_value;
							min_x = j;
							min_y = y;
						}
					}
				}
			}
		}
		else {
			for(ts_s32 y = CAPTUREBOUNDARY; y < (new_height - CAPTUREBOUNDARY); y++){
				for (ts_s32 j = CAPTUREBOUNDARY; j < (new_width - CAPTUREBOUNDARY); j++) {
					index =  (y * vot_sst_ctx.u16_width) + j;
					ts_u16 tmp_value = (pu16_ptr[index]) >> 2;
					/* */
					if(tmp_value == 0x0000) {
						continue;
					}
					if(maxTemp < tmp_value) {
						maxTemp = tmp_value;
						max_x = j;
						max_y = y;
					}
					if(minTemp > tmp_value) {
						minTemp = tmp_value;
						min_x = j;
						min_y = y;
					}
				}
			}
		}
		if(UsermenuParameter.Mirror == 1){ //X镜像
			max_x = (new_width - 1) - max_x;
			min_x = (new_width - 1) - min_x;
		}else if(UsermenuParameter.Mirror == 2){ //Y镜像
			max_y = (new_height - 1) - max_y;
			min_y = (new_height - 1) - min_y;
		}else if(UsermenuParameter.Mirror == 3){ //XY镜像
			max_x = (new_width - 1) - max_x;
			min_x = (new_width - 1) - min_x;
			max_y = (new_height - 1) - max_y;
			min_y = (new_height - 1) - min_y;
		}
		spot_max.spot_X_Pos = max_x;
		spot_max.spot_Y_Pos = max_y;
		spot_max.spot_AD = maxTemp;

		spot_min.spot_X_Pos = min_x;
		spot_min.spot_Y_Pos = min_y;
		spot_min.spot_AD = minTemp;
		// vision_log_notice("spot_maxTemp = 0x%x, maxTemp = 0x%x, spot_minTemp = 0x%x, minTemp = 0x%x\n", spot_max.spot_AD, maxTemp, spot_min.spot_AD, minTemp);
	}
	return TS_OK;
}
/************************* End ********************************/

int16_t	Temp_AllCorrect(int16_t Temp0)
{
	if(Temp0>250)
	{
		Temp0 = Tamb_TiEmiss_Correct(Temp0);
		Temp0 = Distance_Correct(Temp0);
	}
	else
	{
		Temp0 = Tamb_TiEmiss_Correct(Temp0);
	}
//	bsp_printf("SpotTempCorrect1= %x\n",Temp0 );
	return Temp0;
}
/********************************************************
 * UVC接口输出RAW+YUV图像时，温度帧开启/禁用功能
 *******************************************************/
#define LUT_SIZE 		4096
#define POINT_COUNT 	12  // 数组长度均为12

/** @note AD<--->Temp 线性插值算法函数 */
static ts_void generate_complete_lut(uint16_t* hex_array, int16_t* signed_array, ts_u16* pstrLut_table, ts_u08 v_settings) {
    /* 1、计算数据点 */
    typedef struct {
        uint16_t index;
        uint16_t value;
    } DataPoint;
    
    DataPoint points[POINT_COUNT];

    for (int i = 0; i < POINT_COUNT; i++) {
        // 计算索引
        points[i].index = hex_array[i] / 4;
        
        // 计算值
		int32_t temp_val;
		if (v_settings == 1) {
			temp_val = (int32_t)(signed_array[i] + 40) * 100;
		}
		else if (v_settings == 2) {
			temp_val = (int32_t)(signed_array[i] + 40) * 10;
		}
		else if (v_settings == 3) {
			temp_val = (int32_t)(signed_array[i] + 40) * 10;
		}
        if (temp_val < 0) temp_val = 0;
        if (temp_val > 65535) temp_val = 65535;
        points[i].value = (uint16_t)temp_val;
        vision_log_debug("point[%2d]: hex=0x%04X -> index=%4d, signed=%5d -> Value=%5d\n", 
                        i, hex_array[i], points[i].index, 
                        signed_array[i], points[i].value);
    }

    // 2、按索引排序
    for (int i = 0; i < POINT_COUNT - 1; i++) {
        for (int j = 0; j < POINT_COUNT - i - 1; j++) {
            if (points[j].index > points[j + 1].index) {
                DataPoint temp = points[j];
                points[j] = points[j + 1];
                points[j + 1] = temp;
            }
        }
    }
    // printf("\n排序后的数据点:\n");
    // printf("序号\t索引\t值\n");
    // for (int i = 0; i < POINT_COUNT; i++) {
    //     printf("%2d\t%4d\t%5d\n", i, points[i].index, points[i].value);
    // }

    // 3. 获取边界
    uint16_t min_index = points[0].index;
    uint16_t max_index = points[POINT_COUNT - 1].index;
    uint16_t min_value = points[0].value;
    uint16_t max_value = points[POINT_COUNT - 1].value;
    
    // 4. 生成LUT表
    int point_ptr = 0;

    for (uint16_t i = 0; i < LUT_SIZE; i++) {
        // 边界处理
        if (i < min_index) {
            pstrLut_table[i] = min_value;
        } else if (i > max_index) {
            pstrLut_table[i] = max_value;
        } 
        // 数据点本身
        else if (i == points[point_ptr].index) {
            pstrLut_table[i] = points[point_ptr].value;
        }
        // 在数据点之间
        else if (point_ptr < POINT_COUNT - 1 && i < points[point_ptr + 1].index) {
            uint16_t idx1 = points[point_ptr].index;
            uint16_t idx2 = points[point_ptr + 1].index;
            uint16_t val1 = points[point_ptr].value;
            uint16_t val2 = points[point_ptr + 1].value;
            
            // 线性插值
            uint32_t diff_idx = i - idx1;
            uint32_t diff_val = val2 - val1;
            uint32_t interval = idx2 - idx1;
            
            pstrLut_table[i] = (uint16_t)(val1 + (diff_idx * diff_val + interval/2) / interval);
        }
        // 移动到下一个区间
        else if (point_ptr < POINT_COUNT - 1 && i >= points[point_ptr + 1].index) {
            point_ptr++;
            i--;  // 重新处理当前索引
        }
    }
}
/*--------------------------------------
 * 测温矩阵模块化处理---》Start
----------------------------------------*/
// LUT 基础配置结构
typedef struct {
    ts_u16 mk;
    ts_u16 mb;
    ts_u16 b1;
    ts_u08 lut_update_en;
    ts_u32 enable;
    ts_u32 lut_base_addr;
    ts_u08 source_sel;
} temp_lut_cfg_t;

// AD_sensor偏移量还原
static ts_s16 temp_lut_calc_ad_sensor_offset(void) 
{
	ts_u16 sensor = Temp_Parameter.AD_sensor;
	/* bit15 = 符号 */
	if (sensor & 0x8000) {
		return -(ts_s16)(sensor & 0x7FFF);
	}
	else {
		return (ts_s16)(sensor & 0x7FFF);
	}
}

/* 温度矩阵 mb 计算统一入口
 * 标定模式 → offset = 0
 * 非标定模式 → 带符号 AD_sensor
**/
static ts_u16 temp_lut_calc_mb(ts_u32 ad_mean_u32, ts_bool calib_mode)
{
	ts_s16 offset = 0;
	ts_u16 Ret_mb = 0;
	if (!calib_mode)  {
		offset = temp_lut_calc_ad_sensor_offset();
	}else {
		offset = 0;
	}
	if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_HAND384) {
		ts_f32 float1 = (ts_f32)global_sys_ctrl_info.SetValue_INTtoMK/8192*(ts_f32)ad_mean_u32;
		ts_u16 Bakx = (ts_u16)float1;
		Ret_mb = (ts_u16)((ts_s32)0x1000 - Bakx + offset);
	}else{
		Ret_mb = (ts_u16)((ts_s32)0x1000 - (ts_s32)ad_mean_u32 + offset);
	}
	// return (ts_u16)((ts_s32)0x1000 - (ts_s32)ad_mean_u32 + offset);
	return Ret_mb;
}
/* LUT 数据填充, 输出测温AD */
static void temp_lut_fill_ad_raw(ts_u32 base_addr)
{
    ts_u16 *lut = (ts_u16 *)base_addr;

    for (ts_u32 i = 0; i < 4096; i++) {
        lut[i] = i * 4;
    }
}
/* LUT 数据填充, 温度查表 */
static void temp_lut_fill_temperature(ts_u08 temp_level) {
	ts_u16 *lut = (ts_u16 *)global_sys_ctrl_info.u32_lut_base_addr;

	ts_cpl_memset(lut, 0, 4096 * 2);
	switch (temp_level)
	{
		case 1: {
			if(Temp_Parameter.Second_CalibFlag == 1){ //二次标定
				generate_complete_lut(Temp_Parameter.Second_AD1, Temp_Parameter.Second_TEMP1, lut, 1);
			}else{
            generate_complete_lut(Temp_Parameter.AD_data1, Temp_Parameter.TEMP_data1, lut, 1);
			}
		}break;
		case 2: {
			if(Temp_Parameter.Second_CalibFlag == 1){ //二次标定
				generate_complete_lut(Temp_Parameter.Second_AD2, Temp_Parameter.Second_TEMP2, lut, 2);
			}else{
            generate_complete_lut(Temp_Parameter.AD_data2, Temp_Parameter.TEMP_data2, lut, 2);
			}
		}break;
		case 3: {
            generate_complete_lut(Temp_Parameter.AD_data3, Temp_Parameter.TEMP_data3, lut, 3);
		}break;
		default:break;
	}
}
static temp_disp_tyep_e temp_lut_get_mode(ts_u08 output_type) 
{
	/* 输出原始AD，即关闭温度矩阵表 */
	if (output_type == 0) {
		return TEMP_NORMAL_DISP_ORIAD;
	}
	/* 输出测温AD */
	if (output_type == 1) {
		return TEMP_NORMAL_DISP_LUT_AD;
	}
	/* 输出标定所需测温AD */
	if (output_type == 3) {
		return TEMP_CALIBRATION_DISP_AD;
	}
	/* 默认：输出查表温度 */
	return TEMP_NORMAL_DISP_TEMP;
}
/* 温度矩阵统一接口 */
static void temp_lut_apply(const temp_lut_cfg_t *cfg)
{
	ts_vpo_templut_attr_s_t attr;
	ts_cpl_memset(&attr, 0, sizeof(attr));

    attr.u16_mk = cfg->mk;
    attr.u16_mb = cfg->mb;
    attr.u16_b1 = cfg->b1;
    attr.u08_lut_update_en = cfg->lut_update_en;
    attr.u32_enable = cfg->enable;
    attr.u32_lut_base_addr = cfg->lut_base_addr;
    attr.u08_source_sel = cfg->source_sel;

    ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0, &attr);	
}
void vot_update_templut_table(ts_u08 vEnable_type, ts_u32 ad_mean_u32) {
	static ts_u08 last_temp_level = 0xff;
	temp_lut_cfg_t cfg;
	temp_disp_tyep_e output_mode = temp_lut_get_mode(vEnable_type);

	ts_cpl_memset(&cfg, 0, sizeof(cfg));
    // cfg.mk = 8192;
	cfg.mk = FUN_Return_MK(SystemParaMenu2.m_temp_regularity);
    cfg.b1 = 0;
    cfg.lut_base_addr = global_sys_ctrl_info.u32_lut_base_addr;
    cfg.source_sel = TS_LUT_POS_BEFORE_AGC;
    cfg.enable = 1;
    cfg.lut_update_en = 1;

	switch (output_mode) {
		case TEMP_NORMAL_DISP_LUT_AD: 
			cfg.mb = temp_lut_calc_mb(ad_mean_u32, 0);
			temp_lut_fill_ad_raw(cfg.lut_base_addr);
			break;
		case TEMP_NORMAL_DISP_TEMP: 
            if (global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag || last_temp_level != SystemParaMenu2.Usermenu_SW) {
				vision_log_debug("last_temp_level = %d, SystemParaMenu2.Usermenu_SW = %d\n", last_temp_level, SystemParaMenu2.Usermenu_SW);
                global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag = 0;
                last_temp_level = SystemParaMenu2.Usermenu_SW;
                temp_lut_fill_temperature(SystemParaMenu2.Usermenu_SW);
            }
            cfg.mb = temp_lut_calc_mb(ad_mean_u32, 0);
			break;
		case TEMP_CALIBRATION_DISP_AD: 
            // cfg.mb = 0x1000 - ad_mean_u32;
			cfg.mb = temp_lut_calc_mb(ad_mean_u32, 1);
            temp_lut_fill_ad_raw(cfg.lut_base_addr);
			break;
		case TEMP_NORMAL_DISP_ORIAD:
		default:
            cfg.enable = 0;
            cfg.lut_update_en = 0;
            cfg.mb = 0;
            break;		
	}
	temp_lut_apply(&cfg);
}

ts_u16 FUN_Return_MK(ts_u08 vEnable_type)
{
	ts_u16 MKVal = 0;
	if(vEnable_type == TEMP_REGULARITY_GENERAL || vEnable_type == TEMP_REGULARITY_QC)
	{
		MKVal = 8192;
	}else if(vEnable_type == TEMP_REGULARITY_HAND384)
	{
		MKVal = global_sys_ctrl_info.SetValue_INTtoMK;
	}else{
		MKVal = 8192;
	}
	return MKVal;
}
/***************************** End ***************************/

/********************************************************
 * 通过FIRE模块方式读取测温点/测温区域/整帧捕获功能
 * 暂定 1点2区域1捕获 功能
 *******************************************************/
/** @note FIRE模块读取AD */
static ts_s32 vot_fire_get_ispfrmData(void) {
    ts_s32 s32_ret;
    ts_isp_fire_attr_s_t set_attr = {0};

	if (Spot_state == 0 && (Area_state&0x0f) == 0 && UsermenuParameter.CaptureFlag == 0) {
		return TS_ERR;
	}
	
    // 设置自动分块使能和尺寸
    set_attr.u08_enable = 1;            /*使能，0 关闭， 1，开启*/  
    set_attr.u08_blk_size_w = 32;       /*横向块大小*/
    set_attr.u08_blk_size_h = 32;       /*纵向块大小*/

    // 设置手动分块区域
	/* 区域1 */
	if (Area_state & 0x01) {
		set_attr.ast_rect[0].b_enable = 1;
		set_attr.ast_rect[0].u16_start_x = Areainit[0].Area_Lx;
		set_attr.ast_rect[0].u16_start_y = Areainit[0].Area_Ly;
		set_attr.ast_rect[0].u16_end_x = Areainit[0].Area_Rx;
		set_attr.ast_rect[0].u16_end_y = Areainit[0].Area_Ry;
	}
	else {
		set_attr.ast_rect[0].b_enable = 0;
	}
	/* 区域2 */
	if ((Area_state >> 1) & 0x01) {
		set_attr.ast_rect[1].b_enable = 1;
		set_attr.ast_rect[1].u16_start_x = Areainit[1].Area_Lx;
		set_attr.ast_rect[1].u16_start_y = Areainit[1].Area_Ly;
		set_attr.ast_rect[1].u16_end_x = Areainit[1].Area_Rx;
		set_attr.ast_rect[1].u16_end_y = Areainit[1].Area_Ry;
	}
	else {
		set_attr.ast_rect[1].b_enable = 0;
	}
	set_attr.ast_rect[2].b_enable = 0;

    // 设置坐标点
	set_attr.ast_pos[0].u16_pos_x = 0;
	set_attr.ast_pos[0].u16_pos_y = 0;
	if (Spot_state & 0x01) {
		if ((SpotInit[0].spot_X_Pos <= 640 && SpotInit[0].spot_X_Pos >= 0) && (SpotInit[0].spot_Y_Pos <= 512 && SpotInit[0].spot_Y_Pos >= 0)) 
		{
			set_attr.ast_pos[0].u16_pos_x = SpotInit[0].spot_X_Pos;
			set_attr.ast_pos[0].u16_pos_y = SpotInit[0].spot_Y_Pos;			
		}
	}

    // 调用 set 函数
    s32_ret = ts_tdk_isp_set_fire_attr(TS_ISP_MDEV0, &set_attr);
    if (s32_ret != 0) {
        vision_log_warn("Set function failed! Error: %d\n", s32_ret);
		return s32_ret;
    }
/*
    // 调用 get 函数
    s32_ret = ts_tdk_isp_get_fire_attr(TS_ISP_MDEV0, &get_attr);
    if (s32_ret != 0) {
        vision_log_warn("Get function failed! Error: %d\n", s32_ret);
		return s32_ret;
    }

    //验证配置是否生效
    if (memcmp(&set_attr, &get_attr, sizeof(ts_isp_fire_attr_s_t)) != 0) {
        vision_log_warn("Data mismatch between set and get!\n");
		return s32_ret;
    }
*/
    ts_isp_fire_statis_s_t fire_statis;
    memset(&fire_statis, 0, sizeof(ts_isp_fire_statis_s_t));
    
    // 测试获取数据
    s32_ret = ts_tdk_isp_get_fire_statis(TS_ISP_MDEV0, &fire_statis, 1000);
    if (s32_ret != 0) {
        vision_log_warn("Error: Get fire statis failed! (ret=%d)\n", s32_ret);
		return s32_ret;
    }
	// 整帧捕获
    if (UsermenuParameter.CaptureFlag) {
		spot_max.spot_X_Pos = fire_statis.st_frm_info.u16_max_pos_x;
		spot_max.spot_Y_Pos = fire_statis.st_frm_info.u16_max_pos_y;
		spot_max.spot_AD = fire_statis.st_frm_info.u16_max_val;

		spot_min.spot_X_Pos = fire_statis.st_frm_info.u16_min_pos_x;
		spot_min.spot_Y_Pos = fire_statis.st_frm_info.u16_min_pos_y;
		spot_min.spot_AD = fire_statis.st_frm_info.u16_min_val;		
	}
	// 2个测温区域
	if (Area_state & 0x01) {
		ts_isp_fire_man_info_s_t *man_info = &fire_statis.ast_man_info[0];
		Areainit[0].Area_temp_AVG = man_info->u16_mean;
		Areainit[0].Area_temp_MAX = man_info->st_blk_info.u16_max_val;
		Areainit[0].Area_temp_MIN = man_info->st_blk_info.u16_min_val;
	}
	if ((Area_state >> 1) & 0x01) {
		ts_isp_fire_man_info_s_t *man_info = &fire_statis.ast_man_info[1];
		Areainit[1].Area_temp_AVG = man_info->u16_mean;
		Areainit[1].Area_temp_MAX = man_info->st_blk_info.u16_max_val;
		Areainit[1].Area_temp_MIN = man_info->st_blk_info.u16_min_val;
	}

	// 测温点
	if (Spot_state & 0x01) {
		if ((SpotInit[0].spot_X_Pos <= 640 && SpotInit[0].spot_X_Pos >= 0) && (SpotInit[0].spot_Y_Pos <= 512 && SpotInit[0].spot_Y_Pos >= 0)) {
			SpotInit[0].spot_AD = fire_statis.au16_pos_value[0];
		}
	}
    // 释放资源
    s32_ret = ts_tdk_isp_free_fire_statis(TS_ISP_MDEV0, &fire_statis);
    if (s32_ret != 0) {
        vision_log_warn("Error: Free fire statis failed! (ret=%d)\n", s32_ret);
    }
	return TS_OK;
}
/** @note 外部调用FIRE模块 */
ts_s32 votSafe_execute_FireModule(void) {
    ts_s32 ret = TS_ERR;
    
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_fireModule_get_ad_mutex);
    ret = vot_fire_get_ispfrmData();
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_fireModule_get_ad_mutex);
    
    return ret;
}
/***************************** End ***************************/

/********************************************************
 * 参数行，预留4行，后3行为空，第一行前77字节为有效值
 *******************************************************/
/**
 * @brief 将温度值转换为带符号位的十六进制显示
 * @param temp 实际温度值（摄氏度）
 * @return 16位十六进制值，最高位为符号位
 */
uint16_t temp_to_hex_with_sign(float temp) {
    // 乘以10并四舍五入到整数
    int16_t temp_scaled = (int16_t)(temp * 10.0f + (temp > 0 ? 0.5f : -0.5f));
    // vision_log_notice("temp = %+0.1f, temp_scaled = %d\n", temp, temp_scaled);
    // 转换为16位无符号整数（保持二进制补码形式）
    return (uint16_t)temp_scaled;
}

/**
 * @brief 从版本字符串中提取1.0.00信息
 * @param version_str 版本字符串，格式："xxx.xxx.xxx.xxx.xxx.xxx.xxx"
 * @param major 输出参数：主版本号（单字节char）
 * @param minor 输出参数：次版本号（单字节char）
 * @param revision 输出参数：修订号（int16_t）
 * @return 0:成功, -1:参数错误, -2:解析失败
 */
int extract_version_info(const char *version_str, unsigned char *major, unsigned char *minor, int16_t *revision)
{
    char major_str[4] = {0};
    char minor_str[4] = {0};
    char rev_str[4] = {0};
    
    // 参数检查
    if (version_str == NULL || major == NULL || minor == NULL || revision == NULL) {
        return -1;
    }
    
    // 解析版本字符串：跳过第一个字段，提取第2、3、4个字段
    // 格式：000.1.0.00.100000.000.000
    if (sscanf(version_str, "%*[^.].%3[^.].%3[^.].%3[^.]", 
               major_str, minor_str, rev_str) != 3) {
        return -2;  // 解析失败
    }
    
    // 转换为目标类型
    *major = (unsigned char)atoi(major_str);     // "1" -> 1
    *minor = (unsigned char)atoi(minor_str);     // "0" -> 0
    *revision = (int16_t)atoi(rev_str);           // "00" -> 0
    
    return 0;  // 成功
}
/** @note 参数行日志打印函数 */
static void __attribute__((unused)) dump_rowData_buff_safe(const ts_u08 *buf, int len)
{
    char log_buf[1024];
    int offset = 0;

    offset += snprintf(log_buf + offset, sizeof(log_buf) - offset, "rowData_buff:\n");
    for (int i = 0; i < len; i++) {
        offset += snprintf(log_buf + offset, sizeof(log_buf) - offset, "0x%02x ", buf[i]);

        if ((i % 20 == 19) || (i == len - 1)) {
            offset += snprintf(log_buf + offset, sizeof(log_buf) - offset, "\n");
        }
        /* 防止溢出 */
        if (offset >= (int)sizeof(log_buf) - 16) {
            vision_log_debug("WARNING: dump truncated\n");
            break;
        }
    }
    /* 保证结尾换行 */
    if (offset > 0 && log_buf[offset - 1] != '\n') {
        if (offset < (int)sizeof(log_buf) - 1) {
            log_buf[offset++] = '\n';
        } else {
            log_buf[sizeof(log_buf) - 2] = '\n';
        }
    }
    vision_log_debug("%s", log_buf);
}
/** @note 填充参数行数据 */
ts_void vot_Set_imgParam_row_data(ts_u08 mode, ts_u08 lens) {
	ts_u08 rowData_buff[0x90] = {0};

	/* 1、设置数据类型 */
	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_NONE) {
		rowData_buff[0x00] = 0x01;
		rowData_buff[0x01] = 0x02;
	}
	else {
		if (SystemParaMenu2.m_user_tempFrame_Mode == TEMP_NORMAL_DISP_ORIAD) {
			rowData_buff[0x00] = 0x01;
			rowData_buff[0x01] = 0x02;
		}
		else if (SystemParaMenu2.m_user_tempFrame_Mode == TEMP_NORMAL_DISP_LUT_AD) {
			rowData_buff[0x00] = 0x01;
			rowData_buff[0x01] = 0x03;
		}
		else if (SystemParaMenu2.m_user_tempFrame_Mode == TEMP_NORMAL_DISP_TEMP) {
			rowData_buff[0x00] = 0x01;
			rowData_buff[0x01] = 0x04;
		}
		else if (SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
			rowData_buff[0x00] = 0x01;
			rowData_buff[0x01] = 0x01;
		}
	}
	/* 2、机型 */
	rowData_buff[0x02] = 0xf1;
	rowData_buff[0x03] = 0x55;
	rowData_buff[0x04] = 0xf2;
	rowData_buff[0x05] = 0x55;
	/* 3、FPGA版本 */
	rowData_buff[0x06] = 0x1A;
	rowData_buff[0x07] = 0x2A;
	rowData_buff[0x08] = 0x3A;
	rowData_buff[0x09] = 0x4A;
	/* 4、SOC版本 */
    unsigned char major_byte = 0;
    unsigned char minor_byte = 0;
    int16_t revision_int16 = 0;
	extract_version_info(VOT_SOFTWARE_VER_FIXED_HEADER, &major_byte, &minor_byte, &revision_int16);
	rowData_buff[0x0A] = major_byte;
	rowData_buff[0x0B] = minor_byte;
	rowData_buff[0x0C] = (uint8_t)((revision_int16 >> 8) & 0xFF);
	rowData_buff[0x0D] = (uint8_t)(revision_int16 & 0xFF);
	/* 5、红外接口长度 */
	rowData_buff[0x0E] = 0xCC;
	/* 6、温度表 AD表 */
	if (SystemParaMenu2.Usermenu_SW == 1) {
		if(Temp_Parameter.Second_CalibFlag == 1){
			for (int i = 0; i < 12; i++) {
				rowData_buff[0x0F+(i*2+1)] = (ts_u08)(Temp_Parameter.Second_TEMP1[i] & 0xFF);	// 温度表低字节
				rowData_buff[0x0F+(i*2)] = (ts_u08)((Temp_Parameter.Second_TEMP1[i] >> 8) & 0xFF); // 温度表高字节

				rowData_buff[0x27+(i*2+1)] = (ts_u08)(Temp_Parameter.Second_AD1[i] & 0xFF);	// AD表低字节
				rowData_buff[0x27+(i*2)] = (ts_u08)((Temp_Parameter.Second_AD1[i] >> 8) & 0xFF); // AD表高字节	
			}		
		}else{
		for (int i = 0; i < 12; i++) {
			rowData_buff[0x0F+(i*2+1)] = (ts_u08)(Temp_Parameter.TEMP_data1[i] & 0xFF);	// 温度表低字节
			rowData_buff[0x0F+(i*2)] = (ts_u08)((Temp_Parameter.TEMP_data1[i] >> 8) & 0xFF); // 温度表高字节

			rowData_buff[0x27+(i*2+1)] = (ts_u08)(Temp_Parameter.AD_data1[i] & 0xFF);	// AD表低字节
			rowData_buff[0x27+(i*2)] = (ts_u08)((Temp_Parameter.AD_data1[i] >> 8) & 0xFF); // AD表高字节			
		}
		}
	}
	else if (SystemParaMenu2.Usermenu_SW == 2) {
		if(Temp_Parameter.Second_CalibFlag == 1){
			for (int i = 0; i < 12; i++) {
				rowData_buff[0x0F+(i*2+1)] = (ts_u08)(Temp_Parameter.Second_TEMP2[i] & 0xFF);	// 温度表低字节
				rowData_buff[0x0F+(i*2)] = (ts_u08)((Temp_Parameter.Second_TEMP2[i] >> 8) & 0xFF); // 温度表高字节

				rowData_buff[0x27+(i*2+1)] = (ts_u08)(Temp_Parameter.Second_AD2[i] & 0xFF);	// AD表低字节
				rowData_buff[0x27+(i*2)] = (ts_u08)((Temp_Parameter.Second_AD2[i] >> 8) & 0xFF); // AD表高字节			
			}
		}else{
		for (int i = 0; i < 12; i++) {
			rowData_buff[0x0F+(i*2+1)] = (ts_u08)(Temp_Parameter.TEMP_data2[i] & 0xFF);	// 温度表低字节
			rowData_buff[0x0F+(i*2)] = (ts_u08)((Temp_Parameter.TEMP_data2[i] >> 8) & 0xFF); // 温度表高字节

			rowData_buff[0x27+(i*2+1)] = (ts_u08)(Temp_Parameter.AD_data2[i] & 0xFF);	// AD表低字节
			rowData_buff[0x27+(i*2)] = (ts_u08)((Temp_Parameter.AD_data2[i] >> 8) & 0xFF); // AD表高字节			
		}
		}
	}
	else if (SystemParaMenu2.Usermenu_SW == 3) {
		for (int i = 0; i < 12; i++) {
			rowData_buff[0x0F+(i*2+1)] = (ts_u08)(Temp_Parameter.TEMP_data3[i] & 0xFF);	// 温度表低字节
			rowData_buff[0x0F+(i*2)] = (ts_u08)((Temp_Parameter.TEMP_data3[i] >> 8) & 0xFF); // 温度表高字节

			rowData_buff[0x27+(i*2+1)] = (ts_u08)(Temp_Parameter.AD_data3[i] & 0xFF);	// AD表低字节
			rowData_buff[0x27+(i*2)] = (ts_u08)((Temp_Parameter.AD_data3[i] >> 8) & 0xFF); // AD表高字节			
		}
	}
	/* 7、补偿值 */
	rowData_buff[0x3F] = (ts_u08)(Temp_Parameter.AD_sensor & 0xFF);
	rowData_buff[0x40] = (ts_u08)((Temp_Parameter.AD_sensor >> 8) & 0xFF);
	/* 8、触发类型，序号 */
	rowData_buff[0x41] = 0xA1;
	rowData_buff[0x42] = 0xB2;
	rowData_buff[0x43] = 0xC3;
	/* 9、快门状态、心跳信号 */
	rowData_buff[0x44] = global_sys_ctrl_info.Iris_Current_State;
	rowData_buff[0x45] = global_sys_ctrl_info.Heartbeat_Flag;
	/* 10、快门温度 */
	ts_f32 vot_Temperature = READ_TMP112_Temp();
	uint16_t tmpAD = temp_to_hex_with_sign(vot_Temperature);
	rowData_buff[0x46] = (uint8_t)(tmpAD & 0xFF);
	rowData_buff[0x47] = (uint8_t)((tmpAD >> 8) & 0xFF);
	/* 11、DET温度 */
	ts_u16 temp_ad_value_u16 = 0;
	ts_f32 temp_value_f32 = 0;
	ISP_get_det_ad_info(&temp_value_f32, &temp_ad_value_u16);
	tmpAD = temp_to_hex_with_sign(temp_value_f32);
	rowData_buff[0x48] = (uint8_t)(tmpAD & 0xFF);
	rowData_buff[0x49] = (uint8_t)((tmpAD >> 8) & 0xFF);
	/* 12、sensor类型 */
	rowData_buff[0x4A] = SystemParaMenu2.sensor;
	/* 13、测温档位切换标志 */
	rowData_buff[0x4B] = global_sys_ctrl_info.Usermenu_SW_Switching;
	/* 14、防灼烧标志 */
	rowData_buff[0x4C] = global_sys_ctrl_info.prevent_burn_flag;
	/* 15、当前测温挡位 */
	rowData_buff[0x4D] = mode;
	/* 16、镜头类型 */
	rowData_buff[0x4E] = lens; //Temp_Parameter.Lens_Set;
	/* 17、距离修正表 0x4F - 0x8F */
	ts_u16 tmpValue = 0;
	ts_u08 modeLevel = mode - 1;
	for (int i = 0; i < DIST_CORR_MAX_DIST; i++) {
		tmpValue = global_sys_ctrl_info.g_dist_corr_tbl[modeLevel][lens][i].factor * 10000;
		rowData_buff[0x4F + (i * 2 + 1)] = (ts_u08)(tmpValue & 0xFF);
		rowData_buff[0x4F + (i * 2)] = (ts_u08)((tmpValue >> 8) & 0xFF);
	}
	/* 填充参数行数据 */
	vot_add_user_metadata_info(rowData_buff, 0x8F);
	/* 打印参数行数据 */
	// dump_rowData_buff_safe(rowData_buff, 0x8F);
}
/***************************** End ***************************/
/** @note 进入或退出PC测温标定 
 * 56 4F 54 00 06 00 AC FF EE 7B 05
*/
ts_void EnterORExit_PC_templut_Calib(void) {
	if (SystemParaMenu2.Usermenu_SW) {
		if (SystemParaMenu2.m_user_tempFrame_Mode != TEMP_CALIBRATION_DISP_AD) {
			ISP_cut();
			/* 测温工具进入标定 */
			SystemParaMenu2.m_temperCalibra_Flag = 1;
			SystemParaMenu2.AD_T = 3;
			/* RAW+YUV模式下进入标定 */
			SystemParaMenu2.m_user_tempFrame_Mode = TEMP_CALIBRATION_DISP_AD;
			if(Temp_Parameter.Second_CalibFlag == 1) { //二次标定
				SecondCalib_Parameter.CalibFlag = 1;
				Spot_state |= 0x01;
				SpotInit[0].spot_X_Pos = 320;
				SpotInit[0].spot_Y_Pos = 256;
			}
			UpdatePromptInfoWithTimeMs("CALIBRATION IN PROGRESS ...", 1000, VOT_LOG_TYPE_MENU2);
		}
		else {
			/* 测温工具退出标定 */
			SystemParaMenu2.m_temperCalibra_Flag = 0;
			SystemParaMenu2.AD_T = 2;
			/* RAW+YUV模式下退出标定 */
			SystemParaMenu2.m_user_tempFrame_Mode = TEMP_NORMAL_DISP_LUT_AD;
			ISP_cut();
			if(Temp_Parameter.Second_CalibFlag == 1) { //二次标定
				SecondCalib_Parameter.CalibFlag = 0;
			}
			UpdatePromptInfoWithTimeMs("MEASUREMENT MODE ...", 1000, VOT_LOG_TYPE_MENU2);
		}
	}
}

ts_s08 votLocal_TempAD_TO_RawAD(ts_u16 temp_ad, ts_u16* t_rawAD) {
		if (t_rawAD == ts_null) {
			return -1;
		}
        // ts_s16 temp_ad = (ts_s16)t_TempAD;

        /* offset 带符号：bit15=1 为负，bit15=0 为正 */
        ts_s16 offset = (ts_s16)(Temp_Parameter.AD_sensor & 0x7FFF);
        if (Temp_Parameter.AD_sensor & 0x8000)
            offset = -offset;

        ts_s16 bak_mean = (ts_s16)(global_sys_ctrl_info.m_cur_bak_ad_mean_u32 & 0x3FFF);
        ts_s32 raw_s32 = (ts_s32)temp_ad - 0x1000 - (ts_s32)offset + (ts_s32)bak_mean;

        /* 14bit 钳位（原始AD最大值 0x3FFF） */
        if (raw_s32 < 0)
		{
			return -1;
		}
        if (raw_s32 > 0x3FFF)
		{
			return -1;
		}

        *t_rawAD = (ts_u16)raw_s32;
		vision_log_notice("temp_ad=0x%x, offset=%x, bak_mean=0x%x, raw_ad=0x%x\n", temp_ad, offset, bak_mean, *t_rawAD);
		return 0;
}
#if 1
/** @note 距离修正系数这块，存在多种镜头时还需要优化代码结构*/
int16_t Distance_Correct(int16_t Temp)
{
	ts_u16 dist = (Temp_Parameter.distance / 10);
	ts_u08 mode = SystemParaMenu2.Usermenu_SW - 1;
	ts_u08 lens = Temp_Parameter.Lens_Set;

    // if (mode >= 3 || lens >= 15) {
	// 	return Temp;
	// }

	uint8_t blk_offset = 0;
	if (mode == 1) {
		blk_offset = (Temp > 2500) ? 1 : 0;
	}
	else if (mode == 0) {
		blk_offset = 0;
	}
	else {
		return Temp;	// mode 2 暂不支持
	}
	if (dist > (DIST_CORR_MAX_DIST/2)) {
		dist = 16;
	}
    ts_f32 factor = global_sys_ctrl_info.g_dist_corr_tbl[mode][lens][dist + blk_offset*(DIST_CORR_MAX_DIST/2)].factor;

    return (int16_t)((ts_f32)Temp * factor);
}
#else
int16_t Distance_Correct(int16_t Temp)
{
	ts_f32 TempData_f;
	ts_u16 DistanceVal;

	// TempData_f = (float)Temp*1;	
	// Temp = (int16_t)TempData_f;
	// return Temp;

	DistanceVal  = Temp_Parameter.distance/10;
	if(SystemParaMenu2.Usermenu_SW==1)  //一档测温距离修正
	{
		switch(DistanceVal) //距离m
		{
			case 0:
				TempData_f = (float)Temp*0.9875;
				break;
			case 1:
				// if(Temp_Parameter.Lens_Set==1)	//9.1mm	
				TempData_f = (float)Temp*0.9875; //9.1mm
				break;
			case 2:
				TempData_f = (float)Temp*1; //9.1mm
				break;
			case 3:
				TempData_f = (float)Temp*1.015; //9.1mm
				break;

			case 4:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.02; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.07; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.03; //18mm
				// else		Temp = (float)Temp*1; //其余镜头（暂定）
				TempData_f = (float)Temp*1.0217; //9.1mm
				break;
			case 5:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.01; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.04; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.09; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.04; //18mm
				// else		Temp = (float)Temp*1; //其余镜头（暂定）
				TempData_f = (float)Temp*1.0288; //9.1mm
				break;
			case 6:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.01; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.01; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.04; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.01; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.1; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.04; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.0288; //9.1mm
				break;
			case 7:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.01; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.02; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.05; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.01; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.04; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.0373; //9.1mm
				break;
			case 8:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.02; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.04; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm -8m外
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.02; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.04; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.04; //9.1mm
				break;
			case 9:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.02; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.05; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.01; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.02; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.04; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.05; //9.1mm
				break;
			case 10:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.02; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.01; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.01; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.04; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.05; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.059; //9.1mm
				break;
			case 11:
				// if(Temp_Parameter.Lens_Set==3)		Temp = (float)Temp*1.02; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.02; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.01; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.04; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.06; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.065; //9.1mm
				break;
			case 12:
				// if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.03; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.02; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.05; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.06; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.0729; //9.1mm
				break;
			case 13:
				// if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.02; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.05; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.06; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.083; //9.1mm
				break;
			case 14:
				// if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.04; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.05; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.06; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.0924; //9.1mm
				break;
			case 15:
				// if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.05; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.05; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1.01; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1.01; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.08; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.098; //9.1mm
				break;
			// case 16:
			// 	if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
			// 	else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
			// 	else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
			// 	else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
			// 	else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.05; //19mm
			// 	else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.06; //25mm
			// 	else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1.02; //35mm
			// 	else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1.02; //50mm
			// 	else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
			// 	else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.08; //18mm
			// 	else		Temp = (float)Temp*1;
			// 	break;
			// case 17:
			// 	if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
			// 	else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
			// 	else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
			// 	else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
			// 	else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.05; //19mm
			// 	else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.07; //25mm
			// 	else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1.02; //35mm
			// 	else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1.02; //50mm
			// 	else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
			// 	else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.08; //18mm
			// 	else		Temp = (float)Temp*1;
			// 	break;
			default:
				// if(Temp_Parameter.Lens_Set==3) 		Temp = (float)Temp*1.04; //15mm
				// else if(Temp_Parameter.Lens_Set==2)		Temp = (float)Temp*1.03; //13mm
				// else if(Temp_Parameter.Lens_Set==1)		Temp = (float)Temp*1.07; //9.1mm
				// else if(Temp_Parameter.Lens_Set==0)		Temp = (float)Temp*1.06; //4.6mm
				// else if(Temp_Parameter.Lens_Set==4)		Temp = (float)Temp*1.05; //19mm
				// else if(Temp_Parameter.Lens_Set==5)		Temp = (float)Temp*1.07; //25mm
				// else if(Temp_Parameter.Lens_Set==7)		Temp = (float)Temp*1.03; //35mm
				// else if(Temp_Parameter.Lens_Set==9)		Temp = (float)Temp*1.03; //50mm
				// else if(Temp_Parameter.Lens_Set==14)	Temp = (float)Temp*1.13; //5.7mm
				// else if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.08; //18mm
				// else		Temp = (float)Temp*1;
				TempData_f = (float)Temp*1.098; //9.1mm
				break;
		}
	}
	else if(SystemParaMenu2.Usermenu_SW==2)  //二档测温距离修正
	{
		//TempData_f = (float)Temp*1;
//#if 0
		//switch(DistanceVal) //距离m
		if(Temp>=2500)
		{ 
			switch(DistanceVal) //距离m
			{
				case 0:
					//Temp = (float)Temp*1;
					TempData_f = (float)Temp*0.992;
					break;
				case 1: 
				//Temp = (float)Temp*1;
					TempData_f = (float)Temp*0.992;
					break;
				
				case 2:
					TempData_f = (float)Temp*1.025;                
					break;
				case 3:
					TempData_f = (float)Temp*1.05;   
					break;

				case 4:
					TempData_f = (float)Temp*1.063;   
					break;
				case 5:
					TempData_f = (float)Temp*1.076;   
					break;
				case 6:
					TempData_f = (float)Temp*1.082;   
					break;
				case 7:
					TempData_f = (float)Temp*1.09;   
					break;
				case 8:
					TempData_f = (float)Temp*1.1;   
					break;
				case 9:
					TempData_f = (float)Temp*1.115;   
					break;
				case 10:
					TempData_f = (float)Temp*1.125;   
					break;
				case 11:
					TempData_f = (float)Temp*1.131;   
					break;
				case 12:
					TempData_f = (float)Temp*1.139;   
					break;
				case 13:
					TempData_f = (float)Temp*1.152;   
					break;
				case 14:
					TempData_f = (float)Temp*1.162;   
					break;
				case 15:
					TempData_f = (float)Temp*1.18;   
					break;
					#if 0
				case 16:
					if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
					else		Temp = (float)Temp*1;
					break;
				case 17:
					if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
					else		Temp = (float)Temp*1;
					break;
					#endif
				
				default:
					#if 0
					if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
					else		Temp = (float)Temp*1;
					#endif
					TempData_f = (float)Temp*1.18;
					break;
			}
		}
		else
		{
			switch(DistanceVal) //距离m
			{
			case 0:
				//Temp = (float)Temp*1;
				TempData_f = (float)Temp*0.992;
				break;
			case 1: 
			//Temp = (float)Temp*1;
				TempData_f = (float)Temp*0.992;
				break;
			
			case 2:
				TempData_f = (float)Temp*1.006;                
				break;
			case 3:
				TempData_f = (float)Temp*1.02;   
				break;

			case 4:
				TempData_f = (float)Temp*1.032;   
				break;
			case 5:
				TempData_f = (float)Temp*1.051;   
				break;
			case 6:
				TempData_f = (float)Temp*1.056;   
				break;
			case 7:
				TempData_f = (float)Temp*1.061;   
				break;
			case 8:
				TempData_f = (float)Temp*1.068;   
				break;
			case 9:
				TempData_f = (float)Temp*1.077;   
				break;
			case 10:
				TempData_f = (float)Temp*1.083;   
				break;
			case 11:
				TempData_f = (float)Temp*1.089;   
				break;
			case 12:
				TempData_f = (float)Temp*1.098;   
				break;
			case 13:
				TempData_f = (float)Temp*1.109;   
				break;
			case 14:
				TempData_f = (float)Temp*1.112;   
				break;
			case 15:
				TempData_f = (float)Temp*1.125;   
				break;
			#if 0
			case 16:
				if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
				else		Temp = (float)Temp*1;
				break;
			case 17:
				if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
				else		Temp = (float)Temp*1;
				break;
			#endif
			
			default:
				#if 0
				if(Temp_Parameter.Lens_Set==15)	Temp = (float)Temp*1.1; //18mm
				else		Temp = (float)Temp*1;
				#endif
				TempData_f = (float)Temp*1.125;
				break;
			}
		}
//#endif
	}

	Temp = (int16_t)TempData_f;
	return Temp;
}
#endif
/***********************************************/
/****************二次标定相关函数*****************/
/***********************************************/

void SecondCalib_Parameter_Init(void)
{
	SecondCalib_Parameter.CalibMode = 0;
	SecondCalib_Parameter.CalibFlag = 0;
	SecondCalib_Parameter.Global_offsetFlag = 0;
	SecondCalib_Parameter.CalibAD = 0;
	SecondCalib_Parameter.CalibTemp = 0;
	vision_log_notice("========SecondCalib_Parameter Init!! CalibFlag = [%d] \n",Temp_Parameter.Second_CalibFlag);
}

void SecondTemp_Autocalibration(void)
{
	if(!SecondCalib_Parameter.CalibFlag){
		if(SecondCalib_Parameter.CalibMode)	{
			vision_log_notice("SecondCalib Exit!!\n");
			UpdatePromptInfo(" ", 0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfoWithTimeMs("Autocalibration exit.",1500,VOT_LOG_TYPE_MENU2);
			SecondCalib_Parameter.CalibMode = 0;
		}
		return;
	}
	switch(SecondCalib_Parameter.CalibMode)
	{
		case 1:  //30度标定状态
			UpdatePromptInfo("The First data Collecting...",1,VOT_LOG_TYPE_MENU2);
			ISP_cut();
			ts_cpl_fix_msleep(2000);
			if(SystemParaMenu2.Usermenu_SW == 1){
				Temp_Parameter.Second_AD1[4] = SecondCalib_Parameter.CalibAD;}
			else if(SystemParaMenu2.Usermenu_SW == 2){
				Temp_Parameter.Second_AD2[4] = SecondCalib_Parameter.CalibAD;}
			vision_log_notice("SecondCalib_CalibAD[4] = [0x%x] \n", SecondCalib_Parameter.CalibAD);
			UpdatePromptInfo("The First data Collecting...",0,VOT_LOG_TYPE_MENU2);
			// UpdatePromptInfoWithTimeMs("Collection completed.",1500);
			// ts_cpl_fix_msleep(300);
			UpdatePromptInfo("Please collect the Second black data.", 1,VOT_LOG_TYPE_MENU2);
			SecondCalib_Parameter.CalibMode = 5;
			break;

		case 2:  // 70/140度标定状态
			UpdatePromptInfo("Please collect the Second black data.", 0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfo("The Second data Collecting...",1,VOT_LOG_TYPE_MENU2);
			if(SystemParaMenu2.Usermenu_SW == 1){
				Temp_Parameter.Second_AD1[6] = SecondCalib_Parameter.CalibAD;}
			else if(SystemParaMenu2.Usermenu_SW == 2){
				Temp_Parameter.Second_AD2[6] = SecondCalib_Parameter.CalibAD;}
			vision_log_notice("SecondCalib_CalibAD[6] = [0x%x] \n", SecondCalib_Parameter.CalibAD);
			UpdatePromptInfo("The Second data Collecting...",0,VOT_LOG_TYPE_MENU2);
			// UpdatePromptInfoWithTimeMs("Collection completed.",1500);
			UpdatePromptInfo("Please collect the Third black data.", 1,VOT_LOG_TYPE_MENU2);
			SecondCalib_Parameter.CalibMode = 5;
			break;

		case 3:  // 150/600度标定状态
			UpdatePromptInfo("Please collect the Third black data.", 0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfo("The Third data Collecting...",1,VOT_LOG_TYPE_MENU2);
			if(SystemParaMenu2.Usermenu_SW == 1){
				Temp_Parameter.Second_AD1[8] = SecondCalib_Parameter.CalibAD;}
			else if(SystemParaMenu2.Usermenu_SW == 2){
				Temp_Parameter.Second_AD2[7] = SecondCalib_Parameter.CalibAD;}
			vision_log_notice("SecondCalib_CalibAD[8] = [0x%x] \n", SecondCalib_Parameter.CalibAD);
			UpdatePromptInfo("The Third data Collecting...",0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfo("Collection completed And Saving...",1,VOT_LOG_TYPE_MENU2);
			SecondCalib_Parameter.CalibMode = 4;
			break;
		
		case 4:  //标定完成状态
			if(SystemParaMenu2.Usermenu_SW == 1)
			{
				Temp_Parameter.Second_TEMP1[0] = -30;
				Temp_Parameter.Second_TEMP1[1] = -20;
				Temp_Parameter.Second_TEMP1[2] = -10;
				Temp_Parameter.Second_TEMP1[3] = 0;
				Temp_Parameter.Second_TEMP1[4] = 30;
				Temp_Parameter.Second_TEMP1[5] = 40;
				Temp_Parameter.Second_TEMP1[6] = 70;
				Temp_Parameter.Second_TEMP1[7] = 100;
				Temp_Parameter.Second_TEMP1[8] = 150;
				Temp_Parameter.Second_TEMP1[9] = 160;
				Temp_Parameter.Second_TEMP1[10] = 170;
				Temp_Parameter.Second_TEMP1[11] = 180;
				Temp_Parameter.Second_correct1 = 0;
			}else if(SystemParaMenu2.Usermenu_SW == 2)
			{
				Temp_Parameter.Second_TEMP2[0] = -30;
				Temp_Parameter.Second_TEMP2[1] = -20;
				Temp_Parameter.Second_TEMP2[2] = -10;
				Temp_Parameter.Second_TEMP2[3] = 0;
				Temp_Parameter.Second_TEMP2[4] = 30;
				Temp_Parameter.Second_TEMP2[5] = 70;
				Temp_Parameter.Second_TEMP2[6] = 140;
				Temp_Parameter.Second_TEMP2[7] = 300;
				Temp_Parameter.Second_TEMP2[8] = 600;
				Temp_Parameter.Second_TEMP2[9] = 650;
				Temp_Parameter.Second_TEMP2[10] = 700;
				Temp_Parameter.Second_TEMP2[11] = 750;
				Temp_Parameter.Second_correct2 = 0;
			}
			SecondCalib_Cal(SystemParaMenu2.Usermenu_SW);
			Write_TempParameter_cfg(&Temp_Parameter);
			SecondCalib_Parameter.CalibMode = 0;
			EnterORExit_PC_templut_Calib();
			vision_log_notice("SecondCalib_Parameter Finish!!\n");
			UpdatePromptInfo("Collection completed And Saving...",0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfoWithTimeMs("Calibration Data save completed.", 2000,VOT_LOG_TYPE_MENU2);
			break;

		default:
				break;
	}
}

void SecondCalib_Offset_Function(void)
{
	static ts_u08 CalNum = 0;
	ts_s16 tmptemp;
	if((SecondCalib_Parameter.Global_offsetFlag == 0)|| SecondCalib_Parameter.CalibMode)
	{
		return;
	}
	switch(SecondCalib_Parameter.Global_offsetFlag)
	{
		case 1:
			if(SystemParaMenu2.AD_T != 2)	{break;}
			UpdatePromptInfo("The Global Offset Setting...",1,VOT_LOG_TYPE_MENU2);
			ISP_cut_new();
			if(SystemParaMenu2.Usermenu_SW == 1){
				Temp_Parameter.Second_correct1 = 0;
			}else if(SystemParaMenu2.Usermenu_SW == 2){
				Temp_Parameter.Second_correct2 = 0;
			}
			ts_cpl_fix_msleep(2000);
			SecondCalib_Parameter.Global_offsetFlag = 2;
			vision_log_notice("SecondCalib_Parameter Global_offsetFlag 1111\n");
			break;
		case 2:
			if(SystemParaMenu2.Usermenu_SW == 1){
				if(SecondCalib_Parameter.CalibTemp > 302)		{SecondCalib_Parameter.Global_offsetFlag = 3;}
				else if(SecondCalib_Parameter.CalibTemp < 298)	{SecondCalib_Parameter.Global_offsetFlag = 4;}
				else{
					CalNum++;
					if(CalNum>5)	SecondCalib_Parameter.Global_offsetFlag = 5;
				}
			}else if(SystemParaMenu2.Usermenu_SW == 2){
				if(SecondCalib_Parameter.CalibTemp > 302)		{SecondCalib_Parameter.Global_offsetFlag = 3;}
				else if(SecondCalib_Parameter.CalibTemp < 298)	{SecondCalib_Parameter.Global_offsetFlag = 4;}
				else{
					CalNum++;
					if(CalNum>5)	SecondCalib_Parameter.Global_offsetFlag = 5;
				}
			}
			vision_log_notice("SecondCalib_Parameter Global_offsetFlag 2222,CalibTemp =[%d],Num =[%d]\n", SecondCalib_Parameter.CalibTemp,CalNum);
			ts_cpl_fix_msleep(500);
			break;
		case 3:
			tmptemp = SecondCalib_Parameter.CalibTemp;
			if(CalNum)	{CalNum = 0;}
			if(SystemParaMenu2.Usermenu_SW == 1) {
				if(tmptemp <= 302)	{SecondCalib_Parameter.Global_offsetFlag = 5;break;}
				Temp_Parameter.Second_correct1 -= 2;
			}
			else if(SystemParaMenu2.Usermenu_SW == 2) {
				if(tmptemp <= 302)	{SecondCalib_Parameter.Global_offsetFlag = 5;break;}
				Temp_Parameter.Second_correct2 -= 2;
			}
			vision_log_notice("SecondCalib offsetFlag 3333,tmptemp =[%d],correct1=[%d], correct2=[%d]\n", tmptemp, Temp_Parameter.Second_correct1,Temp_Parameter.Second_correct2);
			ts_cpl_fix_msleep(2000);
			break;
		case 4:
			tmptemp = SecondCalib_Parameter.CalibTemp;
			if(CalNum)	{CalNum = 0;}
			if(SystemParaMenu2.Usermenu_SW == 1) {
				if(tmptemp >= 298)	{SecondCalib_Parameter.Global_offsetFlag = 5;break;}
				Temp_Parameter.Second_correct1 += 2;
			}
			else if(SystemParaMenu2.Usermenu_SW == 2) {
				if(tmptemp >= 298)	{SecondCalib_Parameter.Global_offsetFlag = 5;break;}
				Temp_Parameter.Second_correct2 += 2;
			}
			vision_log_notice("SecondCalib offsetFlag 4444,tmptemp =[%d],correct1=[%d], correct2=[%d]\n", tmptemp, Temp_Parameter.Second_correct1,Temp_Parameter.Second_correct2);
			ts_cpl_fix_msleep(2000);
			break;
		case 5:
			SecondCalib_Parameter.Global_offsetFlag = 0;
			CalNum = 0;
			Write_TempParameter_cfg(&Temp_Parameter);
			vision_log_notice("SecondCalib_Parameter Global_offsetFlag Finish!!!\n");
			UpdatePromptInfo("The Global Offset Setting...",0,VOT_LOG_TYPE_MENU2);
			UpdatePromptInfoWithTimeMs("Calibration_Offset Set completed.", 2000,VOT_LOG_TYPE_MENU2);
			break;

		default:
			break;
	}

}

void Auto_GOFSet_HAND(void)
{
	if(SystemParaMenu2.Usermenu_SW != 1){
		return;
	}
	if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_HAND384){ //适用手持机384机器测温
		ts_u08 gofVal = 0;
		ts_f32 temp = READ_TMP112_Temp();
		if(temp == 0x8000){
			return;
		}
		if(temp < -20.0){
			gofVal = 67;
		}else if(temp >= -20.0 && temp < -10.0){
			gofVal = 67;
		}else if(temp >= -10.0 && temp < 0.0){
			gofVal = 69;
		}else if(temp >= 0.0 && temp < 10.0){
			gofVal = 71;
		}else if(temp >= 10.0 && temp < 20.0){
			gofVal = 74;
		}else if(temp >= 20.0){
			gofVal = SystemParaMenu2.GOF;
		}

		planck640c_write_gof(gofVal);
		ts_cpl_fix_msleep(100);
	}
}


#if 0
uint16_t LookUP_TemtoAD(int16_t TemVal,uint8_t TemLevel)  //TemVal为实际设置温度值*10
{
	uint8_t	i;
	int16_t t0,t1,t2;

	uint16_t temp1;
	uint16_t temp2;
	uint16_t TemToAD;
	float mfloat1;
	float mfloat2;

	t0 = TemVal;

	if(TemLevel == 1)  //一档
	{
		for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		{
			t1    = TEMP_data1[i] *10;        //温度表
			temp1 = AD_data1[i];         //AD表
			i++;
			t2    = TEMP_data1[i] *10;
			temp2 = AD_data1[i];
			if((t0>=t1)&&(t0<=t2))
			{
				i=88;
			}
		}
	 }
	else if(TemLevel == 2) //二档
	{
		for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		{
			t1    = TEMP_data2[i] *10;        //温度表
			temp1 = AD_data2[i];         //AD表
			i++;
			t2    = TEMP_data2[i] *10;
			temp2 = AD_data2[i];
			if((t0>=t1)&&(t0<=t2))
			{
				i=88;
			}
		}
	}
	else if(TemLevel == 3) //三档
	{
		for(i=0;i<12;)             //温度点 暂时设置了12个 所以查询表只需要查12次
		{
			t1    = TEMP_data3[i] *10;        //温度表
			temp1 = AD_data3[i];         //AD表
			i++;
			t2    = TEMP_data3[i] *10;
			temp2 = AD_data3[i];
			if((t0>=t1)&&(t0<=t2))
			{
				i=88;
			}
		}
	}
		temp2=temp2-temp1;						   //ad2-ad1
		mfloat1=(float)temp2;
		temp2=t0-t1;      							 //t-t1
		mfloat2=(float)temp2;
		mfloat1=mfloat2*mfloat1;         //(t-t1)*(ad2-ad1)

		temp2=t2-t1;   		               //t2-t1
		mfloat2=(float)temp2;
		mfloat2=mfloat1/mfloat2;        //(t-t1)*((ad2-ad1)/(t2-t1))

		mfloat1=(float)temp1;
		mfloat2=mfloat2+mfloat1;
		TemToAD = (uint16_t)mfloat2; //反查表出的AD

		return TemToAD;
}


/**
 * @brief:开启/关闭高温热点追踪功能
 * @parameter:SwitchFun:0:关闭热点追踪，1:开启热点追踪
 */
void MaxTempPointTraceFunction(u8 SwitchFun)
{
#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
	if(SwitchFun)//开启高温热点追踪功能
	{
		spot_max.spot_Y_Pos = ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)>>16)&0xFFFF);
		spot_max.spot_X_Pos = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)&0xFFFF);
		if((TempXpos!=spot_max.spot_X_Pos)||(TempYpos!=spot_max.spot_Y_Pos))
		{
			DisPlaySpot(TempXpos,TempYpos,2,0,0);
			TempXpos = spot_max.spot_X_Pos;
			TempYpos = spot_max.spot_Y_Pos;
		}
		DisPlayIcon(0,spot_max.spot_X_Pos,spot_max.spot_Y_Pos,2,CharacterColor);
	}
	else//关闭热点追踪功能
	{
		DisPlaySpot(spot_max.spot_X_Pos,spot_max.spot_Y_Pos,2,0,0);
	}
#endif
}

/**
 * @brief:开启/关闭高温报警功能
 * @parameter:NULL
 */
void MaxTempAlarmFunction(void)
{
	static u8 SwitchFun = 0;
#if(SUPPORT_ZhongLin_UART_PROTOCOL == 1)
	if(!SwitchFun)
	{
		SwitchFun = 1;
		OverPixNum = (APB3_REGR(0xf8100098,0)&0x3FFFC000)>>14;
		if(OverPixNum >= SystemParaMenu2.pixeLimit)
		{
			//DisPlayWarnIcon((u8*)Aralm_Icon,340,10,4,6,0);
		}
	}
	else
	{
		SwitchFun = 0;
		//DisPlayWarnIcon((u8*)Aralm_Icon,340,10,4,0,1);
	}
#endif
}

#endif
