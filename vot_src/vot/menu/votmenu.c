/*
 * votmenu.c -- ��о�˵�
 */
#include <stdio.h>
#include "votmenu.h"
#include "ASCII.h"
#include "name.h"
#include <cpl/cpl_mutex.h>
#include <string.h>
#include <snr_comm_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <uapi/upl_v100.h>
#include "ispfunction.h"
#include <libupload.h>
#include <menu/testmenufunction.h>
#include <menu/usermenufunction.h>
#include <menu/usermenu.h>
#include <fontlib_vot.h>
#include <vot_tlsf.h>
extern SysCtrlInfo_s_t global_sys_ctrl_info;

struct Vot_Misc_Para global_vot_misc_cfg;

KeyDef key;

struct _Usermenu_Parameter UsermenuParameter;
ts_u16 Spot_state;
ts_u16 Area_state;
PT_Spot select_Spot;
PT_Area select_Area;
struct Spot spot_max;
struct Spot spot_min;
ts_u08 VersionTab[3] = {0x20, 0x20, 0};
ts_u32 UserTimerCounter = 0;
ts_u08 ShutterTimerReachFlag = 0;
ts_u16 ShutterTimer;
ts_u08 Flag_Calibration = 0; // �궨��־λ 1 ��ʾ����궨
ts_u08 Calibration_State = 0;
ts_u08 Deviceinfo_Flag;
ts_u08 ProtectFlag = 0;
ts_u16 ProtectTime = 0;
ts_u16 Alarm_Flag = 0;

ts_u32 Quad_TSum = 3300;
ts_u32 Quad_PSum = 0;

float Avg_T_F = 0;
int Avg_T_I = 0;
ts_u08 DataBitNum = 0;
ts_u08 Quad_Index = 1;
ts_u08 QuadTemAlarmFlag = 0;
ts_u16 temptime = 0;
ts_u08 Focus_Calibration_Flag = 0;
ts_u16 PixelPositionArray[80][4] = {0};
/*-------------------------------------------------------------------------*/
struct Menu2_Para SystemParaMenu2;
struct Menu2_Disp_Para DispSysMenu2;
#ifdef Electrolytic_copper
Point vertices[4] = {{200, 200}, {275, 275}, {200, 350}, {125, 275}}; // �ĸ���������
Point Tvertices[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};				  // �ĸ���������
#endif
/*-------------------------------------------------------------------------*/

#define TXT_BASE_ADDR 0x1100000
#ifdef CONFIG_SNR_PLANCK1280C
#define IR_X_pixel 1280
#define IR_Y_pixel 1024
#else
#define IR_X_pixel 640
#define IR_Y_pixel 512
#endif


struct Spot SpotInit[SPOT_NUM] = {

	{"Spot1", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, 1, 0, 0, 0},
	{"Spot2", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, 2, 0, 0, 0},
	{"Spot3", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, 4, 0, 0, 0},
	{"Spot4", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, 8, 0, 0, 0},
	{"Spot5", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, 16, 0, 0, 0},
	{"Spot6", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, 32, 0, 0, 0},
	{"Spot7", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 0, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, 64, 0, 0, 0},
	{"Spot8", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 1, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, 128, 0, 0, 0},
	{"Spot9", 0, SPOT_XSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, SPOT_YSTART_ADDR + (SPOT_AREA_SIZE + SPOT_AREA_GAP) * 2, 256, 0, 0, 0},
	{"SpotB", 0, SPOT_BAD_X_START_ADDR, SPOT_BAD_Y_START_ADDR, 0x4000},
};

struct Area Areainit[AREA_NUM] = {

	{"Area1", AREA_XSTART_ADDR + 40 * 0, AREA_YSTART_ADDR, 1, 0, 0, 0, AREA_LXSTART + 40 * 0, AREA_LYSTART, AREA_LXSTART + 32 + 40 * 0, AREA_LYSTART + 32},
	{"Area2", AREA_XSTART_ADDR + 40 * 1, AREA_YSTART_ADDR, 2, 0, 0, 0, AREA_LXSTART + 40 * 1, AREA_LYSTART, AREA_LXSTART + 32 + 40 * 1, AREA_LYSTART + 32},
	{"Area3", AREA_XSTART_ADDR + 40 * 2, AREA_YSTART_ADDR, 4, 0, 0, 0, AREA_LXSTART + 40 * 2, AREA_LYSTART, AREA_LXSTART + 32 + 40 * 2, AREA_LYSTART + 32},
	{"Area4", AREA_XSTART_ADDR + 40 * 3, AREA_YSTART_ADDR, 8, 0, 0, 0, AREA_LXSTART + 40 * 3, AREA_LYSTART, AREA_LXSTART + 32 + 40 * 3, AREA_LYSTART + 32},
	{"Area5", AREA_XSTART_ADDR + 40 * 4, AREA_YSTART_ADDR, 16, 0, 0, 0, AREA_LXSTART + 40 * 4, AREA_LYSTART, AREA_LXSTART + 32 + 40 * 4, AREA_LYSTART + 32},
};

/**
 * @description: 主动控制图像冻结
 * @param {int} on_off_flag  1：冻结图像 ， 0：不冻结图像
 * @return {void}
 */
void SetImgFreeze(int on_off_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_freeze_img_mutex);

	if (on_off_flag == 1)
	{
		vot_vpo_freeze();
	}
	else
	{
		vot_vpo_unfreeze();
	}
	global_sys_ctrl_info.m_freeze_img_flag = on_off_flag;

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_freeze_img_mutex);
}

/**
 * @description: 自动切换冻结图像模式，冻结和不冻结循环切换
 * @return {void}
 */
void SetImgAutoFreeze(void)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_freeze_img_mutex);

	ts_u08 on_off_flag = 1 - global_sys_ctrl_info.m_freeze_img_flag;

	if (on_off_flag == 1)
	{
		vot_vpo_freeze();
	}
	else
	{
		vot_vpo_unfreeze();
	}
	global_sys_ctrl_info.m_freeze_img_flag = on_off_flag;

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_freeze_img_mutex);
}


void InitSysCtrlInfoBuff(void)
{
	ts_u32 u32_pool_size = 6*1024*1024;
	ts_u32 lutMaxSize = 4096*2;

	#ifdef CONFIG_SNR_PLANCK1280C
	//tmm -i 查看内存 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。由于1280探测器用A12芯片DDR只有64MB,因此屏蔽占用固定内存空间的全局变量，改用局部变量
	u32_pool_size = 16*1024*1024;// 1280探测器使用16MB内存池

	//else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
	#else
	{
		global_sys_ctrl_info.m_high_temp_ad_buff = 0;
		global_sys_ctrl_info.m_low_temp_ad_buff = 0;
	}
	#endif
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");

	global_sys_ctrl_info.u32_lut_base_addr = (ts_u32)ts_cpl_tmm_malloc(ts_null, lutMaxSize, 32, "lut_base_addr");
	#ifdef Receive_JSON_File_EN
	global_sys_ctrl_info.m_json_buff = (ts_u32)ts_cpl_tmm_malloc(ts_null, 1024*4 , 32, "json_buff");
	#endif
	
	ts_cpl_memset((void *)global_sys_ctrl_info.u32_lut_base_addr, 0, lutMaxSize);
}

/**
 * @description: 初始化系统控制信息
 * @return {void}
 */
void InitSysCtrlInfo(void)
{

	ts_cpl_mutex_init(&global_sys_ctrl_info.m_bottom_osd_info_mutex);

	ts_cpl_mutex_init(&global_sys_ctrl_info.m_freeze_img_mutex);

	ts_cpl_mutex_init(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_shutter_fd_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_viu_stats_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_special_osd_info_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_uart1_msg_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_bottom_status_force_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_det_switch_force_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_det_update_ad_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_update_fire_statis_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_feeding_flag_mutex);

	/* 测温区域, 捕获 */
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_templut_osd_info_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_fireModule_get_ad_mutex);
	// vision_log_warn("global_sys_ctrl_info.m_update_fire_statis_mutex %p \n",global_sys_ctrl_info.m_update_fire_statis_mutex);
	ts_cpl_mutex_init(&global_sys_ctrl_info.m_tmp117_i2c_mutex);

	global_sys_ctrl_info.m_bottom_osd_info_visible_flag = 0;
	global_sys_ctrl_info.m_dev_soft_info_visible_flag = 0;
	global_sys_ctrl_info.m_device_info_visible_flag = 0;

	memset(&global_sys_ctrl_info.st_cvbs_vpo_attr, 0, sizeof(ts_vpo_chn_attr_s_t));

	global_sys_ctrl_info.m_freeze_img_flag = 0;

	global_sys_ctrl_info.m_prompt_keep_seconds = 0;
	global_sys_ctrl_info.m_prompt_refresh_flag = 0;
	global_sys_ctrl_info.m_prompt_visible_flag = 0;
	// global_sys_ctrl_info.m_enter_BPC_manual_flag = 0;
	global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;

	global_sys_ctrl_info.m_det_temp_ad_value = 0;
	global_sys_ctrl_info.m_det_tes_ad_value = 0x3900;
	global_sys_ctrl_info.m_det_ad_mean_value = 0x2000;
	global_sys_ctrl_info.m_ignore_uart_msg_flag = 0;

	global_sys_ctrl_info.m_sys_init_finished_flag = 0;


	global_sys_ctrl_info.m_high_temp_ad_buff = 0;
	global_sys_ctrl_info.m_low_temp_ad_buff = 0;

	global_sys_ctrl_info.u32_lut_base_addr = 0;
	global_sys_ctrl_info.m_json_buff = 0;

	ts_cpl_gettimeofday(&global_sys_ctrl_info.m_tv_start);

	global_sys_ctrl_info.isp_auto_cut_run_cnt_u64 = 0;
	global_sys_ctrl_info.isp_cut_cnt_u32 = 0;
	global_sys_ctrl_info.isp_cut_time_sec_u32 = 0;
	global_sys_ctrl_info.isp_temp_ad_value_u16 = 0;

	global_sys_ctrl_info.m_transfer_callback = NULL;

	global_sys_ctrl_info.m_cur_bak_ad_mean_u32 = 0x2000;
	global_sys_ctrl_info.m_sun_pt_force_flag = 0;
	global_sys_ctrl_info.m_det_calibrationing_flag = 0;

	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
		global_sys_ctrl_info.m_sensor_s32_img_w = 1280;
		global_sys_ctrl_info.m_sensor_s32_img_h = 1024;
	}
	else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
	{
		global_sys_ctrl_info.m_sensor_s32_img_w = 640;
		global_sys_ctrl_info.m_sensor_s32_img_h = 512;
	}
	global_sys_ctrl_info.m_shutter_force_close_flag = 0;

	global_sys_ctrl_info.m_det_switch_force_flag = 0; // 强制切换det
	global_sys_ctrl_info.m_bottom_statistic_force_flag = 0;
	global_sys_ctrl_info.m_det_switching_flag = 0;

	global_sys_ctrl_info.m_det_temp_ad_value_u16 = 0;
	global_sys_ctrl_info.m_det_temp_value_f32 = 0;

	global_sys_ctrl_info.m_show_det_cut_info = 0;

	global_sys_ctrl_info.m_det_cur_mode = DET_TEMP_MODE_AUTO;

	global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc = OUTPUT_MODE_ENLARGE_OFF;

	global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_N6;
	global_sys_ctrl_info.m_npu_aisp_flag = TS_OK;
	global_sys_ctrl_info.m_user_metadata_addr = 0;
    global_sys_ctrl_info.m_user_metadata_size = 0;

	memset(&global_sys_ctrl_info.m_isp_fire_statis,0 , sizeof(ts_isp_fire_statis_s_t));

	global_sys_ctrl_info.m_update_fire_statis_force_flag = 0;
	global_sys_ctrl_info.m_hst_min_cursor_osd_dot = ts_null;
	global_sys_ctrl_info.m_hst_max_cursor_osd_dot = ts_null;
	global_sys_ctrl_info.m_backup_hst_mode = HOT_SPOT_TRACKING_MODE_NONE;
	global_sys_ctrl_info.m_run_feeding_flag  = 1;
	global_sys_ctrl_info.m_powerup_det_res_type = DECTOR_RESOLUTION_640X512;
	global_sys_ctrl_info.m_powerup_det_name_type = DECTOR_TYPE_VOT;
	global_sys_ctrl_info.m_powerup_board_port_type = BOARD_PORT_UAD3_LITE;
	global_sys_ctrl_info.m_powerup_temp_drift_comp_mode = TEMP_DRIFT_COMPENSATION_MODE_OFF;

	global_sys_ctrl_info.prevent_burn_flag = 0;
	global_sys_ctrl_info.Iris_Current_State = 1;
	global_sys_ctrl_info.Shifting_Gears_Flag = 0;
	global_sys_ctrl_info.Usermenu_SW_Switching = 0;
	global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag = 1;
	global_sys_ctrl_info.m_temp_get_streaming_flag = 0;
	global_sys_ctrl_info.m_allowGetFrameStream_flag = 0; // 默认不允许获取数据流，特殊定制需求可以通过修改这个变量来禁止获取数据流

	global_sys_ctrl_info.m_powerup_inited_flag = 0;
	global_sys_ctrl_info.SetValue_INTtoMK = 8192;	// 默认8192


	InitSysCtrlInfoBuff();
}

