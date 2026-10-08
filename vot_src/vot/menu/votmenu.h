/*
 * votmenu.h
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

#ifndef SRC_MENU_MENU_H_
#define SRC_MENU_MENU_H_
// #include <base/ts_type.h>
#include "../myheader.h"
#include "config.h"
#include <stdbool.h>
#include "IDE_tree.h"
#include <cpl/cpl_mutex.h>
#include <uapi/vpo_v100.h>
#include <api.h>
#include "uapi/isp_reg.h"
#include "common/DistanceCoeffs.h"

#define EXTBPC_POSITION_MAX 7

typedef ts_u32(FuncVotTransfer)(ts_u08 *payload, ts_u32 payloadlen);


#define    ST_0_INTERVAL_S_DEFAULT          2
#define    ST_0_TOTAL_DURATION_S_DEFAULT    13
#define    ST_0_INTERVAL_S_MAX          20
#define    ST_0_TOTAL_DURATION_S_MAX    60

#define KEY_UP 0xFB
#define KEY_DOWN 0xFD
#define KEY_LEFT 0xEF
#define KEY_RIGHT 0xF7
#define KEY_OK 0xFE
#define KEY_ESC 0x7F
#define KEY_SAVE 0xBF
#define KEY_AUTO 0xDF

#define DETECTOR_NEW_VERSION_1 0xe8
#define DETECTOR_NEW_VERSION_2 0xe9

// #define SUPPORT_CVBS	//因USB与CVBS的版本颜色索引值一致，所以暂时使用框架菜单不添加颜色

#ifdef SUPPORT_CVBS

#define BACKGROUND_COLOR 0 // 未选中项的背景颜色

#define SELECT_ITEM_COLOR 0 // 选中项的背景颜色

#define FONT_COLOR 5 // 字体颜色,此处暂定为5灰色,其他颜色为白色

#define CLEAR_COLOR 0 // 清除背景色

#define VERSION_YLINE 424 // 版本显示的Y轴位置

#else

#define BACKGROUND_COLOR 0 // 未选中项的背景颜色

#define SELECT_ITEM_COLOR 0 // 选中项的背景颜色

#define FONT_COLOR 5 // 字体颜色灰色,其他颜色为白色

#define CLEAR_COLOR 0 // 清除背景色

#define VERSION_YLINE 330 // 版本显示的Y轴位置

#endif

#define BACKGROUND_LAYER 5
#define PARAMETER_LAYER 4
#define FONT_LAYER 3
#define ICON_LAYER 2

#define AutoSimiRemind 0

#define LOCAL_ISP_MAGIC_NUM 0x12345678

#define DEFAULT_SUN_PT_GRAY_LEVEL 1000
#define MAX_SUN_PT_GRAY_LEVEL 1025

#define MAX_SUN_PT_THRESHOLD 65535 //(640*512)
#define DEFAULT_SUN_PT_THRESHOLD 20

#define MAX_SUN_PT_TIME_SECS 600
#define DEFAULT_SUN_PT_TIME_SECS 60 // 20

#define VOT_DEBUG_OUTPUT_CONFIRM_MAGIC (0x3456789A)

typedef struct KeyDef
{
	ts_u08 old;
	ts_u08 new;
	ts_u08 one;
	ts_u08 ten;
	ts_u08 hundred;
	ts_u08 valid;
} KeyDef;

extern KeyDef key;


typedef enum
{
	VOT_BOOT_MODE_NORMAL = 0,
	VOT_BOOT_MODE_FASTBOOT,
	VOT_BOOT_MODE_BUTT
}VOT_BOOT_MODE_ENUM;

typedef enum
{
	VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B = 0, // 同时升级A和B分区
	VOT_UPDATE_FIRMWARE_POLICY_ONLY_A,		// 只升级A分区
	VOT_UPDATE_FIRMWARE_POLICY_ONLY_B,		// 只升级B分区
	VOT_UPDATE_FIRMWARE_POLICY_BUTT
}VOT_UPDATE_FIRMWARE_POLICY_ENUM;


typedef enum
{
	VOT_ASIC_NAND_FLASH_128MB = 0,
	VOT_ASIC_NOR_FLASH_8MB,
	VOT_ASIC_NAND_FLASH_BUTT
}VOT_ASIC_NAND_FLASH_ENUM;

typedef enum
{
	VOT_POD_TYPE_WHITE_GRAY = 0,
	VOT_POD_TYPE_BLACK,
	VOT_POD_TYPE_GRAY,
	VOT_POD_TYPE_BUTT
} VOT_POD_TYPE_ENUM;

typedef enum
{
	TEMP_DRIFT_COMPENSATION_MODE_OFF = 0,
	TEMP_DRIFT_COMPENSATION_MODE_ON,
	TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION,
	TEMP_DRIFT_COMPENSATION_MODE_BUTT
} TEMP_DRIFT_COMPENSATION_MODE_ENUM;

typedef enum
{
	TEMP_DRIFT_COMPENSATION_AI_APP_OFF = 0,
	TEMP_DRIFT_COMPENSATION_AI_POST_PROCESS_ON,
	TEMP_DRIFT_COMPENSATION_AI_APP_BUTT
} TEMP_DRIFT_COMPENSATION_AI_APP_ENUM;



typedef enum
{
	VOT_LOG_TYPE_MENU1 = 0,
	VOT_LOG_TYPE_MENU2,
	VOT_LOG_TYPE_MOTOR,
	VOT_LOG_TYPE_BUTT
} VOT_LOG_TYPE_ENUM;


typedef enum
{
	VOT_MENU_OSD_RES_640 = 0, 
	VOT_MENU_OSD_RES_384 ,
	VOT_MENU_OSD_RES_1280 ,
	VOT_MENU_OSD_RES_BUTT
} VOT_MENU_ENUM;

typedef enum
{
	VOT_SWITCH_OFF = 0,
	VOT_SWITCH_ON =1,
	VOT_SWITCH_ON_LIKE0=0,
	VOT_SWITCH_OFF_LIKE1=1,
	VOT_SWITCH_BUTT
} VOT_SWITCH_ENUM;


typedef enum
{
	VOT_MODE_AUTO = 0,
	VOT_MODE_MANUAL ,
	VOT_MODE_BUTT
} VOT_MODE_ENUM;

typedef enum
{
	VOT_ISO_POSITION_MODE_INSIDE =0,
	VOT_ISO_POSITION_MODE_OUTSIDE,
	VOT_ISO_POSITION_MODE_ABOVE,
	VOT_ISO_POSITION_MODE_BELOW, 
	VOT_ISO_POSITION_MODE_BUTT
} VOT_ISO_POSITION_MODE_ENUM;

typedef enum
{
	VOT_ISO_COLOR_MODE_GREEN =0,
	VOT_ISO_COLOR_MODE_RED,
	VOT_ISO_COLOR_MODE_BLACK,
	VOT_ISO_COLOR_MODE_WHITE, 
	VOT_ISO_COLOR_MODE_BUTT
} VOT_ISO_COLOR_MODE_ENUM;

typedef enum
{
	SUN_PT_MODE_AUTO = 0,
	SUN_PT_MODE_MANUAL ,
	SUN_PT_MODE_BUTT
} SUN_PT_MODE_ENUM;

typedef enum
{
	VOT_COLOR_PALETTE_MODE_GRAY_TAB = 0, //白热
	VOT_COLOR_PALETTE_MODE_GRAY_INV_TAB, //黑热
	VOT_COLOR_PALETTE_MODE_IRON_TAB,		//铁红
	VOT_COLOR_PALETTE_MODE_INV_IRON_TAB,	//反铁红
	VOT_COLOR_PALETTE_MODE_RAINBOW_TAB,		//彩虹
	VOT_COLOR_PALETTE_MODE_RED_HOT_TAB,		//红热
	VOT_COLOR_PALETTE_MODE_LAVA_TAB,		//熔岩
	VOT_COLOR_PALETTE_MODE_YU_HONG_TAB,		//羽红
	VOT_COLOR_PALETTE_MODE_CHUN_YU_HONG_TAB,  //春羽红
	VOT_COLOR_PALETTE_MODE_HU_PO_TAB,		//琥珀
	VOT_COLOR_PALETTE_MODE_TIE_HUI_TAB,		//铁灰
	VOT_COLOR_PALETTE_MODE_LAN_HEI_TAB,		//蓝黑
	VOT_COLOR_PALETTE_MODE_LAN_HONG_TAB,		//蓝红
	VOT_COLOR_PALETTE_MODE_DUO_SE_TAB,		//多色
	VOT_COLOR_PALETTE_MODE_RGB_FAN_SE_TAB2,	//RGB反色
	VOT_COLOR_PALETTE_MODE_IRON_HOT2_TAB,	//铁红T 定制枪瞄伪彩
	// VOT_COLOR_PALETTE_MODE_RED_BLACK_TAB,   //红黑

	VOT_COLOR_PALETTE_MODE_BUTT
}VOT_COLOR_PALETTE_MODE_ENUM;

typedef enum
{
	LUMA_PERCENT_MODE_AUTO = 0,
	LUMA_PERCENT_MODE_25 =25,
	LUMA_PERCENT_MODE_50 =50,
	LUMA_PERCENT_MODE_75 =75,
	LUMA_PERCENT_MODE_100 =100,
	LUMA_PERCENT_MODE_BUTT
} LUMA_PERCENT_MODE_ENUM;

typedef enum
{
	CONTRAST_PERCENT_MODE_AUTO = 0,
	CONTRAST_PERCENT_MODE_25 =25,
	CONTRAST_PERCENT_MODE_50 =50,
	CONTRAST_PERCENT_MODE_75 =75,
	CONTRAST_PERCENT_MODE_100 =100,
	CONTRAST_PERCENT_MODE_BUTT
} CONTRAST_PERCENT_MODE_ENUM;

typedef enum
{
	RZ_RATIO_MODE_1X = 0,
	RZ_RATIO_MODE_2X ,
	RZ_RATIO_MODE_3X,
	RZ_RATIO_MODE_4X,
	RZ_RATIO_MODE_BUTT
} RZ_RATIO_MODE_ENUM;

typedef enum
{
	PIP_MODE_NONE = 0,
	PIP_MODE_P1,
	PIP_MODE_P2,
	PIP_MODE_P3,
	PIP_MODE_P4,
	PIP_MODE_P5,
	PIP_MODE_P6,
	PIP_MODE_P7,
	PIP_MODE_P8,
	PIP_MODE_P9,
	PIP_MODE_ANY_WHERE,
	PIP_MODE_BUTT
} PIP_MODE_ENUM;

typedef enum
{
	HOT_SPOT_TRACKING_MODE_NONE = 0,
	HOT_SPOT_TRACKING_MODE_MAX,
	HOT_SPOT_TRACKING_MODE_MIN,
	HOT_SPOT_TRACKING_MODE_MAX_MIN,
	HOT_SPOT_TRACKING_MODE_BUTT
}HOT_SPOT_TRACKING_MODE_ENUM;


typedef enum
{
	BT_TIMING_SEQ_MODE_NONE = 0,
	BT_TIMING_SEQ_MODE_WIN2_50HZ,
	BT_TIMING_SEQ_MODE_WIN2_30HZ,
	BT_TIMING_SEQ_MODE_WIN2_25HZ,
	BT_TIMING_SEQ_MODE_BUTT
} BT_TIMING_SEQ_MODE_ENUM;


typedef enum
{
	MIPI_LANE_MODE_2_LANES = 0,
	MIPI_LANE_MODE_4_LANES,
	MIPI_LANE_MODE_BUTT
} MIPI_LANE_MODE_ENUM;

typedef enum
{
	USER_METADATA_DISABLE = 0,
	USER_METADATA_ENABLE ,
	USER_METADATA_BUTT
} USER_METADATA_ENUM;


typedef enum
{
	T10_CPU_TYPE_N6 = 0,
	T10_CPU_TYPE_A12,
	T10_CPU_TYPE_A6,
	T10_CPU_TYPE_N6E,
	T10_CPU_TYPE_BUTT
} T10_CPU_TYPE_ENUM;


typedef enum
{
	FOCUS_TYPE_PRIME_LENS = 0, 				//定焦
	FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER,	//调焦（电位器）
	FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER,		//变焦（电位器）
	FOCUS_TYPE_FOCUS_LENS_ENCODER, 			//调焦（编码器）
	FOCUS_TYPE_ZOOM_LENS_ENCODER, 			//变焦（编码器）
	FOCUS_TYPE_NMOD,				//不可修改
	FOCUS_TYPE_BUTT
} FOCUS_TYPE_ENUM;

typedef enum
{
	FUNCTION_TYPE_INFRARED_IMAGING = 0, 		//红外观察图像
	FUNCTION_TYPE_INFRARED_THERMOMETRY , 		//红外测温
	FUNCTION_TYPE_FIXED,
	FUNCTION_TYPE_BUTT
} FUNCTION_TYPE_ENUM;


typedef enum
{
	CUSTOMER_TYPE_STANDARD = 0, 
	CUSTOMER_TYPE_DSW,			//深圳鼎深微-UAD3机芯
	CUSTOMER_TYPE_GENERAL,		//中性版本，不带vot属性的
	CUSTOMER_TYPE_ZM,           //卓目定制项目
	CUSTOMER_TYPE_HAND,         //手持机定制项目，用于开启384超分768且T8模式RAW+YUV输出
	
	CUSTOMER_TYPE_NMOD,			//不可修改
	CUSTOMER_TYPE_BUTT
} CUSTOMER_TYPE_ENUM;

typedef enum
{
	FORCE_DETECTOR_TYPE_VOT_640X512 = 0,
	FORCE_DETECTOR_TYPE_VOT_384X288,
	FORCE_DETECTOR_TYPE_VOT_WLP_640X512,
	FORCE_DETECTOR_TYPE_VOT_WLP_384X288,
	FORCE_DETECTOR_TYPE_VOT_640X512_9C1,
	FORCE_DETECTOR_TYPE_VOT_384X288_9C1,
	FORCE_DETECTOR_TYPE_VOT_640X512_C122,
	FORCE_DETECTOR_TYPE_VOT_384X288_C122,
	FORCE_DETECTOR_TYPE_RTD_640X512,
	FORCE_DETECTOR_TYPE_VOT_1280X1024,
	FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024,
	FORCE_DETECTOR_TYPE_RTD_1280X1024,
	FORCE_DETECTOR_TYPE_RTD_384X288,
	FORCE_DETECTOR_TYPE_NMOD,				//不可修改
	FORCE_DETECTOR_TYPE_BUTT
} FORCE_DETECTOR_TYPE_ENUM;



typedef enum
{
	FORCE_HARDWARE_PORT_TYPE_UAD3_LITE = 0, 
	FORCE_HARDWARE_PORT_TYPE_UAD3,
	FORCE_HARDWARE_PORT_TYPE_UAW,
	FORCE_HARDWARE_PORT_TYPE_UAW_LITE,
	FORCE_HARDWARE_PORT_TYPE_NMOD,			//不可修改
	FORCE_HARDWARE_PORT_TYPE_BUTT
} FORCE_HARDWARE_PORT_TYPE_ENUM;

typedef enum
{
	RAW_YUV_MODE_NONE = 0,
	RAW_YUV_MODE_RAW_YUV,
	RAW_YUV_MODE_YUV_RAW,
	RAW_YUV_MODE_BUTT
} RAW_YUV_MODE_ENUM;

typedef enum
{
	VIDEO_OUTPUT_MODE_ENUM_CVBS = 0,
	VIDEO_OUTPUT_MODE_ENUM_UVC,
	VIDEO_OUTPUT_MODE_ENUM_BT656,
	VIDEO_OUTPUT_MODE_ENUM_BT601,
	VIDEO_OUTPUT_MODE_ENUM_MIPI,
	// VIDEO_OUTPUT_MODE_ENUM_LIBUSB,
	VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG,
	VIDEO_OUTPUT_MODE_ENUM_BUTT,
} VIDEO_OUTPUT_MODE_ENUM;

typedef enum
{
	SYSTEM_MODE_NORMAL = 0,
	SYSTEM_MODE_BPC,
	SYSTEM_MODE_UPDATE_FIRMWARE,
} SYSTEM_MODE_ENUM;

typedef enum SFFC_MODE_ENUM
{
	SFFC_MODE_OFF = 0,
	SFFC_MODE_AUTO,
	SFFC_MODE_MANUAl,
	SFFC_MODE_PRE,
	SFFC_MODE_BUTT,
} SFFC_MODE_ENUM_t;

typedef enum OPERATION_MODE
{
	OPERATION_MODE_RESTORE = 0,
	OPERATION_MODE_BACKUP,
	OPERATION_MODE_NONE,
	OPERATION_MODE_BUTT,
} OPERATION_MODE_t;

typedef enum
{
	DEVICE_MODE_INDOOR = 0,
	DEVICE_MODE_OUTDOOR,
	DEVICE_MODE_OUTDOOR_K1,			//low atd
	DEVICE_MODE_OUTDOOR_FLY,		//pod view
	DEVICE_MODE_OUTDOOR_FLY_ENH,	//pod view2 在pod view上进一步压制噪声
	DEVICE_MODE_OUTDOOR_NUC_MANUAL, //手动调节亮度，只需要NUC数据
	DEVICE_MODE_OUTDOOR_RTD6, 		//适配rtd6122 探测器配置
	DEVICE_MODE_OUTDOOR_GUAN_NIAO,  //观鸟场景
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1,  //low atd dde v1
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2,  //low atd dde v2
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3,  //low atd dde v3
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4,  //guan niao 2 T10ISP_20260609195758.bin
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5,  //T10ISP_20260609145336-均匀面判断.bin
	DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6,  //T10ISP_20260609145336-均匀面判断-降噪强.bin
	DEVICE_MODE_NORMAL_END, 			
	DEVICE_MODE_CUSTOMER_START=64, 			//后面的是客户定制场景
	DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT, //观鸟场景 高亮勾边
	DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT,  //观鸟场景 白热
	DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT,  //观鸟场景 黑热
	DEVICE_MODE_OUTDOOR_CITY_IRON_HOT,   //观鸟场景 铁红
	DEVICE_MODE_OUTDOOR_CITY_RED_HOT,  	 //观鸟场景 红热
	DEVICE_MODE_OUTDOOR_BPC,
	DEVICE_MODE_ISO_MODE,
	DEVICE_MODE_384_START=128,
	DEVICE_MODE_384_LOW_ATD,
	DEVICE_MODE_384_LOW_ATD_RB,      //解决高温区黑点问题
	DEVICE_MODE_384_END,
	DEVICE_MODE_1280_START=192,
	DEVICE_MODE_1280_END,
	DEVICE_MODE_BUTT,
} DEVICE_MODE_ENUM;


typedef enum
{
	DEVICE_MODE_1280_R1 = 0,
	DEVICE_MODE_1280_R2,
	DEVICE_MODE_1280_R2_1,
	DEVICE_MODE_1280_R2_2,
	DEVICE_MODE_1280_R2_3,			
	
	DEVICE_MODE_1280_BUTT,
} PLANKC1280_DEVICE_MODE_ENUM;


typedef enum
{
	DEVICE_CVBS_DISPLAY_MODE_ENLARGE = 0,
	DEVICE_CVBS_DISPLAY_MODE_ORG,
	DEVICE_CVBS_DISPLAY_MODE_BUTT,
} DEVICE_DISPLAY_MODE_ENUM;

typedef enum
{
	OUTPUT_MODE_ENLARGE_ON = 0,
	OUTPUT_MODE_ENLARGE_OFF,
	OUTPUT_MODE_ENLARGE_BUTT,
} OUTPUT_MODE_ENLARGE_ENUM;

typedef enum
{
	DEVICE_FPS_MODE_50FPS = 0,
	DEVICE_FPS_MODE_40FPS,
	DEVICE_FPS_MODE_30FPS,
	DEVICE_FPS_MODE_25FPS,
	DEVICE_FPS_MODE_20FPS,
	DEVICE_FPS_MODE_10FPS,
	DEVICE_FPS_MODE_5FPS,
	DEVICE_FPS_MODE_BUTT
} DEVICE_FPS_MODE_ENUM;

typedef enum
{
	EE_ENHANCE_MODE_OFF = 0,
	EE_ENHANCE_MODE_WHITE,
	EE_ENHANCE_MODE_BLACK,
	EE_ENHANCE_MODE_WHITE_BLACK,
	EE_ENHANCE_MODE_BUTT,
} EE_ENHANCE_MODE_ENUM;

typedef enum
{
	BPC_MODE_ON_CVBS = 0,
	BPC_MODE_ON_UVC,
	BPC_MODE_ON_BUTT,
} BPC_MODE_ON_ENUM;

typedef enum
{
	SENSOR_STATUS_POWER_DOWN = 0,
	SENSOR_STATUS_WORKING,
} VOT_SENSOR_STATUS_ENUM;

typedef enum
{
	OSD_POSITION_ON_CVBS = 0,
	OSD_POSITION_ON_UVC,
	OSD_POSITION_ON_BUTT,
} OSD_POSITION_ON_ENUM;

typedef enum
{
	DET_TEMP_MODE_AUTO = 0,
	DET_TEMP_MODE_LOW,
	DET_TEMP_MODE_NORMAL,
	DET_TEMP_MODE_HIGH,
	DET_TEMP_MODE_OFF,
	DET_TEMP_MODE_BUTT,

} DET_TEMP_MODE_ENUM;

typedef enum
{
	MIRROR_MODE_OFF = 0,
	MIRROR_MODE_X,
	MIRROR_MODE_Y,
	MIRROR_MODE_X_Y,
	MIRROR_MODE_BUTT,
} MIRROR_MODE_ENUM;

typedef enum
{
	NPU_SR_OFF = 0,
	NPU_SR_640X512_960X768,
	NPU_SR_640X512_1280X1024,
	NPU_SR_384X288_640X512,
	NPU_SR_384X288_768X576,
	NPU_SR_BUTT,
} NPU_SR_ENUM;

typedef enum
{
	TEMP_REGULARITY_GENERAL = 0,
	TEMP_REGULARITY_QC,
	TEMP_REGULARITY_HAND384,
	TEMP_REGULARITY_BUTT,
} TEMP_REGULARITY_ENUM;

typedef struct user_cfg_node_header_s
{
	ts_char name[16];
	ts_u32 offset;
	ts_u32 size;
} user_cfg_node_header_s_t;

typedef struct user_cfg_header_s
{
	ts_u32 base_addr;		// 基地址
	ts_u32 cfg_total_bytes; // sizeof(user_cfg_header_s) + isp cfg + ...
	ts_u32 cfg_node_nums;	// 有效配置数量
	ts_u32 reserved_bytes;
	user_cfg_node_header_s_t cfg_node_array[64]; // 定义最多64块配置数
} user_cfg_header_s_t;

/***********************************************************/
typedef struct {
    ts_f32 factor;   /* Temp × factor */
} DIST_CORR_ITEM;

