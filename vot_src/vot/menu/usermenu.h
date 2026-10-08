/*
 * @Author: shixiaobo 328494516@qq.com
 * @Date: 2025-09-10 16:14:34
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-06-29 09:25:57
 * @FilePath: /ucc3_a_gen/vot_src/vot/menu/usermenu.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef USER_MENU_H
#define USER_MENU_H

#include <base/ts_type.h>
#include <menu/testmenu.h>


typedef enum {
    BOARD_PORT_UAD3_LITE,
    BOARD_PORT_UAD3,
	BOARD_PORT_UAW,//wlp
	BOARD_PORT_UAW_SE_LITE,//17x17 陶封wlp机芯
    BOARD_PORT_UNKNOWN
} board_type_t;


typedef enum {
	DECTOR_TYPE_VOT=0,
    DECTOR_TYPE_RTD,
	DECTOR_TYPE_VOT_WLP,
	DECTOR_TYPE_AUTO,
    DECTOR_TYPE_UNKNOWN
} dector_type_t;

typedef enum {
	DECTOR_RESOLUTION_640X512=0,
    DECTOR_RESOLUTION_384X288,
	DECTOR_RESOLUTION_1280X1024,
	DECTOR_RESOLUTION_AUTO,
    DECTOR_RESOLUTION_UNKNOWN
} dector_resolution_type_t;

typedef enum {
	PROJECT_NAME_TYPE_UAD3=0,
    PROJECT_NAME_TYPE_WLP,
    PROJECT_NAME_TYPE_UNKNOWN
} project_name_type_t;


// T10_250717_PLK6 C25_0527P1_PLK6
#define VOT_HARDWARE_PLATFORM  "HP_T10_PLK6"
#define VOT_SOFTWARE_PLATFORM  "VER_C25_P1_PLK6"

#ifndef VOT_SOFTWARE_VER_FIXED_HEADER
#define VOT_SOFTWARE_VER_FIXED_HEADER  "000.1.0.00.100000.000.000"
#endif

#ifndef RESOLUTION_TYPE
#define RESOLUTION_TYPE  "RES_640_512"
#endif

#ifndef HARD_BOARD_PORT_TYPE
#define HARD_BOARD_PORT_TYPE  "UAD3"
#endif

#ifndef DETECTOR_TYPE_NAME
#define DETECTOR_TYPE_NAME  "VOT"
#endif


#ifndef PRODUCTION_NAME
#define PRODUCTION_NAME  "UAD3"


#ifndef CUSTOMER_TYPE
#define CUSTOMER_TYPE  "STANDARD"
#endif

#ifndef FOCUS_TYPE
#define FOCUS_TYPE  "PRIME_LENS_TYPE"
#endif


#ifndef FUNCTION_TYPE
#define FUNCTION_TYPE  "INFRARED_IMAGING_TYPE"
#endif


#endif



// #define UsermenuItemMaxNumber	16//用户菜单可容纳的最多的结点数

#define UTab1_Length		4//"图像"、"测温"、"系统"、"调节"
//#define UTab2_Length 		6//"场景"、"模式"、"亮度"、"对比度"、"伪彩"、"电子放大"
#define UTab2_Length 		8//"亮度"、"对比度"、"伪彩"、"电子放大" “阳光保护” 
#define UTab3_Length 		5//"测温点"、"测温区域"、"捕获点"、"测温参数"、"测温档位"、"四边形" 、"报警阈值”

#ifdef OPR_ALARM
	#define UTab4_Length 	8//"语言"、"字符"、"镜像"、"挡片模式"、"自动挡片"、"保存设置"、"默认设置"、"系统信息"、"阳光保护"、"保护时间"
#else
	#define UTab4_Length 	6//"语言"、"字符"、"镜像"、"挡片模式"、"自动挡片"、"保存设置"、"默认设置"、"系统信息"
#endif

//#define UTab5_Length	 	3//"图像增强"、"空间滤波"、"中心点"
#define UTab5_Length	 	3//"图像增强"、"空间滤波"
#define UTab6_Length 		3//测温点1~测温点9
#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
	#define UTab7_Length 		3//测温区域1~测温区域3
#else
	#define UTab7_Length 		2//测温区域1~测温区域4
#endif
#define UTab8_Length 		6//"辐射率"、"反射温度"、"环境温度"、"相对湿度"、"距离"、"镜头设置"

#define UTab9_Length		2

#define UsermenuTabNum 		9//用户菜单组数，现在用户菜单一共9组

#define X_GAP				4//2
#define Y_GAP				22

#define X_START				32
#define Y_START				48
#define X_INCREMENT 		144

typedef struct _NodeData
{
	const char **Name;	//每个菜单项的名称
	unsigned char line_Y;	//每个菜单项显示的Y轴位置
	unsigned short line_X;	//每个菜单项显示的X轴位置
	unsigned short ID;		//每个菜单项的ID(唯一)
	unsigned char SonNum;	//每个菜单项子节点数量
	unsigned char NameAdd[64];
}NodeData;

typedef struct _MenuStructure
{
	NodeData Info;//每个菜单项携带的数据信息
	struct _MenuStructure *Parent;		//每个菜单项的父亲节点
	struct _MenuStructure *Previous;	//每个菜单项的上一节点
	struct _MenuStructure *Next;		//每个菜单项的下一节点
	struct _MenuStructure *Son;			//每个菜单项的儿子节点
}MenuStructure,*Point_MenuStructure;

typedef struct _Selection
{
	struct _MenuStructure* SelectItem;	//当前选中的菜单项
	unsigned char MenuState;
	unsigned char IsEnableEnligh;
}Selection;

typedef struct _AllMenuItem
{
	Point_MenuStructure pt;
	unsigned char number;
}AllMenuItem;

extern Selection SelectionMenu;
extern Selection RecallMenu;
extern MenuStructure UsermenuTab2[UTab2_Length];
extern AllMenuItem UsermenuItemTable[UsermenuTabNum];
// extern AllMenuItem TestmenuItemTable[TestmenuTabNum];
extern ts_u08 CharacterColor,BackgroundColor;
extern const char *Focusslect[4];
#ifdef No_Shutter
 extern u8 NoShutter_ExtFlag;
#endif

void UserMenuInit(void);

ts_void init_menu1_parameters(void);
ts_void init_menu2_parameters(void);
ts_void save_menu1_parameters(void);
ts_void save_menu2_parameters(void);
void DisplayDeviceInfoAuto(void);
void DisplaySocVersionInfo(ts_char *ver_info, ts_u08 is_active);

void DisplayUserInfo(ts_char *user_info, ts_u08 is_active);
void UpdateSpecialInfo(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible);
void UpdateMenu2StatisInfo(ts_char *statis_info, int is_visible,int line_idx);
void UpdateSpecialInfoFont64x64(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible);
void UpdateSpecialInfoFont16x16(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible);
void UpdateSpecialInfoWithNodeBuf(ts_nodebuf_s_t *pst_lbl, ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible);

void Key_ExitUsermenu(void);
void Key_EnterUsermenu(void);
void Key_RightUpdataParameter(void);
void Key_LeftUpdataParameter(void);
void Key_EnterNextUsermenuElement(void);
void Key_EnterPreviousUsermenuElement(void);

void Clear_MenuSonInfo(Point_MenuStructure Pt_T4,ts_u08 number,ts_u08 SonStartYline);
void Clear_MenuInfo(Point_MenuStructure Pt_T3,ts_u08 number);
void Display_MenuItem(Point_MenuStructure Pt_T0,ts_u08 number,ts_u08 number_active);
void Display_MenuItemLabel(Point_MenuStructure Pt_T0, ts_u08 is_active);
void Add_MenuItemFatherList(Point_MenuStructure PTSon,Point_MenuStructure PTFather);
void CreatMenuList(Point_MenuStructure IN_node_list,unsigned char Node_Max_Num);
void CreatUsermenu(void);



#endif//USER_MENU_H