/**
 * @description: 反注销系统控制信息
 * @return {void}
 */
void DeinitSysCtrlInfo(void)
{
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_freeze_img_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_prompt_osd_info_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_shutter_fd_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_viu_stats_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_special_osd_info_mutex);

	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_uart1_msg_mutex);

	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_bottom_status_force_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_det_update_ad_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_update_fire_statis_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_feeding_flag_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_det_switch_force_mutex);

	/* 测温区域, 捕获 */
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_templut_osd_info_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_fireModule_get_ad_mutex);
	ts_cpl_mutex_destory(&global_sys_ctrl_info.m_tmp117_i2c_mutex);

	if (global_sys_ctrl_info.u32_lut_base_addr)
	{
		ts_cpl_tmm_free((ts_void *)global_sys_ctrl_info.u32_lut_base_addr);
		global_sys_ctrl_info.u32_lut_base_addr = 0;
	}
#ifdef Receive_JSON_File_EN
	if (global_sys_ctrl_info.m_json_buff)
	{
		ts_cpl_tmm_free((ts_void *)global_sys_ctrl_info.m_json_buff);
		global_sys_ctrl_info.m_json_buff = 0;
	}
#endif

	vot_tlsf_mem_deinit(&g_VotMemPoolInfo);
}

void DisPlay16(ts_u08 buf[], ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 layer, ts_u08 color)
{
	ts_u08 i, j;
	ts_u32 addrc;
	ts_u08 data1, data2;
	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
	{
		for (i = 0; i < 32; i++)
		{
			data1 = buf[i * 2 + 3]; // ȥ��ǰ��λʶ���뿪ʼ��
			data2 = buf[i * 2 + 4];
			for (j = 0; j < 32; j++)
			{	
				addrc = TXT_BASE_ADDR + 1280 * (j + Y_LINE);
				if (j < 16)
				{
					if (data1 & 0x80)
					{
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					}
					data1 = data1 << 1;
				}
				else
				{
					if (data2 & 0x80)
					{
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					}
					data2 = data2 << 1;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < 16; i++)
		{
			data1 = buf[i * 2 + 3]; // ȥ��ǰ��λʶ���뿪ʼ��
			data2 = buf[i * 2 + 4];
			for (j = 0; j < 16; j++)
			{
				addrc = TXT_BASE_ADDR + 640 * (j + Y_LINE);
				if (j < 8)
				{
					if (data1 & 0x80)
					{
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					}
					data1 = data1 << 1;
				}
				else
				{
					if (data2 & 0x80)
					{
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					}
					data2 = data2 << 1;
				}
			}
		}
	}
}

void DisPlay_AREA_Icon(PT_Area select_Area, ts_u08 layer, ts_u08 color)
{
	ts_osd_attr_s_t st_attr;

	if(select_Area == NULL) {
		return;
	}
	else
	{
		templut_captions_refresh_display(1);
		if (color)
		{
			ts_tdk_osd_get_attr(shdl_osd_dev, global_sys_ctrl_info.m_templut_info_osd_node, &st_attr);
			ts_cpl_mutex_lock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
			semiAutoMode_DrawRect(global_sys_ctrl_info.m_templut_info_osd_node, st_attr.u16_width, st_attr.u16_height, select_Area->Area_Lx, select_Area->Area_Ly, select_Area->Area_Rx, select_Area->Area_Ry);
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
		}
		else {
			ts_cpl_mutex_lock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
			ts_lib_selfAdaptive_AreasScreen_black(global_sys_ctrl_info.m_templut_info_osd_node, select_Area->Area_Lx, select_Area->Area_Ly, select_Area->Area_Rx, select_Area->Area_Ry);
			ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_templut_osd_info_mutex);			
		}
	}
}

void ClearUncheckAreaIcon(PT_Area temp_area)
{
	if(temp_area == NULL) {
		return;
	}
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
	ts_lib_selfAdaptive_AreasScreen_black(global_sys_ctrl_info.m_templut_info_osd_node, temp_area->Area_Lx, temp_area->Area_Ly, temp_area->Area_Rx, temp_area->Area_Ry);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_templut_osd_info_mutex);
}

unsigned char Chartoascii(char data, unsigned char *obuf) // �з���16λ�����õ�3λASCII�룬�ӷ���λ��4λ���
{
	ts_u08 data1;
	if (data == 0)
	{
		obuf[0] = 0x30;
		obuf[1] = 0;
	}
	else if ((data >> 7) == 0) // ����
	{
		if ((data > 0) && (data < 10))
		{
			obuf[0] = (data % 10) + 0x30;
			obuf[1] = 0;
		}
		else if ((data >= 10) && (data < 100))
		{
			obuf[0] = (data / 10) + 0x30;
			obuf[1] = (data % 10) + 0x30;
			obuf[2] = 0;
		}
	}
	else
	{
		data1 = ~data + 1;
		if ((data1 > 0) && (data1 < 10))
		{
			obuf[0] = 0x2D;
			obuf[1] = (data1 % 10) + 0x30;
			obuf[2] = 0;
		}
		else if ((data1 >= 10) && (data1 < 100))
		{
			obuf[0] = 0x2D;
			obuf[1] = (data1 / 10) + 0x30;
			obuf[2] = (data1 % 10) + 0x30;
			obuf[3] = 0;
		}
	}
	return 0;
}

void display_Charnumber(ts_u32 X_LINE, ts_u32 Y_LINE, char pData, ts_u08 layer, ts_u08 color)
{
	// unsigned char Temp_CharNumberTab[5] = {0x20,0x20,0x20,0x20,0};

	// Chartoascii(pData,Temp_CharNumberTab);
	// DisPlayNewStr(X_LINE,Y_LINE,Temp_CharNumberTab,layer,color);
}

unsigned char u8_ascii(char data, unsigned char *obuf)
{
	if (data == 0)
	{
		obuf[0] = 0x30;
		obuf[1] = 0;
	}
	else if ((data > 0) && (data < 10))
	{
		obuf[0] = (data % 10) + 0x30;
		obuf[1] = 0;
	}
	else if ((data >= 10) && (data < 100))
	{
		obuf[0] = (data / 10) + 0x30;
		obuf[1] = (data % 10) + 0x30;
		obuf[2] = 0;
	}
	else if ((data >= 100) && (data <= 255))
	{
		obuf[0] = (data / 100) + 0x30;
		obuf[1] = ((data % 100) / 10) + 0x30;
		obuf[2] = (data % 10) + 0x30;
		obuf[3] = 0;
	}
	return 0;
}

unsigned char u16_ascii(ts_u16 data, unsigned char *obuf)
{
	if (data == 0)
	{
		obuf[0] = 0x30;
		obuf[1] = 0;
	}
	else if ((data > 0) && (data < 10))
	{
		obuf[0] = (data % 10) + 0x30;
		obuf[1] = 0;
	}
	else if ((data >= 10) && (data < 100))
	{
		obuf[0] = (data / 10) + 0x30;
		obuf[1] = (data % 10) + 0x30;
		obuf[2] = 0;
	}
	else if ((data >= 100) && (data <= 999))
	{
		obuf[0] = (data / 100) + 0x30;
		obuf[1] = ((data % 100) / 10) + 0x30;
		obuf[2] = (data % 10) + 0x30;
		obuf[3] = 0;
	}
	else if ((data >= 1000) && (data <= 9999))
	{
		obuf[0] = (data / 1000) + 0x30;
		obuf[1] = ((data % 1000) / 100) + 0x30;
		obuf[2] = ((data % 100) / 10) + 0x30;
		obuf[3] = (data % 10) + 0x30;
		obuf[4] = 0;
	}
	else if ((data >= 10000) && (data <= 65535))
	{
		obuf[0] = (data / 10000) + 0x30;
		obuf[1] = ((data % 10000) / 1000) + 0x30;
		obuf[2] = ((data % 1000) / 100) + 0x30;
		obuf[3] = ((data % 100) / 10) + 0x30;
		obuf[4] = (data % 10) + 0x30;
		obuf[5] = 0;
	}
	return 0;
}

void display_u16number(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color)
{
#if 0
	unsigned char Temp_ts_u16NumberTab[6] = {0x20,0x20,0x20,0x20,0x20,0};

	u16_ascii(pData,Temp_ts_u16NumberTab);
	DisPlayNewStr(X_LINE,Y_LINE,(ts_char*)Temp_ts_u16NumberTab,layer,color);
#endif
}

void display_u8number(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 pData, ts_u08 layer, ts_u08 color)
{
#if 0
	unsigned char Temp_ts_u08NumberTab[5] = {0x20,0x20,0x20,0x20,0};
	u8_ascii(pData,Temp_ts_u08NumberTab);
	DisPlayNewStr(X_LINE,Y_LINE,Temp_ts_u08NumberTab,layer,color);
#endif
}

void display_append_u8number(ts_u08 *buf, ts_u08 pData)
{
	unsigned char Temp_ts_u08NumberTab[5] = {0x20, 0x20, 0x20, 0x20, 0};
	vision_log_debug("display_append_u8number pData = %d\n", pData);
	u8_ascii(pData, Temp_ts_u08NumberTab);
	memcpy(buf, Temp_ts_u08NumberTab, 5);
}
void display_append_u16number(ts_u16 *buf, ts_u16 pData)
{
	unsigned char Temp_ts_u16NumberTab[6] = {0x20, 0x20, 0x20, 0x20, 0x20, 0};
	vision_log_debug("display_append_u16number pData = %d\n", pData);
	u16_ascii(pData, Temp_ts_u16NumberTab);
	memcpy(buf, Temp_ts_u16NumberTab, 6);
}

void Select_CharacterLayer(ts_u08 layer, ts_u08 color, ts_u32 temp_address)
{ /*
	  switch(layer)
	  {
		 case 1:menu_layer1(color,temp_address); break;
		 case 2:menu_layer2(color,temp_address); break;
		 case 3:menu_layer3(color,temp_address); break;
		 case 4:menu_layer4(color,temp_address); break;
		 case 5:menu_layer5(color,temp_address); break;
	  }*/
}