typedef enum {
    UVC_YUV_FORMAT_YUYV = 0,
    UVC_YUV_FORMAT_UYVY = 1,
	UVC_YUV_FORMAT_BUTT,
} uvc_yuv_format_e;

typedef struct SysCtrlInfo_s
{
	ts_cpl_mutex_s_t m_bottom_osd_info_mutex; // 刷新底部状态osd句柄锁
	ts_cpl_mutex_s_t m_freeze_img_mutex;	  // 图像冻结锁
	ts_cpl_mutex_s_t m_prompt_osd_info_mutex; // 提示词行osd句柄锁
	ts_cpl_mutex_s_t m_dump_ad_mutex;		  // dump ad 值句柄锁
	ts_cpl_mutex_s_t m_shutter_fd_mutex;	  // shutter句柄锁
	ts_cpl_mutex_s_t m_viu_stats_mutex;		  // viu获取温度和tes值句柄锁
	ts_cpl_mutex_s_t m_bottom_status_mutex;	  // 刷新状态栏锁

	ts_cpl_mutex_s_t m_special_osd_info_mutex; // 刷新底部状态osd句柄锁
	ts_cpl_mutex_s_t m_uart1_msg_mutex;		   // 处理串口锁

	ts_cpl_mutex_s_t m_bottom_status_force_mutex;
	ts_cpl_mutex_s_t m_det_switch_force_mutex;

	ts_cpl_mutex_s_t m_det_update_ad_mutex; // 单独刷新ad值

	ts_nodebuf_s_t *pst_framebuffer_osd;
	ts_nodebuf_s_t *m_bottom_info_osd_node;
	ts_nodebuf_s_t *m_special_info_osd_node;
	ts_nodebuf_s_t *m_cvbs_cursor_osd_dot;
	ts_nodebuf_s_t *m_cvbs_cursor_osd_rect;
//	ts_nodebuf_s_t *m_384_special_info_osd_node;
//	ts_nodebuf_s_t *m_640_special_info_osd_node;
//	ts_nodebuf_s_t *m_1280_special_info_osd_node;

	ts_vpo_chn_attr_s_t st_cvbs_vpo_attr; // CVBS设备节点的参数

	int m_bottom_osd_info_visible_flag;

	ts_char m_prompt_info[64];

	ts_u08 m_freeze_img_flag;			 // 冻结图像标记，0：不冻结，1：冻结
	ts_u08 m_dev_soft_info_visible_flag; // 版本信息是否显示
	ts_u08 m_device_info_visible_flag;	 // 设备信息是否显示

	ts_s08 m_prompt_keep_seconds; //-1 : 一直显示 , 非零值对应的second
	ts_u08 m_prompt_refresh_flag; // 0：需要更新，1：不需要更新
	ts_u08 m_prompt_visible_flag; // 0: 关闭， 1：开启显示

	ts_u16 m_system_mode; // 系统模式 SYSTEM_MODE_ENUM
	ts_u32 m_firmware_len;

	// volatile ts_u16 m_enter_BPC_manual_flag;			//1：坏点校正手动模式界面，1：正常操作
	ts_u16 m_BPC_cursor_pos_x; // 坏点手动校准时候的cursor的x坐标
	ts_u16 m_BPC_cursor_pos_y; // 坏点手动校准时候的cursor的用y坐标

	ts_u16 m_prompt_cursor_pos_x; // 坏点校正时，提示词显示位置坐标点x
	ts_u16 m_prompt_cursor_pos_y; // 坏点校正时，提示词显示位置坐标点y

	ts_u16 m_det_temp_ad_value;	   // 保存探测器温度ad值
	ts_f32 m_det_temp_value;	   // 保存探测器温度ad值
	ts_u16 m_det_tes_ad_value;	   // 保存探测器tes值
	ts_u16 m_det_ad_mean_value;	   // 保存探测器ad均值
	ts_u16 m_ignore_uart_msg_flag; // 是否忽略串口消息，用于匹配中某些参数时，避免资源竞争。 0：处理uart消息，其他值：忽略uart消息

	ts_u32 m_high_temp_ad_buff; // 用于保存nuc校准的高温60 ad值
	ts_u32 m_low_temp_ad_buff;	// 用于保存nuc校准的低温10 ad值

	ts_u16 m_high_ad_mean_value; // 高温ad均值
	ts_u16 m_low_ad_mean_value;	 // 低温ad均值

	ts_isp_cfg_reg_s_t m_isp_cfg; // isp配置寄存器值

	ts_cpl_timeval_s_t m_tv_start;		  // 系统启动的是时间戳，粗略的计算
	FuncVotTransfer *m_transfer_callback; // 实际消息回传接口

	ts_u08 m_sffc_mode_backup;
	ts_u08 m_shutter_mode_backup;
	ts_u08 m_shutter_mode;
	ts_u32 u32_lut_base_addr;
	volatile ts_u08 m_sys_init_finished_flag;

	ts_u64 isp_auto_cut_run_cnt_u64; // 上电后，打档线程运行次数
	ts_u32 isp_cut_cnt_u32;			 // 上电后，打档次数
	ts_u32 isp_cut_time_sec_u32;	 // 上电后，累计打档的多长时间，正常是线性累加
	ts_u16 isp_temp_ad_value_u16;	 // 当前探测器温度AD值

	ts_u32 m_cur_bak_ad_mean_u32; // 探测器bak均值

	ts_u08 m_det_calibrationing_flag; // 机芯正在标定中
	ts_u08 m_sun_pt_force_flag;		  // 阳光保护强制开启

	ts_s32 m_sensor_s32_img_w;
	ts_s32 m_sensor_s32_img_h;

	ts_u08 m_shutter_force_close_flag; // 强制关闭挡片

	ts_u08 m_det_switch_force_flag;		  // 请求强制切换det
	ts_u08 m_det_switching_flag;		  // 正在切换det参数中
	ts_u08 m_bottom_statistic_force_flag; // 菜单二底部统计信息强制刷新

	ts_u16 m_det_temp_ad_value_u16; // 保存探测器温度ad值
	ts_f32 m_det_temp_value_f32;	// 保存探测器温度浮点值

	ts_u08 m_show_det_cut_info; // 显示cut 相关的统计信息

	ts_u08 m_det_cur_mode;

	ts_u08 m_uvc_enlarger_mode_in_bpc; // 用于指示bpc矫正是,uvc是否开启放大
	ts_u32 m_json_buff;				   // 用于保存json字符串的buff地址

	ts_u08 m_t10_tpu_type;  			//cpu 类型

	ts_u32 m_user_metadata_addr; 	//增加用户冗余数据的地址
	ts_u32 m_user_metadata_size; 	//增加用户冗余数据的字节数

	ts_isp_fire_statis_s_t m_isp_fire_statis;   	//fire模块统计信息
	ts_u08 m_update_fire_statis_force_flag; 		// fire模块统计信息强制刷新标志
	ts_cpl_mutex_s_t m_update_fire_statis_force_mutex;
	ts_cpl_mutex_s_t m_update_fire_statis_mutex;
	ts_nodebuf_s_t *m_hst_min_cursor_osd_dot;	
	ts_nodebuf_s_t *m_hst_max_cursor_osd_dot;	
	ts_u32 m_backup_hst_mode;

	ts_nodebuf_s_t *mTemplut_OsdCursor_dot_1st;	// 定义两个用于测温的光标1句柄
	ts_nodebuf_s_t *mTemplut_OsdCursor_dot_2st;	// 定义两个用于测温的光标2句柄
	ts_nodebuf_s_t *mTemplut_OsdCursor_dot_3st;	// 定义两个用于测温的光标3句柄

	ts_nodebuf_s_t *m_lutCapture_maxCursor_dot;	// 定义用于捕获最大值的光标句柄
	ts_nodebuf_s_t *m_lutCapture_minCursor_dot;	// 定义用于捕获最小值的光标句柄

	ts_cpl_mutex_s_t m_templut_osd_info_mutex;	// 定义用于区域的的OSD互斥锁
	ts_nodebuf_s_t *m_templut_info_osd_node;	// 定义用于区域的OSD节点句柄

	ts_cpl_mutex_s_t m_fireModule_get_ad_mutex;	// FIRE模块获取AD的调用锁

	ts_nodebuf_s_t *m_Crosshair_info_osd_dot;	// 定义菜单一中心光标OSD节点句柄

	ts_u08 m_run_feeding_flag; 					//是否进行喂狗，用于配置响应，机器重启
	ts_cpl_mutex_s_t m_feeding_flag_mutex;	

	DIST_CORR_ITEM g_dist_corr_tbl[3][DIST_CORR_MAX_LENS][DIST_CORR_MAX_DIST];	//SystemParaMenu2.Usermenu_SW(1/2)

	ts_f32 votShutter_temp_at_powerup;			// 单点采集：开机快门温度
	ts_u08 Usermenu_SW_Switching;   			// 单点采集：测温挡位(0/1/2/3)是否正在切换
	ts_u08 prevent_burn_flag;					// 单点采集：防灼烧标志
	ts_u08 Shifting_Gears_Flag;					// 单点采集：切换挡位时，不能进行打挡
	ts_u08 IsUpdate_TempLutCalc_Flag;			// 单点采集：标记温度lut的原始表是否有更新
	ts_u08 Iris_Current_State;					// 单点采集：挡片当前的状态
	ts_u08 Heartbeat_Flag;						// 单点采集：心跳信号
	ts_u08 m_temp_get_streaming_flag;			// 测温功能是否在获取数据流, 0：不获取，1：获取；用于避免与"去竖纹功能"的资源竞争
	ts_u08 m_allowGetFrameStream_flag;			// 是否允许获取数据流的标志，0：不允许，1：允许
	ts_u32 m_sync_latest_frame_id;				// 此变量主要为了解决 "串口请求 → 强制同步一帧"

	ts_u08 m_powerup_det_res_type;				//上电时，探测器的分辨率
	ts_u08 m_powerup_det_name_type;				//上电时，探测器的类型
	ts_u08 m_powerup_board_port_type;			//上电时，硬件接口板类型
	ts_cpl_mutex_s_t m_tmp117_i2c_mutex;		// 互斥锁，主要用于IIC读取

	ts_s32 m_cali_temp_value;					//用于记录nuc计算前后的温度值，用于温漂补偿	
	ts_s32 m_cali_index_offset;					//温漂标定节点索引号
	ts_u08 m_powerup_temp_drift_comp_mode;		//上电时,温漂补偿的模式类型
	ts_s32 m_npu_aisp_flag;        				// 监控超分创建状态
	ts_u16 m_uart_spot1_raw_ad;   				// 单点采集点1的原始AD值(未温补)

	ts_u08 m_powerup_inited_flag;	

	ts_u08 m_flash_type; 						// 上电时，检测到的flash类型
	ts_u16 SetValue_INTtoMK;	//测温系数mk相关
} SysCtrlInfo_s_t;

