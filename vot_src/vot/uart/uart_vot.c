/*
 * uart_vot.c
 *
 *  Created on: 2023年10月17日
 *      Author: Administrator
 */

#include "uart_vot.h"
#include "menu/name.h"
#include <stdarg.h>
#include <uapi/upl_v100.h>
#include <menu/ispfunction.h>
#include <menu/testmenufunction.h>
#include <menu/usermenufunction.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <menu/usermenu.h>
#include <snr_comm_vot.h>
#include "sffs_files_vot.h"
#include "sffs_files.h"
#include "snr_comm.h"
#include <uapi/isp_v100.h>

u8 TShutFun = 0;
u8 BADCorrectFlag = 0;
struct VOT_TOOLS VotTools;
struct VOT_TOOLS VotFPGAData;
struct VOT_Parameters VotParameters = {0};
struct VOT_Arrays VotArrays = {{"UCC-20240118P1"}, {"20240118P1"}, {"2024.01.18"}, {0}};
// 温度采集线程管理变量
// static ts_cpl_task_s_t g_temp_collect_thread;
// static ts_u32 g_temp_collect_thread_running = 0;

ts_u08 Area_TemCalFlag = 0;
u8 MaxTempAlarmFunSW = 0;
u32 MaxTempLimit = 0;
u32 OverPixNum = 0;
u8 Pulse_Sig = 1;
/******************VOT_UART*********************/
COM_data com_data;
SEND_data send_data;
extern SysCtrlInfo_s_t global_sys_ctrl_info;

ParaMsgSwap_S g_stParaMsgSwap;
void *p_temp_buff;	  // 申请3M内存用于存放上传参数到上位机
void *p_memrecv_buff; // 申请3M内存用于存放上位机下发的参数
extern void NPU_SR_ParamAvailable(void);

ts_u08 check_asic_firmware_version_info(ts_u08 *input_byte, AsicFirmwareVersion_s_t* version_info)
{
	
    char part1[10], part2[10], part3[10], part4[10], part5[10], part6[10],part7[10]; 
    
 	// 使用sscanf提取各部分，保留前导零
    int result = sscanf((ts_char*)input_byte, "%9[^.].%9[^.].%9[^.].%9[^.].%9[^.].%9[^.].%9s", part1, part2, part3, part4, part5, part6,part7 );
    
    if (result == 7) {
		vision_log_debug("get %d \n", result);
        vision_log_debug(" %s\n", part1);
        vision_log_debug(" %s\n", part2);
        vision_log_debug(" %s\n", part3);
        vision_log_debug(" %s\n", part4);
        vision_log_debug(" %s\n", part5);
        vision_log_debug(" %s\n", part6);
		vision_log_debug(" %s\n", part7);
    } else {
        vision_log_debug("error get %d \n", result);
        return 1;
    }

   // 转换为十六进制输出
    // vision_log_debug("\n=== to hex ===\n");
    unsigned long val1, val2, val3, val4, val5, val6,val7 ;
    
    // 使用strtoul转换，支持十进制和十六进制输入
    val1 = strtoul(part1, NULL, 10); // 按十进制解析
    val2 = strtoul(part2, NULL, 10);
    val3 = strtoul(part3, NULL, 10);
    val4 = strtoul(part4, NULL, 10);
    val5 = strtoul(part5, NULL, 10);
    val6 = strtoul(part6, NULL, 10);
	val7 = strtoul(part7, NULL, 10);
    
    // vision_log_debug(" 0x%lX\n", val1);
    // vision_log_debug(" 0x%lX\n", val2);
    // vision_log_debug(" 0x%lX\n", val3);
    // vision_log_debug(" 0x%lX\n", val4);
    // vision_log_debug(" 0x%lX\n", val5);
    // vision_log_debug(" 0x%lX\n", val6);
	// vision_log_debug(" 0x%lX\n", val7);

	if(version_info){
		version_info->customer_id = val1;
		version_info->major_version = val2;
		version_info->minor_version = val3;
		version_info->build_version = val4;
		version_info->function_id = val5;
		version_info->extend_function_id = val6;
		version_info->test_id = val7;

		vision_log_debug("version_info->customer_id %lu \n", version_info->customer_id);
		vision_log_debug("version_info->major_version %lu \n", version_info->major_version);
		vision_log_debug("version_info->minor_version %lu \n", version_info->minor_version);
		vision_log_debug("version_info->build_version %lu \n", version_info->build_version);
		vision_log_debug("version_info->function_id %lu \n", version_info->function_id);
		vision_log_debug("version_info->extend_function_id %lu \n", version_info->extend_function_id);
		vision_log_debug("version_info->test_id %lu \n", version_info->test_id);
		
	}


	return 0;
}

/**
 * @description: 检查版本信息是否满足升级要求，
 * @param {AsicFirmwareTail_s_t*} 固件尾部信息
 * @return {ts_u08}，返回值 0：表示合适的固件，非0值：表示固件版本错误
 */
ts_u08 check_asic_firmware_version(AsicFirmwareTail_s_t *firm_s)
{
	// 版本信息：如"000.1.0.01.100000.000","000.1.0.1.100000.000"
	ts_u08 cur_version[64];
	// memcpy(cur_version,VOT_SOFTWARE_VER_FIXED_HEADER,strlen(VOT_SOFTWARE_VER_FIXED_HEADER));
	snprintf((ts_char *)cur_version, 64, "%s", VOT_SOFTWARE_VER_FIXED_HEADER);

	// // 客户类型不一致
	// if (!(firm_s->version[0] == cur_version[0] && firm_s->version[1] == cur_version[1] && firm_s->version[2] == cur_version[2]))
	// {
	// 	return 1;
	// }

	AsicFirmwareVersion_s_t flash_version_info;
	AsicFirmwareVersion_s_t new_version_info;
	check_asic_firmware_version_info(firm_s->version,&flash_version_info);
	check_asic_firmware_version_info(cur_version,&new_version_info);
	// 硬件平台一致：机芯平台、探测器类型，探测器分辨率
	//  if( !(firm_s->version[11] == cur_version[11] && firm_s->version[12] == cur_version[12] && firm_s->version[13] == cur_version[13])){
	//      return 1;
	//  }

	// ts_u08 sub_version = (firm_s->version[8] - 0x30)*10 + ((firm_s->version[9] - 0x30));
	// ts_u08 will_board_type_version =  (firm_s->version[18] - 0x30)*100 +  (firm_s->version[19] - 0x30)*10 +  (firm_s->version[20] - 0x30);

	// 兼容1.0.10版本以前的固件升级
	//  if(sub_version>=10){
	//  	if( !(firm_s->version[18] == cur_version[18] && firm_s->version[19] == cur_version[19] && firm_s->version[20] == cur_version[20])){
	//  		vision_log_debug("sub_version %d will_board_type_version %d \n",sub_version, will_board_type_version);
	//  		return 1;
	//  	}
	//  }

	return 0;
}

void Asic_Update_Comm_Data(ts_u08 *msg_ptr, ts_u16 msg_len)
{

	if (msg_ptr && msg_len > 0)
	{
		com_data.count = msg_len;
		memcpy(com_data.buff, msg_ptr, msg_len);
	}
	else
	{
		com_data.count = 0;
	}

	// vot_show_packet(msg_ptr, msg_len, "PC->A");
}

void Asic_Clear_Processed_Data(void)
{
	if (com_data.count)
	{
		memset(com_data.buff, 0, sizeof(com_data.buff));
	}
	com_data.count = 0;
	VotTools.END_FLAG = 0;
}

void ASIC_Data_Packets_Receive_And_Process(void)
{
	for (VotTools.DATA_LENGTH = 0; VotTools.DATA_LENGTH < com_data.count; VotTools.DATA_LENGTH++)
	{
		switch (VotTools.STATUS_FLAG)
		{
		default:
			VotTools.STATUS_FLAG = 0;
			break;
		case 0:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head1)
				VotTools.STATUS_FLAG = 1;
			break;
		case 1:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head2)
				VotTools.STATUS_FLAG = 2;
			else
			{
				MessageReturnState(UnframedHeader);
				VotTools.STATUS_FLAG = 0;
				VotTools.DATA_LENGTH--;
			}
			break;
		case 2:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head3)
				VotTools.STATUS_FLAG = 3;
			else
			{
				MessageReturnState(UnframedHeader);
				VotTools.STATUS_FLAG = 0;
				VotTools.DATA_LENGTH--;
			}
			break;
		case 3:
			VotTools.HEAD_FLAG = VotTools.DATA_LENGTH - 3;
			int high_byte_offset = VotTools.DATA_LENGTH;
			int low_byte_offset = ++VotTools.DATA_LENGTH;
			VotTools.COMMAND_LENGTH = ((com_data.buff[high_byte_offset] << 8) | com_data.buff[(low_byte_offset)]);
			Data_Verification(&com_data.buff[VotTools.DATA_LENGTH - 4], VotTools.COMMAND_LENGTH + 3);
			if ((com_data.buff[VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH - 1] == VotArrays.Rcvbuf[0] &&
				 com_data.buff[VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH] == VotArrays.Rcvbuf[1]))
				VotTools.STATUS_FLAG = 4;
			else
			{
				if ((VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH + 1 <= com_data.count) && (VotTools.COMMAND_LENGTH != 0))
				{
					MessageReturnState(CalibrationFailure);
					VotTools.END_FLAG = VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH + 1;
				}
				VotTools.STATUS_FLAG = 0;
			}
			break;
		case 4:
			if (com_data.buff[VotTools.DATA_LENGTH + 2] != 0x3A && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x48 && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x55 && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x56)
			{
#ifndef SUPPORT_BoYu_UART_PROTOCOL
				MessageReturnState(ReceivedSuccess);
#endif
			}
			if (global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_UPDATE_FIRMWARE)
			{
				// 避免指定异常，进入锁定在升级模式
				if (com_data.buff[VotTools.DATA_LENGTH + 2] != 0x3A)
				{
					global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
				}
			}

			ts_cpl_msleep(10);
			ASIC_Valid_Data_Handle(&com_data.buff[VotTools.DATA_LENGTH - 5]);
			// 更新索引值
			VotTools.DATA_LENGTH = VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH - 1;
			VotTools.END_FLAG = VotTools.DATA_LENGTH + 1;
			VotTools.STATUS_FLAG = 0;
			break;
		}
	}
	Asic_Clear_Processed_Data();
}

void ASIC_Valid_Data_Handle(u8 *Valid_data)
{
	// vision_log_debug("enter:  !\n");
	// if(global_sys_ctrl_info.m_enter_BPC_manual_flag==1){
	// 	send_data.rx_buff[3] = 0x00;//返回信息长度
	// 	send_data.rx_buff[4] = 0x06;
	// 	vot_uart_handle_for_BPC(Valid_data);
	// 	send_data.rx_buff[8] = Valid_data[8];
	// }
	// else
	// vision_log_debug("enter: Valid_data[7] %d !\n",Valid_data[7]);
	switch (Valid_data[7])
	{

	default:
		Menu_Two_Protocol(Valid_data);
		// MessageReturnState(CommandNotSupported);
		return;

	case 0xFE:
		UART_OPR_TEST(Valid_data);
		break; // Test1   暂无
	case 0xFF:
		UART_OPR_KEY(Valid_data);
		break; // 按键传输协议
	case 0x04:
		UART_OPR_TemplutCollect_Data(Valid_data);
		break;//采集模式：支持两种模式采集单点采集和连续录制
	case 0x05:
		UART_OPR_TemCalibration_T_Set(Valid_data);
		break;//设置温度标定温度数据 PC——>机芯
	case 0x06:
		UART_OPR_TemCalibration_AD_Set(Valid_data);
		break;//设置温度标定AD数据	 PC——>机芯
	case 0x07:
		UART_OPR_TemCalibration_Read(Valid_data);
		break;//回读温度标定数据	 机芯——>PC
	case 0x08:
		UART_OPR_Set_TemptestCollect(Valid_data);
		break;//设置采集的时间间隔 	
	case 0x09:
		UART_OPR_Temp_correct_Set(Valid_data);
		break;//设置温度标定全局偏移值 	  PC——>机芯
	case 0x0E:
		UART_OPR_GetDeviceInfo(Valid_data);
		break; // 公司名称、镜头信息等数据
	case 0x13:
		UART_OPR_SET_Palette(Valid_data);
		break; // 切换伪彩
	case 0x14:
		UART_OPR_Set_Mirror(Valid_data);
		break; // 设置镜像模式
	case 0x17:
		UART_OPR_Set_ISP_AGC02_CFG(Valid_data);
		break; // 设置截止灰度最小值/最大值
	case 0x18:
		UART_OPR_SET_DDE_Level(Valid_data);
		break; // 切换DDE
	case 0x19:
		UART_OPR_SET_Bright(Valid_data);
		break; // 设置亮度
	case 0x1A:
		UART_OPR_SET_Contrast(Valid_data);
		break; // 设置对比度
	case 0x1D:
		UART_OPR_SET_AutoShutter(Valid_data);
		break; // 设置自动打档时间
	case 0x1E:
		UART_OPR_SET_SFilter(Valid_data);
		break; // 设置空间滤波
	case 0x1F:
		UART_OPR_SET_Enlarge(Valid_data);
		break;														 // 设置电子放大
	/*case 0x20:	UART_OPR_SET_TFilter(Valid_data);		break;*/ // 设置时域滤波  暂无
	case 0x21:
		UART_OPR_Save_Menu1Par(Valid_data);
		break; // 保存参数设置
	case 0x22:
		UART_OPR_Temp_Drift(Valid_data);
		break;//温飘补偿
	case 0x24:
		UART_OPR_Temp_correct_Read(Valid_data);
		break;//回读机芯标定的全局偏移值  机芯——>PC
	case 0x25:
		UART_OPR_UserTempFrame_Mode(Valid_data);
		break;// 控制温度矩阵输出原始AD/测温AD/查表温度
	case 0x26:
		UART_OPR_Temp_Meas_Range(Valid_data);
		break;//测温档位切换功能设置
	case 0x27:
		UART_OPR_Set_TempGear_MaxLimit(Valid_data);
		break;//测温档位量程限制
	case 0x29:
		UART_OPR_Set_Read_SmartData(Valid_data);
		break; // 探测器408bit数据回读 //bit407-bit0, 高位在前，低位在后
	case 0x28:
		UART_OPR_Set_Write_SmartData(Valid_data);
		break; // UCC3代光智探测器408bit数据传输
	case 0x2A:
		UART_OPR_ReadWriteTempParam(Valid_data);
		break; // 修改/获取设备的一系列测温参数
	case 0x2B:
		UART_OPR_OutputFormat_Ctrl(Valid_data);
		break;	// 串口设置硬件接口输出数据格式
	case 0x2C:
		UART_OPR_TEMP_AD_TO_RAW_AD(Valid_data);
		break;	// 测温AD转换成原始AD
	case 0x2D:
		UART_OPR_Set_AutoShutterByMin(Valid_data);
		break; // 自动打档时间间隔（精度0.5 MIN）
	case 0x37:
		UART_OPR_Save_AllParameter(Valid_data);
		break; // 一键保存菜单1，菜单2，NUC,OOC,坏点等数据
	case 0x3C:
		UART_OPR_ReadPointTemp_Multi(Valid_data);
		break;//点测温功能相关设置
	case 0x3D:
		UART_OPR_ReadAreaTemp_Multi(Valid_data);
		break;//区域测温功能相关设置
	case 0x3E:
		UART_OPR_ReadCaptureTemp_Multi(Valid_data);
		break;//捕捉点功能相关设置
	/*
	case 0x3F:	UART_OPR_SET_HOST_SW(Valid_data);  		break;//去横纹开关  暂无
	case 0x40:	UART_OPR_SET_DEST_SW(Valid_data);		break;//去竖纹开关  暂无
	case 0x41:	UART_OPR_SET_Gmax(Valid_data);			break;//DDE_MODE切换功能已取消   SystemParaMenu2.Gmax 使用默认值  暂无
	*/
	case 0x42:
		UART_OPR_TempCalib_Second_Function(Valid_data); // 二次校准相关功能
		break;
	case 0x44:
		UART_OPR_Correct_Bad_Pix(Valid_data);
		break; // 机芯一键校正坏点功能(功能未完善)
	case 0x47:  
		UART_OPR_AutoFocus(Valid_data);		
		break;//自动调焦相关功能	
	case 0x4B:
		UART_OPR_Sunlight_Protection(Valid_data);
		break; // 阳光保护功能设置
	case 0x50:
		UART_OPR_SET_NUC(Valid_data);
		break; // NUC校正相关
	case 0x58:
		UART_OPR_Semi_Automatic(Valid_data);
		break; // 半自动校正坏点操作
	case 0x5F:	
		UART_OPR_FollowFocus(Valid_data);	
		break;//跟焦功能相关
	/*
	case 0x60:	UART_OPR_SET_Enlarge_ShowMes(Valid_data);break;//设置电子放大信息显示与否	暂无
	*/
	case 0x63:
		UART_OPR_Correct_One_Badpixel(Valid_data);
		break; // 设置单个坐标点校正坏点
	case 0x64:
		UART_OPR_SET_OCC(Valid_data);
		break; // OOC校正相关
	case 0x65:
		UART_OPR_OneClick(Valid_data);
		break; // 机芯一键操作功能（菜单参数初始化、保存操作等）
	case 0x66:
		UART_OPR_SET_BPC(Valid_data);
		break; // 坏斑校正功能相关
	case 0x68:
		UART_OPR_ShutterControl(Valid_data);
		break; // 挡片控制开关 功能协议
	case 0x6C:
		UART_OPR_ReadParMenu1(Valid_data);
		break; // 回读菜单一设置功能
	case 0x72:
		UART_OPR_Get_Firmware_Version(Valid_data);
		break; // 获取机芯固件版本信息
	case 0x77:
		UART_OPR_PC_Correct_Badpixel(Valid_data);
		break; // PC校正坏点相关功能
	case 0x78:
		UART_OPR_SET_Env_Mode(Valid_data);
		break; // 场景Env_Mode功能调节
	case 0x7D:
		UART_OPR_AllData_Backup(Valid_data);
		break; // 机芯数据导入导出功能
	case 0x3A:
		UART_OPR_OnlineUupgrade(Valid_data);
		break; // SOC固件升级
	case 0xF5:
		UART_OPR_ImportJsonParameters(Valid_data);
		break; // 机芯默认菜单配置参数导入功能
	case 0xF6:
		UART_OPR_ExportJsonParameters(Valid_data);
		break; // 机芯默认菜单配置参数导出功能
	}

	/*判断是否已发送不支持的指令*/
	if (send_data.rx_buff[3] == 0x00 && send_data.rx_buff[4] == 0x00)
	{
		return;
	}

	Return_Message_Processing();
	memset(send_data.rx_buff, 0, sizeof(send_data.rx_buff));
}

void Data_Packets_Receive_And_Process(void)
{
	for (VotTools.DATA_LENGTH = 0; VotTools.DATA_LENGTH < com_data.count; VotTools.DATA_LENGTH++)
	{
		switch (VotTools.STATUS_FLAG)
		{
		default:
			VotTools.STATUS_FLAG = 0;
			break;
		case 0:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head1)
				VotTools.STATUS_FLAG = 1;
			break;
		case 1:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head2)
				VotTools.STATUS_FLAG = 2;
			else
			{
				MessageReturnState(UnframedHeader);
				VotTools.STATUS_FLAG = 0;
				VotTools.DATA_LENGTH--;
			}
			break;
		case 2:
			if (com_data.buff[VotTools.DATA_LENGTH] == Vot_command_head3)
				VotTools.STATUS_FLAG = 3;
			else
			{
				MessageReturnState(UnframedHeader);
				VotTools.STATUS_FLAG = 0;
				VotTools.DATA_LENGTH--;
			}
			break;
		case 3:
			VotTools.HEAD_FLAG = VotTools.DATA_LENGTH - 3;
			int high_byte_offset = VotTools.DATA_LENGTH;
			int low_byte_offset = ++VotTools.DATA_LENGTH;
			VotTools.COMMAND_LENGTH = ((com_data.buff[high_byte_offset] << 8) | com_data.buff[(low_byte_offset)]);
			
			Data_Verification(&com_data.buff[VotTools.DATA_LENGTH - 4], VotTools.COMMAND_LENGTH + 3);
			if ((com_data.buff[VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH - 1] == VotArrays.Rcvbuf[0] &&
				 com_data.buff[VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH] == VotArrays.Rcvbuf[1]))
				VotTools.STATUS_FLAG = 4;
			else
			{
				if ((VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH + 1 <= com_data.count) && (VotTools.COMMAND_LENGTH != 0))
				{
					MessageReturnState(CalibrationFailure);
					VotTools.END_FLAG = VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH + 1;
				}
				VotTools.STATUS_FLAG = 0;
			}
			break;
		case 4:
			if (com_data.buff[VotTools.DATA_LENGTH + 2] != 0x3A && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x48 && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x55 && com_data.buff[VotTools.DATA_LENGTH + 2] != 0x56)
			{
#ifndef SUPPORT_BoYu_UART_PROTOCOL
				MessageReturnState(ReceivedSuccess);
#endif
			}
			//		 		Valid_Data_Handle(&com_data.buff[VotTools.DATA_LENGTH-5]);
			VotTools.DATA_LENGTH = VotTools.DATA_LENGTH + VotTools.COMMAND_LENGTH - 1;
			VotTools.END_FLAG = VotTools.DATA_LENGTH + 1;
			VotTools.STATUS_FLAG = 0;
			break;
		}
	}
	Clear_Processed_Data();
}
#ifdef SUPPORT_BoYu_UART_PROTOCOL

void UartHandle_0x61(u8 *p)
{
	u8 flag = 0;
	u16 command = 0;
	u32 data = 0;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x0B;

	DisplayBackground(320, 260, 20, BACKGROUND_LAYER, BackgroundColor);
	Display_Hex16bit(320, 260, *(p + 8), 3, CharacterColor);
	Display_Hex16bit(320 + 8 * 5, 260, *(p + 10), 3, CharacterColor);
	Display_Hex16bit(320 + 8 * 10, 260, *(p + 12), 3, CharacterColor);

	switch (*(p + 8))
	{
	case 0x10:
		switch (*(p + 9))
		{
		case 0x02:
			flag = 1;
			break; // 自动聚焦到标定位置
		case 0x06:
			flag = 1;
			break;
		}
		break;

	case 0x20:
		switch (*(p + 9))
		{
		case 0x01:
			flag = 1;
			break; // 单步聚焦+
		case 0x02:
			flag = 1;
			break;
		case 0x03:
			flag = 1;
			break;
		case 0x04:
			flag = 1;
			break;
		case 0x05:
			flag = 1;
			break;
		}
		break;

	default:
		//			MessageReturnState(CommandNotSupported);
		break;
	}
	if (flag)
	{
		memcpy(send_data.rx_buff + 8, p + 8, 6);
		command = *(p + 8) << 8 | *(p + 9);
		data = *(p + 10) << 24 | *(p + 11) << 16 | *(p + 12) << 8 | *(p + 13) << 0;

		BoYuUartOPR(command, data);
	}
}
void UartHandle_0x62(u8 *p)
{
	u8 flag = 0;
	u16 command = 0;
	u32 data = 0;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x0B;

	switch (*(p + 8))
	{
	case 0x30:
		switch (*(p + 9))
		{
		case 0x01:
			flag = 1;
			break; // 单步变倍+
		case 0x02:
			flag = 1;
			break;
		case 0x03:
			flag = 1;
			break;
		case 0x04:
			flag = 1;
			break;
		case 0x05:
			flag = 1;
			break;
		}
		break;

	default:
		//			MessageReturnState(CommandNotSupported);
		break;
	}
	if (flag)
	{
		memcpy(send_data.rx_buff + 8, p + 8, 6);
		command = *(p + 8) << 8 | *(p + 9);
		data = *(p + 10) << 24 | *(p + 11) << 16 | *(p + 12) << 8 | *(p + 13) << 0;
		BoYuUartOPR(command, data);
	}
}
#endif

void Clear_Processed_Data(void)
{
	u16 i, j;
	//	int CHECKSUM = 0;
	u8 RESIDUAL_DATA[DATA_MAX_NUM];
#ifndef SUPPORT_ZhongLin_UART_PROTOCOL
	if ((VotTools.END_FLAG == com_data.count) || (com_data.count >= DATA_MAX_NUM))
	{
#endif
		memset(com_data.buff, 0, sizeof(com_data.buff));
		com_data.count = 0;
		VotTools.END_FLAG = 0;
#ifndef SUPPORT_ZhongLin_UART_PROTOCOL
	}
	else
	{
		for (i = VotTools.END_FLAG, j = 0; i < com_data.count; i++, j++)
		{
			RESIDUAL_DATA[j] = com_data.buff[i];
		}
		memset(com_data.buff, 0, sizeof(com_data.buff));
		for (i = 0; i < j; i++)
		{
			com_data.buff[i] = RESIDUAL_DATA[i];
		}
		com_data.count = j;
		VotTools.END_FLAG = 0;
	}
#endif
}

void Return_Message_Processing(void)
{
	send_data.rx_buff[0] = Vot_command_head1;
	send_data.rx_buff[1] = Vot_command_head2;
	send_data.rx_buff[2] = Vot_command_head3;
	send_data.rx_buff[5] = com_data.buff[VotTools.HEAD_FLAG + 5];
	send_data.rx_buff[6] = 0xCA;
	send_data.rx_buff[7] = com_data.buff[VotTools.HEAD_FLAG + 7];

	VotTools.RETURN_LENGTH = ((send_data.rx_buff[3] << 8) | send_data.rx_buff[4]);
	Data_Verification(send_data.rx_buff, VotTools.RETURN_LENGTH + 3);
	send_data.rx_buff[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
	send_data.rx_buff[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];
	// for(u16 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
	// {
	// 	//bsp_printf("%c",send_data.rx_buff[i]);
	// }

	if (global_sys_ctrl_info.m_transfer_callback)
	{
		(*global_sys_ctrl_info.m_transfer_callback)(send_data.rx_buff, VotTools.RETURN_LENGTH + 5);
	}
	// vot_uart_write(send_data.rx_buff,VotTools.RETURN_LENGTH+5);
	//	memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
}

void Return_Message_Processing_with_pipeline_number(ts_u08 pipeline_number)
{
	send_data.rx_buff[0] = Vot_command_head1;
	send_data.rx_buff[1] = Vot_command_head2;
	send_data.rx_buff[2] = Vot_command_head3;
	send_data.rx_buff[5] = pipeline_number;
	send_data.rx_buff[6] = 0xCA;
	send_data.rx_buff[7] = com_data.buff[VotTools.HEAD_FLAG + 7];

	VotTools.RETURN_LENGTH = ((send_data.rx_buff[3] << 8) | send_data.rx_buff[4]);
	Data_Verification(send_data.rx_buff, VotTools.RETURN_LENGTH + 3);
	send_data.rx_buff[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
	send_data.rx_buff[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];

	if (global_sys_ctrl_info.m_transfer_callback)
	{
		(*global_sys_ctrl_info.m_transfer_callback)(send_data.rx_buff, VotTools.RETURN_LENGTH + 5);
	}
}

#if 0
void Return_Message_Processing(u16 total, ...){
	va_list args;
	va_start(args, total);

	send_data.rx_buff[0] = Vot_command_head1;
	send_data.rx_buff[1] = Vot_command_head2;
	send_data.rx_buff[2] = Vot_command_head3;
	send_data.rx_buff[3] = (total + 5) >> 8;
	send_data.rx_buff[4] = (total + 5) >> 0;
	send_data.rx_buff[5] = com_data.buff[VotTools.HEAD_FLAG+5];
	send_data.rx_buff[6] = 0xCA;
	send_data.rx_buff[7] = com_data.buff[VotTools.HEAD_FLAG+7];

	for(u16 i = 0; i < total; i++) {
		send_data.rx_buff[8 + i] = va_arg(args, int);
	}
	va_end(args);

	VotTools.RETURN_LENGTH = ((send_data.rx_buff[3]<<8)|send_data.rx_buff[4]);
	Data_Verification(send_data.rx_buff,VotTools.RETURN_LENGTH+3);
	send_data.rx_buff[VotTools.RETURN_LENGTH+3] = VotArrays.Rcvbuf[0];
	send_data.rx_buff[VotTools.RETURN_LENGTH+4] = VotArrays.Rcvbuf[1];
	for(u16 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
	{
		//bsp_printf("%c",send_data.rx_buff[i]);
	}
}
#endif

void MessageReturnState(u8 State)
{
	send_data.rx_buff[0] = Vot_command_head1;
	send_data.rx_buff[1] = Vot_command_head2;
	send_data.rx_buff[2] = Vot_command_head3;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	send_data.rx_buff[5] = com_data.buff[VotTools.HEAD_FLAG + 5];

	send_data.rx_buff[6] = 0xCA;
	send_data.rx_buff[7] = 0x00;

	send_data.rx_buff[8] = State;

	VotTools.RETURN_LENGTH = ((send_data.rx_buff[3] << 8) | send_data.rx_buff[4]);
	Data_Verification(send_data.rx_buff, VotTools.RETURN_LENGTH + 3);
	send_data.rx_buff[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
	send_data.rx_buff[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];

	// for(u8 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
	// {
	// 	//bsp_printf("%c",send_data.rx_buff[i]);

	// }
	if (global_sys_ctrl_info.m_transfer_callback)
	{
		(*global_sys_ctrl_info.m_transfer_callback)(send_data.rx_buff, VotTools.RETURN_LENGTH + 5);
	}
	// vot_uart_write(send_data.rx_buff,VotTools.RETURN_LENGTH+5);
	memset(send_data.rx_buff, 0, sizeof(send_data.rx_buff));
}

void Data_Verification(const u8 *Array, u32 Len)
{
	u32 IX;
	u16 IY;
	u16 CRC;
	if (Len <= 0)
	{
		return;
	}
	CRC = 0xFFFF;
	for (IX = 0; IX < Len; IX++)
	{
		CRC = CRC ^ (u16)(Array[IX]);
		for (IY = 0; IY <= 7; IY++)
		{
			if ((CRC & 1) != 0)
			{
				CRC = (CRC >> 1) ^ 0xA001;
			}
			else
			{
				CRC = CRC >> 1;
			}
		}
	}
	VotArrays.Rcvbuf[0] = (CRC & 0xff00) >> 8;
	VotArrays.Rcvbuf[1] = (CRC & 0x00ff);
}

void Vot_Tools_Init(void)
{
	VotTools.STATUS_FLAG = 0;
	VotTools.DATA_LENGTH = 0;
	VotTools.COMMAND_LENGTH = 0;
	VotTools.RETURN_LENGTH = 0;
	VotTools.HEAD_FLAG = 0;
	VotTools.END_FLAG = 0;
	VotTools.BIGDATA_FALG = 0;
	VotTools.BIGDATA_TIME = 0;
}

void Vot_Parameters_Init(void)
{
	VotParameters.Update_Firmware_Len = 0;
	VotParameters.Version_Falg = 0;
	VotParameters.Test_Flag = 0;
	VotParameters.Execute_Flag = 1;
}

void Data_u16_Limit(u16 *Data, u16 Max, u16 Min)
{
	if (*Data > Max)
	{
		*Data = Max;
	}
	else if (*Data < Min)
	{
		*Data = Min;
	}
}

void Data_u8_Limit(u8 *Data, u8 Max, u8 Min)
{
	if (*Data > Max)
	{
		*Data = Max;
	}
	else if (*Data < Min)
	{
		*Data = Min;
	}
}
/****************************************************/

/********************Planck640******************/
void Get_VOTReservedAreaDataFromUart(u8 *data)
{
#if 0 // lyb
//#ifdef SUPPORT_VOT_DETECTOR
	u32 TempData = 0;
	TempData = (data[3]<<24)+(data[2]<<16)+(data[1]<<8)+data[0];//bit0 - bit31
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,24,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,24)&0x00000000)|(TempData)));

	TempData = (data[7]<<24)+(data[6]<<16)+(data[5]<<8)+data[4];//bit32 - bit63
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,28,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,28)&0x00000000)|(TempData)));

	TempData = (data[11]<<24)+(data[10]<<16)+(data[9]<<8)+data[8];//bit64 - bit95
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,32,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,32)&0x00000000)|(TempData)));

	TempData = (data[15]<<24)+(data[14]<<16)+(data[13]<<8)+data[12];//bit96 - bit127
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,36,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,36)&0x00000000)|(TempData)));

	TempData = (data[19]<<24)+(data[18]<<16)+(data[17]<<8)+data[16];//bit128 - bit159
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,40,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,40)&0x00000000)|(TempData)));

	TempData = (TempData&0x00000000) + ((data[21]&0x01)<<8) + data[20];//bit168+bit167~bit160
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xfffffe00)|(TempData)));

	TempData = (TempData&0x00000000) + (((data[22]&0x80)>>7)<<18);//bit183
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20)&0xfffbffff)|(TempData)));

	TempData = (TempData&0x00000000) + (((data[23]&0x20)>>5)<<17);//bit189
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20)&0xfffdffff)|(TempData)));

	TempData = (TempData&0x00000000) + ((((data[25])>>3) + ((data[26])<<5))<<24);//bit203 - bit210
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20)&0x00ffffff)|(TempData)));

	TempData = (TempData&0x00000000) + (((data[30]<<16)+(data[29]<<8)+data[28])<<8);//bit224 - bit247
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,8,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,8)&0x000000ff)|(TempData)));

	TempData = (data[34]<<24)+(data[33]<<16)+(data[32]<<8)+data[31];//bit248 - bit279
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,12,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,12)&0x00000000)|(TempData)));

	TempData = (data[38]<<24)+(data[37]<<16)+(data[36]<<8)+data[35];//bit280 - bit311
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,16,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,16)&0x00000000)|(TempData)));

	TempData = (TempData&0x00000000) +((data[41]&0x01)<<16) + (data[40]<<8) + data[39];//bit328 + (bit312 - bit327)
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,20)&0xfffe0000)|(TempData)));

	TempData = (data[45]<<24)+(data[44]<<16)+(data[43]<<8)+data[42];//bit336 - bit367
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,0)&0x00000000)|(TempData)));

	TempData = (data[49]<<24)+(data[48]<<16)+(data[47]<<8)+data[46];//bit368 - bit399
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,4,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,4)&0x00000000)|(TempData)));

	//bit400 - bit407
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,8,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,8)&0xffffff00)|(data[50])));

	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffff3fff)|(0x0000c000)));//OOC_SEL

	SystemParaMenu2.GAIN = (data[41]>>1);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
	SystemParaMenu2.NDAC_L = (data[26]>>3);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffffc1ff)|(SystemParaMenu2.NDAC_L<<9)));
	SystemParaMenu2.NDAC_H = data[27];
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));

	if(SystemParaMenu2.Usermenu_SW == 0) //
	{
		SystemParaMenu2.GOF = (data[24]>>4)+((data[25]&0x07)<<4);
		SystemParaMenu2.OOC_Par = (data[23]>>6)+((data[24]&0x0f)<<2);
		SystemParaMenu2.INT_C = ((data[23]&0x1f)>>2);
		SystemParaMenu2.INT_T = (data[21]>>1);
	}
	else if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
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
	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF
	write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
#if 0
#ifdef Temperature_Version
	SystemParaMenu2.GAIN = (data[41]>>1);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
	SystemParaMenu2.NDAC_L = (data[26]>>3);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffffc1ff)|(SystemParaMenu2.NDAC_L<<9)));
	SystemParaMenu2.NDAC_H = data[27];
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));

	if(SystemParaMenu2.Usermenu_SW == 0) //
	{
		SystemParaMenu2.GOF = (data[24]>>4)+((data[25]&0x07)<<4);
		SystemParaMenu2.OOC_Par = (data[23]>>6)+((data[24]&0x0f)<<2);
		SystemParaMenu2.INT_C = ((data[23]&0x1f)>>2);
		SystemParaMenu2.INT_T = (data[21]>>1);
	}
	else if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
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
	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF
	write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
#else
	SystemParaMenu2.GOF = (data[24]>>4)+((data[25]&0x07)<<4);
	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF

	SystemParaMenu2.GAIN = (data[41]>>1);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值

	SystemParaMenu2.NDAC_H = data[27];
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xff00ffff)|(SystemParaMenu2.NDAC_H<<16)));

	SystemParaMenu2.NDAC_L = (data[26]>>3);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffffc1ff)|(SystemParaMenu2.NDAC_L<<9)));

	SystemParaMenu2.OOC_Par = (data[23]>>6)+((data[24]&0x0f)<<2);
	write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);

	SystemParaMenu2.INT_C = ((data[23]&0x1f)>>2);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C

	SystemParaMenu2.INT_T = (data[21]>>1);
	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
#endif
#endif
#endif
}

// void AutomaticParameterConfiguration(void)
// {
// #ifdef SUPPORT_VOT_DETECTOR

// 	u16 TempDETAD1,TempDETAD2 = 0;
// 	u8 TempGOF = 0;
// 	u16 TempADV1,TempADV2 = 0;
// 	u8 ErrorFlag = 0;
// 	u16 threshold = 0x3FFF;

// 	if(SystemParaMenu2.Usermenu_SW)	return;
// #if (DET_Change_Para && SUPPORT_VOT_DETECTOR)
// 		if(SystemParaMenu2.DetMode == 4 || SystemParaMenu2.DetMode == 2){}
// 		else return;
// #endif

// 	displayReminderInfo(Do);

// //	if(SystemParaMenu2.DetMode == 4){
// //		threshold = 0x3E80;
// //	}else if(SystemParaMenu2.DetMode == 2){
// //		threshold = 0x3FFF;
// //	}

// 	if(ReadDETAD_TEST() < threshold)
// 	{
// 		SystemParaMenu2.GOF = 77;
// 		SystemParaMenu2.GAIN = 54;
// 		SystemParaMenu2.INT_C = 3;
// 		SystemParaMenu2.INT_T = 66;
// 		SystemParaMenu2.OOC_Par = 35;
// 		Int_Shutter_pro();  //test use
// 	}

// 	TempGOF = SystemParaMenu2.GOF;
// 	SystemParaMenu2.GOF = 99;

// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
// 	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF
// 	delay_1ms(100);

// 	while(ReadDETAD_TEST() >= threshold)
// 	{
// 		if(SystemParaMenu2.GAIN > 0)
// 		{
// 			SystemParaMenu2.GAIN = SystemParaMenu2.GAIN - 1;
// 			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
// 			delay_1ms(100);
// 		}
// 		else
// 		{
// 			ErrorFlag = 1;
// 			break;
// 		}
// 	}

// 	if(ErrorFlag)
// 	{
// 		ErrorFlag = 0;
// 		return;
// 	}

// //	if(SystemParaMenu2.DetMode == 4){
// //		TempDETAD1 = ReadDETAD_TEST();
// //		SystemParaMenu2.GAIN = SystemParaMenu2.GAIN + 1;
// //		APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
// //		delay_1ms(100);
// //		TempDETAD2 = ReadDETAD_TEST();
// //		if(abs(TempDETAD2 - threshold) > abs(threshold - TempDETAD1))
// //		{
// //			SystemParaMenu2.GAIN = SystemParaMenu2.GAIN - 1;
// //			APB3_REGW(IO_APB_SLAVE_0_INPUT_function02,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function02,0)&0x80ffffff)|(SystemParaMenu2.GAIN<<24)));//GAIN值
// //		}
// //	}

// 	SystemParaMenu2.GOF = TempGOF;
// 	write_u8(SystemParaMenu2.GOF,IO_APB_SLAVE_0_INPUT_COF);	//GOF
// #if DET_Change_Para
// 	if(SystemParaMenu2.DetMode == 4){
// 		SystemParaMenu2.INT_C = 6;
// 		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C
// 		SystemParaMenu2.INT_T = 50;
// 		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
// 	}else if(SystemParaMenu2.DetMode == 2){
// 		SystemParaMenu2.INT_C = 3;
// 		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C
// 		SystemParaMenu2.INT_T = 45;
// 		APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
// 	}
// #else
// 	SystemParaMenu2.INT_C = 6;
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0xff1fffff)|(SystemParaMenu2.INT_C<<21)));//INT_C
// 	SystemParaMenu2.INT_T = 50;
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_function07,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_function07,0)&0x80ffffff)|(SystemParaMenu2.INT_T<<24)));//INT_T
// #endif

// 	delay_1ms(1500);

// 	if(ReadImgRaw_Avg()>=0x2000)
// 	{
// 		while(ReadImgRaw_Avg()>=0x2000)
// 		{
// 			if(SystemParaMenu2.OOC_Par > 0)
// 			{
// 				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
// 				write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 				delay_1ms(200);
// 			}
// 			else
// 			{
// 				ErrorFlag = 1;
// 				break;
// 			}
// 		}
// 		if(ErrorFlag)
// 		{
// 			ErrorFlag = 0;
// 			return;
// 		}
// 		TempADV1 = ReadImgRaw_Avg();//<2000

// 		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
// 		write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 		delay_1ms(200);
// 		TempADV2 = ReadImgRaw_Avg();//>2000

// 		if(abs(TempADV2 - 0x2000) > abs(0x2000 - TempADV1))
// 		{
// 			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
// 			write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 		}

// 	}
// 	else
// 	{
// 		while(ReadImgRaw_Avg() < 0x2000)
// 		{
// 			if(SystemParaMenu2.OOC_Par <= 63)
// 			{
// 				SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
// 				write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 				delay_1ms(200);
// 			}
// 			else
// 			{
// 				ErrorFlag = 1;
// 				break;
// 			}
// 		}
// 		if(ErrorFlag)
// 		{
// 			ErrorFlag = 0;
// 			return;
// 		}
// 		TempADV1 = ReadImgRaw_Avg();//>2000

// 		SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par - 1;
// 		write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 		delay_1ms(200);
// 		TempADV2 = ReadImgRaw_Avg();//<2000

// 		if(abs(TempADV1 - 0x2000) < abs(0x2000 - TempADV2))
// 		{
// 			SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par + 1;
// 			write_u8(SystemParaMenu2.OOC_Par,IO_APB_SLAVE_0_INPUT_OOC_PAR);
// 		}
// 	}
// #if DET_Change_Para
// 	if(SystemParaMenu2.DetMode == 4){
// 		SystemParaMenu2.offChange.INT_C = SystemParaMenu2.INT_C;
// 		SystemParaMenu2.offChange.INT_T = SystemParaMenu2.INT_T;
// 		SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
// 	}else if(SystemParaMenu2.DetMode == 2){
// 		SystemParaMenu2.NormalTemp.INT_C = SystemParaMenu2.INT_C;
// 		SystemParaMenu2.NormalTemp.INT_T = SystemParaMenu2.INT_T;
// 		//SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
// 		//SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 8;
// 		SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
// 		//SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 4;
// 	}

// 	refreshDetMenu();
// #endif

// 	Int_Shutter_pro();  //test use

// 	displayReminderInfo(Done);
// #endif
// }

// void Write_VOTReservedAreaDataToFlash(u8 *PointData)
// {
// #ifdef SUPPORT_VOT_DETECTOR
// 	PointData[VOT_RESERVED_DATA_LENGTH] = FLASH_SAVE_FLAG1;
// 	PointData[VOT_RESERVED_DATA_LENGTH+1] = FLASH_SAVE_FLAG2;
// 	write_flash(N25Q032A_VOT_RES_DATA_ADDRESS,PointData,VOT_RESERVED_DATA_LENGTH+2);

// #endif
// }

// void VOTDetectorInit(void)
// {
// #ifdef SUPPORT_VOT_DETECTOR
// 	u8 TempVOTDataInit[] =
// 	{
// 		0x00,0x40,0x1C,0xBD,0xA6,0xC3,0x3E,0x33,0x51,0x04,
// 		0x08,0x80,0x00,0x41,0x6C,0x1B,0xB6,0x6D,0xB3,0xC5,
// 		0x16,0x8C,0x87,0x0C,0xD8,0x6C,0x82,0xF0,0x1F,0x70,
// 		0x5D,0x83,0x5F,0x03,0x0C,0x0C,0x1D,0xFF,0xFD,0x57,
// 		0x3B,0x6F,0x34,0x3B,0x15,0x34,0x37,0x0A,0x39,0x01,0x00,
// 		0x00,0x00,
// 	};
// 	Get_VOTReservedAreaDataFromUart(TempVOTDataInit);
// 	Write_VOTReservedAreaDataToFlash(TempVOTDataInit);
// #endif
// }

void Reversal_VOTDetectorData(u8 *data)
{
	u8 temp;
	for (u8 i = 0; i < 26; i++)
	{
		temp = data[i];
		data[i] = data[50 - i];
		data[50 - i] = temp;
	}
}

u8 Change_NdacValue(u8 TempData)
{
	u8 i = 0, j = 0;
	for (i = 0; i < 8; i++)
	{
		if (TempData)
		{
			j++;
			TempData = TempData >> 1;
		}
	}
	return (j - 1);
}
/********************************************************/

/*************Temp*******************************************/

// void ReturnTemptureFotTime(void)
// {
// 	u16 max_Spot_AD,max_Spot_X_Pos,max_Spot_Y_Pos,min_Spot_AD,min_Spot_X_Pos,min_Spot_Y_Pos,AverageAD;
// 	// u16 Max_AD,Min_AD;

// 	send_data.rx_buff[3] = 0x00;
// 	send_data.rx_buff[4] = 0x13;
// 	//send_data.rx_buff[4] = 0x17;

// 	Areainit[0].Area_Lx = 0;
// 	Areainit[0].Area_Ly = 0;
// 	Areainit[0].Area_Rx = 639;
// 	Areainit[0].Area_Ry = 511;
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Left_XY__SET,0)&0x00000000)|(( Areainit[0].Area_Ly<<16)+( Areainit[0].Area_Lx)));
// 	APB3_REGW(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area1_Right_XY__SET,0)&0x00000000)|(( Areainit[0].Area_Ry<<16)+ Areainit[0].Area_Rx));

// 	max_Spot_AD = (read_u32(IO_APB_SLAVE_0_INPUT_AREAN_MIN_MAX_TEMP_START)&0xFFFF0000)>>16;
// 	min_Spot_AD = read_u32(IO_APB_SLAVE_0_INPUT_AREAN_MIN_MAX_TEMP_START)&0x0000FFFF;
// 	AverageAD = read_u32(IO_APB_SLAVE_0_INPUT_AREAN_TEMPAVG_START)/(( Areainit[0].Area_Rx -  Areainit[0].Area_Lx)*( Areainit[0].Area_Ry- Areainit[0].Area_Ly));

// 	if(Temp_Parameter.AD_sensor & 0x8000){
// 		max_Spot_AD = max_Spot_AD - (Temp_Parameter.AD_sensor&0x7FFF);
// 		min_Spot_AD = min_Spot_AD - (Temp_Parameter.AD_sensor&0x7FFF);
// 		AverageAD = AverageAD - (Temp_Parameter.AD_sensor&0x7FFF);
// 	}else{
// 		max_Spot_AD = max_Spot_AD + Temp_Parameter.AD_sensor;
// 		min_Spot_AD = min_Spot_AD + Temp_Parameter.AD_sensor;
// 		AverageAD = AverageAD + Temp_Parameter.AD_sensor;
// 	}

// 	max_Spot_Y_Pos= ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)>>16)&0xFFFF);
// 	max_Spot_X_Pos = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)&0xFFFF);
// 	//Max_AD = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX,0)&0xFFFF);

// 	min_Spot_Y_Pos= ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MIN_POS,0)>>16)&0xFFFF);
// 	min_Spot_X_Pos = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MIN_POS,0)&0xFFFF);
// 	//Min_AD = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MIN,0)&0xFFFF);

// 	send_data.rx_buff[8] = max_Spot_AD>>8;
// 	send_data.rx_buff[9] = max_Spot_AD;
// 	send_data.rx_buff[10] = max_Spot_X_Pos>>8;
// 	send_data.rx_buff[11] = max_Spot_X_Pos;
// 	send_data.rx_buff[12] = max_Spot_Y_Pos>>8;
// 	send_data.rx_buff[13] = max_Spot_Y_Pos;

// 	send_data.rx_buff[14] = min_Spot_AD>>8;
// 	send_data.rx_buff[15] = min_Spot_AD;
// 	send_data.rx_buff[16] = min_Spot_X_Pos>>8;
// 	send_data.rx_buff[17] = min_Spot_X_Pos;
// 	send_data.rx_buff[18] = min_Spot_Y_Pos>>8;
// 	send_data.rx_buff[19] = min_Spot_Y_Pos;

// 	send_data.rx_buff[20] = AverageAD>>8;
// 	send_data.rx_buff[21] = AverageAD;
// 		//send_data.rx_buff[22] = Max_AD>>8;
// 		//send_data.rx_buff[23] = Max_AD;
// 		//send_data.rx_buff[24] = Min_AD>>8;
// 		//send_data.rx_buff[25] = Min_AD;
// 	send_data.rx_buff[0] = Vot_command_head1;
// 	send_data.rx_buff[1] = Vot_command_head2;
// 	send_data.rx_buff[2] = Vot_command_head3;
// 	send_data.rx_buff[5] = 0x00;
// 	send_data.rx_buff[6] = 0xCA;
// 	send_data.rx_buff[7] = 0x47;

// 	VotTools.RETURN_LENGTH = ((send_data.rx_buff[3]<<8)|send_data.rx_buff[4]);
// 	Data_Verification(send_data.rx_buff,VotTools.RETURN_LENGTH+3);
// 	send_data.rx_buff[VotTools.RETURN_LENGTH+3] = VotArrays.Rcvbuf[0];
// 	send_data.rx_buff[VotTools.RETURN_LENGTH+4] = VotArrays.Rcvbuf[1];
// 	for(u16 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
// 	{
// 		//bsp_printf("%c",send_data.rx_buff[i]);
// 	}
// 	memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
// }

void ReturnFirePointTem(void)
{
	// 	s16 max_Spot;
	// 	u16 max_Spot_X_Pos,max_Spot_Y_Pos;
	// 	u16 LENGTH;
	// 	u8 DataBUFF[45] = {0};

	// 	if(VotTools.BIGDATA_FALG)	{return;}
	// //	//bsp_printf("AlarmNum = %x ,AlarmAD = %x",read_u16(Hypprram_Alarm6),SystemParaMenu2.AlarmAD);
	// 	if(read_u16(Hypprram_Alarm6) >=SystemParaMenu2.AlarmNum)
	// 	{
	// 		DataBUFF[3] = 0x00;
	// 		DataBUFF[4] = 0x28;

	// 	temp_convert_display_H((u32 *)IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX,&max_Spot);
	// 	max_Spot +=600;

	// 	max_Spot_Y_Pos= ((APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)>>16)&0xFFFF);
	// 	max_Spot_X_Pos = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_MAX_POS,0)&0xFFFF);
	// 	//Max_AD = (APB3_REGR(IO_APB_SLAVE_0_INPUT_SPOT_AD_TEMP_MAX,0)&0xFFFF);
	// 	max_Spot_X_Pos = (uint16_t)((float)(max_Spot_X_Pos*1.125));
	// 	max_Spot_Y_Pos = (uint16_t)((float)(max_Spot_Y_Pos*1.125));

	// 	DataBUFF[8] = 0x01;
	// 	DataBUFF[9] = max_Spot_X_Pos>>8;
	// 	DataBUFF[10] = max_Spot_X_Pos;
	// 	DataBUFF[11] = max_Spot_Y_Pos>>8;
	// 	DataBUFF[12] = max_Spot_Y_Pos;
	// 	DataBUFF[13] = max_Spot>>8;
	// 	DataBUFF[14] = max_Spot;

	// 	DataBUFF[15] = 0x02;
	// 	DataBUFF[16] = 0>>8;
	// 	DataBUFF[17] = 0;
	// 	DataBUFF[18] = 0>>8;
	// 	DataBUFF[19] = 0;
	// 	DataBUFF[20] = 0>>8;
	// 	DataBUFF[21] = 0;

	// 	DataBUFF[22] = 0x03;
	// 	DataBUFF[23] = 0>>8;
	// 	DataBUFF[24] = 0;
	// 	DataBUFF[25] = 0>>8;
	// 	DataBUFF[26] = 0;
	// 	DataBUFF[27] = 0>>8;
	// 	DataBUFF[28] = 0;

	// 	DataBUFF[29] = 0x04;
	// 	DataBUFF[30] = 0>>8;
	// 	DataBUFF[31] = 0;
	// 	DataBUFF[32] = 0>>8;
	// 	DataBUFF[33] = 0;
	// 	DataBUFF[34] = 0>>8;
	// 	DataBUFF[35] = 0;

	// 	DataBUFF[36] = 0x05;
	// 	DataBUFF[37] = 0>>8;
	// 	DataBUFF[38] = 0;
	// 	DataBUFF[39] = 0>>8;
	// 	DataBUFF[40] = 0;
	// 	DataBUFF[41] = 0>>8;
	// 	DataBUFF[42] = 0;

	// 	DataBUFF[0] = Vot_command_head1;
	// 	DataBUFF[1] = Vot_command_head2;
	// 	DataBUFF[2] = Vot_command_head3;
	// 	DataBUFF[5] = 0x00;
	// 	DataBUFF[6] = 0xCA;
	// 	DataBUFF[7] = 0x67;

	// 	LENGTH = ((DataBUFF[3]<<8)|DataBUFF[4]);
	// 	Data_Verification(DataBUFF,LENGTH+3);
	// 	DataBUFF[LENGTH+3] = VotArrays.Rcvbuf[0];
	// 	DataBUFF[LENGTH+4] = VotArrays.Rcvbuf[1];
	// 	for(u16 i = 0; i < LENGTH+5; i++)
	// 	{
	// 		//bsp_printf("%c",DataBUFF[i]);
	// 	}
	// 	memset(DataBUFF, 0, sizeof (DataBUFF));
	// 	}
}

// u8 FPGAVersionTransmit(u8 *senddata)
// {
// // 	u8 VerData[16]={0x00};
// // 	u8 i,j,k;
// // 	u8 FPGAData,FPGAlength;

// // 	VerData[0] = 'F';
// // 	for(k=0,j=1;k<4;k++,j++)
// // 	{
// // 		FPGAData = APB3_REGR(IO_APB_SLAVE_0_INPUT_FPGA_VERSION,0)>>((3-k)*8);
// // 		i= FPGAData/16;
// // 		if(i<10)		VerData[j] =  FPGAData/16 + 0x30;
// // 		else			VerData[j] =  FPGAData/16 + 0x37;
// // 		i = FPGAData%16;
// // 		j++;
// // 		if(i<10)		VerData[j] =  FPGAData%16 + 0x30;
// // 		else			VerData[j] =  FPGAData%16 + 0x37;
// // 	} //j=9
// // 	j--;
// // 	if(VerData[j-1] == '0')
// // 	{
// // 		VerData[j-1] = 'T';
// // 		VerData[j+1] = 0x20;
// // 		VerData[j+2] = 0x20;
// // 		VerData[j+3] = 0x20;
// // 		VerData[j+4] = 0x20;
// // 		VerData[j+5] = 0x20;
// // 		VerData[j+6] = 0x20;
// // 		VerData[j+7] = 0x20;
// // 	}
// // 	else if(VerData[j-1] == 'A')
// // 	{
// // 		VerData[j-1] = 'P';
// // 		VerData[j+1] = '_';
// // 		VerData[j+2] = 'P';
// // 		VerData[j+3] = 'L';
// // 		VerData[j+4] = 'K';
// // 		VerData[j+5] = '6';
// // 		VerData[j+6] = '_';
// // 		VerData[j+7] = 'T';
// // //		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
// // //		DisPlayNewStr(XLINE+16,YLINE,"_PLK6_T",layer,color);
// // 	}
// // 	else if(VerData[j-1] == 'B')
// // 	{
// // 		VerData[j-1] = 'P';
// // 		VerData[j+1] = '_';
// // 		VerData[j+2] = 'P';
// // 		VerData[j+3] = 'L';
// // 		VerData[j+4] = 'K';
// // 		VerData[j+5] = '6';
// // 		VerData[j+6] = 0x20; //' '
// // 		VerData[j+7] = 0x20; //' '
// // //		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
// // //		DisPlayNewStr(XLINE+16,YLINE,"_PLK6",layer,color);
// // 	}
// // 	else if(VerData[j-1] == 'C')
// // 	{
// // 		VerData[j-1] = 'P';
// // 		VerData[j+1] = '_';
// // 		VerData[j+2] = 'R';
// // 		VerData[j+3] = 'T';
// // 		VerData[j+4] = 'D';
// // 		VerData[j+5] = '6';
// // 		VerData[j+6] = '_';
// // 		VerData[j+7] = 'T';
// // //		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
// // //		DisPlayNewStr(XLINE+16,YLINE,"_RTD6_T",layer,color);
// // 	}
// // 	else if(VerData[j-1] == 'D')
// // 	{
// // 		VerData[j-1] = 'P';
// // 		VerData[j+1] = '_';
// // 		VerData[j+2] = 'R';
// // 		VerData[j+3] = 'T';
// // 		VerData[j+4] = 'D';
// // 		VerData[j+5] = '6';
// // 		VerData[j+6] = 0x20;
// // 		VerData[j+7] = 0x20;
// // //		DisPlayNewStr(XLINE,YLINE,"P",layer,color);
// // //		DisPlayNewStr(XLINE+16,YLINE,"_RTD6",layer,color);
// // 	}
// // 	FPGAlength = sizeof(VerData);
// // //	//bsp_printf("FPGAlength:%d ,j=%d , VerData = %c\n",FPGAlength,j,VerData[j-1]);
// // 	for(i=0;i<FPGAlength;i++)
// // 	{
// // 		*senddata = VerData[i];
// // //		//bsp_printf("VerData[%d]:%c\n",i,*senddata);
// // 		senddata++;
// // 	}
// // 	return FPGAlength;
// }

void UART_OPR_Correct_Bad_Pix(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0x00)
	{
#ifdef VOT
		UpdatePromptInfoWithTimeMs("Auto correct bad pixel start", 2000,VOT_LOG_TYPE_MENU2);
		//					GetPixelMaxAndMinValue();  //自动校正坏点功能 功能后续补
		UpdatePromptInfoWithTimeMs("Auto correct bad pixel completed", 2000,VOT_LOG_TYPE_MENU2);
#endif
	}
	else if (Valid_data[8] == 0x01)
	{
#ifdef VOT
		UpdatePromptInfoWithTimeMs("Cancel correct bad pixel start", 2000,VOT_LOG_TYPE_MENU2);
		//					CancalPixelMaxAndMinValue(); //取消上一次校正效果 功能后续补
		UpdatePromptInfoWithTimeMs("Cancel OK", 2000,VOT_LOG_TYPE_MENU2);
#endif
	}
	else if (Valid_data[8] == 0x02)
	{
		// 					InitHyram();  //先不做处理
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	send_data.rx_buff[8] = Valid_data[8];
}

void UART_OPR_KEY(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00; // 返回信息长度
	send_data.rx_buff[4] = 0x06;
	send_data.rx_buff[8] = Valid_data[8];
	// 短按单个按键
	if (Valid_data[8] == 0xFE)
	{
		KEY_FE_PRO();
	} // KEY0:enter
	else if (Valid_data[8] == 0xFD)
	{
		KEY_FD_PRO();
	} // KEY1:up
	else if (Valid_data[8] == 0xFB)
	{
		KEY_FB_PRO();
	} // KEY2:down
	else if (Valid_data[8] == 0xF7)
	{
		KEY_F7_PRO();
	} // KEY3:left
	else if (Valid_data[8] == 0xEF)
	{
		KEY_EF_PRO();
	} // KEY4:right
	else if (Valid_data[8] == 0x01)
	{
		// if (SystemParaMenu2.m_osd_show_on_dev > OSD_POSITION_ON_CVBS)
		// {
		// 	SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
		// 	SystemParaMenu2.m_video_out_mode = VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG;
		// }
		// else
		// {
		// 	SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_UVC;
		// 	SystemParaMenu2.m_video_out_mode = VIDEO_OUTPUT_MODE_ENUM_UVC;
		// }
		// vision_log_debug(".m_video_out_mode %d .m_osd_show_on_dev %d \n", SystemParaMenu2.m_video_out_mode, SystemParaMenu2.m_osd_show_on_dev);
		// ISP_write_sys_menu2_cfg(SystemParaMenu2);
	}
	else if (Valid_data[8] == 0x02)
	{
		if (SystemParaMenu2.m_cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE)
		{
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
			// set_osd_position(TS_OSD_CVBS_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 50, 50);
			// set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 50, 50);
		}
		else
		{
			SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
			// set_osd_position(TS_OSD_CVBS_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 100, 100);
			// set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 100, 100);
		}
		vision_log_debug(".m_cvbs_display_mode %d \n", SystemParaMenu2.m_cvbs_display_mode);
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
	}
	else if (Valid_data[8] == 0x03)
	{
		send_data.rx_buff[4] = 0x07;
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];

		if (Valid_data[9] == 0)
		{
			vision_log_debug("unset user isp cfg  \n");
			unsave_local_isp_cfg();
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 1)
		{
			vision_log_debug("set user isp cfg  \n");
			save_local_isp_cfg();
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 2)
		{
			vision_log_debug("set BPC_MODE_ON_CVBS \n");
			SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_CVBS;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 3)
		{
			vision_log_debug("set BPC_MODE_ON_UVC  \n");
			SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_UVC;
			// ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 4)
		{

			static ts_u08 index = 0;

			ts_u32 u32_sub = TS_UPL_POS_NUC;
			ts_u32 u32_mode = 0;

			if (index == 6)
			{
				index = TS_UPL_POS_VPO_YUV;
			}
			u32_sub = index;
			vision_log_debug(" switch uvc index %d \n", index);

			if (index >= TS_UPL_POS_VPO_Y8)
			{
				index = TS_UPL_POS_RAW;
			}
			else
			{
				index++;
			}

			vot_uvc_set_attr(0, u32_sub, u32_mode);
		}
	}
	else if (Valid_data[8] == 0x04)
	{
		send_data.rx_buff[4] = 0x07;
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];

		if (Valid_data[9] == 0)
		{
			vision_log_debug("bpc cfg on   \n");
			SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_ON;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 1)
		{
			vision_log_debug("bpc cfg BPC_CFG_MODE_OFF_EE_OFF_DDE  \n");
			SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_OFF_EE_OFF_DDE;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 2)
		{
			vision_log_debug("bpc cfg BPC_CFG_MODE_ON_EE_OFF_DDE   \n");
			SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_ON_EE_OFF_DDE;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 3)
		{
			vision_log_debug("bpc cfg BPC_CFG_MODE_ON_EE_ON_DDE  \n");
			SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_ON_EE_ON_DDE;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
		else if (Valid_data[9] == 4)
		{
			vision_log_debug("bpc cfg BPC_CFG_MODE_OFF_EE_ON_DDE  \n");
			SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_OFF_EE_ON_DDE;
			ISP_write_sys_menu2_cfg(SystemParaMenu2);
		}
	}
	else if (Valid_data[8] == 0x05)
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		send_data.rx_buff[8] = Valid_data[8];

		ts_u32 u32_sub = TS_UPL_POS_NUC;
		ts_u32 u32_mode = 0;
		ts_u16 index = Valid_data[9] << 8 | Valid_data[10];
		u32_sub = index;

		vision_log_debug(" switch uvc index %d u32_sub %d \n", index, u32_sub);
		vot_uvc_set_attr(0, u32_sub, u32_mode);
	}
	else if (Valid_data[8] == 0x06)
	{
		send_data.rx_buff[4] = 0x07;
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];

		if (Valid_data[9] == 0)
		{
			vision_log_debug("disable lut   \n");
			// vot_set_lut_info(0, 1, 0, 0, 0, 0);
		}
		else if (Valid_data[9] == 1)
		{
			vision_log_debug("enable lut   \n");
			// vot_set_lut_info(1, 1, 0, 0, 0, 0);
		}
	}
	else if (Valid_data[8] == 0xC1) // NUC保存 在NUC操作中也有相关记录
	{
		NUC_Save(1);
	}
	else if (Valid_data[8] == 0xDF) // KEY5:shutter
	{
		ISP_cut();
	}
	else if (Valid_data[8] == 0x7F)
	{
		Key_ExitUsermenu();
	} // KEY6:cancel
	// //短按组合按键
	else if (Valid_data[8] == 0xDE)
	{
		BADMarkSemiAutoOpr();
	} // KEY0+KEY5:E+A	坏点标记	  确认/取消校准坏点
	else if (Valid_data[8] == 0xF4)
	{
		DisplayDeviceInfoAuto();
	} // KEY0+KEY1+KEY3:E+U+L 左上E，	显示公司名称、探测器信息、生产日期序列号
	else if (Valid_data[8] == 0xFA) // D+E空下E，	图像冻结
	{
		SetImgAutoFreeze(); // 图像冻结
	}
	else if(Valid_data[8] == 0xEE){  //进入or退出 标定模式
		EnterORExit_PC_templut_Calib();
	}
	else if (Valid_data[8] == 0xF5) // KEY1+KEY3:U+L   左上，Area1整体缩小
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_XY_Shrink);
		else 
			AreaN_Scale(select_Area,1,AreaBad_XY_Shrink);
	}
	else if (Valid_data[8] == 0xFC)
	{
	} // KEY0+KEY1:E+U   上E，变倍X2
	else if (Valid_data[8] == 0xE7) // L+R 左右，Area1拉长
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_X_Expand);
		else 
			AreaN_Scale(select_Area,1,AreaBad_X_Expand);
	}
	else if (Valid_data[8] == 0xEB) // R+D 右下，Area1变矮
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_Y_Shrink);
		else 
			AreaN_Scale(select_Area,1,AreaBad_Y_Shrink);
	}
	else if (Valid_data[8] == 0xED) // R+U 右上，Area1整体放大
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_XY_Expand);
		else 
			AreaN_Scale(select_Area,1,AreaBad_XY_Expand);
	}
	else if (Valid_data[8] == 0xF3) // L+D 左下，Area1变短
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_X_Shrink);
		else 
			AreaN_Scale(select_Area,1,AreaBad_X_Shrink);
	}
	// else if(Valid_data[8] == 0xF2){OneFramePrintf();}//L+D+E 空 左下E，	十字区域值AD温度切换 //暂时 ***********关挡片
	else if (Valid_data[8] == 0xF9) // D+U上下，Area1拉高
	{
		if ((Area_state & 0x0010) && (SystemParaMenu2.Auto_Semi))
			BadSemiAutoOpr(select_Area, 3, AreaBad_Y_Expand);
		else 
			AreaN_Scale(select_Area,1,AreaBad_Y_Expand);
	}
	else if (Valid_data[8] == 0xE1)
	{
		EnterOrExitBPCMaualModeAuto();
	} // KEY1+KEY2+KEY3+KEY4:U+D+L+R 坏点校正 长按上下左右，	校准坏点
	else if (Valid_data[8] == 0x78) // KEY0+KEY1+KEY2+KEY6:E+U+D+C 版本信息 上中下C，		打开/关闭版本信息
	{
		// 版本信息
		DisPlayDeviceInfoAndSoftVersionInfoAuto(1);
	}
	else if (Valid_data[8] == 0xD9)
	{
		enter_menu2();
	} // KEY0+KEY1+KEY2+KEY6:E+U+D+C 版本信息 上下A，		  长按进入菜单二
	else if (Valid_data[8] == 0xE4) // 闪烁十字光标置于中心位置显示与隐藏,命令：56 4F 54 00 06 00 AC FF E4 7C 85
	{
		// vision_log_debug("global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_width = %d , global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_height = %d\n",global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_width, global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_height);
		// create_cursor_display(); // 演示闪烁十字光标的显示和隐藏
		if (UsermenuParameter.m_Crosshair_OnlyDisplay) {
			UsermenuParameter.m_Crosshair_OnlyDisplay = OFF;
		}
		else {
			UsermenuParameter.m_Crosshair_OnlyDisplay = ON;
		}
		getDisplay_Crosshair_Switch();
	}
	else if (Valid_data[8] == 0xE5) // 控制去圆斑功能的开启和关闭,命令：56 4F 54 00 06 00 AC FF E5 BC 44	
	{
		create_sffc_run_state();// 创建一个开关函数，控制去圆斑功能的开启和关闭
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_SET_HOST_SW(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	if (Valid_data[8] == 1 || Valid_data[8] == 0)
	{
		SystemParaMenu2.HOST_SW = Valid_data[8];
		ISP_snc_set_en(SystemParaMenu2.HOST_SW);
		send_data.rx_buff[8] = SystemParaMenu2.HOST_SW;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}
void UART_OPR_SET_DEST_SW(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 1 || Valid_data[8] == 0)
	{
		SystemParaMenu2.DEST_SW = Valid_data[8];
		ISP_fpnc_set_en(SystemParaMenu2.DEST_SW);
		send_data.rx_buff[8] = SystemParaMenu2.DEST_SW;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_SET_Gmax(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 1 || Valid_data[8] == 0)
	{
		SystemParaMenu2.Gmax = Valid_data[8];
		ISP_dde_set_en(SystemParaMenu2.Gmax);
		send_data.rx_buff[8] = SystemParaMenu2.Gmax;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_SET_Palette(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	if (Valid_data[8] >= 0x00 && Valid_data[8] < VOT_COLOR_PALETTE_MODE_BUTT)
	{
		UsermenuParameter.Palette = Valid_data[8];
		load_palette(UsermenuParameter.Palette);

		send_data.rx_buff[8] = Valid_data[8];
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 设置截止灰度最小值/最大值 
 * 56 4F 54 00 0a 00 AC 17 01 3A 00 3F F0 46 FA
 * 56 4F 54 00 0a 00 AC 17 01 00 00 00 00 2A E7
*/
void UART_OPR_Set_ISP_AGC02_CFG(u8 *Valid_data) {
	ts_u16 tmp_length;
	ts_u32 *pu32_addr, u32_len;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	send_data.rx_buff[8] = 0x01;
	tmp_length = (Valid_data[3] << 8) | Valid_data[4];

	if (Valid_data[8] == 0x01 && tmp_length == 0x0a) {
		/*图像调节通过寄存器调节， 先获取isp寄存器*/
		if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
			vision_log_err("get isp reg error !\n");
			send_data.rx_buff[8] = 0xff;
		}
		else {
			ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t *)pu32_addr;
			pst_isp_reg->st_agc_v02.u08_mode = 1;
			// pst_isp_reg->st_agc_v02.u08_upd_speed = 255;
			pst_isp_reg->st_agc_v02.u16_loc_min = (Valid_data[9] << 8) | Valid_data[10];
			pst_isp_reg->st_agc_v02.u16_loc_max = (Valid_data[11] << 8) | Valid_data[12];
			// vision_log_debug("Valid_data[8] = %d, u16_loc_min = 0x%04x, u16_loc_max = 0x%04x\n", Valid_data[8], pst_isp_reg->st_agc_v02.u16_loc_min, pst_isp_reg->st_agc_v02.u16_loc_max);

			ts_tdk_isp_update_reg(TS_ISP_MDEV0);
		}
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
void UART_OPR_SET_DDE_Level(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] >= 0x00 && Valid_data[8] <= 0x10)
	{
		UsermenuParameter.enhance_level = Valid_data[8];
		if (UsermenuParameter.enhance_level == 0)
		{
			// ISP_dde_level(0);
			ISP_enhance_set(0);
		}
		else
		{
			// ISP_dde_level(SystemParaMenu2.DDE_Level*4);
			ISP_enhance_set(UsermenuParameter.enhance_level * 16);
		}
		send_data.rx_buff[8] = UsermenuParameter.enhance_level;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}
// 串口设置亮度
void UART_OPR_SET_Bright(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		ts_u08 m_luma_percent_mode = Valid_data[8];
		if (m_luma_percent_mode >= LUMA_PERCENT_MODE_BUTT)
		{
			m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
		}
		
		UsermenuParameter.m_luma_percent_mode = m_luma_percent_mode;
		vot_isp_luma_set();
		send_data.rx_buff[8] = UsermenuParameter.m_luma_percent_mode;
	}
	else{

		// manual模式独立可调
		if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL)
		{
			if (Valid_data[8] > 127)
			{
				Valid_data[8] = 127;
			}
			//场景的亮度和对比度可独立调节，保存；
			UsermenuParameter.manual_ImagePara.Bright = Valid_data[8];
			vot_isp_luma_set();
			send_data.rx_buff[8] = Valid_data[8];

		}
		else
		{
			if (UsermenuParameter.ImageAdjust == 0) // 0:手动模式,1：自动模式
			{
				if (UsermenuParameter.ImageMode == 0) // 模式0：常温模式0
				{
					if (Valid_data[8] > 127)
					{
						Valid_data[8] = 127;
					}
					UsermenuParameter.Bright = Valid_data[8];
					vot_isp_luma_set();
					send_data.rx_buff[8] = Valid_data[8];
					//ISP_write_sys_cfg();// 亮度和对比度的设置都保存到当前使用的场景中
				}
			}
		}
	}
}

// 串口设置对比度
void UART_OPR_SET_Contrast(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		ts_u08 m_contrast_percent_mode = Valid_data[8];
		if (m_contrast_percent_mode >= CONTRAST_PERCENT_MODE_BUTT)
		{
			m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
		}
		
		UsermenuParameter.m_contrast_percent_mode = m_contrast_percent_mode;
		vot_isp_coef_set();
		send_data.rx_buff[8] = UsermenuParameter.m_contrast_percent_mode;
		
	}
	else{

		// manual模式独立可调
		if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL)
		{
			if (Valid_data[8] > 32)
			{
				Valid_data[8] = 32;
			}
			// 场景的亮度和对比度可独立调节，保存；
			UsermenuParameter.manual_ImagePara.Contrast = Valid_data[8];
			vot_isp_coef_set();
			send_data.rx_buff[8] = Valid_data[8];
		}
		else
		{
			if (UsermenuParameter.ImageAdjust == 0) // 0:手动模式,1：自动模式
			{
				if (UsermenuParameter.ImageMode == 0) // 模式0：常温模式0
				{
					if (Valid_data[8] > 32)
					{
						Valid_data[8] = 32;
					}
					UsermenuParameter.Contrast = Valid_data[8];
					vot_isp_coef_set();
					send_data.rx_buff[8] = UsermenuParameter.Contrast;
					//ISP_write_sys_cfg();// 亮度和对比度的设置都保存到当前使用的场景中
				}
			}
		}
	}
}
void UART_OPR_SET_AutoShutter(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] >= 0 && Valid_data[8] <= 40)
	{
		UsermenuParameter.AutoShutter = Valid_data[8];
		// ShutterTimer = 0;
		if (UsermenuParameter.AutoShutter > 0)
		{
			UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
		}
		else if (UsermenuParameter.AutoShutter == 0)
		{
			UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
		}
		if (UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF)
		{
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
		}
		else
		{
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
		}
		ISP_set_auto_cut_flag_auto();
		send_data.rx_buff[8] = UsermenuParameter.AutoShutter;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}
void UART_OPR_SET_SFilter(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] >= 0x00 && Valid_data[8] <= 0x20)
	{
		UsermenuParameter.SFilter = Valid_data[8];
		ISP_nr_set(UsermenuParameter.SFilter * 8);
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}

	send_data.rx_buff[8] = UsermenuParameter.SFilter;
}
void UART_OPR_SET_Enlarge(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
		//pip模式开启后，不支持电子变倍
		if(UsermenuParameter.m_pip_mode != PIP_MODE_NONE){
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
#ifdef SUPPORT_CONTINUOUS_ZOOM
	if (Valid_data[8] >= 10 && Valid_data[8] <= 100)
	{
		ISP_vot_zoom_set(Valid_data[8]);
	}
#else
	if (Valid_data[8] >= 0x00 && Valid_data[8] <= 0x03)
	{

		ISP_vot_zoom_set(Valid_data[8]);
	}
#endif
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	send_data.rx_buff[8] = UsermenuParameter.Enlarge;
}

void UART_OPR_SET_Enlarge_ShowMes(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	send_data.rx_buff[8] = Valid_data[8];
}

void UART_OPR_SET_TFilter(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	send_data.rx_buff[8] = Valid_data[8];
}

void UART_OPR_Save_Menu1Par(u8 *Valid_data) // 用户菜单数据保存
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0x01)
	{
		// 用户菜单数据保存
		send_data.rx_buff[8] = Valid_data[8];
		save_menu1_parameters();
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Set_Read_SmartData(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x38;

	ts_u08 DataLength = VOT_RESERVED_DATA_LENGTH, i = 0;
	ts_u08 DetectorData[VOT_RESERVED_DATA_LENGTH + 2] = {0};
	planck640c_read_config_408bit(DetectorData, DataLength);
	i = 0;
	ts_u08 index = VOT_RESERVED_DATA_LENGTH - 1;
	while (DataLength--)
	{
		send_data.rx_buff[i + 8] = DetectorData[index];
		i++;
		index--;
	}
}

// 保存UCC3代光智探测器408bit数据到flash中
void FlashSave_SmartData408bit(u8 *config_408bit)
{
	struct Menu2_Para SysMenuDefParam;
	if (config_408bit == NULL)
	{
		return;
	}
	if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 读取系统菜单默认参数配置成功
	{
		SysMenuDefParam.SmartData408bitValidFlag = 0x01; // 获取到UCC3代光智探测器408bit数据的有效标志0xCC
		for (ts_u08 i = 0; i < VOT_RESERVED_DATA_LENGTH; i++)
		{
			SysMenuDefParam.SmartData408bit[i] = config_408bit[i];
		}
		// 将获取到的UCC3代光智探测器408bit数据保存到系统菜单参数中，并写入flash
		if (ISP_write_SysMenu_def_para_cfg(SysMenuDefParam) == 0)
		{
			vision_log_debug("Write SysMenuDefParam.SmartData408bit to flash success.\n");
		}
		else
		{
			vision_log_debug("Write SysMenuDefParam.SmartData408bit to flash failure.\n");
			ISP_write_SysMenu_def_para_cfg(SysMenuDefParam); // 系统菜单默认参数写入flash失败，再试一次
		}
	}
	// else
	// {
	// 	vision_log_debug("Failed to read system menu 2 configuration.\n");
	// }
}

// 保存UCC3代光智探测器256bit数据到flash中
void FlashSave_SmartData256bit(u8 *config_256bit)
{
	struct Menu2_Para SysMenuDefParam;
	if (config_256bit == NULL)
	{
		return;
	}
	if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 读取系统菜单默认参数配置成功
	{
		SysMenuDefParam.SmartData256bitValidFlag = 0x01; // 获取到UCC3代光智探测器256bit数据的有效标志0x01
		for (ts_u08 i = 0; i < 32; i++)
		{
			SysMenuDefParam.SmartData256bit[i] = config_256bit[i];
		}
		// 将获取到的UCC3代光智探测器408bit数据保存到系统菜单参数中，并写入flash
		if (ISP_write_SysMenu_def_para_cfg(SysMenuDefParam) == 0)
		{
			vision_log_debug("Write SysMenuDefParam.SmartData256bit to flash success.\n");
		}
		else
		{
			vision_log_debug("Write SysMenuDefParam.SmartData256bit to flash failure.\n");
			ISP_write_SysMenu_def_para_cfg(SysMenuDefParam); // 系统菜单默认参数写入flash失败，再试一次
		}
	}
	// else
	// {
	// 	vision_log_debug("Failed to read system menu 2 configuration.\n");
	// }
}
void UART_OPR_Set_Write_SmartData(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	// planck640c_write_config_408bit(&Valid_data[8]); //直接修改提示有问题
	 //如果是vot_planck1280c
	#ifdef CONFIG_SNR_PLANCK1280C
	{
	vot_planck1280c_write_config_256bit(&Valid_data[8]);
	FlashSave_SmartData256bit(&Valid_data[8]); // 保存UCC3代光智探测器256bit数据到flash中
	}
	#else
	{
		vot_planck640c_write_config_408bit(&Valid_data[8]);
		FlashSave_SmartData408bit(&Valid_data[8]); // 保存UCC3代光智探测器408bit数据到flash中
	}
	#endif
	send_data.rx_buff[8] = 0;
}
void UART_OPR_Set_AutoShutterByMin(u8 *Valid_data) // 自动快门时间设置 最小精度0.5M
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] >= 0 && Valid_data[8] <= 40)
	{
		UsermenuParameter.AutoShutter = Valid_data[8];
		if (UsermenuParameter.AutoShutter == 0)
		{
			UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
		}
		else
		{
			UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
		}

		send_data.rx_buff[8] = UsermenuParameter.AutoShutter;

		if (UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF)
		{
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
		}
		else
		{
			global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
		}
		ISP_set_auto_cut_flag_auto();
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Save_AllParameter(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0x01)
	{
		save_menu1_parameters();
		save_menu2_parameters();
		NUC_Save(1);
		OCC_Save(1);
		send_data.rx_buff[8] = Valid_data[8];
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Set_Mirror(u8 *Valid_data)
{
	// u8 MirrorFlag_value;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;

	// ts_u08 MirrorFlag_value = UsermenuParameter.Mirror;
	if (Valid_data[9] >= 0x00 && Valid_data[9] <= 0x03)
	{
		UsermenuParameter.Mirror = Valid_data[9];
		ISP_mirr_set(UsermenuParameter.Mirror);
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}

	if (Valid_data[8] == 0x01)
	{
		ISP_write_sys_cfg();
	}

	send_data.rx_buff[8] = Valid_data[8];
	send_data.rx_buff[9] = Valid_data[9];
}

void UART_OPR_TEST(u8 *Valid_data)
{
}

void UART_OPR_GetDeviceInfo(u8 *Valid_data)
{
	// u8 i,j;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	// 设置设备信息
	GetDeviceInfo(Valid_data, com_data.count);

	send_data.rx_buff[8] = 0x01;
}

void UART_OPR_SET_NUC(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0x00)
	{
		SystemParaMenu2.NUC_SW = 0;
		ISP_clr_nuc_k();
		send_data.rx_buff[8] = 0x00;
	}
	else if (Valid_data[8] == 0x01)
	{
		SystemParaMenu2.NUC_SW = 1;
		ISP_reset_nuc_k();
		send_data.rx_buff[8] = 0x01;
	}
	else if (Valid_data[8] == 0x02)
	{
		NUC_Init(1);
		send_data.rx_buff[8] = 0x02;
	}
	else if (Valid_data[8] == 0x03) // 左键
	{
		NUC_Calc(2); // low temp
		send_data.rx_buff[8] = 0x03;
	}
	else if (Valid_data[8] == 0x04) // 右键
	{
		NUC_Calc(1); // high temp
		send_data.rx_buff[8] = 0x04;
	}
	else if (Valid_data[8] == 0x05) // 保存NUC
	{
		NUC_Save(1);
		send_data.rx_buff[8] = 0x05;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}
void UART_OPR_SET_OCC(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;

	if ((Valid_data[8] == 0x02) && (Valid_data[9] == 0x00))
	{
		OCC_Init(1);
		send_data.rx_buff[8] = 0x02;
		send_data.rx_buff[9] = 0x00;
	}
	else if ((Valid_data[8] == 0x03) && (Valid_data[9] == 0x00)) // 计算
	{
		OCC_Calculate(1);
		send_data.rx_buff[8] = 0x03;
		send_data.rx_buff[9] = 0x00;
	}
	else if ((Valid_data[8] == 0x04) && (Valid_data[9] == 0x00)) // 保存OCC
	{
		OCC_Save(1);
		send_data.rx_buff[8] = 0x04;
		send_data.rx_buff[9] = 0x00;
	}
	else if ((Valid_data[8] == 0x06) && (Valid_data[9] == 0x00)) // 一键OCC
	{
		AutomaticOccParameterConfiguration();
		send_data.rx_buff[8] = 0x06;
		send_data.rx_buff[9] = 0x00;
	}
	else if ((Valid_data[8] == 0x09) && (Valid_data[9] == 0x00)) // OCC_b
	{
		OCC_c_and_b(1);
		send_data.rx_buff[8] = 0x09;
		send_data.rx_buff[9] = 0x00;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Semi_Automatic(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	if (Valid_data[8] == 0x01) // 打开坏点校正开关
	{
		if (Valid_data[9] == 0x00) // 最小值选择
		{
			if (SystemParaMenu2.BAD_SW == 0)
			{
				SystemParaMenu2.BAD_SW = 1;
				ISP_bpc_on();
			}
			SystemParaMenu2.Auto_Semi = 1;
			BADCorrectFlag = 1;

			send_data.rx_buff[8] = 0x01;
			send_data.rx_buff[9] = 0x00;
		}
		else if (Valid_data[9] == 0x01) // 最大值选择
		{
			if (SystemParaMenu2.BAD_SW == 0)
			{
				SystemParaMenu2.BAD_SW = 1;
				ISP_bpc_on();
			}
			SystemParaMenu2.Auto_Semi = 2;
			BADCorrectFlag = 1;

			send_data.rx_buff[8] = 0x01;
			send_data.rx_buff[9] = 0x01;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else if (Valid_data[8] == 0x00) // 关闭坏点校正开关
	{
		SystemParaMenu2.Auto_Semi = 0;
		BADCorrectFlag = 0;

		send_data.rx_buff[8] = 0x00;
		send_data.rx_buff[9] = 0x00;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}

	switch (SystemParaMenu2.Auto_Semi)
	{
	case 0:
		if (Area_state & 0x0010)
		{
			Area_state = (Area_state & 0xffef) | 0x0000;
			// DisPlay_BAD_Opr(&BadMes,2,0);
			BadArea_Display(0);
		}
		break;
	case 1:
	case 2:
		if ((Area_state & 0x0010) == 0)
		{
			Area_state = (Area_state & 0xffef) | 0x0010;
			select_Area = &Areainit[4];
			BadSemiAutoOpr(select_Area, 0, AreaBad_XY_Normal);
		}
		else
		{
			BadSemiAutoOpr(&Areainit[4], 0, AreaBad_XY_Normal);
		}
		break;
	}
}

void UART_OPR_Correct_One_Badpixel(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if ((Valid_data[8] <= 0x02) && (Valid_data[10] <= 0x01))
	{
		// BADCorrectFlag=1;
		BadMes.Spot_x = Valid_data[9] | (Valid_data[8] << 8);
		BadMes.Spot_y = Valid_data[11] | (Valid_data[10] << 8);
		if ((BadMes.Spot_x < IR_X_pixel) && (BadMes.Spot_y < IR_Y_pixel))
		{

			BPC_Maual_mode_set_or_unset_bpc_with_xy(BadMes.Spot_x, BadMes.Spot_y);
			NUC_Save(1); // 保存坏点信息
			send_data.rx_buff[8] = 0x00;
		}
		else
		{
			send_data.rx_buff[8] = 0x01;
		}
		// BADCorrectFlag=0;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_OneClick(u8 *Valid_data)
{
	// MessageReturnState(CommandNotSupported);
	// return;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0xA1)
	{ // 初始化菜单一
		init_menu1_parameters();
	}
	else if (Valid_data[8] == 0xA2)
	{ // 保存菜单一
		save_menu1_parameters();
	}
	else if (Valid_data[8] == 0xB2)
	{ // 初始化菜单二
		init_menu2_parameters();
	}
	else if (Valid_data[8] == 0xC2)
	{ // 菜单二参数保存
		save_menu2_parameters();
		// send_data.rx_buff[8] = 0xC2;
	}
	else if (Valid_data[8] == 0xD1)
	{
		save_menu1_parameters();
		save_menu2_parameters();
		NUC_Save(1);
		// send_data.rx_buff[8] = Valid_data[8];
	}
	else if (Valid_data[8] == 0xE1) // 功能暂无
	{
		UpdatePromptInfo("BPC init Start",1,VOT_LOG_TYPE_MENU2);
		// ISP_nuc_init();

		// UpdatePromptInfoWithTimeMs("BPC init Completed",1000,VOT_LOG_TYPE_MENU2);
		// 坏点初始化,清楚自定义坏点表（包含vot 自动过滤的坏点和手动指定的坏点）
		vot_isp_clr_bp();
	}
	else if (Valid_data[8] == 0xF1) // 一键DET
	{
		AutomaticDetParameterConfiguration();
	}
	else if (Valid_data[8] == 0xF2)
	{
		DetParInit(1);
		ts_cpl_msleep(1000);
		ISP_cut();
	}
	else if (Valid_data[8] == 0xF3) // 一键锅盖校正计算
	{
		sffc_calculate(1);
	}
	else if (Valid_data[8] == 0xF4) // 锅盖状态刷新
	{
		SFFC_Refresh(1);
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	send_data.rx_buff[8] = Valid_data[8];
}

void UART_OPR_ReadParMenu1(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];
	switch (Valid_data[8])
	{
	default:
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
	case 0x14: // 镜像状态
		send_data.rx_buff[9] = UsermenuParameter.Mirror;
		break;
	case 0x2D: // 自动打档时间间隔
		send_data.rx_buff[9] = UsermenuParameter.AutoShutter;
		break;
	// case 0x1C: //图像模式
	// send_data.rx_buff[9] = UsermenuParameter.ImageAdjust;
	// break;
	// case 0x1B: //AGC模式
	// 	send_data.rx_buff[9] = UsermenuParameter.ImageMode;
	// 	break;
	case 0x13: // 伪彩
		send_data.rx_buff[9] = UsermenuParameter.Palette;
		break;
	case 0x19: // 亮度
			   //  if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
			   //  if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
		send_data.rx_buff[9] = UsermenuParameter.Bright;
		// 	}
		// }else{
		// 	return;
		// }
		break;
	case 0x1A: // 对比度
			   //  if(UsermenuParameter.ImageAdjust == 0){//0:手动模式,1：自动模式
			   //  	if(UsermenuParameter.ImageMode == 0){//模式0：常温模式0
		send_data.rx_buff[9] = UsermenuParameter.Contrast;
		// }
		// }else{
		// 	return;
		// }
		break;
	case 0x1F: // 电子放大
		send_data.rx_buff[9] = UsermenuParameter.Enlarge;
		break;
	case 0x18: // 图像边缘增强
		send_data.rx_buff[9] = UsermenuParameter.enhance_level;
		break;
	case 0x1E: // 空间滤波
		send_data.rx_buff[9] = UsermenuParameter.SFilter;
		break;
	case 0x78: // 场景模式
		send_data.rx_buff[9] = UsermenuParameter.m_dev_env_mode;
		break;
	}
}
/** @note 查询机芯版本信息 */
void UART_OPR_Get_Firmware_Version(u8 *Valid_data)
{
	const ts_char *type_suffix = NULL;
	ts_char Vision[] = VOT_SOFTWARE_VER_FIXED_HEADER;
	ts_u08 len_core = strlen(VOT_SOFTWARE_VER_FIXED_HEADER);
	ts_u08 reserved_len = 8;                // 额外保留8个字节
	ts_u08 suffix_len = 0;
	ts_u08 i = 0;
	const ts_char *focus_ty = "";// 默认空字符串
	char buffer[10];

	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER){
		focus_ty = Focusslect[0];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
		focus_ty = Focusslect[1];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER){
		focus_ty = Focusslect[2];
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		focus_ty = Focusslect[3];
	}

    if (SystemParaMenu2.Usermenu_SW == 0) {
       snprintf(buffer, sizeof(buffer), "%s", focus_ty);   // 观测版：无后缀，保留区全填空格
    }
    else if ((SystemParaMenu2.Usermenu_SW >= 1) && (SystemParaMenu2.Usermenu_SW <= 3)) {
        // type_suffix = ".T";   // 1/2/3 -> 测温版
		snprintf(buffer, sizeof(buffer), ".T%s", focus_ty);
    }
    else {
        // type_suffix = ".E";   // 其他异常值
		snprintf(buffer, sizeof(buffer), ".E%s", focus_ty);
    }
    type_suffix = buffer;

	suffix_len = strlen(type_suffix);
	vision_log_debug("FW Ver: Usermenu_SW=%d, type_suffix=%s suffix_len=%d \n", SystemParaMenu2.Usermenu_SW, type_suffix, suffix_len);

	if (Valid_data[8] == 0x01) {
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = len_core + 5;

		for (i = 0; i < len_core; i++)
		{
			send_data.rx_buff[8 + i] = Vision[i];
		}
	}
	else if (Valid_data[8] == 0x02) // 新增：带类型标识的版本信息
	{
        send_data.rx_buff[3] = 0x00;
        send_data.rx_buff[4] = len_core + reserved_len + 6;
		send_data.rx_buff[8] = Valid_data[8];

        for (i = 0; i < len_core; i++) {
            send_data.rx_buff[9 + i] = Vision[i];
        }

        for (i = 0; i < suffix_len; i++) {
            send_data.rx_buff[9 + len_core + i] = type_suffix[i];
        }

        // 剩余字节用空格符(0x20)填充，确保占满 6 字节
        for (i = suffix_len; i < reserved_len; i++) {
            send_data.rx_buff[9 + len_core + i] = 0x20;
        }
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_OnlineUupgrade(u8 *Valid_data)
{
	static ts_u32 firm_buff=ts_null;
	ts_u32 Len;
	static ts_u32 seg = 0;
	ts_u08 *ptr, *tail;
	ts_u16 crcCal, crc;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	Len = Valid_data[3] << 8 | Valid_data[4];

	//由于升级通常是一次性的，并且是需要重启的，故升级的内存采用动态分配
	if(firm_buff==ts_null){
		// firm_buff = (ts_u32)ts_cpl_tmm_malloc(ts_null,  1024*1024*2, 32, "firmware_buff"); //2MB
		firm_buff = (ts_u32)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,1024*1024*2);
		if(firm_buff == ts_null){
			vision_log_warn("error: no memory for update firmware ! \n");
			send_data.rx_buff[8] = 0x07;
			Return_Message_Processing();
			return;
		}
	}

	if (Len == 6)
	{

		switch (Valid_data[8])
		{
		default:
			MessageReturnState(CommandNotSupported);
			return;
		case 0x01:
			send_data.rx_buff[8] = 0x01;
			global_sys_ctrl_info.m_firmware_len = 0;
			Return_Message_Processing();
			global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_UPDATE_FIRMWARE;
			return;
		case 0x02: // 取消应答
			// VotTools.BIGDATA_FALG = 1;
			send_data.rx_buff[8] = 0x02;
			// Return_Message_Processing();
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x00;
			return;

		case 0x03:
		{
			global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
			// VotTools.BIGDATA_FALG = 1;

			if (firm_buff == ts_null || global_sys_ctrl_info.m_firmware_len == 0)
			{
				vision_log_warn("error: no memory for update firmware ! \n");
				send_data.rx_buff[8] = 0x06;
				// VotParameters.Update_Firmware_Len = 0;
				global_sys_ctrl_info.m_firmware_len = 0;
				VotTools.BIGDATA_FALG = 0;
				Return_Message_Processing();
				return;
			}

			// [16byte fixed head] + [ ts_vison.bin] + [2byte crc16-mobus]
			ptr = (ts_u08 *)firm_buff;

			tail = ptr + global_sys_ctrl_info.m_firmware_len;

			// asic固件 + 64 Bytes asic tail + 2bytes(crc-16)
			AsicFirmwareTail_s_t firm_tail;
			ts_cpl_memcpy(&firm_tail, tail - 66, 64);

			// crc = *(tail - 34)<<8| *(tail - 33);
			crc = firm_tail.crc[0] << 8 | firm_tail.crc[1];

			// 为了兼容asic官方烧写上位机，不加开头的16 Bytes
			global_sys_ctrl_info.m_firmware_len = global_sys_ctrl_info.m_firmware_len - 66;

			// ptr = ptr+16;

			Data_Verification(ptr, global_sys_ctrl_info.m_firmware_len);

			crcCal = VotArrays.Rcvbuf[0] << 8 | VotArrays.Rcvbuf[1];

			// vision_log_debug(" crcCal %u  crc %u \n",crcCal, crc);
			if (crc == crcCal)
			{

				ts_u08 version_vaild = check_asic_firmware_version(&firm_tail);
				if (version_vaild > 0)
				{
					// 固件版本错误
					send_data.rx_buff[8] = 0x05;
				}
				else
				{
					// flash 地址
					ts_u32 u32_addr = 0x80000;
					ts_s32 ret = 0;
					ts_u32 u32_wr_size = 0;

					ts_u32 update_ok_cnt = 0;

					u32_wr_size = ((global_sys_ctrl_info.m_firmware_len + 3) / 4) * 4;
					
					ts_u32 u32_blk_addr = 4;


					if(SystemParaMenu2.m_update_firmware_policy_mode == VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B){
						vision_log_debug(" VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B ! \n");

						vision_log_debug(" B firmware updating ! \n");
						
						// 双备份固件写入
						u32_wr_size = ((global_sys_ctrl_info.m_firmware_len + 3) / 4) * 4;
						u32_addr = 0x180000;

						if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
							u32_blk_addr = 24;
						}
						else{
							u32_blk_addr = 12; //128MB spi nand flash

						}
						ts_lib_sfc_set_protect(0, u32_blk_addr, 0);
						ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
						if (ret > 0)
						{
							update_ok_cnt += 1;
							vision_log_debug(" B firmware update success ! \n");
						}
						else{
							vision_log_debug(" B firmware update failed ! \n");
						}

						ts_cpl_msleep(500);
						ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
						if(ret<0){
							vision_log_debug(" B firmware in falsh crc check failed ! \n");
							update_ok_cnt = 0;

							ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
							if (ret > 0)
							{
								ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
								if(ret==0){
									update_ok_cnt += 1;
								}
								vision_log_debug(" B firmware update success ! \n");
							}
							else{
								vision_log_debug(" B firmware update failed ! \n");
							}
						}
						ts_lib_sfc_set_protect(0, u32_blk_addr, 1);
						

						//先烧写第二份固件，只有第二份固件烧写成功，才进行第1份固件烧写，防止第二份烧写异常，导致第一份固件也丢失
						if(update_ok_cnt==1){
							ts_cpl_msleep(500);

							u32_addr = 0x80000;
							if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
								u32_blk_addr = 8;
							}
							else{
								u32_blk_addr = 4;

							}
							ts_lib_sfc_set_protect(0, u32_blk_addr, 0);
							vision_log_debug(" A firmware updating ! \n");
							ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
							if (ret > 0)
							{
								update_ok_cnt += 1;
								vision_log_debug(" A firmware update success ! \n");
							}
							else{
								vision_log_debug(" A firmware update failed ! \n");
							}
							
							ts_cpl_msleep(500);
							ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
							if(ret<0){
								vision_log_debug(" A firmware in falsh crc check failed ! \n");
								update_ok_cnt = 0;
								//尝试再写一次
								ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
								if (ret > 0)
								{
									ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
									if(ret==0){
										update_ok_cnt += 1;
									}
									vision_log_debug(" A firmware update success ! \n");
								}
								else{
									vision_log_debug(" A firmware update failed ! \n");
								}
							}
							
							ts_lib_sfc_set_protect(0, u32_blk_addr, 1);
						
						}
					}
					else if(SystemParaMenu2.m_update_firmware_policy_mode == VOT_UPDATE_FIRMWARE_POLICY_ONLY_A){
						vision_log_debug(" VOT_UPDATE_FIRMWARE_POLICY_ONLY_A ! \n");

						u32_addr = 0x80000;
						if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
							u32_blk_addr = 8;
						}
						else{
							u32_blk_addr = 4;

						}
						ts_lib_sfc_set_protect(0, u32_blk_addr, 0);
						vision_log_debug(" A firmware updating ! \n");
						ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
						if (ret > 0)
						{
							update_ok_cnt += 1;
							vision_log_debug(" A firmware update success ! \n");
						}
						else{
							vision_log_debug(" A firmware update failed ! \n");
						}

						ts_cpl_msleep(500);
						ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
						if(ret<0){
							vision_log_debug(" A firmware in falsh crc check failed ! \n");
							update_ok_cnt = 0;

							ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
							if (ret > 0)
							{
								ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
								if(ret==0){
									update_ok_cnt += 1;
								}
								vision_log_debug(" A firmware update success ! \n");
							}
							else{
								vision_log_debug(" A firmware update failed ! \n");
							}
						}

						ts_lib_sfc_set_protect(0, u32_blk_addr, 1);
					}
					else  if(SystemParaMenu2.m_update_firmware_policy_mode == VOT_UPDATE_FIRMWARE_POLICY_ONLY_B){
						    vision_log_debug(" VOT_UPDATE_FIRMWARE_POLICY_ONLY_B ! \n");
							vision_log_debug(" B firmware updating ! \n");
							
							u32_wr_size = ((global_sys_ctrl_info.m_firmware_len + 3) / 4) * 4;
							u32_addr = 0x180000;

							if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
								u32_blk_addr = 24;
							}
							else{
								u32_blk_addr = 12; //128MB spi nand flash

							}
							ts_lib_sfc_set_protect(0, u32_blk_addr, 0);
							ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
							if (ret > 0)
							{
								update_ok_cnt += 1;
								vision_log_debug(" B firmware update success ! \n");
							}
							else{
								vision_log_debug(" B firmware update failed ! \n");
							}

							ts_cpl_msleep(500);
							ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
							if(ret<0){
								vision_log_debug(" B firmware in falsh crc check failed ! \n");
								update_ok_cnt = 0;

								ret = ts_lib_sfc_raw_write(u32_addr, ptr, u32_wr_size);
								if (ret > 0)
								{
									ret = vot_check_firmware_crc32_from_flash(crc, u32_addr, u32_wr_size);
									if(ret==0){
										update_ok_cnt += 1;
									}
									vision_log_debug(" B firmware update success ! \n");
								}
								else{
									vision_log_debug(" B firmware update failed ! \n");
								}
							}

							ts_lib_sfc_set_protect(0, u32_blk_addr, 1);
					}

					// 只要一个分区升级成功，就当做升级成功
					if (update_ok_cnt == 0)
					{
						send_data.rx_buff[8] = 0x07;
					}
					else
					{
						send_data.rx_buff[8] = 0x03;
						// menu1\_default_parameters_init();
						// ISP_write_sys_cfg();
					}
					Return_Message_Processing();

					//只有升级成功才会自动重启系统
					if (update_ok_cnt){
						//关闭阳光保护
						SystemParaMenu2.SunPT_SW = 0;
						// 1秒后重启系统
						
						vot_vpo_freeze();
						ts_cpl_fix_msleep(1000);
						// 软件主动断开电源，保护探测器
						planck640c_stop();
						cli_simple_run_command("reboot");
					}

					if(firm_buff){
						// ts_cpl_tmm_free((ts_void *)firm_buff);
						vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&firm_buff);
						firm_buff = ts_null;
					}

				}
			}
			else
			{
				// 固件校验失败

				send_data.rx_buff[8] = 0x04;

			}
			global_sys_ctrl_info.m_firmware_len = 0;
			VotTools.BIGDATA_FALG = 0;
			if(firm_buff){
				ts_cpl_tmm_free((ts_void *)firm_buff);
				firm_buff = ts_null;
			}
			Return_Message_Processing();
			return;
		}
		}
	}
	else
	{

		ptr = (ts_u08 *)firm_buff;

		seg = Valid_data[8] << 8 | Valid_data[9];

		// 初始段,校验版本字段
		//  if(seg == 0) {
		//  	// u8 str[] = CPU_VERSION;
		//  	// u8 str1[16] = "C";
		//  	// u8 *pos1 = strchr(str, 0x5F);
		//  	// u8 *pos2 = strchr(pos1, 0x20);
		//  	// strncpy(str1 + 1, pos1, pos2 - pos1);

		// 	// for(u8 i = 0, j = 10; i < 16; i++, j++) {
		// 	// 	if(str1[i] != Valid_data[j]) {
		// 	// 		send_data.rx_buff[8] = 0x05;
		// 	// 		VotTools.BIGDATA_FALG = 0;
		// 	// 		Return_Message_Processing();
		// 	// 		return;
		// 	// 	}
		// 	// }
		// }

		Len = Len - 7; // 去除多余字节
		// 目前协议中，包不可能大于20248字节
		if (Len > 2048)
		{
			send_data.rx_buff[8] = 0x05;
			// VotTools.BIGDATA_FALG = 0;
			Return_Message_Processing();
			return;
		}

		tail = ptr + global_sys_ctrl_info.m_firmware_len;
		ts_cpl_memcpy(tail, &Valid_data[10], Len);

		global_sys_ctrl_info.m_firmware_len = global_sys_ctrl_info.m_firmware_len + Len;
		// vision_log_debug("seg %d m_firmware_len %u  Len %u \n",seg,global_sys_ctrl_info.m_firmware_len, Len);
		// VotTools.BIGDATA_TIME = 0;

		if (seg == 0)
		{
			// 56 4f 54 00 06 00 CA 3A 06 7A B6
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x06;
			ts_u08 pipeline_number = 0x00;
			send_data.rx_buff[8] = 0x06;
			Return_Message_Processing_with_pipeline_number(pipeline_number);
			// 外层发送不执行
		}
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x00;
	}
}

void UART_OPR_PC_Correct_Badpixel(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x0A;
	/*假如截像素，传入的384坐标要在这里全部转换成640的坐标*/

	vision_log_debug("Valid_data[8] %d  \n", Valid_data[8]);
	switch (Valid_data[8])
	{
	case 0: // 开始PC坏点标定模式
		if (global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_NORMAL)
		{
			global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc = OUTPUT_MODE_ENLARGE_OFF;
			SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_UVC;
			EnterOrExitBPCMaualMode(0);
			// 开启提示信息
			cvbs_osd_refresh_display(1);
			// 关闭光标
			cvbs_cursor_refresh_display_auto();
		}

		// Enter_PC_BPCMaualMode();
		//			PCMoveCursorBPC(global_sys_ctrl_info.m_BPC_cursor_pos_x,global_sys_ctrl_info.m_BPC_cursor_pos_y);
		break;
	case 1: // 表示开启PC坏点标定模式
		if (global_sys_ctrl_info.m_system_mode != SYSTEM_MODE_BPC)
			return;
		//			PCMoveCursorBPC((Valid_data[9]<<8)+Valid_data[10],(Valid_data[11]<<8)+Valid_data[12]);
		ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
		PC_MarkBadPixels((Valid_data[9] << 8) + Valid_data[10], (Valid_data[11] << 8) + Valid_data[12]);
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
		break;
	case 2: // 开启/取消坏点校正
		// if(global_sys_ctrl_info.m_system_mode != SYSTEM_MODE_BPC) return;
		// if(Valid_data[9])
		// {
		// 	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
		// 	PC_CorrectBadPixels();
		// 	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

		// }
		// else
		// {
		// 	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
		// 	PC_CancalAllMarkBadPixels();
		// 	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

		// }
		break;
	case 3: // 保存/初始化坏点坐标

		break;
	case 4: // 取消上一次标记的一个坏点
		if (global_sys_ctrl_info.m_system_mode != SYSTEM_MODE_BPC)
			return;
		ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
		PC_CancalOneMarkBadPixels((Valid_data[9] << 8) + Valid_data[10], (Valid_data[11] << 8) + Valid_data[12]);
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
		break;
	case 5: // 关闭PC坏点标定模式
		if (global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_BPC)
		{
			global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc = OUTPUT_MODE_ENLARGE_ON;
			EnterOrExitBPCMaualMode(1);
			SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_CVBS;
			// cvbs_osd_refresh_display(1);
		}
		// EXIT_PC_BPCMaualMode();
		break;
	case 6: // 显示/隐藏光标
	{
		cvbs_cursor_refresh_display_auto();
	}
	break;
	}
	send_data.rx_buff[8] = Valid_data[8];
}

void UART_OPR_SET_Env_Mode(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] <= (DEVICE_MODE_OUTDOOR_BPC - 1))
	{
		ts_u08 dev_env_mode = Valid_data[8];

		vot_m_dev_env_mode_set(dev_env_mode);

		send_data.rx_buff[8] = Valid_data[8];
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Sunlight_Protection(u8 *Valid_data)
{
	u16 temp;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;

	send_data.rx_buff[8] = Valid_data[8];
	temp = Valid_data[9] << 8 | Valid_data[10];
	if (Valid_data[8] == 0x01) // 阳光保护开关
	{
		if (temp == 1 || temp == 0)
		{
			ts_u16 pt_sw = Valid_data[10];
			// SystemParaMenu2.SunPT_SW = Valid_data[10];
			ISP_set_sun_pt_sw(pt_sw);
			// APB3_REGW_SunPT_SW(SystemParaMenu2.SunPT_SW);
			send_data.rx_buff[9] = pt_sw;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else if (Valid_data[8] == 0x02) // 查询参数状态
	{
		if (temp == 0)
		{
			send_data.rx_buff[9] = SystemParaMenu2.SunPT_SW;
			send_data.rx_buff[10] = SystemParaMenu2.S_Range >> 8;
			send_data.rx_buff[11] = SystemParaMenu2.S_Range >> 0;
			send_data.rx_buff[12] = SystemParaMenu2.S_Val >> 8;
			send_data.rx_buff[13] = SystemParaMenu2.S_Val >> 0;
			ts_u16 pt_gap = ISP_get_sun_pt_gap();
			send_data.rx_buff[14] = pt_gap >> 8;
			send_data.rx_buff[15] = pt_gap >> 0;

			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x0D;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else if (Valid_data[8] == 0x03) // 修改阈值
	{
		if (temp < 0)
		{
			SystemParaMenu2.S_Range = 0;
		}
		else if (temp > MAX_SUN_PT_GRAY_LEVEL)
		{
			SystemParaMenu2.S_Range = MAX_SUN_PT_GRAY_LEVEL;
		}
		else
		{
			SystemParaMenu2.S_Range = temp;
		}
		// if(SystemParaMenu2.Usermenu_SW){
		// 	if(SystemParaMenu2.Usermenu_SW==1)				{SystemParaMenu2.S_Range1 = SystemParaMenu2.S_Range;}
		// 	else if(SystemParaMenu2.Usermenu_SW==2)		{SystemParaMenu2.S_Range2 = SystemParaMenu2.S_Range;}
		// 	APB3_REGW_S_Range(SystemParaMenu2.S_Range);}
		// else {
		// 	SystemParaMenu2.S_Range0 = SystemParaMenu2.S_Range;
		// 	APB3_REGW_S_Range(SystemParaMenu2.S_Range - FPGA_Bak_AVG()/2);}

		send_data.rx_buff[9] = 0x01;
	}
	else if (Valid_data[8] == 0x04) // 修改超出阈值个数
	{
		// SystemParaMenu2.S_Val = temp;
		ISP_set_sun_pt_val(temp);
		// if(SystemParaMenu2.Usermenu_SW){
		// 	if(SystemParaMenu2.Usermenu_SW==1)				{SystemParaMenu2.S_Val1 = SystemParaMenu2.S_Val;}
		// 	else if(SystemParaMenu2.Usermenu_SW==2)		{SystemParaMenu2.S_Val2 = SystemParaMenu2.S_Val;}}
		// else	SystemParaMenu2.S_Val0 = SystemParaMenu2.S_Val;

		send_data.rx_buff[9] = 0x01;
	}
	else if (Valid_data[8] == 0x05) // 修改保护时间
	{
#if 1

		ISP_set_sun_pt_gap(temp);
#else
		if (temp < 1)
		{
			UsermenuParameter.P_second_Time = 1;
		}
		else if (temp > 1200)
		{
			UsermenuParameter.P_second_Time = 1200;
		}
		else
		{
			UsermenuParameter.P_second_Time = temp;
		}
#endif
		send_data.rx_buff[9] = 0x01;
	}
	else if (Valid_data[8] == 0x06) // 弹开挡片保护
	{
		// APB3_REGW_Shutter_SW(ON);
		// //SetFreeze_img(ON);
		// delay_1ms(50);
		// shutterOpr(ON);
		// //SetFreeze_img(OFF);
		// APB3_REGW_Shutter_SW(OFF);

		// ShutterTimer=0;
		// ProtectFlag = 0;
		// delay_1ms(200);
		// Int_Shutter_pro();
		ProtectTime = 0;
		// 强制打开挡片
		vot_shutter_force_open();
		// 退出当前阳光保护操作
		ISP_set_sun_pt_force_flag(0);
		vot_set_sun_pt_notice_info(0);
		ISP_cut_new();

		send_data.rx_buff[9] = 0x01;
	}
	else if (Valid_data[8] == 0x07) // 提示信息是否显示
	{
		if (temp == 1 || temp == 0)
		{
			ts_u08 pt_info_sw = Valid_data[10];

			ISP_set_sun_pt_info_sw(pt_info_sw);
			send_data.rx_buff[9] = pt_info_sw;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else if (Valid_data[8] == 0x08) // 阳光保护模式,auto/manual
	{
		if (temp == 1 || temp == 0)
		{
			ts_u08 sun_pt_mode = Valid_data[10];

			ISP_set_sun_pt_mode(sun_pt_mode);
			send_data.rx_buff[9] = sun_pt_mode;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_ShutterControl(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if (Valid_data[8] == 0x02) // 开启挡片
	{
		vot_shutter_force_open();
		send_data.rx_buff[8] = 0x02;
	}
	else if (Valid_data[8] == 0x00) // 关闭挡片
	{
		vot_shutter_force_close();
		send_data.rx_buff[8] = 0x00;
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_SET_BPC(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x08;

	send_data.rx_buff[8] = Valid_data[8];
	send_data.rx_buff[9] = Valid_data[9];

	if (Valid_data[8] == 0xA0) // 坏斑功能bp3
	{
		if (Valid_data[9] == 0x00) // 设置功能开关与索引号
		{
			// Valid_data[10] 为索引号 1~10
			// Valid_data[11]，00：关闭，01：开启

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x01) // 设置坏斑坐标
		{
			// Valid_data[10]|Valid_data[11]为坐标X，Valid_data[12]|Valid_data[13]为坏斑坐标Y

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x02) // 设置坏斑大小
		{
			// Valid_data[10]为坏斑大小1~5

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x03) // 删除对应索引号坏点信息
		{
			// Valid_data[10]为对应的索引号

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else if (Valid_data[8] == 0xA1) // 区域设置功能bp2
	{
		if (Valid_data[9] == 0x00) // 设置功能开关与索引号
		{
			// Valid_data[10] 为索引号 0~15
			// Valid_data[11]，00：关闭，01：开启
			ts_u08 Num, SW;
			Num = Valid_data[10];
			SW = Valid_data[11] & 0x01;
			if (Num > EXTBPC_POSITION_MAX)
				Num = EXTBPC_POSITION_MAX;
			SystemParaMenu2.Extbpc_para[Num].enable = SW;
			vot_snr_set_ext_bp2(Num, SystemParaMenu2.Extbpc_para[Num].enable, SystemParaMenu2.Extbpc_para[Num].bad_x, SystemParaMenu2.Extbpc_para[Num].bad_y,
								SystemParaMenu2.Extbpc_para[Num].bad_w, SystemParaMenu2.Extbpc_para[Num].bad_h);

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x01) // 设置区域起始坐标
		{
			// Valid_data[10]|Valid_data[11]为区域X1，Valid_data[12]|Valid_data[13]为区域Y1
			// Valid_data[14]|Valid_data[15]为区域宽，Valid_data[16]|Valid_data[17]为区域高
			ts_u08 number;
			number = Valid_data[10];
			if (number > EXTBPC_POSITION_MAX)
				number = EXTBPC_POSITION_MAX;
			SystemParaMenu2.Extbpc_para[number].bad_x = (Valid_data[11] << 8) | Valid_data[12];
			SystemParaMenu2.Extbpc_para[number].bad_y = (Valid_data[13] << 8) | Valid_data[14];

			SystemParaMenu2.Extbpc_para[number].bad_w = Valid_data[15];
			if (SystemParaMenu2.Extbpc_para[number].bad_w > 7)
				SystemParaMenu2.Extbpc_para[number].bad_w = 7;
			else if (SystemParaMenu2.Extbpc_para[number].bad_w < 1)
				SystemParaMenu2.Extbpc_para[number].bad_w = 1;

			SystemParaMenu2.Extbpc_para[number].bad_h = Valid_data[16];
			if (SystemParaMenu2.Extbpc_para[number].bad_h > 7)
				SystemParaMenu2.Extbpc_para[number].bad_h = 7;
			else if (SystemParaMenu2.Extbpc_para[number].bad_h < 1)
				SystemParaMenu2.Extbpc_para[number].bad_h = 1;

			vot_snr_set_ext_bp2(number, SystemParaMenu2.Extbpc_para[number].enable, SystemParaMenu2.Extbpc_para[number].bad_x, SystemParaMenu2.Extbpc_para[number].bad_y,
								SystemParaMenu2.Extbpc_para[number].bad_w, SystemParaMenu2.Extbpc_para[number].bad_h);

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x02) // 删除对应索引号区域信息
		{
			// Valid_data[10]为对应的索引号
			ts_u08 number;
			number = Valid_data[10];
			if (number > 15) {
				number = 15;
			}
        	SystemParaMenu2.Extbpc_para[number].enable = 0;
			vot_snr_del_ext_bp(number);

			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x03) // 初始化
		{
			if (Valid_data[10] == 0x01) // 初始化所有区域使能
			{
				// 初始化操作
				Bpc_ext_Init(1);
			}
			else if (Valid_data[10] == 0x02) // 关闭坏斑模块
			{
				SystemParaMenu2.Bpc_ext_SW = 0;
				UpdatePromptInfoWithTimeMs("please reboot system for bpc ext !", 1000,VOT_LOG_TYPE_MENU2);
			}
			else if (Valid_data[10] == 0x03) // 开启坏斑模块
			{
				SystemParaMenu2.Bpc_ext_SW = 1;
				UpdatePromptInfoWithTimeMs("please reboot system for bpc ext !", 1000,VOT_LOG_TYPE_MENU2);
			}
			else
			{
				MessageReturnState(CommandNotSupported);
				return;
			}
			send_data.rx_buff[10] = 0x01; // 设置成功
		}
		else if (Valid_data[9] == 0x04) // 单区域信息查询
		{
			// Valid_data[10]为对应的索引号
			ts_u08 number;
			number = Valid_data[10];
			if (number > 15)
				number = 15;

			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x10;

			send_data.rx_buff[10] = SystemParaMenu2.Extbpc_para[number].enable;
			send_data.rx_buff[11] = SystemParaMenu2.Extbpc_para[number].bad_x >> 8;
			send_data.rx_buff[12] = SystemParaMenu2.Extbpc_para[number].bad_x;
			send_data.rx_buff[13] = SystemParaMenu2.Extbpc_para[number].bad_y >> 8;
			send_data.rx_buff[14] = SystemParaMenu2.Extbpc_para[number].bad_y;
			send_data.rx_buff[15] = SystemParaMenu2.Extbpc_para[number].bad_w;
			send_data.rx_buff[16] = SystemParaMenu2.Extbpc_para[number].bad_h;
		}
		else if (Valid_data[9] == 0x05) // 多区域信息查询
		{
			ts_u08 i;
			ts_u08 *p = &send_data.rx_buff[12];   // 数据从 [11] 开始

			send_data.rx_buff[10] = EXTBPC_POSITION_MAX + 1;   // 区域个数 = 8
			send_data.rx_buff[11] = SystemParaMenu2.Bpc_ext_SW;

			for (i = 0; i <= EXTBPC_POSITION_MAX; i++)
			{
				*p++ = i;                                              // 索引号 0~7
				*p++ = SystemParaMenu2.Extbpc_para[i].enable;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_x >> 8;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_x;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_y >> 8;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_y;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_w >> 8;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_w;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_h >> 8;
				*p++ = SystemParaMenu2.Extbpc_para[i].bad_h;
			}
			ts_u16 rx_len = 9 + (10 * (EXTBPC_POSITION_MAX + 1));
			send_data.rx_buff[3] = rx_len >> 8;
			send_data.rx_buff[4] = rx_len;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	else
	{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_AutoFocus(u8 *Valid_data)
{
	// uint8_t  Message1[5]={0};
	uint32_t test = 0;
	uint8_t Motor_Speed,Motor_Step;
	u16 temp;
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_PRIME_LENS){
		MessageReturnState(CommandNotSupported);return;
	}
	temp = Valid_data[3]<<8|Valid_data[4];

	if(temp == 0x09)
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x0A;
	/***********FocusMotor*************/
		if(Valid_data[8] == 0xAA) //自动调焦功能
		{
			if((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x00))
			{
				if(!SystemParaMenu2.Zoom_Lens){
					FocusParaInit();
					FocusParameters.FocusStepMax = MeasureParameters.LimitStepMax;
					FocusParameters.FocusStepMin = MeasureParameters.LimitStepMin;
					FocusParameters.FocusFlag = 1;
				}else{
					FocusParaInit();
					FocusParameters.FocusFlag = 1;
				}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0;
				send_data.rx_buff[10]=0;
				send_data.rx_buff[11] = 0;
				send_data.rx_buff[12] = 0;
			}
			else if ((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x01))
			{
				if(SystemParaMenu2.FocusSite && SystemParaMenu2.FocusSite!=0xFFFF)
				{
					FocusParaInit();
					FocusParameters.FocusStepMax = SystemParaMenu2.FocusSite + SystemParaMenu2.Left_Val;
					FocusParameters.FocusStepMin = SystemParaMenu2.FocusSite - SystemParaMenu2.Right_Val;
					FocusParameters.FocusFlag = 1;
				}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0;
				send_data.rx_buff[10]=0;
				send_data.rx_buff[11] = 0;
				send_data.rx_buff[12] = 1;
			}
		}
		else if(Valid_data[8] == 0xBB)//自动测量程
		{
			if((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x00))
			{
				Focus_Calibration_Flag = 1;
				MeasureParaInit();
				FocusParaInit();

			if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER){  
				Encoder_Clear(0,Focus_MCP3221_addr);
				Encoder_Clear(0,Zoom_MCP3221_addr);
			}
//						MeasureParameters.MeasureFlag = 1;
//						MotorAutoMeasure();
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] =  0;
				send_data.rx_buff[10]= 0;
				send_data.rx_buff[11] = 0;
				send_data.rx_buff[12] = 0;
			}

		}
		/*调焦电机单步操作*/
		else if(Valid_data[8] == 0xCC)	//单步
		{
			if(Valid_data[9]== 0x01) //单步向前
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];//5
				FocusMotorOperate(NormalState,Motor_Speed,MeasureParameters.focusForward,Motor_Step);
				ts_cpl_msleep(50);
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,5);
				ts_cpl_msleep(50);
			}
			else if(Valid_data[9]== 0x02) //单步向后
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11]; //5
				FocusMotorOperate(NormalState,Motor_Speed,MeasureParameters.focusReversal,Motor_Step);
				ts_cpl_msleep(50);
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
				ts_cpl_msleep(50);
			}
			test = GetVol(Focus_MCP3221_addr);
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = test>>24;
			send_data.rx_buff[10]= test>>16;
			send_data.rx_buff[11] = test>>8;
			send_data.rx_buff[12] = test>>0;

			// vision_log_debug("AFSVal == [%d]\n",AFS_EvaluationFuc_Cal());
			// AFS_EvaluationFuc_Cal();
		}
		/*调焦电机持续转动操作*/
		else if(Valid_data[8] == 0xDD)	//一直转动或到达尽头
		{
			if(Valid_data[9]== 0x01) //一直向前到达尽头
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];	//2
				FocusMotorOperate(NormalState,Motor_Speed,MeasureParameters.focusForward,Motor_Step);  //MotorForward
	//  		delay_1ms(100);
	// 			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
			}
			else if(Valid_data[9]== 0x02) //一直向后到达尽头
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];	//2
				FocusMotorOperate(NormalState,Motor_Speed,MeasureParameters.focusReversal,Motor_Step);  //  MeasureParameters.focusReversal
				//delay_1ms(100);
				//FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
			}
			test = GetVol(Focus_MCP3221_addr);
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = test>>24;
			send_data.rx_buff[10]= test>>16;
			send_data.rx_buff[11] = test>>8;
			send_data.rx_buff[12] = test>>0;

		}
		else if(Valid_data[8] == 0xFF)	//停止持续转动
		{
			if((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x00))
			{
				FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
				ts_cpl_msleep(50);
				test = GetVol(Focus_MCP3221_addr);
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = test>>24;
				send_data.rx_buff[10]= test>>16;
				send_data.rx_buff[11] = test>>8;
				send_data.rx_buff[12] = test>>0;

			}
		}
		else if(Valid_data[8] == 0xEE)	//电机标定
		{
			if((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x00))
			{
				Focus_Calibration_Flag = 1;
				MeasureParaInit();
				SystemParaMenu2.Zoom_Lens =0;
				SystemParaMenu2.Left_Val = 100;
				SystemParaMenu2.Right_Val = 100;
				SystemParaMenu2.FocusSite = 0;
				//Write_TestmenuParameterToFlash();
//						MeasureParameters.MeasureFlag = 1;
//						MotorAutoMeasure();
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] =  0;
				send_data.rx_buff[10]= 0;
				send_data.rx_buff[11] = 0;
				send_data.rx_buff[12] = 0;
			}
		}
		/***********ZoomMotor*************/

		else if(Valid_data[8] == 0x2A) //自动变倍
		{
			if(Valid_data[10] == 0x00 && Valid_data[11] == 0x00 )
			{
				FocusParameters.FocusFlag = 0;
				MeasureParameters.MeasureFocusFlag = 0;
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				ZoomParameters.ZoomFlag = 1;
				switch(Valid_data[9]){
				default:break;
				case 1:ZoomParameters.ZoomSite = 0x96; break;
				case 2:ZoomParameters.ZoomSite = 0x390; break;
				case 4:ZoomParameters.ZoomSite = 0x7A8; break;
				case 8:ZoomParameters.ZoomSite = 0xC5C; break;
				}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = Valid_data[9];
				send_data.rx_buff[10] = 0;
				send_data.rx_buff[11] = 0;
				send_data.rx_buff[12] = 0;
			}
		}
		else if(Valid_data[8] == 0x2C)	//单步
		{
			if(Valid_data[9]== 0x01) //单步放大
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];//5
				ZoomMotorOperate(NormalState,Motor_Speed,MeasureParameters.zoomForward,Motor_Step);
				delay_1ms(50);
				ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
				delay_1ms(50);
			}
			else if(Valid_data[9]== 0x02) //单步缩小
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11]; //5
				ZoomMotorOperate(NormalState,Motor_Speed,MeasureParameters.zoomReversal,Motor_Step);
				delay_1ms(50);
				ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
				delay_1ms(50);
			}
			test = GetVol(Zoom_MCP3221_addr);
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = test>>24;
			send_data.rx_buff[10]= test>>16;
			send_data.rx_buff[11] = test>>8;
			send_data.rx_buff[12] = test>>0;
		}
		else if(Valid_data[8] == 0x2D)	//持续放大
		{
			if(Valid_data[9]== 0x01) //
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];
				ZoomMotorOperate(NormalState,Motor_Speed,MeasureParameters.zoomForward,Motor_Step);
	//  		delay_1ms(100);
	// 			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
			}
			else if(Valid_data[9]== 0x02) //持续缩小
			{
				Motor_Speed = Valid_data[10];
				Motor_Step = Valid_data[11];
				ZoomMotorOperate(NormalState,Motor_Speed,MeasureParameters.zoomReversal,Motor_Step);
				//delay_1ms(100);
				//FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,5);
			}
			test = GetVol(Zoom_MCP3221_addr);
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = test>>24;
			send_data.rx_buff[10]= test>>16;
			send_data.rx_buff[11] = test>>8;
			send_data.rx_buff[12] = test>>0;

		}
		else if(Valid_data[8] == 0x2F)	//停止持续转动
		{
			if((Valid_data[9] == 0x00)&&(Valid_data[10] == 0x00)&&(Valid_data[11] == 0x00))
			{
				ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
				delay_1ms(50);
				test = GetVol(Zoom_MCP3221_addr);

				FocusParaInit();
				FocusParameters.FocusFlag = 1;

				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = test>>24;
				send_data.rx_buff[10]= test>>16;
				send_data.rx_buff[11] = test>>8;
				send_data.rx_buff[12] = test>>0;
			}
		}

		else
		{MessageReturnState(CommandNotSupported);return;}
	}
	else if(temp == 0x08)
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x06;

		if(Valid_data[8] == 0x01){
			if(Valid_data[9] == 0 && Valid_data[10] > 0x00 && Valid_data[10] <= 0x02)
			{
				SystemParaMenu2.Zoom_Lens = Valid_data[10] - 1;
			}
			else
			{MessageReturnState(CommandNotSupported);return;}
		}else if(Valid_data[8] == 0x02){
			temp = Valid_data[9]<<8 | Valid_data[10];
			if(temp >= 0x1E && temp <= 0xFFFF)
			{
				SystemParaMenu2.Left_Val = temp;
				//Write_TestmenuParameterToFlash();
			}
			else
			{MessageReturnState(CommandNotSupported);return;}
		}else if(Valid_data[8] == 0x03){
			temp = Valid_data[9]<<8 | Valid_data[10];
			if(temp >= 0x1E && temp <= 0x1F4)
			{
				SystemParaMenu2.Right_Val = temp;
				//Write_TestmenuParameterToFlash();
			}
			else
			{MessageReturnState(CommandNotSupported);return;}
		}else if(Valid_data[8] == 0x04){
			FocusParameters.PresetPos = Valid_data[9]<<8 | Valid_data[10];
			MotorPresetParameters.PresetSwitch = ON_State;
//					if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
//						FocusParameters.PresetRightFlag = 1;
//					}else{
//						FocusParameters.PresetLeftFlag = 1;
//					}
		}else if(Valid_data[8] == 0x05){
			ZoomParameters.PresetPos = Valid_data[9]<<8 | Valid_data[10];
			MotorPresetParameters.PresetSwitch = ON_State;
//					if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//						ZoomParameters.PresetRightFlag = 1;
//					}else{
//						ZoomParameters.PresetLeftFlag = 1;
//					}
		}
		send_data.rx_buff[8] = 0x01;
		// Write_TestmenuParameterToFlash();
	}else if(temp == 0x0E){			//设置聚焦区域
		if(Valid_data[8] == 0x04){
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x06;

			SystemParaMenu2.Focus_Lx = Valid_data[9]<<8|Valid_data[10];
			SystemParaMenu2.Focus_Ly = Valid_data[11]<<8|Valid_data[12];
			SystemParaMenu2.Focus_Rx = Valid_data[13]<<8|Valid_data[14];
			SystemParaMenu2.Focus_Ry = Valid_data[15]<<8|Valid_data[16];

			Data_u16_Limit(&SystemParaMenu2.Focus_Lx,630,10);
			Data_u16_Limit(&SystemParaMenu2.Focus_Rx,630,10);
			Data_u16_Limit(&SystemParaMenu2.Focus_Ly,510,10);
			Data_u16_Limit(&SystemParaMenu2.Focus_Ry,510,10);

			// FocusAreaSet(SystemParaMenu2.Focus_Lx,SystemParaMenu2.Focus_Ly,SystemParaMenu2.Focus_Rx,SystemParaMenu2.Focus_Ry);
		}
		send_data.rx_buff[8] = 0x01;
		// Write_TestmenuParameterToFlash();
	}
	else
	{MessageReturnState(CommandNotSupported);return;}
}

void UART_OPR_FollowFocus(u8 *Valid_data)
{
	u32 test;
	// u32 ImageVal[8] = {0};
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_PRIME_LENS){
		MessageReturnState(CommandNotSupported);return;
	}
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;

	if(Valid_data[8] == 0x01){
		Data_u8_Limit(&Valid_data[9],9,0);
		PresetPositionParaInit();
		MotorPresetParameters.CalibrationFlag = 1;
		MotorPresetParameters.PresetNum = Valid_data[9];
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = MotorPresetParameters.PresetNum;
	}else if(Valid_data[8] == 0x00){
		MotorPresetParameters.CalibrationFlag = 0;
	}else if(Valid_data[8] == 0x02){
		MotorPresetParameters.PresetNum = Valid_data[9];
		FocusParameters.PresetPos = MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[15];
		ZoomParameters.PresetPos = MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[15];
		MotorPresetParameters.PresetSwitch = ON_State;
	}else if(Valid_data[8] == 0x03){
		Data_u8_Limit(&Valid_data[9],1,0);
		if(Valid_data[9] == 0){
			MotorPresetParameters.FollowSwitch = OFF_State;
		}else{
			MotorPresetParameters.FollowSwitch = ON_State;
		}
	}else if(Valid_data[8] == 0x04){
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x0A;

		if(Valid_data[9]== 0x01) //放大
		{
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,5);
			delay_1ms(30);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,5);
			delay_1ms(30);
	}
			MotorPresetParameters.ZoomPresetSpeed = Valid_data[10];
			MotorPresetParameters.FocusPresetSpeed = Valid_data[11];//5
			// MotorPresetParameters.FocusPresetSpeed = SystemParaMenu2.OperatorSpeed;
			MotorPresetParameters.ZoomOperateFlag = 1;
			MotorPresetParameters.FollowFlag = 1;
			ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomForward,2);
		}
		else if(Valid_data[9]== 0x02) //缩小
		{
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
			delay_1ms(30);
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
			delay_1ms(30);
	}
			MotorPresetParameters.ZoomPresetSpeed = Valid_data[10];
			MotorPresetParameters.FocusPresetSpeed = Valid_data[11]; //5
			// MotorPresetParameters.FocusPresetSpeed = SystemParaMenu2.OperatorSpeed;
			MotorPresetParameters.ZoomOperateFlag = 1;
			MotorPresetParameters.FollowFlag = 1;
			ZoomMotorOperate(NormalState,MotorPresetParameters.ZoomPresetSpeed,MeasureParameters.zoomReversal,2);
		}
		else if(Valid_data[9]== 0x03) //停止
		{
			ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomReversal,5);
			delay_1ms(50);
			MotorPresetParameters.ZoomOperateFlag = 0;
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
			MotorPresetParameters.FollowFlag = 1;
			// FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,5);
			// delay_1ms(30);
		}
		}
		test = GetVol(Zoom_MCP3221_addr);
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = test>>8;
		send_data.rx_buff[10]= test>>0;
		test = GetVol(Focus_MCP3221_addr);
		send_data.rx_buff[11] = test>>8;
		send_data.rx_buff[12] = test>>0;
		}
	else if(Valid_data[8] == 0x05){   //查询调焦、变倍位置信息
	#if 0
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x0B;

		Data_u8_Limit(&Valid_data[9],1,0);
		if(Valid_data[9] == 0) {
			test = GetVol(Focus_MCP3221_addr);
			send_data.rx_buff[10] = test>>24;
			send_data.rx_buff[11] = test>>16;
			send_data.rx_buff[12] = test>>8;
			send_data.rx_buff[13] = test>>0;
		} else {
			test = GetVol(Zoom_MCP3221_addr);
			send_data.rx_buff[10] = test>>24;
			send_data.rx_buff[11] = test>>16;
			send_data.rx_buff[12] = test>>8;
			send_data.rx_buff[13] = test>>0;
		}
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];
	#else
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x09;

		Data_u8_Limit(&Valid_data[9],1,0);
		if(Valid_data[9] == 0) {
			test = GetVol(Focus_MCP3221_addr);
			send_data.rx_buff[10] = test>>8;
			send_data.rx_buff[11] = test>>0;
		} else {
			test = GetVol(Zoom_MCP3221_addr);
			send_data.rx_buff[10] = test>>8;
			send_data.rx_buff[11]= test>>0;
		}
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];
	#endif
//////////////////
		// for(u8 i = 0; i<8 ;i++){
		// 	ImageVal[i] = AFS_EvaluationFuc_Cal();
		// }
		// BubbleSort(ImageVal,8);
	// ImageVal[4] = AFS_EvaluationFuc_Cal();
	// vision_log_debug("------ImageVal[4] = [%d]\n", ImageVal[4]);

	}
	// else if(Valid_data[8] == 0x06){			//设置跟焦标定变倍微调速度
	// 	SystemParaMenu2.Calibra_ZoomSpeed = Valid_data[9];
	// 	if(SystemParaMenu2.Calibra_ZoomSpeed >48)	SystemParaMenu2.Calibra_ZoomSpeed = 48;
	// 	send_data.rx_buff[8] = Valid_data[8];
	// 	send_data.rx_buff[9] = SystemParaMenu2.Calibra_ZoomSpeed;
	// }
	else if(Valid_data[8] == 0x07){		 //回读跟焦标定表数据
		u8 CalFlag;
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x84;

		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = MotorPresetParameters.PresetNum; //当前标定表标号
		CalFlag = Valid_data[9];
		send_data.rx_buff[10] = CalFlag;	//回读的标定表标号
		for(u8 i=0,j=11;i<MOTORPOS_NUM+1;i++,j+=4){
			send_data.rx_buff[j] = MotorPresetParemeters[CalFlag].FocusMotorPos[i]>>8;
			send_data.rx_buff[j+1] = MotorPresetParemeters[CalFlag].FocusMotorPos[i];
			send_data.rx_buff[j+2] = MotorPresetParemeters[CalFlag].ZoomMotorPos[i]>>8;
			send_data.rx_buff[j+3] = MotorPresetParemeters[CalFlag].ZoomMotorPos[i];
			vision_log_notice("--Motorgroup[%d].Num[%d] = F[0x%04x]Z[0x%04x]--\n", CalFlag,i,MotorPresetParemeters[CalFlag].FocusMotorPos[i],MotorPresetParemeters[CalFlag].ZoomMotorPos[i]);
		}
	}else if(Valid_data[8] == 0x08){			//回读量程
		if(Valid_data[9] == 0x01)
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x0E;

			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = MeasureParameters.LimitStepMin>>8;
			send_data.rx_buff[10] = MeasureParameters.LimitStepMin>>0;
			send_data.rx_buff[11] = MeasureParameters.LimitStepMax>>8;
			send_data.rx_buff[12] = MeasureParameters.LimitStepMax>>0;

			send_data.rx_buff[13] = MeasureParameters.LimitZoomMin>>8;
			send_data.rx_buff[14] = MeasureParameters.LimitZoomMin>>0;
			send_data.rx_buff[15] = MeasureParameters.LimitZoomMax>>8;
			send_data.rx_buff[16] = MeasureParameters.LimitZoomMax>>0;
		}
	}else if(Valid_data[8] == 0x09)  //手动修改跟焦标定表
	{
		u8 Num,GroupNum;
		GroupNum = Valid_data[9];
		if(GroupNum >10)	GroupNum = 10;
		Num =  Valid_data[10];
		if(Num > 31)		Num = 31;
		MotorPresetParemeters[GroupNum].FocusMotorPos[Num] = Valid_data[12]|Valid_data[11]<<8;
		MotorPresetParemeters[GroupNum].ZoomMotorPos[Num] = Valid_data[14]|Valid_data[13]<<8;
		Write_MotorParameterToFlash();

		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x0C;
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Valid_data[9];
		send_data.rx_buff[10] = Valid_data[10];

		send_data.rx_buff[11] = MotorPresetParemeters[GroupNum].FocusMotorPos[Num]>>8;
		send_data.rx_buff[12] = MotorPresetParemeters[GroupNum].FocusMotorPos[Num];
		send_data.rx_buff[13] = MotorPresetParemeters[GroupNum].ZoomMotorPos[Num]>>8;
		send_data.rx_buff[14] = MotorPresetParemeters[GroupNum].ZoomMotorPos[Num];
	}
	else if(Valid_data[8] == 0x0A)  //切换多个跟焦标定表与单个标定表
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;

		SystemParaMenu2.MotorSenFlag = Valid_data[9]&0x01;
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = SystemParaMenu2.MotorSenFlag;
	}else if(Valid_data[8] == 0x0B)  //获取测温小板Sensor温度值
	{
		float Sens_Val;
		s16 SensVal;
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x09;
		send_data.rx_buff[8] = Valid_data[8];

		Sens_Val = GetSensor()*10;
		if(Sens_Val< 0)
		{
			send_data.rx_buff[9] = 0x01;
			SensVal = (s16)(Sens_Val*(-1));
		}
		else
		{
			send_data.rx_buff[9] = 0x00;
			SensVal = (s16)(Sens_Val);
		}
		send_data.rx_buff[10] = SensVal>>8;
		send_data.rx_buff[11] = SensVal>>0;
	}else if(Valid_data[8] == 0x0C)  //自动标定表切换和手动切换标定表
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x08;
		send_data.rx_buff[8] = Valid_data[8];

		if(Valid_data[9] == 0x00){
			FocusTable_AorM = 0; //0-手动切换  1-自动切换
			MotorPresetParameters.PresetNum = Valid_data[10];
		}else if(Valid_data[9] == 0x01)
		{
			FocusTable_AorM = 1; //0-手动切换  1-自动切换
		}
		send_data.rx_buff[9] = FocusTable_AorM;
		send_data.rx_buff[10] = MotorPresetParameters.PresetNum;
	}
	else{
		MessageReturnState(CommandNotSupported);
		return;}
}

//温漂补偿
void UART_OPR_Temp_Drift(u8 *Valid_data){

	uint8_t tmpFunction,tmIndex,tmLastMode;

	tmpFunction = Valid_data[8];
	tmIndex = Valid_data[9];
	tmLastMode = SystemParaMenu2.m_temp_drift_comp_mode;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = tmpFunction;
	send_data.rx_buff[9] = tmIndex;
	
	switch(tmpFunction)
	{
		case 0:
			switch(tmIndex)
			{
				case 0:
					UpdatePromptInfoWithTimeMs(" please restart system for temp drift comp !", 1000,VOT_LOG_TYPE_MENU2);
					vot_set_temp_drift_calibration_mode(TEMP_DRIFT_COMPENSATION_MODE_OFF,0);
					break;//关闭自动补偿
				case 1:
					UpdatePromptInfoWithTimeMs(" please restart system for temp drift comp !", 1000,VOT_LOG_TYPE_MENU2);
					vot_set_temp_drift_calibration_mode(TEMP_DRIFT_COMPENSATION_MODE_ON,0);
					break;//开启自动补偿
				case 2:
					vot_set_temp_drift_calibration_mode(TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION,0);
					break;//切换为标定模式
				default:
					UpdatePromptInfoWithTimeMs(" invalid mode !", 2000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = 0xEE;
					return;
			}
			SystemParaMenu2.m_temp_drift_comp_mode = tmIndex;//更新状态
			if(tmLastMode != SystemParaMenu2.m_temp_drift_comp_mode){
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
			}	
			break;//模式选择
		case 1:
			if(SystemParaMenu2.m_temp_drift_comp_mode!=TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){				
				UpdatePromptInfoWithTimeMs("The mode is not support !", 2000,VOT_LOG_TYPE_MENU2);
				send_data.rx_buff[9] = 0xEE;
				return;
				}
			if(tmIndex >= 0 && tmIndex < 9){
				//return ts_tdk_snr_write_bcfg_hdr(FN_B_CFG, s32_oft1, s32_flag, s32_temp)
				vot_read_det_ad();//读取det温度值,并转换为温度值存入global_sys_ctrl_info.m_det_temp_value全局变量中
				SystemParaMenu2.m_temp_drift_temp[tmIndex] = 10*global_sys_ctrl_info.m_det_temp_value;
				SystemParaMenu2.m_temp_drift_index &= ~(1<<tmIndex);
				global_sys_ctrl_info.m_cali_index_offset = tmIndex;
				ISP_set_TDC_node(global_sys_ctrl_info.m_cali_index_offset);
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
			}
			else{
				UpdatePromptInfoWithTimeMs(" invalid index !", 2000,VOT_LOG_TYPE_MENU2);
				send_data.rx_buff[9] = 0xEE;
				return;
			}
			break;//补偿标定
		case 2:	
		   send_data.rx_buff[4] = 0x1A;		   
           send_data.rx_buff[9] = SystemParaMenu2.m_temp_drift_index & 0xff;
		   send_data.rx_buff[10] = (SystemParaMenu2.m_temp_drift_index >>8) & 0xff;
		   for(int8_t i=0;i<9;i++){
			for(int8_t j=2;j>0;j--){
				send_data.rx_buff[13+i*2-j] = (SystemParaMenu2.m_temp_drift_temp[i]>>(j-1)*8)&0xff;
			}
		   }	
			break;//信息查询
		case 3:
			if(tmIndex >= 0 && tmIndex < 9){
				SystemParaMenu2.m_temp_drift_temp[tmIndex] = 0xffff;
		   		SystemParaMenu2.m_temp_drift_index |= (1<<tmIndex);				
				ts_tdk_snr_write_bcfg_hdr(FN_B_CFG, tmIndex, 0, 0);
			}
		    else if((SystemParaMenu2.m_temp_drift_index!=0xffff)&&(tmIndex == 0xEE)){
				 memset(SystemParaMenu2.m_temp_drift_temp,0xff,sizeof(SystemParaMenu2.m_temp_drift_temp));
		   		 SystemParaMenu2.m_temp_drift_index = ~(SystemParaMenu2.m_temp_drift_index & 0);
				 for(int8_t i=0;i<9;i++){
 					ts_tdk_snr_write_bcfg_hdr(FN_B_CFG, i, 0, 0);
				 }
				 send_data.rx_buff[9] = 0x09;											  	 
		    }
		    else{
				send_data.rx_buff[9] = 0xEE;
		    }
			
		    ISP_write_sys_menu2_cfg(SystemParaMenu2);		   
			break;//清除所有已保存的索引，以及温度（SOC层面，实际使用数据任然保存，用于PC上操作）
		default:
			MessageReturnState(CommandNotSupported);
			return;
	}

}

/* 温度 → 16进制数据包 */
static int16_t Temp_Convert_ToHex(float temp) {
    int16_t value;

    /* 错误处理：原函数返回 0x8000 */
    if (*(uint16_t*)&temp == 0x8000) {
        value = -32768;  // 0x8000 表示错误
    }
    else {
        /* 限幅保护 */
        if (temp < -50.0f) temp = -50.0f;
        if (temp > 500.0f) temp = 500.0f;

        /* 转换为 (T+100)*10，四舍五入 */
        value = (int16_t)((temp + 100.0f) * 10.0f + 0.5f);
    }

    /* 小端格式 */
    return value;
}

/** @note 采集模式：支持两种模式采集单点采集和连续录制 
 * 56 4F 54 00 06 00 AC 04 01 C7 07
*/
void UART_OPR_TemplutCollect_Data(u8 *Valid_data) {
	int16_t tmpAD;
	ts_f32 vot_Temperature;
	ts_u16 temp_ad_value_u16 = 0;
	ts_f32 temp_value_f32 = 0;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x12;
	send_data.rx_buff[8] = 0x01;

	if (Valid_data[8] == 0x01) {
		/* 打快门标志位 */
		send_data.rx_buff[13] = (uint8_t)((global_sys_ctrl_info.Iris_Current_State >> 8) & 0xFF);
		send_data.rx_buff[14] = (uint8_t)(global_sys_ctrl_info.Iris_Current_State & 0xFF);
		/* FIRE模块读取 */
		vot_hst_update_attr_safe();
		send_data.rx_buff[9] = (uint8_t)((global_sys_ctrl_info.m_uart_spot1_raw_ad >> 8) & 0xFF);
		send_data.rx_buff[10] = (uint8_t)(global_sys_ctrl_info.m_uart_spot1_raw_ad & 0xFF);
		vision_log_debug("Iris_Current_State = %d, uart_spot1_raw_ad = 0x%x \n", global_sys_ctrl_info.Iris_Current_State, global_sys_ctrl_info.m_uart_spot1_raw_ad);
		/* 快门AD */
		send_data.rx_buff[11] = (uint8_t)((global_sys_ctrl_info.m_cur_bak_ad_mean_u32 >> 8) & 0xFF);
		send_data.rx_buff[12] = (uint8_t)(global_sys_ctrl_info.m_cur_bak_ad_mean_u32 & 0xFF);
		/* 开机快门温度 */
		tmpAD = Temp_Convert_ToHex(global_sys_ctrl_info.votShutter_temp_at_powerup);
		send_data.rx_buff[15] = (uint8_t)((tmpAD >> 8) & 0xFF);
		send_data.rx_buff[16] = (uint8_t)(tmpAD & 0xFF);
		/* 实时快门温度 */
		vot_Temperature = READ_TMP112_Temp();
		tmpAD = Temp_Convert_ToHex(vot_Temperature);
		send_data.rx_buff[17] = (uint8_t)((tmpAD >> 8) & 0xFF);
		send_data.rx_buff[18] = (uint8_t)(tmpAD & 0xFF);
		/* 实时焦温 */
		ISP_get_det_ad_info(&temp_value_f32, &temp_ad_value_u16);
		tmpAD = Temp_Convert_ToHex(temp_value_f32);
		send_data.rx_buff[19] = (uint8_t)((tmpAD >> 8) & 0xFF);
		send_data.rx_buff[20] = (uint8_t)(tmpAD & 0xFF);
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_TemCalibration_T_Set(u8 *Valid_data)
{
	ts_u08  i,j;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	j = 0;
	for(i=0;i<12;i++)
	{
		TEMP_data[i]  =  Valid_data[j+8]<<8;
		TEMP_data[i] |=  Valid_data[j+9]<<0;
		if(TEMP_data[i]>>15)
		{
			TEMP_data[i] &= ~(1<<15);
			TEMP_data[i] = 0 -TEMP_data[i];
		}
		j+=2;
		if(SystemParaMenu2.Usermenu_SW == 1)			Temp_Parameter.TEMP_data1[i] = TEMP_data[i];
		else if(SystemParaMenu2.Usermenu_SW == 2)		Temp_Parameter.TEMP_data2[i] = TEMP_data[i];
		else if(SystemParaMenu2.Usermenu_SW == 3)		Temp_Parameter.TEMP_data3[i] = TEMP_data[i];
	}

	send_data.rx_buff[8] = 0x01;
	Write_TempParameter_cfg(&Temp_Parameter);
	global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag = 1;
	UpdatePromptInfoWithTimeMs("Cal_Tem save completed",2000,VOT_LOG_TYPE_MENU2);
}

void UART_OPR_TemCalibration_AD_Set(u8 *Valid_data)
{
	ts_u08  i,j;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	j = 0;
	for(i=0;i<12;i++)
	{
		AD_data[i]  = Valid_data[j+8]<<8;
		AD_data[i] |= Valid_data[j+9]<<0;
		j+=2;

		if(SystemParaMenu2.Usermenu_SW == 1)			Temp_Parameter.AD_data1[i] = AD_data[i];
		else if(SystemParaMenu2.Usermenu_SW == 2)		Temp_Parameter.AD_data2[i] = AD_data[i];
		else if(SystemParaMenu2.Usermenu_SW == 3)		Temp_Parameter.AD_data3[i] = AD_data[i];
	}
		send_data.rx_buff[8] = 0x01;
	Write_TempParameter_cfg(&Temp_Parameter);
	global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag = 1;
	UpdatePromptInfoWithTimeMs("Cal_AD save completed",2000,VOT_LOG_TYPE_MENU2);
}
/** @note PC上位机回读测温功能标定表 */
void UART_OPR_TemCalibration_Read(u8 *Valid_data)
{
	uint8_t  i,j;
	int16_t  Temp;

	if(Valid_data[8] == 0x01)
	{
		if(SystemParaMenu2.Usermenu_SW==1)
		{
			for(i=0;i<12;i++)
			{
				TEMP_data[i] = Temp_Parameter.TEMP_data1[i];
				AD_data[i] = Temp_Parameter.AD_data1[i];
			}
		}
		else if(SystemParaMenu2.Usermenu_SW==2)
		{
			for(i=0;i<12;i++)
			{
				TEMP_data[i] = Temp_Parameter.TEMP_data2[i];
				AD_data[i] = Temp_Parameter.AD_data2[i];
			}
		}
		else if(SystemParaMenu2.Usermenu_SW==3)
		{
			for(i=0;i<12;i++)
			{
				TEMP_data[i] = Temp_Parameter.TEMP_data3[i];
				AD_data[i] = Temp_Parameter.AD_data3[i];
			}
		}else {MessageReturnState(CommandNotSupported);return;}
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x1E;
/************************************  发送温度数据  ******************************************************/
		send_data.rx_buff[8] = 0x01;  //发送温度数据
		for(i=9,j=0;i<33;i+=2)
		{
			Temp = TEMP_data[j];
			if(Temp>>15)
			{
				Temp -= 1;
				Temp = ~Temp;
				Temp = (1<<15) + Temp;
			}
			send_data.rx_buff[i+0] = Temp>>8;
			send_data.rx_buff[i+1] = Temp>>0;
			j++;
		}
		Return_Message_Processing();
/**************************************  发送AD数据  ****************************************************/
		send_data.rx_buff[8] = 0x02;   //发送AD数据
		for(i=9,j=0;i<33;i+=2)
		{
			send_data.rx_buff[i+0] = AD_data[j]>>8;
			send_data.rx_buff[i+1] = AD_data[j]>>0;
			j++;
		}
		Return_Message_Processing();
		memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Temp_correct_Set(u8 *Valid_data)
{
	uint16_t i;

	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x08;

	if(Valid_data[8] == 0x00) //第一档测温
	{
		if(SystemParaMenu2.Usermenu_SW==1)
		{
			Temp_Parameter.Temp_correct1  = Valid_data[9]<<8;
			Temp_Parameter.Temp_correct1 |= Valid_data[10]<<0;

			if(Temp_Parameter.Temp_correct1 & (1<<15))
			{
				i = Temp_Parameter.Temp_correct1 &(~(1<<15));
				Temp_Parameter.Temp_correct1 = 0-i;
			}
			send_data.rx_buff[8] = 0x00;
			send_data.rx_buff[9] = Valid_data[9];
			send_data.rx_buff[10] = Valid_data[10];
		}else {
			MessageReturnState(CommandNotSupported);return;}
	}
	else if(Valid_data[8] == 0x01)//第二档测温
	{
		if(SystemParaMenu2.Usermenu_SW==2)
		{
			Temp_Parameter.Temp_correct2  = Valid_data[9]<<8;
			Temp_Parameter.Temp_correct2 |= Valid_data[10]<<0;

			if(Temp_Parameter.Temp_correct2 & (1<<15))
			{
				i = Temp_Parameter.Temp_correct2 & (~(1<<15));
				Temp_Parameter.Temp_correct2 = 0-i;
			}
			send_data.rx_buff[8] = 0x01;
			send_data.rx_buff[9] = Valid_data[9];
			send_data.rx_buff[10] = Valid_data[10];
		}else {
			MessageReturnState(CommandNotSupported);return;}
	}
	else if(Valid_data[8] == 0x02)//第三档测温
	{
		if(SystemParaMenu2.Usermenu_SW==3)
		{
			Temp_Parameter.Temp_correct3  = Valid_data[9]<<8;
			Temp_Parameter.Temp_correct3 |= Valid_data[10]<<0;
			if(Temp_Parameter.Temp_correct3 & (1<<15))
			{
				i = Temp_Parameter.Temp_correct3 & (~(1<<15));
				Temp_Parameter.Temp_correct3 = 0-i;
			}
			send_data.rx_buff[8] = 0x02;
			send_data.rx_buff[9] = Valid_data[9];
			send_data.rx_buff[10] = Valid_data[10];
		}else {
			MessageReturnState(CommandNotSupported);return;}
	}
	else{
		MessageReturnState(CommandNotSupported);
		return;
	}
	Write_TempParameter_cfg(&Temp_Parameter);
}

void UART_OPR_Temp_correct_Read(u8 *Valid_data)
{
	uint16_t i;
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x08;

	if(Valid_data[8] == 0x01)
	{
		if(SystemParaMenu2.Usermenu_SW==1)
		{
			if(Temp_Parameter.Temp_correct1 & (1<<15))
			{
				i = ~(Temp_Parameter.Temp_correct1-1);
				i = i|(1<<15);
			}
			else		{i = Temp_Parameter.Temp_correct1;}
			send_data.rx_buff[8] = 0x01;
			send_data.rx_buff[9] = i>>8;
			send_data.rx_buff[10] = i>>0;
		}
		else if(SystemParaMenu2.Usermenu_SW==2)
		{
			if(Temp_Parameter.Temp_correct2 & (1<<15))
			{
				i = ~(Temp_Parameter.Temp_correct2-1);
				i = i|(1<<15);
			}
			else		{i = Temp_Parameter.Temp_correct2;}
			send_data.rx_buff[8] = 0x02;
			send_data.rx_buff[9] = i>>8;
			send_data.rx_buff[10] = i>>0;
		}
		else if(SystemParaMenu2.Usermenu_SW==3)
		{
			if(Temp_Parameter.Temp_correct3 & (1<<15))
			{
				i = ~(Temp_Parameter.Temp_correct3-1);
				i = i|(1<<15);
			}
			else		{i = Temp_Parameter.Temp_correct3;}
			send_data.rx_buff[8] = 0x03;
			send_data.rx_buff[9] = i>>8;
			send_data.rx_buff[10] = i>>0;
		}
	}
	else{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void UART_OPR_Set_TemptestCollect(u8 *Valid_data)
{
	// VotParameters.Test_Flag = Valid_data[8];
	// send_data.rx_buff[3] = 0x00;//返回信息长度
	// send_data.rx_buff[4] = 0x06;
	// send_data.rx_buff[8] = VotParameters.Test_Flag;

	// /** 这里的标定有效值和AD_T有效范围值均有修改，请核对 */
	// if(VotParameters.Test_Flag)
	// {
	// 	if(start_temperature_collection_thread() == 0)
	// 	{
	// 		UpdatePromptInfo("TempCollecting...", 1,VOT_LOG_TYPE_MENU2);
	// 		SystemParaMenu2.m_temperCalibra_Flag = 1;
	// 		SystemParaMenu2.AD_T = 1;
	// 	}else{
	// 		UpdatePromptInfoWithTimeMs("TempCollect Failed",1500,VOT_LOG_TYPE_MENU2);
	// 	}
	// }
	// else {
	// 	if(stop_temperature_collection_thread() == 0) {
	// 		UpdatePromptInfoWithTimeMs("TempCollect completed",1500,VOT_LOG_TYPE_MENU2);
	// 		SystemParaMenu2.m_temperCalibra_Flag = 0;
	// 		SystemParaMenu2.AD_T = 0;
	// 	}
	// 	else {
	// 		UpdatePromptInfoWithTimeMs("TempCollect Failed",1500,VOT_LOG_TYPE_MENU2);
	// 	}
	// }

	uint16_t Data_16bit;
	uint16_t Temp_int,Temp_point;
	uint16_t tmp_ad_Avg;
	ts_isp_fire_statis_s_t Fire_statis_ad;

	if(Valid_data[8] == 0x01) //开始采集
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x06;
		UpdatePromptInfo("TempCollecting...", 1,VOT_LOG_TYPE_MENU2);
		VotParameters.Test_Flag = 1;
		SystemParaMenu2.m_temperCalibra_Flag = 0;
		SystemParaMenu2.AD_T = 1;
		send_data.rx_buff[8] = 0x01;
	}else if(Valid_data[8] == 0x03) //停止采集
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x06;
		UpdatePromptInfo("TempCollecting...", 0,VOT_LOG_TYPE_MENU2);
		UpdatePromptInfoWithTimeMs("TempCollect completed",1500,VOT_LOG_TYPE_MENU2);
		VotParameters.Test_Flag = 0;
		SystemParaMenu2.m_temperCalibra_Flag = 0;
		SystemParaMenu2.AD_T = 2;
		send_data.rx_buff[8] = 0x01;
	}else if(Valid_data[8] == 0x02) //获取采集结果
	{
		if((SystemParaMenu2.m_temperCalibra_Flag != 0)||(SystemParaMenu2.AD_T != 1)){
			MessageReturnState(CommandNotSupported);
			return;
		}
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x16;

		ISP_cut();

/*1、获取DET相关参数的值*/
		vot_read_det_ad();
		Data_16bit = global_sys_ctrl_info.m_det_temp_ad_value; //DET的值-AD值
		// temp_value = global_sys_ctrl_info.m_det_temp_value;
		send_data.rx_buff[8] = Data_16bit>>8;
		send_data.rx_buff[9] = Data_16bit;

		send_data.rx_buff[10] = SystemParaMenu2.GAIN &0x7f; //7bit
		send_data.rx_buff[11] = SystemParaMenu2.GOF &0x7f;  //7bit
		send_data.rx_buff[12] = SystemParaMenu2.OOC_Par &0x3f;  //6bit
		send_data.rx_buff[13]  = SystemParaMenu2.INT_C &0x07; //3bit
		send_data.rx_buff[14] = SystemParaMenu2.INT_T &0x7f;	//7bit			GAIN_IMAGE
/*2、获取Sensor温度值*/
		ts_f32 TEM_Sensor = READ_TMP112_Temp();
		if(TEM_Sensor<0)
		{
			Temp_int= (u16) abs((int)TEM_Sensor);
			Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
			Temp_int = Temp_int |0x8000;

		}
		else
		{
			Temp_int = (u16)TEM_Sensor;
			Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
			Temp_int = Temp_int &0x7fff;
		}
		send_data.rx_buff[15]  = Temp_int>>8;	 //整数
		send_data.rx_buff[16]  = Temp_int>>0;
		send_data.rx_buff[17]  = Temp_point>>8;	 //小数
		send_data.rx_buff[18]  = Temp_point;	 //
/*3、获取平均AD值*/
		vot_get_fire_statis(&Fire_statis_ad);
		ts_u16 ad_avg = Fire_statis_ad.st_frm_info.u16_mean;

		send_data.rx_buff[19] = ad_avg>>8;
		send_data.rx_buff[20] = ad_avg;

/*4、获取2个区域的参数*/
		ts_f32 Ret = TZ_GetTempFromMem(&Areainit[0]);
		//AD14bit = 	(FPGA_Area1_sum/((mPostPara.Area1_Rx-mPostPara.Area1_Lx)*(mPostPara.Area1_Ry-mPostPara.Area1_Ly)));
		if(Ret == TS_ERR){
			send_data.rx_buff[21]  = 0xFF;
			send_data.rx_buff[22]  = 0xFF;
		}else{
			tmp_ad_Avg = Areainit[0].Area_temp_AVG + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			send_data.rx_buff[21]  = tmp_ad_Avg>>8;
			send_data.rx_buff[22]  = tmp_ad_Avg;
		}

		Ret = TZ_GetTempFromMem(&Areainit[1]);
		// Area_Display_Process(&Areainit[1]);
		// AD14bit = Temp/((Areainit[1].Area_Rx - Areainit[1].Area_Lx)*(Areainit[1].Area_Ry-Areainit[1].Area_Ly));
		if(Ret == TS_ERR){
			send_data.rx_buff[21]  = 0xFF;
			send_data.rx_buff[22]  = 0xFF;
		}else{
			tmp_ad_Avg = Areainit[1].Area_temp_AVG + 0x1000 - global_sys_ctrl_info.m_cur_bak_ad_mean_u32;
			send_data.rx_buff[23]  = tmp_ad_Avg>>8;
			send_data.rx_buff[24]  = tmp_ad_Avg;
		}
	}else{
		MessageReturnState(CommandNotSupported);
		return;
	}

}
/** @note 串口协议之测温挡位切换 
 * 56 4F 54 00 06 00 AC 26 01 67 1F
 * 56 4F 54 00 06 00 AC 26 02 66 5F
 * 56 4F 54 00 06 00 AC 26 03 A6 9E
 * 56 4F 54 00 06 00 AC 26 05 A4 1E
 * 规则：
 * 1. OFF(0) 状态不允许切换，需先通过菜单二开启测温
 * 2. 只能在 1/2/3 挡之间切换，且受 m_templutMode_Limit 限制
 * 3. 当前挡位与目标挡位相同时，不执行切换，但串口正常返回当前状态
*/
void UART_OPR_Temp_Meas_Range(u8 *Valid_data)
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;

	if(Valid_data[8] < 0x04 && Valid_data[8] > 0) {
		u8 target_sw = Valid_data[8];

        if (SystemParaMenu2.m_templutMode_Limit < 1 || SystemParaMenu2.m_templutMode_Limit > 3) {
            SystemParaMenu2.m_templutMode_Limit = 1;
        }
		//
		if (SystemParaMenu2.m_templut_target_gear != 0xFF) {
			vision_log_debug("Blocked: target=%d pending, reboot required\n", SystemParaMenu2.m_templut_target_gear);
			UpdatePromptInfoWithTimeMs("Please power off/on...", 1000, VOT_LOG_TYPE_MENU2);
			MessageReturnState(CommandNotSupported);
			return;
		}
        if (SystemParaMenu2.Usermenu_SW == 0) {
			vision_log_debug("UART: current is OFF (Usermenu_SW=0)\n");
			// OFF状态下，只允许切换到1挡
			if (target_sw == 0x01) {
				vision_log_debug("UART: allow switch to gear 1 from OFF state\n");
				SystemParaMenu2.m_templut_target_gear = 1;
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
				UpdatePromptInfoWithTimeMs("Please power off/on...", 1000, VOT_LOG_TYPE_MENU2);
				send_data.rx_buff[8] = SystemParaMenu2.Usermenu_SW;
			}
			else {
				// OFF状态下，不允许切换到2或3挡
				vision_log_debug("UART: reject switch to gear %d from OFF state\n", target_sw);
				MessageReturnState(CommandNotSupported);
			}
            return;
        }
		// 目标挡位不能超过限制
        if (target_sw > SystemParaMenu2.m_templutMode_Limit) {
            vision_log_debug("UART: target=%d exceeds Limit=%d, rejected\n", target_sw, SystemParaMenu2.m_templutMode_Limit);
            MessageReturnState(CommandNotSupported);
            return;
        }
        if (SystemParaMenu2.Usermenu_SW == target_sw) {
            vision_log_debug("UART: already at gear %d, no switch needed\n", target_sw);
        } else {
            SystemParaMenu2.Usermenu_SW = target_sw;

            global_sys_ctrl_info.Usermenu_SW_Switching = 1;
            // ts_cpl_fix_msleep(60);
            UpdatePromptInfo("Temperature Change...", 1, VOT_LOG_TYPE_MENU2);
            global_sys_ctrl_info.Shifting_Gears_Flag = 1;
            TemLevel_convert();
            global_sys_ctrl_info.Shifting_Gears_Flag = 0;
			UpdatePromptInfo("Temperature Change...", 0, VOT_LOG_TYPE_MENU2);
            global_sys_ctrl_info.Usermenu_SW_Switching = 0;

            ISP_write_sys_menu2_cfg(SystemParaMenu2);
            if (SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG) {
                SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
                cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
                cvbs_osd_refresh_display(1);
            }
        }
        send_data.rx_buff[8] = SystemParaMenu2.Usermenu_SW;
	}
	else if(Valid_data[8] == 0x05) {
		send_data.rx_buff[8] = SystemParaMenu2.Usermenu_SW;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 控制温度矩阵输出原始AD/测温AD/查表温度 
 * 56 4F 54 00 06 00 AC 25 02 96 5F        //将测温AD作为输出
 * 56 4F 54 00 06 00 AC 25 03 56 9E
 * 56 4F 54 00 06 00 AC 25 04 94 DF
*/
void UART_OPR_UserTempFrame_Mode(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	
	if (Valid_data[8] < 0x04 && Valid_data[8] > 0) {
		if (Valid_data[8] == 0x01) {
			SystemParaMenu2.m_user_tempFrame_Mode = TEMP_NORMAL_DISP_ORIAD;
		}
		else if (Valid_data[8] == 0x02) {
			SystemParaMenu2.m_user_tempFrame_Mode = TEMP_NORMAL_DISP_LUT_AD;
		}
		else if (Valid_data[8] == 0x03) {
			SystemParaMenu2.m_user_tempFrame_Mode = TEMP_NORMAL_DISP_TEMP;
		}
		send_data.rx_buff[8] = SystemParaMenu2.m_user_tempFrame_Mode;
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
	}
	else if (Valid_data[8] == 0x04) {
		send_data.rx_buff[8] = SystemParaMenu2.m_user_tempFrame_Mode;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 串口协议之测温挡位量程限制 
 * 56 4F 54 00 06 00 AC 27 02 F6 5E		--设置二挡为最高挡位
*/
void UART_OPR_Set_TempGear_MaxLimit(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x06;
	
	if(Valid_data[8] < 0x04 && Valid_data[8] > 0) {
		if (Valid_data[8] == 0x01) {
			SystemParaMenu2.m_templutMode_Limit = 1;
		}
		else if (Valid_data[8] == 0x02) {
			SystemParaMenu2.m_templutMode_Limit = 2;
		}
		else if (Valid_data[8] == 0x03) {
			SystemParaMenu2.m_templutMode_Limit = 3;
		}

		send_data.rx_buff[8] = SystemParaMenu2.m_templutMode_Limit;
	}
	else if (Valid_data[8] == 0x04) {
		send_data.rx_buff[8] = SystemParaMenu2.m_templutMode_Limit;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 读写辐射率 
 * 56 4F 54 00 07 00 AC 2A 01 29 C4 26	--设置辐射率为0.41
*/
static void UART_ReadWrite_Emissivity(u8 *Valid_data) 
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if (Valid_data[9] == 0xA5) {	// 获取辐射率
		send_data.rx_buff[9] = Temp_Parameter.Emiss;
	}
	else if (Valid_data[9] >= 0 || Valid_data[9] <= 0x64) {	// 设置辐射率，范围：0-100
		Temp_Parameter.Emiss = Valid_data[9];
		send_data.rx_buff[9] = Temp_Parameter.Emiss;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;		
	}
}
/** @note 读写反射温度 
 * 56 4F 54 00 07 00 AC 2A 02 3C FB E7	--设置反射温度为0℃
 * 56 4F 54 00 07 00 AC 2A 02 A5 91 27	--查询
*/
static void UART_ReadWrite_ReflectTemp(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if (Valid_data[9] == 0xA5) {
		send_data.rx_buff[9] = Temp_Parameter.Trefl/10;
	}
	else if (Valid_data[9] >= 0 && Valid_data[9] <= 0xA0) {
		Temp_Parameter.Trefl = Valid_data[9]*10;
		send_data.rx_buff[9] = Temp_Parameter.Trefl/10;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;		
	}
}
/** @note 读写环境温度 
 * 56 4F 54 00 07 00 AC 2A 03 19 B0 27	--设置环境温度为-35℃
*/
static void UART_ReadWrite_AmbientTemp(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if (Valid_data[9] == 0xA5) {
		send_data.rx_buff[9] = Temp_Parameter.Tamb/10;
	}
	else if (Valid_data[9] >= 0 && Valid_data[9] <= 0xA0) {
		Temp_Parameter.Tamb = Valid_data[9]*10;
		send_data.rx_buff[9] = Temp_Parameter.Tamb/10;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 读取相对湿度 
 * 56 4F 54 00 07 00 AC 2A 04 52 B7 65	--设置相对湿度为82%
*/
static void UART_ReadWrite_RelativeHumidity(u8 *Valid_data) 
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if (Valid_data[9] == 0xA5) {
		// 获取相对湿度
		send_data.rx_buff[9] = Temp_Parameter.RelHum;
	}
	else if (Valid_data[9] >= 0 && Valid_data[9] <= 0x64) {	// 设置相对湿度，范围：0-100
		Temp_Parameter.RelHum = Valid_data[9];
		send_data.rx_buff[9] = Temp_Parameter.RelHum;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 读写距离，步进为1 
 * 56 4F 54 00 07 00 AC 2A 05 01 1A 24	--距离设置为1m
*/
static void UART_ReadWrite_Distance(u8 *Valid_data) 
{
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if(Valid_data[9] >= 0 && Valid_data[9] <= 0x64) {
		Temp_Parameter.distance = Valid_data[9]*10;
		send_data.rx_buff[9] = Temp_Parameter.distance/10;
	}
	else if (Valid_data[9] == 0xA5) {
		send_data.rx_buff[9] = Temp_Parameter.distance/10;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 设置设备的镜头毫米数
 * 56 4F 54 00 07 00 AC 2A 06 01 EA 24	--设置镜头为A_9.1m
*/
void UART_OPR_Write_LensParam(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if(Valid_data[9] >= 0 && Valid_data[9] < 15) {
		Temp_Parameter.Lens_Set = Valid_data[9];
		send_data.rx_buff[9] = Temp_Parameter.Lens_Set;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 获取设备的镜头毫米数 
 * 56 4F 54 00 07 00 AC 2A 07 A5 51 25	--获取当前镜头值
*/
void UART_OPR_Read_LensParam(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if(Valid_data[9] == 0xA5) {
		send_data.rx_buff[9] = Temp_Parameter.Lens_Set;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}
}
/** @note 获取/切换测温芯片地址
 * 56 4F 54 00 07 00 AC 2A 08 00 4A E1	--设置为JK
 * 56 4F 54 00 07 00 AC 2A 08 01 8A 20	--设置为GC
 * 56 4F 54 00 07 00 AC 2A 08 A5 31 21	--查询Sensor值
*/
static void UART_ReadWrite_TempSensor(u8 *Valid_data)
{
    send_data.rx_buff[3] = 0x00;
    send_data.rx_buff[4] = 0x07;
    send_data.rx_buff[8] = Valid_data[8];
    
    if (Valid_data[9] == 0x00 || Valid_data[9] == 0x01) {
        // 仅当新值与当前值不同时才更新并执行初始化
        if (SystemParaMenu2.sensor != Valid_data[9]) {
            SystemParaMenu2.sensor = Valid_data[9];
            Sensor_Init();
        }
        send_data.rx_buff[9] = SystemParaMenu2.sensor;
    }
    else if (Valid_data[9] == 0xA5) {
        send_data.rx_buff[9] = SystemParaMenu2.sensor;
    }
    else {
        MessageReturnState(CommandNotSupported);
        return;
    }
}
/** @note 获取/设置读写测温参数 */
void UART_OPR_ReadWriteTempParam(u8 *Valid_data) 
{
	switch(Valid_data[8]) {
		case 0x01: {	// 辐射率
			UART_ReadWrite_Emissivity(Valid_data);
			break;
		}
		case 0x02: {	// 反射温度
			UART_ReadWrite_ReflectTemp(Valid_data);
			break;
		}
		case 0x03: {	// 环境温度
			UART_ReadWrite_AmbientTemp(Valid_data);
			break;
		}
		case 0x04: {	// 相对湿度
			UART_ReadWrite_RelativeHumidity(Valid_data);
			break;
		}
		case 0x05: {	// 距离
			UART_ReadWrite_Distance(Valid_data);
			break;
		}
		case 0x06: {	// 镜头设置
			UART_OPR_Write_LensParam(Valid_data);
			break;
		}
		case 0x07: {	// 镜头获取
			UART_OPR_Read_LensParam(Valid_data);
			break;
		}
		case 0x08: {	// 测温芯片地址
			UART_ReadWrite_TempSensor(Valid_data);
			break;
		}
		default: {
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
}
/** @note 获取/设置UVC输出YUYV或UYVY格式 
 * 56 4F 54 00 07 00 AC 2B 01 00 DA B6
*/
static void UART_ReadWrite_UVC_Format(u8 *Valid_data) {
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x07;
	send_data.rx_buff[8] = Valid_data[8];

	if (Valid_data[9] == 0) {
		SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_YUYV;
		send_data.rx_buff[9] = SystemParaMenu2.m_uvc_output_format;
	}
	else if (Valid_data[9] == 1) {
		SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_UYVY;
		send_data.rx_buff[9] = SystemParaMenu2.m_uvc_output_format;
	}
	else if (Valid_data[9] == 0xA5) {
		send_data.rx_buff[9] = SystemParaMenu2.m_uvc_output_format;
	}
	else {
		MessageReturnState(CommandNotSupported);
		return;
	}	
}
/** @note 获取/设置硬件接口输出数据帧格式 */
void UART_OPR_OutputFormat_Ctrl(u8 *Valid_data) {
	switch(Valid_data[8]) {
		case 0x01: {
			UART_ReadWrite_UVC_Format(Valid_data);
			break;
		}
		default: {
			MessageReturnState(CommandNotSupported);
			return;			
		}
	}
}
/** @note 手持机上测温AD转换成原始AD 
 * 56 4F 54 00 08 00 AC 2C 01 0F 96 5C 20
*/
void UART_OPR_TEMP_AD_TO_RAW_AD(u8 *Valid_data) {
    send_data.rx_buff[3] = 0x00;
    send_data.rx_buff[4] = 0x08;
    send_data.rx_buff[8] = Valid_data[8];

    if (Valid_data[8] == 0x01) {
        /* 收到的 [9][10] 是测温AD */
        ts_s16 temp_ad = (ts_s16)(((ts_u16)Valid_data[9] << 8) | Valid_data[10]);

        /* offset 带符号：bit15=1 为负，bit15=0 为正 */
        ts_s16 offset = (ts_s16)(Temp_Parameter.AD_sensor & 0x7FFF);
        if (Temp_Parameter.AD_sensor & 0x8000)
            offset = -offset;

        ts_s16 bak_mean = (ts_s16)(global_sys_ctrl_info.m_cur_bak_ad_mean_u32 & 0x3FFF);
        ts_s32 raw_s32 = (ts_s32)temp_ad - 0x1000 - (ts_s32)offset + (ts_s32)bak_mean;

        /* 14bit 钳位（原始AD最大值 0x3FFF） */
        if (raw_s32 < 0)
            raw_s32 = 0;
        if (raw_s32 > 0x3FFF)
            raw_s32 = 0x3FFF;

        ts_u16 raw_ad = (ts_u16)raw_s32;

        /* 填充发送缓冲 */
        send_data.rx_buff[9]  = (ts_u08)((raw_ad >> 8) & 0xFF);
        send_data.rx_buff[10] = (ts_u08)(raw_ad & 0xFF);

        vision_log_debug("temp_ad=0x%x, offset=%x, bak_mean=0x%x, raw_ad=0x%x\n", temp_ad, offset, bak_mean, raw_ad);
    }
    else {
        MessageReturnState(CommandNotSupported);
        return;
    }
}
/** @note 获取测温点温度 
 * 56 4F 54 00 0B 00 AC 3C 01 00 00 61 00 37 85 B0
 * 56 4F 54 00 0B 00 AC 3C 01 01 01 61 01 80 5F CD
 * 56 4F 54 00 0B 00 AC 3C 01 03 00 61 00 36 45 35
 * 56 4F 54 00 07 00 AC 3C 01 04 1D 07	// 内部测温标定使用
*/
void UART_OPR_ReadPointTemp_Multi(u8 *Valid_data) {
    ts_u08 idx_raw;       // 上位机传入的索引（1~9）
    ts_u08 mode;          // 模式（00/01/03/04）
    ts_u16 spot_x, spot_y;// 坐标（小端序）
	ts_u16 max_width = 0, max_heigth = 0;
	ts_u32 start_id = global_sys_ctrl_info.m_sync_latest_frame_id;

    idx_raw = Valid_data[8];
    mode    = Valid_data[9];
	if (mode == 0x04) {
		spot_x = SpotInit[0].spot_X_Pos;
		spot_y = SpotInit[0].spot_Y_Pos;
	}
	else {
		spot_x  = (Valid_data[10] << 8) | Valid_data[11];
		spot_y  = (Valid_data[12] << 8) | Valid_data[13];
	}

    if (idx_raw < 1 || idx_raw > UTab6_Length) {
        MessageReturnState(CommandNotSupported); // 索引非法
        return;
    }
    if (mode != 0x00 && mode != 0x01 && mode != 0x03 && mode != 0x04) {
        MessageReturnState(CommandNotSupported); // 模式非法
        return;
    }
	v_getDector_DisplayResolution(&max_width, &max_heigth);
	if (spot_x > max_width -1 || spot_y > max_heigth -1) {
		MessageReturnState(CommandNotSupported); // 坐标非法
		return;
	}
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x08;
    /* 计算数组索引(将1~9转换为0~8) */
    ts_u08 idx = idx_raw - 1;

	DisPlayIcon(SpotInit[idx].spot_ID, SpotInit[idx].spot_X_Pos, SpotInit[idx].spot_Y_Pos, 0);
	Refresh_templutSpot_Captions("", &SpotInit[idx], 0);
	if (mode == 0x00) {
		Spot_state = Spot_state & (~(1 << idx));
		send_data.rx_buff[8] = idx_raw;	// 回传测温点索引
		send_data.rx_buff[9] = 0xFF;
		send_data.rx_buff[10] = 0xFF;

	if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
		Write_TempParameter_cfg(&Temp_Parameter);
	}

	}
	else if (mode == 0x01 || mode == 0x03) {
		if (mode == 0x01) {
			global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
			Spot_state = Spot_state & (~(1 << idx));
		}
		else {
			Spot_state |= (1 << idx);
			SpotInit[idx].spot_X_Pos = spot_x;
			SpotInit[idx].spot_Y_Pos = spot_y;
		}
		while (global_sys_ctrl_info.m_sync_latest_frame_id == start_id) {
			ts_cpl_msleep(5);
		}
		ts_u16 tmp_SpotAD = TP_GetTempFromMem(spot_x, spot_y);
		if (tmp_SpotAD == 0xFFFF) {
			MessageReturnState(CommandNotSupported);
			return;
		}
		int16_t tmp_SpotTemp = UART_ConvertRawAdToTemp(tmp_SpotAD);
		send_data.rx_buff[8] = idx_raw; // 回传测温点索引
		send_data.rx_buff[9] = tmp_SpotTemp >> 8;
		send_data.rx_buff[10] = tmp_SpotTemp & 0xFF;
		vision_log_debug("Point %d ON(%d): X=%d, Y=%d, temp = %+.1f\n", idx_raw, mode, spot_x, spot_y, tmp_SpotTemp/10.0f);

		if(SystemParaMenu2.m_temp_regularity == TEMP_REGULARITY_QC){
		Write_TempParameter_cfg(&Temp_Parameter);
		}

	}
	else if (mode == 0x04) {	// 内部测温标定使用
		if (idx_raw == 1) {
			global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
			while (global_sys_ctrl_info.m_sync_latest_frame_id == start_id) {
				ts_cpl_msleep(5);
			}
			ts_u16 tmp_SpotAD = TP_GetTempFromMem(spot_x, spot_y);
			if (tmp_SpotAD == 0xFFFF) {
				MessageReturnState(CommandNotSupported);
				return;
			}
			ts_u16 tmp_ad;
			if ((SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) || ((SystemParaMenu2.m_temperCalibra_Flag == 1) && (SystemParaMenu2.AD_T == 3))) 
			{
				tmp_ad = UART_ConvertRawAd_To_CalibAd(tmp_SpotAD);
			}
			else {
				tmp_ad = UART_ConvertRawAd_To_tempAd(tmp_SpotAD);
			}
			send_data.rx_buff[8] = idx_raw; // 回传测温点索引
			send_data.rx_buff[9] = tmp_ad >> 8;
			send_data.rx_buff[10] = tmp_ad & 0xFF;
			vision_log_debug("Point %d ON(%d): X=%d, Y=%d, tempAd = 0x%x\n", idx_raw, mode, spot_x, spot_y, tmp_ad);
		}
		else {
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
    else {
        MessageReturnState(CommandNotSupported);
        return;
    }	
}
/** @note 读取测温区域温度 
 * 56 4F 54 00 0F 00 AC 3D 01 00 01 5E 01 2C 01 7C 01 4A 4E 1D		关区域1
 * 56 4F 54 00 0F 00 AC 3D 01 01 01 5E 01 2C 01 7C 01 4A DE 10		关区域1,获取温度值
 * 56 4F 54 00 0F 00 AC 3D 01 03 01 5E 01 2C 01 7C 01 4A BE 09		开区域1,获取温度值
 * 56 4F 54 00 0F 00 AC 3D 02 01 00 5E 01 0C 00 7C 01 2A 0E A1		关区域2,获取温度值
 * 56 4F 54 00 0F 00 AC 3D 02 03 00 5E 01 0C 00 7C 01 2A 6E B8		开区域2,获取温度值
 * 56 4F 54 00 0F 00 AC 3D 03 00 01 3E 01 2C 01 6C 01 4A 47 DD		关区域3
 * 56 4F 54 00 0F 00 AC 3D 03 03 01 3E 01 2C 01 6C 01 4A B7 C9		开区域3,获取温度值
*/
void UART_OPR_ReadAreaTemp_Multi(u8 *Valid_data) {
	ts_u16 max_width = 0, max_height = 0;
    ts_u08 area_index_raw = Valid_data[8];  // 区域索引(1~4)
	ts_u08 mode = Valid_data[9];  // 0x00=关闭, 0x01=开启不显示, 0x03=开启显示
    ts_u08 area_idx = area_index_raw - 1;   // 转换为数组索引(0~3)
	struct Area tmp_AreaValue = {0};
	ts_u32 start_id = global_sys_ctrl_info.m_sync_latest_frame_id;

    if (area_index_raw < 1 || area_index_raw > UTab7_Length) {
        MessageReturnState(CommandNotSupported);
        return;
    }
    send_data.rx_buff[3] = 0x00;
    send_data.rx_buff[4] = 0x0C;  // 数据长度固定为15字节
    /* 读取区域坐标（小端序） */
    tmp_AreaValue.Area_Lx = (Valid_data[10] << 8) | Valid_data[11];
    tmp_AreaValue.Area_Ly = (Valid_data[12] << 8) | Valid_data[13];
    tmp_AreaValue.Area_Rx = (Valid_data[14] << 8) | Valid_data[15];
    tmp_AreaValue.Area_Ry = (Valid_data[16] << 8) | Valid_data[17];	
	v_getDector_DisplayResolution(&max_width, &max_height);
    if (tmp_AreaValue.Area_Lx+30 > max_width || tmp_AreaValue.Area_Ly+30 > max_height || tmp_AreaValue.Area_Rx > max_width || 
		tmp_AreaValue.Area_Ry > max_height || tmp_AreaValue.Area_Lx >= tmp_AreaValue.Area_Rx || tmp_AreaValue.Area_Ly >= tmp_AreaValue.Area_Ry) {
        MessageReturnState(CommandNotSupported);
        return;
    }
	DisPlay_AREA_Icon(&Areainit[area_idx], 2, 0);
	Refresh_templut_AreaCaptions("", &Areainit[area_idx], 0);
	/* 区域控制逻辑 */
	if (mode == 0x00) {
		Area_state &= (~(1 << area_idx)) & 0x0F;
        send_data.rx_buff[8]  = 0xFF;  	// 区域索引
        send_data.rx_buff[9]  = 0xFF;  	// 最高温度
        send_data.rx_buff[10] = 0xFF;  
        send_data.rx_buff[11] = 0xFF;  	// 最低温度
        send_data.rx_buff[12] = 0xFF;
        send_data.rx_buff[13] = 0xFF;	// 平均温度
        send_data.rx_buff[14] = 0xFF;
	}
	else if (mode == 0x01 || mode == 0x03) {
		if (mode == 0x01) {
			global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
			Area_state &= (~(1 << area_idx)) & 0x0F;
		}
		else {
			Area_state|= (1 << area_idx)&0x0F;
			// 更新区域坐标
			Areainit[area_idx].Area_Lx = tmp_AreaValue.Area_Lx;
			Areainit[area_idx].Area_Ly = tmp_AreaValue.Area_Ly;
			Areainit[area_idx].Area_Rx = tmp_AreaValue.Area_Rx;
			Areainit[area_idx].Area_Ry = tmp_AreaValue.Area_Ry;
			DisPlay_AREA_Icon(&Areainit[area_idx], 2, 1);
		}
		while (global_sys_ctrl_info.m_sync_latest_frame_id == start_id) {
			ts_cpl_msleep(5);
		}
		ts_s08 cRet = TZ_GetTempFromMem(&tmp_AreaValue);
		if (cRet == TS_ERR) {
			send_data.rx_buff[8]  = 0xFE;  // 区域索引
			send_data.rx_buff[9]  = 0xFF;  // 最高温度
			send_data.rx_buff[10] = 0xFF;  // 最低温度
			send_data.rx_buff[11] = 0xFF;  // 平均温度
			send_data.rx_buff[12] = 0xFF;
			send_data.rx_buff[13] = 0xFF;
			send_data.rx_buff[14] = 0xFF;
			return;
		}
		int16_t max_temp = UART_ConvertRawAdToTemp(tmp_AreaValue.Area_temp_MAX);
		int16_t min_temp = UART_ConvertRawAdToTemp(tmp_AreaValue.Area_temp_MIN);
		int16_t avg_temp = UART_ConvertRawAdToTemp(tmp_AreaValue.Area_temp_AVG);
		// 填充返回数据
		send_data.rx_buff[8]  = area_index_raw;  // 透传区域索引
		send_data.rx_buff[9]  = (max_temp >> 8) & 0xFF;
		send_data.rx_buff[10] = max_temp & 0xFF;
		send_data.rx_buff[11] = (min_temp >> 8) & 0xFF;
		send_data.rx_buff[12] = min_temp & 0xFF;
		send_data.rx_buff[13] = (avg_temp >> 8) & 0xFF;
		send_data.rx_buff[14] = avg_temp & 0xFF;

		vision_log_debug("Area %d (idx=%d) ON(%d): LX=%d, LY=%d, RX=%d, RY=%d, MAX=%.1f, MIN=%.1f, AVG=%.1f\n", area_index_raw, area_idx, mode, 
			tmp_AreaValue.Area_Lx, tmp_AreaValue.Area_Ly, tmp_AreaValue.Area_Rx, tmp_AreaValue.Area_Ry, max_temp/10.0f, min_temp/10.0f, avg_temp/10.0f);
	}
    else {
        MessageReturnState(CommandNotSupported);
        return;
    }
}
/** @note 读取全屏最高和最低温度值 
 * 56 4F 54 00 06 00 AC 3E 00 A7 D4		// 全关
 * 56 4F 54 00 06 00 AC 3E 10 6B D5		// 全关，有最大值
 * 56 4F 54 00 06 00 AC 3E 30 B3 D4		// 显示最高光标，有最大值
 * 56 4F 54 00 06 00 AC 3E 01 67 15		// 全关，有最小值
 * 56 4F 54 00 06 00 AC 3E 11 AB 14		// 全关，有最大值/最小值
 * 56 4F 54 00 06 00 AC 3E 31 73 15		// 显示最高光标，有最大值/最小值
 * 56 4F 54 00 06 00 AC 3E 03 A6 94		// 显示最低光标，有最小值
 * 56 4F 54 00 06 00 AC 3E 13 6A 95		// 显示最低光标，有最大值/最小值
 * 56 4F 54 00 06 00 AC 3E 33 B2 94		// 显示最高/最低光标，有最大值/最小值
*/
void UART_OPR_ReadCaptureTemp_Multi(u8 *Valid_data) {
	struct Spot tmp_Spot_max = {0};
	struct Spot tmp_Spot_min = {0};
	int16_t max_temp = 0, min_temp = 0;
	ts_u32 start_id = global_sys_ctrl_info.m_sync_latest_frame_id;

	/********************************************/
	/*****高4位--最高温 低4位--最低温*************/ 
	/*****0x00-关闭测温（返回数据0xFF）**********/
	/*****0x01-开启测温但不显示光标*************/
	/*****0x03-开启测温并显示光标**************/
	/***************************************/
	/* 协议：高4位-最高温标志位(Bit1:开关, Bit0:光标), 低4位-最低温标志位 */
	ts_u08 max_flag = (Valid_data[8] >> 4) & 0x03;
	ts_u08 min_flag = Valid_data[8] & 0x03;
    /* ---------- 1. 非法参数检查 ---------- */
    if (!((max_flag == 0x00 || max_flag == 0x01 || max_flag == 0x03) &&
          (min_flag == 0x00 || min_flag == 0x01 || min_flag == 0x03))) {
        MessageReturnState(CommandNotSupported);
        return;
    }

	if (max_flag == 0x01) {
		global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
		if (min_flag == 0x00) {
			UsermenuParameter.CaptureFlag = 0x00;
		}
		else if (min_flag == 0x03) {
			UsermenuParameter.CaptureFlag = 0x01;
		}
		else {
			UsermenuParameter.CaptureFlag = 0x00;
		}
	}
	else if (max_flag == 0x03) {
		UsermenuParameter.CaptureFlag = 0x02;
		if (min_flag == 0x01){
			global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
		}
		else if (min_flag == 0x03) {
			UsermenuParameter.CaptureFlag = 0x03;
		}
	}
	else {
		UsermenuParameter.CaptureFlag = 0x00;
		if (min_flag == 0x01) {
			global_sys_ctrl_info.m_allowGetFrameStream_flag = 1;
		}
		else if (min_flag == 0x03) {
			UsermenuParameter.CaptureFlag = 0x01;
		}
	}
	send_data.rx_buff[3] = 0x00;
	send_data.rx_buff[4] = 0x11;

	while (global_sys_ctrl_info.m_sync_latest_frame_id == start_id) {
		ts_cpl_msleep(5);
	}
	CAP_GetTempFromMem(&tmp_Spot_max, &tmp_Spot_min);

	if (max_flag == 0x00) {
		// 关闭最高温显示：填充 0xFF
		send_data.rx_buff[8]  = 0xFF; send_data.rx_buff[9]  = 0xFF; // Temp
		send_data.rx_buff[10] = 0xFF; send_data.rx_buff[11] = 0xFF; // X, Y
		send_data.rx_buff[12] = 0xFF; send_data.rx_buff[13] = 0xFF; // X, Y
	}
	else {
		max_temp = UART_ConvertRawAdToTemp(tmp_Spot_max.spot_AD);
		send_data.rx_buff[8]  = (max_temp >> 8) & 0xFF;
		send_data.rx_buff[9]  = max_temp & 0xFF;
		send_data.rx_buff[10] = (tmp_Spot_max.spot_X_Pos >> 8) & 0xFF;
		send_data.rx_buff[11] = tmp_Spot_max.spot_X_Pos & 0xFF;
		send_data.rx_buff[12] = (tmp_Spot_max.spot_Y_Pos >> 8) & 0xFF;
		send_data.rx_buff[13] = tmp_Spot_max.spot_Y_Pos & 0xFF;
	}
	if (min_flag == 0x00) {
		// 关闭最低温显示：填充 0xFF
		send_data.rx_buff[14] = 0xFF; send_data.rx_buff[15] = 0xFF; // Temp
		send_data.rx_buff[16] = 0xFF; send_data.rx_buff[17] = 0xFF; // X, Y
		send_data.rx_buff[18] = 0xFF; send_data.rx_buff[19] = 0xFF; // X, Y
	}
	else {
		min_temp = UART_ConvertRawAdToTemp(tmp_Spot_min.spot_AD);
		send_data.rx_buff[14] = (min_temp >> 8) & 0xFF;
		send_data.rx_buff[15] = min_temp & 0xFF;
		send_data.rx_buff[16] = (tmp_Spot_min.spot_X_Pos >> 8) & 0xFF;
		send_data.rx_buff[17] = tmp_Spot_min.spot_X_Pos & 0xFF;
		send_data.rx_buff[18] = (tmp_Spot_min.spot_Y_Pos >> 8) & 0xFF;
		send_data.rx_buff[19] = tmp_Spot_min.spot_Y_Pos & 0xFF;
	}
	Disp_Capture_Info();
	vision_log_debug("max_flag = %d, max_temp = %+.1f; min_flag = %d, min_temp = %+.1f\n", max_flag, max_temp/10.0f, min_flag, min_temp/10.0f);
}

void UART_OPR_TempCalib_Second_Function(u8 *Valid_data)
{
	if(Valid_data[8] == 0x00){ // 二次校准功能开关
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if(!SystemParaMenu2.Usermenu_SW)	 {MessageReturnState(CommandNotSupported);return;}

		Temp_Parameter.Second_CalibFlag = Valid_data[9]&0x01;
		Write_TempParameter_cfg(&Temp_Parameter);
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = Temp_Parameter.Second_CalibFlag;
	}else if(Valid_data[8] == 0x01){ // 二次校准标定半自动模式
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if((!SystemParaMenu2.Usermenu_SW)||(!Temp_Parameter.Second_CalibFlag))	 {MessageReturnState(CommandNotSupported);return;}
		if(Valid_data[9] == 0x00) // 30度标定
		{
			SecondCalib_Parameter.CalibMode = 1;
		}else if(Valid_data[9] == 0x01) // 70/140度标定
		{
			SecondCalib_Parameter.CalibMode = 2;
		}else if(Valid_data[9] == 0x02) // 150/600度标定
		{
			SecondCalib_Parameter.CalibMode = 3;
		}else{
			MessageReturnState(CommandNotSupported);return;
		}

		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = 0x01;
	}else if(Valid_data[8] == 0x02){ // 二次校准标定表回读
		u8 i,j;
		int16_t  Temp;
		int16_t Sec_TEMPdata[12] = {0};
		uint16_t Sec_ADdata[12] = {0};
		if(Valid_data[9] == 0x01){
			if((!SystemParaMenu2.Usermenu_SW)||(!Temp_Parameter.Second_CalibFlag))	 {MessageReturnState(CommandNotSupported);return;}
			if(SystemParaMenu2.Usermenu_SW==1)
			{
				for(i=0;i<12;i++)
				{
					Sec_TEMPdata[i] = Temp_Parameter.Second_TEMP1[i];
					Sec_ADdata[i] = Temp_Parameter.Second_AD1[i];
				}
			}
			else if(SystemParaMenu2.Usermenu_SW==2)
			{
				for(i=0;i<12;i++)
				{
					Sec_TEMPdata[i] = Temp_Parameter.Second_TEMP2[i];
					Sec_ADdata[i] = Temp_Parameter.Second_AD2[i];
				}
			}
			else {MessageReturnState(CommandNotSupported);return;}
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x1E;
	/************************************  发送温度数据  ******************************************************/
			send_data.rx_buff[8] = 0x01;  //发送温度数据
			for(i=9,j=0;i<33;i+=2)
			{
				Temp = Sec_TEMPdata[j];
				if(Temp>>15)
				{
					Temp -= 1;
					Temp = ~Temp;
					Temp = (1<<15) + Temp;
				}
				send_data.rx_buff[i+0] = Temp>>8;
				send_data.rx_buff[i+1] = Temp>>0;
				j++;
			}
			Return_Message_Processing();
	/**************************************  发送AD数据  ****************************************************/
			send_data.rx_buff[8] = 0x02;   //发送AD数据
			for(i=9,j=0;i<33;i+=2)
			{
				send_data.rx_buff[i+0] = Sec_ADdata[j]>>8;
				send_data.rx_buff[i+1] = Sec_ADdata[j]>>0;
				j++;
			}
			Return_Message_Processing();
			memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
			}else{
				MessageReturnState(CommandNotSupported);return;}
	}else if(Valid_data[8] == 0x03){ // 二次校准标定自动全局偏移修正开启
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if((!SystemParaMenu2.Usermenu_SW)||(!Temp_Parameter.Second_CalibFlag))	 {MessageReturnState(CommandNotSupported);return;}
		
		if(Valid_data[9] == 0x01)
		{
			SecondCalib_Parameter.Global_offsetFlag = 1;
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = 0x01;
		}else{MessageReturnState(CommandNotSupported);return;}	
	}else if(Valid_data[8] == 0x04){ // 二次校准标定全局偏移查询
		uint16_t i;
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x09;

		if(Valid_data[9] == 0x01)
		{
			if((!SystemParaMenu2.Usermenu_SW)||(!Temp_Parameter.Second_CalibFlag))	 {MessageReturnState(CommandNotSupported);return;}
			if(SystemParaMenu2.Usermenu_SW==1)
			{
				if(Temp_Parameter.Second_correct1 & (1<<15))
				{
					i = ~(Temp_Parameter.Second_correct1-1);
					i = i|(1<<15);
				}
				else		{i = Temp_Parameter.Second_correct1;}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0x01;
				send_data.rx_buff[10] = i>>8;
				send_data.rx_buff[11] = i>>0;
			}
			else if(SystemParaMenu2.Usermenu_SW==2)
			{
				if(Temp_Parameter.Second_correct2 & (1<<15))
				{
					i = ~(Temp_Parameter.Second_correct2-1);
					i = i|(1<<15);
				}
				else		{i = Temp_Parameter.Second_correct2;}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0x02;
				send_data.rx_buff[10] = i>>8;
				send_data.rx_buff[11] = i>>0;
			}
			// else if(SystemParaMenu2.Usermenu_SW==3)
			// {
			// 	if(Temp_Parameter.Temp_correct3 & (1<<15))
			// 	{
			// 		i = ~(Temp_Parameter.Second_correct3-1);
			// 		i = i|(1<<15);
			// 	}
			// 	else		{i = Temp_Parameter.Second_correct3;}
			// 	send_data.rx_buff[8] = Valid_data[8];
			// 	send_data.rx_buff[9] = 0x03;
			// 	send_data.rx_buff[10] = i>>8;
			// 	send_data.rx_buff[11] = i>>0;
			// }
		}
	}else if(Valid_data[8] == 0x05)   //二次校准手动标定设置
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if((!SystemParaMenu2.Usermenu_SW)||(!Temp_Parameter.Second_CalibFlag))	 {MessageReturnState(CommandNotSupported);return;}
		if(SystemParaMenu2.Usermenu_SW == 1){
			Temp_Parameter.Second_AD1[4] = Valid_data[9]<<8|Valid_data[10];
			Temp_Parameter.Second_AD1[6] = Valid_data[11]<<8|Valid_data[12];
			Temp_Parameter.Second_AD1[8] = Valid_data[13]<<8|Valid_data[14];

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
		}else if(SystemParaMenu2.Usermenu_SW == 2){
			Temp_Parameter.Second_AD2[4] = Valid_data[9]<<8|Valid_data[10];
			Temp_Parameter.Second_AD2[6] = Valid_data[11]<<8|Valid_data[12];
			Temp_Parameter.Second_AD2[7] = Valid_data[13]<<8|Valid_data[14];

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
		}else{
			MessageReturnState(CommandNotSupported);return;}

		SecondCalib_Cal(SystemParaMenu2.Usermenu_SW);
		Write_TempParameter_cfg(&Temp_Parameter);	
		send_data.rx_buff[8] = Valid_data[8];
		send_data.rx_buff[9] = 0x01;

	}else if(Valid_data[8] == 0x06)  // 二次校准标定手动设置全局偏移值
	{
		ts_s16 k;
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if(Valid_data[9] == 0x00) //第一档测温
		{
			if((SystemParaMenu2.Usermenu_SW==1)&&Temp_Parameter.Second_CalibFlag)
			{
				Temp_Parameter.Second_correct1  = Valid_data[10]<<8;
				Temp_Parameter.Second_correct1 |= Valid_data[11]<<0;

				if(Temp_Parameter.Second_correct1 & (1<<15))
				{
					k = Temp_Parameter.Second_correct1 &(~(1<<15));
					Temp_Parameter.Second_correct1 = 0-k;
				}
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0x01;
			}else {
				MessageReturnState(CommandNotSupported);return;}
		}
		else if(Valid_data[9] == 0x01)//第二档测温
		{
			if((SystemParaMenu2.Usermenu_SW==2)&&Temp_Parameter.Second_CalibFlag)
			{
				Temp_Parameter.Second_correct2  = Valid_data[10]<<8;
				Temp_Parameter.Second_correct2 |= Valid_data[11]<<0;

				if(Temp_Parameter.Second_correct2 & (1<<15))
				{
					k = Temp_Parameter.Second_correct2 & (~(1<<15));
					Temp_Parameter.Second_correct2 = 0-k;
				}
				
				send_data.rx_buff[8] = Valid_data[8];
				send_data.rx_buff[9] = 0x01;
			}else {
				MessageReturnState(CommandNotSupported);return;}
		}else {
				MessageReturnState(CommandNotSupported);return;}
		Write_TempParameter_cfg(&Temp_Parameter);	
	}else if(Valid_data[8] == 0x07)  // 二次校准功能状态查询
	{
		send_data.rx_buff[3] = 0x00;
		send_data.rx_buff[4] = 0x07;
		if(!SystemParaMenu2.Usermenu_SW)	 {MessageReturnState(CommandNotSupported);return;}
		if(Valid_data[9] == 0x01){  //查询二次校准功能状态
			
			send_data.rx_buff[8] = Valid_data[8];
			send_data.rx_buff[9] = Temp_Parameter.Second_CalibFlag;
		}else{
			MessageReturnState(CommandNotSupported);return;}
	}
	else{
		MessageReturnState(CommandNotSupported);
		return;
	}
}

void Menu_Two_Protocol(u8 *databuff)
{
	switch (databuff[7])
	{
	default:
		MessageReturnState(CommandNotSupported);
		return;
	case 0x94:					 // DET探测器相关参数设置、查询
		if (databuff[8] == 0xAA) // 设置
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x08;
			send_data.rx_buff[8] = databuff[8];
			if (databuff[9] == 0x00) // Gain
			{
				SystemParaMenu2.GAIN = databuff[10] & 0x7F;
				//planck640c_write_gain_adc(SystemParaMenu2.GAIN);

				 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
				#ifdef CONFIG_SNR_PLANCK1280C
				{
					SystemParaMenu2.GAIN1280=SystemParaMenu2.GAIN;
					planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
				}
				#else
				{
					planck640c_write_gain_adc(SystemParaMenu2.GAIN);
				}
				#endif
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.GAIN;
			}
			else if (databuff[9] == 0x01) // GOF
			{
				SystemParaMenu2.GOF = databuff[10] & 0x7F;
				//planck640c_write_gof(SystemParaMenu2.GOF);
				 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
				#ifdef CONFIG_SNR_PLANCK1280C
				{	
					SystemParaMenu2.GOF1280=SystemParaMenu2.GOF;	
					planck1280c_write_gof(SystemParaMenu2.GOF1280);
				}
				#else
				{
					planck640c_write_gof(SystemParaMenu2.GOF);
				}
				#endif
				if (SystemParaMenu2.Usermenu_SW <= 1) // 一档测温 1
				{
					SystemParaMenu2.GOF_Val[0] = SystemParaMenu2.GOF;
				}
				else if (SystemParaMenu2.Usermenu_SW == 2) // 二档测温
				{
					SystemParaMenu2.GOF_Val[1] = SystemParaMenu2.GOF;
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.GOF;
			}
			else if (databuff[9] == 0x02) // Cint
			{
				SystemParaMenu2.INT_C = databuff[10] & 0x07;
				//planck640c_write_cint(SystemParaMenu2.INT_C);
				 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
				#ifdef CONFIG_SNR_PLANCK1280C
				{
					SystemParaMenu2.Cint1280=SystemParaMenu2.INT_C;
					planck1280c_write_cint(SystemParaMenu2.Cint1280);
				}
				#else
				{
					planck640c_write_cint(SystemParaMenu2.INT_C);
				}
				#endif
				if (SystemParaMenu2.Usermenu_SW <= 1) // 一档测温 1
				{
					SystemParaMenu2.INT_C_Val[0] = SystemParaMenu2.INT_C;
				}
				else if (SystemParaMenu2.Usermenu_SW == 2) // 二档测温
				{
					SystemParaMenu2.INT_C_Val[1] = SystemParaMenu2.INT_C;
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.INT_C;
			}
			else if (databuff[9] == 0x03) // Tint
			{
				SystemParaMenu2.INT_T = databuff[10];
				if (SystemParaMenu2.INT_T > 76)
					SystemParaMenu2.INT_T = 76;
				planck640c_write_tint(SystemParaMenu2.INT_T);
				planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
				if (SystemParaMenu2.Usermenu_SW <= 1) // 一档测温 1
				{
					SystemParaMenu2.INT_T_Val[0] = SystemParaMenu2.INT_T;
				}
				else if (SystemParaMenu2.Usermenu_SW == 2) // 二档测温
				{
					SystemParaMenu2.INT_T_Val[1] = SystemParaMenu2.INT_T;
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.INT_T;
			}
			else if (databuff[9] == 0x04) // OOC_PAR/GOS
			{
				SystemParaMenu2.OOC_Par = databuff[10];
				if (SystemParaMenu2.OOC_Par > 63)
					SystemParaMenu2.OOC_Par = 63;
				planck640c_write_gos(SystemParaMenu2.OOC_Par);
				if (SystemParaMenu2.Usermenu_SW <= 1) // 一档测温 1
				{
					SystemParaMenu2.OOC_Par_Val[0] = SystemParaMenu2.OOC_Par;
				}
				else if (SystemParaMenu2.Usermenu_SW == 2) // 二档测温
				{
					SystemParaMenu2.OOC_Par_Val[1] = SystemParaMenu2.OOC_Par;
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.OOC_Par;
			}
			else if (databuff[9] == 0x05) // DET_Init
			{
				DetParInit(1);
				Int_Shutter_pro();
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = 0x00;
			}
			else if (databuff[9] == 0x06) // DET_Mode
			{
				SystemParaMenu2.DetMode = databuff[10];
				if (SystemParaMenu2.DetMode > DET_MODE_OFF)
					SystemParaMenu2.DetMode = DET_MODE_OFF;
				// VotDETParSelect();
				ISP_set_det_switch_force_flag(2);

				Int_Shutter_pro();
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.DetMode;
			}
			else if (databuff[9] == 0x07) // DET_Calc
			{
				if (SystemParaMenu2.DetMode == DET_MODE_NORMAL || SystemParaMenu2.DetMode == DET_MODE_OFF) // OFF或Normal模式下生效
				{
					SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6;
					SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
					SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2;
					vot_auto_calc_gos_for_det(SystemParaMenu2.DetMode );
					UpdatePromptInfoWithTimeMs("Det calc finished!", 800,VOT_LOG_TYPE_MENU2);

					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = 0x01;
				}
				else
				{
					UpdatePromptInfoWithTimeMs("DetMode not supported!", 800,VOT_LOG_TYPE_MENU2);
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x08) // NdacL
			{
				SystemParaMenu2.NDAC_L = databuff[10];
				if (SystemParaMenu2.NDAC_L > 31)
					SystemParaMenu2.NDAC_L = 31;
				planck640c_write_ndac((SystemParaMenu2.NDAC_H << 5) | SystemParaMenu2.NDAC_L);
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.NDAC_L;
			}
			else if (databuff[9] == 0x09) // NdacH
			{
				if (databuff[10] == 0)
				{
					SystemParaMenu2.NDAC_H = 0x80;
				}
				else if (databuff[10] == 0x01)
				{
					SystemParaMenu2.NDAC_H = 0x80;
				}
				else if (databuff[10] == 0x02)
				{
					SystemParaMenu2.NDAC_H = 0xC0;
				}
				else if (databuff[10] == 0x03)
				{
					SystemParaMenu2.NDAC_H = 0xE0;
				}
				else if (databuff[10] == 0x04)
				{
					SystemParaMenu2.NDAC_H = 0xF0;
				}
				else if (databuff[10] == 0x05)
				{
					SystemParaMenu2.NDAC_H = 0xF8;
				}
				else if (databuff[10] == 0x06)
				{
					SystemParaMenu2.NDAC_H = 0xFC;
				}
				else if (databuff[10] == 0x07)
				{
					SystemParaMenu2.NDAC_H = 0xFE;
				}
				else if (databuff[10] == 0x08)
				{
					SystemParaMenu2.NDAC_H = 0xFF;
				}
				else
				{
					SystemParaMenu2.NDAC_H = 0xFF;
				}
				planck640c_write_ndac((SystemParaMenu2.NDAC_H << 5) | SystemParaMenu2.NDAC_L);
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.NDAC_H;
			}
			else
			{
				MessageReturnState(CommandNotSupported);
				return;
			}
		}
		else if (databuff[8] == 0xFF) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x0E;
			send_data.rx_buff[8] = databuff[8];

			send_data.rx_buff[9] = SystemParaMenu2.GAIN;
			send_data.rx_buff[10] = SystemParaMenu2.GOF;
			send_data.rx_buff[11] = SystemParaMenu2.INT_C;
			send_data.rx_buff[12] = SystemParaMenu2.INT_T;
			send_data.rx_buff[13] = SystemParaMenu2.OOC_Par;
			send_data.rx_buff[14] = SystemParaMenu2.DetMode;
			send_data.rx_buff[15] = SystemParaMenu2.NDAC_L;
			send_data.rx_buff[16] = SystemParaMenu2.NDAC_H;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
		break;

	case 0x90:					 // 菜单二 USB项，Env项，Enlarge项 设置、查询
		if (databuff[8] == 0xAA) // 设置
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x08;
			send_data.rx_buff[8] = databuff[8];
			if (databuff[9] == 0x00) // 设置USB项
			{
				ts_u08 m_video_out_mode = 0;
				m_video_out_mode = databuff[10];
				if (m_video_out_mode < VIDEO_OUTPUT_MODE_ENUM_BUTT)
				{
					SystemParaMenu2.m_video_out_mode = m_video_out_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);

					if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC)
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
					else if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG)
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
					else if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
					else if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601)
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
					else if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI)
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
					// else if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB ){
					// 	UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					// }
					else
					{
						UpdatePromptInfoWithTimeMs(" please restart system !", 500,VOT_LOG_TYPE_MENU2);
					}
				}
				else
				{
					UpdatePromptInfoWithTimeMs(" para is not vaild !", 500,VOT_LOG_TYPE_MENU2);
				}

				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_video_out_mode;
			}
			else if (databuff[9] == 0x01) // 设置cvbs Enlarge项
			{
				ts_u08 requested_mode = databuff[10] & 0x01;
				if (databuff[10] >= DEVICE_CVBS_DISPLAY_MODE_BUTT) {
					MessageReturnState(CommandNotSupported);
					return;
				}
				if (SystemParaMenu2.Usermenu_SW && (requested_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE)) {
					MessageReturnState(CommandNotSupported);
					return;
				}
				// 模式发生变化，执行切换
				if (requested_mode != SystemParaMenu2.m_cvbs_display_mode) {
					SystemParaMenu2.m_cvbs_display_mode = requested_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);

					if (SystemParaMenu2.m_cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE) {
						UpdatePromptInfoWithTimeMs("cvbs Enlarge ON mode !", 1000, VOT_LOG_TYPE_MENU2);
					} else {
						UpdatePromptInfoWithTimeMs("cvbs Enlarge OFF mode !", 1000, VOT_LOG_TYPE_MENU2);
					}
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
					cvbs_osd_refresh_display(1);

					send_data.rx_buff[9]  = databuff[9];
					send_data.rx_buff[10] = SystemParaMenu2.m_cvbs_display_mode;
				} 
				else {
					send_data.rx_buff[9]  = databuff[9];
					send_data.rx_buff[10] = 0xFF;
				}
			}
			else if (databuff[9] == 0x02) // 设置开机后强制打档的时间段（Min为单位） 0-10Min
			{
				SystemParaMenu2.m_st1_m = databuff[10];
				if (SystemParaMenu2.m_st1_m > 10)
					SystemParaMenu2.m_st1_m = 10;
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_st1_m;
			}
			else if (databuff[9] == 0x03) // 设置强制打档功能的打档时间间隔（S为单位） 5-30S
			{
				if (SystemParaMenu2.m_st1_m > 0)
				{
					SystemParaMenu2.m_st1_s = databuff[10];
					if (SystemParaMenu2.m_st1_s > 30)
						SystemParaMenu2.m_st1_s = 30;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = SystemParaMenu2.m_st1_s;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x04) // 设置DET温度变化而打档的阈值
			{
				SystemParaMenu2.m_det_st = databuff[10];
				if (SystemParaMenu2.m_det_st > 50)
					SystemParaMenu2.m_det_st = 50;
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_det_st;
			}
			else if (databuff[9] == 0x05) // 设置uvc Enlarge项
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				if (ts_mode >= 0 && ts_mode < 3)
				{
					if (ts_mode == 2)
					{
						// 软件重启机芯
						SystemParaMenu2.SunPT_SW = 0;
						
						vot_vpo_freeze();
						ts_cpl_fix_msleep(1000);
						planck640c_stop();
						cli_simple_run_command("reboot");
						UpdatePromptInfoWithTimeMs("system will reboot !", 1000,VOT_LOG_TYPE_MENU2);
					}
					else if ((databuff[10] & 0x01) != SystemParaMenu2.m_uvc_enlarger_mode)
					{
						ts_u08 requested_mode = databuff[10] & 0x01;
						// 最高优先级：Usermenu_SW 有效时禁止开启放大
						if (SystemParaMenu2.Usermenu_SW && (requested_mode == OUTPUT_MODE_ENLARGE_ON)) {
							MessageReturnState(CommandNotSupported);
							return;
						}
						SystemParaMenu2.m_uvc_enlarger_mode = requested_mode;
						ISP_write_sys_menu2_cfg(SystemParaMenu2);
						if (SystemParaMenu2.m_uvc_enlarger_mode == OUTPUT_MODE_ENLARGE_ON) {
							UpdatePromptInfoWithTimeMs("uvc Enlarge ON mode !", 1000,VOT_LOG_TYPE_MENU2);
						}
						else if (SystemParaMenu2.m_uvc_enlarger_mode == OUTPUT_MODE_ENLARGE_OFF) {
							UpdatePromptInfoWithTimeMs("uvc Enlarge OFF  mode  !", 1000,VOT_LOG_TYPE_MENU2);
						}

						vot_uvc_usr_reset();

						send_data.rx_buff[9] = databuff[9];
						send_data.rx_buff[10] = SystemParaMenu2.m_uvc_enlarger_mode;
					}
					else
					{
						// 模式无变化，显式回复 0xFF
						send_data.rx_buff[9]  = databuff[9];
						send_data.rx_buff[10] = 0xFF;
					}
				}
				else if (ts_mode >= 3 && ts_mode <= 4)
				{
					// mipi lanes
					if (ts_mode == 3)
					{
						SystemParaMenu2.m_mipi_lane_mode = MIPI_LANE_MODE_2_LANES;
					}
					else if (ts_mode == 4)
					{
						SystemParaMenu2.m_mipi_lane_mode = MIPI_LANE_MODE_4_LANES;
					}
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for mipi lanes !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else if (ts_mode >= 5 && ts_mode <= 8)
				{
					// bt's timing seq for win2
					if (ts_mode == 5)
					{
						SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_NONE;
					}
					else if (ts_mode == 6)
					{
						SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_WIN2_50HZ;
					}
					else if (ts_mode == 7)
					{
						SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_WIN2_25HZ;
					}
					else if (ts_mode == 8)
					{
						SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_WIN2_30HZ;
					}
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for win2 bt timing !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else if (ts_mode >= 17 && ts_mode <= 18)
				{
					// 
					if (ts_mode == 17)
					{
						SystemParaMenu2.m_user_metadata_flag = USER_METADATA_DISABLE;
					}
					else if (ts_mode == 18)
					{
						SystemParaMenu2.m_user_metadata_flag = USER_METADATA_ENABLE;
					}
					UpdatePromptInfoWithTimeMs("system must  reboot for user metadata !", 1000,VOT_LOG_TYPE_MENU2);
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else if ((databuff[10] & 0x01) == 0x00 || (databuff[10] & 0x01) == 0x01)
				{
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = 0xFF;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x06) // 配置固件运行模式
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				if (ts_mode >= FORCE_DETECTOR_TYPE_VOT_640X512 && ts_mode < FORCE_DETECTOR_TYPE_BUTT)
				{
					SystemParaMenu2.m_force_detector_type = ts_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for detector  !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x07) // 配置硬件接口板类型
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				vision_log_warn("ts_mode=%d\n", ts_mode);
				if (ts_mode >= 0 && ts_mode < FORCE_HARDWARE_PORT_TYPE_BUTT)
				{
					SystemParaMenu2.m_force_hardware_port_type = ts_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for hardware port  !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x08) // 配置带参数的
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				ts_u08 ts_mode = databuff[10] & 0xFF;
				ts_u08 ts_value = databuff[11] & 0xFF;

				if (ts_mode == 0)
				{
					if (ts_value == 0)
					{
						SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_RAW_YUV;
					}
					else if (ts_value == 1)
					{
						SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_YUV_RAW;
					}
					else if (ts_value == 2)
					{
						SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_NONE;
					}

					
					UpdatePromptInfoWithTimeMs("system must  reboot for T8 mode !", 1000,VOT_LOG_TYPE_MENU2);
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else if (ts_mode == 1)
				{

					SystemParaMenu2.m_raw_node_pos = vot_check_lut_raw_pos(ts_value);

					UpdatePromptInfoWithTimeMs("system must  reboot for T8 mode !", 1000,VOT_LOG_TYPE_MENU2);
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else if (ts_mode == 2) // 配置帧率fps
				{
					ts_u08 m_device_fps = ts_value;
					if(m_device_fps>=DEVICE_FPS_MODE_50FPS && m_device_fps <DEVICE_FPS_MODE_BUTT)
					{
						SystemParaMenu2.m_device_fps = m_device_fps;
						UpdatePromptInfoWithTimeMs("system must  reboot for fps config !", 1000,VOT_LOG_TYPE_MENU2);
						ISP_write_sys_menu2_cfg(SystemParaMenu2);
					}
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else if (ts_mode == 3) // 配置fix_snc开关
				{
					ts_u08 m_fix_snc_switch = ts_value;
					if(m_fix_snc_switch>=VOT_SWITCH_OFF && m_fix_snc_switch <VOT_SWITCH_BUTT)
					{
						SystemParaMenu2.m_fix_snc_switch = m_fix_snc_switch;
						UpdatePromptInfoWithTimeMs("system must  reboot for fix snc config !", 1000,VOT_LOG_TYPE_MENU2);
						ISP_write_sys_menu2_cfg(SystemParaMenu2);
					}
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else if (ts_mode == 4) // 配置是否使用特殊定制json配置
				{
					ts_u08 EnableFlag = ts_value;

					ts_u08 UseUserMenuDefParam = 0;
					ts_u08 UseSysMenuDefParam = 0;

					//bit0代表为user菜单， bit1代表sys菜单,
					UseUserMenuDefParam = (EnableFlag & 0x01)?1:0;
					UseSysMenuDefParam = (EnableFlag & 0x02)?1:0;

					struct _Usermenu_Parameter UsermenuDefParam;
					if (ISP_read_UserMenu_def_para_cfg(&UsermenuDefParam) == 0) // 读取用户菜单默认参数配置成功
					{
						UsermenuDefParam.UseUserMenuDefParam = UseUserMenuDefParam;
						ISP_write_UserMenu_def_para_cfg(UsermenuDefParam);
					}

					struct Menu2_Para SysMenuDefParam;
					if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 读取系统菜单默认参数配置成功
					{
						SysMenuDefParam.UseSysMenuDefParam = UseSysMenuDefParam;
						ISP_write_SysMenu_def_para_cfg(SysMenuDefParam);
					}		
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else if (ts_mode == 5) // 配置para init /para save
				{
					ts_u08 ts_mode = ts_value;
					if(ts_mode==0)
					{
						InitializeTestmenuParameter(1);
					}
					else {
						SaveTestmenuParameter(0);
					}
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = ts_value;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x09) // 配置带2byte参数的
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0A;
				send_data.rx_buff[8] = databuff[8];
				ts_u08 ts_mode = databuff[10] & 0xFF;
			

				if (ts_mode == 0) //pip mode
				{
					
					if (get_customer_type() == CUSTOMER_TYPE_DSW)
					{
						ts_u08 m_pip_mode = databuff[11] & 0xFF;
						ts_u08 m_rz_ratio = databuff[12] & 0xFF;

						UsermenuParameter.m_pip_mode = m_pip_mode;
						UsermenuParameter.m_rz_ratio = m_rz_ratio;
						vot_pip_mode_set(m_pip_mode, m_rz_ratio, 1);
						ISP_write_sys_cfg();
						send_data.rx_buff[9] = databuff[9];
						send_data.rx_buff[10] = ts_mode;
						send_data.rx_buff[11] = m_pip_mode;
						send_data.rx_buff[12] = m_rz_ratio;
						vision_log_debug("m_pip_mode %d m_rz_ratio %d \n",m_pip_mode,m_rz_ratio);
					}
					else{
						MessageReturnState(CommandNotSupported);
						return;
					}

				}
				else if (ts_mode == 1) //hot spot tracking mode
				{
					if (get_customer_type() == CUSTOMER_TYPE_DSW)
					{
						ts_u08 m_hst_mode = databuff[11] & 0xFF;
	
						vot_hst_mode_set(m_hst_mode);
				
						send_data.rx_buff[9] = databuff[9];
						send_data.rx_buff[10] = ts_mode;
						send_data.rx_buff[11] = m_hst_mode;
						send_data.rx_buff[12] = databuff[12];
						vision_log_debug("m_hst_mode %d \n",m_hst_mode);
					}
					else{
						MessageReturnState(CommandNotSupported);
						return;
					}
				}
				else if (ts_mode == 2) //force T8 模式下支持坏点矫正
				{
					ts_u08 t8_mode_force_bpc = databuff[11] & 0xFF;
					vot_set_t8_mode_force_bpc(t8_mode_force_bpc);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = t8_mode_force_bpc;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("t8_mode_force_bpc %d \n",t8_mode_force_bpc);
				}
				else if (ts_mode == 3) //vot_pod_type
				{
					ts_u08 vot_pod_type = databuff[11] & 0xFF;
					vot_set_vot_pod_type(vot_pod_type);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = vot_pod_type;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("vot_pod_type %d \n",vot_pod_type);
					
				}
				else if (ts_mode == 4) //update firmware policy
				{
					ts_u08 update_policy = databuff[11] & 0xFF;
					
					if(update_policy>VOT_UPDATE_FIRMWARE_POLICY_BUTT){
						update_policy = VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B;
					}
					
					SystemParaMenu2.m_update_firmware_policy_mode = update_policy;
    
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = update_policy;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("update_policy %d \n",update_policy);
					
				}
				else if (ts_mode == 5) //sys boot mode
				{
					ts_u08 boot_mode = databuff[11] & 0xFF;
					
					if(boot_mode>VOT_BOOT_MODE_BUTT){
						boot_mode = VOT_BOOT_MODE_NORMAL;
					}
					
					SystemParaMenu2.m_sys_boot_mode = boot_mode;
    
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = boot_mode;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("boot_mode %d \n",boot_mode);
					
				}
				else if (ts_mode == 6) //ST0 duration
				{
					ts_u08 st0_duration = databuff[11] & 0xFF;
					
					if(st0_duration>ST_0_TOTAL_DURATION_S_MAX){
						st0_duration = ST_0_TOTAL_DURATION_S_DEFAULT;
					}
					
					SystemParaMenu2.m_st0_total_duration_s = st0_duration;
    
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = st0_duration;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("st0_duration %d \n",st0_duration);
					
				}
				else if (ts_mode == 7) //ST0 interval
				{
					ts_u08 st0_interval = databuff[11] & 0xFF;
					
					if(st0_interval>ST_0_INTERVAL_S_MAX){
						st0_interval = ST_0_INTERVAL_S_DEFAULT;
					}
					
					SystemParaMenu2.m_st0_interval_s = st0_interval;
    
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = st0_interval;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					vision_log_debug("st0_interval %d \n",st0_interval);
					
				}
				else if (ts_mode == 8) //drift aisp mode
				{
					ts_u08 temp_drift_aisp_mode = databuff[11] & 0xFF;
					
					if(temp_drift_aisp_mode>TEMP_DRIFT_COMPENSATION_AI_APP_BUTT){
						temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
					}
					
                    vot_set_temp_drift_aisp_mode(temp_drift_aisp_mode);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = temp_drift_aisp_mode;
					send_data.rx_buff[12] = databuff[12];
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("reboot for temp drift aisp !", 1000,VOT_LOG_TYPE_MENU2);
					vision_log_debug("temp_drift_aisp_mode %d \n",temp_drift_aisp_mode);
					
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x0a) // 查询 场景和提示词
			{
				
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				send_data.rx_buff[9] =  databuff[9];
				send_data.rx_buff[10] = UsermenuParameter.m_dev_env_mode ;
				send_data.rx_buff[11] = SystemParaMenu2.SunPT_Info_SW ;  
			}
			else if (databuff[9] == 0x0b) // 配置
			{

			}
			else if (databuff[9] == 0x0c) // 配置客户ID类型
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				vision_log_warn("ts_mode=%d\n", ts_mode);
				if (ts_mode >= 0 && ts_mode < (CUSTOMER_TYPE_BUTT-1))
				{
					SystemParaMenu2.m_force_customer_id = ts_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for customer  id  !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x0d) // 配置镜头类型
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				vision_log_warn("ts_mode=%d\n", ts_mode);
				if (ts_mode >= 0 && ts_mode < (FOCUS_TYPE_BUTT-1))
				{
					SystemParaMenu2.m_force_focus_type = ts_mode;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					UpdatePromptInfoWithTimeMs("system must  reboot for focus type !", 1000,VOT_LOG_TYPE_MENU2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x0E) // 配置是否开启日志输出
			{
				ts_u08 ts_mode = databuff[10] & 0xFF;
				vision_log_warn("debug log =%d\n", ts_mode);
				// if (ts_mode >= 0 && ts_mode <=1)
				// {
				// 	global_vot_misc_cfg.m_force_debug_switch = ts_mode;
				// 	ISP_write_vot_misc_cfg(global_vot_misc_cfg);
				// 	UpdatePromptInfoWithTimeMs("system must  reboot for force debug_switch !", 1000,VOT_LOG_TYPE_MENU2);
				// 	send_data.rx_buff[9] = databuff[9];
				// 	send_data.rx_buff[10] = ts_mode;
				// }
				// else 
				if (ts_mode >= 2 && ts_mode <=3)
				{
					if(ts_mode==2){
						global_vot_misc_cfg.m_force_debug_confirm = VOT_DEBUG_OUTPUT_CONFIRM_MAGIC;
					}
					else if(ts_mode==3){
						global_vot_misc_cfg.m_force_debug_confirm = 0;
					}
					vision_log_warn("m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
					ISP_write_vot_misc_cfg(global_vot_misc_cfg);
					UpdatePromptInfoWithTimeMs("system must  reboot for force debug confirm !", 1000,VOT_LOG_TYPE_MENU2);
				
					ISP_read_vot_misc_cfg(&global_vot_misc_cfg);
					vision_log_warn("m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x0F) // 配置是ISO / manual ad mode
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0A;
				send_data.rx_buff[8] = databuff[8];
				ts_u08 ts_mode = databuff[10] & 0xFF;

				if (ts_mode == 0) //iso mode
				{
					send_data.rx_buff[3] = 0x00;
					send_data.rx_buff[4] = 0x0F;
					ts_u08 switch_flag = databuff[11] & 0xFF;
					ts_u08 iso_position_mode  = databuff[12] & 0xFF; 
					ts_u08 iso_color_mode = databuff[13] & 0xFF;
					ts_u16 u16_iso_h = ((databuff[14] << 8 )| databuff[15] ) ;  
					ts_u16 u16_iso_l = ((databuff[16] << 8 )| databuff[17] ) ; 

					ts_u16 raw_u16_iso_h =0;
					ts_u16 raw_u16_iso_l =0;

					vision_log_debug("switch_flag %u iso_position_mode %u iso_color_mode %u  u16_iso_h %u,u16_iso_l  %u\n",switch_flag,iso_position_mode ,iso_color_mode ,u16_iso_h,u16_iso_l);
					vision_log_warn("input temp ad : temp_u16_iso_h %u, temp_u16_iso_l %u\n" ,u16_iso_h,u16_iso_l);
					if(switch_flag>0){
					
						ts_s08 ret = votLocal_TempAD_TO_RawAD(u16_iso_h, &raw_u16_iso_h);
						if(ret<0){
							MessageReturnState(CommandNotSupported);
							return;
						}

						ret = votLocal_TempAD_TO_RawAD(u16_iso_l, &raw_u16_iso_l);
						if(ret<0){
							MessageReturnState(CommandNotSupported);
							return;
						}
						vision_log_warn("raw ad :  raw_u16_iso_h %u [0x%x],raw_u16_iso_l %u [0x%x] \n" ,raw_u16_iso_h,raw_u16_iso_h,raw_u16_iso_l, raw_u16_iso_l);
					}

					vot_set_iso_parameters( switch_flag, iso_position_mode,  iso_color_mode,  raw_u16_iso_h, raw_u16_iso_l );
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = databuff[11];
					send_data.rx_buff[12] = databuff[12];
					send_data.rx_buff[13] = databuff[13];
					send_data.rx_buff[14] = databuff[14];
					send_data.rx_buff[15] = databuff[15];
					send_data.rx_buff[16] = databuff[16];
					send_data.rx_buff[17] = databuff[17];
					
				}
				else if (ts_mode == 1) //ad filter  mode
				{
					send_data.rx_buff[3] = 0x00;
					send_data.rx_buff[4] = 0x0D;

					ts_u08 filter_ad_mode = databuff[11] & 0xFF;
					ts_u16 u16_iso_h = ((databuff[12] << 8 )| databuff[13] ) ;  
					ts_u16 u16_iso_l = ((databuff[14] << 8 )| databuff[15] ) ; 
					
					ts_u16 raw_u16_iso_h =0;
					ts_u16 raw_u16_iso_l =0;
					
					vision_log_debug(" ts_mode %u filter_ad_mode %u    u16_iso_h %u,u16_iso_l  %u\n",ts_mode, filter_ad_mode ,u16_iso_h,u16_iso_l);
					vision_log_warn("input temp ad : temp_u16_iso_h %u, temp_u16_iso_l %u\n" ,u16_iso_h,u16_iso_l);

					if(filter_ad_mode>0){
						
						ts_s08 ret = votLocal_TempAD_TO_RawAD(u16_iso_h, &raw_u16_iso_h);
						if(ret<0){
							MessageReturnState(CommandNotSupported);
							return;
						}

						ret = votLocal_TempAD_TO_RawAD(u16_iso_l, &raw_u16_iso_l);
						if(ret<0){
							MessageReturnState(CommandNotSupported);
							return;
						}
						vision_log_warn("raw ad :  raw_u16_iso_h %u [0x%x],raw_u16_iso_l %u [0x%x] \n" ,raw_u16_iso_h,raw_u16_iso_h,raw_u16_iso_l, raw_u16_iso_l);
					}

					vot_set_ad_filter_mode(filter_ad_mode, raw_u16_iso_h,raw_u16_iso_l);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_mode;
					send_data.rx_buff[11] = databuff[11];
					send_data.rx_buff[12] = databuff[12];
					send_data.rx_buff[13] = databuff[13];
					send_data.rx_buff[14] = databuff[14];
					send_data.rx_buff[15] = databuff[15];
					vision_log_debug(" ts_mode %u filter_ad_mode %u    u16_iso_h %u,u16_iso_l  %u\n",ts_mode, filter_ad_mode ,u16_iso_h,u16_iso_l);
				}
				else{
					MessageReturnState(CommandNotSupported);
					return;
				}
			}
			else if (databuff[9] == 0x10) // 配置是否显示菜单一提示osd
			{
				ts_u08 ts_switch_flag = databuff[10] & 0xFF;
				vision_log_warn("ts_switch_flag =%d\n", ts_switch_flag);
				if (ts_switch_flag == 0)
				{
					UsermenuParameter.m_menu1_info_tip_visible=VOT_SWITCH_ON_LIKE0;
					
					// ISP_write_sys_cfg();
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_switch_flag;
				}
				else if (ts_switch_flag == 1)
				{
					UsermenuParameter.m_menu1_info_tip_visible=VOT_SWITCH_OFF_LIKE1;
					
					// ISP_write_sys_cfg();
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_switch_flag;
				}
				else if (ts_switch_flag == 2)
				{
					SystemParaMenu2.m_menu2_info_tip_visible=VOT_SWITCH_ON_LIKE0;
					
					// ISP_read_sys_menu2_cfg(&SystemParaMenu2);
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_switch_flag;
				}
				else if (ts_switch_flag == 3)
				{
					SystemParaMenu2.m_menu2_info_tip_visible=VOT_SWITCH_OFF_LIKE1;
					
					// ISP_read_sys_menu2_cfg(&SystemParaMenu2);
					
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = ts_switch_flag;
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}

				
			}
			else if (databuff[9] == 0x11) // 配置超分功能
			{
				ts_u08 npu_ops_sr_en = databuff[10] & 0xFF;
				vision_log_warn("npu_ops_sr_en =%d\n", npu_ops_sr_en);
				if (npu_ops_sr_en < NPU_SR_BUTT)
				{
					//0x00：为OFF,关闭超分；
					// 0x01：640x512->960x768超分使能；
					// 0x02：640x512->1280x1024超分使能；
					// 0x03：384x288->640x512超分使能；
					SystemParaMenu2.m_ts_npu_ops_sr_en=npu_ops_sr_en;
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = npu_ops_sr_en;
					ISP_write_sys_menu2_cfg(SystemParaMenu2);
					if (get_customer_type() == CUSTOMER_TYPE_HAND)//手持机定制,384分辨率下，A6和A12芯片 NPU超分为384->768,无需重启设备，参数直接生效
					{
						NPU_SR_ParamAvailable();//超分使能切换,无需设备重启，参数直接生效
					}
				}
				else
				{
					MessageReturnState(CommandNotSupported);
					return;
				}			
			}
			else if (databuff[9] == 0x12) // 配置ForceDectorType
			{
					ts_u08 m_force_detector_type = databuff[10] & 0xFF;
					vision_log_warn("m_force_detector_type =%d\n", m_force_detector_type);
					if (m_force_detector_type < FORCE_DETECTOR_TYPE_BUTT) {
						SystemParaMenu2.m_force_detector_type=m_force_detector_type;
						send_data.rx_buff[9] = databuff[9];
						send_data.rx_buff[10] = m_force_detector_type;
					}
					else {
						MessageReturnState(CommandNotSupported);
						return;
					}                        
			}
			else {
				MessageReturnState(CommandNotSupported);
				return;
			}
		}
		else if (databuff[8] == 0xFF) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x08;
			send_data.rx_buff[8] = databuff[8];
			if(databuff[9] == 0x00 && databuff[10] == 0x00)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_video_out_mode;
			}
			else if(databuff[9] == 0x00 && databuff[10] == 0x01)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_cvbs_display_mode;
			}
			else if(databuff[9] == 0x00 && databuff[10] == 0x02)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_st1_m;
			}
			else if(databuff[9] == 0x00 && databuff[10] == 0x03)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_st1_s;
			}
			else if(databuff[9] == 0x00 && databuff[10] == 0x04)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_det_st;
			}
			else if(databuff[9] == 0x00 && databuff[10] == 0x05)
			{
					send_data.rx_buff[9] = databuff[10];
					send_data.rx_buff[10] = SystemParaMenu2.m_force_detector_type; 
			}
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}

		break;

	case 0x91: // ASK_Algorithm图像算法相关设置、查询
	{
		// u16 DDE_LevelVal;

		if (databuff[8] == 0xAA) // 设置
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x09;
			send_data.rx_buff[8] = databuff[8];
			if (databuff[9] == 0x00) // 设置SFFC
			{
				SystemParaMenu2.m_sffc_mode = databuff[11];
				if (SystemParaMenu2.m_sffc_mode > 2)
				{
					SystemParaMenu2.m_sffc_mode = 2;
				}

				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = SystemParaMenu2.m_sffc_mode;
			}
			else if (databuff[9] == 0x01) // sffc_calculate计算
			{
				sffc_calculate(1); // 锅盖计算

				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
			}
			else if (databuff[9] == 0x02) // 设置EE_Mode
			{
				SystemParaMenu2.m_ee_enhance_mode = databuff[11];
				if (SystemParaMenu2.m_ee_enhance_mode > 3)
					SystemParaMenu2.m_ee_enhance_mode = 3;
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
				ISP_ee_enhance_mode_set(SystemParaMenu2.m_ee_enhance_mode, 0);

				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = SystemParaMenu2.m_ee_enhance_mode;
			}
			else if (databuff[9] == 0x03) // 锅盖刷新
			{
				SFFC_Refresh(1);
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = 0x00;
				send_data.rx_buff[11] = 0x00;
			}
			else
			{
				MessageReturnState(CommandNotSupported);
				return;
			}
		}
		else if (databuff[8] == 0xFF) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x08;
			send_data.rx_buff[8] = databuff[8];

			send_data.rx_buff[9] = SystemParaMenu2.m_sffc_mode;
			send_data.rx_buff[10] = SystemParaMenu2.m_ee_enhance_mode;
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	break;
#if 0
		case 0x87:			//调焦菜单相关设置、查询
		{
			if(databuff[8] == 0xAA)
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				if(databuff[9] == 0x00)		//调焦算法切换
				{
					if(databuff[11]==0x01)				SystemParaMenu2.Zoom_Lens=0;
					else if(databuff[11]==0x02)		SystemParaMenu2.Zoom_Lens = 1;
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			    else if(databuff[9] == 0x01)//设置聚焦Left_Val
				{
			    	SystemParaMenu2.Left_Val = databuff[10]<<8|databuff[11];
			    	send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			    else if(databuff[9] == 0x02)//设置聚焦Right_Val
			    {
			    	SystemParaMenu2.Right_Val = databuff[10]<<8|databuff[11];
					if(SystemParaMenu2.Right_Val > 500)			{SystemParaMenu2.Right_Val = 500;}
			    	send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
			    }
			    else if(databuff[9] == 0x03)//设置自动调焦速度Speed
				{
			    	SystemParaMenu2.OperatorSpeed = databuff[11];
					if(SystemParaMenu2.OperatorSpeed > 50)		{SystemParaMenu2.OperatorSpeed = 50;}
			    	send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			    else if(databuff[9] == 0x04)//Focus_Init初始化操作；
				{
			    	FPGA_SHOWMES(1);
#ifdef FunctionAutoFocus
					SystemParaMenu2.Zoom_Lens = 1;
					SystemParaMenu2.Right_Val = 100;
					SystemParaMenu2.Left_Val = 100;
					SystemParaMenu2.FocusSite = 0;
					//SystemParaMenu2.FCal_SW = 1;
					SystemParaMenu2.OperatorSpeed = 5;
					SystemParaMenu2.Focus_Lx = 10;
					SystemParaMenu2.Focus_Ly = 10;
					SystemParaMenu2.Focus_Rx = 610;
					SystemParaMenu2.Focus_Ry = 490;
					FocusAreaSet(SystemParaMenu2.Focus_Lx,SystemParaMenu2.Focus_Ly,SystemParaMenu2.Focus_Rx,SystemParaMenu2.Focus_Ry);
					delay_25ms(20);
					Focus_Calibration_Flag = 1;

#endif
					FPGA_SHOWMES(0);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			}
			else if(databuff[8] == 0xFF)	//查询
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0C;
				send_data.rx_buff[8] = databuff[8];

				if(SystemParaMenu2.Zoom_Lens == 0)					send_data.rx_buff[9] = 0x01; //老算法
				else if(SystemParaMenu2.Zoom_Lens == 1)			send_data.rx_buff[9] = 0x02; //新算法
				send_data.rx_buff[10] = SystemParaMenu2.Left_Val>>8;
				send_data.rx_buff[11] = SystemParaMenu2.Left_Val>>0;
				send_data.rx_buff[12] = SystemParaMenu2.Right_Val>>8;
				send_data.rx_buff[13] = SystemParaMenu2.Right_Val>>0;
				send_data.rx_buff[14] = SystemParaMenu2.OperatorSpeed;

			}else{MessageReturnState(CommandNotSupported);return;}
		}
		break;

		case 0x88:			//高温报警功能相关
		{
			if(databuff[8] == 0xAA)
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				if(databuff[9] == 0x00)		//高温报警开关
				{
					MaxTempAlarmFunSW = databuff[11]&0x01;
					APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xBFFFFFFF)|(MaxTempAlarmFunSW<<30)));
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x01)		//高温报警温度阈值
				{
					MaxTempLimit = databuff[10]<<8|databuff[11];
					APB3_REGW(0xf8100098,0,((APB3_REGR(0xf8100098,0)&0xFFFFC000)|(MaxTempLimit&0x00003FFF)));
					OverPixNum = (APB3_REGR(0xf8100098,0)&0x3FFFC000)>>14;
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x02)		//超阈值像素个数
				{
					SystemParaMenu2.pixeLimit = databuff[10]<<8|databuff[11];
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			}
			else if(databuff[8] == 0xFF) //查询
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0B;
				send_data.rx_buff[8] = databuff[8];

				send_data.rx_buff[9] =  MaxTempAlarmFunSW;
				send_data.rx_buff[10] = MaxTempLimit>>8;
				send_data.rx_buff[11] = MaxTempLimit>>0;
				send_data.rx_buff[12] = SystemParaMenu2.pixeLimit>>8;
				send_data.rx_buff[13] = SystemParaMenu2.pixeLimit>>0;
			}else{MessageReturnState(CommandNotSupported);return;}
		}
		break;

		case 0x89:			//区域入侵报警功能相关
		{
			if(databuff[8] == 0xAA)
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				if(databuff[9] == 0x00)//设置区域入侵报警开关
				{
					SystemParaMenu2.Alarm_SW = databuff[11];
					APB3_REGW_Alarm_Range(SystemParaMenu2.Alarm_SW);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x01)//采集计算区域报警阈值的开关
				{
					SystemParaMenu2.Range_SW = databuff[11];
					APB3_REGW_Alarm_SW(SystemParaMenu2.Range_SW);
					if(SystemParaMenu2.Range_SW ==1)
					{
						APB3_REGW_Alarm_AutoVal(SystemParaMenu2.AlarmAutoVal);
					}
					else if(SystemParaMenu2.Range_SW ==2)   APB3_REGW_Alarm_Val(SystemParaMenu2.AlarmVal);

					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x02)//设置手动标定阈值
				{
					SystemParaMenu2.AlarmVal = databuff[10]<<8|databuff[11];
					APB3_REGW_Alarm_Val(SystemParaMenu2.AlarmVal);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			}
			else if(databuff[8] == 0xFF)//查询
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0A;
				send_data.rx_buff[8] = databuff[8];

				send_data.rx_buff[9] = SystemParaMenu2.Alarm_SW;
				send_data.rx_buff[10] = SystemParaMenu2.Range_SW;
				send_data.rx_buff[11] = SystemParaMenu2.AlarmVal>>8;
				send_data.rx_buff[12] = SystemParaMenu2.AlarmVal>>0;
			}else{MessageReturnState(CommandNotSupported);return;}
		}
		break;

		case 0x8A:			//阳光保护功能相关
		{
			if(databuff[8] == 0xAA)
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x09;
				send_data.rx_buff[8] = databuff[8];
				if(databuff[9] == 0x00)//设置阳光保护功能开关
				{
					SystemParaMenu2.SunPT_SW = databuff[11]&0x01;
					APB3_REGW_SunPT_SW(SystemParaMenu2.SunPT_SW);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x01)//设置阳光保护阈值
				{
					SystemParaMenu2.S_Range = databuff[10]<<8|databuff[11];
					// APB3_REGW_S_Range(SystemParaMenu2.S_Range - FPGA_Bak_AVG()/2);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x02)//设置阳光保护超阈值的个数
				{
					SystemParaMenu2.S_Val = databuff[10]<<8|databuff[11];
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
				else if(databuff[9] == 0x03)//设置保护时间
				{
					UsermenuParameter.P_Time = databuff[11];
					if(UsermenuParameter.P_Time <  1)	UsermenuParameter.P_Time = 1;
					else if(UsermenuParameter.P_Time > 20)	UsermenuParameter.P_Time  = 20;
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
				}
			}
			else if(databuff[8] == 0xFF)//查询
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0C;
				send_data.rx_buff[8] = databuff[8];

				send_data.rx_buff[9] =  SystemParaMenu2.SunPT_SW;
				send_data.rx_buff[10] = SystemParaMenu2.S_Range>>8;
				send_data.rx_buff[11] = SystemParaMenu2.S_Range>>0;
				send_data.rx_buff[12] = SystemParaMenu2.S_Val>>8;
				send_data.rx_buff[13] = SystemParaMenu2.S_Val>>0;
				send_data.rx_buff[14] = UsermenuParameter.P_Time;

			}else{MessageReturnState(CommandNotSupported);return;}
		}
		break;

		case 0x8B:			//火点报警功能相关
		{
			if(databuff[8] == 0xAA)
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0B;
				send_data.rx_buff[8] = databuff[8];
				if(databuff[9] == 0x01)		//设置火点报警的AD阈值
				{
					SystemParaMenu2.AlarmAD = databuff[12]<<8|databuff[13];
					APB3_REGW_AlarmAD(SystemParaMenu2.AlarmAD);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
					send_data.rx_buff[12] = databuff[12];
					send_data.rx_buff[13] = databuff[13];
				}
				else if(databuff[9] == 0x02)//设置火点报警超阈值的个数
				{
					SystemParaMenu2.AlarmNum = databuff[10]<<24|databuff[11]<<16|databuff[12]<<8|databuff[13];
					APB3_REGW_AlarmNum(SystemParaMenu2.AlarmNum);
					send_data.rx_buff[9] = databuff[9];
					send_data.rx_buff[10] = databuff[10];
					send_data.rx_buff[11] = databuff[11];
					send_data.rx_buff[12] = databuff[12];
					send_data.rx_buff[13] = databuff[13];
				}
			}
			else if(databuff[8] == 0xFF) //查询
			{
				send_data.rx_buff[3] = 0x00;
				send_data.rx_buff[4] = 0x0C;
				send_data.rx_buff[8] = databuff[8];

				send_data.rx_buff[9] =  SystemParaMenu2.AlarmAD>>8;
				send_data.rx_buff[10] = SystemParaMenu2.AlarmAD>>0;
				send_data.rx_buff[11] = SystemParaMenu2.AlarmNum>>24;
				send_data.rx_buff[12] = SystemParaMenu2.AlarmNum>>16;
				send_data.rx_buff[13] = SystemParaMenu2.AlarmNum>>8;
				send_data.rx_buff[14] = SystemParaMenu2.AlarmNum>>0;
			}else{MessageReturnState(CommandNotSupported);return;}
		}
		break;
#endif
	case 0x92: // 菜单二栏信息显示查询
	{
		ts_u16 DET_TempAD;
		ts_f32 DET_TempT;
		ts_u16 AD_avg = 0, AD_max = 0;
		ts_u16 TES_Val;

		// float K,TEMP;
		u16 DETTem = 0, SenTem = 0;
		// u16 Data16bit;

		if ((databuff[8] == 0xFF) && (databuff[9] == 0x01)) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x16;
			send_data.rx_buff[8] = databuff[8];

			ts_isp_fire_statis_s_t fire_statis;
			vot_hst_update_attr_safe();
			vot_get_fire_statis(&fire_statis);
			AD_avg = fire_statis.st_frm_info.u16_mean;
			AD_max = fire_statis.st_frm_info.u16_max_val;

			vot_get_temperature_info(&DET_TempAD, &DET_TempT);
			TES_Val = vot_get_tes_info();

			/************ADV****************/
			send_data.rx_buff[9] = AD_avg >> 8;
			send_data.rx_buff[10] = AD_avg >> 0;
			/************AD_Max************/
			send_data.rx_buff[11] = AD_max >> 8;
			send_data.rx_buff[12] = AD_max >> 0;
			/************DET**************/
			send_data.rx_buff[13] = DET_TempAD >> 8;
			send_data.rx_buff[14] = DET_TempAD >> 0;
			/**********DET_T*************/
			if (DET_TempT < 0)
			{
				send_data.rx_buff[15] = 0x01;
			}
			else
			{
				send_data.rx_buff[15] = 0x00;
			}
			DETTem = abs((s16)(DET_TempT * 10));
			send_data.rx_buff[16] = DETTem >> 8;
			send_data.rx_buff[17] = DETTem >> 0;
			/**********Sensor温度*************/
			if (SystemParaMenu2.Usermenu_SW) {
				ts_f32 tmp_i2cTemp = READ_TMP112_Temp();
				// 符号位
				send_data.rx_buff[18] = (tmp_i2cTemp < 0) ? 0x01 : 0x00;
				
				SenTem = abs((s16)(tmp_i2cTemp * 10));
				send_data.rx_buff[19] = SenTem >> 8;
				send_data.rx_buff[20] = SenTem >> 0;
			}
			else {
				send_data.rx_buff[18] = 0x00;
				send_data.rx_buff[19] = 0x00;
				send_data.rx_buff[20] = 0x00;
			}
			/**********TES值*************/
			send_data.rx_buff[21] = TES_Val >> 8;
			send_data.rx_buff[22] = TES_Val >> 0;
			/**********BAK值*************/
			send_data.rx_buff[23] = (ts_u08)((global_sys_ctrl_info.m_cur_bak_ad_mean_u32 >> 8) & 0xFF);
			send_data.rx_buff[24] = (ts_u08)(global_sys_ctrl_info.m_cur_bak_ad_mean_u32 & 0xFF);
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	break;

	case 0x8D: // 查询序列号、镜头信息、生产日期、探测器编号等
	{
		u8 i = 0;
		if ((databuff[8] == 0xFF) && (databuff[9] == 0x01)) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x33;
			send_data.rx_buff[8] = databuff[8];

			for (i = 0; i < 14; i++)
			{
				send_data.rx_buff[9 + i] = SystemParaMenu2.MerchineSN[i];
			}
			for (i = 0; i < 10; i++)
			{
				send_data.rx_buff[23 + i] = SystemParaMenu2.LensSN[i];
			}
			for (i = 0; i < 10; i++)
			{
				send_data.rx_buff[33 + i] = SystemParaMenu2.ProduceDATA[i];
			}
			for (i = 0; i < 11; i++)
			{
				send_data.rx_buff[43 + i] = SystemParaMenu2.DetectorDATA[i];
			}
			if (SystemParaMenu2.exDetectorDATA[0] == DETECTOR_NEW_VERSION_1 && SystemParaMenu2.exDetectorDATA[1] == DETECTOR_NEW_VERSION_2)
			{
				send_data.rx_buff[4] = 0x3A;
				for (i = 0; i < 7; i++)
				{
					send_data.rx_buff[54 + i] = SystemParaMenu2.exDetectorDATA[i + 2];
				}
			}
		}
		else
		{
			MessageReturnState(CommandNotSupported);
			return;
		}
	}
	break;

	case 0x80: // 坏点功能开关
	{
		if (databuff[8] == 0xAA) // 设置
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x08;
			send_data.rx_buff[8] = databuff[8];
			if (databuff[9] == 0x00) // 设置坏点功能开关
			{
				SystemParaMenu2.BAD_SW = databuff[10] & 0x01;
				if (SystemParaMenu2.BAD_SW == 0)
				{
					ISP_bpc_off();
				}
				else
				{
					ISP_bpc_on();
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.BAD_SW;
			}
			else if (databuff[9] == 0x01) // BPC_Mode
			{
				SystemParaMenu2.m_bpc_ref_mode = databuff[10] & 0x01;
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_bpc_ref_mode;
			}
			else if (databuff[9] == 0x02) // BPC_cfg
			{
				SystemParaMenu2.m_bpc_cfg = databuff[10];
				if (SystemParaMenu2.m_bpc_cfg >= 4)
					SystemParaMenu2.m_bpc_cfg = 4;
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_bpc_cfg;
			}
			else if (databuff[9] == 0x03) // BPC_view
			{
				SystemParaMenu2.m_bpc_view = databuff[10] & 0x01;
				if (SystemParaMenu2.m_bpc_view == 0)
				{
					ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3, SystemParaMenu2.m_bpc_view, 1);
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
					cvbs_osd_refresh_display(1);
				}
				else
				{
					ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3, SystemParaMenu2.m_bpc_view, 1);
					SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
					cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);
					cvbs_osd_refresh_display(1);
				}
				send_data.rx_buff[9] = databuff[9];
				send_data.rx_buff[10] = SystemParaMenu2.m_bpc_view;
			}
		}
		else if (databuff[8] == 0xFF) // 查询
		{
			send_data.rx_buff[3] = 0x00;
			send_data.rx_buff[4] = 0x0A;
			send_data.rx_buff[8] = databuff[8];

			send_data.rx_buff[9] = SystemParaMenu2.BAD_SW;
			send_data.rx_buff[10] = SystemParaMenu2.m_bpc_ref_mode;
			send_data.rx_buff[11] = SystemParaMenu2.m_bpc_cfg;
			send_data.rx_buff[12] = SystemParaMenu2.m_bpc_view;
		}
	}
	break;
	}
	Return_Message_Processing();
	memset(send_data.rx_buff, 0, sizeof(send_data.rx_buff));
}

/* 增加导入导出功能 */
#define RETRY_TIME 3U
#define PART_FRAME_LEN 1024U
#define CORE_DATA_BUFF_SIZE (1024 * 1024 * 3)
#define CORE_DATA_ALIGN 32
#define CORE_DATA_NAME "inout_core_data"
#define PACKET_DATA_SIZE 1024
/*---------------导入导出的测试程序----------------*/
static uint32_t ResetToZero_Size = 0;
void *Resetzero_buff;
/** @note 清菜单一 */
static int Reset_Menu1_flashData(void) {
	ts_s32 data_valid;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	if (s32_hfile_r > 0) {
		data_valid = 0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (Resetzero_buff + ResetToZero_Size), 1024);
		data_valid = 1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();

		ResetToZero_Size += 1024;
		vision_log_debug("[Reset]: Menu1 TotalSize=%ld, Current Menu1Size=%d\n", ResetToZero_Size, sizeof(UsermenuParameter));

		return TS_OK;
	}
	return TS_ERR;
}
/** @note 清菜单二 */
static int Reset_Menu2_flashData(void) {
	ts_s32 data_valid;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_APP_MENU2_CFG);
	if (s32_hfile_r > 0) {
		data_valid = 0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (Resetzero_buff + ResetToZero_Size), 1024);
		data_valid = 1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();

		ResetToZero_Size += 1024;
		vision_log_debug("[Reset]: Menu2 TotalSize=%ld, Current Menu2Size=%d\n", ResetToZero_Size, sizeof(SystemParaMenu2));
		return TS_OK;
	}

	return TS_ERR;
}
/** @note 清OCC */
static int Reset_OCC_flashData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	ts_buffer_s_t st_occ_buf;

	ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" occ info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, st_occ_buf.u32_width, st_occ_buf.u32_height);
		strcpy(st_sfc_file.ac_cfg_name, FN_OCC_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(Resetzero_buff + ResetToZero_Size), st_occ_buf.sz_size);
		if (cRet > 0) {
			ResetToZero_Size += st_occ_buf.sz_size;
			u32_cfg_flag = 1;
			vision_log_debug("[Reset]: TotalSize=%ld, sz_size=%d\n", ResetToZero_Size, st_occ_buf.sz_size);
		}
		else {
			vision_log_debug("[Reset]: RST OCC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 官方NUC K 总表 */
static int Reset_NUC_K_flashData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" nuc K info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_K_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(Resetzero_buff + ResetToZero_Size), pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
		if (cRet > 0) {
			ResetToZero_Size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
			vision_log_debug("[Reset]: ToatalSize=%ld, sz_size=%d\n", ResetToZero_Size, pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
			u32_cfg_flag = 1;
		}
		else {
			vision_log_debug("[Reset]: RST NUC K Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note VOT NUC坏表 */
static int Reset_NUC_flashData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" nuc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_VOT_NUC_KB_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(Resetzero_buff + ResetToZero_Size), vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
		if (cRet > 0) {
			ResetToZero_Size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
			u32_cfg_flag = 1;
			vision_log_debug("[Reset]: TotalSize=%ld, sz_size=%d\n", ResetToZero_Size, vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
		}
		else {
			vision_log_debug("[Reset]: RST NUC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}

	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 清SFFC */
static ts_s32 Reset_SFFC_flashData(void)
{
	ts_s32 cRet;
	ts_u32 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" sffc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_SFFC_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		ts_u32 u32_size = pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(Resetzero_buff + ResetToZero_Size), u32_size);
		if (cRet > 0) {
			ResetToZero_Size += u32_size;
			vision_log_debug("[Reset]: TotalSize=%ld, u32_size=%d\n", ResetToZero_Size, u32_size);
			u32_cfg_flag =  1;
		}
		else {
			vision_log_debug("[Reset]: Write SFFC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/*----------------导入导出功能程序--------------------*/
uint16_t crc16_cal_ex(uint16_t u16Poly, uint16_t u16Cs, const uint8_t *pu8Buf, uint32_t u32Len)
{
	uint32_t i, j;
	uint16_t u16Tmp;

	for (i = 0; i < u32Len; i++)
	{
		u16Cs ^= (uint16_t)pu8Buf[i];
		for (j = 0; j < 8; j++)
		{
			u16Tmp = (unsigned int)(u16Cs & 0x0001U);
			u16Cs >>= 1U;
			if (u16Tmp == 1U)
			{
				u16Cs ^= u16Poly; // 0xA001U;//多项式，可以更换的
			}
		}
	}
	return u16Cs;
}

uint16_t crc16_cal(uint16_t u16Poly, const uint8_t *pu8Buf, uint32_t u32Len)
{
	uint16_t u16Trialfcs;
	u16Trialfcs = crc16_cal_ex(u16Poly, 0xFFFFU, pu8Buf, u32Len);
	return u16Trialfcs;
}

/** @note 写入OCC数据 */
static int Import_Write_OCC_CoreData(void) {
	ts_s32 cRet;
	ts_buffer_s_t st_occ_buf;
	ts_u08 u32_cfg_flag = 0;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	//
	ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);
	sfc_cfg_file_s_t st_sfc_file;
	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug("occ info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, st_occ_buf.u32_width, st_occ_buf.u32_height);
		strcpy(st_sfc_file.ac_cfg_name, FN_OCC_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		//
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(p_buff_Start + g_stParaMsgSwap.Import_size), st_occ_buf.sz_size);
		if (cRet > 0) {
			g_stParaMsgSwap.Import_size += st_occ_buf.sz_size;
			u32_cfg_flag = 1;
			vision_log_debug("[Import]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.Import_size, st_occ_buf.sz_size);
		}
		else {
			vision_log_debug("[Import]: Write OCC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 读取OCC数据 */
static ts_s32 Export_Read_OCC_CoreData(void)
{
	ts_s32 cRet;
	ts_buffer_s_t st_occ_buf;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 u32_cfg_flag = 0;

	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & 0x01)) {
			ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);
            vision_log_debug("occ info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
                st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, st_occ_buf.u32_width, st_occ_buf.u32_height);
			cRet = ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(p_buff_Start + g_stParaMsgSwap.File_size), st_occ_buf.sz_size);
			if (cRet > 0) {
				g_stParaMsgSwap.File_size += st_occ_buf.sz_size;
				vision_log_debug("[Export]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.File_size, st_occ_buf.sz_size);
				u32_cfg_flag = 1;
			}
			else {
				vision_log_debug("[Export]: Read OCC Data ERROR\n");
			}
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 写入NUC K 表 */
static int Import_Write_NUC_K_flashData(void) {
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;

	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" nuc K info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_K_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(p_buff_Start + g_stParaMsgSwap.Import_size), pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
		if (cRet > 0) {
			g_stParaMsgSwap.Import_size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
			vision_log_debug("[Import]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.Import_size, pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
			u32_cfg_flag = 1;
		}
		else {
			vision_log_debug("[Import]:  Write NUC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 写入NUC 坏表 */
static int Import_WriteNUC_CoreData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" nuc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_VOT_NUC_KB_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(p_buff_Start + g_stParaMsgSwap.Import_size), vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
		if (cRet > 0) {
			g_stParaMsgSwap.Import_size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
			u32_cfg_flag = 1;
			vision_log_debug("[Import]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.Import_size, vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
		}
		else {
			vision_log_debug("[Import]: Write NUC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 读取NUC K表 */
static ts_s32 Export_Read_NUC_K_flashData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug("nuc K info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & 0x01)) {
			cRet = ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), p_buff_Start + g_stParaMsgSwap.File_size, pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
			if (cRet > 0) {
				g_stParaMsgSwap.File_size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
				u32_cfg_flag = 1;
				vision_log_debug("[Export]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.File_size, pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
			}
		}
		else {
			vision_log_debug("[Export]: Read NUC K u32_valid_code != SFC_FILE_VALID_CODE\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 读取NUC 坏点 */
static ts_s32 Export_Read_NUC_CoreData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" nuc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & 0x01)) {
			cRet = ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), p_buff_Start + g_stParaMsgSwap.File_size, vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
			if (cRet > 0) {
				g_stParaMsgSwap.File_size += pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
				u32_cfg_flag = 1;
				vision_log_debug("[Export]: TotalSize=%ld, sz_size=%d\n", g_stParaMsgSwap.File_size, vot_get_fn_vot_nuc_kb_table_size(pst_attr->s32_img_w, pst_attr->s32_img_h));
			}
		}
		else {
			vision_log_debug("[Export]: Read NUC u32_valid_code != SFC_FILE_VALID_CODE\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 写入SFFC数据 */
static ts_s32 Import_WriteSFFC_CoreData(void)
{
	ts_s32 cRet;
	ts_u08 u32_cfg_flag = 0;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	snr_attr_s_t *pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug(" sffc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		strcpy(st_sfc_file.ac_cfg_name, FN_SFFC_CFG);
		st_sfc_file.u32_cfg_num = 1;
		st_sfc_file.u32_valid_flag = 1;
		st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		ts_u32 u32_size = pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
		cRet = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)(p_buff_Start + g_stParaMsgSwap.Import_size), u32_size);
		if (cRet > 0) {
			g_stParaMsgSwap.Import_size += u32_size;
			vision_log_debug("[Import]: TotalSize=%ld, u32_size=%d\n", g_stParaMsgSwap.Import_size, u32_size);
			u32_cfg_flag = 1;
		}
		else {
			vision_log_debug("[Import]: Write SFFC Data ERROR\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}
	return TS_OK;
}
/** @note 读取SFFC数据 */
static ts_s32 Export_Read_SFFC_CoreData(void)
{
	ts_s32 s32_hfile_r, cRet;
	snr_attr_s_t *pst_attr;
	sfc_cfg_file_s_t st_sfc_file;
	ts_u08 u32_cfg_flag = 0;

	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;

	s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
	ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
	if (s32_hfile_r > 0) {
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
		vision_log_debug("sffc info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
			st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, pst_attr->s32_img_w, pst_attr->s32_img_h);
		if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & 0x01)) {
			ts_u32 u32_size = pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
			cRet = ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), p_buff_Start + g_stParaMsgSwap.File_size, u32_size);
			if (cRet > 0) {
				g_stParaMsgSwap.File_size += u32_size;
				vision_log_debug("[Export]: TotalSize=%ld, u32_size=%d\n", g_stParaMsgSwap.File_size, u32_size);
				u32_cfg_flag = 1;
			}
		}
		else {
			vision_log_debug("[Export]: Read SFFC u32_valid_code != SFC_FILE_VALID_CODE\n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if (u32_cfg_flag == 0) {
		return TS_ERR;
	}

	return TS_OK;
}
/** @note 写菜单一 */
static int OPR_WriteMenu1_CoreData(void) {
	ts_s32 data_valid;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;

	g_stParaMsgSwap.Import_size = 0;
	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	if (s32_hfile_r > 0) {
		data_valid = 0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (p_buff_Start + g_stParaMsgSwap.Import_size), 1024);
		data_valid = 1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();

		g_stParaMsgSwap.Import_size += 1024;
		vision_log_debug("[Import]: Menu1 TotalSize=%ld, Current Menu1Size=%d\n", g_stParaMsgSwap.Import_size, sizeof(UsermenuParameter));

		return TS_OK;
	}

	return TS_ERR;
}
/** @note 写菜单二 */
static int OPR_WriteMenu2_CoreData(void) {
	ts_s32 data_valid;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;

	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_APP_MENU2_CFG);
	if (s32_hfile_r > 0) {
		data_valid = 0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (p_buff_Start + g_stParaMsgSwap.Import_size), 1024);
		data_valid = 1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08 *)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();

		g_stParaMsgSwap.Import_size += 1024;
		vision_log_debug("[Import]: Menu2 TotalSize=%ld, Current Menu2Size=%d\n", g_stParaMsgSwap.Import_size, sizeof(SystemParaMenu2));
		return TS_OK;
	}

	return TS_ERR;
}
/* 读出菜单一，菜单二的数据
 * 返回值含义如下：
 * -1：申请内存异常
 * -2：read数据异常/数据无效，需释放内存
 */
int OPR_ReadMenu1_2_CoreData(void)
{
	ts_s32 read_len = 0;
	ts_s32 data_valid_menu1 = 0;
	ts_s32 data_valid_menu2 = 0;

	/* 申请出3M内存 */
	p_temp_buff = ts_cpl_tmm_malloc(ts_null, CORE_DATA_BUFF_SIZE, CORE_DATA_ALIGN, CORE_DATA_NAME);
	if (ts_null == p_temp_buff) {
		vision_log_debug("out data malloc buff fail!\n");
		return -1;
	}
	/* 初始化参数传输管理器 */
	memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(ParaMsgSwap_S));
	memset((uint8_t *)p_temp_buff, 0x00, CORE_DATA_BUFF_SIZE);
	//菜单一参数保存
	ISP_write_sys_cfg();
	ISP_write_sys_menu2_cfg(SystemParaMenu2);
	// 
	g_stParaMsgSwap.m_core_data_buff = (ts_u32)p_temp_buff;
	ts_u08 *p_buff_Start = (ts_u08 *)g_stParaMsgSwap.m_core_data_buff;
	g_stParaMsgSwap.File_size = 0;

	/* 菜单1参数 */
	ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	if (s32_hfile_r > 0) {
		read_len = ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&data_valid_menu1), 4);
		if (read_len != 4)
		{
			ts_lib_sfc_close(s32_hfile_r);
			vision_log_debug("UsermenuParameter read data_valid fail (read = %d)\n", read_len);
			return -1;
		}
		if (data_valid_menu1 == 1)
		{
			/* 暂时不对读出的长度做限制，兼容增减菜单参数的情况 */
			ts_lib_sfc_read(s32_hfile_r, 4, p_buff_Start + g_stParaMsgSwap.File_size, 1024);
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	else
	{
		vision_log_debug("[Export]: open Menu 1 fail\n");
	}
	if (data_valid_menu1 != 1) {
		return -2;
	}
	g_stParaMsgSwap.File_size += 1024; // 预留长度1024字节
	vision_log_debug("[Export]:Menu1 TotalSize=%ld, Menu1Size=%d\n", g_stParaMsgSwap.File_size, sizeof(UsermenuParameter));
	/*菜单2参数*/
	s32_hfile_r = -1;
	s32_hfile_r = ts_lib_sfc_open(FN_APP_MENU2_CFG);
	if (s32_hfile_r > 0) {
		read_len = ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&data_valid_menu2), 4);
		if (read_len != 4)
		{
			ts_lib_sfc_close(s32_hfile_r);
			vision_log_debug("SystemParaMenu2 read data_valid fail read=%d\n", read_len);
			return -1;
		}
		if (data_valid_menu2 == 1) /*暂时不对读出的长度做限制，兼容增减菜单参数的情况*/
		{
			ts_lib_sfc_read(s32_hfile_r, 4, p_buff_Start + g_stParaMsgSwap.File_size, 1024);
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	else
	{
		vision_log_debug("[Export]: open Menu 2 fail\n");
	}
	if (data_valid_menu2 != 1) {
		return -3;
	}
	g_stParaMsgSwap.File_size += 1024; // 预留长度1024字节
	vision_log_debug("[Export]:Menu2 TotalSize=%ld, Menu2Size=%d\n", g_stParaMsgSwap.File_size, sizeof(SystemParaMenu2));

	if (Export_Read_OCC_CoreData() != TS_OK)
	{
		return -2;
	}
	if (Export_Read_NUC_CoreData() != TS_OK)
	{
		return -2;
	}
	if (Export_Read_SFFC_CoreData() != TS_OK)
	{
		return -2;
	}
	if (Export_Read_NUC_K_flashData() != TS_OK)
	{
		return -2;
	}
	return TS_OK;
}
/* 上位机请求下位机，上发菜单一，菜单二，OCC，NUC，SFFC等数据 */
static int gen_parameter(void)
{
	// int i;
	// uint32_t u32Tmp;
	// uint8_t u8TmpBuf_ARY[4U] = {0};

	if (OPR_ReadMenu1_2_CoreData() < 0) // 内存分配异常
	{
		send_data.rx_buff[9U] = 0xEE;
		Return_Message_Processing(); /* 发送异常数据包 */
		memset(send_data.rx_buff, 0, sizeof(send_data.rx_buff));
		return -1;
	}
	/* 按1024一帧内容计算需要的帧数，payload的校验值
	payload总长度是：机芯编码 + TestmenuParameterBuff + NUC + OOC + 3(标识，长度) + 3(标识，长度) + 4(标识，长度) + 4(标识，长度) + 2(校验)
	*/
	// g_stParaMsgSwap.u16FrameNum = (14U + TESTMENU_LENGTH + ((IR_X_pixel * IR_Y_pixel) * 3U) + 16U + 2U + 1023U) >> 10U;//还有机芯根本没有编码

	//	g_stParaMsgSwap.u16FrameNum = (14U + 3U + FLASH_SIZE + 4U + 2U + 1023U) >> 10U;//还有机芯根本没有编码
	/* 总帧数 */
	g_stParaMsgSwap.u16FrameNum = (14U + 3U + 4U + g_stParaMsgSwap.File_size + 2U + PACKET_DATA_SIZE - 1) / PACKET_DATA_SIZE;

	/* 机芯报文 */
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = 0xE0; // 机芯
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = 0x00;
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = 14U;
	memcpy((g_stParaMsgSwap.u8ParaBuf_ARY + g_stParaMsgSwap.u16FrameLen), SystemParaMenu2.MerchineSN, 14U);
	g_stParaMsgSwap.u16FrameLen += 14U;
	/* flash报文 */
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = 0xE1; // flash
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = (g_stParaMsgSwap.File_size >> 16U) & 0x00FFU;
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = (g_stParaMsgSwap.File_size >> 8U) & 0x00FFU;
	g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = g_stParaMsgSwap.File_size & 0x000000FFU;
	// u16FrameLen = 21
	// vision_log_debug("Upload: u16FrameNum = %d, u16FrameLen = %d\n", g_stParaMsgSwap.u16FrameNum, g_stParaMsgSwap.u16FrameLen);
	/* 求出Flash数据存储的偏移地址 */
	g_stParaMsgSwap.u32FrameOffset = PART_FRAME_LEN - g_stParaMsgSwap.u16FrameLen;
	//	read_flash(START_ADDR, (g_stParaMsgSwap.u8ParaBuf_ARY + g_stParaMsgSwap.u16FrameLen), g_stParaMsgSwap.u32FrameOffset);

	memcpy(g_stParaMsgSwap.u8ParaBuf_ARY + g_stParaMsgSwap.u16FrameLen, (ts_u08 *)g_stParaMsgSwap.m_core_data_buff, g_stParaMsgSwap.u32FrameOffset);
	g_stParaMsgSwap.u16FrameLen += g_stParaMsgSwap.u32FrameOffset;
	/* payload不包括分帧总量和当前分帧 */
	g_stParaMsgSwap.u16VerifyCheck = crc16_cal(0xA001U, g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen); // 先算出一段crc

	/* 分帧总量 */
	send_data.rx_buff[9U] = g_stParaMsgSwap.u16FrameNum >> 8U;
	send_data.rx_buff[10U] = g_stParaMsgSwap.u16FrameNum & 0x00FFU;
	/* 当前分帧 */
	send_data.rx_buff[11U] = g_stParaMsgSwap.u16FrameIndex >> 8U;
	send_data.rx_buff[12U] = g_stParaMsgSwap.u16FrameIndex & 0x00FFU;
	/* 本帧 */
	memcpy((send_data.rx_buff + 13U), g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen); // 做成1024定长
	// g_u16testVerify = crc16_cal(0xA001U, g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen);
	/* 本帧长 */
	send_data.rx_buff[3U] = (g_stParaMsgSwap.u16FrameLen + 10) >> 8;
	send_data.rx_buff[4U] = (g_stParaMsgSwap.u16FrameLen + 10) & 0x00FF;
	// send_data.rx_buff[len=1034], g_stParaMsgSwap.u16FrameLen = 1024
	// vision_log_debug("send_data.rx_buff[len=%d], g_stParaMsgSwap.u16FrameLen = %d\n", ((send_data.rx_buff[3U] << 8) + send_data.rx_buff[4U]), g_stParaMsgSwap.u16FrameLen);
	return (g_stParaMsgSwap.u16FrameLen + 10); //(Offset + g_stParaMsgSwap.u32FrameOffset);//返回报文长度
}

/* 生成后续帧
严格说来step的取值范围应该在0~3，对应编号，DET，NUC和OOC，因为发送缓存够大，编号和DET完全可以一帧
发出，实际上后续帧都是NUC和OOC，所以取值只会是2和3 */
int para_sequence(void)
{
	int Ret;
	/* 更新序列号 */
	g_stParaMsgSwap.u16FrameIndex++;
	if (g_stParaMsgSwap.u16FrameIndex >= g_stParaMsgSwap.u16FrameNum)
	{
		/* 发完了，或出错了 */
		memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
		return -1;
	}
	/* 分帧总量 */
	send_data.rx_buff[9U] = g_stParaMsgSwap.u16FrameNum >> 8U;
	send_data.rx_buff[10U] = g_stParaMsgSwap.u16FrameNum & 0x00FFU;
	/* 当前分帧 */
	send_data.rx_buff[11U] = g_stParaMsgSwap.u16FrameIndex >> 8U;
	send_data.rx_buff[12U] = g_stParaMsgSwap.u16FrameIndex & 0x00FFU;
	//
	Ret = (g_stParaMsgSwap.File_size - g_stParaMsgSwap.u32FrameOffset);
	// vision_log_debug("Ret = %d, g_stParaMsgSwap.File_size = %d, g_stParaMsgSwap.u32FrameOffset = %d\n", Ret, g_stParaMsgSwap.File_size, g_stParaMsgSwap.u32FrameOffset);
	// 分包
	if (Ret > PART_FRAME_LEN)
	{
		g_stParaMsgSwap.u16FrameLen = PART_FRAME_LEN; // 做成1024定长
		/* 下一帧的内容 */
		//		read_flash((START_ADDR + g_stParaMsgSwap.u32FrameOffset), g_stParaMsgSwap.u8ParaBuf_ARY , g_stParaMsgSwap.u16FrameLen);
		memcpy(g_stParaMsgSwap.u8ParaBuf_ARY, (ts_u08 *)g_stParaMsgSwap.m_core_data_buff + g_stParaMsgSwap.u32FrameOffset, g_stParaMsgSwap.u16FrameLen);
		memcpy((send_data.rx_buff + 13U), g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen);
		g_stParaMsgSwap.u16VerifyCheck = crc16_cal_ex(0xA001U, g_stParaMsgSwap.u16VerifyCheck, g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen);
		/* 更新偏移 */
		g_stParaMsgSwap.u32FrameOffset += g_stParaMsgSwap.u16FrameLen;
	}
	else // 已经不够一包
	{
		g_stParaMsgSwap.u16FrameLen = Ret;
		//		read_flash((START_ADDR + g_stParaMsgSwap.u32FrameOffset), g_stParaMsgSwap.u8ParaBuf_ARY , g_stParaMsgSwap.u16FrameLen);
		memcpy(g_stParaMsgSwap.u8ParaBuf_ARY, (ts_u08 *)g_stParaMsgSwap.m_core_data_buff + g_stParaMsgSwap.u32FrameOffset, g_stParaMsgSwap.u16FrameLen);

		g_stParaMsgSwap.u16VerifyCheck = crc16_cal_ex(0xA001U, g_stParaMsgSwap.u16VerifyCheck, g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen);
		/* 写入校验，这里FrameLen等于1023时，将无法写入校验位；校验位传输规则 */
		g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = g_stParaMsgSwap.u16VerifyCheck >> 8U;
		g_stParaMsgSwap.u8ParaBuf_ARY[g_stParaMsgSwap.u16FrameLen++] = g_stParaMsgSwap.u16VerifyCheck & 0x00FFU;
		memcpy((send_data.rx_buff + 13U), g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen);
	}
	/* 本帧长 */
	send_data.rx_buff[3U] = (g_stParaMsgSwap.u16FrameLen + 10U) >> 8U;
	send_data.rx_buff[4U] = (g_stParaMsgSwap.u16FrameLen + 10U) & 0x00FFU;
	// vision_log_debug("u16FrameIndex = %d, u16FrameLen = %d\n", g_stParaMsgSwap.u16FrameIndex, g_stParaMsgSwap.u16FrameLen);
	/* 发送后续帧 */
	return (g_stParaMsgSwap.u16FrameLen + 10U);
}
/* 重发上一帧 */
int para_resend(void)
{
	// int Ret = 0;

	/* 没有分帧报文 */
	if ((g_stParaMsgSwap.u16FrameNum == 0) || (g_stParaMsgSwap.u16FrameIndex >= g_stParaMsgSwap.u16FrameNum))
	{
		return -1;
	}

	send_data.rx_buff[3U] = (g_stParaMsgSwap.u16FrameLen + 10) >> 8;
	send_data.rx_buff[4U] = (g_stParaMsgSwap.u16FrameLen + 10) & 0x00FF;
	/* 分帧总量 */
	send_data.rx_buff[9U] = g_stParaMsgSwap.u16FrameNum >> 8U;
	send_data.rx_buff[10U] = g_stParaMsgSwap.u16FrameNum & 0x00FFU;
	/* 当前分帧 */
	send_data.rx_buff[11U] = g_stParaMsgSwap.u16FrameIndex >> 8U;
	send_data.rx_buff[12U] = g_stParaMsgSwap.u16FrameIndex & 0x00FFU;
	/* 本帧 */
	memcpy((send_data.rx_buff + 13U), g_stParaMsgSwap.u8ParaBuf_ARY, g_stParaMsgSwap.u16FrameLen); // 做成1024定长
	// vision_log_debug("u16FrameIndex = %d, u16FrameLen = %d\n", g_stParaMsgSwap.u16FrameIndex, g_stParaMsgSwap.u16FrameLen);
	/* 发送缓存的报文被别的地方清干净了，这里要重组 */
	return (g_stParaMsgSwap.u16FrameLen + 10);
}
/* 根据重发次数判断是否重发，停止 */
int para_error(void)
{
	/* 一不超过重发次数 */
	if (++g_stParaMsgSwap.u8RetryTime < RETRY_TIME)
		return 2; // 重传
	else
	{
		memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
		if (p_memrecv_buff != ts_null)
		{
			ts_cpl_tmm_free(p_memrecv_buff);
			p_memrecv_buff = ts_null;
			vision_log_debug("[Import] reSending: free memory\n");
		}
		return 3; // 停止
	}
}

/* 本函数假设参数写入报文必然是从选择的项按顺序传输到结尾的，且报文长度一般超过1000，
那种灵活的中间跳过某些项的操作是不会遇到的 */
int para_recv(const uint8_t *pu8Frame, uint16_t u16Len)
{
	// uint32_t u32Tmp;
	uint16_t u16_crcTmp;
	uint16_t u16Verify, u16Tmp, u16Offset = 0U;

	/* 从新开始接收报文 */
	if (g_stParaMsgSwap.u16FrameNum == 0U)
	{
		/* 清内容 */
		memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
		/* 确认总帧数 */
		g_stParaMsgSwap.u16FrameNum = pu8Frame[u16Offset++];
		g_stParaMsgSwap.u16FrameNum <<= 8U;
		g_stParaMsgSwap.u16FrameNum += pu8Frame[u16Offset++];
		/* 当前分帧 */
		g_stParaMsgSwap.u16FrameIndex = pu8Frame[u16Offset++];
		g_stParaMsgSwap.u16FrameIndex <<= 8U;
		g_stParaMsgSwap.u16FrameIndex += pu8Frame[u16Offset++];
		/* 判断首帧是否为第一帧 */
		if (g_stParaMsgSwap.u16FrameIndex != 0U)
		{
			/* 首帧的当前分帧不可能不是0 */
			vision_log_debug("ERROR: The first frame index is not the first frame\n");
			return 3; /* 返回错误 */
		}
		u16_crcTmp = u16Offset;
		/* 一般人从E0或E1开始发首帧数据，但不排除有人从E2或E3开始发首帧数据 */
		if (pu8Frame[u16Offset] == 0xE0)
		{
			u16Offset++;
			/* 机芯编号长度 */
			g_stParaMsgSwap.u32FrameLen = pu8Frame[u16Offset++];
			g_stParaMsgSwap.u32FrameLen <<= 8U;
			g_stParaMsgSwap.u32FrameLen += pu8Frame[u16Offset++];
			/* 写入编码 */
			memcpy(SystemParaMenu2.MerchineSN, (pu8Frame + u16Offset), g_stParaMsgSwap.u32FrameLen);
			u16Offset += g_stParaMsgSwap.u32FrameLen;
		}
		else
		{
			memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
			return para_error(); // 出错了，u8RetryTime赋值存在逻辑问题
		}
		//
		if (pu8Frame[u16Offset] == 0xE1)
		{
			u16Offset++;
			/* Flash数据长度 */
			g_stParaMsgSwap.u32FrameLen = pu8Frame[u16Offset++];
			g_stParaMsgSwap.u32FrameLen <<= 8U;
			g_stParaMsgSwap.u32FrameLen += pu8Frame[u16Offset++];
			g_stParaMsgSwap.u32FrameLen <<= 8U;
			g_stParaMsgSwap.u32FrameLen += pu8Frame[u16Offset++];
		}
		else
		{
			memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
			return para_error(); // 出错了，u8RetryTime赋值存在逻辑问题
		}
		/* flash全擦 */
		// 7.21. Chip Erase (CE) (60/C7H)
		//  GlobalUnlock();
		//  for (u16Verify = 0; u16Verify < 20; u16Verify++)
		//  	BlockErase(START_ADDR + (u16Verify << 16U));//256*256
		/* 暂不考虑报文错误的情况 */
		u16Tmp = u16Len - u16Offset - 2U;
		// u16Len = 1030, u16Offset = 25, u16Tmp = 1003, *(pu8Frame + u16_crcTmp) = 0xe0, u16_crcTmp = 4
		// vision_log_debug("u16Len = %d, u16Offset = %d, u16Tmp = %d, *(pu8Frame + u16_crcTmp) = 0x%x, u16_crcTmp = %d\n",u16Len, u16Offset, u16Tmp, *(pu8Frame + u16_crcTmp), u16_crcTmp);
		g_stParaMsgSwap.u16VerifyCheck = crc16_cal(0xA001U, (pu8Frame + u16_crcTmp), u16Len - u16_crcTmp - 2U); // 减去数据包最后的CRC校验

		g_stParaMsgSwap.m_core_data_buff = (ts_u32)p_memrecv_buff;
		// Flash FrameLen = 0x140000, first frame CRC = 0xa70b
		// vision_log_debug("Flash FrameLen = 0x%x, first frame CRC = 0x%x\n", g_stParaMsgSwap.u32FrameLen, g_stParaMsgSwap.u16VerifyCheck);
		// 剩下的直接写进flash
		memcpy((ts_u08 *)g_stParaMsgSwap.m_core_data_buff + g_stParaMsgSwap.u32FrameOffset, (uint8_t *)(pu8Frame + u16Offset), u16Tmp);
		g_stParaMsgSwap.u32FrameOffset += u16Tmp;
		/* 接收正常，准备接收下一帧 */
		g_stParaMsgSwap.u16FrameIndex++;
		g_stParaMsgSwap.u8RetryTime = 0U;

		return 1;
	}
	/* 已发过报文，或后续帧，或重传*/
	else
	{
		/* 总帧数 */
		u16Tmp = pu8Frame[u16Offset++];
		u16Tmp <<= 8U;
		u16Tmp += pu8Frame[u16Offset++];
		if (u16Tmp != g_stParaMsgSwap.u16FrameNum)
		{
			/* 总帧数对不上 */
			vision_log_debug("[Import] Data ERROR: Total frame count abnormal\n");
			return para_error();
		}
		/* 接收后续帧要考虑重传 */
		u16Tmp = pu8Frame[u16Offset++];
		u16Tmp <<= 8U;
		u16Tmp += pu8Frame[u16Offset++];

		if (u16Tmp != g_stParaMsgSwap.u16FrameIndex)
		{
			/* 帧序号对不上 */
			vision_log_debug("[Import] Data ERROR: Frame Number Abnormal (%d != %d)\n", u16Tmp, g_stParaMsgSwap.u16FrameIndex);
			return para_error();
		}
		/* 最后一帧，到这里收到的报文，全都是flash的内容，不会是别的 */
		if ((g_stParaMsgSwap.u16FrameIndex + 1U) == g_stParaMsgSwap.u16FrameNum)
		{
			/* 最后一帧，最后两字节是crc */
			// flash_programm((START_ADDR + g_stParaMsgSwap.u32FrameOffset), (uint8_t*)(pu8Frame + u16Offset), (u16Len - u16Offset - 2U));
			// GlobalLock();
			u16Verify = pu8Frame[u16Len - 4U];
			u16Verify <<= 8U;
			u16Verify += pu8Frame[u16Len - 3U];
			g_stParaMsgSwap.u16VerifyCheck = crc16_cal_ex(0xA001U, g_stParaMsgSwap.u16VerifyCheck, (pu8Frame + u16Offset), (u16Len - u16Offset - 4U));
			// vision_log_debug("pu8Frame[%d] = %d, PC send file crc value: u16Verify = 0x%x, g_stParaMsgSwap.u16VerifyCheck = 0x%x\n", u16Len-1, pu8Frame[u16Len-1], u16Verify, g_stParaMsgSwap.u16VerifyCheck);

			if (u16Verify != g_stParaMsgSwap.u16VerifyCheck)
			{
				memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
				if (p_memrecv_buff != ts_null)
				{
					ts_cpl_tmm_free(p_memrecv_buff);
					p_memrecv_buff = ts_null;
					vision_log_debug("[Import]: Receive ERROR, free memory\n");
				}
				return 3; // 接收或flash存储错误，停止
			}
			memcpy((ts_u08 *)g_stParaMsgSwap.m_core_data_buff + g_stParaMsgSwap.u32FrameOffset, (uint8_t *)(pu8Frame + u16Offset), (u16Len - u16Offset - 4U));

			/* 参数灌入的正确性不能只依靠传入报文，需要在全部参数传完后把整个flash数据再读出来看校验是否正确 */
			// g_stParaMsgSwap.u32FrameOffset = 0U;
			// while(g_stParaMsgSwap.u32FrameOffset < FLASH_SIZE)
			// {
			// 	u32Tmp = (FLASH_SIZE - g_stParaMsgSwap.u32FrameOffset);
			// 	if (u32Tmp > PART_FRAME_LEN)
			// 		u32Tmp = PART_FRAME_LEN;
			// 	read_flash((START_ADDR + g_stParaMsgSwap.u32FrameOffset), g_stParaMsgSwap.u8ParaBuf_ARY, u32Tmp);
			// 	g_stParaMsgSwap.u16VerifyCheck = crc16_cal_ex(0xA001U, g_stParaMsgSwap.u16VerifyCheck, g_stParaMsgSwap.u8ParaBuf_ARY, u32Tmp);
			// 	g_stParaMsgSwap.u32FrameOffset += u32Tmp;
			// }
			//
			// 写入flash
			if (OPR_WriteMenu1_CoreData() != TS_OK)
				return 3;
			if (OPR_WriteMenu2_CoreData() != TS_OK)
				return 3;
			if (Import_Write_OCC_CoreData() != TS_OK)
				return 3;
			if (Import_WriteNUC_CoreData() != TS_OK)
				return 3;
			if (Import_WriteSFFC_CoreData() != TS_OK)
				return 3;
			if (Import_Write_NUC_K_flashData() != TS_OK)
				return 3;
			
			// 睡眠，清内存
			memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
			if (p_memrecv_buff != ts_null)
			{
				ts_cpl_tmm_free(p_memrecv_buff);
				p_memrecv_buff = ts_null;
				vision_log_debug("[Import]: Transfer Complete, free memory\n");
			}
			return 4; // 全部接收正确 保存数据到flash
		}
		/* 中间帧 */
		else
		{
			g_stParaMsgSwap.u16VerifyCheck = crc16_cal_ex(0xA001U, g_stParaMsgSwap.u16VerifyCheck, (pu8Frame + u16Offset), (u16Len - u16Offset - 2U));
			/* 全部写进flash */
			//			flash_programm((START_ADDR + g_stParaMsgSwap.u32FrameOffset), (uint8_t*)(pu8Frame + u16Offset), (u16Len - u16Offset));
			memcpy((ts_u08 *)g_stParaMsgSwap.m_core_data_buff + g_stParaMsgSwap.u32FrameOffset, (uint8_t *)(pu8Frame + u16Offset), u16Len - u16Offset - 2U);
			g_stParaMsgSwap.u32FrameOffset += (u16Len - u16Offset - 2U);
			// vision_log_debug("u16Tmp = %d, g_stParaMsgSwap.u16FrameIndex = %d, g_stParaMsgSwap.u32FrameOffset = %d\n", u16Tmp, g_stParaMsgSwap.u16FrameIndex, g_stParaMsgSwap.u32FrameOffset);
			/* 接收正常，准备接收下一帧 */
			g_stParaMsgSwap.u16FrameIndex++;

			return 1;
		}
	}
}
/* 机芯参数操作指令解析 */
int parameter_operation(const uint8_t *pu8Data, uint16_t u16Len)
{
	int Ret = 0;
	uint16_t u16Offset = 0U;

	/* 向发送缓存里写执行内容格式 */
	send_data.rx_buff[8U] = pu8Data[0U]; // 比如0x55、0xCC
	// 解析数据
	switch (pu8Data[0U])
	{
	case 0xAAU:
	{
		switch (pu8Data[1U])
		{
		case 0x00U: // 请求数据
			Ret = gen_parameter();
			if (Ret <= 0)
			{
				if (p_temp_buff != ts_null)
				{
					ts_cpl_tmm_free(p_temp_buff); // 释放内存
					p_temp_buff = ts_null;
					vision_log_debug("Export: Request frame ERROR, Free memory\n");
				}
			}
			break;
		case 0x01U: // 请求下一帧
			Ret = para_sequence();
			if (Ret <= 0)
			{
				if (p_temp_buff != ts_null)
				{
					ts_cpl_tmm_free(p_temp_buff); // 释放内存
					p_temp_buff = ts_null;
					vision_log_debug("Export: Next frame ERROR, Free memory\n");
				}
			}
			break;
		case 0x02U: // 重传上一帧
			Ret = para_resend();
			if (Ret <= 0)
			{
				if (p_temp_buff != ts_null)
				{
					ts_cpl_tmm_free(p_temp_buff); // 释放内存
					p_temp_buff = ts_null;
					vision_log_debug("Export: Resend frame ERROR, Free memory\n");
				}
			}
			break;
		case 0x04U: // 传输完成
		case 0x03U: // 停止
			/* 清内容 */
			memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
			if (p_temp_buff != ts_null)
			{
				ts_cpl_tmm_free(p_temp_buff); // 释放内存
				p_temp_buff = ts_null;
				if (pu8Data[1U] == 4)
				{
					vision_log_debug("[Export]: Transfer Complete, Free memory\n");
				}
				else
				{
					vision_log_debug("[Export]: Transfer ERROR, Free memory\n");
				}
			}
			break;
		default:
			break;
		}
	}
	break;
	case 0x55U: // 上位机请求写入机芯参数，仅仅是个请求动作
	{
		if (pu8Data[1] == 0xE0U)
		{
			Ret = 0x07;
			send_data.rx_buff[3U] = 0x00;
			send_data.rx_buff[4U] = Ret;
			/* 匹配机芯编码 */
			u16Offset = pu8Data[2U];
			u16Offset <<= 8U;
			u16Offset += pu8Data[3U];

			/* rx_buff的第八位，数据段1的请求返回代码号 */
			send_data.rx_buff[9U] = 0x00U; /* 同意 */
			// 先不做机器设备限制
			//  if (memcmp ((pu8Data + 4U), SystemParaMenu2.MerchineSN, u16Offset) == 0U)
			//  {
			//  	/* 本机参数，可以接收 */
			//  	send_data.rx_buff[9U] = 0x00U;
			//  }
			//  else
			//  {
			//  	/* 不是本机参数，拒绝接收 */
			//  	send_data.rx_buff[9U] = 0xFFU;
			//  }

			/* 申请出3M内存 */
			if (p_memrecv_buff != ts_null)
			{
				ts_cpl_tmm_free(p_memrecv_buff);
				p_memrecv_buff = ts_null;
			}
			p_memrecv_buff = ts_cpl_tmm_malloc(ts_null, CORE_DATA_BUFF_SIZE, CORE_DATA_ALIGN, CORE_DATA_NAME);
			if (ts_null == p_memrecv_buff)
			{
				vision_log_debug("[Import]: Request malloc buff fail!\n");
				return TS_ERR;
			}
			memset((uint8_t *)p_memrecv_buff, 0x00, CORE_DATA_BUFF_SIZE);
			memset((uint8_t *)&g_stParaMsgSwap, 0x00, sizeof(g_stParaMsgSwap));
		}
		else
		{
			/* 非法报文 */
			return -1;
		}
	}
	break;
	case 0xCCU:
	{
		Ret = 0x07;
		send_data.rx_buff[3U] = 0x00;
		send_data.rx_buff[4U] = Ret;
		/* 报文处理结果， pu8Data + 1U值为机芯编号 */
		send_data.rx_buff[9U] = para_recv((pu8Data + 1U), (u16Len - 1U));
		// vision_log_debug("send_data.rx_buff[9U] = %d\n", send_data.rx_buff[9U]);
		/* 返回写入Flash成功 */
		if (send_data.rx_buff[9U] == 0x04) {
			Return_Message_Processing();
			ISP_pause();
			/* 软重启 */
			ts_cpl_fix_msleep(1000);
			vot_vpo_freeze();
			planck640c_stop();
			UpdatePromptInfoWithTimeMs("Rebooting...", 2000,VOT_LOG_TYPE_MENU2);// 提示即将重启
			cli_simple_run_command("reboot");// 参数导入成功后重启系统，使更改的参数生效
		}
	}
	break;
	case 0x56U:
	{ /* 56 4F 54 00 07 59 AC 7D 56 F7 71 35 */
		if (pu8Data[1] == 0xF7U)
		{
			// 申请内存
			Resetzero_buff = ts_cpl_tmm_malloc(ts_null, CORE_DATA_BUFF_SIZE, CORE_DATA_ALIGN, CORE_DATA_NAME);
			if (Resetzero_buff == ts_null)
			{
				vision_log_debug("[Reset]: Request malloc buff fail!\n");
				return TS_ERR;
			}
			ResetToZero_Size = 0;
			memset((uint8_t *)Resetzero_buff, 0x00, CORE_DATA_BUFF_SIZE);
			ts_cpl_msleep(100);
			// 执行清零
			Reset_Menu1_flashData();
			Reset_Menu2_flashData();
			Reset_OCC_flashData();
			Reset_NUC_flashData();
			Reset_SFFC_flashData();
			Reset_NUC_K_flashData();
			// 释放内存
			if (Resetzero_buff != ts_null)
			{
				ts_cpl_tmm_free(Resetzero_buff);
				Resetzero_buff = ts_null;
			}
			/* 软重启 */
			ts_cpl_fix_msleep(200);
			vot_vpo_freeze();
			planck640c_stop();
			UpdatePromptInfoWithTimeMs("Rebooting...", 2000,VOT_LOG_TYPE_MENU2);// 提示即将重启
			cli_simple_run_command("reboot");// 参数导入成功后重启系统，使更改的参数生效
		}
	}
	break;
	}
	return Ret;
}

void UART_OPR_AllData_Backup(u8 *Valid_data)
{
	uint16_t u16RawLen = (Valid_data[3] << 8) | Valid_data[4];
	uint16_t u16DataLen = (u16RawLen >= 5) ? (u16RawLen - 3) : 0;

	if (u16DataLen > 0 && parameter_operation(Valid_data + 8, u16DataLen) > 0)
	{
		Return_Message_Processing();
		memset(send_data.rx_buff, 0, sizeof(send_data.rx_buff));
	}
}
/**
 * @brief 温度采集线程函数
 * @description: 执行温度数据采集和处理的专用线程
 * @param pv_data 线程参数（可传递采集配置）
 * @return void
 */
#if 0
ts_void temperature_collection_thread(ts_void *pv_data)
{
	ts_cpl_timeval_s_t Tempcol_time_start, Tempcol_time_cur;   
    ts_cpl_gettimeofday(&Tempcol_time_start);
	ts_s32 s32_collect_time_static = VotParameters.Test_Flag * 1000; //15秒
    vision_log_debug("temperature_collection thread\n");
    
    // 线程主循环
    for(;;) {
		ts_cpl_gettimeofday(&Tempcol_time_cur);
        ts_s32 s32_Tempcoltime_diff = ((Tempcol_time_cur.tv_sec - Tempcol_time_start.tv_sec) * 1000000 + Tempcol_time_cur.tv_usec - Tempcol_time_start.tv_usec) / 1000;
	 	if(s32_Tempcoltime_diff >= s32_collect_time_static)
	 	{
			 ISP_cut_new();
			//  Test_Value_Send();
			 ts_cpl_gettimeofday(&Tempcol_time_start);
			// vision_log_debug("temperature_collection NOW....\n");
	 	}
		 ts_cpl_msleep(100);
   
    }
}
/**
 * @brief 启动温度采集线程
 * @description: 安全地创建温度采集线程
 * @return ts_s32 0=成功，-1=失败
 */
ts_s32 start_temperature_collection_thread(void)
{
	// ts_cpl_task_s_t g_temp_collect_thread_struct;
    // 检查线程是否已在运行
    // if (g_temp_collect_thread != NULL) {
    //     vision_log_warn("temperature_collection thread already running");
    //     return -1;
    // }
    
    // 创建线程
    // g_temp_collect_thread = ts_cpl_kthread_create(
    //     temperature_collection_thread,
    //     NULL,                           // 线程参数
    //     "TempCollect",                  // 线程名称
    //     19,                            // 线程优先级（中等）
    //     2*2048                           // 栈大小（4KB）
    // );
 	ts_u32 Ret = ts_cpl_kthread_create((ts_cpl_task_s_t *)&g_temp_collect_thread, (ts_cpl_kthread_t)temperature_collection_thread, ts_null, "TempCollect", 19, 2*2048);

    if (Ret != 0) {
        vision_log_warn("temperature_collection thread create fail\n");
        return -1;
    }
    
    g_temp_collect_thread_running = 1;
    vision_log_debug("temperature_collection thread create success\n");
    return 0;
}

/**
 * @brief 停止温度采集线程
 * @description: 安全地停止并销毁温度采集线程
 * @return ts_s32 0=成功，-1=失败
 */
ts_s32 stop_temperature_collection_thread(void)
{
    // if (g_temp_collect_thread == NULL) {
    //     vision_log_warn("Temperature_collection thread not running");
    //     return -1;
    // }
    // vision_log_debug("temperature_collection thread stop request\n");
    
    // 温和停止线程（等待线程自然退出）
    ts_cpl_kthread_destroy((ts_cpl_task_s_t *)&g_temp_collect_thread, 0);
    
    // 等待线程完全退出（最多等待3秒）
    ts_u32 timeout = 1000; // 3秒超时
    while (g_temp_collect_thread_running && timeout > 0) {
        ts_cpl_msleep(100);
        timeout -= 100;
    }
    
    // // 如果线程仍未退出，强制停止
    // if (g_temp_collect_thread_running) {
    //     vision_log_warn("Temperature_collection_Stop fail");
    //     ts_cpl_kthread_destroy(g_temp_collect_thread, 1);
    // }
    
    g_temp_collect_thread_running = 0;
    vision_log_debug("Temperature_collection thread stop success\n");
    return 0;
}
#endif

#include "cJSON/cJSON.h"
#include <ctype.h>
// 将十六进制字符转换为数值
int hexCharToInt(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1; // 非法字符
}

// 将十六进制字符串转换为unsigned char型数组
int hexStringToUCharArray(const char *hexString, unsigned char *result, int maxLength)
{
	int len = strlen(hexString);
	int resultIndex = 0;

	// 每两个十六进制字符组成一个unsigned char
	for (int i = 0; i < len && resultIndex < maxLength; i += 2)
	{
		// 检查是否是合法的十六进制字符
		if (!isxdigit((int)hexString[i]) || !isxdigit((int)hexString[i + 1]))
		{
			return -1; // 非法字符
		}

		// 转换两个字符为一个unsigned char
		int high = hexCharToInt(hexString[i]);
		int low = hexCharToInt(hexString[i + 1]);
		result[resultIndex++] = (unsigned char)((high << 4) | low);
	}

	return resultIndex; // 返回数组长度
}

// unsigned char型数组转换为将十六进制字符串
void unsignedCharArrayToHexString(const unsigned char *array, int len, char *hexString)
{
	int i;
	for (i = 0; i < len; i++)
	{
		// 每次写入2个字符（1个十六进制数字和'\0'）
		sprintf(hexString + i * 2, "%02X", array[i]);
	}
	// 确保字符串以null终止
	hexString[len * 2] = '\0';
}

/**
 * @brief:导入解析JSON字符串
 * @parameter:json_str
 * @return:0-success 1-failure
 */
ts_u08 ParseJsonParameters(const char *json_str) // 解析JSON字符串，提取参数值并进行相应处理
{
	// 使用cJSON解析JSON字符串
	cJSON *root = cJSON_Parse(json_str);
	if (root == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
		{
			vision_log_debug("JSON Parse Error: %s\n", error_ptr);
		}
		return 1;
	}
	
	struct _Usermenu_Parameter UsermenuDefParam;
	struct Menu2_Para SysMenuDefParam;
	struct _Usermenu_Parameter UserParamValid;
	struct Menu2_Para SysParamValid;

	if(UsermenuParameter.UseUserMenuDefParam != 1) // 如果是新机芯，从来没有导入JSON的用户菜单默认参数，则先初始化用户菜单默认参数为机芯原始默认值，再根据JSON导入的参数进行覆盖
	{
		menu1_parameters_init();// 初始化用户菜单默认参数为机芯原始默认值
		menu1_default_parameters_init();
		ISP_write_sys_cfg();
	}
    if(SystemParaMenu2.UseSysMenuDefParam != 1) // 如果是新机芯，从来没有导入JSON的系统菜单默认参数，则先初始化系统菜单默认参数为机芯原始默认值，再根据JSON导入的参数进行覆盖
	{
		menu2_parameters_init();// 初始化系统菜单默认参数为机芯原始默认值
		menu2_default_parameters_init();
		ISP_write_sys_menu2_cfg(SystemParaMenu2);
	}

	// 解析热成像机芯用户菜单默认配置参数
	cJSON *UserMenuDefaultParameters = cJSON_GetObjectItem(root, "UserMenuDefaultParameters");
	if (UserMenuDefaultParameters != NULL)
	{
		UsermenuDefParam.UseUserMenuDefParam = 1; // 标记正在使用导入JSON的用户菜单默认参数
		cJSON *UseUserMenuDefParam = cJSON_GetObjectItem(UserMenuDefaultParameters, "UseUserMenuDefParam");
		if (UseUserMenuDefParam != NULL)
		{
			//解析UseUserMenuDefParam参数是0还是1，0表示不使用导入的用户菜单默认参数，1表示使用导入的用户菜单默认参数
			UsermenuDefParam.UseUserMenuDefParam = (unsigned char)UseUserMenuDefParam->valueint;
		}
		//解析UseUserMenuDefParam参数是0,表示要还原为机芯原始默认值，1表示要使用导入的用户菜单默认参数
        if (UsermenuDefParam.UseUserMenuDefParam == 0)// 如果不使用JSON导入的用户菜单默认参数，则用户菜单默认参数有效标志位都清0
		{
          memset(&UserParamValid, 0, sizeof(struct _Usermenu_Parameter));//所有的用户菜单默认参数有效标志位都清0,用于清除导入的参数配置
		}
		UsermenuParameter.UseUserMenuDefParam = UsermenuDefParam.UseUserMenuDefParam; // 将解析的UseUserMenuDefParam参数赋值给全局变量UsermenuParameter.UseUserMenuDefParam

		cJSON *Bright = cJSON_GetObjectItem(UserMenuDefaultParameters, "Bright");
		if (Bright != NULL)
		{
			UsermenuDefParam.Bright = (unsigned char)Bright->valueint;// 解析用户菜单的亮度参数
			UserParamValid.Bright = 1;// 标记该参数有效
			UsermenuParameter.Bright = UsermenuDefParam.Bright;// 将解析的亮度参数赋值给全局变量UsermenuParameter.Bright
		}
		cJSON *Contrast = cJSON_GetObjectItem(UserMenuDefaultParameters, "Contrast");
		if (Contrast != NULL)
		{
			UsermenuDefParam.Contrast = (unsigned char)Contrast->valueint;
			UserParamValid.Contrast = 1;// 标记该参数有效
			UsermenuParameter.Contrast = UsermenuDefParam.Contrast;// 将解析的对比度参数赋值给全局变量UsermenuParameter.Contrast
		}
		cJSON *SFilter = cJSON_GetObjectItem(UserMenuDefaultParameters, "SFilter");
		if (SFilter != NULL)
		{
			UsermenuDefParam.SFilter = (unsigned char)SFilter->valueint;
			UserParamValid.SFilter = 1;// 标记该参数有效
			UsermenuParameter.SFilter = UsermenuDefParam.SFilter;// 将解析的锐度参数赋值给全局变量UsermenuParameter.SFilter
		}
		cJSON *enhance_level = cJSON_GetObjectItem(UserMenuDefaultParameters, "enhance_level");
		if (enhance_level != NULL)
		{
			UsermenuDefParam.enhance_level = (unsigned char)enhance_level->valueint;
			UserParamValid.enhance_level = 1;// 标记该参数有效
			UsermenuParameter.enhance_level = UsermenuDefParam.enhance_level;// 将解析的增强等级参数赋值给全局变量UsermenuParameter.enhance_level
		}
		cJSON *Palette = cJSON_GetObjectItem(UserMenuDefaultParameters, "Palette");
		if (Palette != NULL)
		{
			UsermenuDefParam.Palette = (unsigned char)Palette->valueint;
			UserParamValid.Palette = 1;// 标记该参数有效
			UsermenuParameter.Palette = UsermenuDefParam.Palette;// 将解析的调色板参数赋值给全局变量UsermenuParameter.Palette
		}
		cJSON *Mirror = cJSON_GetObjectItem(UserMenuDefaultParameters, "Mirror");
		if (Mirror != NULL)
		{
			UsermenuDefParam.Mirror = (unsigned char)Mirror->valueint;
			UserParamValid.Mirror = 1;// 标记该参数有效
			UsermenuParameter.Mirror = UsermenuDefParam.Mirror;// 将解析的镜像参数赋值给全局变量UsermenuParameter.Mirror
		}
		cJSON *ShutterMode = cJSON_GetObjectItem(UserMenuDefaultParameters, "ShutterMode");
		if (ShutterMode != NULL)
		{
			UsermenuDefParam.ShutterMode = (unsigned char)ShutterMode->valueint;
			UserParamValid.ShutterMode = 1;// 标记该参数有效
			UsermenuParameter.ShutterMode = UsermenuDefParam.ShutterMode;// 将解析的快门模式参数赋值给全局变量UsermenuParameter.ShutterMode
		}
		cJSON *Enlarge = cJSON_GetObjectItem(UserMenuDefaultParameters, "Enlarge");
		if (Enlarge != NULL)
		{
			UsermenuDefParam.Enlarge = (unsigned char)Enlarge->valueint;
			UserParamValid.Enlarge = 1;// 标记该参数有效
			UsermenuParameter.Enlarge = UsermenuDefParam.Enlarge;// 将解析的放大参数赋值给全局变量UsermenuParameter.Enlarge
		}
		cJSON *EE_Level = cJSON_GetObjectItem(UserMenuDefaultParameters, "EE_Level");
		if (EE_Level != NULL)
		{
			UsermenuDefParam.EE_Level = (unsigned char)EE_Level->valueint;
			UserParamValid.EE_Level = 1;// 标记该参数有效
			UsermenuParameter.EE_Level = UsermenuDefParam.EE_Level;// 将解析的EE_Level参数赋值给全局变量UsermenuParameter.EE_Level
		}
		cJSON *DDE_Level = cJSON_GetObjectItem(UserMenuDefaultParameters, "DDE_Level");
		if (DDE_Level != NULL)
		{
			UsermenuDefParam.DDE_Level = (unsigned char)DDE_Level->valueint;
			UserParamValid.DDE_Level = 1;// 标记该参数有效
			UsermenuParameter.DDE_Level = UsermenuDefParam.DDE_Level;// 将解析的DDE_Level参数赋值给全局变量UsermenuParameter.DDE_Level
		}
		cJSON *AutoShutter = cJSON_GetObjectItem(UserMenuDefaultParameters, "AutoShutter");
		if (AutoShutter != NULL)
		{
			UsermenuDefParam.AutoShutter = (unsigned char)AutoShutter->valueint;
			UserParamValid.AutoShutter = 1;// 标记该参数有效
			UsermenuParameter.AutoShutter = UsermenuDefParam.AutoShutter;// 将解析的自动快门参数赋值给全局变量UsermenuParameter.AutoShutter
		}
		cJSON *IsEnableEnligh = cJSON_GetObjectItem(UserMenuDefaultParameters, "IsEnableEnligh");
		if (IsEnableEnligh != NULL)
		{
			UsermenuDefParam.IsEnableEnligh = (unsigned char)IsEnableEnligh->valueint;
			UserParamValid.IsEnableEnligh = 1;// 标记该参数有效
			UsermenuParameter.IsEnableEnligh = UsermenuDefParam.IsEnableEnligh;// 将解析的启用增强参数赋值给全局变量UsermenuParameter.IsEnableEnligh
		}
		cJSON *m_dev_env_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_dev_env_mode");
		if (m_dev_env_mode != NULL)
		{
			UsermenuDefParam.m_dev_env_mode = (unsigned char)m_dev_env_mode->valueint;
			UserParamValid.m_dev_env_mode = 1;// 标记该参数有效
			UsermenuParameter.m_dev_env_mode = UsermenuDefParam.m_dev_env_mode;// 将解析的设备环境模式参数赋值给全局变量UsermenuParameter.m_dev_env_mode
		}
		cJSON *ImageMode = cJSON_GetObjectItem(UserMenuDefaultParameters, "ImageMode");
		if (ImageMode != NULL)
		{
			UsermenuDefParam.ImageMode = (unsigned char)ImageMode->valueint;
			UserParamValid.ImageMode = 1;// 标记该参数有效
			UsermenuParameter.ImageMode = UsermenuDefParam.ImageMode;// 将解析的图像模式参数赋值给全局变量UsermenuParameter.ImageMode
		}
		cJSON *ImageAdjust = cJSON_GetObjectItem(UserMenuDefaultParameters, "ImageAdjust");
		if (ImageAdjust != NULL)
		{
			UsermenuDefParam.ImageAdjust = (unsigned char)ImageAdjust->valueint;
			UserParamValid.ImageAdjust = 1;// 标记该参数有效
			UsermenuParameter.ImageAdjust = UsermenuDefParam.ImageAdjust;// 将解析的图像调整参数赋值给全局变量UsermenuParameter.ImageAdjust
		}
		cJSON *CaptureFlag = cJSON_GetObjectItem(UserMenuDefaultParameters, "CaptureFlag");
		if (CaptureFlag != NULL)
		{
			UsermenuDefParam.CaptureFlag = (unsigned char)CaptureFlag->valueint;
			UserParamValid.CaptureFlag = 1;// 标记该参数有效
			UsermenuParameter.CaptureFlag = UsermenuDefParam.CaptureFlag;// 将解析的捕获标志参数赋值给全局变量UsermenuParameter.CaptureFlag
		}
		cJSON *FontColor = cJSON_GetObjectItem(UserMenuDefaultParameters, "FontColor");
		if (FontColor != NULL)
		{
			UsermenuDefParam.FontColor = (unsigned char)FontColor->valueint;
			UserParamValid.FontColor = 1;// 标记该参数有效
			UsermenuParameter.FontColor = UsermenuDefParam.FontColor;// 将解析的字体颜色参数赋值给全局变量UsermenuParameter.FontColor
		}
		cJSON *pause_ctl = cJSON_GetObjectItem(UserMenuDefaultParameters, "pause_ctl");
		if (pause_ctl != NULL)
		{
			UsermenuDefParam.pause_ctl = (unsigned char)pause_ctl->valueint;
			UserParamValid.pause_ctl = 1;// 标记该参数有效
			UsermenuParameter.pause_ctl = UsermenuDefParam.pause_ctl;// 将解析的暂停控制参数赋值给全局变量UsermenuParameter.pause_ctl
		}
		cJSON *Menu1ParaLength = cJSON_GetObjectItem(UserMenuDefaultParameters, "Menu1ParaLength");
		if (Menu1ParaLength != NULL)
		{
			UsermenuDefParam.Menu1ParaLength = (unsigned char)Menu1ParaLength->valueint;
			UserParamValid.Menu1ParaLength = 1;// 标记该参数有效
			UsermenuParameter.Menu1ParaLength = UsermenuDefParam.Menu1ParaLength;// 将解析的菜单1参数长度参数赋值给全局变量UsermenuParameter.Menu1ParaLength
		}
		cJSON *SunPT_SW = cJSON_GetObjectItem(UserMenuDefaultParameters, "SunPT_SW");
		if (SunPT_SW != NULL)
		{
			UsermenuDefParam.SunPT_SW = (unsigned char)SunPT_SW->valueint;
			UserParamValid.SunPT_SW = 1;// 标记该参数有效
			UsermenuParameter.SunPT_SW = UsermenuDefParam.SunPT_SW;// 将解析的太阳位置跟踪开关参数赋值给全局变量UsermenuParameter.SunPT_SW
		}
		cJSON *P_Time = cJSON_GetObjectItem(UserMenuDefaultParameters, "P_Time");
		if (P_Time != NULL)
		{
			UsermenuDefParam.P_Time = (unsigned char)P_Time->valueint;
			UserParamValid.P_Time = 1;// 标记该参数有效
			UsermenuParameter.P_Time = UsermenuDefParam.P_Time;// 将解析的P_Time参数赋值给全局变量UsermenuParameter.P_Time
		}
		cJSON *Quad_SW = cJSON_GetObjectItem(UserMenuDefaultParameters, "Quad_SW");
		if (Quad_SW != NULL)
		{
			UsermenuDefParam.Quad_SW = (unsigned char)Quad_SW->valueint;
			UserParamValid.Quad_SW = 1;// 标记该参数有效
			UsermenuParameter.Quad_SW = UsermenuDefParam.Quad_SW;// 将解析的Quad_SW参数赋值给全局变量UsermenuParameter.Quad_SW
		}
		cJSON *SystemTimeCounter = cJSON_GetObjectItem(UserMenuDefaultParameters, "SystemTimeCounter");
		if (SystemTimeCounter != NULL)
		{
			UsermenuDefParam.SystemTimeCounter = (unsigned char)SystemTimeCounter->valueint;
			UserParamValid.SystemTimeCounter = 1;// 标记该参数有效
			UsermenuParameter.SystemTimeCounter = UsermenuDefParam.SystemTimeCounter;// 将解析的系统时间计数器参数赋值给全局变量UsermenuParameter.SystemTimeCounter
		}
		cJSON *ShutterCounter = cJSON_GetObjectItem(UserMenuDefaultParameters, "ShutterCounter");
		if (ShutterCounter != NULL)
		{
			UsermenuDefParam.ShutterCounter = (unsigned char)ShutterCounter->valueint;
			UserParamValid.ShutterCounter = 1;// 标记该参数有效
			UsermenuParameter.ShutterCounter = UsermenuDefParam.ShutterCounter;// 将解析的快门计数器参数赋值给全局变量UsermenuParameter.ShutterCounter
		}
		cJSON *ContinuousZoom = cJSON_GetObjectItem(UserMenuDefaultParameters, "ContinuousZoom");
		if (ContinuousZoom != NULL)
		{
			UsermenuDefParam.ContinuousZoom = (unsigned char)ContinuousZoom->valueint;
			UserParamValid.ContinuousZoom = 1;// 标记该参数有效
			UsermenuParameter.ContinuousZoom = UsermenuDefParam.ContinuousZoom;// 将解析的连续变焦参数赋值给全局变量UsermenuParameter.ContinuousZoom
		}
		cJSON *HY_ShutterTime1 = cJSON_GetObjectItem(UserMenuDefaultParameters, "HY_ShutterTime1");
		if (HY_ShutterTime1 != NULL)
		{
			UsermenuDefParam.HY_ShutterTime1 = (unsigned char)HY_ShutterTime1->valueint;
			UserParamValid.HY_ShutterTime1 = 1;// 标记该参数有效
			UsermenuParameter.HY_ShutterTime1 = UsermenuDefParam.HY_ShutterTime1;// 将解析的HY_ShutterTime1参数赋值给全局变量UsermenuParameter.HY_ShutterTime1
		}
		cJSON *HY_ShutterTime2 = cJSON_GetObjectItem(UserMenuDefaultParameters, "HY_ShutterTime2");
		if (HY_ShutterTime2 != NULL)
		{
			UsermenuDefParam.HY_ShutterTime2 = (unsigned char)HY_ShutterTime2->valueint;
			UserParamValid.HY_ShutterTime2 = 1;// 标记该参数有效
			UsermenuParameter.HY_ShutterTime2 = UsermenuDefParam.HY_ShutterTime2;// 将解析的HY_ShutterTime2参数赋值给全局变量UsermenuParameter.HY_ShutterTime2
		}
		cJSON *HY_ShutterTime3 = cJSON_GetObjectItem(UserMenuDefaultParameters, "HY_ShutterTime3");
		if (HY_ShutterTime3 != NULL)
		{
			UsermenuDefParam.HY_ShutterTime3 = (unsigned char)HY_ShutterTime3->valueint;
			UserParamValid.HY_ShutterTime3 = 1;// 标记该参数有效
			UsermenuParameter.HY_ShutterTime3 = UsermenuDefParam.HY_ShutterTime3;// 将解析的HY_ShutterTime3参数赋值给全局变量UsermenuParameter.HY_ShutterTime3
		}
		cJSON *CenterSpotSW = cJSON_GetObjectItem(UserMenuDefaultParameters, "CenterSpotSW");
		if (CenterSpotSW != NULL)
		{
			UsermenuDefParam.CenterSpotSW = (unsigned char)CenterSpotSW->valueint;
			UserParamValid.CenterSpotSW = 1;// 标记该参数有效
			UsermenuParameter.CenterSpotSW = UsermenuDefParam.CenterSpotSW;// 将解析的CenterSpotSW参数赋值给全局变量UsermenuParameter.CenterSpotSW
		}
		cJSON *manual_ImagePara_Bright = cJSON_GetObjectItem(UserMenuDefaultParameters, "manual_ImagePara.Bright");
		if (manual_ImagePara_Bright != NULL)
		{
			UsermenuDefParam.manual_ImagePara.Bright = (unsigned char)manual_ImagePara_Bright->valueint;
			UserParamValid.manual_ImagePara.Bright = 1;// 标记该参数有效
			UsermenuParameter.manual_ImagePara.Bright = UsermenuDefParam.manual_ImagePara.Bright;// 将解析的manual_ImagePara.Bright参数赋值给全局变量UsermenuParameter.manual_ImagePara.Bright
		}
		cJSON *manual_ImagePara_Contrast = cJSON_GetObjectItem(UserMenuDefaultParameters, "manual_ImagePara.Contrast");
		if (manual_ImagePara_Contrast != NULL)
		{
			UsermenuDefParam.manual_ImagePara.Contrast = (unsigned char)manual_ImagePara_Contrast->valueint;
			UserParamValid.manual_ImagePara.Contrast = 1;// 标记该参数有效
			UsermenuParameter.manual_ImagePara.Contrast = UsermenuDefParam.manual_ImagePara.Contrast;// 将解析的manual_ImagePara.Contrast参数赋值给全局变量UsermenuParameter.manual_ImagePara.Contrast
		}

		cJSON *m_pip_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_pip_mode");
		if (m_pip_mode != NULL)
		{
			UsermenuDefParam.m_pip_mode = (unsigned char)m_pip_mode->valueint;
			UserParamValid.m_pip_mode = 1;// 标记该参数有效
			UsermenuParameter.m_pip_mode = UsermenuDefParam.m_pip_mode;// 将解析的m_pip_mode参数赋值给全局变量UsermenuParameter.m_pip_mode
		}

		cJSON *m_hst_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_hst_mode");
		if (m_hst_mode != NULL)
		{
			UsermenuDefParam.m_hst_mode = (unsigned char)m_hst_mode->valueint;
			UserParamValid.m_hst_mode = 1;// 标记该参数有效
			UsermenuParameter.m_hst_mode = UsermenuDefParam.m_hst_mode;// 将解析的m_hst_mode参数赋值给全局变量UsermenuParameter.m_hst_mode
		}

		cJSON *m_luma_percent_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_luma_percent_mode");
		if (m_luma_percent_mode != NULL)
		{
			UsermenuDefParam.m_luma_percent_mode = (unsigned char)m_luma_percent_mode->valueint;
			UserParamValid.m_luma_percent_mode = 1;// 标记该参数有效
			UsermenuParameter.m_luma_percent_mode = UsermenuDefParam.m_luma_percent_mode;// 将解析的m_luma_percent_mode参数赋值给全局变量UsermenuParameter.m_luma_percent_mode
		}

		cJSON *m_contrast_percent_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_contrast_percent_mode");
		if (m_contrast_percent_mode != NULL)
		{
			UsermenuDefParam.m_contrast_percent_mode = (unsigned char)m_contrast_percent_mode->valueint;
			UserParamValid.m_contrast_percent_mode = 1;// 标记该参数有效
			UsermenuParameter.m_contrast_percent_mode = UsermenuDefParam.m_contrast_percent_mode;// 将解析的m_contrast_percent_mode参数赋值给全局变量UsermenuParameter.m_contrast_percent_mode
		}

		cJSON *m_rz_ratio = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_rz_ratio");
		if (m_rz_ratio != NULL)
		{
			UsermenuDefParam.m_rz_ratio = (unsigned char)m_rz_ratio->valueint;
			UserParamValid.m_rz_ratio = 1;// 标记该参数有效
			UsermenuParameter.m_rz_ratio = UsermenuDefParam.m_rz_ratio;// 将解析的m_rz_ratio参数赋值给全局变量UsermenuParameter.m_rz_ratio
		}

		// //m_SunPT_mode
		cJSON *m_SunPT_mode = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_SunPT_mode");
		if (m_SunPT_mode != NULL)
		{
			UsermenuDefParam.m_SunPT_mode = (unsigned char)m_SunPT_mode->valueint;
			UserParamValid.m_SunPT_mode = 1;// 标记该参数有效
			UsermenuParameter.m_SunPT_mode = UsermenuDefParam.m_SunPT_mode;// 将解析的m_SunPT_mode参数赋值给全局变量UsermenuParameter.m_SunPT_mode
		}

		cJSON *m_SunPT_gap = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_SunPT_gap");
		if (m_SunPT_gap != NULL)
		{
			UsermenuDefParam.m_SunPT_gap = (unsigned char)m_SunPT_gap->valueint;
			UserParamValid.m_SunPT_gap = 1;// 标记该参数有效
			UsermenuParameter.m_SunPT_gap = UsermenuDefParam.m_SunPT_gap;// 将解析的m_SunPT_gap参数赋值给全局变量UsermenuParameter.m_SunPT_gap
		}

		cJSON *m_menu1_info_tip_visible = cJSON_GetObjectItem(UserMenuDefaultParameters, "m_menu1_info_tip_visible");
		if (m_menu1_info_tip_visible != NULL)
		{
			UsermenuDefParam.m_menu1_info_tip_visible = (unsigned char)m_menu1_info_tip_visible->valueint;
			UserParamValid.m_menu1_info_tip_visible = 1;// 标记该参数有效
			UsermenuParameter.m_menu1_info_tip_visible = UsermenuDefParam.m_menu1_info_tip_visible;// 将解析的m_menu1_info_tip_visible参数赋值给全局变量UsermenuParameter.m_menu1_info_tip_visible
		}

	// 便于扩展，在此往后添加新变量....



		// 将已修改的用户菜单默认参数写入flash
		if (ISP_write_UserMenu_def_para_cfg(UsermenuDefParam) == 0)
		{
			//前512K空间存储JSON配置的用户菜单默认参数，偏移512K,后512K空间存储JSON配置的用户菜单默认参数是否有效标志位
			vision_log_debug("Write UsermenuParameter to flash success.\n");
			if(write_JsonUserMenuDefPara_valid_flag(UserParamValid)==0)// 将用户菜单默认参数有效标志位写入flash
			{
				ISP_write_sys_cfg();//保存当前已同步更新的用户菜单参数，以便机芯重启,更改的参数生效
				UpdatePromptInfoWithTimeMs("UserMenuDefPara write to flash success", 2000,VOT_LOG_TYPE_MENU2);
			}
			else
			{
				vision_log_debug("Write UsermenuParameter valid flag to flash failure.\n");
				UpdatePromptInfoWithTimeMs("UserMenuDefPara valid write failure", 2000,VOT_LOG_TYPE_MENU2);
				cJSON_Delete(root);
				return 1;// 用户菜单默认参数有效标志位写入flash失败，返回错误
			}
		}
		else
		{
			vision_log_debug("Write UsermenuParameter to flash failure.\n");
			UpdatePromptInfoWithTimeMs("UserMenuDefPara write to flash failure", 2000,VOT_LOG_TYPE_MENU2);
			cJSON_Delete(root);
			return 1;// 用户菜单默认参数写入flash失败，返回错误
		}
	}
	

	
	// 解析热成像机芯系统菜单默认配置参数
	cJSON *SystemMenuDefaultParameters = cJSON_GetObjectItem(root, "SystemMenuDefaultParameters");
	if (SystemMenuDefaultParameters != NULL)
	{
		SysMenuDefParam.UseSysMenuDefParam = 1; // 标记正在使用导入JSON的系统菜单默认参数
		cJSON *UseSysMenuDefParam = cJSON_GetObjectItem(SystemMenuDefaultParameters, "UseSysMenuDefParam");
		if (UseSysMenuDefParam != NULL)
		{
			//解析UseSysMenuDefParam参数是0还是其他，0表示不使用导入的系统菜单默认参数，1表示使用导入的系统菜单默认参数
			SysMenuDefParam.UseSysMenuDefParam = (unsigned char)UseSysMenuDefParam->valueint;
		}
		//解析UseSysMenuDefParam参数是0,表示要还原为机芯原始默认值，1表示要使用导入的系统菜单默认参数
        if (SysMenuDefParam.UseSysMenuDefParam == 0)// 如果不使用导入的系统菜单默认参数，则清0系统菜单默认参数有效标志位
		{
          memset(&SysParamValid, 0, sizeof(struct Menu2_Para));//所有的系统菜单默认参数有效标志位都清0,用于清除导入的参数配置
		}
		SystemParaMenu2.UseSysMenuDefParam = SysMenuDefParam.UseSysMenuDefParam;// 将解析的UseSysMenuDefParam参数赋值给全局变量SystemParaMenu2.UseSysMenuDefParam
		
		/* 测温功能-> 挡位切换 */
		cJSON *Usermenu_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Usermenu_SW");
		if (Usermenu_SW != NULL)
		{
			SysMenuDefParam.Usermenu_SW = (unsigned char)Usermenu_SW->valueint;
			SysParamValid.Usermenu_SW = 1;// 标记该参数有效
			SystemParaMenu2.Usermenu_SW = SysMenuDefParam.Usermenu_SW;// 将解析的Usermenu_SW参数赋值给全局变量SystemParaMenu2.Usermenu_SW
		}

		//菜单二Algorithm选项解析	
		cJSON *m_sffc_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_sffc_mode");
		if (m_sffc_mode != NULL)
		{
			SysMenuDefParam.m_sffc_mode = (unsigned char)m_sffc_mode->valueint;
			SysParamValid.m_sffc_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_sffc_mode = SysMenuDefParam.m_sffc_mode;// 将解析的m_sffc_mode参数赋值给全局变量SystemParaMenu2.m_sffc_mode
		}
	
		cJSON *m_sffc_level = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_sffc_level");
		if (m_sffc_level != NULL)
		{
			SysMenuDefParam.m_sffc_level = m_sffc_level->valueint;
			SysParamValid.m_sffc_level = 1;// 标记该参数有效
			SystemParaMenu2.m_sffc_level = SysMenuDefParam.m_sffc_level;// 将解析的m_sffc_level参数赋值给全局变量SystemParaMenu2.m_sffc_level
		}
		cJSON *m_osd_show_on_dev = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_osd_show_on_dev");
		if (m_osd_show_on_dev != NULL)
		{
			SysMenuDefParam.m_osd_show_on_dev = (unsigned char)m_osd_show_on_dev->valueint;
			SysParamValid.m_osd_show_on_dev = 1;// 标记该参数有效
			SystemParaMenu2.m_osd_show_on_dev = SysMenuDefParam.m_osd_show_on_dev;// 将解析的m_osd_show_on_dev参数赋值给全局变量SystemParaMenu2.m_osd_show_on_dev
		}
		cJSON *m_ee_enhance_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_ee_enhance_mode");
		if (m_ee_enhance_mode != NULL)
		{
			SysMenuDefParam.m_ee_enhance_mode = (unsigned char)m_ee_enhance_mode->valueint;
			SysParamValid.m_ee_enhance_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_ee_enhance_mode = SysMenuDefParam.m_ee_enhance_mode;// 将解析的m_ee_enhance_mode参数赋值给全局变量SystemParaMenu2.m_ee_enhance_mode
		}

		cJSON *m_fix_snc_switch = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_fix_snc_switch");
		if (m_fix_snc_switch != NULL)
		{
			SysMenuDefParam.m_fix_snc_switch = (unsigned char)m_fix_snc_switch->valueint;
			SysParamValid.m_fix_snc_switch = 1;// 标记该参数有效
			SystemParaMenu2.m_fix_snc_switch = SysMenuDefParam.m_fix_snc_switch;// 将解析的m_fix_snc_switch参数赋值给全局变量SystemParaMenu2.m_fix_snc_switch
		}

		cJSON *BAD_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "BAD_SW");
		if (BAD_SW != NULL)
		{
			SysMenuDefParam.BAD_SW = (unsigned char)BAD_SW->valueint;
			SysParamValid.BAD_SW = 1;// 标记该参数有效
			SystemParaMenu2.BAD_SW = SysMenuDefParam.BAD_SW;// 将解析的BAD_SW参数赋值给全局变量SystemParaMenu2.BAD_SW
		}
		cJSON *NUC_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NUC_SW");
		if (NUC_SW != NULL)
		{
			SysMenuDefParam.NUC_SW = (unsigned char)NUC_SW->valueint;
			SysParamValid.NUC_SW = 1;// 标记该参数有效
			SystemParaMenu2.NUC_SW = SysMenuDefParam.NUC_SW;// 将解析的NUC_SW参数赋值给全局变量SystemParaMenu2.NUC_SW
		}
		cJSON *m_bpc_cfg = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bpc_cfg");
		if (m_bpc_cfg != NULL)
		{
			SysMenuDefParam.m_bpc_cfg = (unsigned char)m_bpc_cfg->valueint;
			SysParamValid.m_bpc_cfg = 1;// 标记该参数有效
			SystemParaMenu2.m_bpc_cfg = SysMenuDefParam.m_bpc_cfg;// 将解析的m_bpc_cfg参数赋值给全局变量SystemParaMenu2.m_bpc_cfg
		}
		cJSON *m_bpc_view = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bpc_view");
		if (m_bpc_view != NULL)
		{
			SysMenuDefParam.m_bpc_view = (unsigned char)m_bpc_view->valueint;
			SysParamValid.m_bpc_view = 1;// 标记该参数有效
			SystemParaMenu2.m_bpc_view = SysMenuDefParam.m_bpc_view;// 将解析的m_bpc_view参数赋值给全局变量SystemParaMenu2.m_bpc_view
		}
		cJSON *m_local_isp_magic_value = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_local_isp_magic_value");
		if (m_local_isp_magic_value != NULL)
		{
			SysMenuDefParam.m_local_isp_magic_value = (unsigned int)m_local_isp_magic_value->valueint;
			SysParamValid.m_local_isp_magic_value = 1;// 标记该参数有效
			SystemParaMenu2.m_local_isp_magic_value = SysMenuDefParam.m_local_isp_magic_value;// 将解析的m_local_isp_magic_value参数赋值给全局变量SystemParaMenu2.m_local_isp_magic_value
		}
		cJSON *m_bpc_ref_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bpc_ref_mode");
		if (m_bpc_ref_mode != NULL)
		{
			SysMenuDefParam.m_bpc_ref_mode = (unsigned char)m_bpc_ref_mode->valueint;
			SysParamValid.m_bpc_ref_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_bpc_ref_mode = SysMenuDefParam.m_bpc_ref_mode;// 将解析的m_bpc_ref_mode参数赋值给全局变量SystemParaMenu2.m_bpc_ref_mode
		}

		//菜单二DET选项解析	
		cJSON *DetMode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "DetMode");
		if (DetMode != NULL)
		{
			SysMenuDefParam.DetMode = (unsigned char)DetMode->valueint;
			SysParamValid.DetMode = 1;// 标记该参数有效
			SystemParaMenu2.DetMode = SysMenuDefParam.DetMode;// 将解析的DetMode参数赋值给全局变量SystemParaMenu2.DetMode
		}
			
		cJSON *NormalTempINT_C = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NormalTemp.INT_C");
		if (NormalTempINT_C != NULL)
		{
			SysMenuDefParam.NormalTemp.INT_C = (unsigned char)NormalTempINT_C->valueint;
			SysParamValid.NormalTemp.INT_C = 1;// 标记该参数有效
			SystemParaMenu2.NormalTemp.INT_C = SysMenuDefParam.NormalTemp.INT_C;// 将解析的NormalTemp.INT_C参数赋值给全局变量SystemParaMenu2.NormalTemp.INT_C
		}
		cJSON *NormalTempINT_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NormalTemp.INT_T");
		if (NormalTempINT_T != NULL)
		{
			SysMenuDefParam.NormalTemp.INT_T = (unsigned char)NormalTempINT_T->valueint;
			SysParamValid.NormalTemp.INT_T = 1;// 标记该参数有效
			SystemParaMenu2.NormalTemp.INT_T = SysMenuDefParam.NormalTemp.INT_T;// 将解析的NormalTemp.INT_T参数赋值给全局变量SystemParaMenu2.NormalTemp.INT_T
		}
		cJSON *NormalTempOOC_Par = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NormalTemp.OOC_Par");
		if (NormalTempOOC_Par != NULL)
		{
			SysMenuDefParam.NormalTemp.OOC_Par = (unsigned char)NormalTempOOC_Par->valueint;
			SysParamValid.NormalTemp.OOC_Par = 1;// 标记该参数有效
			SystemParaMenu2.NormalTemp.OOC_Par = SysMenuDefParam.NormalTemp.OOC_Par;// 将解析的NormalTemp.OOC_Par参数赋值给全局变量SystemParaMenu2.NormalTemp.OOC_Par
		}
		cJSON *LowTempINT_C = cJSON_GetObjectItem(SystemMenuDefaultParameters, "LowTemp.INT_C");
		if (LowTempINT_C != NULL)
		{
			SysMenuDefParam.LowTemp.INT_C = (unsigned char)LowTempINT_C->valueint;
			SysParamValid.LowTemp.INT_C = 1;// 标记该参数有效
			SystemParaMenu2.LowTemp.INT_C = SysMenuDefParam.LowTemp.INT_C;// 将解析的LowTemp.INT_C参数赋值给全局变量SystemParaMenu2.LowTemp.INT_C
		}
		cJSON *LowTempINT_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "LowTemp.INT_T");
		if (LowTempINT_T != NULL)
		{
			SysMenuDefParam.LowTemp.INT_T = (unsigned char)LowTempINT_T->valueint;
			SysParamValid.LowTemp.INT_T = 1;// 标记该参数有效
			SystemParaMenu2.LowTemp.INT_T = SysMenuDefParam.LowTemp.INT_T;// 将解析的LowTemp.INT_T参数赋值给全局变量SystemParaMenu2.LowTemp.INT_T
		}
		cJSON *LowTempOOC_Par = cJSON_GetObjectItem(SystemMenuDefaultParameters, "LowTemp.OOC_Par");
		if (LowTempOOC_Par != NULL)
		{
			SysMenuDefParam.LowTemp.OOC_Par = (unsigned char)LowTempOOC_Par->valueint;
			SysParamValid.LowTemp.OOC_Par = 1;// 标记该参数有效
			SystemParaMenu2.LowTemp.OOC_Par = SysMenuDefParam.LowTemp.OOC_Par;// 将解析的LowTemp.OOC_Par参数赋值给全局变量SystemParaMenu2.LowTemp.OOC_Par
		}
		cJSON *HighTempINT_C = cJSON_GetObjectItem(SystemMenuDefaultParameters, "HighTemp.INT_C");
		if (HighTempINT_C != NULL)
		{
			SysMenuDefParam.HighTemp.INT_C = (unsigned char)HighTempINT_C->valueint;
			SysParamValid.HighTemp.INT_C = 1;// 标记该参数有效
			SystemParaMenu2.HighTemp.INT_C = SysMenuDefParam.HighTemp.INT_C;// 将解析的HighTemp.INT_C参数赋值给全局变量SystemParaMenu2.HighTemp.INT_C
		}
		cJSON *HighTempINT_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "HighTemp.INT_T");
		if (HighTempINT_T != NULL)
		{
			SysMenuDefParam.HighTemp.INT_T = (unsigned char)HighTempINT_T->valueint;
			SysParamValid.HighTemp.INT_T = 1;// 标记该参数有效
			SystemParaMenu2.HighTemp.INT_T = SysMenuDefParam.HighTemp.INT_T;// 将解析的HighTemp.INT_T参数赋值给全局变量SystemParaMenu2.HighTemp.INT_T
		}
		cJSON *HighTempOOC_Par = cJSON_GetObjectItem(SystemMenuDefaultParameters, "HighTemp.OOC_Par");
		if (HighTempOOC_Par != NULL)
		{
			SysMenuDefParam.HighTemp.OOC_Par = (unsigned char)HighTempOOC_Par->valueint;
			SysParamValid.HighTemp.OOC_Par = 1;// 标记该参数有效
			SystemParaMenu2.HighTemp.OOC_Par = SysMenuDefParam.HighTemp.OOC_Par;// 将解析的HighTemp.OOC_Par参数赋值给全局变量SystemParaMenu2.HighTemp.OOC_Par
		}
		cJSON *offChangeINT_C = cJSON_GetObjectItem(SystemMenuDefaultParameters, "offChange.INT_C");
		if (offChangeINT_C != NULL)
		{
			SysMenuDefParam.offChange.INT_C = (unsigned char)offChangeINT_C->valueint;
			SysParamValid.offChange.INT_C = 1;// 标记该参数有效
			SystemParaMenu2.offChange.INT_C = SysMenuDefParam.offChange.INT_C;// 将解析的offChange.INT_C参数赋值给全局变量SystemParaMenu2.offChange.INT_C
		}
		cJSON *offChangeINT_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "offChange.INT_T");
		if (offChangeINT_T != NULL)
		{
			SysMenuDefParam.offChange.INT_T = (unsigned char)offChangeINT_T->valueint;
			SysParamValid.offChange.INT_T = 1;// 标记该参数有效
			SystemParaMenu2.offChange.INT_T = SysMenuDefParam.offChange.INT_T;// 将解析的offChange.INT_T参数赋值给全局变量SystemParaMenu2.offChange.INT_T
		}
		cJSON *offChangeOOC_Par = cJSON_GetObjectItem(SystemMenuDefaultParameters, "offChange.OOC_Par");
		if (offChangeOOC_Par != NULL)
		{
			SysMenuDefParam.offChange.OOC_Par = (unsigned char)offChangeOOC_Par->valueint;
			SysParamValid.offChange.OOC_Par = 1;// 标记该参数有效
			SystemParaMenu2.offChange.OOC_Par = SysMenuDefParam.offChange.OOC_Par;// 将解析的offChange.OOC_Par参数赋值给全局变量SystemParaMenu2.offChange.OOC_Par
		}

		cJSON *IR_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "IR_SW");
		if (IR_SW != NULL)
		{
			SysMenuDefParam.IR_SW = (unsigned char)IR_SW->valueint;
			SysParamValid.IR_SW = 1;// 标记该参数有效
			SystemParaMenu2.IR_SW = SysMenuDefParam.IR_SW;// 将解析的IR_SW参数赋值给全局变量SystemParaMenu2.IR_SW
		}
		cJSON *GAIN = cJSON_GetObjectItem(SystemMenuDefaultParameters, "GAIN");
		if (GAIN != NULL)
		{
			SysMenuDefParam.GAIN = (unsigned char)GAIN->valueint;
			SysParamValid.GAIN = 1;// 标记该参数有效
			SystemParaMenu2.GAIN = SysMenuDefParam.GAIN;// 将解析的GAIN参数赋值给全局变量SystemParaMenu2.GAIN
		}
		cJSON *GOF = cJSON_GetObjectItem(SystemMenuDefaultParameters, "GOF");
		if (GOF != NULL)
		{
			SysMenuDefParam.GOF = (unsigned char)GOF->valueint;
			SysParamValid.GOF = 1;// 标记该参数有效
			SystemParaMenu2.GOF = SysMenuDefParam.GOF;// 将解析的GOF参数赋值给全局变量SystemParaMenu2.GOF
		}
		cJSON *INT_C = cJSON_GetObjectItem(SystemMenuDefaultParameters, "INT_C");
		if (INT_C != NULL)
		{
			SysMenuDefParam.INT_C = (unsigned char)INT_C->valueint;
			SysParamValid.INT_C = 1;// 标记该参数有效
			SystemParaMenu2.INT_C = SysMenuDefParam.INT_C;// 将解析的INT_C参数赋值给全局变量SystemParaMenu2.INT_C
		}
		cJSON *INT_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "INT_T");
		if (INT_T != NULL)
		{
			SysMenuDefParam.INT_T = (unsigned char)INT_T->valueint;
			SysParamValid.INT_T = 1;// 标记该参数有效
			SystemParaMenu2.INT_T = SysMenuDefParam.INT_T;// 将解析的INT_T参数赋值给全局变量SystemParaMenu2.INT_T
		}
		cJSON *OOC_Par = cJSON_GetObjectItem(SystemMenuDefaultParameters, "OOC_Par");
		if (OOC_Par != NULL)
		{
			SysMenuDefParam.OOC_Par = (unsigned char)OOC_Par->valueint;
			SysParamValid.OOC_Par = 1;// 标记该参数有效
			SystemParaMenu2.OOC_Par = SysMenuDefParam.OOC_Par;// 将解析的OOC_Par参数赋值给全局变量SystemParaMenu2.OOC_Par
		}
		cJSON *OOC_Mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "OOC_Mode");
		if (OOC_Mode != NULL)
		{
			SysMenuDefParam.OOC_Mode = (unsigned char)OOC_Mode->valueint;
			SysParamValid.OOC_Mode = 1;// 标记该参数有效
			SystemParaMenu2.OOC_Mode = SysMenuDefParam.OOC_Mode;// 将解析的OOC_Mode参数赋值给全局变量SystemParaMenu2.OOC_Mode
		}
		cJSON *OOC_Fix = cJSON_GetObjectItem(SystemMenuDefaultParameters, "OOC_Fix");
		if (OOC_Fix != NULL)
		{
			SysMenuDefParam.OOC_Fix = (unsigned char)OOC_Fix->valueint;
			SysParamValid.OOC_Fix = 1;// 标记该参数有效
			SystemParaMenu2.OOC_Fix = SysMenuDefParam.OOC_Fix;// 将解析的OOC_Fix参数赋值给全局变量SystemParaMenu2.OOC_Fix
		}
		cJSON *BAD_Cross = cJSON_GetObjectItem(SystemMenuDefaultParameters, "BAD_Cross");
		if (BAD_Cross != NULL)
		{
			SysMenuDefParam.BAD_Cross = (unsigned char)BAD_Cross->valueint;
			SysParamValid.BAD_Cross = 1;// 标记该参数有效
			SystemParaMenu2.BAD_Cross = SysMenuDefParam.BAD_Cross;// 将解析的BAD_Cross参数赋值给全局变量SystemParaMenu2.BAD_Cross
		}
		cJSON *AD_Temp = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AD_Temp");
		if (AD_Temp != NULL)
		{
			SysMenuDefParam.AD_Temp = (unsigned char)AD_Temp->valueint;
			SysParamValid.AD_Temp = 1;// 标记该参数有效
			SystemParaMenu2.AD_Temp = SysMenuDefParam.AD_Temp;// 将解析的AD_Temp参数赋值给全局变量SystemParaMenu2.AD_Temp
		}
		cJSON *TFilter = cJSON_GetObjectItem(SystemMenuDefaultParameters, "TFilter");
		if (TFilter != NULL)
		{
			SysMenuDefParam.TFilter = (unsigned char)TFilter->valueint;
			SysParamValid.TFilter = 1;// 标记该参数有效
			SystemParaMenu2.TFilter = SysMenuDefParam.TFilter;// 将解析的TFilter参数赋值给全局变量SystemParaMenu2.TFilter
		}
		cJSON *SFilter = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SFilter");
		if (SFilter != NULL)
		{
			SysMenuDefParam.SFilter = (unsigned char)SFilter->valueint;
			SysParamValid.SFilter = 1;// 标记该参数有效
			SystemParaMenu2.SFilter = SysMenuDefParam.SFilter;// 将解析的SFilter参数赋值给全局变量SystemParaMenu2.SFilter
		}
		cJSON *DDE_Level = cJSON_GetObjectItem(SystemMenuDefaultParameters, "DDE_Level");
		if (DDE_Level != NULL)
		{
			SysMenuDefParam.DDE_Level = (unsigned char)DDE_Level->valueint;
			SysParamValid.DDE_Level = 1;// 标记该参数有效
			SystemParaMenu2.DDE_Level = SysMenuDefParam.DDE_Level;// 将解析的DDE_Level参数赋值给全局变量SystemParaMenu2.DDE_Level
		}
		cJSON *DEST_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "DEST_SW");
		if (DEST_SW != NULL)
		{
			SysMenuDefParam.DEST_SW = (unsigned char)DEST_SW->valueint;
			SysParamValid.DEST_SW = 1;// 标记该参数有效
			SystemParaMenu2.DEST_SW = SysMenuDefParam.DEST_SW;// 将解析的DEST_SW参数赋值给全局变量SystemParaMenu2.DEST_SW
		}
		cJSON *test_state = cJSON_GetObjectItem(SystemMenuDefaultParameters, "test_state");
		if (test_state != NULL)
		{
			SysMenuDefParam.test_state = (unsigned char)test_state->valueint;
			SysParamValid.test_state = 1;// 标记该参数有效
			SystemParaMenu2.test_state = SysMenuDefParam.test_state;// 将解析的test_state参数赋值给全局变量SystemParaMenu2.test_state
		}
		cJSON *offset_X = cJSON_GetObjectItem(SystemMenuDefaultParameters, "offset_X");
		if (offset_X != NULL)
		{
			SysMenuDefParam.offset_X = (unsigned char)offset_X->valueint;
			SysParamValid.offset_X = 1;// 标记该参数有效
			SystemParaMenu2.offset_X = SysMenuDefParam.offset_X;// 将解析的offset_X参数赋值给全局变量SystemParaMenu2.offset_X
		}
		cJSON *offset_Y = cJSON_GetObjectItem(SystemMenuDefaultParameters, "offset_Y");
		if (offset_Y != NULL)
		{
			SysMenuDefParam.offset_Y = (unsigned char)offset_Y->valueint;
			SysParamValid.offset_Y = 1;// 标记该参数有效
			SystemParaMenu2.offset_Y = SysMenuDefParam.offset_Y;// 将解析的offset_Y参数赋值给全局变量SystemParaMenu2.offset_Y
		}
		cJSON *Auto_Semi = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Auto_Semi");
		if (Auto_Semi != NULL)
		{
			SysMenuDefParam.Auto_Semi = (unsigned char)Auto_Semi->valueint;
			SysParamValid.Auto_Semi = 1;// 标记该参数有效
			SystemParaMenu2.Auto_Semi = SysMenuDefParam.Auto_Semi;// 将解析的Auto_Semi参数赋值给全局变量SystemParaMenu2.Auto_Semi
		}
		cJSON *ImgT = cJSON_GetObjectItem(SystemMenuDefaultParameters, "ImgT");
		if (ImgT != NULL)
		{
			SysMenuDefParam.ImgT = (unsigned char)ImgT->valueint;
			SysParamValid.ImgT = 1;// 标记该参数有效
			SystemParaMenu2.ImgT = SysMenuDefParam.ImgT;// 将解析的ImgT参数赋值给全局变量SystemParaMenu2.ImgT
		}
		cJSON *Temp_TD = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Temp_TD");
		if (Temp_TD != NULL)
		{
			SysMenuDefParam.Temp_TD = (unsigned char)Temp_TD->valueint;
			SysParamValid.Temp_TD = 1;// 标记该参数有效
			SystemParaMenu2.Temp_TD = SysMenuDefParam.Temp_TD;// 将解析的Temp_TD参数赋值给全局变量SystemParaMenu2.Temp_TD
		}
		cJSON *Auto = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Auto");
		if (Auto != NULL)
		{
			SysMenuDefParam.Auto = (unsigned char)Auto->valueint;
			SysParamValid.Auto = 1;// 标记该参数有效
			SystemParaMenu2.Auto = SysMenuDefParam.Auto;// 将解析的Auto参数赋
		}
			
		cJSON *Bri_A = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Bri_A");
		if (Bri_A != NULL)
		{
			SysMenuDefParam.Bri_A = (unsigned char)Bri_A->valueint;
			SysParamValid.Bri_A = 1;// 标记该参数有效
			SystemParaMenu2.Bri_A = SysMenuDefParam.Bri_A;// 将解析的Bri_A参数赋值给全局变量SystemParaMenu2.Bri_A
		}
		cJSON *Ctrs_A = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Ctrs_A");
		if (Ctrs_A != NULL)
		{
			SysMenuDefParam.Ctrs_A = (unsigned char)Ctrs_A->valueint;
			SysParamValid.Ctrs_A = 1;// 标记该参数有效
			SystemParaMenu2.Ctrs_A = SysMenuDefParam.Ctrs_A;// 将解析的Ctrs_A参数赋值给全局变量SystemParaMenu2.Ctrs_A
		}
		cJSON *Bri_M = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Bri_M");
		if (Bri_M != NULL)
		{
			SysMenuDefParam.Bri_M = (unsigned char)Bri_M->valueint;
			SysParamValid.Bri_M = 1;// 标记该参数有效
			SystemParaMenu2.Bri_M = SysMenuDefParam.Bri_M;// 将解析的Bri_M参数赋值给全局变量SystemParaMenu2.Bri_M
		}
		cJSON *Ctrs_M = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Ctrs_M");
		if (Ctrs_M != NULL)
		{
			SysMenuDefParam.Ctrs_M = (unsigned char)Ctrs_M->valueint;
			SysParamValid.Ctrs_M = 1;// 标记该参数有效
			SystemParaMenu2.Ctrs_M = SysMenuDefParam.Ctrs_M;// 将解析的Ctrs_M参数赋值给全局变量SystemParaMenu2.Ctrs_M
		}
		cJSON *HOST_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "HOST_SW");
		if (HOST_SW != NULL)
		{
			SysMenuDefParam.HOST_SW = (unsigned char)HOST_SW->valueint;
			SysParamValid.HOST_SW = 1;// 标记该参数有效
			SystemParaMenu2.HOST_SW = SysMenuDefParam.HOST_SW;// 将解析的HOST_SW参数赋值给全局变量SystemParaMenu2.HOST_SW
		}
		cJSON *NDAC_H = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NDAC_H");
		if (NDAC_H != NULL)
		{
			SysMenuDefParam.NDAC_H = (unsigned char)NDAC_H->valueint;
			SysParamValid.NDAC_H = 1;// 标记该参数有效
			SystemParaMenu2.NDAC_H = SysMenuDefParam.NDAC_H;// 将解析的NDAC_H参数赋值给全局变量SystemParaMenu2.NDAC_H
		}
		cJSON *NDAC_L = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NDAC_L");
		if (NDAC_L != NULL)
		{
			SysMenuDefParam.NDAC_L = (unsigned char)NDAC_L->valueint;
			SysParamValid.NDAC_L = 1;// 标记该参数有效
			SystemParaMenu2.NDAC_L = SysMenuDefParam.NDAC_L;// 将解析的NDAC_L参数赋值给全局变量SystemParaMenu2.NDAC_L
		}
		cJSON *Output_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Output_mode");
		if (Output_mode != NULL)
		{
			SysMenuDefParam.Output_mode = (unsigned char)Output_mode->valueint;
			SysParamValid.Output_mode = 1;// 标记该参数有效
			SystemParaMenu2.Output_mode = SysMenuDefParam.Output_mode;// 将解析的Output_mode参数赋值给全局变量SystemParaMenu2.Output_mode
		}
		cJSON *Gmax = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Gmax");
		if (Gmax != NULL)
		{
			SysMenuDefParam.Gmax = (unsigned char)Gmax->valueint;
			SysParamValid.Gmax = 1;// 标记该参数有效
			SystemParaMenu2.Gmax = SysMenuDefParam.Gmax;// 将解析的Gmax参数赋值给全局变量SystemParaMenu2.Gmax
		}
		cJSON *Gamma = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Gamma");
		if (Gamma != NULL)
		{
			SysMenuDefParam.Gamma = (unsigned char)Gamma->valueint;
			SysParamValid.Gamma = 1;// 标记该参数有效
			SystemParaMenu2.Gamma = SysMenuDefParam.Gamma;// 将解析的Gamma参数赋值给全局变量SystemParaMenu2.Gamma
		}
		cJSON *AD_THR = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AD_THR");
		if (AD_THR != NULL)
		{
			SysMenuDefParam.AD_THR = (unsigned char)AD_THR->valueint;
			SysParamValid.AD_THR = 1;// 标记该参数有效
			SystemParaMenu2.AD_THR = SysMenuDefParam.AD_THR;// 将解析的AD_THR参数赋值给全局变量SystemParaMenu2.AD_THR
		}
		cJSON *NUMTHR = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NUMTHR");
		if (NUMTHR != NULL)
		{
			SysMenuDefParam.NUMTHR = (unsigned char)NUMTHR->valueint;
			SysParamValid.NUMTHR = 1;// 标记该参数有效
			SystemParaMenu2.NUMTHR = SysMenuDefParam.NUMTHR;// 将解析的NUMTHR参数赋值给全局变量SystemParaMenu2.NUMTHR
		}
		cJSON *AlgorithmSW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AlgorithmSW");
		if (AlgorithmSW != NULL)
		{
			SysMenuDefParam.AlgorithmSW = (unsigned char)AlgorithmSW->valueint;
			SysParamValid.AlgorithmSW = 1;// 标记该参数有效
			SystemParaMenu2.AlgorithmSW = SysMenuDefParam.AlgorithmSW;// 将解析的AlgorithmSW参数赋值给全局变量SystemParaMenu2.AlgorithmSW
		}
		cJSON *Alarm_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Alarm_SW");
		if (Alarm_SW != NULL)
		{
			SysMenuDefParam.Alarm_SW = (unsigned char)Alarm_SW->valueint;
			SysParamValid.Alarm_SW = 1;// 标记该参数有效
			SystemParaMenu2.Alarm_SW = SysMenuDefParam.Alarm_SW;// 将解析的Alarm_SW参数赋值给全局变量SystemParaMenu2.Alarm_SW
		}
		cJSON *Range_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Range_SW");
		if (Range_SW != NULL)
		{
			SysMenuDefParam.Range_SW = (unsigned char)Range_SW->valueint;
			SysParamValid.Range_SW = 1;// 标记该参数有效
			SystemParaMenu2.Range_SW = SysMenuDefParam.Range_SW;// 将解析的Range_SW参数赋值给全局变量SystemParaMenu2.Range_SW
		}
		cJSON *AlarmVal = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AlarmVal");
		if (AlarmVal != NULL)
		{
			SysMenuDefParam.AlarmVal = (unsigned short)AlarmVal->valueint;
			SysParamValid.AlarmVal = 1;// 标记该参数有效
			SystemParaMenu2.AlarmVal = SysMenuDefParam.AlarmVal;// 将解析的AlarmVal参数赋值给全局变量SystemParaMenu2.AlarmVal
		}
		cJSON *AlarmAutoVal = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AlarmAutoVal");
		if (AlarmAutoVal != NULL)
		{
			SysMenuDefParam.AlarmAutoVal = (unsigned short)AlarmAutoVal->valueint;
			SysParamValid.AlarmAutoVal = 1;// 标记该参数有效
			SystemParaMenu2.AlarmAutoVal = SysMenuDefParam.AlarmAutoVal;// 将解析的AlarmAutoVal参数赋值给全局变量SystemParaMenu2.AlarmAutoVal
		}
		cJSON *TempChangeValue = cJSON_GetObjectItem(SystemMenuDefaultParameters, "TempChangeValue");
		if (TempChangeValue != NULL)
		{
			SysMenuDefParam.TempChangeValue = (unsigned char)TempChangeValue->valueint;
			SysParamValid.TempChangeValue = 1;// 标记该参数有效
			SystemParaMenu2.TempChangeValue = SysMenuDefParam.TempChangeValue;// 将解析的TempChangeValue参数赋值给全局变量SystemParaMenu2.TempChangeValue
		}
		cJSON *BaudLevel = cJSON_GetObjectItem(SystemMenuDefaultParameters, "BaudLevel");
		if (BaudLevel != NULL)
		{
			SysMenuDefParam.BaudLevel = (unsigned char)BaudLevel->valueint;
			SysParamValid.BaudLevel = 1;// 标记该参数有效
			SystemParaMenu2.BaudLevel = SysMenuDefParam.BaudLevel;// 将解析的BaudLevel参数赋值给全局变量SystemParaMenu2.BaudLevel
		}
		cJSON *CVBS_flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "CVBS_flag");
		if (CVBS_flag != NULL)
		{
			SysMenuDefParam.CVBS_flag = (unsigned char)CVBS_flag->valueint;
			SysParamValid.CVBS_flag = 1;// 标记该参数有效
			SystemParaMenu2.CVBS_flag = SysMenuDefParam.CVBS_flag;// 将解析的CVBS_flag参数赋值给全局变量SystemParaMenu2.CVBS_flag
		}
		/* 测温功能-> 测温工具显示类型 */
		cJSON *AD_T = cJSON_GetObjectItem(SystemMenuDefaultParameters, "AD_T");
		if (AD_T != NULL)
		{
			SysMenuDefParam.AD_T = (unsigned char)AD_T->valueint;
			SysParamValid.AD_T = 1;// 标记该参数有效
			SystemParaMenu2.AD_T = SysMenuDefParam.AD_T;// 将解析的AD_T参数赋值给全局变量SystemParaMenu2.AD_T
		}
		cJSON *LensSN = cJSON_GetObjectItem(SystemMenuDefaultParameters, "LensSN");
		if (LensSN!= NULL){
			char* hexString=LensSN->valuestring;
			for(int i=0;i<strlen(hexString);i++)
			{
				SysMenuDefParam.LensSN[i]=(char)hexString[i];
				SysParamValid.LensSN[i] = 1;// 标记该参数有效
				SystemParaMenu2.LensSN[i] = SysMenuDefParam.LensSN[i];// 将解析的LensSN参数赋值给全局变量SystemParaMenu2.LensSN
			}
			
		}// 字符串类型参数需要逐个字符复制到结构体成员中
		cJSON *MerchineSN = cJSON_GetObjectItem(SystemMenuDefaultParameters, "MerchineSN");
		if (MerchineSN!= NULL){
			char* hexString=MerchineSN->valuestring;
			for(int i=0;i<strlen(hexString);i++)
			{
				SysMenuDefParam.MerchineSN[i]=(char)hexString[i];
				SysParamValid.MerchineSN[i] = 1;// 标记该参数有效
				SystemParaMenu2.MerchineSN[i] = SysMenuDefParam.MerchineSN[i];// 将解析的MerchineSN参数赋值给全局变量SystemParaMenu2.MerchineSN
			}
			
		}// 字符串类型参数需要逐个字符复制到结构体成员中

		cJSON *ProduceDATA = cJSON_GetObjectItem(SystemMenuDefaultParameters, "ProduceDATA");
		if (ProduceDATA!= NULL){
			char* hexString=ProduceDATA->valuestring;
			for(int i=0;i<strlen(hexString);i++)
			{
				SysMenuDefParam.ProduceDATA[i]=(char)hexString[i];
				SysParamValid.ProduceDATA[i] = 1;// 标记该参数有效
				SystemParaMenu2.ProduceDATA[i] = SysMenuDefParam.ProduceDATA[i];// 将解析的ProduceDATA参数赋值给全局变量SystemParaMenu2.ProduceDATA
			}
		}// 字符串类型参数需要逐个字符复制到结构体成员中

		cJSON *LOGO_Flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "LOGO_Flag");
		if (LOGO_Flag != NULL)
		{
			SysMenuDefParam.LOGO_Flag = (unsigned char)LOGO_Flag->valueint;
			SysParamValid.LOGO_Flag = 1;// 标记该参数有效
			SystemParaMenu2.LOGO_Flag = SysMenuDefParam.LOGO_Flag;// 将解析的LOGO_Flag参数赋值给全局变量SystemParaMenu2.LOGO_Flag
		}
		/* 测温功能-> 实验开关 */
		cJSON *TemTest_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "TemTest_SW");
		if (TemTest_SW != NULL)
		{
			SysMenuDefParam.TemTest_SW = (unsigned char)TemTest_SW->valueint;
			SysParamValid.TemTest_SW = 1;// 标记该参数有效
			SystemParaMenu2.TemTest_SW = SysMenuDefParam.TemTest_SW;// 将解析的TemTest_SW参数赋值给全局变量SystemParaMenu2.TemTest_SW
		}
		cJSON *Alpha = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Alpha");
		if (Alpha != NULL)
		{
			SysMenuDefParam.Alpha = (unsigned char)Alpha->valueint;
			SysParamValid.Alpha = 1;// 标记该参数有效
			SystemParaMenu2.Alpha = SysMenuDefParam.Alpha;// 将解析的Alpha参数赋值给全局变量SystemParaMenu2.Alpha
		}
		cJSON *NUC_Flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NUC_Flag");
		if (NUC_Flag != NULL)
		{
			SysMenuDefParam.NUC_Flag = (unsigned char)NUC_Flag->valueint;
			SysParamValid.NUC_Flag = 1;// 标记该参数有效
			SystemParaMenu2.NUC_Flag = SysMenuDefParam.NUC_Flag;// 将解析的NUC_Flag参数赋值给全局变量SystemParaMenu2.NUC_Flag
		}
		cJSON *Ran_Limit = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Ran_Limit");
		if (Ran_Limit != NULL)
		{
			SysMenuDefParam.Ran_Limit = (unsigned char)Ran_Limit->valueint;
			SysParamValid.Ran_Limit = 1;// 标记该参数有效
			SystemParaMenu2.Ran_Limit = SysMenuDefParam.Ran_Limit;// 将解析的Ran_Limit参数赋值给全局变量SystemParaMenu2.Ran_Limit
		}
		cJSON *Menu2ParaLength = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Menu2ParaLength");
		if (Menu2ParaLength != NULL)
		{
			SysMenuDefParam.Menu2ParaLength = (unsigned char)Menu2ParaLength->valueint;
			SysParamValid.Menu2ParaLength = 1;// 标记该参数有效
			SystemParaMenu2.Menu2ParaLength = SysMenuDefParam.Menu2ParaLength;// 将解析的Menu2ParaLength参数赋值给全局变量SystemParaMenu2.Menu2ParaLength
		}

		//菜单二BPC选项解析
		cJSON *m_bpc_cluster_on_flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bpc_cluster_on_flag");
		if (m_bpc_cluster_on_flag != NULL)
		{
			SysMenuDefParam.m_bpc_cluster_on_flag = (unsigned char)m_bpc_cluster_on_flag->valueint;
			SysParamValid.m_bpc_cluster_on_flag = 1;// 标记该参数有效
			SystemParaMenu2.m_bpc_cluster_on_flag = SysMenuDefParam.m_bpc_cluster_on_flag;// 将解析的m_bpc_cluster_on_flag参数赋值给全局变量SystemParaMenu2.m_bpc_cluster_on_flag
		}
		cJSON *Bpc_ext_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Bpc_ext_SW");
		if (Bpc_ext_SW != NULL)
		{
			SysMenuDefParam.Bpc_ext_SW = (unsigned char)Bpc_ext_SW->valueint;
			SysParamValid.Bpc_ext_SW = 1;// 标记该参数有效
			SystemParaMenu2.Bpc_ext_SW = SysMenuDefParam.Bpc_ext_SW;// 将解析的Bpc_ext_SW参数赋值给全局变量SystemParaMenu2.Bpc_ext_SW
		}

		//菜单二System选项解析
		cJSON *m_video_out_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_video_out_mode");
		if (m_video_out_mode != NULL)
		{
			SysMenuDefParam.m_video_out_mode = (unsigned char)m_video_out_mode->valueint;
			SysParamValid.m_video_out_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_video_out_mode = SysMenuDefParam.m_video_out_mode;// 将解析的m_video_out_mode参数赋值给全局变量SystemParaMenu2.m_video_out_mode
		}

		//Misc选项解析
		cJSON *m_cvbs_display_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_cvbs_display_mode");
		if (m_cvbs_display_mode != NULL)
		{
			SysMenuDefParam.m_cvbs_display_mode = (unsigned char)m_cvbs_display_mode->valueint;
			SysParamValid.m_cvbs_display_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_cvbs_display_mode = SysMenuDefParam.m_cvbs_display_mode;// 将解析的m_cvbs_display_mode参数赋值给全局变量SystemParaMenu2.m_cvbs_display_mode
		}
		cJSON *m_uvc_enlarger_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_uvc_enlarger_mode");
		if (m_uvc_enlarger_mode != NULL)
		{
			SysMenuDefParam.m_uvc_enlarger_mode = (unsigned char)m_uvc_enlarger_mode->valueint;
			SysParamValid.m_uvc_enlarger_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_uvc_enlarger_mode = SysMenuDefParam.m_uvc_enlarger_mode;// 将解析的m_uvc_enlarger_mode参数赋值给全局变量SystemParaMenu2.m_uvc_enlarger_mode
		}
		cJSON *m_mipi_lane_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_mipi_lane_mode");
		if (m_mipi_lane_mode != NULL)
		{
			SysMenuDefParam.m_mipi_lane_mode = (unsigned char)m_mipi_lane_mode->valueint;
			SysParamValid.m_mipi_lane_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_mipi_lane_mode = SysMenuDefParam.m_mipi_lane_mode;// 将解析的m_mipi_lane_mode参数赋值给全局变量SystemParaMenu2.m_mipi_lane_mode
		}
		cJSON *m_force_detector_type = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_force_detector_type");
		if (m_force_detector_type != NULL)
		{
			SysMenuDefParam.m_force_detector_type = (unsigned char)m_force_detector_type->valueint;
			SysParamValid.m_force_detector_type = 1;// 标记该参数有效
			SystemParaMenu2.m_force_detector_type = SysMenuDefParam.m_force_detector_type;// 将解析的m_force_detector_type参数赋值给全局变量SystemParaMenu2.m_force_detector_type
		}

		cJSON *m_force_hardware_port_type = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_force_hardware_port_type");
		if (m_force_hardware_port_type != NULL)
		{
			SysMenuDefParam.m_force_hardware_port_type = (unsigned char)m_force_hardware_port_type->valueint;
			SysParamValid.m_force_hardware_port_type = 1;// 标记该参数有效
			SystemParaMenu2.m_force_hardware_port_type = SysMenuDefParam.m_force_hardware_port_type;// 将解析的m_force_hardware_port_type参数赋值给全局变量SystemParaMenu2.m_force_hardware_port_type
		}

		cJSON *m_force_customer_id = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_force_customer_id");
		if (m_force_customer_id != NULL)
		{
			SysMenuDefParam.m_force_customer_id = (unsigned char)m_force_customer_id->valueint;
			SysParamValid.m_force_customer_id = 1;// 标记该参数有效
			SystemParaMenu2.m_force_customer_id = SysMenuDefParam.m_force_customer_id;// 将解析的m_force_customer_id参数赋值给全局变量SystemParaMenu2.m_force_customer_id
		}

		cJSON *m_force_focus_type = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_force_focus_type");
		if (m_force_focus_type != NULL)
		{
			SysMenuDefParam.m_force_focus_type = (unsigned char)m_force_focus_type->valueint;
			SysParamValid.m_force_focus_type = 1;// 标记该参数有效
			SystemParaMenu2.m_force_focus_type = SysMenuDefParam.m_force_focus_type;// 将解析的m_force_focus_type参数赋值给全局变量SystemParaMenu2.m_force_focus_type
		}

		cJSON *m_force_function_type = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_force_function_type");
		if (m_force_function_type != NULL)
		{
			SysMenuDefParam.m_force_function_type = (unsigned char)m_force_function_type->valueint;
			SysParamValid.m_force_function_type = 1;// 标记该参数有效
			SystemParaMenu2.m_force_function_type = SysMenuDefParam.m_force_function_type;// 将解析的m_force_function_type参数赋值给全局变量SystemParaMenu2.m_force_function_type
		}
		//Sun_Prot选项解析
		cJSON *m_st1_m = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_st1_m");
		if (m_st1_m != NULL)
		{
			SysMenuDefParam.m_st1_m = (unsigned char)m_st1_m->valueint;
			SysParamValid.m_st1_m = 1;// 标记该参数有效
			SystemParaMenu2.m_st1_m = SysMenuDefParam.m_st1_m;// 将解析的m_st1_m参数赋值给全局变量SystemParaMenu2.m_st1_m
		}
		cJSON *m_st1_s = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_st1_s");
		if (m_st1_s != NULL)
		{
			SysMenuDefParam.m_st1_s = (unsigned char)m_st1_s->valueint;
			SysParamValid.m_st1_s = 1;// 标记该参数有效
			SystemParaMenu2.m_st1_s = SysMenuDefParam.m_st1_s;// 将解析的m_st1_s参数赋值给全局变量SystemParaMenu2.m_st1_s
		}
		cJSON *m_det_st = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_det_st");
		if (m_det_st != NULL)
		{
			SysMenuDefParam.m_det_st = (unsigned char)m_det_st->valueint;
			SysParamValid.m_det_st = 1;// 标记该参数有效
			SystemParaMenu2.m_det_st = SysMenuDefParam.m_det_st;// 将解析的m_det_st参数赋值给全局变量SystemParaMenu2.m_det_st
		}
		// //m_SunPT_mode
		cJSON *m_SunPT_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_SunPT_mode");
		if (m_SunPT_mode != NULL)
		{
			SysMenuDefParam.m_SunPT_mode = (unsigned char)m_SunPT_mode->valueint;
			SysParamValid.m_SunPT_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_SunPT_mode = SysMenuDefParam.m_SunPT_mode;// 将解析的m_SunPT_mode参数赋值给全局变量SystemParaMenu2.m_SunPT_mode
		}
		cJSON *SunPT_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SunPT_SW");
		if (SunPT_SW != NULL)
		{
			SysMenuDefParam.SunPT_SW = (unsigned char)SunPT_SW->valueint;
			SysParamValid.SunPT_SW = 1;// 标记该参数有效
			SystemParaMenu2.SunPT_SW = SysMenuDefParam.SunPT_SW;// 将解析的SunPT_SW参数赋值给全局变量SystemParaMenu2.SunPT_SW
		}
		cJSON *S_Val = cJSON_GetObjectItem(SystemMenuDefaultParameters, "S_Val");
		if (S_Val != NULL)
		{
			SysMenuDefParam.S_Val = (unsigned short)S_Val->valueint;
			SysParamValid.S_Val = 1;// 标记该参数有效
			SystemParaMenu2.S_Val = SysMenuDefParam.S_Val;// 将解析的S_Val参数赋值给全局变量SystemParaMenu2.S_Val
		}
		cJSON *S_Range = cJSON_GetObjectItem(SystemMenuDefaultParameters, "S_Range");
		if (S_Range != NULL)
		{
			SysMenuDefParam.S_Range = (unsigned short)S_Range->valueint;
			SysParamValid.S_Range = 1;// 标记该参数有效
			SystemParaMenu2.S_Range = SysMenuDefParam.S_Range;// 将解析的S_Range参数赋值给全局变量SystemParaMenu2.S_Range
		}
		cJSON *P_Time_U16 = cJSON_GetObjectItem(SystemMenuDefaultParameters, "P_Time_U16");
		if (P_Time_U16 != NULL)
		{
			SysMenuDefParam.P_Time_U16 = (unsigned short)P_Time_U16->valueint;
			SysParamValid.P_Time_U16 = 1;// 标记该参数有效
			SystemParaMenu2.P_Time_U16 = SysMenuDefParam.P_Time_U16;// 将解析的P_Time_U16参数赋值给全局变量SystemParaMenu2.P_Time_U16
		}

		cJSON *SunPT_Info_SW = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SunPT_Info_SW");
		if (SunPT_Info_SW != NULL)
		{
			SysMenuDefParam.SunPT_Info_SW = (unsigned short)SunPT_Info_SW->valueint;
			SysParamValid.SunPT_Info_SW = 1;// 标记该参数有效
			SystemParaMenu2.SunPT_Info_SW = SysMenuDefParam.SunPT_Info_SW;// 将解析的SunPT_Info_SW参数赋值给全局变量SystemParaMenu2.SunPT_Info_SW
		}

		//T8模式选项解析
		cJSON *m_raw_yuv_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_raw_yuv_mode");
		if (m_raw_yuv_mode != NULL)
		{
			SysMenuDefParam.m_raw_yuv_mode = (unsigned char)m_raw_yuv_mode->valueint;
			SysParamValid.m_raw_yuv_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_raw_yuv_mode = SysMenuDefParam.m_raw_yuv_mode;// 将解析的m_raw_yuv_mode参数赋值给全局变量SystemParaMenu2.m_raw_yuv_mode
		}
			
		cJSON *m_raw_node_pos = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_raw_node_pos");
		if (m_raw_node_pos != NULL)
		{
			SysMenuDefParam.m_raw_node_pos = (unsigned char)m_raw_node_pos->valueint;
			SysParamValid.m_raw_node_pos = 1;// 标记该参数有效
			SystemParaMenu2.m_raw_node_pos = SysMenuDefParam.m_raw_node_pos;// 将解析的m_raw_node_pos参数赋值给全局变量SystemParaMenu2.m_raw_node_pos
		}
		cJSON *m_user_metadata_flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_user_metadata_flag");
		if (m_user_metadata_flag != NULL)
		{
			SysMenuDefParam.m_user_metadata_flag = (unsigned char)m_user_metadata_flag->valueint;
			SysParamValid.m_user_metadata_flag = 1;// 标记该参数有效
			SystemParaMenu2.m_user_metadata_flag = SysMenuDefParam.m_user_metadata_flag;// 将解析的m_user_metadata_flag参数赋值给全局变量SystemParaMenu2.m_user_metadata_flag
		}
		cJSON *m_bt_time_sequece_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bt_time_sequece_mode");
		if (m_bt_time_sequece_mode != NULL)
		{
			SysMenuDefParam.m_bt_time_sequece_mode = (unsigned char)m_bt_time_sequece_mode->valueint;
			SysParamValid.m_bt_time_sequece_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_bt_time_sequece_mode = SysMenuDefParam.m_bt_time_sequece_mode;// 将解析的m_bt_time_sequece_mode参数赋值给全局变量SystemParaMenu2.m_bt_time_sequece_mode
		}

		cJSON *m_bt_enlarger_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_bt_enlarger_mode");
		if (m_bt_enlarger_mode != NULL)
		{
			SysMenuDefParam.m_bt_enlarger_mode = (unsigned char)m_bt_enlarger_mode->valueint;
			SysParamValid.m_bt_enlarger_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_bt_enlarger_mode = SysMenuDefParam.m_bt_enlarger_mode;// 将解析的m_bt_enlarger_mode参数赋值给全局变量SystemParaMenu2.m_bt_enlarger_mode
		}

		cJSON *m_mipi_enlarger_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_mipi_enlarger_mode");
		if (m_mipi_enlarger_mode != NULL)
		{
			SysMenuDefParam.m_mipi_enlarger_mode = (unsigned char)m_mipi_enlarger_mode->valueint;
			SysParamValid.m_mipi_enlarger_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_mipi_enlarger_mode = SysMenuDefParam.m_mipi_enlarger_mode;// 将解析的m_mipi_enlarger_mode参数赋值给全局变量SystemParaMenu2.m_mipi_enlarger_mode
		}
		/* 测温功能-> RAW图数据输出类型 */
		cJSON *m_user_tempFrame_Mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_user_tempFrame_Mode");
		if (m_user_tempFrame_Mode != NULL)
		{
			SysMenuDefParam.m_user_tempFrame_Mode = (unsigned char)m_user_tempFrame_Mode->valueint;
			SysParamValid.m_user_tempFrame_Mode = 1;// 标记该参数有效
			SystemParaMenu2.m_user_tempFrame_Mode = SysMenuDefParam.m_user_tempFrame_Mode;// 将解析的m_user_tempFrame_Mode参数赋值给全局变量SystemParaMenu2.m_user_tempFrame_Mode
		}
		/* 测温功能-> 挡位量程限制 */
		cJSON *m_templutMode_Limit = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_templutMode_Limit");
		if (m_templutMode_Limit != NULL)
		{
			SysMenuDefParam.m_templutMode_Limit = (unsigned char)m_templutMode_Limit->valueint;
			SysParamValid.m_templutMode_Limit = 1;// 标记该参数有效
			SystemParaMenu2.m_templutMode_Limit = SysMenuDefParam.m_templutMode_Limit;// 将解析的m_templutMode_Limit参数赋值给全局变量SystemParaMenu2.m_templutMode_Limit
		}
		/* 测温功能-> 测温芯片切换 */
		cJSON *sensor = cJSON_GetObjectItem(SystemMenuDefaultParameters, "sensor");
		if (sensor != NULL)
		{
			SysMenuDefParam.sensor = (unsigned char)sensor->valueint;
			SysParamValid.sensor = 1;// 标记该参数有效
			SystemParaMenu2.sensor = SysMenuDefParam.sensor;// 将解析的sensor参数赋值给全局变量SystemParaMenu2.sensor
		}
		/* 测温功能-> 测温规律类型切换 */
		cJSON *m_temp_regularity = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_temp_regularity");
		if (m_temp_regularity != NULL)
		{
			SysMenuDefParam.m_temp_regularity = (unsigned char)m_temp_regularity->valueint;	
			SysParamValid.m_temp_regularity = 1;// 标记该参数有效
			SystemParaMenu2.m_temp_regularity = SysMenuDefParam.m_temp_regularity;// 将解析的m_temp_regularity参数赋值给全局变量SystemParaMenu2.m_temp_regularity
		}

		// 便于扩展，在此往后添加新变量....
			//如果是vot_planck1280c
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		cJSON *GAIN1280 = cJSON_GetObjectItem(SystemMenuDefaultParameters, "GAIN1280");
		if (GAIN1280 != NULL)
		{
			SysMenuDefParam.GAIN1280 = (unsigned char)GAIN1280->valueint;
			SysParamValid.GAIN1280 = 1;// 标记该参数有效
			SystemParaMenu2.GAIN1280 = SysMenuDefParam.GAIN1280;//将新导入的系统菜单默认参数同步更新到当前系统菜单参数结构体变量中，以便后续写入flash保存
		}
			
		cJSON *GOF1280 = cJSON_GetObjectItem(SystemMenuDefaultParameters, "GOF1280");
		if (GOF1280 != NULL)
		{
			SysMenuDefParam.GOF1280 = (unsigned char)GOF1280->valueint;
			SysParamValid.GOF1280 = 1;// 标记该参数有效
			SystemParaMenu2.GOF1280 = SysMenuDefParam.GOF1280;//将新导入的系统菜单默认参数同步更新到当前系统菜单参数结构体变量中，以便后续写入flash保存
		}	
		cJSON *Cint1280 = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Cint1280");
		if (Cint1280 != NULL)
		{
			SysMenuDefParam.Cint1280 = (unsigned char)Cint1280->valueint;
			SysParamValid.Cint1280 = 1;// 标记该参数有效
			SystemParaMenu2.Cint1280 = SysMenuDefParam.Cint1280;//将新导入的系统菜单默认参数同步更新到当前系统菜单参数结构体变量中，以便后续写入flash保存
		}
		cJSON *Nint = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Nint");
		if (Nint != NULL)
		{
			SysMenuDefParam.Nint = (unsigned short)Nint->valueint;
			SysParamValid.Nint = 1;// 标记该参数有效
			SystemParaMenu2.Nint = SysMenuDefParam.Nint;//将新导入的系统菜单默认参数同步更新到当前系统菜单参数结构体变量中，以便后续写入flash保存
		}
		cJSON *Sel_PJ = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Sel_PJ");
		if (Sel_PJ != NULL)
		{
			SysMenuDefParam.Sel_PJ = (unsigned char)Sel_PJ->valueint;
			SysParamValid.Sel_PJ = 1;// 标记该参数有效
			SystemParaMenu2.Sel_PJ = SysMenuDefParam.Sel_PJ;//将新导入的系统菜单默认参数同步更新到当前系统菜单参数结构体变量中，以便后续写入flash保存
		}
		cJSON *NTCC = cJSON_GetObjectItem(SystemMenuDefaultParameters, "NTCC");
		if (NTCC != NULL)			
		{
			SysMenuDefParam.NTCC = (unsigned char)NTCC->valueint;
			SysParamValid.NTCC = 1;// 标记该参数有效
			SystemParaMenu2.NTCC = SysMenuDefParam.NTCC;
		}
		cJSON *Voccm = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Voccm");
		if (Voccm != NULL)	
		{
			SysMenuDefParam.Voccm = (unsigned char)Voccm->valueint;
			SysParamValid.Voccm = 1;// 标记该参数有效
			SystemParaMenu2.Voccm = SysMenuDefParam.Voccm;
		}	
		cJSON *detConfig = cJSON_GetObjectItem(SystemMenuDefaultParameters, "detConfig");
		if (detConfig != NULL)		
		{
			SysMenuDefParam.detConfig = (unsigned char)detConfig->valueint;
			SysParamValid.detConfig = 1;// 标记该参数有效
			SystemParaMenu2.detConfig = SysMenuDefParam.detConfig;
		}
		cJSON *Vfid = cJSON_GetObjectItem(SystemMenuDefaultParameters, "Vfid");
		if (Vfid != NULL)			
		{
			SysMenuDefParam.Vfid = (unsigned char)Vfid->valueint;
			SysParamValid.Vfid = 1;// 标记该参数有效
			SystemParaMenu2.Vfid = SysMenuDefParam.Vfid;
		}


		cJSON *SmartData256bit = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SmartData256bit");
		if (SmartData256bit != NULL)
		{
			const char *hexString = SmartData256bit->valuestring;
			hexStringToUCharArray(hexString, SysMenuDefParam.SmartData256bit, 32);
			for(int i=0;i<32;i++)
			{
				SystemParaMenu2.SmartData256bit[i] = SysMenuDefParam.SmartData256bit[i];
				SysParamValid.SmartData256bit[i] = 1;// 标记该参数有效
			}
		}
		cJSON *SmartData256bitValidFlag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SmartData256bitValidFlag");
		if (SmartData256bitValidFlag != NULL)
		{
			SysMenuDefParam.SmartData256bitValidFlag = (unsigned char)SmartData256bitValidFlag->valueint;
			SysParamValid.SmartData256bitValidFlag = 1;// 标记该参数有效
			SystemParaMenu2.SmartData256bitValidFlag = SysMenuDefParam.SmartData256bitValidFlag;
		}
	}
	#else
	{
		cJSON *SmartData408bit = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SmartData408bit");
		if (SmartData408bit != NULL)
		{
			const char *hexString = SmartData408bit->valuestring;
			hexStringToUCharArray(hexString, SysMenuDefParam.SmartData408bit, 51);
			for(int i=0;i<51;i++)
			{
				SystemParaMenu2.SmartData408bit[i] = SysMenuDefParam.SmartData408bit[i];
				SysParamValid.SmartData408bit[i] = 1;// 标记该参数有效
			}
		}
		cJSON *SmartData408bitValidFlag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "SmartData408bitValidFlag");
		if (SmartData408bitValidFlag != NULL)
		{
			SysMenuDefParam.SmartData408bitValidFlag = (unsigned char)SmartData408bitValidFlag->valueint;
			SysParamValid.SmartData408bitValidFlag = 1;// 标记该参数有效
			SystemParaMenu2.SmartData408bitValidFlag = SysMenuDefParam.SmartData408bitValidFlag;
		}
			
	}
	#endif
	
		cJSON *set_current_ua = cJSON_GetObjectItem(SystemMenuDefaultParameters, "set_current_ua");
		if (set_current_ua != NULL)
		{
			SysMenuDefParam.set_current_ua = (unsigned char)set_current_ua->valueint;
			SysParamValid.set_current_ua = 1;// 标记该参数有效
			SystemParaMenu2.set_current_ua = SysMenuDefParam.set_current_ua;
		}

		cJSON *m_ts_npu_ops_sr_en = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_ts_npu_ops_sr_en");//解析超分功能使能
		if (m_ts_npu_ops_sr_en != NULL)
		{
			SysMenuDefParam.m_ts_npu_ops_sr_en = (unsigned char)m_ts_npu_ops_sr_en->valueint;
			SysParamValid.m_ts_npu_ops_sr_en = 1;// 标记该参数有效
			SystemParaMenu2.m_ts_npu_ops_sr_en=SysMenuDefParam.m_ts_npu_ops_sr_en;
		}

		cJSON *m_menu2_info_tip_visible = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_menu2_info_tip_visible");
		if (m_menu2_info_tip_visible != NULL)
		{
			SysMenuDefParam.m_menu2_info_tip_visible = (unsigned char)m_menu2_info_tip_visible->valueint;
			SysParamValid.m_menu2_info_tip_visible = 1;// 标记该参数有效
			SystemParaMenu2.m_menu2_info_tip_visible = SysMenuDefParam.m_menu2_info_tip_visible;
		}

		cJSON *m_temp_drift_comp_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_temp_drift_comp_mode");
		if (m_temp_drift_comp_mode != NULL)
		{
			SysMenuDefParam.m_temp_drift_comp_mode = (unsigned char)m_temp_drift_comp_mode->valueint;
			SysParamValid.m_temp_drift_comp_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_temp_drift_comp_mode = SysMenuDefParam.m_temp_drift_comp_mode;
		}

		cJSON *m_temp_drift_aisp_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_temp_drift_aisp_mode");
		if (m_temp_drift_aisp_mode != NULL)
		{
			SysMenuDefParam.m_temp_drift_aisp_mode = (unsigned char)m_temp_drift_aisp_mode->valueint;
			SysParamValid.m_temp_drift_aisp_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_temp_drift_aisp_mode = SysMenuDefParam.m_temp_drift_aisp_mode;
		}
		
		
		cJSON *m_t8_mode_force_bpc_enable = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_t8_mode_force_bpc_enable");
		if (m_t8_mode_force_bpc_enable != NULL)
		{
			SysMenuDefParam.m_t8_mode_force_bpc_enable = (unsigned char)m_t8_mode_force_bpc_enable->valueint;
			SysParamValid.m_t8_mode_force_bpc_enable = 1;// 标记该参数有效
			SystemParaMenu2.m_t8_mode_force_bpc_enable = SysMenuDefParam.m_t8_mode_force_bpc_enable;
		}

		cJSON *m_vot_pod_type = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_vot_pod_type");
		if (m_vot_pod_type != NULL)
		{
			SysMenuDefParam.m_vot_pod_type = (unsigned char)m_vot_pod_type->valueint;
			SysParamValid.m_vot_pod_type = 1;// 标记该参数有效
			SystemParaMenu2.m_vot_pod_type = SysMenuDefParam.m_vot_pod_type;
		}

		cJSON *m_st0_total_duration_s = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_st0_total_duration_s");
		if (m_st0_total_duration_s != NULL)
		{
			SysMenuDefParam.m_st0_total_duration_s = (unsigned char)m_st0_total_duration_s->valueint;
			SysParamValid.m_st0_total_duration_s = 1;// 标记该参数有效
			SystemParaMenu2.m_st0_total_duration_s = SysMenuDefParam.m_st0_total_duration_s;
		}
		cJSON *m_st0_interval_s = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_st0_interval_s");
		if (m_st0_interval_s != NULL)
		{
			SysMenuDefParam.m_st0_interval_s = (unsigned char)m_st0_interval_s->valueint;
			SysParamValid.m_st0_interval_s = 1;// 标记该参数有效
			SystemParaMenu2.m_st0_interval_s = SysMenuDefParam.m_st0_interval_s;
		}

		/* UVC终端输出数据格式 */
		cJSON *m_uvc_output_format = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_uvc_output_format");
		if (m_uvc_output_format != NULL) {
			SysMenuDefParam.m_uvc_output_format = (unsigned char)m_uvc_output_format->valueint;
			SysParamValid.m_uvc_output_format = 1;// 标记该参数有效
			SystemParaMenu2.m_uvc_output_format = SysMenuDefParam.m_uvc_output_format;
		}
		/* 测温功能-> 测温工具标定/非标定模式 */
		cJSON *m_temperCalibra_Flag = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_temperCalibra_Flag");
		if (m_temperCalibra_Flag != NULL) {
			SysMenuDefParam.m_temperCalibra_Flag = (unsigned char)m_temperCalibra_Flag->valueint;
			SysParamValid.m_temperCalibra_Flag = 1;// 标记该参数有效
			SystemParaMenu2.m_temperCalibra_Flag = SysMenuDefParam.m_temperCalibra_Flag;
		}

		cJSON *m_update_firmware_policy_mode = cJSON_GetObjectItem(SystemMenuDefaultParameters, "m_update_firmware_policy_mode");
		if (m_update_firmware_policy_mode != NULL)
		{
			SysMenuDefParam.m_update_firmware_policy_mode = (unsigned char)m_update_firmware_policy_mode->valueint;
			SysParamValid.m_update_firmware_policy_mode = 1;// 标记该参数有效
			SystemParaMenu2.m_update_firmware_policy_mode = SysMenuDefParam.m_update_firmware_policy_mode;
		}

		// 将已修改的系统菜单默认参数写入flash
		if (ISP_write_SysMenu_def_para_cfg(SysMenuDefParam) == 0)
		{
			//前512K空间存储JSON配置的系统菜单默认参数，偏移512K,后512K空间存储JSON配置的系统菜单默认参数是否有效标志位
			vision_log_debug("Write SysMenuDefParam to flash success.\n");
			if(write_JsonSystemMenuDefPara_valid_flag(SysParamValid)==0)// 将系统菜单默认参数有效标志位写入flash
			{
				ISP_write_sys_menu2_cfg(SystemParaMenu2);//保存当前已同步更新的系统菜单参数，以便机芯重启，更改的参数生效
				UpdatePromptInfoWithTimeMs("SysMenuDefParam write to flash success.", 2000,VOT_LOG_TYPE_MENU2);
			}
			else
			{
				vision_log_debug("Write SysMenuDefParam valid flag to flash failure.\n");
				UpdatePromptInfoWithTimeMs("SysMenuDefParam valid write failure.", 2000,VOT_LOG_TYPE_MENU2);
				cJSON_Delete(root);
				return 1;// 系统菜单默认参数有效标志位写入flash失败，返回错误
			}
		}
		else
		{
			vision_log_debug("Write SysMenuDefParam to flash failure.\n");
			UpdatePromptInfoWithTimeMs("SysMenuDefParam write to flash failure.", 2000,VOT_LOG_TYPE_MENU2);
			cJSON_Delete(root);
			return 1;// 系统菜单默认参数写入flash失败，返回错误
		}
	}
	

	// 释放cJSON内存
	cJSON_Delete(root);

	// 软件重启机芯
	SystemParaMenu2.SunPT_SW = 0;
	ts_cpl_fix_msleep(200);
	vot_vpo_freeze();
	planck640c_stop();
	cli_simple_run_command("reboot");//json参数导入成功后重启系统，使更改的参数生效
	UpdatePromptInfoWithTimeMs("Rebooting...", 2000,VOT_LOG_TYPE_MENU2);//提示即将重启

	return 0;
}

/**
 * @brief:导出JSON字符串的函数，供外部调用以获取当前的用户菜单默认参数和系统菜单默认参数的JSON字符串
 * @parameter:无
 * @return:0-success 1-failure
 */
int Create_json_string(void)
{
	struct _Usermenu_Parameter UsermenuDefParam;
	struct Menu2_Para SysMenuDefParam;
	// 创建根对象
	cJSON *root = cJSON_CreateObject();
	if (!root)
		return 1;

	// 添加基础字段
	//if(ISP_read_UserMenu_def_para_cfg(&UsermenuDefParam) == 0)cJSON_AddStringToObject(root, "version", (char *)UsermenuDefParam.json_version);
	cJSON_AddStringToObject(root, "version", "1.0.0");
	// cJSON_AddStringToObject(root, "device_type", "VISION_UAD3");
	// cJSON_AddStringToObject(root, "manufacturer", "光智科技VOT");
	// cJSON_AddStringToObject(root, "description", "红外热成像机芯的默认参数配置");
	//  添加 UserMenuDefaultParameters 对象
	cJSON *user_menu = cJSON_CreateObject();
	if (user_menu != NULL)
	{
		memcpy(&UsermenuDefParam, &UsermenuParameter, sizeof(struct _Usermenu_Parameter));//把当前RAM中的用户菜单数据输出json
		// vision_log_debug("UsermenuDefParam.Bright:%d",UsermenuDefParam.Bright);
		//菜单一：图像Image
		cJSON_AddNumberToObject(user_menu, "Bright", (const double)UsermenuDefParam.Bright);
		cJSON_AddNumberToObject(user_menu, "Contrast", (const double)UsermenuDefParam.Contrast);
		cJSON_AddNumberToObject(user_menu, "Palette", (const double)UsermenuDefParam.Palette);
		cJSON_AddNumberToObject(user_menu, "Enlarge", (const double)UsermenuDefParam.Enlarge);

		//菜单一：测温Measurement

		//菜单一：系统System
		cJSON_AddNumberToObject(user_menu, "IsEnableEnligh", (const double)UsermenuDefParam.IsEnableEnligh);
		cJSON_AddNumberToObject(user_menu, "Mirror", (const double)UsermenuDefParam.Mirror);
		cJSON_AddNumberToObject(user_menu, "AutoShutter", (const double)UsermenuDefParam.AutoShutter);

		//菜单一：调节Adjust
		cJSON_AddNumberToObject(user_menu, "enhance_level", (const double)UsermenuDefParam.enhance_level);
		cJSON_AddNumberToObject(user_menu, "SFilter", (const double)UsermenuDefParam.SFilter);
		cJSON_AddNumberToObject(user_menu, "m_dev_env_mode", (const double)UsermenuDefParam.m_dev_env_mode);
	    //菜单一：预留
		//cJSON_AddNumberToObject(user_menu, "EE_Level", (const double)UsermenuDefParam.EE_Level);
		//cJSON_AddNumberToObject(user_menu, "DDE_Level", (const double)UsermenuDefParam.DDE_Level);
		//cJSON_AddNumberToObject(user_menu, "ShutterMode", (const double)UsermenuDefParam.ShutterMode);
		// 便于扩展，在此往后添加新变量....
		cJSON_AddNumberToObject(user_menu, "manual_ImagePara.Bright", (const double)UsermenuDefParam.manual_ImagePara.Bright);
		cJSON_AddNumberToObject(user_menu, "manual_ImagePara.Contrast", (const double)UsermenuDefParam.manual_ImagePara.Contrast);
		cJSON_AddNumberToObject(user_menu, "m_luma_percent_mode", (const double)UsermenuDefParam.m_luma_percent_mode);
		cJSON_AddNumberToObject(user_menu, "m_contrast_percent_mode", (const double)UsermenuDefParam.m_contrast_percent_mode);
		cJSON_AddNumberToObject(user_menu, "m_pip_mode", (const double)UsermenuDefParam.m_pip_mode);
		cJSON_AddNumberToObject(user_menu, "m_hst_mode", (const double)UsermenuDefParam.m_hst_mode);
		cJSON_AddNumberToObject(user_menu, "m_rz_ratio", (const double)UsermenuDefParam.m_rz_ratio);

		cJSON_AddNumberToObject(user_menu, "m_SunPT_mode", (const double)UsermenuDefParam.m_SunPT_mode);
		cJSON_AddNumberToObject(user_menu, "m_SunPT_gap", (const double)UsermenuDefParam.m_SunPT_gap);

		cJSON_AddNumberToObject(user_menu, "m_menu1_info_tip_visible", (const double)UsermenuDefParam.m_menu1_info_tip_visible);

		cJSON_AddItemToObject(root, "UserMenuDefaultParameters", user_menu);
	}

	// 添加 SystemMenuDefaultParameters 对象
	cJSON *system_menu = cJSON_CreateObject();
	if (system_menu != NULL)
	{
		memcpy(&SysMenuDefParam, &SystemParaMenu2, sizeof(struct Menu2_Para));//把当前RAM中的系统菜单数据输出json

		ts_char ver_info[64];
		ts_cpl_snprintf(ver_info, 64, "%s",VOT_SOFTWARE_VER_FIXED_HEADER);
		cJSON_AddStringToObject(system_menu, "SocSoftwareVersion", (const char *)ver_info);//SOC软件版本信息

		ts_char Det_ID[64];
		//ts_cpl_snprintf(Det_ID, 64, "%s %s",SystemParaMenu2.DetectorDATA,SystemParaMenu2.exDetectorDATA);
		ts_cpl_snprintf(Det_ID, 64, "%s",SystemParaMenu2.DetectorDATA);
		cJSON_AddStringToObject(system_menu, "Det_ID", (const char *)Det_ID);//探测器ID
		ts_char DeviceInfo[80];
		ts_cpl_snprintf(DeviceInfo, 80, "S/N:%s  LensInfo:%s  ProductionDate:%s",SystemParaMenu2.MerchineSN,SystemParaMenu2.LensSN,SystemParaMenu2.ProduceDATA);
		cJSON_AddStringToObject(system_menu, "DeviceInformation", (const char *)DeviceInfo);//设备信息

		//菜单二：Algorithm
		// cJSON_AddNumberToObject(system_menu, "m_sffc_mode", (const double)SysMenuDefParam.m_sffc_mode);
		// cJSON_AddNumberToObject(system_menu, "m_sffc_level", (const double)SysMenuDefParam.m_sffc_level);
       	// cJSON_AddNumberToObject(system_menu, "m_ee_enhance_mode", (const double)SysMenuDefParam.m_ee_enhance_mode);


		//菜单二：OCC

		//菜单二：NUC
		//cJSON_AddNumberToObject(system_menu, "NUC_SW", (const double)SysMenuDefParam.NUC_SW);

		//菜单二：DET
		#if 0
		cJSON_AddNumberToObject(system_menu, "GAIN", (const double)SysMenuDefParam.GAIN);
		cJSON_AddNumberToObject(system_menu, "GOF", (const double)SysMenuDefParam.GOF);
		cJSON_AddNumberToObject(system_menu, "INT_C", (const double)SysMenuDefParam.INT_C);
		cJSON_AddNumberToObject(system_menu, "INT_T", (const double)SysMenuDefParam.INT_T);
		cJSON_AddNumberToObject(system_menu, "OOC_Par", (const double)SysMenuDefParam.OOC_Par);
		cJSON_AddNumberToObject(system_menu, "NDAC_L", (const double)SysMenuDefParam.NDAC_L);
		cJSON_AddNumberToObject(system_menu, "NDAC_H", (const double)SysMenuDefParam.NDAC_H);
	
		cJSON_AddNumberToObject(system_menu, "DetMode", (const double)SysMenuDefParam.DetMode);
		cJSON_AddNumberToObject(system_menu, "NormalTemp.INT_C", (const double)SysMenuDefParam.NormalTemp.INT_C);
		cJSON_AddNumberToObject(system_menu, "NormalTemp.INT_T", (const double)SysMenuDefParam.NormalTemp.INT_T);
		cJSON_AddNumberToObject(system_menu, "NormalTemp.OOC_Par", (const double)SysMenuDefParam.NormalTemp.OOC_Par);
		cJSON_AddNumberToObject(system_menu, "LowTemp.INT_C", (const double)SysMenuDefParam.LowTemp.INT_C);
		cJSON_AddNumberToObject(system_menu, "LowTemp.INT_T", (const double)SysMenuDefParam.LowTemp.INT_T);
		cJSON_AddNumberToObject(system_menu, "LowTemp.OOC_Par", (const double)SysMenuDefParam.LowTemp.OOC_Par);
		cJSON_AddNumberToObject(system_menu, "HighTemp.INT_C", (const double)SysMenuDefParam.HighTemp.INT_C);
		cJSON_AddNumberToObject(system_menu, "HighTemp.INT_T", (const double)SysMenuDefParam.HighTemp.INT_T);
		cJSON_AddNumberToObject(system_menu, "HighTemp.OOC_Par", (const double)SysMenuDefParam.HighTemp.OOC_Par);
		cJSON_AddNumberToObject(system_menu, "offChange.INT_C", (const double)SysMenuDefParam.offChange.INT_C);
		cJSON_AddNumberToObject(system_menu, "offChange.INT_T", (const double)SysMenuDefParam.offChange.INT_T);
		cJSON_AddNumberToObject(system_menu, "offChange.OOC_Par", (const double)SysMenuDefParam.offChange.OOC_Par);
		#endif
		//菜单二：BPC
		// cJSON_AddNumberToObject(system_menu, "m_bpc_cluster_on_flag", (const double)SysMenuDefParam.m_bpc_cluster_on_flag);
		// //cJSON_AddNumberToObject(system_menu, "BAD_SW", (const double)SysMenuDefParam.BAD_SW);
		// cJSON_AddNumberToObject(system_menu, "m_bpc_ref_mode", (const double)SysMenuDefParam.m_bpc_ref_mode);
		// // cJSON_AddNumberToObject(system_menu, "Auto_Semi", (const double)SysMenuDefParam.Auto_Semi);
		// cJSON_AddNumberToObject(system_menu, "m_bpc_cfg", (const double)SysMenuDefParam.m_bpc_cfg);
		// cJSON_AddNumberToObject(system_menu, "m_bpc_view", (const double)SysMenuDefParam.m_bpc_view);
		// cJSON_AddNumberToObject(system_menu, "Bpc_ext_SW", (const double)SysMenuDefParam.Bpc_ext_SW);
		// cJSON_AddNumberToObject(system_menu, "m_local_isp_magic_value", (const double)SysMenuDefParam.m_local_isp_magic_value);
		
		//菜单二：系统System
		//cJSON_AddNumberToObject(system_menu, "m_osd_show_on_dev", (const double)SysMenuDefParam.m_osd_show_on_dev);
		cJSON_AddNumberToObject(system_menu, "m_video_out_mode", (const double)SysMenuDefParam.m_video_out_mode);
		//Misc
		cJSON_AddNumberToObject(system_menu, "m_cvbs_display_mode", (const double)SysMenuDefParam.m_cvbs_display_mode);
		cJSON_AddNumberToObject(system_menu, "m_uvc_enlarger_mode", (const double)SysMenuDefParam.m_uvc_enlarger_mode);
		cJSON_AddNumberToObject(system_menu, "m_mipi_lane_mode", (const double)SysMenuDefParam.m_mipi_lane_mode);
		cJSON_AddNumberToObject(system_menu, "m_force_detector_type", (const double)SysMenuDefParam.m_force_detector_type);
		cJSON_AddNumberToObject(system_menu, "m_force_hardware_port_type", (const double)SysMenuDefParam.m_force_hardware_port_type);
		cJSON_AddNumberToObject(system_menu, "m_force_customer_id", (const double)SysMenuDefParam.m_force_customer_id);
		cJSON_AddNumberToObject(system_menu, "m_force_focus_type", (const double)SysMenuDefParam.m_force_focus_type);
		//cJSON_AddNumberToObject(system_menu, "m_force_function_type", (const double)SysMenuDefParam.m_force_function_type);
		cJSON_AddNumberToObject(system_menu, "m_ts_npu_ops_sr_en", (const double)SysMenuDefParam.m_ts_npu_ops_sr_en);//导出超分功能使能

		
		//cJSON_AddNumberToObject(system_menu, "m_device_fps", (const double)SysMenuDefParam.m_device_fps);
		cJSON_AddNumberToObject(system_menu, "m_st1_m", (const double)SysMenuDefParam.m_st1_m);
		cJSON_AddNumberToObject(system_menu, "m_st1_s", (const double)SysMenuDefParam.m_st1_s);
		cJSON_AddNumberToObject(system_menu, "m_det_st", (const double)SysMenuDefParam.m_det_st);
		//Sun_Prot
		//cJSON_AddNumberToObject(system_menu, "m_SunPT_mode", (const double)SysMenuDefParam.m_SunPT_mode);
		cJSON_AddNumberToObject(system_menu, "SunPT_SW", (const double)SysMenuDefParam.SunPT_SW);
		cJSON_AddNumberToObject(system_menu, "S_Range", (const double)SysMenuDefParam.S_Range);
		cJSON_AddNumberToObject(system_menu, "S_Val", (const double)SysMenuDefParam.S_Val);
		cJSON_AddNumberToObject(system_menu, "P_Time_U16", (const double)SysMenuDefParam.P_Time_U16);

		cJSON_AddNumberToObject(system_menu, "m_fix_snc_switch", (const double)SysMenuDefParam.m_fix_snc_switch);

		//cJSON_AddNumberToObject(system_menu, "Usermenu_SW", (const double)SysMenuDefParam.Usermenu_SW);
		//T8模式		
		cJSON_AddNumberToObject(system_menu, "m_raw_yuv_mode", (const double)SysMenuDefParam.m_raw_yuv_mode);
		cJSON_AddNumberToObject(system_menu, "m_raw_node_pos", (const double)SysMenuDefParam.m_raw_node_pos);
		cJSON_AddNumberToObject(system_menu, "m_user_metadata_flag", (const double)SysMenuDefParam.m_user_metadata_flag);
		cJSON_AddNumberToObject(system_menu, "m_bt_time_sequece_mode", (const double)SysMenuDefParam.m_bt_time_sequece_mode);
		
		cJSON_AddNumberToObject(system_menu, "m_menu2_info_tip_visible", (const double)SysMenuDefParam.m_menu2_info_tip_visible);
		cJSON_AddNumberToObject(system_menu, "m_temp_drift_comp_mode", (const double)SysMenuDefParam.m_temp_drift_comp_mode);
		cJSON_AddNumberToObject(system_menu, "m_temp_drift_aisp_mode", (const double)SysMenuDefParam.m_temp_drift_aisp_mode);
		
		// 便于扩展，在此往后添加新变量....
#if 0
		 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
		#ifdef CONFIG_SNR_PLANCK1280C
		{
			cJSON_AddNumberToObject(system_menu, "GAIN1280", (const double)SysMenuDefParam.GAIN1280);
			cJSON_AddNumberToObject(system_menu, "GOF1280", (const double)SysMenuDefParam.GOF1280);
			cJSON_AddNumberToObject(system_menu, "Cint1280", (const double)SysMenuDefParam.Cint1280);
			cJSON_AddNumberToObject(system_menu, "Nint", (const double)SysMenuDefParam.Nint);
			cJSON_AddNumberToObject(system_menu, "Sel_PJ", (const double)SysMenuDefParam.Sel_PJ);
			cJSON_AddNumberToObject(system_menu, "NTCC", (const double)	SysMenuDefParam.NTCC);
			cJSON_AddNumberToObject(system_menu, "Voccm", (const double)SysMenuDefParam.Voccm);
			cJSON_AddNumberToObject(system_menu, "detConfig", (const double)SysMenuDefParam.detConfig);
			cJSON_AddNumberToObject(system_menu, "Vfid", (const double)	SysMenuDefParam.Vfid);

			char hexString[32 * 2 + 1]; // +1 for the null terminator
			unsignedCharArrayToHexString(SysMenuDefParam.SmartData256bit, 32, hexString);
			cJSON_AddStringToObject(system_menu, "SmartData256bit", (const char *)hexString);
			cJSON_AddNumberToObject(system_menu, "SmartData256bitValidFlag", (const double)SysMenuDefParam.SmartData256bitValidFlag);
		}
		#else
		{
			char hexString[51 * 2 + 1]; // +1 for the null terminator
			unsignedCharArrayToHexString(SysMenuDefParam.SmartData408bit, 51, hexString);
			cJSON_AddStringToObject(system_menu, "SmartData408bit", (const char *)hexString);
			cJSON_AddNumberToObject(system_menu, "SmartData408bitValidFlag", (const double)SysMenuDefParam.SmartData408bitValidFlag);
		}
		#endif
#endif
		//cJSON_AddNumberToObject(system_menu, "set_current_ua", (const double)SysMenuDefParam.set_current_ua);
		/* UVC终端输出格式 */
		cJSON_AddNumberToObject(system_menu, "m_uvc_output_format", (const double)SysMenuDefParam.m_uvc_output_format);
		/* 测温功能-> 测温芯片切换 */
		cJSON_AddNumberToObject(system_menu, "sensor", (const double)SysMenuDefParam.sensor);
		/* 测温功能-> 挡位量程限制 */
		cJSON_AddNumberToObject(system_menu, "m_templutMode_Limit", (const double)SysMenuDefParam.m_templutMode_Limit);
		/* 测温功能-> 实验开关 */
		cJSON_AddNumberToObject(system_menu, "TemTest_SW", (const double)SysMenuDefParam.TemTest_SW);
		/* 测温功能-> 挡位切换 */
		cJSON_AddNumberToObject(system_menu, "Usermenu_SW", (const double)SysMenuDefParam.Usermenu_SW);
		/* 测温功能-> 测温工具显示类型 */
		cJSON_AddNumberToObject(system_menu, "AD_T", (const double)SysMenuDefParam.AD_T);
		/* 测温功能-> 测温工具标定/非标定模式 */
		cJSON_AddNumberToObject(system_menu, "m_temperCalibra_Flag", (const double)SysMenuDefParam.m_temperCalibra_Flag);
		/* 测温功能-> RAW图数据输出类型 */
		cJSON_AddNumberToObject(system_menu, "m_user_tempFrame_Mode", (const double)SysMenuDefParam.m_user_tempFrame_Mode);
		/* 测温功能-> 测温规律类型选择 */
		cJSON_AddNumberToObject(system_menu, "m_temp_regularity", (const double)SysMenuDefParam.m_temp_regularity);



		cJSON_AddItemToObject(root, "SystemMenuDefaultParameters", system_menu);
	}
	// 添加 ExtendedParameters 对象
	// cJSON *extended = cJSON_CreateObject();
	// if (extended != NULL)
	// {
	// 	// cJSON_AddStringToObject(extended, "resolution", "640x512");
	// 	cJSON_AddItemToObject(root, "ExtendedParameters", extended);
	// }

	const int json_buffer_size = 1024*4;//注意：json字符串大小不要超过4K，否则超过栈深导致程序崩溃
	char json_buffer[json_buffer_size];
	// 使用预分配缓冲区打印 JSON (第二个参数是缓冲区，第三个是大小)
	//    这里我们使用 format = 1，打印格式化的 JSON，便于阅读
	int success = cJSON_PrintPreallocated(root, json_buffer, json_buffer_size, 1);
	if (success)
	{
		// 打印成功，buffer 中即为 JSON 字符串
		vision_log_debug("json_string:%s", json_buffer);

		unsigned char buffer[json_buffer_size + 10];
		unsigned short len = strlen(json_buffer) + 5;
		buffer[0] = Vot_command_head1;
		buffer[1] = Vot_command_head2;
		buffer[2] = Vot_command_head3;
		buffer[3] = (unsigned char)((len >> 8) & 0xFF);
		buffer[4] = (unsigned char)(len & 0xFF);
		buffer[5] = 0x01;
		buffer[6] = 0xCA;
		buffer[7] = 0xF6;
		for (int i = 0; i < strlen(json_buffer); i++)
		{
			buffer[8 + i] = json_buffer[i];
		}
		VotTools.RETURN_LENGTH = ((buffer[3] << 8) | buffer[4]);
		Data_Verification(buffer, VotTools.RETURN_LENGTH + 3);
		buffer[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
		buffer[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];
		if (global_sys_ctrl_info.m_transfer_callback)
		{
			(*global_sys_ctrl_info.m_transfer_callback)(buffer, VotTools.RETURN_LENGTH + 5);
		}
	}

	// 释放内存
	cJSON_Delete(root);
	return 0;
}

// 扩展默认参数配置
//ts_u08 ExtendedMenuParameters[44] = {0x56, 0x4F, 0x54, 0x00, 0x27, 0x00, 0xCA, 0xF5, 0x00, 0x00};

// 机芯导入JSON默认配置参数操作指令解析
void UART_OPR_ImportJsonParameters(u8 *Valid_data)
{

	uint16_t u16RawLen = (Valid_data[3] << 8) | Valid_data[4];
//若无需上位机参与解析，可屏蔽。即PC端上位机不用解析JSON，只负责导出保存JSON文件和导入JSON文件。
	#if 0
	struct _Usermenu_Parameter UsermenuDefParam;
	struct Menu2_Para SysMenuDefParam;
	// 解析出用户菜单默认参数配置
	if (u16RawLen == 0x0027 && Valid_data[9] == 0x00)
	{
		memcpy(&UsermenuDefParam, &UsermenuParameter, sizeof(struct _Usermenu_Parameter));
		if(ISP_write_UserMenu_def_para_cfg(UsermenuDefParam)!=0)return;
		if (ISP_read_UserMenu_def_para_cfg(&UsermenuDefParam) == 0) // 先读出来用户菜单默认参数，修改了再写回去
		{
			// memcpy(&UsermenuDefParam, &UsermenuParameter, sizeof(struct _Usermenu_Parameter));
			UsermenuDefParam.UseUserMenuDefParam = 1; // 标记使用用户菜单默认专用参数
			UsermenuDefParam.Bright = Valid_data[10];
			UsermenuDefParam.Contrast = Valid_data[11];
			UsermenuDefParam.SFilter = Valid_data[12];
			UsermenuDefParam.enhance_level = Valid_data[13];
			UsermenuDefParam.Palette = Valid_data[14];
			UsermenuDefParam.Mirror = Valid_data[15];
			UsermenuDefParam.ShutterMode = Valid_data[16];
			UsermenuDefParam.Enlarge = Valid_data[17];
			UsermenuDefParam.EE_Level = Valid_data[18];
			UsermenuDefParam.DDE_Level = Valid_data[19];
			UsermenuDefParam.AutoShutter = Valid_data[20];
			UsermenuDefParam.IsEnableEnligh = Valid_data[21];
			UsermenuDefParam.m_dev_env_mode = Valid_data[22];
			// 预留添加用户菜单默认参数
			//  ......

			// 写入flash
			if (ISP_write_UserMenu_def_para_cfg(UsermenuDefParam) == 0)
			{
				vision_log_debug("Write UsermenuDefParam to flash success.\n");
				memcpy(&UsermenuParameter, &UsermenuDefParam, sizeof(struct _Usermenu_Parameter));
				ISP_write_sys_cfg();
			}
			else
			{
				vision_log_debug("Write UsermenuDefParam to flash failure.\n");
				ISP_write_UserMenu_def_para_cfg(UsermenuDefParam); // 写用户菜单参数到flash失败，再试一次
			}
		}
	}

	// 解析出系统菜单默认参数配置
	if (u16RawLen == 0x0087 && Valid_data[9] == 0x01)
	{
		memcpy(&SysMenuDefParam, &SystemParaMenu2, sizeof(struct Menu2_Para));
		if(ISP_write_SysMenu_def_para_cfg(SysMenuDefParam)!=0)return;		  // 写系统菜单参数到flash
		if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 先读出来系统菜单默认参数，修改了再写回去
		{
			SysMenuDefParam.UseSysMenuDefParam = 1; // 标记使用系统菜单默认专用参数
			SysMenuDefParam.m_video_out_mode = Valid_data[10];
			SysMenuDefParam.Usermenu_SW = Valid_data[11];
			SysMenuDefParam.m_sffc_mode = Valid_data[12];
			SysMenuDefParam.m_sffc_level = (Valid_data[13] << 24) | (Valid_data[14] << 16) | (Valid_data[15] << 8) | Valid_data[16];
			SysMenuDefParam.m_osd_show_on_dev = Valid_data[17];
			SysMenuDefParam.m_ee_enhance_mode = Valid_data[18];
			SysMenuDefParam.m_cvbs_display_mode = Valid_data[19];
			SysMenuDefParam.BAD_SW = Valid_data[20];
			SysMenuDefParam.NUC_SW = Valid_data[21];
			SysMenuDefParam.m_bpc_cfg = Valid_data[22];
			SysMenuDefParam.m_bpc_view = Valid_data[23];
			SysMenuDefParam.m_local_isp_magic_value = (Valid_data[24] << 24) | (Valid_data[25] << 16) | (Valid_data[26] << 8) | Valid_data[27];
			SysMenuDefParam.m_bpc_ref_mode = Valid_data[28];
			SysMenuDefParam.m_st1_m = Valid_data[29];
			SysMenuDefParam.m_st1_s = Valid_data[30];
			SysMenuDefParam.m_det_st = Valid_data[31];
			SysMenuDefParam.SunPT_SW = Valid_data[32];
			SysMenuDefParam.S_Val = (Valid_data[33] << 8) | Valid_data[34];
			SysMenuDefParam.S_Range = (Valid_data[35] << 8) | Valid_data[36];
			SysMenuDefParam.P_Time_U16 = (Valid_data[37] << 8) | Valid_data[38];
			SysMenuDefParam.DetMode = Valid_data[39];
			SysMenuDefParam.NormalTemp.INT_C = Valid_data[40];
			SysMenuDefParam.NormalTemp.INT_T = Valid_data[41];
			SysMenuDefParam.NormalTemp.OOC_Par = Valid_data[42];
			SysMenuDefParam.LowTemp.INT_C = Valid_data[43];
			SysMenuDefParam.LowTemp.INT_T = Valid_data[44];
			SysMenuDefParam.LowTemp.OOC_Par = Valid_data[45];
			SysMenuDefParam.HighTemp.INT_C = Valid_data[46];
			SysMenuDefParam.HighTemp.INT_T = Valid_data[47];
			SysMenuDefParam.HighTemp.OOC_Par = Valid_data[48];
			SysMenuDefParam.offChange.INT_C = Valid_data[49];
			SysMenuDefParam.offChange.INT_T = Valid_data[50];
			SysMenuDefParam.offChange.OOC_Par = Valid_data[51];

			// 预留添加系统菜单默认参数, 若在此处添加参数，需要与PC上位机沟通通信协议
			//  ......
			//  SysMenuDefParam.m_bpc_cluster_on_flag= Valid_data[52];
			//  SysMenuDefParam.Bpc_ext_SW= Valid_data[53];
			//  SysMenuDefParam.m_raw_yuv_mode= Valid_data[54];
			//  SysMenuDefParam.m_raw_node_pos= Valid_data[55];
			//  SysMenuDefParam.m_uvc_enlarger_mode= Valid_data[56];
			//  for(int i=0;i<51;i++)
			//  SysMenuDefParam.SmartData408bit[i]= Valid_data[57+i];
			//  SysMenuDefParam.SmartData408bitValidFlag= Valid_data[108];
			//  SysMenuDefParam.m_mipi_lane_mode= Valid_data[109];
			//  SysMenuDefParam.m_force_detector_type= Valid_data[110];

			// 写入flash
			if (ISP_write_SysMenu_def_para_cfg(SysMenuDefParam) == 0)
			{
				vision_log_debug("Write SysMenuDefParam to flash success.\n");
				memcpy(&SystemParaMenu2, &SysMenuDefParam, sizeof(struct Menu2_Para));
				ISP_write_sys_menu2_cfg(SystemParaMenu2);
			}
			else
			{
				vision_log_debug("Write SysMenuDefParam to flash failure.\n");
				ISP_write_SysMenu_def_para_cfg(SysMenuDefParam); // 写系统菜单参数到flash失败，再试一次
			}
		}
	}
	// 解析出扩展默认参数配置
	if (u16RawLen == 0x0027 && Valid_data[9] == 0x02)
	{
		for (int i = 10; i < 32; i++)
		{
			ExtendedMenuParameters[i] = Valid_data[i];
		}

		// 预留添加扩展默认参数
		//  ......
		// menu1_default_parameters_init();
		// menu2_default_parameters_init();
		
		// vot_vpo_freeze();
		// // 软件主动断开电源，保护探测器
		// planck640c_stop();
		// // 自动重启
		// cli_simple_run_command("reboot");
	}
	#endif

	// 直接解析json文件
	if (u16RawLen > 40) //(56 4F 54 N+5 00	AC F5	JSON字符串(N字节)	CRC16)
	{
		char json_str[u16RawLen - 5];
		for (int i = 0; i < u16RawLen - 5; i++)
		{
			json_str[i] = (char)Valid_data[8 + i];
		}
		ParseJsonParameters(json_str);
	}
}
// 机芯导出JSON默认配置参数操作
void UART_OPR_ExportJsonParameters(u8 *Valid_data)
{
	//若无需上位机参与解析，可屏蔽。即PC端上位机不用解析JSON，只负责导出保存JSON文件和导入JSON文件。
	#if 0
	static unsigned char UserMenuParameters[44] = {0x56, 0x4F, 0x54, 0x00, 0x27, 0x00, 0xCA, 0xF6, 0x00, 0x00};
	static unsigned char SystemMenuParameters[140] = {0x56, 0x4F, 0x54, 0x00, 0x87, 0x00, 0xCA, 0xF6, 0x00, 0x00};
	struct _Usermenu_Parameter UsermenuDefParam;
	struct Menu2_Para SysMenuDefParam;

	if (Valid_data[9] == 0x00)
	{
		memcpy(&UsermenuDefParam, &UsermenuParameter, sizeof(struct _Usermenu_Parameter));
		ISP_write_UserMenu_def_para_cfg(UsermenuDefParam);			// 将当前RAM中的用户菜单默认参数写入Flash
		if (ISP_read_UserMenu_def_para_cfg(&UsermenuDefParam) == 0) // 读取用户菜单默认参数配置成功
		{
			// 组装用户菜单默认参数配置
			UserMenuParameters[10] = UsermenuDefParam.Bright;
			UserMenuParameters[11] = UsermenuDefParam.Contrast;
			UserMenuParameters[12] = UsermenuDefParam.SFilter;
			UserMenuParameters[13] = UsermenuDefParam.enhance_level;
			UserMenuParameters[14] = UsermenuDefParam.Palette;
			UserMenuParameters[15] = UsermenuDefParam.Mirror;
			UserMenuParameters[16] = UsermenuDefParam.ShutterMode;
			UserMenuParameters[17] = UsermenuDefParam.Enlarge;
			UserMenuParameters[18] = UsermenuDefParam.EE_Level;
			UserMenuParameters[19] = UsermenuDefParam.DDE_Level;
			UserMenuParameters[20] = UsermenuDefParam.AutoShutter;
			UserMenuParameters[21] = UsermenuDefParam.IsEnableEnligh;
			UserMenuParameters[22] = UsermenuDefParam.m_dev_env_mode;
			// 预留添加用户菜单默认参数
			//.....

			// 发送用户菜单默认参数配置
			UserMenuParameters[0] = Vot_command_head1;
			UserMenuParameters[1] = Vot_command_head2;
			UserMenuParameters[2] = Vot_command_head3;
			UserMenuParameters[3] = 0x00;
			UserMenuParameters[4] = 0x27;
			UserMenuParameters[5] = com_data.buff[VotTools.HEAD_FLAG + 5];
			UserMenuParameters[6] = 0xCA;
			UserMenuParameters[7] = 0xF6;
			UserMenuParameters[8] = com_data.buff[VotTools.HEAD_FLAG + 8];
			UserMenuParameters[9] = com_data.buff[VotTools.HEAD_FLAG + 9];
			VotTools.RETURN_LENGTH = ((UserMenuParameters[3] << 8) | UserMenuParameters[4]);
			Data_Verification(UserMenuParameters, VotTools.RETURN_LENGTH + 3);
			UserMenuParameters[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
			UserMenuParameters[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];

			// vision_log_debug("UserMenuParameter:");
			//  for(u16 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
			//  {
			//  	vision_log_debug("%02X ",UserMenuParameters[i]);
			//  }

			if (global_sys_ctrl_info.m_transfer_callback)
			{
				(*global_sys_ctrl_info.m_transfer_callback)(UserMenuParameters, VotTools.RETURN_LENGTH + 5);
			}
			// vot_uart_write(UserMenuParameters,VotTools.RETURN_LENGTH+5);
		}
	}
	else if (Valid_data[9] == 0x01)
	{
		memcpy(&SysMenuDefParam, &SystemParaMenu2, sizeof(struct Menu2_Para));
		ISP_write_SysMenu_def_para_cfg(SysMenuDefParam);		  // //将当前RAM中的系统菜单默认参数写入Flash
		if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 读取系统菜单默认参数配置成功
		{
			// 组装系统菜单默认参数配置
			SystemMenuParameters[10] = SysMenuDefParam.m_video_out_mode;
			SystemMenuParameters[11] = SysMenuDefParam.Usermenu_SW;
			SystemMenuParameters[12] = SysMenuDefParam.m_sffc_mode;
			SystemMenuParameters[13] = (SysMenuDefParam.m_sffc_level >> 24) & 0xFF;
			SystemMenuParameters[14] = (SysMenuDefParam.m_sffc_level >> 16) & 0xFF;
			SystemMenuParameters[15] = (SysMenuDefParam.m_sffc_level >> 8) & 0xFF;
			SystemMenuParameters[16] = SysMenuDefParam.m_sffc_level & 0xFF;
			SystemMenuParameters[17] = SysMenuDefParam.m_osd_show_on_dev;
			SystemMenuParameters[18] = SysMenuDefParam.m_ee_enhance_mode;
			SystemMenuParameters[19] = SysMenuDefParam.m_cvbs_display_mode;
			SystemMenuParameters[20] = SysMenuDefParam.BAD_SW;
			SystemMenuParameters[21] = SysMenuDefParam.NUC_SW;
			SystemMenuParameters[22] = SysMenuDefParam.m_bpc_cfg;
			SystemMenuParameters[23] = SysMenuDefParam.m_bpc_view;
			SystemMenuParameters[24] = (SysMenuDefParam.m_local_isp_magic_value >> 24) & 0xFF;
			SystemMenuParameters[25] = (SysMenuDefParam.m_local_isp_magic_value >> 16) & 0xFF;
			SystemMenuParameters[26] = (SysMenuDefParam.m_local_isp_magic_value >> 8) & 0xFF;
			SystemMenuParameters[27] = SysMenuDefParam.m_local_isp_magic_value & 0xFF;
			SystemMenuParameters[28] = SysMenuDefParam.m_bpc_ref_mode;
			SystemMenuParameters[29] = SysMenuDefParam.m_st1_m;
			SystemMenuParameters[30] = SysMenuDefParam.m_st1_s;
			SystemMenuParameters[31] = SysMenuDefParam.m_det_st;
			SystemMenuParameters[32] = SysMenuDefParam.SunPT_SW;
			SystemMenuParameters[33] = (SysMenuDefParam.S_Val >> 8) & 0xFF;
			SystemMenuParameters[34] = SysMenuDefParam.S_Val & 0xFF;
			SystemMenuParameters[35] = (SysMenuDefParam.S_Range >> 8) & 0xFF;
			SystemMenuParameters[36] = SysMenuDefParam.S_Range & 0xFF;
			SystemMenuParameters[37] = (SysMenuDefParam.P_Time_U16 >> 8) & 0xFF;
			SystemMenuParameters[38] = SysMenuDefParam.P_Time_U16 & 0xFF;
			SystemMenuParameters[39] = SysMenuDefParam.DetMode;
			SystemMenuParameters[40] = SysMenuDefParam.NormalTemp.INT_C;
			SystemMenuParameters[41] = SysMenuDefParam.NormalTemp.INT_T;
			SystemMenuParameters[42] = SysMenuDefParam.NormalTemp.OOC_Par;
			SystemMenuParameters[43] = SysMenuDefParam.LowTemp.INT_C;
			SystemMenuParameters[44] = SysMenuDefParam.LowTemp.INT_T;
			SystemMenuParameters[45] = SysMenuDefParam.LowTemp.OOC_Par;
			SystemMenuParameters[46] = SysMenuDefParam.HighTemp.INT_C;
			SystemMenuParameters[47] = SysMenuDefParam.HighTemp.INT_T;
			SystemMenuParameters[48] = SysMenuDefParam.HighTemp.OOC_Par;
			SystemMenuParameters[49] = SysMenuDefParam.offChange.INT_C;
			SystemMenuParameters[50] = SysMenuDefParam.offChange.INT_T;
			SystemMenuParameters[51] = SysMenuDefParam.offChange.OOC_Par;
			// 预留添加系统菜单默认参数
			//  for(u16 i = 52; i < 140; i++)
			//  {
			//  	SystemMenuParameters[i] = 0x00; //预留参数默认值
			//  }
			SystemMenuParameters[52] = SysMenuDefParam.m_bpc_cluster_on_flag;
			SystemMenuParameters[53] = SysMenuDefParam.Bpc_ext_SW;
			SystemMenuParameters[54] = SysMenuDefParam.m_raw_yuv_mode;
			SystemMenuParameters[55] = SysMenuDefParam.m_raw_node_pos;
			SystemMenuParameters[56] = SysMenuDefParam.m_uvc_enlarger_mode;
			for (int i = 0; i < 51; i++)
			{
				SystemMenuParameters[57 + i] = SysMenuDefParam.SmartData408bit[i];
			}
			SystemMenuParameters[108] = SysMenuDefParam.SmartData408bitValidFlag;
			SystemMenuParameters[109] = SysMenuDefParam.m_mipi_lane_mode;
			SystemMenuParameters[110] = SysMenuDefParam.m_force_detector_type;
			SystemMenuParameters[111] =	SystemParaMenu2.m_force_hardware_port_type;

			// 发送系统菜单默认参数配置
			SystemMenuParameters[0] = Vot_command_head1;
			SystemMenuParameters[1] = Vot_command_head2;
			SystemMenuParameters[2] = Vot_command_head3;
			SystemMenuParameters[3] = 0x00;
			SystemMenuParameters[4] = 0x87;
			SystemMenuParameters[5] = com_data.buff[VotTools.HEAD_FLAG + 5];
			SystemMenuParameters[6] = 0xCA;
			SystemMenuParameters[7] = 0xF6;
			SystemMenuParameters[8] = com_data.buff[VotTools.HEAD_FLAG + 8];
			SystemMenuParameters[9] = com_data.buff[VotTools.HEAD_FLAG + 9];
			VotTools.RETURN_LENGTH = ((SystemMenuParameters[3] << 8) | SystemMenuParameters[4]);
			Data_Verification(SystemMenuParameters, VotTools.RETURN_LENGTH + 3);
			SystemMenuParameters[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
			SystemMenuParameters[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];
			// vision_log_debug("SystemMenuParameter:");
			// for(u16 i = 0; i < VotTools.RETURN_LENGTH+5; i++)
			// {
			// 	vision_log_debug("%02X ",SystemMenuParameters[i]);
			// }

			if (global_sys_ctrl_info.m_transfer_callback)
			{
				(*global_sys_ctrl_info.m_transfer_callback)(SystemMenuParameters, VotTools.RETURN_LENGTH + 5);
			}
			// vot_uart_write(SystemMenuParameters,VotTools.RETURN_LENGTH+5);
		}
	}
	else if (Valid_data[9] == 0x02)
	{
		// 预留添加扩展菜单默认参数配置发送
		// ExtendedMenuParameters[0] = Vot_command_head1;
		// ExtendedMenuParameters[1] = Vot_command_head2;
		// ExtendedMenuParameters[2] = Vot_command_head3;
		// ExtendedMenuParameters[3] = 0x00;
		// ExtendedMenuParameters[4] = 0x27;
		// ExtendedMenuParameters[5] = 0x00; // com_data.buff[VotTools.HEAD_FLAG+5];
		// ExtendedMenuParameters[6] = 0xCA;
		// ExtendedMenuParameters[7] = 0xF6;
		// ExtendedMenuParameters[8] = com_data.buff[VotTools.HEAD_FLAG + 8];
		// ExtendedMenuParameters[9] = com_data.buff[VotTools.HEAD_FLAG + 9];
		// VotTools.RETURN_LENGTH = ((ExtendedMenuParameters[3] << 8) | ExtendedMenuParameters[4]);
		// Data_Verification(ExtendedMenuParameters, VotTools.RETURN_LENGTH + 3);
		// ExtendedMenuParameters[VotTools.RETURN_LENGTH + 3] = VotArrays.Rcvbuf[0];
		// ExtendedMenuParameters[VotTools.RETURN_LENGTH + 4] = VotArrays.Rcvbuf[1];
		// if (global_sys_ctrl_info.m_transfer_callback)
		// {
		// 	(*global_sys_ctrl_info.m_transfer_callback)(ExtendedMenuParameters, VotTools.RETURN_LENGTH + 5);
		// }
		Create_json_string(); // 输出标准json格式字符串给上位机
	}
	#endif
	if (Valid_data[9] == 0x02)
	{
		Create_json_string(); // 输出标准json格式字符串给上位机
	}
}