void DisPlayWarnIcon(ts_u08 *buf, ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 layer, ts_u08 color, ts_u08 Erase)
{
	ts_u08 i, j;
	ts_u32 addrc, data;
	ts_u32 *p;
	p = (ts_u32 *)buf;
	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)	
	{
		for (i = 0; i < 128; i++)
		{
			data = *(p + i);
			for (j = 0; j < 128; j++)
			{
				addrc = TXT_BASE_ADDR + 1280 * (j + Y_LINE);
				if (Erase)
				{
					Select_CharacterLayer(BACKGROUND_LAYER, 0, addrc + (i + X_LINE) * 2);
				}
				else
				{
					if (data & 0x01)
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					data = data >> 1;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < 32; i++)
		{
			data = *(p + i);
			for (j = 0; j < 32; j++)
			{
				addrc = TXT_BASE_ADDR + 640 * (j + Y_LINE);
				if (Erase)
				{
					Select_CharacterLayer(BACKGROUND_LAYER, 0, addrc + (i + X_LINE) * 2);
				}
				else
				{
					if (data & 0x01)
						Select_CharacterLayer(layer, color, addrc + (i + X_LINE) * 2);
					data = data >> 1;
				}
			}
		}
	}
}

void DisplayBackground(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer, ts_u08 color)
{
	return;
	ts_u16 i, j;
	ts_u32 addr;

	SOC_ReadAndWrite_Hyram(1);

	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
	{
		
		for (j = 0; j < 32; j++)
		{
			addr = TXT_BASE_ADDR + 1280 * 2 * (j + y);
			for (i = 0; i < 16 * len; i++)
			{
				Select_CharacterLayer(layer, color, addr + (i + x) * 2);
			}
		}
	}
	else
	{
		for (j = 0; j < 16; j++)
		{
			addr = TXT_BASE_ADDR + 640 * 2 * (j + y);
			for (i = 0; i < 8 * len; i++)
			{
				Select_CharacterLayer(layer, color, addr + (i + x) * 2);
			}
		}
	}
	SOC_ReadAndWrite_Hyram(0);
}

void vot_menu_init(void)
{
	// if(ISP_read_sys_cfg() != 0)
	// {
	// 	vision_log_debug("ISP_read_sys_cfg ERROR...\n");
	// 	ISP_write_sys_cfg();
	// }

	// Write_VOTDetectorToFPGA();
	//	Read_CommonAreaParameterFromFlash();
	//	Read_TestmenuParameterFromFlash();
	//	Read_UsermenuParameterFromFlash();
	//	Read_DetectorInfoFromFlash();
	//	Write_UserParameterToFPGA();
	//	Write_TestmenuParameterToFPGA();
	//	Write_CommonAreaParameterToFPGA();
	//	SelectFontColor();
	//	Read_QuadPosFromFlash();
	//	Read_SysTimeAndSysShutFromFlash();

	UserMenuInit();
	TestMenuInit();
//*******************���²���*********	//

}
/** @note 测温捕获点--挡位匹配 */
void Disp_Capture_Info(void)
{
	Refresh_templutSpot_Captions("", &spot_max, 0);
	Refresh_templutSpot_Captions("", &spot_min, 0);
	if(UsermenuParameter.CaptureFlag == 1) {
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 0);	/* 关闭捕获最大值光标 */
		templut_captions_refresh_display(1);
		Disp_Capture_Spot_MIN_info(ON);
	}
	else if(UsermenuParameter.CaptureFlag == 2) {
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 0);	/* 关闭捕获最小值光标 */
		templut_captions_refresh_display(1);
		Disp_Capture_Spot_MAX_info(ON);
	}
	else if(UsermenuParameter.CaptureFlag == 3) {
		templut_captions_refresh_display(1);
		Disp_Capture_Spot_MIN_info(ON);
		Disp_Capture_Spot_MAX_info(ON);
	}
	else {
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 0);
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 0);
		ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
	}
}

void enter_menu2(void)
{
	Key_EnterOrExitTestmenu();
}

void KEY_FE_PRO(void) // ȷ��
{
	vision_log_debug("KEY_FE_PRO\n");
	Key_EnterUsermenu();
}

void KEY_FD_PRO(void) // ��
{
	vision_log_debug("KEY_FD_PRO\n");
	if (SystemParaMenu2.CVBS_flag)
	{
		Key_EnterPreviousUsermenuElement();
	}
	else
	{
		//		SOC_WR_Hyram_Flag = 0;
		Key_EnterPreviousUsermenuElement();
		//		SOC_WR_Hyram_Flag = 1;
	}
}

void KEY_FB_PRO(void) // ��
{
	vision_log_debug("KEY_FB_PRO\n");
	if (SystemParaMenu2.CVBS_flag)
	{
		Key_EnterNextUsermenuElement();
	}
	else
	{
		//		SOC_WR_Hyram_Flag = 0;
		Key_EnterNextUsermenuElement();
		//		SOC_WR_Hyram_Flag = 1;
	}
}

void KEY_F7_PRO(void) // ��
{
	vision_log_debug("KEY_F7_PRO\n");
	Key_LeftUpdataParameter();
}

void KEY_EF_PRO(void) // ��
{
	vision_log_debug("KEY_EF_PRO\n");
	Key_RightUpdataParameter();
}

#if 0
void KEY_EE_PRO(void)  //����궨
{
	if(!Flag_Calibration)
	{
		DisplayBackground(2,Y_START+416,7,BACKGROUND_LAYER,0);
		DisPlayNewStr(2,Y_START+416,"Start..",4,CharacterColor);
		delay_1ms(1000);
		DisplayBackground(2,Y_START+416,8,BACKGROUND_LAYER,0);
		Flag_Calibration = 1;
		SystemParaMenu2.AD_T = 1;
		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
		Calibration_State = 1;
	}
	else
	{
		DisplayBackground(2,Y_START+416,8,BACKGROUND_LAYER,0);
		DisPlayNewStr(2,Y_START+416,"Complete",4,CharacterColor);
		delay_1ms(1000);
		DisplayBackground(2,Y_START+416,8,BACKGROUND_LAYER,0);
		Flag_Calibration = 0;
		SystemParaMenu2.AD_T = 0;
		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xfffffdff)|(SystemParaMenu2.AD_T<<9)));
		Calibration_State = 0;
	}
}

void KEY_F4_PRO(void)
{
	if((Deviceinfo_Flag ==0)&&(SelectionMenu.MenuState == 0)&&(Spot_state == 0)&&\
	 		(Area_state == 0)&&(UsermenuParameter.CaptureFlag ==0))
	{
		Display_Deviceinfo();
	}
	else if((Deviceinfo_Flag ==1)&&(SelectionMenu.MenuState == 0)&&(Spot_state == 0)&&\
				 (Area_state == 0)&&(UsermenuParameter.CaptureFlag ==0))
	{
		Erase_Deviceinfo();
	}
}

void KEY_C1_PRO(void)
{
	displayReminderInfo(Save);
	Write_Flash_NUC();
	displayReminderInfo(Done);
}

ts_u08 obtain_open_SpotorArea_num(ts_u16 state)
{
	ts_u08 count = 0,i;

	for(i = 0;i<16;i++)
	{
		if(state&0x01)
		{
			count++;
		}
		state = state>>1;
	}
	return count;
}

ts_u08 modify_Spot_sequence(struct Spot tempspot,ts_u08 state)
{
	ts_u08 i = 0;
	ts_u08 result = 0;
	if(state == 1)
	{
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(SpotInit[i].spot_sequence >= tempspot.spot_sequence)
				{
					SpotInit[i].spot_sequence--;
				}
			}
		}
	}
	else
	{
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(SpotInit[i].spot_sequence >= tempspot.spot_sequence)
				{
					SpotInit[i].spot_sequence--;
				}
			}
		}
	}
	return 0;
}


ts_u08 serach_SpotArea_maxsequence(ts_u08 kind)
{
	ts_u08 i,index= 0;
	ts_u08 max_sequence = 0;
	if(kind == 0)
	{
		max_sequence = SpotInit[0].spot_sequence;
		for(i = 0;i<SPOT_NUM;i++)
		{
			if(SpotInit[i].spot_sequence != 0)
			{
				if(max_sequence <= SpotInit[i].spot_sequence)
				{
					max_sequence =  SpotInit[i].spot_sequence;
					index = i;
				}
			}
		}
		return index;
	}
	return 0;
}

ts_u16 obtain_ts_u16_bitnumber(ts_u16 tempnumber)
{
	ts_u08 bitnumber = 0;
	if(tempnumber == 0) return 0;
	while(tempnumber !=0)
	{
		tempnumber =tempnumber>>1;
		bitnumber++;
	}
	return (bitnumber-1);
}
#endif
void Disp_SpotN_Messages(PT_Spot temp_spot,ts_u08 state)
{
	static  ts_cpl_timeval_s_t  last_move_ms={0};
	ts_cpl_timeval_s_t  now_move_ms={0};
	static 	ts_u16 move_step = 1;
	// ts_u16  AD_16bit,temp1_int;
	// ts_u16 temp0 = 0;
	// ts_u08	Data_8bit,num;
	// ts_char info_tip[64];

	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);

	ts_cpl_gettimeofday(&now_move_ms);
	ts_u32 diff_ms = ts_cpl_get_timedif(&last_move_ms, &now_move_ms);
	last_move_ms = now_move_ms;
	if(diff_ms < 500000){
		move_step++;
	}
	else{
		move_step = 1;
	}

	if(state)
	{
		// DisPlayIcon(0,temp_spot->spot_X_Pos,temp_spot->spot_Y_Pos,2,0);
		if(state == 1)//上
		{
			if (temp_spot->spot_Y_Pos >= move_step) {
				temp_spot->spot_Y_Pos -= move_step;
			}
			else {
				temp_spot->spot_Y_Pos = 0;
			}			
		}
		else if(state == 2)//下
		{
			temp_spot->spot_Y_Pos += move_step;
			if (temp_spot->spot_Y_Pos >= (new_height-1)) {
				temp_spot->spot_Y_Pos = (new_height-1);
			}
		}
		else if(state == 3)//左
		{
			if (temp_spot->spot_X_Pos >= move_step) {
				temp_spot->spot_X_Pos -= move_step;
			}
			else {
				temp_spot->spot_X_Pos = 0;
			}
		}
		else if(state == 4)//右
		{
			temp_spot->spot_X_Pos += move_step;
			if (temp_spot->spot_X_Pos >= (new_width-1)) {
				temp_spot->spot_X_Pos = (new_width-1);
			}
		}
		// DisPlayIcon(0,temp_spot->spot_X_Pos,temp_spot->spot_Y_Pos,2,CharacterColor);
		DisPlayIcon(temp_spot->spot_ID, temp_spot->spot_X_Pos, temp_spot->spot_Y_Pos, 1);
	}
	
	Sport_Display_Process(temp_spot);
	// switch(temp_spot->spot_ID)
	// {
	// 	case 1:
	// 		if(SystemParaMenu2.AD_T==0)	{

	// 		}
	// 		else{

	// 		}	
	// 		break;

		// case 2:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT2_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT2_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT2_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT2_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 4:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT3_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT3_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT3_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT3_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 8:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT4_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT4_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT4_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT4_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 16:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT5_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT5_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT5_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT5_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 32:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT6_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT6_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT6_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT6_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 64:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT7_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT7_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT7_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT7_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 128:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT8_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT8_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT8_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT8_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;

		// case 256:
		// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT9_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT9_X_Y__SET,0)&0x00000000)|((temp_spot->spot_Y_Pos<<16)+(temp_spot->spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
		// 	if(SystemParaMenu2.AD_T==0)	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT9_AD_TEMP_START,&spot_temp_int);     //spotʵ����ʾ���¶�ֵ
		// 	else	temp_spot->spot_AD = APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT9_AD_TEMP_START,0)&0x0000FFFF;
		// 	break;
	// }
		// SpotMessage(temp_spot->spot_X_Pos,temp_spot->spot_Y_Pos,spot_temp_int,temp_spot->spot_AD,temp_spot->spot_ID,3);
		// DisPlayIcon(0,temp_spot->spot_X_Pos,temp_spot->spot_Y_Pos,2,CharacterColor);
}
#if 0
void ShutterOpr(ts_u16 count)
{
	if(ShutterTimer >= count)
	{
		ShutterTimer=0;
		Int_Shutter_pro();
	}
}

void Auto_shutter(void)
{
	if(ShutterTimerReachFlag)
	{
		ShutterTimerReachFlag = 0;
		ShutterTimer++;
		/*****************************�Զ��ɼ�����*******************************/
		if(VotParameters.Test_Flag)
		{
			if(ShutterTimer >= VotParameters.Test_Flag)
			{
				ShutterTimer=0;
				Int_Shutter_pro();
				Test_Value_Send();
			}
			return;
		}

	if(!Flag_Calibration ) //����궨ģʽ�²����д򵵲���
	{
//#ifdef OPR_ALARM
//		if(!UsermenuParameter.SunPT_SW)
//		{
//#endif
			switch(systemPar.shutter_flag)
			{
#ifdef WLP
				case 1: ShutterOpr(5); break;
				case 2: ShutterOpr(8); break;
				case 3: ShutterOpr(10); break;
				case 4: ShutterOpr(15); break;
				case 5: ShutterOpr(30); break;
				case 6: ShutterOpr(60); break;
				case 7:
					if(UsermenuParameter.AutoShutter == 0x00) ShutterTimer=0;
					else ShutterOpr((ts_u16)(30 * UsermenuParameter.AutoShutter));
					break;
#else

				case 1: ShutterOpr(15); break;
				case 2:
#ifdef SUPPORT_HuanYu_UART_PROTOCOL
					ShutterOpr((ts_u16)(30 * UsermenuParameter.HY_ShutterTime1));
#else
					ShutterOpr(30);
#endif
					break;
				case 3:
//				if(UsermenuParameter.AutoShutter ==0x01) 	  ShutterOpr(30);
//				else if(UsermenuParameter.AutoShutter ==0x02) ShutterOpr(60);
//				else if(UsermenuParameter.AutoShutter ==0x03) ShutterOpr(90);
//				else ShutterTimer=0;
/*******************�ɶ�����_TEST******����ԭ���Զ��򵵹���*****************/
				if(UsermenuParameter.AutoShutter == 0x00) ShutterTimer=0;
				else
				{
#ifdef SUPPORT_HuanYu_UART_PROTOCOL
					ShutterOpr((ts_u16)(30 * UsermenuParameter.HY_ShutterTime2));
#else
					ShutterOpr((ts_u16)(30 * UsermenuParameter.AutoShutter));
#endif
				}
				break;
#ifdef SUPPORT_HuanYu_UART_PROTOCOL
				case 4:
					ShutterOpr((ts_u16)(30 * UsermenuParameter.HY_ShutterTime3));break;
#endif
#endif
			}
//#ifdef OPR_ALARM
//		}
//#endif
	}
	}
}