//设备所处环境下的图像参数结构体定义
struct dev_env_ImagePara
{
	ts_u08 Bright;      // 亮度，
	ts_u08 Contrast;    // 对比度，
};

struct _Usermenu_Parameter
{
	ts_u08 ImageMode;	// 图像模式
	ts_u08 Bright;		// 亮度，此版本暂时不用
	ts_u08 Contrast;	// 对比度，此版本暂时不用
	ts_u08 ImageAdjust; // 图像调节：手动调节、自动调节
	ts_u08 Palette;		// 伪彩
	ts_u08 CaptureFlag; // 捕获标志位
	ts_u08 FontColor;	// 字符颜色
	ts_u08 Mirror;		// 镜像
	volatile ts_u08 ShutterMode;
	ts_u08 AutoShutter; // 自动打档时间设置
	ts_u08 Enlarge;		// 电子放大
	ts_u08 pause_ctl;
	ts_u08 Menu1ParaLength;
	ts_u08 SunPT_SW;
	ts_u08 P_Time;
	ts_u08 Quad_SW;
	ts_u32 SystemTimeCounter;
	ts_u32 ShutterCounter;
	ts_u08 ContinuousZoom; // 变倍是否显示信息
	//******************寰宇协议相关****************//
	ts_u08 HY_ShutterTime1;
	ts_u08 HY_ShutterTime2;
	ts_u16 HY_ShutterTime3;
	ts_u08 CenterSpotSW;

	ts_u08 SFilter;
	ts_u08 EE_Level; // 图像增强，EE模块，也可以较锐化
	ts_u08 IsEnableEnligh;
	ts_u08 DDE_Level; // 为了方便管理放在菜单一
	ts_u08 enhance_level;
	ts_u08 m_dev_env_mode;		// 设备所处环境模式
	ts_u08 UseUserMenuDefParam; // 是否使用用户菜单默认专用参数，0：不使用，1：使用。使用菜单默认参数时，系统会将菜单默认参数写入当前参数中，覆盖掉当前参数的修改，达到恢复默认参数的功能
   // ts_s08 json_version[10];   //json版本号
   struct dev_env_ImagePara  manual_ImagePara;//DevEnvImagePara[DEVICE_MODE_BUTT]; // 不同环境模式下的图像参数

   ts_u08 m_pip_mode;   				//pip模式
   ts_u08 m_hst_mode;   				//hot spot tracking mode
   ts_u08 m_luma_percent_mode;  		//亮度百分比
   ts_u08 m_contrast_percent_mode;  	//对比对百分比
   ts_u08 m_rz_ratio;					//pip 模式下的放大倍数

   ts_u08 m_SunPT_mode; 	// 阳光保护模式 由于用户定制需求，需要在菜单一配置
   ts_u16 m_SunPT_gap;	  	// 阳光保护闭合挡片周期 ，此时间可以自动关联菜单二的 P_Time_U16，任意修改其中一个会自动更新另外一个。