unsigned char Hex16bit_ascii4(ts_u16 data,unsigned char *obuf)
{
	if(((data>>12)&0x000f) <= 9)
		obuf[0]=((data>>12)&0x000f) + 0x30;
	else
		obuf[0]=((data>>12)&0x000f) + 0x37;

	if(((data>>8)&0x000f) <= 9)
		obuf[1]=((data>>8)&0x000f) + 0x30;
	else
		obuf[1]=((data>>8)&0x000f) + 0x37;

	if(((data>>4)&0x000f) <= 9)
		obuf[2]=((data>>4)&0x000f) + 0x30;
	else
		obuf[2]=((data>>4)&0x000f) + 0x37;

	if((data&0x000f) <= 9)
		obuf[3]=(data&0x000f) + 0x30;
	else
		obuf[3]=(data&0x000f) + 0x37;

	return 0;
}

unsigned char Hex32bit_ascii6(ts_u32 data,unsigned char *obuf)
{

	if(((data>>20)&0x0000000f) <= 9)
		obuf[0]=((data>>20)&0x000f) + 0x30;
	else
		obuf[0]=((data>>20)&0x0000000f) + 0x37;

	if(((data>>16)&0x0000000f) <= 9)
		obuf[1]=((data>>16)&0x000f) + 0x30;
	else
		obuf[1]=((data>>16)&0x0000000f) + 0x37;

	if(((data>>12)&0x0000000f) <= 9)
		obuf[2]=((data>>12)&0x000f) + 0x30;
	else
		obuf[2]=((data>>12)&0x0000000f) + 0x37;

	if(((data>>8)&0x0000000f) <= 9)
		obuf[3]=((data>>8)&0x000f) + 0x30;
	else
		obuf[3]=((data>>8)&0x0000000f) + 0x37;

	if(((data>>4)&0x0000000f) <= 9)
		obuf[4]=((data>>4)&0x000f) + 0x30;
	else
		obuf[4]=((data>>4)&0x0000000f) + 0x37;

	if((data&0x0000000f) <= 9)
		obuf[5]=(data&0x000f) + 0x30;
	else
		obuf[5]=(data&0x0000000f) + 0x37;

	return 0;
}

unsigned char int8_ascii4(int8_t data,unsigned char *obuf)	//�з���8λ�����õ�3λASCII�룬�ӷ���λ��4λ���
{
	ts_u08	buf[4];
	ts_u08	data1;

	if(data==0)
	{
			obuf[0]=0x20;
			obuf[1]=0x20;
			obuf[2]=0x20;
			obuf[3]=0x30;
	}
	else if((data>>7)==0)
	{
		buf[0]	=	0;
		buf[1]	=	data/100;
		buf[2]	=	(data%100)/10;
		buf[3]	=	data%10;
		//obuf[0] = 0x20;	//ascii �ո�
		obuf[0] = 0x2B;	//ascii +
		if(buf[1]==0)
			{
				obuf[1]=0x20;
				if (buf[2]==0)		obuf[2]=0x20;
				else							obuf[2]=buf[2] + 0x30;
				obuf[3]=buf[3] + 0x30;
			}
		else
		{
			obuf[1]=buf[1] + 0x30;
			obuf[2]=buf[2] + 0x30;
			obuf[3]=buf[3] + 0x30;
		}
	}
	else
	{
		buf[0]	=	1;
		data1 = ~data + 1;
		buf[1]	=	data1/100;
		buf[2]	=	(data1%100)/10;
		buf[3]	=	data1%10;
		obuf[0] = 0x2D;		//ascii -
		if(buf[1]==0)
			{
				obuf[1]=0x20;
				if (buf[2]==0)
					obuf[2]=0x20;
				else
					obuf[2]=buf[2] + 0x30;
				obuf[3]=buf[3] + 0x30;
			}
		else
		{
			obuf[1]=buf[1] + 0x30;
			obuf[2]=buf[2] + 0x30;
			obuf[3]=buf[3] + 0x30;
		}
	}
	return 0;
}
unsigned char uint8_ascii3(ts_u08 data,unsigned char *obuf)	//�޷���8λ�����õ�3λASCII�룬������ʾ
{
	ts_u08	buf[3];

	buf[0]	=	data/100;
	buf[1]	=	(data%100)/10;
	buf[2]	=	data%10;
	if(buf[0]==0)
		{
			obuf[0]=0x20;	//ascii �ո�
			if (buf[1]==0)
				obuf[1]=0x20;
			else
				obuf[1]=buf[1] + 0x30;
			obuf[2]=buf[2] + 0x30;
		}
	else
	{
		obuf[0]=buf[0] + 0x30;
		obuf[1]=buf[1] + 0x30;
		obuf[2]=buf[2] + 0x30;
	}
	return 0;
}
unsigned char uint8_ascii2(ts_u08 data,unsigned char *obuf)	//�޷���8λ�����õ�2λASCII��(��λ����)��������ʾ
{
	ts_u08	buf[2];

	buf[0]	=	(data%100)/10;
	buf[1]	=	data%10;
	if(buf[0]==0)
	{
		obuf[0]=0x20;
		obuf[1]=buf[1] + 0x30;
	}
	else
	{
		obuf[0]=buf[0] + 0x30;
		obuf[1]=buf[1] + 0x30;
	}
	return 0;
}
unsigned char uint8_ascii1(ts_u08 data,unsigned char *obuf)	//�޷���8λ�����õ�2λASCII��(�١�ʮλ����)��������ʾ
{
	ts_u08	buf[1];

	buf[0]	=	data%10;
	obuf[0]=buf[0] + 0x30;
	return 0;
}
unsigned char uint16_ascii4(ts_u16 data,unsigned char *obuf)	//�޷���16λ�����õ�4λASCII�룬������ʾ������9999��ʾ9999
{
	ts_u16 data1;

	if(data>9999)		data1 = 9999;
	else				data1 = data;
	if(data1 == 0)
	{
		obuf[0] = 0x30;
		obuf[1] = 0;
	}
	else if((data1>0)&&(data1<10))
	{
		obuf[0] = (data1%10)+0x30;
		obuf[1] = 0;
	}
	else if((data1>=10)&&(data1<100))
	{
		obuf[0] = (data1/10)+0x30;
		obuf[1] = (data1%10)+0x30;
		obuf[2] = 0;
	}
	else if((data1>=100)&&(data1<1000))
	{
		obuf[0] = (data1/100)+0x30;
		obuf[1] = ((data1%100)/10)+0x30;
		obuf[2] = (data1%10)+0x30;
		obuf[3] = 0;
	}
	else if((data1>=1000)&&(data1<=10000))
	{
		obuf[0]	= data1/1000+0x30;
		obuf[1]	= (data1%1000)/100+0x30;
		obuf[2]	= (data1%100)/10+0x30;
		obuf[3]	= data1%10+0x30;
	}
	return 0;
}
void Display_Hex16bit(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)
{
	unsigned char	buf[6] = {0x00};

	Hex16bit_ascii4(pData,buf);
	buf[4] = 0x48;//"H";
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void Display_Hex24bit(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u32 pData,ts_u08 layer,ts_u08 color)
{
	unsigned char	buf[8] = {0x00};

	Hex32bit_ascii6(pData,buf);
	buf[6] = 0x48;//"H";
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void Display_int8_Dec4(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)	//3λ���ּ�1����λ
{
	unsigned char	buf[5] = {0x00};		//��Ҫ��ʾ������1����Ԥ��0��������ʾֹͣ�ж�
	int8_ascii4(pData,buf);
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void Display_uint8_Dec2(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)
{
	unsigned char	buf[3] = {0x00};
	uint8_ascii2(pData,buf);
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void Display_uint8_Dec3(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)	//��3λʮ���Ƶķ�ʽ��ʾһ���޷���8λ����
{
	unsigned char	buf[4] = {0x00};
	uint8_ascii3(pData,buf);
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}
void Display_uint8_Dec1(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)	//��1λʮ���Ƶķ�ʽ��ʾһ���޷���8λ����(�١�ʮλ����)
{
	unsigned char	buf[3] = {0x00};
	uint8_ascii1(pData,buf);
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void Display_uint16_Dec4(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u16 pData,ts_u08 layer,ts_u08 color)	//��4λʮ���Ƶķ�ʽ��ʾһ���޷���16λ����(<10000)����4λ�з������ɶ������
{
	unsigned char	buf[5] = {0x00};
	uint16_ascii4(pData,buf);
	DisPlayNewStr(X_LINE,Y_LINE,buf,layer,color);
}

void BadCroOpr(ts_u08 opr)
{
	DisPlay_BAD_Cro(&badCro,2,0);
	switch(opr){
		default:break;
		case badCroUp:	  		if(badCro.Spot_y>0)			badCro.Spot_y--; 		break;
		case badCroDown:   	if(badCro.Spot_y<511)		badCro.Spot_y++;     break;
		case badCroLeft:	  		if(badCro.Spot_x>0)			badCro.Spot_x--;		break;
		case badCroRight:	  	if(badCro.Spot_x<639)		badCro.Spot_x++;		break;
	}
	DisPlay_BAD_Cro(&badCro,2,CharacterColor);
}
#endif
ts_void v_getDector_DisplayResolution(ts_u16* vWidth, ts_u16* vHeigth)
{
	dector_resolution_type_t res_type = get_powerup_dector_resolution_type();
	if(res_type == DECTOR_RESOLUTION_640X512) {
		*vWidth = 640;
		*vHeigth = 512;
	}
	else if(res_type == DECTOR_RESOLUTION_1280X1024) {
		*vWidth = 1280;
		*vHeigth = 1024;
	}
	else {
		*vWidth = 384;
		*vHeigth = 288;
	}
}
/* 在坏点校正模式下获取当前分辨率 */
void vGet_BPC_MODE_CurrentResolution(ts_u16 *vWidth, ts_u16 *vHeigth)
{
	ts_u16 max_width = 640;
	ts_u16 max_heigth = 512;

	update_cvbs_vpo_attr();
	if (SystemParaMenu2.m_bpc_ref_mode == BPC_MODE_ON_CVBS) {
		max_width = global_sys_ctrl_info.m_sensor_s32_img_w;
		max_heigth = global_sys_ctrl_info.m_sensor_s32_img_h;
	}

	if ((max_width == 384) && (max_heigth == 288))
	{
		*vWidth = 384;
		*vHeigth = 288;
	}
	else if ((max_width ==640) && (max_heigth == 512))
	{
		*vWidth = 640;
		*vHeigth = 512;
	}
	else if ((max_width ==1280) && (max_heigth == 1024))
	{
		*vWidth = 1280;
		*vHeigth = 1024;
	}

	// vision_log_debug("vWidth = %d,vHeigth = %d\n", *vWidth, *vHeigth);
}
/** @note (Semi_A)半自动校点-进入/退出函数 */
ts_void EnterOrExit_SemiAutoMode(ts_u08 vExit_flag)
{
	if (SystemParaMenu2.Usermenu_SW) {
		if ((Spot_state != 0) || ((Area_state&0x0f) != 0) || (UsermenuParameter.CaptureFlag != 0)) {
			return;
		}
	}
	cvbs_osd_refresh_display(0);
	cvbs_cursor_refresh_display(0);
	if (SystemParaMenu2.Usermenu_SW == 0) {
		/* 切换模式 */
		if (SystemParaMenu2.m_bpc_ref_mode == BPC_MODE_ON_CVBS)
		{
			if (vExit_flag == 0)
			{
				if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG)
				{
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
				}
			}
			else
			{
				if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ENLARGE)
				{
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
				}
			}
		}
	}
	else {
		if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG) {
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
			cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
		}
	}


	update_cvbs_vpo_attr();
	/* 在不同环境下做对应的操作 */
	if (vExit_flag == 0)
	{
		vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
		IsHide_templut_osd_display(0);
		ResetDefaultMenuDisplay();
		global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_BPC;
		vGetAndDisplay_SemiAutoBPC();
		SelectionMenu.MenuState = 0x0B; // 当前菜单状态为0x0B,表示进入半自动校点界面
		// ISP_clr_nuc_k();
	}
	else
	{
		// GetAndDisplayIspStatisInfo(0);
		vot_osd_node_clear_info(global_sys_ctrl_info.m_bottom_info_osd_node);
		vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
		// ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, 0);
		// ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
		// ISP_reset_nuc_k();
		Exit_BPC_Maual_mode();
		SystemParaMenu2.Auto_Semi = 0; // 关闭半自动校点
		SelectionMenu.MenuState = 0x0;
		global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
		ISP_set_bottom_statistic_force_flag(1);
		vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
	}
}

void BadArea_ADCalc(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_pos, ts_u16 *ad_avg, ts_u16 *ad_max, ts_u16 *ad_min)
{
	ts_buffer_s_t st_kbuf;
	ts_u16 *pu16_k_buf = null;

	// ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
	ts_s32 ret = ts_cpl_mutex_trylock(&global_sys_ctrl_info.m_dump_ad_mutex);
	if (ret != TS_OK)
	{
		vision_log_warn(" ts_cpl_mutex_trylock m_dump_ad_mutex failed  \n");
		return;
	}

	// ts_s32 s32_cnt = 0;
	ts_u16 maxTemp = 0, minTemp = 0x3fff, avgTemp = 0;
	ts_u64 sumTemp = 0;
	ts_u64 sumPoint = s32_img_w * s32_img_h;
	ts_u64 frame_size = vot_isp_get_upl_get_frm_strid(s32_pos);

	ts_u16 *m_dst_ad_buff = (ts_u16 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,frame_size) ;

	ts_u16 *pu16_ptr = m_dst_ad_buff;

	/*获取NUC的K值表*/
	ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
	pu16_k_buf = (ts_u16 *)st_kbuf.pu08_ptr;

 	if(vot_isp_dump_raw_sigle(s32_pos,pu16_ptr,frame_size )==TS_OK)
	{
		
		/* 计算当前节点 */
		ts_u16 tmp_value = 0x0;
		if (ad_avg)
		{
			for (ts_s32 y = BadMes.Area_Ly; y < BadMes.Area_Ry; y++)
			{
				for (ts_s32 j = BadMes.Area_Lx; j < BadMes.Area_Rx; j++)
				{

					ts_s32 index = y * s32_img_w + j;

					tmp_value = (pu16_ptr[index] & 0x3fff);
					if ((pu16_k_buf[index] >> 15) == 0) // 已标记的坏点不参与计算
					{
						if (maxTemp < tmp_value)
						{
							maxTemp = tmp_value;
							BadMes.MaxAD_X = j;
							BadMes.MaxAD_Y = y;
						}
						if (minTemp > tmp_value)
						{
							minTemp = tmp_value;
							BadMes.MinAD_X = j;
							BadMes.MinAD_Y = y;
						}
					}
					// if(tmp_value==0x3fff){
					//     vision_log_debug(" j %d y %d  tmp_value %x \n", j, y, tmp_value);
					// }
					sumTemp += tmp_value;
				}
			}

			ts_u64 avg_value = (sumTemp / sumPoint);
			avgTemp = (ts_u16)avg_value;
			vision_log_debug(" Badframeindex avgTemp [0x%x]  minTemp[%d,%d] = [0x%x]  maxTemp[%d,%d] = [0x%x]\n", avgTemp, BadMes.MinAD_X,
								BadMes.MinAD_Y, minTemp, BadMes.MaxAD_X, BadMes.MaxAD_Y, maxTemp);
			if (ad_avg)
			{
				*ad_avg = avgTemp;
			}

			if (ad_max)
			{
				*ad_max = maxTemp;
			}

			if (ad_min)
			{
				*ad_min = minTemp;
			}
		}

	}

    if(m_dst_ad_buff){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&m_dst_ad_buff); 
		m_dst_ad_buff = 0;
    }

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

	vision_log_debug("get frm ok,  \n");
}

void BadArea_Display(ts_u08 active)
{
	ts_u16 BadArea_avg, BadArea_max, BadArea_min;
	ts_u16 pos_x = 0, pos_y = 0;
	// ts_char info_tip[64];
	// ts_u16  dst_pos_x, dst_pos_y;
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV) {
		BadMes.Area_Ly = BadMes.Area_Ly % max_heigth;
		BadMes.Area_Ry = BadMes.Area_Ry % max_heigth;
		BadMes.Area_Lx = BadMes.Area_Lx % max_width;
		BadMes.Area_Rx = BadMes.Area_Rx % max_width;
		vision_log_debug("Area_Ly = %d, Area_Ry = %d, Area_Lx = %d, Area_Rx = %d\n", BadMes.Area_Ly, BadMes.Area_Ry, BadMes.Area_Lx, BadMes.Area_Rx);
	}
	
	if (active)
	{
		BadArea_ADCalc(max_width, max_heigth, TS_UPL_POS_NUC, &BadArea_avg, &BadArea_max, &BadArea_min);
		if (SystemParaMenu2.Auto_Semi)
		{
			if (SystemParaMenu2.Auto_Semi == 1)
			{
				pos_x = BadMes.MinAD_X;
				pos_y = BadMes.MinAD_Y;
			}
			else if (SystemParaMenu2.Auto_Semi == 2)
			{
				pos_x = BadMes.MaxAD_X;
				pos_y = BadMes.MaxAD_Y;
			}
			BadMes.Spot_x = pos_x;
			BadMes.Spot_y = pos_y;
			vision_log_debug("Auto_Semi [%d] Pos_X[%d]  Pos_Y[%d]\n", SystemParaMenu2.Auto_Semi, pos_x, pos_y);
			// cvbs_osd_refresh_display(0);
			cvbs_cursor_refresh_display(1);
			// cursor_set_position(global_sys_ctrl_info.m_cvbs_cursor_osd_node, pos_x, pos_y);

			// snprintf(info_tip,64 ,"[%d,%d]", pos_x, pos_y);
			// if(pos_x>320){
			// 	dst_pos_x = 60;
			// }
			// else{
			// 	dst_pos_x = 460;
			// }
			// if(pos_y>260){
			// 	dst_pos_y = 60;
			// }
			// else{
			// 	dst_pos_y = 460;
			// }
			// global_sys_ctrl_info.m_prompt_cursor_pos_x = dst_pos_x;
			// global_sys_ctrl_info.m_prompt_cursor_pos_y = dst_pos_y;
			// UpdateSpecialInfo(info_tip, dst_pos_x, dst_pos_y, 1);
			semiAutoMode_clearScreen(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth);
			PCMoveCursorBPC(pos_x, pos_y);
			semiAutoMode_DrawRect(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth, BadMes.Area_Lx, BadMes.Area_Ly, BadMes.Area_Rx, BadMes.Area_Ry);
		}
	}
	else
	{
		cvbs_cursor_refresh_display(0);
		cvbs_osd_refresh_display(1);
		semiAutoMode_clearScreen(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth);
	}
}