   ts_u08 m_Crosshair_OnlyDisplay;		//中心光标显示/隐藏控制
   ts_u08 m_menu1_info_tip_visible;			//用于控制用户菜单osd提示信息是否显示，默认显示

};

#ifdef Electrolytic_copper
// 定义点
typedef struct
{
	ts_u16 x;
	ts_u16 y;
} Point;

extern Point vertices[4];
extern Point Tvertices[4];
extern ts_u08 Quad_Index;

#endif

extern struct _Usermenu_Parameter UsermenuParameter;

struct ImagePara
{
	ts_u08 SFilter;
	ts_u08 DDELevel;
	ts_u08 Gamma;
	ts_u08 Ran_Limit;
	ts_u08 Gama_TabVal;
	ts_u08 FK8_SW;
	ts_u08 TFilter;
	ts_u08 DDE_SW;
	ts_u08 MedFilt_SW;
	ts_u08 Plat_NX;
	ts_u08 BF_SW;
	ts_u08 Bri_A;
	ts_u08 Ctrs_A;
	ts_u08 JL_EPS;
};

#if DET_Change_Para
struct DETPARA
{
	ts_u08 INT_C;
	ts_u08 INT_T;
	ts_u08 OOC_Par; // det GOS
};
// struct DETPARA lowTemp;
// struct DETPARA normalTemp;
// struct DETPARA highTemp;
#endif