void BadSemiAutoOpr(PT_Area temp_area, ts_u08 state, ts_u08 opr)
{
	// ts_u16 x_data = 60;

	if (temp_area == NULL) {
		vision_log_debug("BadSemiAutoOpr temp_area == NULL\n");
		return;
	}

	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV) {
		BadMes.Area_Ly = BadMes.Area_Ly % max_heigth;
		BadMes.Area_Ry = BadMes.Area_Ry % max_heigth;
		BadMes.Area_Lx = BadMes.Area_Lx % max_width;
		BadMes.Area_Rx = BadMes.Area_Rx % max_width;
		vision_log_debug("Area_Ly = %d, Area_Ry = %d, Area_Lx = %d, Area_Rx = %d\n", BadMes.Area_Ly, BadMes.Area_Ry, BadMes.Area_Lx, BadMes.Area_Rx);
	}

	switch (opr)
	{
	case AreaBad_XY_Normal:
		Area_XY_Normal(state);
		break;
	case AreaBad_XY_Expand:
		Area_XY_Expand();
		break;
	case AreaBad_XY_Shrink:
		Area_XY_Shrink();
		break;
	case AreaBad_X_Expand:
		Area_X_Expand();
		break;
	case AreaBad_X_Shrink:
		Area_X_Shrink();
		break;
	case AreaBad_Y_Expand:
		Area_Y_Expand();
		break;
	case AreaBad_Y_Shrink:
		Area_Y_Shrink();
		break;
	}
	if (SystemParaMenu2.Auto_Semi) {
		BadArea_Display(1);
	}
}
/** @note 测温区域轮询 */
void AreaN_Scale(PT_Area temp_area,ts_u08 state,ts_u08 opr) {
	// ts_char special_Mess[64];

	if(temp_area == NULL)
	{
		return;
	}
	else 
	{
		if(state)	{DisPlay_AREA_Icon(temp_area, 2, 0);}
		switch(opr) {
			case AreaBad_XY_Normal:	  Area_XY_Normal(state); break;
			case AreaBad_XY_Expand:   Area_XY_Expand();      break;
			case AreaBad_XY_Shrink:	  Area_XY_Shrink();		 break;
			case AreaBad_X_Expand:	  Area_X_Expand();		 break;
			case AreaBad_X_Shrink:	  Area_X_Shrink();		 break;
			case AreaBad_Y_Expand:	  Area_Y_Expand();		 break;
			case AreaBad_Y_Shrink:	  Area_Y_Shrink();		 break;
		}
		DisPlay_AREA_Icon(temp_area, 2, 1);
		Area_Display_Process(temp_area);
	}
}
/** @note 刷新四方形 */
void Area_SimultaneousDisplay(ts_u16 state)
{
	switch(state)
	{
		case 1:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			break;
		case 2:
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			break;
		case 3:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			break;
		case 4:
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			break;
		case 5:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			break;
		case 6:
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			break;
		case 7:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			break;
		case 8:
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 9:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 10:
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 11:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 12:
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 13:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 14:
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		case 15:
			DisPlay_AREA_Icon(&Areainit[0],2,1);
			DisPlay_AREA_Icon(&Areainit[1],2,1);
			DisPlay_AREA_Icon(&Areainit[2],2,1);
			DisPlay_AREA_Icon(&Areainit[3],2,1);
			break;
		default:
			break;
	}
}
#if 0
void Area_Val_Simultaneous()
{
#if 0
	if(Area_state&0x0001)
	{
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0)&0x00000000)|((Areainit[0].Area_Ly<<16)+(Areainit[0].Area_Lx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0)&0x00000000)|(((Areainit[0].Area_Ry)<<16)+(Areainit[0].Area_Rx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		temp_convert_display_H((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN_MIN_MAX_TEMP_START,&Temp0_max);     //����¶�ֵ
		temp_convert_display_L((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN_MIN_MAX_TEMP_START,&Temp0_min);     //��С�¶�ֵ
		Areainit[0].Area_temp_AVG = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN_TEMPAVG_START)/((Areainit[0].Area_Rx - Areainit[0].Area_Lx)*(Areainit[0].Area_Ry-Areainit[0].Area_Ly));
		Temp0_avg = AreaTemp_convert_AVG(Areainit[0].Area_temp_AVG);
	}
	if(Area_state&0x0002)
	{
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area2_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area2_Left_XY__SET,0)&0x00000000)|((Areainit[1].Area_Ly<<16)+(Areainit[1].Area_Lx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area2_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area2_Right_XY__SET,0)&0x00000000)|(((Areainit[1].Area_Ry)<<16)+(Areainit[1].Area_Rx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		temp_convert_display_H((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START,&Temp1_max);     //����¶�ֵ
		temp_convert_display_L((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START,&Temp1_min);     //��С�¶�ֵ
		Areainit[1].Area_temp_AVG = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_TEMPAVG_START)/((Areainit[1].Area_Rx - Areainit[1].Area_Lx)*(Areainit[1].Area_Ry-Areainit[1].Area_Ly));
		Temp1_avg = AreaTemp_convert_AVG(Areainit[1].Area_temp_AVG);
	}
	if(Area_state&0x0004)
	{
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area3_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area3_Left_XY__SET,0)&0x00000000)|((Areainit[2].Area_Ly<<16)+(Areainit[2].Area_Lx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area3_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area3_Right_XY__SET,0)&0x00000000)|(((Areainit[2].Area_Ry)<<16)+(Areainit[2].Area_Rx)));//���ò�����������Xλ�ã����ò�����������Yλ��
		temp_convert_display_H((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN3_MIN_MAX_TEMP_START,&Temp2_max);     //����¶�ֵ
		temp_convert_display_L((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN3_MIN_MAX_TEMP_START,&Temp2_min);     //��С�¶�ֵ
		Areainit[2].Area_temp_AVG = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN3_TEMPAVG_START)/((Areainit[2].Area_Rx - Areainit[2].Area_Lx)*(Areainit[2].Area_Ry-Areainit[2].Area_Ly));
		Temp2_avg = AreaTemp_convert_AVG(Areainit[2].Area_temp_AVG);
	}
//	if(Area_state&0x0008)
//	{
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area4_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area4_Left_XY__SET,0)&0x00000000)|((Areainit[3].Area_Ly<<16)+(Areainit[3].Area_Lx)));//���ò�����������Xλ�ã����ò�����������Yλ��
//		APB3_REGW(IO_APB_SLAVE_0_INPUT_Area4_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area4_Right_XY__SET,0)&0x00000000)|(((Areainit[3].Area_Ry)<<16)+(Areainit[3].Area_Rx)));//���ò�����������Xλ�ã����ò�����������Yλ��
//		temp_convert_display_H((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN4_MIN_MAX_TEMP_START,&Temp3_max);     //����¶�ֵ
//		temp_convert_display_L((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN4_MIN_MAX_TEMP_START,&Temp3_min);     //��С�¶�ֵ
//		Areainit[3].Area_temp_AVG = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN4_TEMPAVG_START)/((Areainit[3].Area_Rx - Areainit[3].Area_Lx)*(Areainit[3].Area_Ry-Areainit[3].Area_Ly));
//		Temp3_avg = AreaTemp_convert_AVG(Areainit[3].Area_temp_AVG);
//	}
//	for(ts_u08 l=2;l>=0;l--)
//	{
//		if((Area_state>>l)&0x01)
//		{
//			select_Area = &Areainit[l];
//			return;
//		}
//	}
#endif
}
#ifdef Area2TestDisp
void TestArea2Message(PT_Area temp_area)
{
#if 0
	ts_u16  Data_16bit,temp1_int,temp0_min,temp0_max,temp0_avg;
	ts_u08	Data_8bit,num;
	ts_u16	temp1_point;
	int32_t	temp0;

	DisPlayNewStr(X_START,Y_START+392,"MAX2:",3,CharacterColor);
	DisPlayNewStr(X_START+(88+20)*1,Y_START+392,"MIN2:",3,CharacterColor);
	DisPlayNewStr(X_START+(88+20)*2,Y_START+392,"AVG2:",3,CharacterColor);

	APB3_REGW(IO_APB_SLAVE_0_INPUT_Area2_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area2_Left_XY__SET,0)&0x00000000)|((temp_area->Area_Ly<<16)+(temp_area->Area_Lx)));//���ò�����������Xλ�ã����ò�����������Yλ��
	APB3_REGW(IO_APB_SLAVE_0_INPUT_Area2_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area2_Right_XY__SET,0)&0x00000000)|(((temp_area->Area_Ry)<<16)+(temp_area->Area_Rx)));//���ò�����������Xλ�ã����ò�����������Yλ��
	if(SystemParaMenu2.AD_T==1)
	{
		temp_area->Area_temp_MAX = (read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START)&0xFFFF0000)>>16;
		temp_area->Area_temp_MIN = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START)&0x0000FFFF;
				//temp_area->Area_temp_AVG	 = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_TEMPAVG_START)/((temp_area->Area_Rx - temp_area->Area_Lx)*(temp_area->Area_Ry-temp_area->Area_Ly));
		temp_area->Area_temp_AVG	 = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_TEMPAVG_START)/((temp_area->Area_Rx - temp_area->Area_Lx)*(temp_area->Area_Ry-temp_area->Area_Ly));


		DisplayBackground(X_START+34+4,Y_START+392,7,BACKGROUND_LAYER,0);
		Display_Hex16bit(X_START+36+4,Y_START+392,temp_area->Area_temp_MAX,3,CharacterColor);

		DisplayBackground(X_START+(88+20)*1+34+4,Y_START+392,7,BACKGROUND_LAYER,0);
		Display_Hex16bit(X_START+(88+20)*1+36+4,Y_START+392,temp_area->Area_temp_MIN,3,CharacterColor);

		DisplayBackground(X_START+(88+20)*2+32+4,Y_START+392,7,BACKGROUND_LAYER,0);
		Display_Hex16bit(X_START+(88+20)*2+36+4,Y_START+392,temp_area->Area_temp_AVG,3,CharacterColor);
	}
	else
		{
				temp_convert_display_H((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START,&temp0_max);     //����¶�ֵ
				temp_convert_display_L((ts_u32 *)IO_APB_SLAVE_0_INPUT_AREAN2_MIN_MAX_TEMP_START,&temp0_min);     //��С�¶�ֵ
				temp_area->Area_temp_AVG	 = read_ts_u32(IO_APB_SLAVE_0_INPUT_AREAN2_TEMPAVG_START)/((temp_area->Area_Rx - temp_area->Area_Lx)*(temp_area->Area_Ry-temp_area->Area_Ly));

	//********************************************************�������ֵ*********************************************************//
					DisplayBackground(X_START+36+4,Y_START+392,8,BACKGROUND_LAYER,0);	//

					temp1_int = temp0_max/10;
					if(temp0_max<0)
						{
							Data_16bit = temp0_max-1;
							Data_16bit = ~Data_16bit;
							temp1_int = Data_16bit/10;
							Data_8bit  =  Data_16bit%10;
						}
					else
					{
						temp1_int = temp0_max/10;
						Data_8bit = temp0_max%10;
					}
					if(temp0_max<0)      //Ϊ���¶�
					{
						DisPlayNewStr(X_START+36+4,Y_START+392,"-",3,CharacterColor);		//
					}
					else
					{
						DisPlayNewStr(X_START+36+4,Y_START+392,"+",3,CharacterColor);		//
					}
					if(abs(temp1_int)<100)
					{
						Display_uint8_Dec2(X_START+48+4,Y_START+392,temp1_int,3,CharacterColor);
						num=16;
					}
					else if((abs(temp1_int)>=100)&&((abs(temp1_int)<1000)))
					{
						Display_uint8_Dec3(X_START+48+4,Y_START+392,temp1_int,3,CharacterColor);
						num=26;
					}
					else
					{
						Display_uint16_Dec4(X_START+48+4,Y_START+392,temp1_int,3,CharacterColor);
						num=35;
					}

					DisPlayNewStr(X_START+48+num+4,Y_START+392,".",3,CharacterColor);		//
					Display_uint8_Dec1(X_START+54+num+4,Y_START+392,Data_8bit,3,CharacterColor);
					//DisPlayNewStr(X_START+(88+8)*2+72+num,Y_START+368,"��",3,(UsermenuParameter.FontColor+FONT_COLOR));
					//Display_Hex16bit(X_START+(88+8)*2+40,Y_START+368,temp_area->Area_temp_MAX,3,(UsermenuParameter.FontColor+FONT_COLOR));
					//********************************************************������Сֵ*********************************************************//
					DisplayBackground(X_START+(88+20)*1+36+4,Y_START+392,8,BACKGROUND_LAYER,0); //������Сֵ

					temp1_int = temp0_min/10;
					if(temp0_min<0)
						{
								Data_16bit = temp0_min-1;
								Data_16bit = ~Data_16bit;
								temp1_int = Data_16bit/10;
								Data_8bit  =  Data_16bit%10;
						}
						else
						{
								temp1_int = temp0_min/10;
								Data_8bit = temp0_min%10;
						}
							if(temp0_min<0)      //Ϊ���¶�
							{
								DisPlayNewStr(X_START+(88+20)*1+36+4,Y_START+392,"-",3,CharacterColor);		//
							}
									else
									{
										DisPlayNewStr(X_START+(88+20)*1+36+4,Y_START+392,"+",3,CharacterColor);		//
									}
									if(abs(temp1_int)<100)
									{
										Display_uint8_Dec2(X_START+(88+20)*1+48+4,Y_START+392,temp1_int,3,CharacterColor);
										num=16;
									}
									else if((abs(temp1_int)>=100)&&((abs(temp1_int)<1000)))
									{
										Display_uint8_Dec3(X_START+(88+20)*1+48+4,Y_START+392,temp1_int,3,CharacterColor);
										num=26;
									}
									else
									{
										Display_uint16_Dec4(X_START+(88+20)*1+48+4,Y_START+392,temp1_int,3,CharacterColor);
										num=35;
									}

									DisPlayNewStr(X_START+(88+20)*1+48+num+4,Y_START+392,".",3,CharacterColor);		//
									Display_uint8_Dec1(X_START+(88+20)*1+54+num+4,Y_START+392,Data_8bit,3,CharacterColor);
								//	DisPlayNewStr(X_START+(88+8)*3+72+num,Y_START+368,"��",3,(UsermenuParameter.FontColor+FONT_COLOR));

					//Display_Hex16bit(X_START+(88+8)*3+40,Y_START+368,temp_area->Area_temp_MIN,3,(UsermenuParameter.FontColor+FONT_COLOR));
					//********************************************************����ƽ��ֵ*********************************************************//
					DisplayBackground(X_START+(88+20)*2+32+4,Y_START+392,8,BACKGROUND_LAYER,0);

							temp1_int = temp_area->Area_temp_AVG>>4;
							Data_8bit = temp_area->Area_temp_AVG & 0x0f;		     //�¶�С������
							temp1_point = Data_8bit*625;   //С������ͳһ����10^4 ,����1λС��
							temp1_point /= 1000;
							if(temp1_int<0)
							{
								temp0_avg = temp1_int*10 - temp1_point;
							}
							else
							{
								temp0_avg = temp1_int*10 + temp1_point;
							}
							temp0_avg -=400;                    // �¶���д��֮ǰ�м�40д��Ĵ�����ʵ����ʾʱ��40��ȥ


							temp1_int = temp0_avg/10;
							if(temp0_avg<0)
							{
								Data_16bit = temp0_avg-1;
								Data_16bit = ~Data_16bit;
								temp1_int = Data_16bit/10;
								Data_8bit  =  Data_16bit%10;
							}
							else
							{
								temp1_int = temp0_avg/10;
								Data_8bit = temp0_avg%10;
							}
							if(temp0_avg<0)      //Ϊ���¶�
								{
									DisPlayNewStr(X_START+(88+20)*2+44+4,Y_START+392,"-",3,CharacterColor);		//
								}
							else
								{
									DisPlayNewStr(X_START+(88+20)*2+44+4,Y_START+392,"+",3,CharacterColor);		//
								}
									if(abs(temp1_int)<100)
									{
											Display_uint8_Dec2(X_START+(88+20)*2+56+4,Y_START+392,temp1_int,3,CharacterColor);
											num=16;
									}
									else if((abs(temp1_int)>=100)&&((abs(temp1_int)<1000)))
									{
											Display_uint8_Dec3(X_START+(88+20)*2+56+4,Y_START+392,temp1_int,3,CharacterColor);
											num=26;
									}
									else
									{
										Display_uint16_Dec4(X_START+(88+20)*2+56+4,Y_START+392,temp1_int,3,CharacterColor);
										num=35;
									}

									DisPlayNewStr(X_START+(88+20)*2+56+num+4,Y_START+392,".",3,CharacterColor);		//
									Display_uint8_Dec1(X_START+(88+20)*2+62+num+4,Y_START+392,Data_8bit,3,CharacterColor);
								//	DisPlayNewStr(X_START+(88+8)*4+72+num,Y_START+368,"��",3,(UsermenuParameter.FontColor+FONT_COLOR));

					//Display_Hex16bit(X_START+(88+8)*4+40,Y_START+368,temp_area->Area_temp_AVG,3,(UsermenuParameter.FontColor+FONT_COLOR));
				}
				DisPlay_AREA_Icon(temp_area,2,CharacterColor);
#endif
}
#endif

#endif

void Spot_SimultaneousDisplay(ts_u16 temp_point_status)
{
	if(temp_point_status & 0x0001) {
		DisPlayIcon(SpotInit[0].spot_ID,SpotInit[0].spot_X_Pos,SpotInit[0].spot_Y_Pos,1);
		Sport_Display_Process(&SpotInit[0]);
	}
	if(temp_point_status & 0x0002) {
		DisPlayIcon(SpotInit[1].spot_ID,SpotInit[1].spot_X_Pos,SpotInit[1].spot_Y_Pos, 1);
		Sport_Display_Process(&SpotInit[1]);
	}
	if(temp_point_status & 0x0004) {
		DisPlayIcon(SpotInit[2].spot_ID,SpotInit[2].spot_X_Pos,SpotInit[2].spot_Y_Pos, 1);
		Sport_Display_Process(&SpotInit[2]);		
	}
	// if(temp_point_status & 0x0008)
	// {
	// 	DisPlayIcon(SpotInit[3].spot_ID,SpotInit[3].spot_X_Pos,SpotInit[3].spot_Y_Pos,1);

	// 	SpotMessage(SpotInit[3].name,SpotInit[3].spot_X_Pos,SpotInit[3].spot_Y_Pos,SpotInit[3].spot_ID);	
	// }
	// if(temp_point_status & 0x0010)
	// {
	// 	DisPlayIcon(SpotInit[4].spot_ID,SpotInit[4].spot_X_Pos,SpotInit[4].spot_Y_Pos,1);

	// 	SpotMessage(SpotInit[4].name,SpotInit[4].spot_X_Pos,SpotInit[4].spot_Y_Pos,SpotInit[4].spot_ID);	
	// }
	// if(temp_point_status & 0x0020)
	// {
	// 	DisPlayIcon(SpotInit[5].spot_ID,SpotInit[5].spot_X_Pos,SpotInit[5].spot_Y_Pos,1);

	// 	SpotMessage(SpotInit[5].name,SpotInit[5].spot_X_Pos,SpotInit[5].spot_Y_Pos,SpotInit[5].spot_ID);	
	// }
	// if(temp_point_status & 0x0040)
	// {
	// 	DisPlayIcon(0,SpotInit[6].spot_X_Pos,SpotInit[6].spot_Y_Pos,2,CharacterColor);

	// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT7_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT7_X_Y__SET,0)&0x00000000)|((SpotInit[6].spot_Y_Pos<<16)+(SpotInit[6].spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
	// 	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT7_AD_TEMP_START,&spot_temp7);     //spotʵ����ʾ���¶�ֵ
	// 	spot_AD7	= APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT7_AD_TEMP_START,0)&0x0000FFFF;
	// 	SpotMessage(SpotInit[6].spot_X_Pos,SpotInit[6].spot_Y_Pos,spot_temp7,spot_AD7,SpotInit[6].spot_ID,3);
	// }
	// if(temp_point_status & 0x0080)
	// {
	// 	DisPlayIcon(0,SpotInit[7].spot_X_Pos,SpotInit[7].spot_Y_Pos,2,CharacterColor);

	// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT8_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT8_X_Y__SET,0)&0x00000000)|((SpotInit[7].spot_Y_Pos<<16)+(SpotInit[7].spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
	// 	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT8_AD_TEMP_START,&spot_temp8);     //spotʵ����ʾ���¶�ֵ
	// 	spot_AD8	= APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT8_AD_TEMP_START,0)&0x0000FFFF;
	// 	SpotMessage(SpotInit[7].spot_X_Pos,SpotInit[7].spot_Y_Pos,spot_temp8,spot_AD8,SpotInit[7].spot_ID,3);
	// }
	// if(temp_point_status & 0x0100)
	// {
	// 	DisPlayIcon(0,SpotInit[8].spot_X_Pos,SpotInit[8].spot_Y_Pos,2,CharacterColor);

	// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOT9_X_Y__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT9_X_Y__SET,0)&0x00000000)|((SpotInit[8].spot_Y_Pos<<16)+(SpotInit[8].spot_X_Pos)));//���ò��µ��Xλ��,���ò��µ��Yλ��
	// 	temp_convert_display_H(( ts_u32 *)IO_APB_SLAVE_0_INPUT_SPOT9_AD_TEMP_START,&spot_temp9);     //spotʵ����ʾ���¶�ֵ
	// 	spot_AD9	= APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT9_AD_TEMP_START,0)&0x0000FFFF;
	// 	SpotMessage(SpotInit[8].spot_X_Pos,SpotInit[8].spot_Y_Pos,spot_temp9,spot_AD9,SpotInit[8].spot_ID,3);
	// }
}