// ASIC固件尾部信息
typedef struct AsicFirmwareTail_s
{
	ts_u08 crc[2];
	ts_u08 date[4];		// BCD日期
	ts_u08 version[17]; // 非字符串终止（可能含空格）
	ts_u08 padding[9];
	ts_u08 reserved[32];
} AsicFirmwareTail_s_t;


typedef struct AsicFirmwareVersion_s
{
	unsigned long customer_id; 
	unsigned long major_version;
	unsigned long minor_version;
	unsigned long build_version;
	unsigned long function_id;
	unsigned long extend_function_id;
	unsigned long test_id;
} AsicFirmwareVersion_s_t;

enum DET_MODE_ENUM
{

	DET_MODE_AUTO = 0,
	DET_MODE_LOW_TEMP,
	DET_MODE_NORMAL,
	DET_MODE_HIGH_TEMP,
	DET_MODE_OFF,
	DET_MODE_MAX,
};

enum SHUTTER_MODE_ENUM
{

	SHUTTER_MODE_OFF = 0,
	SHUTTER_MODE_TIMER,
	SHUTTER_MODE_TEMPERATURE,
	SHUTTER_MODE_PRE,
	SHUTTER_MODE_BUTT,
};

enum BPC_CFG_MODE_ENUM
{
	BPC_CFG_MODE_ON = 0,  // 应用特殊的bpc配置找坏点
	BPC_CFG_MODE_OFF = 1, // 在bpc配置的基础上，OFF_EE_OFF_DDE
	BPC_CFG_MODE_OFF_EE_OFF_DDE = 1,
	BPC_CFG_MODE_ON_EE_OFF_DDE, // 在bpc配置的基础上，关闭细节增强dde，开启边沿增强EE
	BPC_CFG_MODE_ON_EE_ON_DDE,	// 在bpc配置的基础上，开启细节增强dde，关闭边沿增强EE
	BPC_CFG_MODE_OFF_EE_ON_DDE,
	BPC_CFG_MODE_BUTT,
};

struct Bpc_ext_Para
{
	ts_u08 enable;
	ts_u16 bad_x;
	ts_u16 bad_y;
	ts_u16 bad_w;
	ts_u16 bad_h;
};

struct Vot_Misc_Para
{
	ts_u32 m_force_debug_confirm; //输出日志确认，用特殊的魔数，避免误操作开启日志
};

/*-------------------------------------------------------------------------*/
struct Menu2_Para
{
	ts_u08 IR_SW;
	ts_u08 GAIN;
	ts_u08 GOF;
	ts_u08 INT_C;
	ts_u08 INT_T;
	ts_u08 OOC_Par;
	ts_u08 OOC_Mode;
	ts_u08 OOC_Fix;
	ts_u08 NUC_SW;
	ts_u08 BAD_SW;
	ts_u08 BAD_Cross;
	ts_u08 AD_Temp;
	ts_u08 TFilter;
	ts_u08 SFilter;
	ts_u08 DDE_Level; // 这个变量暂时不适用，改为用菜单一中的
	ts_u08 DEST_SW;
	ts_u08 test_state;
	ts_u08 offset_X;
	ts_u08 offset_Y; // 保存
	ts_u08 Auto_Semi;
	ts_u08 ImgT;
	ts_u08 Temp_TD;
	ts_u08 Auto;
	ts_u08 Bri_A;
	ts_u08 Ctrs_A;
	ts_u08 Bri_M;
	ts_u08 Ctrs_M;
	ts_u08 HOST_SW;
	ts_u08 NDAC_H;
	ts_u08 NDAC_L;
	ts_u08 Usermenu_SW;
	ts_u08 Output_mode;
	ts_u08 Gmax;
	ts_u08 Gamma;
	ts_u08 AD_THR;
	ts_u08 NUMTHR;
	ts_u08 AlgorithmSW; // 所有算法开关
	/***************Alarm TEST_1**********************/
	ts_u08 Alarm_SW;
	ts_u08 Range_SW;
	ts_u16 AlarmVal;
	ts_u16 AlarmAutoVal;
	/*******************成都浊弥_TEST***********************/
	ts_u08 TempChangeValue;
	ts_u08 BaudLevel;
	/**********************************************************/
	ts_u08 CVBS_flag;
	ts_u08 AD_T;
	ts_char LensSN[10];		 // 镜头信息
	ts_char MerchineSN[14];	 // 序列号
	ts_char ProduceDATA[10]; // 生产日期
	ts_u08 LOGO_Flag;
	ts_u08 TemTest_SW;

	ts_u08 Alpha;
	ts_u08 NUC_Flag;
	ts_u08 Ran_Limit;

	ts_u08 Menu2ParaLength;
	ts_u08 GOF_Val[3];
	ts_u08 INT_C_Val[3];
	ts_u08 INT_T_Val[3];
	ts_u08 OOC_Par_Val[3];

	ts_u16 PlatLimit;
	ts_u08 DTMY_SW;
	ts_u08 DTMY_Init;
	ts_u08 AGC_Filter;

#if (No_Shutter == 1)
	ts_u08 NoShutterFlag;
#endif
	ts_u08 Gmin;
	ts_u08 Gama_TabVal;
	ts_u08 DDE_SW;
	ts_u08 MedFilt_SW;
	struct ImagePara ImageIndoorPara;
	struct ImagePara ImageOutdoorPara;
	struct ImagePara ImageManualPara;
	struct ImagePara ImageOutdoorM3Para;
	ts_u08 AGC_SW;
	ts_u08 Plat_NX;
	ts_u08 FK8_SW;
	ts_u08 BF_SW;

	//	ts_u08 SunPT_SW;
	//	ts_u08 P_Time;
	ts_u16 S_Range;
	ts_u16 S_Val;
	ts_u16 pixeLimit;

	ts_u08 Zoom_Lens;
	ts_u16 Left_Val;
	ts_u16 Right_Val;
	ts_u08 ShutFun;

	ts_u08 SceneFlag;
	ts_u08 Alarm_Switch;
	ts_u08 AF_Val;

	ts_u16 FocusSite;
	// ts_u08 FCal_SW;
	ts_u08 OperatorSpeed;
	ts_u16 Quad_TemAlarm;
	ts_u08 OutdoorSel;

	ts_u16 Focus_Lx;
	ts_u16 Focus_Ly;
	ts_u16 Focus_Rx;
	ts_u16 Focus_Ry;

	ts_u16 AlarmAD;
	ts_u32 AlarmNum;

	ts_u08 SFDDE;
	ts_u08 AD_Set;
	ts_u08 Zoom_Foc_SW;

	ts_u08 sensor;
	ts_u08 piexl_offset;

#if DET_Change_Para
	ts_u08 DetMode;
	struct DETPARA LowTemp;
	struct DETPARA NormalTemp;
	struct DETPARA HighTemp;
	struct DETPARA offChange;
#endif
	ts_u16 S_Range1;
	ts_u16 S_Range2;
	ts_u16 S_Val1;
	ts_u16 S_Val2;
	ts_u16 S_Range0;
	ts_u16 S_Val0;

#if WLP
	ts_u08 WLP_Lens;
#endif
	// ts_u08 Alpha;
	struct ImagePara ImageOutdoorJLPara;
	ts_u08 JLEPS;
	ts_char DetectorDATA[12]; // 探测器编号
	ts_u08 m_video_out_mode;  // 配置视频输出模式
	ts_u08 m_sffc_mode;		  // sffc 模式 SFFC_MODE_ENUM
	ts_s32 m_sffc_level;	  // sffc 手动的模式的强度

	ts_u08 m_osd_show_on_dev; // osd显示在哪个设备上,0:cvbs 1:uvc

	ts_u08 m_ee_enhance_mode;	// 全局边缘增强模式，OFF，白边增强、黑边增强
	ts_u08 m_cvbs_display_mode; // DEVICE_DISPLAY_MODE_ENUM
	ts_u32 m_reserved[16];

	ts_u32 m_local_isp_magic_value; // 用于判定是否使用该isp配置 LOCAL_ISP_MAGIC_NUM
	ts_u08 m_bpc_ref_mode;			// 坏点矫正参考模式
	ts_u08 m_bpc_cfg;				// bpc isp cfg 开关
	ts_u08 m_bpc_view;				// 是否显示已经标记的坏点

	ts_u08 m_device_fps;
	ts_char exDetectorDATA[10]; // 扩展-探测器编号
	ts_u08 m_st1_m;
	ts_u08 m_st1_s;
	ts_u08 m_det_st;

	ts_u16 P_Time_U16;	  // 阳光保护时间
	ts_u08 SunPT_SW;	  // 阳光保护开关
	ts_u08 SunPT_Info_SW; // 阳光保护提示开关

	ts_u08 m_bpc_cluster_on_flag; // 是否开启坏簇矫正

	ts_u08 Bpc_ext_SW;					 // 坏斑总开关
	struct Bpc_ext_Para Extbpc_para[16]; // 坏斑功能参数

	ts_u08 m_raw_yuv_mode; // raw+yuv模式的
	ts_u08 m_raw_node_pos; // raw+yuv模式下的，raw节点对应的数据

	ts_u08 m_uvc_enlarger_mode; // 是否开启uvc放大

	ts_u08 SmartData408bit[51];		 // UCC3代光智探测器408bit数据
	ts_u08 SmartData408bitValidFlag; // 获取到UCC3代光智探测器408bit数据的有效标志;

	ts_u08 m_mipi_lane_mode; // MIPI输出lane模式

	ts_u08 m_force_detector_type; // 用户强制选择的探测器类型

	ts_u08 UseSysMenuDefParam; // 是否使用系统菜单默认专用参数，0：不使用，1：使用。使用菜单默认参数时，系统会将菜单默认参数写入当前参数中，覆盖掉当前参数的修改，达到恢复默认参数的功能

	ts_u08 m_bt_time_sequece_mode; //是否使用用户定义的bt时序

	ts_u08 m_force_hardware_port_type; // 用户强制选择的硬件接口类型

	ts_u08 m_user_metadata_flag; 	//在视频帧后追加用户数据 

	ts_u08 m_force_customer_id; 		//用于表示不同的定制客户，用户定制id号; 		//用于表示不同的定制客户，用户定制id号
	/*********FocusControl**************/
	ts_u08 Focus_DNumSet;
	ts_u08 Focus_UNumSet;

	ts_u08 m_force_focus_type; 		//镜头类型  FOCUS_TYPE_ENUM
	ts_u08 m_force_function_type; 		//产品功能类型 FUNCTION_TYPE_ENUM
	ts_u08 set_current_ua; //gm1215线性稳压电源，设置目标电流0~200μA，单位μA

	ts_u08 m_SunPT_mode; // 阳光保护模式 //deprecation
	ts_u08 m_fix_snc_switch; // fix_snc线程是否开启，断电重启后生效
	ts_u08 m_temperCalibra_Flag;			// 测温标定标志位
	ts_u08 m_templutMode_Limit;					// 测温档位限制

	//等温iso相关配置
	ts_u08 m_iso_switch;
	ts_u08 m_iso_position_mode;
	ts_u08 m_iso_color_mode;	
	ts_u16 m_iso_ad_high;
	ts_u16 m_iso_ad_low;
	//ad 温度过滤相关配置
	ts_u08 m_filter_ad_mode;
	ts_u16 m_manual_ad_high;
	ts_u16 m_manual_ad_low;

	ts_u08 m_bt_enlarger_mode; // 针对384模式，是否开启放大,384x288->640x512
	ts_u08 m_mipi_enlarger_mode; // 针对384模式，是否开启放大,384x288->640x512
	ts_u08 m_user_tempFrame_Mode;		// 控制温度矩阵输出原始AD/测温AD/查表温度
	ts_u08 m_ts_npu_ops_sr_en; //  超分应用使能 TS_NPU_OPS_SR,
	//光智1280探测器Det参数256bit
	ts_u08 	GAIN1280;
	ts_u08 	GOF1280;
	ts_u08 	Cint1280;
	ts_u16  Nint;
	ts_u08  Sel_PJ;
	ts_u08  NTCC;
	ts_u08  Voccm;
	ts_u08  detConfig;
	ts_u08  Vfid;
	ts_u08 SmartData256bit[32];		 // UCC3代光智1280探测器256bit数据
	ts_u08 SmartData256bitValidFlag; // 获取到UCC3代光智1280探测器256bit数据的有效标志;
	ts_u08 m_menu2_info_tip_visible;		//用于控制研发菜单osd提示信息是否显示，默认显示
	ts_u08 m_temp_drift_comp_mode;  		//温漂补偿功能开关Temperature Drift Compensation
	ts_u16 m_temp_drift_index;				//标记的温补表索引值，其中位数0~8位的0-1状态代表是否标定0：已标定,1：未标定
	ts_s16 m_temp_drift_temp[9];			//温补列表，用于存储标定温度		

	ts_u08 m_t8_mode_force_bpc_enable;		//T8 模式下支持坏点矫正
	ts_u08 m_uvc_output_format;   			// UVC输出格式，0=YUYV, 1=UYVY

	ts_u08 m_vot_pod_type; 					//开机画面类型
	ts_u08 m_templut_target_gear;   // 测温功能目标挡位，0xFF=无待生效, 0=OFF, 1~3=目标挡位

	ts_u08 m_update_firmware_policy_mode;   // 升级固件策略模式
	ts_u08 m_temp_regularity;						// 温度规律选择

	ts_u08 m_sys_boot_mode;						// 系统启动模式

	//MotorFocus相关配置
	ts_u08 MotorSenFlag;
	ts_u08 FocusPeakMode;
	ts_u16 Focus_PeakVal;
	ts_u08 ADDR_Sel;

	ts_u08 m_st0_total_duration_s;				// 开机第0段时间总时间，单位：秒 [0-60]
	ts_u08 m_st0_interval_s;					// 第0段时间内的打挡片间隔，单位：秒 [1-20]

	ts_u08 m_temp_drift_aisp_mode;				// 温漂补偿后处理，A6/A12才支持
};

struct Menu2_Disp_Para
{
	ts_u16 ADV_PAR;
	ts_u16 Bak_PAR;
	ts_u16 Det_PAR;
};