void Display_Deviceinfo(void)
{
	// ts_u16 temp1;
#if 0
//	Erase_Deviceinfo();
	Deviceinfo_Flag = 1;
	if(SystemParaMenu2.LOGO_Flag)
	 {
		DisplayBackgroundColor(X_START,Y_START,33*8,BACKGROUND_LAYER,1);
		DisPlayNewStr(X_START,Y_START," VOT Infrared Technology Co. Ltd ",3,CharacterColor);
	 }
	 else
	 {
		 DisPlayNewStr(X_START,Y_START,"                                   ",3,CharacterColor);
	 }

	 DisplayBackgroundColor(X_START+33*8+32,Y_START,16*8,BACKGROUND_LAYER,1);
	 DisPlayNewStr(X_START+33*8+40,Y_START,Deviceinfo[3][SelectionMenu.IsEnableEnligh],3,CharacterColor);//̽����ID
	 DisPlayNewStr(X_START+33*8+96,Y_START,DetectorID,3,CharacterColor);//̽����ID

	 DisplayBackgroundColor(X_START+(X_GAP*4)+(X_GAP*5)*2,400,22*8,BACKGROUND_LAYER,1);
	 DisPlayNewStr(X_START+(X_GAP*4)+(X_GAP*5)*2,400,Deviceinfo[0][SelectionMenu.IsEnableEnligh],3,CharacterColor);//144  ���к�
	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[0]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)+(X_GAP*5)*8,400,3,CharacterColor);

	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[1]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*2+(X_GAP*5)*8,400,3,CharacterColor);

	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[2]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*3+(X_GAP*5)*8,400,3,CharacterColor);

	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[3]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*4+(X_GAP*5)*8,400,3,CharacterColor);

	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[4]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*5+(X_GAP*5)*8,400,3,CharacterColor);

	 temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[5]);
	 DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*6+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[6]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*7+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[7]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*8+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[8]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*9+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[9]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*10+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[10]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*11+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[11]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*12+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[12]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*13+(X_GAP*5)*8,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.MerchineSN[13]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*14+(X_GAP*5)*8,400,3,CharacterColor);

	DisplayBackgroundColor(X_START+(X_GAP*4)*14+(X_GAP*5)*10+12*SelectionMenu.IsEnableEnligh,400,20*8,BACKGROUND_LAYER,1);
	DisPlayNewStr(X_START+(X_GAP*4)*14+(X_GAP*5)*10+32*SelectionMenu.IsEnableEnligh,400,Deviceinfo[1][SelectionMenu.IsEnableEnligh],3,CharacterColor);//1����=2����ĸ ��ͷ��Ϣ//һ����ĸռ4��
	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[0]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*14+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[1]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*15+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[2]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*16+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[3]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*17+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[4]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*18+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[5]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*19+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[6]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*20+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[7]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*21+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[8]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*22+(X_GAP*5)*18,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.LensSN[9]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*23+(X_GAP*5)*18,400,3,CharacterColor);

	DisplayBackgroundColor(X_START+(X_GAP*4)*23+(X_GAP*5)*20+16*SelectionMenu.IsEnableEnligh,400,20*8,BACKGROUND_LAYER,1);
	DisPlayNewStr(X_START+(X_GAP*4)*23+(X_GAP*5)*20+32*SelectionMenu.IsEnableEnligh,400,Deviceinfo[2][SelectionMenu.IsEnableEnligh],3,CharacterColor);   //��������
	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[0]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*23+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[1]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*24+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[2]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*25+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[3]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*26+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[4]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*27+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[5]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*28+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[6]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*29+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[7]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*30+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[8]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*31+(X_GAP*5)*28,400,3,CharacterColor);

	temp1=FindCharactersID(0,0,SystemParaMenu2.ProduceDATA[9]);
	DisPlaySize16X8((ts_u08 *)Font_TAB[temp1],X_START+(X_GAP*4)*32+(X_GAP*5)*28,400,3,CharacterColor);
#endif
}

void Erase_Deviceinfo(void)
{
	//	Deviceinfo_Flag = 0;
	//	ClearSize16X16(X_START,55*8,Y_START);
	//	ClearSize16X16(X_START,600,400);
}
#if 0
#ifdef Electrolytic_copper
// ���ߺ��� (Bresenham�㷨)
void drawLine(int x0, int y0, int x1, int y1, ts_u16 color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (x0 != x1 || y0 != y1) {
    	Select_CharacterLayer(1,color,TXT_BASE_ADDR + 1280*y0+x0*2);
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawIrregularPolygonBox(Point* vertices, ts_u16 color)
{
    for (int i = 0; i < 4; i++)
    {
        int nextIndex = (i + 1) % 4;
        drawLine(vertices[i].x, vertices[i].y, vertices[nextIndex].x, vertices[nextIndex].y,color);
    }
}

void DisplayQuadTemp(void)
{

	Quad_TSum = APB3_REGR(IO_APB_PAT_0X26,0);
	Quad_PSum = APB3_REGR(IO_APB_PAT_0X27,0);

	if(Quad_PSum == 0)
		return;
	Avg_T_F = (Quad_TSum*1.0)/(Quad_PSum*1.0) - 40.0;
	Avg_T_I = Avg_T_F * 10;

	if((vertices[1].x >555)||(vertices[1].y < 8))
	{
		Quad_Index = 2;
	}
	else
	{
		Quad_Index = 1;
	}
	DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);

	if(Avg_T_I < 0)
	{

		DisPlayNewStr(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,"-",3,CharacterColor);
		if((ts_u16)((~(Avg_T_I-1))/10)<10)
		{
			DataBitNum = 1;
		}
		else if(((ts_u16)((~(Avg_T_I-1))/10)>=10)&&((ts_u16)((~(Avg_T_I-1))/10) < 100))
		{
			DataBitNum = 2;
		}
		else if(((ts_u16)((~(Avg_T_I-1))/10)>=100)&&((ts_u16)((~(Avg_T_I-1))/10) < 1000))
		{
			DataBitNum = 3;
		}
		else
		{
			DataBitNum = 4;
		}
		display_u16number(vertices[Quad_Index].x+16,vertices[Quad_Index].y-8+(Quad_Index-1)*8,(ts_u16)((~(Avg_T_I-1))/10),3,CharacterColor);
		DisPlayNewStr(vertices[Quad_Index].x+16+DataBitNum*8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,".",3,CharacterColor);
		display_u8number(vertices[Quad_Index].x+24+DataBitNum*8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,(ts_u08)((~(Avg_T_I-1))%10),3,CharacterColor);
	}
	else
	{
		DisPlayNewStr(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,"+",3,CharacterColor);
		if(Avg_T_I/10 <10)
		{
			DataBitNum = 1;
		}
		else if((Avg_T_I/10 >=10)&&(Avg_T_I/10 <100))
		{
			DataBitNum = 2;
		}
		else if((Avg_T_I/10 >=100)&&(Avg_T_I/10 <1000))
		{
			DataBitNum = 3;
		}
		else
		{
			DataBitNum = 4;
		}
		display_u16number(vertices[Quad_Index].x+16,vertices[Quad_Index].y-8+(Quad_Index-1)*8,(ts_u16)(Avg_T_I/10),3,CharacterColor);
		DisPlayNewStr(vertices[Quad_Index].x+16+DataBitNum*8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,".",3,CharacterColor);
		display_u8number(vertices[Quad_Index].x+24+DataBitNum*8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,(ts_u08)(Avg_T_I%10),3,CharacterColor);
	}
}


void QuadMoveUp(void)
{
	if((vertices[0].y > 0)&&(vertices[1].y > 0))
	{
		drawIrregularPolygonBox(vertices, 0);
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);

		vertices[0].y--;
		vertices[1].y--;
		vertices[2].y--;
		vertices[3].y--;

		drawIrregularPolygonBox(vertices, CharacterColor);
		DisplayQuadTemp();
	}
}