extern struct Menu2_Disp_Para DispSysMenu2;
extern struct Menu2_Para SystemParaMenu2;
extern struct Vot_Misc_Para global_vot_misc_cfg;
// extern struct _ImageIndoorPara ImageIndoorPara;
// extern struct _ImageOutdoorPara ImageOutdoorPara;
// extern struct _ImageManualPara ImageManualPara;
/*-------------------------------------------------------------------------*/
#define SPOT_NUM 10
#define SPOT_XSTART_ADDR 292
#define SPOT_YSTART_ADDR 310

#define SPOT_BAD_X_START_ADDR 300
#define SPOT_BAD_Y_START_ADDR 300

#define AREA_BAD_X_START_ADDR 200
#define AREA_BAD_Y_START_ADDR 200

#define AREA_NUM 5
#define AREA_XSTART_ADDR 120
#define AREA_YSTART_ADDR 310

#define AREA_LXSTART 200
#define AREA_LYSTART 240

#define SPOT_AREA_SIZE 16
#define SPOT_AREA_GAP 8

struct Spot
{
	char *name;
	ts_u08 spot_sequence;
	ts_u16 spot_X_Pos;
	ts_u16 spot_Y_Pos;
	ts_u16 spot_ID;
	ts_s16 spot_temp;
	ts_u16 spot_AD;
	ts_u08 spotoffestFlag;
	ts_char last_SpotMsg[64];
	ts_u16 last_Spot_X_pos;
	ts_u16 last_Spot_Y_pos;
};
typedef struct Spot *PT_Spot;
extern struct Spot SpotInit[SPOT_NUM];
extern struct Spot spot_max;
extern struct Spot spot_min;

struct Area
{
	char *name;
	ts_u16 Area_X_Pos;
	ts_u16 Area_Y_Pos;
	ts_u08 Area_ID;
	ts_u16 Area_temp_MAX;
	ts_u16 Area_temp_MIN;
	ts_u16 Area_temp_AVG;
	ts_u16 Area_Lx;
	ts_u16 Area_Ly;
	ts_u16 Area_Rx;
	ts_u16 Area_Ry;
	ts_char last_osd_Msg[64];
	ts_u16 last_osdMsg_X_pos;
	ts_u16 last_osdMsg_Y_pos;
};
typedef struct Area  *PT_Area;

extern ts_u16 Area_state;
extern PT_Spot select_Spot;
extern PT_Area select_Area;
extern struct Area Areainit[AREA_NUM];

extern ts_u16 Spot_state;
extern ts_u08 Flag_Calibration;
extern ts_u32 UserTimerCounter;
extern ts_u08 ShutterTimerReachFlag;
extern ts_u16 ShutterTimer;
extern ts_u08 ProtectFlag;
extern ts_u16 ProtectTime;
extern ts_u16 Alarm_Flag;
extern ts_u16 temptime;
extern ts_u08 Focus_Calibration_Flag;

extern ts_u08 Deviceinfo_Flag;
extern ts_u08 CharacterColor, BackgroundColor;
extern ts_u08 VersionTab[3];

void vot_menu_init(void);
void display_u16number(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
void KEY_FD_PRO(void); // 上
void KEY_FB_PRO(void); // 下
void KEY_FE_PRO(void); // 确认
void KEY_F7_PRO(void); // 左
void KEY_EF_PRO(void); // 右
void enter_menu2(void);
void KEY_EE_PRO(void); // 标定模式
void KEY_F4_PRO(void);
void KEY_C1_PRO(void);
// ts_u08 obtain_open_SpotorArea_num(ts_u16 state);
ts_u08 modify_Spot_sequence(struct Spot tempspot, ts_u08 state);
ts_u08 serach_SpotArea_maxsequence(ts_u08 kind);
void Disp_SpotN_Messages(PT_Spot temp_spot, ts_u08 state);
void Auto_shutter(void);
void DisPlay_AREA_Iron(ts_u08 mult_enlarge, ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 layer, ts_u08 color);
void Disp_Capture_Info(void);
void BadArea_ADCalc(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_pos, ts_u16 *ad_avg, ts_u16 *ad_max, ts_u16 *ad_min);
void BadArea_Display(ts_u08 active);
void BadSemiAutoOpr(PT_Area temp_area, ts_u08 state, ts_u08 opr);
/*--------------------------------------------------------------------------*/
void DisPlay16(ts_u08 buf[], ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 layer, ts_u08 color);
void DisplayBackground(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer, ts_u08 color);
unsigned char Hex16bit_ascii4(ts_u16 data, unsigned char *obuf);
void Display_Hex16bit(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
void display_u8number(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 pData, ts_u08 layer, ts_u08 color);
void display_Charnumber(ts_u32 X_LINE, ts_u32 Y_LINE, char pData, ts_u08 layer, ts_u08 color);
void Display_uint8_Dec1(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
void Display_uint8_Dec2(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
void Display_uint8_Dec3(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
void Display_int8_Dec4(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color); // 3位数字加1符号位
unsigned char ts_u08_ascii(char data, unsigned char *obuf);
unsigned char uint16_ascii4(ts_u16 data, unsigned char *obuf);
void DisPlayWarnIcon(ts_u08 *buf, ts_u32 X_LINE, ts_u32 Y_LINE, ts_u08 layer, ts_u08 color, ts_u08 Erase);
void Display_uint16_Dec4(ts_u32 X_LINE, ts_u32 Y_LINE, ts_u16 pData, ts_u08 layer, ts_u08 color);
/*--------------------------------------------------------------------------*/
void AreaN_Scale(PT_Area temp_area, ts_u08 state, ts_u08 opr);
/*--------------------------------------------------------------------------*/
void Area_SimultaneousDisplay(ts_u16 state);
// void Area_Val_Simultaneous(void);
void Spot_SimultaneousDisplay(ts_u16 temp_point_status);
void TestArea2Message(PT_Area temp_area);

void Select_CharacterLayer(ts_u08 layer, ts_u08 color, ts_u32 temp_address);
void DisPlay_AREA_Icon(PT_Area select_Area, ts_u08 layer, ts_u08 color);
void ClearUncheckAreaIcon(PT_Area temp_area);
unsigned int GetPixelMaxAndMinValue(void);
unsigned int CancalPixelMaxAndMinValue(void);
/*--------------------------------------------------------------------------------*/
void Display_Deviceinfo(void);
void Erase_Deviceinfo(void);
ts_void EnterOrExit_SemiAutoMode(ts_u08 vExit_flag);
/* 即将弃用 */
void vGet_BPC_MODE_CurrentResolution(ts_u16 *vWidth, ts_u16 *vHeigth);
ts_void v_getDector_DisplayResolution(ts_u16* vWidth, ts_u16* vHeigth);
#ifdef Electrolytic_copper
void drawIrregularPolygonBox(Point *vertices, ts_u16 color);
void DisplayQuadTemp(void);
void QuadMoveUp(void);
void QuadMoveDown(void);
void QuadMoveLeft(void);
void QuadMoveRight(void);
void Quad_TemAlarmSet(void);
#endif
/*-----------------------------------------------------------------------------*/
void BadCroOpr(ts_u08 opr);

void InitSysCtrlInfo(void);
void DeinitSysCtrlInfo(void);
void InitSysCtrlInfoBuff(void);
void display_append_u8number(ts_u08 *buf, ts_u08 pData);
void display_append_u16number(ts_u16 *buf, ts_u16 pData);
void SetImgAutoFreeze(void);

#endif /* SRC_MENU_MENU_H_ */