void QuadMoveDown(void)
{
	if((vertices[2].y < 511)&&(vertices[3].y < 511))
	{
		drawIrregularPolygonBox(vertices, 0);
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);

		vertices[0].y++;
		vertices[1].y++;
		vertices[2].y++;
		vertices[3].y++;

		drawIrregularPolygonBox(vertices, CharacterColor);
		DisplayQuadTemp();
	}
}

void QuadMoveLeft(void)
{
	if((vertices[0].x > 0)&&(vertices[3].x > 0))
	{
		drawIrregularPolygonBox(vertices, 0);
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		vertices[0].x--;
		vertices[1].x--;
		vertices[2].x--;
		vertices[3].x--;

		drawIrregularPolygonBox(vertices, CharacterColor);
		DisplayQuadTemp();
	}
}

void QuadMoveRight(void)
{
	if((vertices[1].x < 639)&&(vertices[2].x < 639))
	{
		drawIrregularPolygonBox(vertices, 0);
		DisplayBackground(vertices[Quad_Index].x+8,vertices[Quad_Index].y-8+(Quad_Index-1)*8,7,3,0);
		vertices[0].x++;
		vertices[1].x++;
		vertices[2].x++;
		vertices[3].x++;

		drawIrregularPolygonBox(vertices, CharacterColor);
		DisplayQuadTemp();
	}
}
void Quad_TemAlarmSet(void)
{
	if(Avg_T_I == 0)
		return;
	if((Avg_T_I >= SystemParaMenu2.Quad_TemAlarm)&&(QuadTemAlarmFlag ==0))
	{
//		send_data.rx_buff[0] = 0x01;
//		send_data.rx_buff[1] = 0x05;
//		send_data.rx_buff[2] = 0x00;
//		send_data.rx_buff[3] = 0x00;
//		send_data.rx_buff[4] = 0xFF;
//		send_data.rx_buff[5] = 0x00;
//		send_data.rx_buff[6] = 0x8C;
//		send_data.rx_buff[7] = 0x3A;

//		for(ts_u08 i = 0; i < 7; i++)
		{
			bsp_printf("A");
			delay_25ms(2);
			bsp_printf("A");
			delay_25ms(2);
			bsp_printf("A");
		}
//		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
		QuadTemAlarmFlag= 1;
	}
	else if((Avg_T_I < SystemParaMenu2.Quad_TemAlarm)&&(QuadTemAlarmFlag ==1))
	{
//		send_data.rx_buff[0] = 0x01;
//		send_data.rx_buff[1] = 0x05;
//		send_data.rx_buff[2] = 0x00;
//		send_data.rx_buff[3] = 0x00;
//		send_data.rx_buff[4] = 0x00;
//		send_data.rx_buff[5] = 0x00;
//		send_data.rx_buff[6] = 0xCD;
//		send_data.rx_buff[7] = 0xCA;
//
//		for(ts_u08 i = 0; i < 7; i++)
//		{
//			bsp_printf("%c",send_data.rx_buff[i]);
//		}
//		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
		bsp_printf("a");
		delay_25ms(2);
		bsp_printf("a");
		delay_25ms(2);
		bsp_printf("a");
		QuadTemAlarmFlag= 0;
	}
}

#endif

unsigned int GetPixelMaxAndMinValue(void)
{
#ifdef VOT
	ts_u16 Get_Spot_Minx = 0,Get_Spot_Miny = 0;
	ts_u16 Get_Spot_Maxx = 0,Get_Spot_Maxy = 0;
	ts_u08 PixelPositionIndex = 0;
	ts_u16 ADValueAverage = 0;
	ts_u16 ADMinValue = 0,ADMaxValue = 0;

	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xFFFFBFFF)|0x4000);

	Area_LxLy(100,100);
	Area_RxRy(228,228);

	Get_Spot_Minx = ReadBadADMin_X();//��������ϵ���һ�δ�FPGA�ᴫ��(0,0)���굼��ʮ�ֹ�겻��ʾ�����
	Get_Spot_Miny = ReadBadADMin_Y();

	Get_Spot_Maxx = ReadBadADMax_X();
	Get_Spot_Maxy = ReadBadADMax_Y();

	Get_Spot_Minx = 164;
	Get_Spot_Miny = 164;

	delay_1ms(100);
  	for(unsigned int j=0; j<512; j = j+64)//��
	{
		for(unsigned int i=0; i<640; i = i+64)//��
		{

			Area_LxLy(i,j);
			Area_RxRy(i+63,j+63);

			delay_1ms(80);

			Get_Spot_Minx = ReadBadADMin_X();
			Get_Spot_Miny = ReadBadADMin_Y();

			Get_Spot_Maxx = ReadBadADMax_X();
			Get_Spot_Maxy = ReadBadADMax_Y();


			if((Get_Spot_Minx != 0)&&(Get_Spot_Miny != 0))
			{
				PixelPositionArray[PixelPositionIndex][0] = Get_Spot_Minx;
				PixelPositionArray[PixelPositionIndex][1] = Get_Spot_Miny;
				PixelPositionArray[PixelPositionIndex][2] = Get_Spot_Maxx;
				PixelPositionArray[PixelPositionIndex][3] = Get_Spot_Maxy;
			}
			PixelPositionIndex++;
		}
	}
  	PixelPositionIndex = 0;
  	// ADValueAverage = ReadImgRaw_Avg();
  	//bsp_printf("ADV:%x\r\n",ADValueAverage);
  	for(unsigned int j=0; j<512; j = j+64)//��
	{
		for(unsigned int i=0; i<640; i = i+64)//��
		{
			Area_LxLy(i,j);
			Area_RxRy(i+63,j+63);

			delay_1ms(80);

			Get_Spot_Minx = ReadBadADMin_X();
			Get_Spot_Miny = ReadBadADMin_Y();

			Get_Spot_Maxx = ReadBadADMax_X();
			Get_Spot_Maxy = ReadBadADMax_Y();

			ADMinValue = (APB3_REGR(IO_APB_PAT_0X36,0)&0X0000FFFF);
			ADMaxValue = (APB3_REGR(IO_APB_PAT_0X36,0)>>16);

			if((Get_Spot_Minx != 0)&&(Get_Spot_Miny != 0))
			{
				if((PixelPositionArray[PixelPositionIndex][0] == Get_Spot_Minx)&&
						(PixelPositionArray[PixelPositionIndex][1] == Get_Spot_Miny))
				{
					if(SystemParaMenu2.piexl_offset)
					{
						if((ADMinValue < ADValueAverage)&&(ADValueAverage - ADMinValue >= SystemParaMenu2.piexl_offset))
						{
							if(SystemParaMenu2.BAD_Cross)
							{
								SOC_ReadAndWrite_Hyram(1);
								DisPlaySpot(Get_Spot_Minx-1,Get_Spot_Miny-2,3,6,1);
								SOC_ReadAndWrite_Hyram(0);
							}
//							BAD_MarkOpr(Get_Spot_Minx-1,Get_Spot_Miny-1);
							BAD_MarkOpr(Get_Spot_Minx,Get_Spot_Miny);
							//bsp_printf("(SpotMinPOS:%d,%d):%x\r\n",Get_Spot_Minx-1,Get_Spot_Miny-2,(APB3_REGR(IO_APB_PAT_0X36,0)&0X0000FFFF));
						}
						else
						{
							PixelPositionArray[PixelPositionIndex][0] = 0;
							PixelPositionArray[PixelPositionIndex][1] = 0;
						}
					}
					else
					{
						if(SystemParaMenu2.BAD_Cross)
						{
							SOC_ReadAndWrite_Hyram(1);
							DisPlaySpot(Get_Spot_Minx-1,Get_Spot_Miny-2,3,6,1);
							SOC_ReadAndWrite_Hyram(0);
						}
//						BAD_MarkOpr(Get_Spot_Minx-1,Get_Spot_Miny-1);
						BAD_MarkOpr(Get_Spot_Minx,Get_Spot_Miny);
						//bsp_printf("(SpotMinPOS:%d,%d):%x\r\n",Get_Spot_Minx-1,Get_Spot_Miny-2,(APB3_REGR(IO_APB_PAT_0X36,0)&0X0000FFFF));
					}
				}
				else
				{
					PixelPositionArray[PixelPositionIndex][0] = 0;
					PixelPositionArray[PixelPositionIndex][1] = 0;
				}

				if((PixelPositionArray[PixelPositionIndex][2] == Get_Spot_Maxx)&&
						(PixelPositionArray[PixelPositionIndex][3] == Get_Spot_Maxy))
				{
					if(SystemParaMenu2.piexl_offset)
					{
						if((ADMaxValue>ADValueAverage)&&(ADMaxValue - ADValueAverage >= SystemParaMenu2.piexl_offset))
						{
							if(SystemParaMenu2.BAD_Cross)
							{
								SOC_ReadAndWrite_Hyram(1);
								DisPlaySpot(Get_Spot_Maxx-1,Get_Spot_Maxy-2,3,6,1);
								SOC_ReadAndWrite_Hyram(0);
							}
//							BAD_MarkOpr(Get_Spot_Maxx-1,Get_Spot_Maxy-1);
							BAD_MarkOpr(Get_Spot_Maxx,Get_Spot_Maxy);
							//bsp_printf("(SpotMaxPOS:%d,%d):%x\r\n",Get_Spot_Maxx-1,Get_Spot_Maxy-2,(APB3_REGR(IO_APB_PAT_0X36,0)>>16));
						}
						else
						{
							PixelPositionArray[PixelPositionIndex][2] = 0;
							PixelPositionArray[PixelPositionIndex][3] = 0;
						}
					}
					else
					{
						if(SystemParaMenu2.BAD_Cross)
						{
							SOC_ReadAndWrite_Hyram(1);
							DisPlaySpot(Get_Spot_Maxx-1,Get_Spot_Maxy-2,3,6,1);
							SOC_ReadAndWrite_Hyram(0);
						}
//						BAD_MarkOpr(Get_Spot_Maxx-1,Get_Spot_Maxy-1);
						BAD_MarkOpr(Get_Spot_Maxx,Get_Spot_Maxy);
						//bsp_printf("(SpotMaxPOS:%d,%d):%x\r\n",Get_Spot_Maxx-1,Get_Spot_Maxy-2,(APB3_REGR(IO_APB_PAT_0X36,0)>>16));
					}

				}
				else
				{
					PixelPositionArray[PixelPositionIndex][2] = 0;
					PixelPositionArray[PixelPositionIndex][3] = 0;
				}
			}
			PixelPositionIndex++;
		}
	}
	APB3_REGW(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOTN_SET,0)&0xFFFFBFFF)|0x0000);
	return 0;
#endif
}

unsigned int CancalPixelMaxAndMinValue(void)
{
#ifdef VOT
	for(ts_u08 PixelPositionIndex = 0;PixelPositionIndex < 80;PixelPositionIndex++)
	{
		if((PixelPositionArray[PixelPositionIndex][0] != 0)&&
								(PixelPositionArray[PixelPositionIndex][1] != 0))
		{
			BAD_CancelOpr(PixelPositionArray[PixelPositionIndex][0]-1,PixelPositionArray[PixelPositionIndex][1]-1);
			if(SystemParaMenu2.BAD_Cross)
			{
				SOC_ReadAndWrite_Hyram(1);
				DisPlaySpot(PixelPositionArray[PixelPositionIndex][0]-1,PixelPositionArray[PixelPositionIndex][1]-2,3,0,1);
				SOC_ReadAndWrite_Hyram(0);
			}
		}
		if((PixelPositionArray[PixelPositionIndex][2] != 0)&&
				(PixelPositionArray[PixelPositionIndex][3] != 0))
		{
			BAD_CancelOpr(PixelPositionArray[PixelPositionIndex][2]-1,PixelPositionArray[PixelPositionIndex][3]-1);
			if(SystemParaMenu2.BAD_Cross)
			{
				SOC_ReadAndWrite_Hyram(1);
				DisPlaySpot(PixelPositionArray[PixelPositionIndex][2]-1,PixelPositionArray[PixelPositionIndex][3]-2,3,0,1);
				SOC_ReadAndWrite_Hyram(0);
			}
		}
	}
	return 0;
#endif
}
#endif
