/*
 * usermenu.c -- 机芯菜单
 */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_media.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/chip_def.h>
#include <uapi/osd_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/viu_v100.h>
#include <command.h>
#include <stdio.h>
#include "testmenu.h"
#include "usermenu.h"
#include "votmenu.h"
#include "fontlib.h"

#include "ASCII.h"
#include "name.h"
#include <menu/testmenufunction.h>
#include <menu/usermenufunction.h>
#include <fontlib_vot.h>
#include <snr_comm_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>


extern SysCtrlInfo_s_t global_sys_ctrl_info;
int xIncrement_spacing = 144;

int one_height_pixels = 24; //当个菜单对应像素高度

extern int test_xIncrement_spacing ;

Selection SelectionMenu;
Selection RecallMenu;
/*********************************用户菜单************************************/
/***************************所有的指针域初始化为NULL*****************************/
/***************************所有的数据域域初始化不为空****************************/
MenuStructure UsermenuTab1[UTab1_Length]  =
{
	//"图像"、"测温"、"系统"、"调节"
	/***********************一级菜单**************************/
	{{UsermenuNameTab1[0],0,X_START+(X_INCREMENT+X_GAP)*0,0x10},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab1[1],0,X_START+(X_INCREMENT+X_GAP)*1,0x20},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab1[2],0,X_START+(X_INCREMENT+X_GAP)*2,0x30},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab1[3],0,X_START+(X_INCREMENT+X_GAP)*3,0x40},NULL,NULL,NULL,NULL},
	/*********************************************************/
};

MenuStructure UsermenuTab9[UTab9_Length]=//Sun Pt
{
	{{UsermenuNameTab9[0],5,X_START+(X_INCREMENT+X_GAP)*1,0x180},NULL,NULL,NULL,NULL},//"Mode"
	{{UsermenuNameTab9[1],6,X_START+(X_INCREMENT+X_GAP)*1,0x181},NULL,NULL,NULL,NULL},//"Protect Time"

};


#if 1
// MenuStructure UsermenuTab2[UTab2_Length]=
// {
// 	//"亮度"、"对比度"、"伪彩"、"电子放大"
// 	/***********************二级菜单第一级-图像********************/
// 	{{UsermenuNameTab2[0],1,X_START+X_INCREMENT*0,0x13},NULL,NULL,NULL,NULL},
// 	{{UsermenuNameTab2[1],2,X_START+X_INCREMENT*0,0x14},NULL,NULL,NULL,NULL},
// 	{{UsermenuNameTab2[2],3,X_START+X_INCREMENT*0,0x15},NULL,NULL,NULL,NULL},
// 	{{UsermenuNameTab2[3],4,X_START+X_INCREMENT*0,0x16},NULL,NULL,NULL,NULL},
// 	/*********************************************************/
// };

MenuStructure UsermenuTab2[UTab2_Length];
ts_s32 Real_UTab2_Length=0;

void InitUsermenuTab2(void)
{
	static ts_s32 first_flag=1;
	ts_u08 Serial_Number = 1;

	if(first_flag==1){
		ts_s32 UTab2_Son_Length=0;
		Real_UTab2_Length=0;
		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x13},NULL,NULL,NULL,NULL};
	
		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x14},NULL,NULL,NULL,NULL};

		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x15},NULL,NULL,NULL,NULL};
	
		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x16},NULL,NULL,NULL,NULL};

		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x1a},NULL,NULL,NULL,NULL};
		/* 插队到阳光保护后面 */
		UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x1b},NULL,NULL,NULL,NULL};

		if (get_customer_type() == CUSTOMER_TYPE_DSW)
		{
			vision_log_warn("CUSTOMER_TYPE_DSW\n");
			UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x18},NULL,NULL,NULL,NULL};
			UsermenuTab2[Real_UTab2_Length++] = (MenuStructure){{UsermenuNameTab2[UTab2_Son_Length++],Serial_Number++,X_START+X_INCREMENT*0,0x19},NULL,NULL,NULL,NULL};
		}
		first_flag=0;
	}
}

#else
MenuStructure UsermenuTab2[UTab2_Length]=
{
	//"场景"、"模式"、"亮度"、"对比度"、"伪彩"、"电子放大"
	/***********************二级菜单第一级-图像********************/
	{{UsermenuNameTab2[0],1,X_START+X_INCREMENT*0,0x11},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab2[1],2,X_START+X_INCREMENT*0,0x12},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab2[2],3,X_START+X_INCREMENT*0,0x13},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab2[3],4,X_START+X_INCREMENT*0,0x14},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab2[4],5,X_START+X_INCREMENT*0,0x15},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab2[5],6,X_START+X_INCREMENT*0,0x16},NULL,NULL,NULL,NULL},
	/*********************************************************/
};
#endif
MenuStructure UsermenuTab3[UTab3_Length]=
{
	//"测温点"、"测温区域"、"捕获点"、"测温参数"、"测温档位"、"四边形"、"四边形报警阈值"
	/***********************二级菜单第二级-测温********************/
	{{UsermenuNameTab3[0],1,X_START+(X_INCREMENT+X_GAP)*1,0x21},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab3[1],2,X_START+(X_INCREMENT+X_GAP)*1,0x22},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab3[2],3,X_START+(X_INCREMENT+X_GAP)*1,0x23},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab3[3],4,X_START+(X_INCREMENT+X_GAP)*1,0x24},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab3[4],5,X_START+(X_INCREMENT+X_GAP)*1,0x25},NULL,NULL,NULL,NULL},
	//{{UsermenuNameTab3[5],6,X_START+(X_INCREMENT+X_GAP)*1,0x26},NULL,NULL,NULL,NULL},
	//{{UsermenuNameTab3[6],7,X_START+(X_INCREMENT+X_GAP)*1,0x27},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab3[5],6,X_START+(X_INCREMENT+X_GAP)*1,0x28},NULL,NULL,NULL,NULL},
	/*********************************************************/
};
MenuStructure UsermenuTab4[UTab4_Length]=
{
	//"语言"、"字符"、"镜像"、"挡片模式"、"自动挡片"、"保存设置"、"默认设置"、"系统信息"
	/***********************二级菜单第三级-系统********************/
	{{UsermenuNameTab4[0],1,X_START+(X_INCREMENT+X_GAP)*2,0x31},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab4[1],2,X_START+(X_INCREMENT+X_GAP)*2,0x32},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[1],2,X_START+(X_INCREMENT+X_GAP)*2,0x33},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab4[2],3,X_START+(X_INCREMENT+X_GAP)*2,0x34},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[2],3,X_START+(X_INCREMENT+X_GAP)*2,0x35},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[3],4,X_START+(X_INCREMENT+X_GAP)*2,0x36},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[4],5,X_START+(X_INCREMENT+X_GAP)*2,0x37},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[5],6,X_START+(X_INCREMENT+X_GAP)*2,0x38},NULL,NULL,NULL,NULL},
	//"阳光保护"、"保护时间"
#ifdef OPR_ALARM
	{{UsermenuNameTab4[6],7,X_START+(X_INCREMENT+X_GAP)*2,0x39},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab4[7],8,X_START+(X_INCREMENT+X_GAP)*2,0x3a},NULL,NULL,NULL,NULL},
#endif
};
	/*********************************************************/
MenuStructure UsermenuTab5[UTab5_Length]=
{
	//"图像增强"、"空间滤波"
	/***********************二级菜单第四级-调节********************/
	{{UsermenuNameTab5[0],1,X_START+(X_INCREMENT+X_GAP)*3,0x41},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab5[1],2,X_START+(X_INCREMENT+X_GAP)*3,0x42},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab5[2],3,X_START+(X_INCREMENT+X_GAP)*3,0x44},NULL,NULL,NULL,NULL},
	/*********************************************************/
};

MenuStructure UsermenuTab6[UTab6_Length]=
{
	/***********************三级菜单第一级-测温点********************/
	//测温点1~测温点9
	{{UsermenuNameTab6[0],2,X_START+(X_INCREMENT+X_GAP)*1,0x210},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab6[1],3,X_START+(X_INCREMENT+X_GAP)*1,0x211},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab6[2],4,X_START+(X_INCREMENT+X_GAP)*1,0x212},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[3],5,X_START+(X_INCREMENT+X_GAP)*1,0x213},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[4],6,X_START+(X_INCREMENT+X_GAP)*1,0x214},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[5],7,X_START+(X_INCREMENT+X_GAP)*1,0x215},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[6],8,X_START+(X_INCREMENT+X_GAP)*1,0x216},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[7],9,X_START+(X_INCREMENT+X_GAP)*1,0x217},NULL,NULL,NULL,NULL},
	// {{UsermenuNameTab6[8],10,X_START+(X_INCREMENT+X_GAP)*1,0x218},NULL,NULL,NULL,NULL},
	/*********************************************************/
};

MenuStructure UsermenuTab7[UTab7_Length]=
{
	/***********************三级菜单第二级-测温区域********************/
	//测温区域1~测温区域4
	{{UsermenuNameTab7[0],3,X_START+(X_INCREMENT+X_GAP)*1,0x220},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab7[1],4,X_START+(X_INCREMENT+X_GAP)*1,0x221},NULL,NULL,NULL,NULL},
// 	{{UsermenuNameTab7[2],5,X_START+(X_INCREMENT+X_GAP)*1,0x222},NULL,NULL,NULL,NULL},
// #ifndef SUPPORT_FPGA_USB_UART_PROTOCOL
// 	{{UsermenuNameTab7[3],6,X_START+(X_INCREMENT+X_GAP)*1,0x223},NULL,NULL,NULL,NULL},
// #endif
	/*********************************************************/
};

MenuStructure UsermenuTab8[UTab8_Length]=
{
	/***********************三级菜单第三级-测温参数********************/
	//"辐射率"、"反射温度"、"环境温度"、"相对湿度"、"距离"、"镜头设置"
	{{UsermenuNameTab8[0],5,X_START+(X_INCREMENT+X_GAP)*1,0x240},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab8[1],6,X_START+(X_INCREMENT+X_GAP)*1,0x241},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab8[2],7,X_START+(X_INCREMENT+X_GAP)*1,0x242},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab8[3],8,X_START+(X_INCREMENT+X_GAP)*1,0x243},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab8[4],9,X_START+(X_INCREMENT+X_GAP)*1,0x244},NULL,NULL,NULL,NULL},
	{{UsermenuNameTab8[5],10,X_START+(X_INCREMENT+X_GAP)*1,0x245},NULL,NULL,NULL,NULL},
	/*********************************************************/
};

// AllMenuItem UsermenuItemTable[UsermenuTabNum] =
// {
//  	{UsermenuTab1,UTab1_Length},//"图像"、"测温"、"系统"、"调节"

//  	{UsermenuTab2,UTab2_Length},//"场景"、"模式"、"亮度"、"对比度"、"伪彩"、"电子放大"
//  	{UsermenuTab3,UTab3_Length},//"测温点"、"测温区域"、"捕获点"、"测温参数"、"测温档位"、"四边形"
//  	{UsermenuTab4,UTab4_Length},//"语言"、"字符"、"镜像"、"自动挡片"、"保存设置"、"默认设置"、"系统信息"
//  	{UsermenuTab5,UTab5_Length},//"图像增强"、"空间滤波"

// 	{UsermenuTab6,UTab6_Length},//测温点1~测温点9
// 	{UsermenuTab7,UTab7_Length},//测温区域1~测温区域4
// 	{UsermenuTab8,UTab8_Length},//"辐射率"、"反射温度"、"环境温度"、"相对湿度"、"距离"
// };


AllMenuItem UsermenuItemTable[UsermenuTabNum];

void InitUsermenuItemTable(void)
{
	static ts_s32 first_flag=1;
	InitUsermenuTab2();

	vision_log_warn("UTab1_Length %d \n",Real_UTab2_Length);

	if(first_flag==1){
		ts_s32 Real_UsermenuTab_Length = 0;
		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab1,UTab1_Length};//"图像"、"测温"、"系统"、"调节"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab2,Real_UTab2_Length};//"亮度"、"对比度"、"伪彩"、"电子放大"、"阳光保护"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab3,UTab3_Length};//"测温点"、"测温区域"、"捕获点"、"测温参数"、"测温档位"、"四边形"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab4,UTab4_Length};//"语言"、"字符"、"镜像"、"自动挡片"、"保存设置"、"默认设置"、"系统信息"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab5,UTab5_Length};//"图像增强"、"空间滤波"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab6,UTab6_Length};//测温点1~测温点9

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab7,UTab7_Length};//测温区域1~测温区域4

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab8,UTab8_Length};//"辐射率"、"反射温度"、"环境温度"、"相对湿度"、"距离"

		UsermenuItemTable[Real_UsermenuTab_Length++] = (AllMenuItem){UsermenuTab9,UTab9_Length};//"阳光保护mode"、"阳光保护时间" //用户定制需求，将阳光保护模式和闭合时间放在菜单一，菜单二选项仍旧保留

		first_flag=0;
	}

}

/**
 * @brief:为所有的父节点添加子链
 * @parameter:NULL
 */
void Add_UsermenuItemSonList(void)
{
	vision_log_debug("Add_UsermenuItemSonList\n");
	for(ts_u08 i=0;i<UsermenuItemTable[0].number;i++)
	{
		UsermenuItemTable[0].pt[i].Son = &UsermenuItemTable[1+i].pt[0];
		UsermenuItemTable[0].pt[i].Parent = &UsermenuItemTable[0].pt[i];
		UsermenuItemTable[0].pt[i].Info.SonNum = UsermenuItemTable[1+i].number;
		memcpy(UsermenuItemTable[0].pt[i].Info.NameAdd, UsermenuItemTable[0].pt[i].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)UsermenuItemTable[0].pt[i].Info.Name[SelectionMenu.IsEnableEnligh])+1);
	}
	UsermenuTab3[0].Son = &UsermenuTab6[0];
	UsermenuTab3[0].Info.SonNum = UTab6_Length;
	memcpy(UsermenuTab3[0].Info.NameAdd, UsermenuTab3[0].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)UsermenuTab3[0].Info.Name[SelectionMenu.IsEnableEnligh])+1);

	UsermenuTab3[1].Son = &UsermenuTab7[0];
	UsermenuTab3[1].Info.SonNum = UTab7_Length;
	memcpy(UsermenuTab3[1].Info.NameAdd, UsermenuTab3[1].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)UsermenuTab3[1].Info.Name[SelectionMenu.IsEnableEnligh])+1);


	UsermenuTab2[4].Son = &UsermenuTab9[0];//Sun_Prot
	UsermenuTab2[4].Info.SonNum = UTab9_Length;
	memcpy(UsermenuTab2[4].Info.NameAdd, UsermenuTab2[4].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)UsermenuTab2[4].Info.Name[SelectionMenu.IsEnableEnligh])+1);


	UsermenuTab3[3].Son = &UsermenuTab8[0];
	UsermenuTab3[3].Info.SonNum = UTab8_Length;
	memcpy(UsermenuTab3[3].Info.NameAdd, UsermenuTab3[3].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)UsermenuTab3[3].Info.Name[SelectionMenu.IsEnableEnligh])+1);
}

/**
 * @brief:为所有的子链添加父节点
 * @parameter:PTSon:当前的子结点
 * @parameter:PTFather:当前的父节点
 */
void Add_MenuItemFatherList(Point_MenuStructure PTSon,Point_MenuStructure PTFather)
{
	vision_log_debug("Add_MenuItemFatherList\n");
	if(PTFather->Info.SonNum>0)
	{
		memcpy(PTFather->Info.NameAdd, PTFather->Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)PTFather->Info.Name[SelectionMenu.IsEnableEnligh])+1);
		for(ts_u08 i=0;i<PTFather->Info.SonNum;i++)
		{
			memcpy(PTSon[i].Info.NameAdd, PTSon[i].Info.Name[SelectionMenu.IsEnableEnligh], strlen((ts_char*)PTSon[i].Info.Name[SelectionMenu.IsEnableEnligh])+1);
			PTSon[i].Parent = PTFather;
		}
	}
}

/**
 * @brief:创建菜单链表,创建不带头结点的双向循环链表
 * @parameter:IN_node_list:指向菜单列表项的指针
 * @parameter:Node_Max_Num:菜单列表中含有最大菜单项的数量
 */
void CreatMenuList(Point_MenuStructure IN_node_list,unsigned char Node_Max_Num)
{
	unsigned char NodeNum = 0;
	Point_MenuStructure ptlist,temp_list;
	temp_list = IN_node_list;
	
	vision_log_debug("CreatMenuList\n");
	if(temp_list !=NULL)
	{
		for(NodeNum = 0;NodeNum< Node_Max_Num;NodeNum++)
		{
			if(NodeNum == 0)
			{
				temp_list->Next = temp_list;
				temp_list->Parent = NULL;
				temp_list->Previous = temp_list;
				temp_list->Son = NULL;
			}
			else
			{
				ptlist = &IN_node_list[NodeNum];

				ptlist->Previous = temp_list;
				ptlist->Next = temp_list->Next;
				temp_list->Next->Previous = ptlist;
				temp_list->Next = ptlist;

				ptlist->Parent = NULL;
				ptlist->Son = NULL;

				temp_list = ptlist;
			}
		}
	}
}

void UserMenuTabSetSpacing(AllMenuItem* item , int xIncrement_spacing, int x_start, int tab_index)
{
	Point_MenuStructure pt = item->pt;
	int item_len = item->number;
	for(item_len=0; item_len<item->number; item_len++)
	{
		if(tab_index==0){
			pt[item_len].Info.line_X = x_start+(xIncrement_spacing+X_GAP)*item_len;
		}
		else{
			//需要特殊处理子菜单的固定间隔
			if(tab_index==8)//sun pt child
			{
				pt[item_len].Info.line_X = x_start+(xIncrement_spacing+X_GAP)*1; //子菜单固定间隔 
			}
			else if(tab_index==7|| tab_index==6 || tab_index==5)//   //测温点1~测温点9//测温区域1~测温区域4//"辐射率"、"反射温度"、"环境温度"、"相对湿度"、"距离
			{
				pt[item_len].Info.line_X = x_start+(xIncrement_spacing+X_GAP)*1; //子菜单固定间隔
			}
			else{
				pt[item_len].Info.line_X = x_start+(xIncrement_spacing+X_GAP)*(tab_index-1);
			}
		}
	}

}

void UserMenu_SetSpacing(int xIncrement_spacing,int x_start)
{
	int tab_index = 0;
	for(tab_index=0; tab_index<UsermenuTabNum; tab_index++){

		UserMenuTabSetSpacing(&UsermenuItemTable[tab_index] ,  xIncrement_spacing, x_start, tab_index);
	}

}
#ifdef CONFIG_SNR_PLANCK1280C
/**
 * @brief:创建用户菜单
 * @parameter:NULL
 */
void CreatUsermenu(void)
{
	InitUsermenuItemTable();
	vision_log_debug("CreatUsermenu\n");
	for(ts_u08 count = 0;count<UsermenuTabNum;count++)
	{
		CreatMenuList(UsermenuItemTable[count].pt,UsermenuItemTable[count].number);//创建菜单
	}
	Add_UsermenuItemSonList();
	for(ts_u08 count = 0;count<4;count++)
	{
		Add_MenuItemFatherList(UsermenuItemTable[count+1].pt,&UsermenuItemTable[0].pt[count]);
	}
	Add_MenuItemFatherList(UsermenuItemTable[5].pt,&UsermenuItemTable[2].pt[0]);
	Add_MenuItemFatherList(UsermenuItemTable[6].pt,&UsermenuItemTable[2].pt[1]);

	Add_MenuItemFatherList(UsermenuItemTable[8].pt,&UsermenuItemTable[1].pt[4]);

	Add_MenuItemFatherList(UsermenuItemTable[7].pt,&UsermenuItemTable[2].pt[3]);	//暂时先不启用
	if(SystemParaMenu2.Usermenu_SW == 0)//默认不开启测温功能：测温功能可在菜单二(测试菜单)中开启
	{
		UsermenuTab1[1].Son = NULL;
	}
}
#else
void CreatUsermenu(void)
{
	static int first_flag=1;
	int x_start=10;

	InitUsermenuItemTable();

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(first_flag==1){
		
		if(osd_type==VOT_MENU_OSD_RES_384){
			xIncrement_spacing = 80; //70+4
			SelectionMenu.IsEnableEnligh = 2;
			x_start=20;
			one_height_pixels = 16;//8+8;
		}
		else if(osd_type==VOT_MENU_OSD_RES_1280){
			xIncrement_spacing = 288;
			x_start=X_START*2; //64
			one_height_pixels = 48; //32+4+4+4+4
		}
		else{
			xIncrement_spacing = 144;
			x_start=X_START; //32
			one_height_pixels = 24; //16+2+2+2+2
		}
	}
	else{
		first_flag = 0;
	}

	UserMenu_SetSpacing( xIncrement_spacing,x_start);

	vision_log_debug("CreatUsermenu\n");
	for(ts_u08 count = 0;count<UsermenuTabNum;count++)
	{
		CreatMenuList(UsermenuItemTable[count].pt,UsermenuItemTable[count].number);//创建菜单
	}
	Add_UsermenuItemSonList();
	for(ts_u08 count = 0;count<4;count++)
	{
		Add_MenuItemFatherList(UsermenuItemTable[count+1].pt,&UsermenuItemTable[0].pt[count]);
	}
	Add_MenuItemFatherList(UsermenuItemTable[5].pt,&UsermenuItemTable[2].pt[0]);
	Add_MenuItemFatherList(UsermenuItemTable[6].pt,&UsermenuItemTable[2].pt[1]);

	Add_MenuItemFatherList(UsermenuItemTable[8].pt,&UsermenuItemTable[1].pt[4]);

	Add_MenuItemFatherList(UsermenuItemTable[7].pt,&UsermenuItemTable[2].pt[3]);	//暂时先不启用
	if(SystemParaMenu2.Usermenu_SW == 0)//默认不开启测温功能：测温功能可在菜单二(测试菜单)中开启
	{
		UsermenuTab1[1].Son = NULL;
	}
}
#endif

/**
 * @brief:菜单选中项初始化
 * @parameter:NULL
 */
void SelectionUserMenuInit(void)
{
	vision_log_debug("SelectionUserMenuInit\n");
	/**********************SelectionMenu.MenuState说明****************************
	菜单标志位(bit0-bit7):
		bit0-bit1是哪个菜单：00主界面，01用户菜单，10测试菜单，11测温点或区域移动或者半自动坏点校正
		bit2-bit4菜单级别：0-7级，
		bit5-bit7菜单组号：0-7组，
		注：当bit0-bit1为11时，即选中测温点或区域移动或者半自动坏点校正时：bit2-bit7没有级别与组号之分
		0x00：主界面(只有图像，没有菜单)
		0x25：用户菜单(菜单一)
		0x26：测试菜单(菜单二)
		0x03：测温点界面
		0x07：测温区域界面
		0x0b：半自动校点界面
		0x0f：四边形移动界面
		0x17：十字光标校正界面
		0x1f: 图像连续变倍功能使用
	****************************************************************************/
	SelectionMenu.SelectItem = NULL;//当前选中项是空
	SelectionMenu.MenuState = 0x00;//当前菜单状态为0，表示没有任何菜单，默认进入主界面
	RecallMenu.SelectItem = NULL;//此项是进入测温点或者测温区域的时候，需要记录当前选中的菜单项
	
    SelectionMenu.IsEnableEnligh = UsermenuParameter.IsEnableEnligh;


}

/**
 * @brief:用户菜单初始化
 * @parameter:NULL
 */
void UserMenuInit(void)
{
	
	SelectionUserMenuInit();
	vision_log_debug("UserMenuInit\n");
	CreatUsermenu();
	vision_log_debug(" SelectionMenu.IsEnableEnligh  %d \n", SelectionMenu.IsEnableEnligh);
	
}

/**
 * @brief:显示用户菜单的参数
 * @parameter:Pt_T6:指向当前选中菜单列表项的指针
 * @parameter:Status:0:显示当前参数值,1:参数++,2:参数--
 */
void DisplayUsermenuItemParameter(Point_MenuStructure Pt_Temp6,ts_u08 Status, ts_u08 is_active)
{
	vision_log_debug("DisplayUsermenuItemParameter Pt_Temp6=%p, Status=0x%02x, Pt_Temp6->Info.ID=0x%02x, is_active=%d\n", Pt_Temp6, Status, Pt_Temp6->Info.ID, is_active);

    vision_log_debug(" SelectionMenu.IsEnableEnligh  %d \n", SelectionMenu.IsEnableEnligh);
	/********************************************************************************/
	/*
	 * 以下每一项函数对应每一个菜单的功能函数
	 * 以下将每个功能函数单独列出来的原因：
	 * 1、为了后面方便维护.
	 * 2、增加代码的可读性.
	 * 3、此种做法,无疑是增加了Flash的开销,增加了代码量,是比较冗余的写法,考虑到外部Flash容量够大,所以不考虑对Flash的影响.(牺牲空间换便利)
	 * 4、此种做法有大量的重复的动作，后续可将部分菜单更新为统一的写法：当前9个测温点与5个测温区域已经融合到一起
	 */
	/********************************************************************************/
	switch(Pt_Temp6->Info.ID)
	{
#if 0
		case 0x11://"场景"
			GetAndDisplaySenceParameter(Pt_Temp6,Status);
			break;
		case 0x12://"模式"
			GetAndDisplayImageModeParameter(Pt_Temp6,Status);
			break;
#endif
		case 0x13://"亮度"
			GetAndDisplayBrightParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x14://"对比度"
			GetAndDisplayContrastParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x15://"伪彩"
			GetAndDisplayPaletteParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x16://"电子放大"
			GetAndDisplayEnlargeParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x180://""
			GetAndDisplaySunPtModeParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x181://""
			GetAndDisplaySunPtGapParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x18://"PIP"
			GetAndDisplayPIPParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x19://"HST"
			GetAndDisplayHSTParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x1B://"中心光标"
			GetAndDisplay_Crosshair_Parameter(Pt_Temp6,Status,is_active);
			break;
		case 0x23://"捕获"
			getAndDisplay_lutCapture_Parameter(Pt_Temp6,Status, is_active);
			break;
		case 0x25://"测温挡位"
			getAndDisplay_lutLevel_Parameter(Pt_Temp6,Status, is_active);
			break;
#if 0
		case 0x26://四边形
			GetAndDisplayQuadSwitchParameter(Pt_Temp6,Status);
			break;
		case 0x27://四边形区域报警阈值
			GetAndDisplayQuadAlarmSetParameter(Pt_Temp6,Status);
			break;
#endif
		case 0x31://"语言"
			GetAndDisplayLanguageParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x32://"字符颜色"
			// vision_log_debug("case 0x32\n");
			GetAndDisplayFontColorParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x33://"镜像"
			GetAndDisplayMirrorParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x34://"挡片模式"
			// GetAndDisplayShutterModeParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x35://"自动挡片"
			GetAndDisplayAutoShutterParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x36://"保存设置"
			SaveUsermenuParameter(Pt_Temp6,Status);
			break;
		case 0x37://"默认设置"
			ResetFactorySettingsParameter(Pt_Temp6,Status);
			break;
		case 0x38://"系统信息"
			// DisPlaySOCAndFPGAVersion(Status);
			DisPlayDeviceInfoAndSoftVersionInfoAuto(Status);
			break;
#if 0
#ifdef OPR_ALARM
		case 0x39://"阳光保护"
			GetAndDisplaySunProtect(Pt_Temp6,Status);
			break;
		case 0x3a://"保护时间"
			GetAndDisplayProtectTime(Pt_Temp6,Status);
			break;
#endif
#endif
		case 0x41://"图像增强"
			// GetAndDisplayEELeverParameter(Pt_Temp6,Status,is_active);
			GetAndDisplayEnhanceLeverParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x42://"空间滤波"
			GetAndDisplaySFilterParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x44://"场景"
			GetDisplayDeviceSenceMode(Pt_Temp6,Status,is_active);
			break;
		// case 0x43://"中心点"
		// 	GetAndDisplayCenterSpotParameter(Pt_Temp6,Status);
		// 	break;
		case 0x210://测温点1
		case 0x211://测温点2
		case 0x212://测温点3
		case 0x213://测温点4
		case 0x214://测温点5
		case 0x215://测温点6
		case 0x216://测温点7
		case 0x217://测温点8
		case 0x218://测温点9
			getAndDisplay_Spots_Parameter(Pt_Temp6,Status,is_active);
			break;
		case 0x220://测温区域1
		case 0x221://测温区域2
		case 0x222://测温区域3
		case 0x223://测温区域4
			getAndDisplay_Areas_Parameter(Pt_Temp6,Status,is_active);
			break;
		case 0x240://"辐射率"
			GetAndDisplayRadianceParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x241://"反射温度"
			GetAndDisplayReflectionTemperatureParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x242://"环境温度"
			GetAndDisplayEnvironmentTemperatureParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x243://"相对湿度"
			GetAndDisplayRelativeHumidityParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x244://"距离"
			GetAndDisplayDistanceParameter(Pt_Temp6,Status,is_active);
			break;
		case 0x245://"镜头设置"
			GetAndDisplayLensSetParameter(Pt_Temp6,Status,is_active);
			break;
		default:
			break;
	}
}

/**
 * @brief:更新菜单的信息
 * @parameter:Pt_T0:指向当前选中菜单列表项的指针
 * @parameter:MenuName:当前菜单需要更新的信息
 * @parameter:is_active:当前菜单是否需要激活显示，0：不需要，1：需要
 */
void Update_MenuItemLabel(Point_MenuStructure Pt_T0, ts_u08 *MenuName, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	vision_log_debug("Update_MenuItemLabel Name:[%s], MenuNameLen=%d Y %d, X %d, ID 0x%04x, SonNum %d is_active %d\n", MenuName, strlen((ts_char*)MenuName), Pt_T0->Info.line_Y, Pt_T0->Info.line_X, Pt_T0->Info.ID, Pt_T0->Info.SonNum, is_active);
    st_attr.u16_left = Pt_T0->Info.line_X;
    st_attr.u16_top = Pt_T0->Info.line_Y*one_height_pixels+one_height_pixels;
    if(SelectionMenu.MenuState==0x26)
    	st_attr.u16_width = 100;
    else
    	st_attr.u16_width = 144;
    st_attr.u16_height = one_height_pixels;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		if(SelectionMenu.MenuState==0x26){
			st_attr.u16_width = test_xIncrement_spacing+4; 
		}
		else{
			st_attr.u16_width = xIncrement_spacing+4; 
		}
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		st_attr.u16_left = Pt_T0->Info.line_X*2;
		st_attr.u16_top = Pt_T0->Info.line_Y*48+48;
		if(SelectionMenu.MenuState==0x26)
			st_attr.u16_width = 200;
		else
			st_attr.u16_width = 288;
		st_attr.u16_height = 48;
	}


    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 2;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

    if(is_active)
    {
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
    	st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_RED;//OSD_COLOR_GRAY;
    }
    else
    {
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    st_attr.u08_rim_width = 3;


	ts_lib_fontl_gen_label_1(global_sys_ctrl_info.pst_framebuffer_osd, (ts_char*)Pt_T0->Info.NameAdd, (ts_fontl_lbl_atts_s_t *)&st_attr);
	
}

/**
 * @brief:显示的菜单的信息
 * @parameter:Pt_T0:指向当前选中菜单列表项的指针
 * @parameter:number:当前有几个菜单需要显示
 */
void Display_MenuItemLabel(Point_MenuStructure Pt_T0, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	vision_log_debug("enter Display_MenuItemLabel \n");

	vision_log_debug("Display_MenuItemLabel Name:%s, Y %d, X %d, ID 0x%04x, SonNum %d is_active %d\n", Pt_T0->Info.Name[SelectionMenu.IsEnableEnligh], Pt_T0->Info.line_Y, Pt_T0->Info.line_X, Pt_T0->Info.ID, Pt_T0->Info.SonNum, is_active);
    st_attr.u16_left = Pt_T0->Info.line_X ;
    st_attr.u16_top = Pt_T0->Info.line_Y*one_height_pixels+one_height_pixels;
	st_attr.u16_height = one_height_pixels;
    if(SelectionMenu.MenuState==0x26){
    	st_attr.u16_width = 100; //96+4
	}
    else{
    	st_attr.u16_width = 144; //140+4
	}

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		if(SelectionMenu.MenuState==0x26){
			st_attr.u16_width = test_xIncrement_spacing+4; 
		}
		else{
			st_attr.u16_width = xIncrement_spacing+4; 
		}
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		 st_attr.u16_left = Pt_T0->Info.line_X*2;
		st_attr.u16_top = Pt_T0->Info.line_Y*48+48;
		if(SelectionMenu.MenuState==0x26)
			st_attr.u16_width = 200;
		else
			st_attr.u16_width = 288;
		st_attr.u16_height = 48;
	}
    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 2;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

    if(is_active)
    {
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
    	st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_RED;//OSD_COLOR_GRAY;
    }
    else
    {
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    st_attr.u08_rim_width = 3;

	ts_lib_fontl_gen_label_1(global_sys_ctrl_info.pst_framebuffer_osd, (ts_char*)Pt_T0->Info.NameAdd, (ts_fontl_lbl_atts_s_t *)&st_attr);
}

/**
 * @brief:显示的菜单的信息
 * @parameter:Pt_T0:指向当前选中菜单列表项的指针
 * @parameter:number:当前有几个菜单需要显示
 */
void Clear_MenuItemLabel(Point_MenuStructure Pt_T0)
{
    ts_fontl_lbl_atts_s_t st_attr;
	vision_log_debug("Clear_MenuItemLabel Name:%s, Y %d, X %d, ID 0x%04x, SonNum %d\n", Pt_T0->Info.Name[SelectionMenu.IsEnableEnligh], Pt_T0->Info.line_Y, Pt_T0->Info.line_X, Pt_T0->Info.ID, Pt_T0->Info.SonNum);
    st_attr.u16_left = Pt_T0->Info.line_X;
    st_attr.u16_top = Pt_T0->Info.line_Y*one_height_pixels+one_height_pixels;
	st_attr.u16_height = one_height_pixels;
    if(SelectionMenu.MenuState==0x26){
    	st_attr.u16_width = 100;
	}
    else{
    	st_attr.u16_width = 144;
	}

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		if(SelectionMenu.MenuState==0x26){
			st_attr.u16_width = test_xIncrement_spacing+4; 
		}
		else{
			st_attr.u16_width = xIncrement_spacing+4; 
		}
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		st_attr.u16_left = Pt_T0->Info.line_X*2;
		st_attr.u16_top = Pt_T0->Info.line_Y*48+48;
		if(SelectionMenu.MenuState==0x26)
			st_attr.u16_width = 200;
		else
			st_attr.u16_width = 288;
		st_attr.u16_height = 48;
	}
	
    st_attr.u08_visiable = 0;
    st_attr.u08_priority = 2;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

    st_attr.u08_font_color = OSD_COLOR_BLACK;
    st_attr.u08_bg_color = OSD_COLOR_BLACK;
    st_attr.u08_rim_color = OSD_COLOR_BLACK;
    st_attr.u08_rim_width = 3;
    ts_lib_fontl_gen_label_1(global_sys_ctrl_info.pst_framebuffer_osd, (ts_char*)Pt_T0->Info.NameAdd, (ts_fontl_lbl_atts_s_t *)&st_attr);
}



void Update_Display_UserMenuItem(Point_MenuStructure Pt_T0, ts_u08 number, unsigned short ID )
{
	vision_log_debug("Update_Display_UserMenuItem SelectionMenu.MenuState=0x%02x, number=%d, ID=%d\n", SelectionMenu.MenuState, number, ID);
	if(Pt_T0 == NULL)
	{
		return;
	}
	else
	{
		for(ts_u08 i=0;i<number;i++)
		{
			vision_log_debug("i=%d,Pt_T0=%p number=%d, number_active=0x%x\n", i, Pt_T0, number, ID);

			if(Pt_T0->Info.ID ==ID){
				DisplayUsermenuItemParameter(Pt_T0,0,1);
			}
			else{
				DisplayUsermenuItemParameter(Pt_T0,0,0);
			}

			Pt_T0 = Pt_T0->Next;
		}
	}
}


/**
 * @brief:显示的菜单的信息
 * @parameter:Pt_T0:指向当前选中菜单列表项的指针
 * @parameter:number:当前有几个菜单需要显示
 */
void Display_MenuItem(Point_MenuStructure Pt_T0,ts_u08 number,ts_u08 number_active)
{
	// ts_u08 FrameLength = 0;
	// ts_u08 BackgroundLength = 0;
	// ts_u08 TotalLength = 0;
	// ts_u08 GapLength = 0;
	
	vision_log_debug("Display_MenuItem SelectionMenu.MenuState=0x%02x, number=%d, number_active=%d\n", SelectionMenu.MenuState, number, number_active);
	if(Pt_T0 == NULL)
	{
		return;
	}
	else
	{
		switch(SelectionMenu.MenuState)
		{
			case 0x25:
				// FrameLength = 140;
				// BackgroundLength = 134;
				// TotalLength = 144;
				// GapLength = 16-8*SelectionMenu.IsEnableEnligh;
				break;

			case 0x26:
				// FrameLength = 96;
				// BackgroundLength = 88;
				// TotalLength = 100;
				// GapLength = 8;
				break;
		}
		for(ts_u08 i=0;i<number;i++)
		{
			vision_log_debug("i=%d,Pt_T0=%p number=%d, number_active=%d\n", i, Pt_T0, number, number_active);

			if(i == number_active)
			{
				Display_MenuItemLabel(Pt_T0, 1);
			}
			else
			{
				Display_MenuItemLabel(Pt_T0, 0);
			}

			switch(SelectionMenu.MenuState)
			{
				case 0x25:
					DisplayUsermenuItemParameter(Pt_T0,0,(i == number_active));
					break;

				case 0x26:
					DisplayTestmenuItemParameter(Pt_T0,0,(i == number_active));
					break;
			}
			Pt_T0 = Pt_T0->Next;
		}
	}
}

/**
 * @brief:更改选中的菜单项
 * @parameter:Pt_T1:指向当前选中菜单列表项的指针
 * @parameter:order:0:退出父节点,进入子节点,1:退出前驱,进入后继,2:退出后继,进入前驱,3:退出子节点，回到父亲节点
 */
void Change_SelectionMenu(Point_MenuStructure Pt_T1,ts_u08 order)
{
	// ts_u08 FrameLength = 0;
	// ts_u08 BackgroundLength = 0;
	// ts_u08 TotalLength = 0;
	// ts_u08 GapLength = 0;
	Point_MenuStructure Pt_T2 = NULL;
	vision_log_debug("Change_SelectionMenu Pt_T1=%p, SelectionMenu.MenuState=0x%02x, order=%d\n", Pt_T1, SelectionMenu.MenuState, order);
	switch(SelectionMenu.MenuState)
	{
		case 0x25:
			// FrameLength = 140;
			// BackgroundLength = 134;
			// TotalLength = 144;
			// GapLength = 16-8*SelectionMenu.IsEnableEnligh;
			break;

		case 0x26:
			// FrameLength = 96;
			// BackgroundLength = 88;
			// TotalLength = 100;
			// GapLength = 8;
			break;
	}
	switch(order)
	{
		case 0:
			Pt_T2 = Pt_T1->Parent;//上一级
			Display_MenuItemLabel(Pt_T2, 1);//is_active = 1
			break;

		case 1:
			Pt_T2 = Pt_T1->Previous;//前驱
			Display_MenuItemLabel(Pt_T2, 0);//is_active = 0
			break;

		case 2:
			Pt_T2 = Pt_T1->Next;//后继
			Display_MenuItemLabel(Pt_T2, 0);//is_active = 0
			break;

		case 3:
			Pt_T2 = NULL;//空指针
			break;
	}
	// if(Pt_T2 != NULL)
	// {
	// 	ClearSelectionMenuFrame(Pt_T2->Info.line_X,Y_START+Y_GAP*Pt_T2->Info.line_Y,
	// 				FrameLength,BACKGROUND_LAYER,5);
	// }
	// DisplaySelectionMenuFrame(Pt_T1->Info.line_X,Y_START+Y_GAP*Pt_T1->Info.line_Y,
	// 		FrameLength,BACKGROUND_LAYER,6);

	Display_MenuItemLabel(Pt_T1, 1);//is_active = 1
}

/**
 * @brief:清除显示的菜单信息
 * @parameter:Pt_T3:指向当前选中菜单列表项的指针
 * @parameter:number:需要清除的数量
 */
void Clear_MenuInfo(Point_MenuStructure Pt_T3,ts_u08 number)
{
	// int i,j;
	// ts_u32 addr;

	vision_log_debug("Clear_MenuInfo SelectionMenu.MenuState=0x%02x, number=%d\n", SelectionMenu.MenuState, number);
	for(ts_u08 i = 0;i<number;i++)
	{
		switch(SelectionMenu.MenuState)
		{
			case 0x25:
				ClearSize16X22(Pt_T3->Info.line_X,X_INCREMENT,Y_START);
				Clear_MenuItemLabel(Pt_T3);
				break;

			case 0x26:
				ClearSize16X22(Pt_T3->Info.line_X,TEST_X_INCREMENT+2,Y_START);
				Clear_MenuItemLabel(Pt_T3);		
				break;
		}
		Pt_T3 = Pt_T3->Next;
	}
}

/**
 * @brief:清除显示的菜单子链信息
 * @parameter:Pt_T4:指向当前选中菜单列表项的指针
 * @parameter:number:需要清除的数量
 * @parameter:SonStartYline:子链起始的行
 */
void Clear_MenuSonInfo(Point_MenuStructure Pt_T4,ts_u08 number,ts_u08 SonStartYline)
{
	// int i,j;
	// ts_u32 addr;

	vision_log_debug("Clear_MenuSonInfo SelectionMenu.MenuState=0x%02x, number=%d\n", SelectionMenu.MenuState, number);
	for(ts_u08 i = 0;i<number;i++)
	{
		switch(SelectionMenu.MenuState)
		{
			case 0x25:
				// ClearSize16X22(Pt_T4->Info.line_X,X_INCREMENT,Y_START+22*(i+SonStartYline));
				Clear_MenuItemLabel(Pt_T4);
				if(Pt_T4->Info.ID==0x38){
					DisPlayDeviceInfoAndSoftVersionInfoAuto(0);
				}
				break;
			case 0x26:
				// ClearSize16X22(Pt_T4->Info.line_X,TEST_X_INCREMENT+2,Y_START+22*(i+SonStartYline));
				Clear_MenuItemLabel(Pt_T4);
				break;
		}
		Pt_T4 = Pt_T4->Next;
	}
}

/**
 * @brief:使用按键进入用户菜单链表的下一项
 * @parameter：NULL
 */
void Key_EnterNextUsermenuElement(void)
{
	vision_log_debug("Key_EnterNextUsermenuElement SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.SelectItem == NULL&& SelectionMenu.MenuState!= 0x17 && SelectionMenu.MenuState!= 0x0b)
	{
		vision_log_debug("SelectionMenu.SelectItem  %p \n", SelectionMenu.SelectItem);
		if((SystemParaMenu2.Auto_Semi))		BadSemiAutoOpr(&Areainit[4],2,AreaBad_XY_Normal);//状态2：表示下键按下
		return;
	}
	else
	{
// #ifdef	SUPPORT_CONTINUOUS_ZOOM
// 		//当前选中项是电子放大界面，且电子放大不是1.0倍,则不能进入菜单二
// 		if(UsermenuParameter.Enlarge>10)
// 		{
// 			return;
// 		}
// #else
// 		//当前选中项是电子放大界面，且电子放大选中了X2/X4/X8,则不能做其他操作
// 		if(UsermenuParameter.Enlarge)
// 		{
// 			return;
// 		}		
// #endif		
	
// 		else
		{
			switch(SelectionMenu.MenuState)
			{
				case 0x25://用户菜单界面：菜单一
				case 0x26://测试菜单界面：菜单二
					SelectionMenu.SelectItem = SelectionMenu.SelectItem->Next;//当前选中后继节点
					Change_SelectionMenu(SelectionMenu.SelectItem,1);//更新选中项：当前选中后继节点
					break;
				case 0x17://十字光标校正界面
					BPC_Maual_mode_move_cursor(0,2);//表示下键
				break;						
				case 0x0b://半自动校点界面
					BadSemiAutoOpr(select_Area,2,AreaBad_XY_Normal);//状态2：表示下键按下
					break;
				case 0x03://测温点界面
					Disp_SpotN_Messages(select_Spot,2);//状态2：表示下键按下
					// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
					break;
				case 0x07://测温区域界面
					AreaN_Scale(select_Area,2,AreaBad_XY_Normal); //状态2：表示下键按下
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
					// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					break;

				// case 0x0f://四边形界面
				// 	#ifdef	Electrolytic_copper
				// 	QuadMoveDown();//状态2：表示下键按下
				// 	#endif
				// 	break;
			}
		}
	}
}

/**
 * @brief:使用按键进入用户菜单链表的上一项
 * @parameter：NULL
 */
void Key_EnterPreviousUsermenuElement(void)
{
	vision_log_debug("Key_EnterPreviousUsermenuElement SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.SelectItem == NULL && SelectionMenu.MenuState!= 0x17 && SelectionMenu.MenuState!= 0x0b)
	{
		vision_log_debug("SelectionMenu.SelectItem  %p \n", SelectionMenu.SelectItem);
		if((SystemParaMenu2.Auto_Semi))		{BadSemiAutoOpr(&Areainit[4],1,AreaBad_XY_Normal); }//状态1：表示上键按下

		return;
	}
	else
	{
// #ifdef	 SUPPORT_CONTINUOUS_ZOOM
// 		//当前选中项是电子放大界面，且电子放大不是1.0倍,则不能进入菜单二
// 		if(UsermenuParameter.Enlarge>10)
// 		{
// 			return;
// 		}
// #else
// 		//当前选中项是电子放大界面，且电子放大选中了X2/X4/X8,则不能做其他操作
// 		if(UsermenuParameter.Enlarge)
// 		{
// 			return;
// 		}		
// #endif		

// 		else
		{
			switch(SelectionMenu.MenuState)
			{
				case 0x25://用户菜单界面：菜单一
				case 0x26://测试菜单界面：菜单二
					SelectionMenu.SelectItem = SelectionMenu.SelectItem->Previous;//当前选中前驱节点
					Change_SelectionMenu(SelectionMenu.SelectItem,2);//更新选中项：当前选中前驱节点
					break;
				case 0x17://十字光标校正界面
					BPC_Maual_mode_move_cursor(0,1);//表示上键
				break;
				case 0x0b://半自动校点界面
					BadSemiAutoOpr(select_Area,1,AreaBad_XY_Normal);//状态1：表示上键按下
					break;
				case 0x03://测温点界面
					Disp_SpotN_Messages(select_Spot,1);//状态1：表示上键按下
					// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
					break;
				case 0x07://测温区域界面
					AreaN_Scale(select_Area,1,AreaBad_XY_Normal); //状态1：表示上键按下
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
					// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					break;

				// case 0x0f://四边形移动界面
				// 	#ifdef	Electrolytic_copper
				// 	QuadMoveUp();//状态1：表示上键按下
				// 	#endif
				// 	break;
			}
		}
	}

}

/**
 * @brief:按键进入菜单的子链表
 * @parameter：NULL
 */
void Key_EnterSonList(void)
{
	ts_u08 ResidueID = 0;
	ts_u16 TempID = 0;
	//菜单一子链为空分俩种情况:1、按确认键进入测温区域与测温点移动的界面
	//					 2、在没有子链的菜单项需要直接返回。
	vision_log_debug("Key_EnterSonList SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	vision_log_debug("SelectionMenu.SelectItem->Info.ID=0x%02x\n",	SelectionMenu.SelectItem->Info.ID);

	if(SelectionMenu.SelectItem->Son == NULL)
	{
		if((SelectionMenu.SelectItem->Info.ID>=0x210)&&(SelectionMenu.SelectItem->Info.ID<=0x218))
		{//进入测温点1~测温点9
			if(Spot_state)
			{
				if((SelectionMenu.SelectItem->Info.ID<0x210)||(SelectionMenu.SelectItem->Info.ID>0x218))
				{
					return;
				}
				ResidueID = (ts_u08)(SelectionMenu.SelectItem->Info.ID-0x210);
				TempID = 1<<ResidueID;
				if(Spot_state&TempID)
				{
					RecallMenu.SelectItem = SelectionMenu.SelectItem;
					//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
					while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
					{
						SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
						Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
								SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
					}
					Clear_MenuInfo(SelectionMenu.SelectItem,UTab1_Length);//清除一级菜单，退出一级菜单
					ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
					select_Spot = &SpotInit[ResidueID];
					SelectionMenu.MenuState = 0x03;//当前菜单状态为0x03,表示进入测温点界面
					Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				}
			}
		}
		else if((SelectionMenu.SelectItem->Info.ID>=0x220)&&(SelectionMenu.SelectItem->Info.ID<=0x223))
		{//进入测温区域1~测温区域4
			if(Area_state & 0x000F)
			{
				if((SelectionMenu.SelectItem->Info.ID<0x220)||(SelectionMenu.SelectItem->Info.ID>0x223))
				{
					return;
				}
				ResidueID = (ts_u08)(SelectionMenu.SelectItem->Info.ID-0x220);
				TempID = 1<<ResidueID;
				if(Area_state&TempID)
				{
					RecallMenu.SelectItem = SelectionMenu.SelectItem;
					//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
					while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
					{
						SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
						Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
								SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
					}
					Clear_MenuInfo(SelectionMenu.SelectItem,UTab1_Length);//清除一级菜单
					ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
					select_Area = &Areainit[ResidueID];
					SelectionMenu.MenuState = 0x07;//当前菜单状态为0x07,表示进入测温区域界面
					// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
					Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				}
			}
		}
		else if(SelectionMenu.SelectItem->Info.ID == 0xAA)//进入半自动校点界面
		{
			if(SystemParaMenu2.Auto_Semi)//打开半自动校点后，才可以进入校正页面
			{
				//RecallMenu.SelectItem = SelectionMenu.SelectItem;
				//SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
				//Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,
						// SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
				//Clear_MenuInfo(SelectionMenu.SelectItem,TTab1_Length);//清除一级菜单
				//SelectionMenu.MenuState = 0x0B;//当前菜单状态为0x0B,表示进入半自动校点界面
				//GetAndDisplayIspStatisInfo(0);//清除DET信息栏

				//if( SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ORG){	
				//SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
				//cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);}
				//BadArea_Display(1); //坏点区域显示
				EnterOrExit_SemiAutoMode(0);	// 进入半自动校点
			}
		}
		else if(SelectionMenu.SelectItem->Info.ID == 0x26)//进入移动四边形界面
		{
			if(UsermenuParameter.Quad_SW)//开启四边形显示功能才可以进入
			{
				RecallMenu.SelectItem = SelectionMenu.SelectItem;
				//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
				while(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
				{
					SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
					Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
							SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
				}
				Clear_MenuInfo(SelectionMenu.SelectItem,UTab1_Length);//清除一级菜单，退出一级菜单
				ClearSize16X16(32,20*16,Y_START+VERSION_YLINE);//清掉版本信息
				#ifdef	Electrolytic_copper
				drawIrregularPolygonBox(vertices, CharacterColor);
				#endif
				SelectionMenu.MenuState = 0x0F;//当前菜单状态为0x0F,表示进入四边形移动界面
			}
		}
		else if(SelectionMenu.SelectItem->Info.ID == 0xA2)//进入十字光标校正界面
		{
			vision_log_debug("SelectionMenu.SelectItem->Info.ID:%x\n",SelectionMenu.SelectItem->Info.ID);
//			if(SystemParaMenu2.BAD_Cross)//开启显示功能才可以进入
			{
// 				RecallMenu.SelectItem = SelectionMenu.SelectItem;
// 				SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
// 				Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,
// 						SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
// 				Clear_MenuInfo(SelectionMenu.SelectItem,TTab1_Length);//清除一级菜单
// 				SelectionMenu.MenuState = 0x17;//当前菜单状态为0x17,表示进入十字光标校点界面
// //				APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,
// //						(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(0<<6));
// 				ClearFont(0,64);//关闭传输数据至HyperRAM的标志位，清空HyperRAM的数据
				// global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_BPC;
				// GetAndDisplayBPC_Maual();
				EnterOrExitBPCMaualMode(0);
			}
		}
	}
	else
	{
		//关闭阳光保护的时候，就不进入菜单一中的阳光保护选项
		if(SelectionMenu.SelectItem->Info.ID == 0x1a ){
			if(SystemParaMenu2.SunPT_SW==0){
				return;
			}
		}

		vision_log_debug("SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem \n");
		//如果当前的选中的菜单项不是一级菜单，只有一级菜单的父节点是自己
		if(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
		{
			vision_log_debug("SelectionMenu.SelectItem->Parent->Info.SonNum %d \n",SelectionMenu.SelectItem->Parent->Info.SonNum);
			vision_log_debug("SelectionMenu.SelectItem->Info.line_Y %d \n",SelectionMenu.SelectItem->Info.line_Y);
			
			Clear_MenuSonInfo(SelectionMenu.SelectItem,\
					SelectionMenu.SelectItem->Parent->Info.SonNum-SelectionMenu.SelectItem->Info.line_Y+1,\
					SelectionMenu.SelectItem->Info.line_Y+1);//清除当前选中的菜单的后面所有的后继结点
		}
		SelectionMenu.SelectItem = SelectionMenu.SelectItem->Son;//选中儿子结点
		Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示子链
		Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
	}
//	APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,
//			(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xff00ffff)|(SelectionMenu.MenuState<<16));
}

/**
 * @brief:按键退出菜单的子链表,回到父节点
 * @parameter：NULL
 */
void Key_ExitSonList(void)
{
	vision_log_debug("Key_ExitSonList SelectionMenu.MenuState=0x%02x, SelectionMenu.SelectItem=%p\n", SelectionMenu.MenuState, SelectionMenu.SelectItem);
	if(SelectionMenu.SelectItem == NULL)
	{
		return;
	}
	else
	{
		//一级菜单的父节点是自己,这里判断当前菜单选中项的父节点是不是自己,是说明已经在一级菜单，反之则不在一级菜单
		if(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
		{
			SelectionMenu.SelectItem = SelectionMenu.SelectItem->Parent;//选中父节点
			Clear_MenuSonInfo(SelectionMenu.SelectItem->Son,SelectionMenu.SelectItem->Info.SonNum,\
					SelectionMenu.SelectItem->Son->Info.line_Y);//清除子链信息
			Change_SelectionMenu(SelectionMenu.SelectItem,3);//更新选中项：当前退出子结点，选中父节点
#ifdef SUPPORT_CONTINUOUS_ZOOM
		// if(SelectionMenu.MenuState==0x25)//清除变倍显示
		// 	{
		// 		 EraseZoomMeg();
		// 	}
#endif

			//如果当前的选中的菜单项的父节点还不是自己，说明之前是从三级菜单退出的，此时需要恢复二级菜单的其他子链信息
			if(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
			{
				Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);
			}
		}
	}
}

/**
 * @brief:使用按键进入用户菜单界面
 * @parameter：NULL
 */
void Key_EnterUsermenu(void)
{
	vision_log_debug("Key_EnterUsermenu SelectionMenu.MenuState=%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.MenuState == 0x00 )//当前没有选中任何菜单
	{
		// Erase_Deviceinfo();
		SelectionMenu.SelectItem = &UsermenuTab1[0];//当前选中项是空
		SelectionMenu.MenuState = 0x25;//当前菜单状态为0x25,进入菜单一
		Display_MenuItem(SelectionMenu.SelectItem,UTab1_Length, 0);

	}
	else
	{
		switch(SelectionMenu.MenuState)
		{
			case 0x25://用户菜单：菜单一
			case 0x26://测试菜单：菜单二
				Key_EnterSonList();
				break;
			case 0x03://测温点界面
			case 0x07://测温区域界面
				SelectionMenu.MenuState = 0x25;//当前菜单状态为0x25,进入菜单一
				SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				Display_MenuItem(SelectionMenu.SelectItem->Parent,UTab1_Length, 0);//显示一级菜单
				Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				if(!BackgroundColor)//如果没有背景色，则需要清除所有的显示的后继结点
				{
					Clear_MenuSonInfo(SelectionMenu.SelectItem,\
							SelectionMenu.SelectItem->Parent->Info.SonNum-SelectionMenu.SelectItem->Info.line_Y+1,\
							SelectionMenu.SelectItem->Info.line_Y+1);//清除当前选中的菜单的后面所有的后继结点
				}
				SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示三级菜单
				Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
				if(Spot_state)
				{
					Spot_SimultaneousDisplay(Spot_state);
				}
				ReDispArea1Message();
				break;

			case 0x0B://半自动校点界面
			#if 0  //取消按确认键退出半自动校点界面功能
				SelectionMenu.MenuState = 0x26;//当前菜单状态为0x26,进入菜单二
				SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				Display_MenuItem(SelectionMenu.SelectItem->Parent,TTab1_Length, 0);//显示一级菜单
				SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
//				APB3_REGW(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0,
//									(APB3_REGR(IO_APB_SLAVE_0_INPUT_TEST_STATE_OFFSET_XY,0)&0xffffffbf)|(1<<6));
			#endif
				BADMarkSemiAutoOpr(); //确认键校正坏点
				break;

			case 0x0f://四边形移动界面
				SelectionMenu.MenuState = 0x25;//当前菜单状态为0x25,进入菜单一
				SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				Display_MenuItem(SelectionMenu.SelectItem->Parent,UTab1_Length, 0);//显示一级菜单
				SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
				break;

			case 0x17://十字光标坏点校正界面
				// SelectionMenu.MenuState = 0x26;//当前菜单状态为0x26,进入菜单二
				// SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				// Display_MenuItem(SelectionMenu.SelectItem->Parent,TTab1_Length, 0);//显示一级菜单
				// SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				// Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				// Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
				// Exit_BPC_Maual_mode();
				// global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
				BPC_Maual_mode_set_or_unset_bpc_with_auto();
				break;
		}
	}
}

/**
 * @brief:使用按键进入用户菜单界面
 * @parameter：NULL
 */
void Key_ExitUsermenu(void)
{
	ts_u08 Len = 0;
	vision_log_debug("Key_ExitUsermenu SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.SelectItem == NULL && SelectionMenu.MenuState!=0x17 && SelectionMenu.MenuState!=0x0b)
	{
		vision_log_debug("SelectionMenu.SelectItem == NULL\n");
		return;
	}
	else
	{
// #ifdef  SUPPORT_CONTINUOUS_ZOOM	
// 			//当前选中项是电子放大界面，且电子放大不是1.0倍,则不能进入菜单二
// 		if(UsermenuParameter.Enlarge>10)
// 		{
// 			return;
// 		}
// #else		
// 		//当前选中项是电子放大界面，且电子放大选中了X2/X4/X8,则不能做其他操作
// 		if(UsermenuParameter.Enlarge)
// 		{
// 			return;
// 		}
// #endif		
		switch(SelectionMenu.MenuState)
		{
			case 0x25://用户菜单：菜单一
			case 0x26://测试菜单：菜单二
				if(SelectionMenu.SelectItem->Parent != SelectionMenu.SelectItem)
				{
					Key_ExitSonList();
				}
				else
				{
					if(SelectionMenu.MenuState == 0x25)
					{
						Len = UTab1_Length;
					}
					else if(SelectionMenu.MenuState == 0x26)
					{
						Len = TTab1_Length;
						//if(SystemParaMenu2.Auto_Semi)//如果现在打开了半自动校点，然后需要退出菜单二，把半自动校点也关掉
						//{
						//	if(Area_state&0x0010)
						//	{
						//		Area_state = (Area_state&0xffef)|0x0000;
						//		// DisPlay_BAD_Opr(&BadMes,2,0);
						//		BadArea_Display(0);
						//	}
						//	SystemParaMenu2.Auto_Semi = 0;
						//}

						// ClearFont(0,64);
						//清楚底部av信息栏
						// GetAndDisplayIspStatisInfo(0);
						
					}
					Clear_MenuInfo(SelectionMenu.SelectItem,Len);//清除一级菜单，退出一级菜单
					SelectionMenu.SelectItem = NULL;//当前选中项是空，退出一级菜单
					SelectionMenu.MenuState = 0x00;//当前菜单状态为0，表示没有任何菜单，也没有进入任何界面
					if(UsermenuParameter.CenterSpotSW)	Display_CenterSpot(UsermenuParameter.CenterSpotSW);  //中心光标显示（无温度）
					ISP_set_bottom_statistic_force_flag(1);
				}
				break;
			case 0x03://测温点界面
			case 0x07://测温区域界面
			case 0x0f://四边形界面
				break;
			case 0x17://十字光标坏点校正界面
				// if(SelectionMenu.SelectItem){
				// 	SelectionMenu.MenuState = 0x26;//当前菜单状态为0x26,进入菜单二
				// 	SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				// 	Display_MenuItem(SelectionMenu.SelectItem->Parent,TTab1_Length, 0);//显示一级菜单
				// 	SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				// 	Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				// 	Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
				// }
				// Exit_BPC_Maual_mode();
				// global_sys_ctrl_info.m_system_mode = SYSTEM_MODE_NORMAL;
				EnterOrExitBPCMaualMode(1);
				break;
			case 0x0b://半自动校点界面
				//if(SystemParaMenu2.m_cvbs_display_mode != DEVICE_CVBS_DISPLAY_MODE_ENLARGE){	
				//SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
				//cvbs_display_mode_set(SystemParaMenu2.m_cvbs_display_mode);}
				//SelectionMenu.MenuState = 0x26;//当前菜单状态为0x26,进入菜单二
				//SelectionMenu.SelectItem = RecallMenu.SelectItem->Parent;//选中父结点
				//Display_MenuItem(SelectionMenu.SelectItem->Parent,TTab1_Length, 0);//显示一级菜单
				//SelectionMenu.SelectItem = RecallMenu.SelectItem;//选中之前记录的菜单项
				//Display_MenuItem(SelectionMenu.SelectItem,SelectionMenu.SelectItem->Parent->Info.SonNum, 0);//显示二级菜单
				//Change_SelectionMenu(SelectionMenu.SelectItem,0);//更新选中项：当前退出父节点，选中儿子结点
				EnterOrExit_SemiAutoMode(1);	//退出半自动校点
				break;
		}
	}

	
}

/**
 * @brief:使用右键更新菜单参数:参数++
 * @parameter：NULL
 */
void Key_RightUpdataParameter(void)
{
	vision_log_debug("Key_RightUpdataParameter SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.SelectItem == NULL && SelectionMenu.MenuState!= 0x17 && SelectionMenu.MenuState!= 0x0b)
	{
		vision_log_debug("SelectionMenu.SelectItem  %p \n", SelectionMenu.SelectItem);
		if((SystemParaMenu2.Auto_Semi))		BadSemiAutoOpr(&Areainit[4],4,AreaBad_XY_Normal);//状态4：表示右键按下
		return;
	}
	else
	{
		unsigned short ID=0;
		switch(SelectionMenu.MenuState)
		{
			case 0x25://用户菜单：菜单一
				DisplayUsermenuItemParameter(SelectionMenu.SelectItem,1, 1);
				ID = SelectionMenu.SelectItem->Info.ID;	
				if(ID == 0x37  || ID == 0x44){			
					Update_Display_UserMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,ID);
				}
				break;

			case 0x26://测试菜单：菜单二
				if(SelectionMenu.SelectItem->Info.ID==0xA2) return;
				DisplayTestmenuItemParameter(SelectionMenu.SelectItem,1, 1);
				ID = SelectionMenu.SelectItem->Info.ID;	
				if(ID == 0x61||ID == 0x68|| ID == 0x69 ||  ID == 0x6B || ID==0xb3 || ID == 0x102 ){
					Update_Display_TestMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,ID);
					
				}
				break;
			case 0x17://十字光标校正界面
				BPC_Maual_mode_move_cursor(2, 0);//表示右键按下
				break;
			case 0x0b://半自动校点界面
				BadSemiAutoOpr(select_Area,4,AreaBad_XY_Normal);//状态4：表示右键按下
				break;
			case 0x03://测温点界面
				Disp_SpotN_Messages(select_Spot,4);//状态4：表示右键按下
				// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
				Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				break;

			case 0x07://测温区域界面
				AreaN_Scale(select_Area,4,AreaBad_XY_Normal); //状态4：表示右键按下
				Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
				break;

			// case 0x1f://变倍显示
			// 	GetAndDisplayEnlargeParameter(SelectionMenu.SelectItem,1);
			// 	break;

			// case 0x0f://四边形移动界面
			// 	#ifdef	Electrolytic_copper
			// 	QuadMoveRight();//状态3：表示左键按下
			// 	#endif
			// 	break;
		}
	}
}

/**
 * @brief:使用左键更新菜单参数:参数--
 * @parameter：NULL
 */
void Key_LeftUpdataParameter(void)
{
	vision_log_debug("Key_LeftUpdataParameter SelectionMenu.MenuState=0x%02x\n", SelectionMenu.MenuState);
	if(SelectionMenu.SelectItem == NULL&& SelectionMenu.MenuState!= 0x17 && SelectionMenu.MenuState!= 0x0b)
	{
		vision_log_debug("SelectionMenu.SelectItem  %p \n", SelectionMenu.SelectItem);
		if((SystemParaMenu2.Auto_Semi))		BadSemiAutoOpr(&Areainit[4],3,AreaBad_XY_Normal);//状态3：表示左键按下
		return;
	}
	else
	{
		unsigned short ID = 0;
		switch(SelectionMenu.MenuState)
		{
			case 0x25://用户菜单：菜单一
				DisplayUsermenuItemParameter(SelectionMenu.SelectItem,2, 1);
				ID = SelectionMenu.SelectItem->Info.ID;
				if(ID == 0x37 || ID == 0x44 ){			
					Update_Display_UserMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,ID);
				}
				break;
			case 0x26://测试菜单：菜单二
				if(SelectionMenu.SelectItem->Info.ID==0xA2) return;
				DisplayTestmenuItemParameter(SelectionMenu.SelectItem,2 , 1);
				ID = SelectionMenu.SelectItem->Info.ID;	
				if(ID == 0x61||ID == 0x68|| ID == 0x69 ||  ID == 0x6B || ID == 0x102){
					Update_Display_TestMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,ID);
				}
				break;
			case 0x17://十字光标校正
				BPC_Maual_mode_move_cursor(1, 0);	//状态3：表示左键按下
				break;
			case 0x0b://半自动校点界面
				BadSemiAutoOpr(select_Area,3,AreaBad_XY_Normal);//状态3：表示左键按下
				break;
			case 0x03://测温点界面
				Disp_SpotN_Messages(select_Spot,3);//状态3：表示左键按下
				// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
				Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				break;
			case 0x07://测温区域界面
				AreaN_Scale(select_Area,3,AreaBad_XY_Normal); //状态3：表示左键按下
				Area_SimultaneousDisplay(Area_state&0x000F);//显示选中的测温区域的信息
				// Spot_SimultaneousDisplay(Spot_state);//显示选中的测温点的信息
				break;

			// case 0x1f://变倍显示
			// 	GetAndDisplayEnlargeParameter(SelectionMenu.SelectItem,2);
			// 	break;
			// case 0x0f://四边形移动界面
			// 	#ifdef	Electrolytic_copper
			// 	QuadMoveLeft();//状态3：表示左键按下
			// 	#endif
			// 	break;
		}
	}
}


void DisplayDeviceInfoFirstLine(ts_char *user_info, ts_u08 is_active, int line_idx)
{
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = 0;

	pixels_width = fontl_get_fit_pixel_width(user_info);
	//左右预留八个
	pixels_width +=8;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  0;  //预留出25的高度
	
    st_attr.u16_width = pixels_width; //400; 

    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
		st_attr.u16_top =  0+(16*line_idx); 
		st_attr.u16_left = 16; 
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = 0;
		pixels_width = fontl_get_fit_pixel_width(user_info);
		pixels_width +=16;
		st_attr.u16_left = 32;
    	st_attr.u16_top =  0;  //预留出25的高度
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}
	
    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字， 底部透明
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {
		st_attr.u08_visiable = 0;

    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, user_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);

  
}


void DisplayDeviceInfoSecondLine(ts_char *user_info, ts_u08 is_active,int line_idx)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = 0;

	pixels_width = fontl_get_fit_pixel_width(user_info);
	//左右预留八个
	pixels_width +=8;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  76;  //预留出25的高度
	
    st_attr.u16_width = pixels_width; //400; 

    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
		st_attr.u16_top =  80+(16*line_idx);  
		st_attr.u16_left = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = 0;
		pixels_width = fontl_get_fit_pixel_width(user_info);
		pixels_width +=16;
		st_attr.u16_left = 32;
		st_attr.u16_top =  150;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}

    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字， 底部透明
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {
		st_attr.u08_visiable = 0;

    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, user_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
  
}

/**
 * @brief:显示的版本信息
 * @parameter:ver_info:版本信息的字符串
 * @parameter:is_active：是否显示出来
 */
void DisplaySocVersionInfo(ts_char *ver_info, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = fontl_get_fit_pixel_width(ver_info);
	//左右预留八个
	pixels_width +=8;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  24;  //
    st_attr.u16_width = pixels_width; //500
    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
		st_attr.u16_top =  32; 
		st_attr.u16_left = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = fontl_get_fit_pixel_width(ver_info);
		pixels_width +=16;
		st_attr.u16_left = 32;
		st_attr.u16_top =50;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}

    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字，灰底
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {
		
		st_attr.u08_visiable = 0;
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, ver_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
  
	// vision_log_debug(" ver_info %s is_active %d u32_ret %d \n", ver_info, is_active,u32_ret);
}


void DisplayUserInfo(ts_char *user_info, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = 0;

	vision_log_debug("user_info: %s is_active = %d \n", user_info, is_active);
	pixels_width = fontl_get_fit_pixel_width(user_info);
	//左右预留八个
	pixels_width +=8;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  0;  //预留出25的高度
	
    st_attr.u16_width = pixels_width; //400; 

    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
		st_attr.u16_top =  80; //32
		st_attr.u16_left = 16;
	}
    else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = fontl_get_fit_pixel_width(user_info);
		pixels_width +=16;
		st_attr.u16_left = 32;
    	st_attr.u16_top =  0;  //预留出25的高度
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}
    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字， 底部透明
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {
		st_attr.u08_visiable = 0;
		
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, user_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
	// vision_log_debug("leave: user_info %s is_active %d \n", user_info, is_active);
}


void DirectDisplayUserInfo(ts_char *user_info, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  0;  //预留出25的高度
    st_attr.u16_width = 400; 
    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 80;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		st_attr.u16_width =800;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}

    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字， 底部透明
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {

		st_attr.u08_visiable = 0;
	
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, user_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
  
}


void DisplayUserInfoByRes(ts_char *user_info, ts_u08 is_active)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	ts_u32 pixels_width = 0;

	vision_log_debug("user_info: %s is_active = %d \n", user_info, is_active);
	pixels_width = fontl_get_fit_pixel_width(user_info);
	//左右预留八个
	pixels_width +=8;

	// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
    st_attr.u16_left = 32;
    st_attr.u16_top =  150;  //预留出25的高度
	
    st_attr.u16_width = pixels_width; //400; 

    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
	if(res_type==DECTOR_RESOLUTION_384X288){
		st_attr.u16_height = 16;
	}
	else if(res_type==DECTOR_RESOLUTION_1280X1024){
		pixels_width = fontl_get_fit_pixel_width(user_info);
		pixels_width +=16;
		// vision_log_debug(" ver_info %s is_active %d\n", ver_info, is_active);
	    st_attr.u16_left = 32;
	    st_attr.u16_top =  300;  
	    st_attr.u16_width = pixels_width; 
	    st_attr.u16_height = 48; 
	}
    if(is_active)
    {
		st_attr.u08_visiable = 1;
		//白字， 底部透明
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {
		st_attr.u08_visiable = 0;
		
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;


	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_special_info_osd_node, user_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
	
	vision_log_debug("leave: user_info %s is_active %d  st_attr.u16_left %u   st_attr.u16_top %u \n",user_info, is_active,st_attr.u16_left,st_attr.u16_top);
}



void UpdateMenu2StatisInfo(ts_char *statis_info, int is_visible,int line_idx)
{
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	if(line_idx>2){
		return;
	}
	ts_u32 pixels_width = fontl_get_fit_pixel_width(statis_info);
	//左右预留八个
	pixels_width +=8;

    st_attr.u16_left = 32;
    st_attr.u16_top =  48;
    // if(SelectionMenu.MenuState==0x26)//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    // else
    // 	st_attr.u16_width = 144;
    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 100;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
		st_attr.u16_top =  48 + (16*line_idx);
		st_attr.u16_left = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280){
		pixels_width = fontl_get_fit_pixel_width(statis_info);
		pixels_width +=16;
	    st_attr.u16_left = 32;
	    st_attr.u16_top =  100;  
	    st_attr.u16_width = pixels_width; 
	    st_attr.u16_height = 48; 
	}
	//显示
	if(is_visible==1)
    { 	
		st_attr.u08_visiable = 1;
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }
    else
    {	
		
		st_attr.u08_visiable = 0;
		//全黑是隐藏
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_bottom_info_osd_node, statis_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_osd_info_mutex);
}

void DisplayDeviceInfoLine(ts_u08 is_active)
{
	ts_u08	 x, y;
	ts_char first_line_info[80];
	ts_char first_line_info_left[80];
	ts_char second_line_info[80];
	ts_char second_line_info_left[80];

	ts_char  LensSN[11];
	ts_char  ProduceDATA[11];
	ts_char  MerchineSN[15];
	ts_char  DetectorDATA[9];
	ts_char  exDetectorDATA[8];	// 扩展的探测器编号
	ts_char  preDetectorDATA[4];	// 探测器编号的前缀

	ts_u08 osd_type  =  vot_menu_osd_type();


	memset(LensSN, '\0', sizeof(LensSN));
	memset(ProduceDATA, '\0', sizeof(ProduceDATA));
	memset(MerchineSN, '\0', sizeof(MerchineSN));
	memset(DetectorDATA, '\0', sizeof(DetectorDATA));
	memset(exDetectorDATA, '\0', sizeof(exDetectorDATA));
	memset(preDetectorDATA, '\0', sizeof(preDetectorDATA));

	memcpy(LensSN,SystemParaMenu2.LensSN,10);
	LensSN[10]='\0';
	memcpy(ProduceDATA,SystemParaMenu2.ProduceDATA,10);
	ProduceDATA[10]='\0';
	memcpy(MerchineSN,SystemParaMenu2.MerchineSN,14);
	MerchineSN[14]='\0';

	for (x = 0,y = 0; y < 3; y++)
	{
		if (SystemParaMenu2.DetectorDATA[y] != 0x20 && SystemParaMenu2.DetectorDATA[y] != '\0')
		{
			preDetectorDATA[x++] = SystemParaMenu2.DetectorDATA[y];
		}
	}
	preDetectorDATA[x] = '\0';

	memcpy(DetectorDATA,SystemParaMenu2.DetectorDATA+3, 8);
	DetectorDATA[8]='\0';

	if (SystemParaMenu2.exDetectorDATA[0] == DETECTOR_NEW_VERSION_1 && SystemParaMenu2.exDetectorDATA[1] == DETECTOR_NEW_VERSION_2)
	{
		memcpy(exDetectorDATA, SystemParaMenu2.exDetectorDATA + 2, 7);
		exDetectorDATA[7]='\0';
	}

	if(SystemParaMenu2.LOGO_Flag==1){
		if(osd_type==VOT_MENU_OSD_RES_384){
			snprintf(first_line_info,80, "Det_ID:%s%s%s",preDetectorDATA, DetectorDATA, exDetectorDATA);
			snprintf(first_line_info_left,80, "Company:VOT Infrared Technology Co. Ltd%s","");
		}
		else{
			snprintf(first_line_info,80, "Det_ID:%s%s%s  Company:VOT Infrared Technology Co. Ltd",preDetectorDATA, DetectorDATA, exDetectorDATA);
		}
	}
	else{
		snprintf(first_line_info,80, "Det_ID:%s%s%s",preDetectorDATA, DetectorDATA, exDetectorDATA);
		snprintf(first_line_info_left,80, "%s", " ");

	}

	if(osd_type==VOT_MENU_OSD_RES_384){
		snprintf(second_line_info,80, "S/N:%s  LensInfo:%s ",MerchineSN,LensSN);
		snprintf(second_line_info_left,80, "ProductionDate:%s",ProduceDATA);
	}
	else{
		snprintf(second_line_info,80, "S/N:%s  LensInfo:%s  ProductionDate:%s",MerchineSN,LensSN, ProduceDATA);
	}



	// vision_log_debug(" first_line_info %s \n",first_line_info);
	// vision_log_debug(" second_line_info %s \n",second_line_info);
	DisplayDeviceInfoFirstLine(first_line_info,  0,0);

	if(osd_type==VOT_MENU_OSD_RES_384){
		DisplayDeviceInfoFirstLine(first_line_info_left,  0,1);
	}
	DisplayDeviceInfoSecondLine(second_line_info,  0,0);
	if(osd_type==VOT_MENU_OSD_RES_384){
		DisplayDeviceInfoSecondLine(second_line_info_left,  0,1);
	}
	ts_cpl_msleep(500);
	DisplayDeviceInfoFirstLine(first_line_info,  is_active,0);
	if(osd_type==VOT_MENU_OSD_RES_384){
		DisplayDeviceInfoFirstLine(first_line_info_left,  is_active,1);
	}
	DisplayDeviceInfoSecondLine(second_line_info,  is_active,0);
	if(osd_type==VOT_MENU_OSD_RES_384){
		DisplayDeviceInfoSecondLine(second_line_info_left, is_active,1);
	}
}

void DisplayDeviceInfoAuto(void)
{

	ts_u08 visible_flag = 1 - global_sys_ctrl_info.m_device_info_visible_flag ; 

	if(visible_flag==1){
		DisplayDeviceInfoLine(1);
	}
	else{
		DisplayDeviceInfoLine(0);
	}

	global_sys_ctrl_info.m_device_info_visible_flag  = visible_flag;
}


void GetDeviceInfo(ts_u08* uart_msg, ts_u16 msg_len)
{
	int i=0,j=0;
	ts_u16 offset = 8;

	// ts_u32 fit_size = offset+1+14+10+10+11+2+4;

	// vision_log_debug(" msg_len %d  fit_size %d \n", msg_len, fit_size);
	ts_u08* msg = (ts_u08*)uart_msg + offset;

	SystemParaMenu2.LOGO_Flag = msg[0];
	memset(SystemParaMenu2.DetectorDATA, '\0', 12);
	memset(SystemParaMenu2.MerchineSN, '\0', 14);
	memset(SystemParaMenu2.LensSN, '\0', 10);
	memset(SystemParaMenu2.ProduceDATA, '\0', 10);
	memset(SystemParaMenu2.exDetectorDATA, '\0', 10);

	msg+=1;
	for(i=0,j=0;   j<14; j++){
		if(msg[j]!=0x20){
			SystemParaMenu2.MerchineSN[i]=msg[j];
			i++;
		}
	}
	msg+=14;
	for(i=0,j=0; j<10; j++){
		if(msg[j]!=0x20){
			SystemParaMenu2.LensSN[i]=msg[j];
			i++;
		}
	}
	msg+=10;
	for(i=0,j=0; j<10; j++){
		if(msg[j]!=0x20){
			SystemParaMenu2.ProduceDATA[i]=msg[j];
			i++;
		}
	}

	msg+=10;
	for(i=0,j=0; j<11; j++){
		if (j > 2)
		{
			if(msg[j] != 0x20)
			{
				SystemParaMenu2.DetectorDATA[i++]=msg[j];
			}
		}
		else
		{
			SystemParaMenu2.DetectorDATA[i++]=msg[j];			
		}
	}

	vision_log_debug("SystemParaMenu2.DetectorDATA %s \n", SystemParaMenu2.DetectorDATA);
	vision_log_debug("SystemParaMenu2.ProduceDATA %s \n", SystemParaMenu2.ProduceDATA);
	vision_log_debug("SystemParaMenu2.LensSN %s \n", SystemParaMenu2.LensSN);
	vision_log_debug("SystemParaMenu2.MerchineSN %s \n", SystemParaMenu2.MerchineSN);

	if (msg_len == 63)
	{
		msg += 11;
		for(i = 2,j = 0; j < 7; j++)
		{
			if(msg[j] != 0x20)
			{
				SystemParaMenu2.exDetectorDATA[i] = msg[j];
				i++;
			}
		}
		SystemParaMenu2.exDetectorDATA[0] = DETECTOR_NEW_VERSION_1;
		SystemParaMenu2.exDetectorDATA[1] = DETECTOR_NEW_VERSION_2;
		if (strlen(SystemParaMenu2.exDetectorDATA+2) > 0)
		{
			vision_log_debug("SystemParaMenu2.exDetectorDATA %s\n", SystemParaMenu2.exDetectorDATA+2);
		}
	}
	//存储到flash
	ISP_write_sys_menu2_cfg(SystemParaMenu2);
}


/**
 * @description: 清除对应坐标的osd信息
 * @param {ts_char} *special_info：osd信息
 * @param {ts_u16} pos_x：x坐标
 * @param {ts_u16} pos_y：y坐标
 * @return {void}
 */
void ClearSpecialInfo(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y)
{
	// static ts_u16 last_pos_x = 0, last_pos_y = 0;
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;
	ts_osd_attr_s_t st_osd_attr;

	ts_u32 pixels_width = fontl_get_fit_pixel_width(special_info);
	//左右预留八个
	pixels_width +=8;
	ts_tdk_osd_get_attr(shdl_osd_dev, global_sys_ctrl_info.m_special_info_osd_node, &st_osd_attr);
	
    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
  	//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    st_attr.u16_height = 24; //16+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;


	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = fontl_get_fit_pixel_width(special_info);
		pixels_width +=16;
		st_attr.u16_left = pos_x;
    	st_attr.u16_top =  pos_y;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}

	//显示
    {	

		st_attr.u08_visiable = 0;
		// st_attr.u16_width =  st_osd_attr.u16_width - st_attr.u16_left;
		//全黑是隐藏
    	st_attr.u08_font_color = OSD_COLOR_BLACK;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;
    }

    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_special_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
}

/**
 * @description: 更新指定坐标的OSD信息更新，目前用于坏点校准的坐标提示
 * @param {ts_char} *special_info：需要提示信息
 * @param {ts_u16} pos_x：x坐标
 * @param {ts_u16} pos_y：y坐标
 * @param {int} is_visible：是否显示osd信息，目前暂时没有使用该标志
 * @return {void}
 */
void UpdateSpecialInfo(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible)
{
	static ts_u16 last_pos_x = 0, last_pos_y = 0;
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	vision_log_debug("UpdateSpecialInfo enter: pos_x=%d, pos_y=%d, is_visible=%d\n", pos_x, pos_y, is_visible);
	if(is_visible==0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
		return;
	}

	if(last_pos_x!=0 && last_pos_y!=0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
	}

	last_pos_x = pos_x; 
	last_pos_y = pos_y;

	ts_u32 pixels_width = fontl_get_fit_pixel_width(special_info);
	//左右预留八个
	pixels_width +=8;

    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
    // if(SelectionMenu.MenuState==0x26)//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    // else
    // 	st_attr.u16_width = 144;
    st_attr.u16_height = 24; //16（16x16的font）+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = fontl_get_fit_pixel_width(special_info);
		pixels_width +=16;
		st_attr.u16_left = pos_x;
		st_attr.u16_top =  pos_y;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}

	//显示

    { 	
		st_attr.u08_visiable = 1;
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }

	
    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
    ts_lib_fontl_gen_label_info(global_sys_ctrl_info.m_special_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
}


void UpdateSpecialInfoWithNodeBuf(ts_nodebuf_s_t *pst_lbl, ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible)
{
	// static ts_u16 last_pos_x = 0, last_pos_y = 0;
	// ts_u32 u32_ret;
    ts_fontl_lbl_atts_s_t st_attr;

	vision_log_debug("UpdateSpecialInfo enter: pos_x=%d, pos_y=%d, is_visible=%d\n", pos_x, pos_y, is_visible);

	// last_pos_x = pos_x; 
	// last_pos_y = pos_y;

	ts_u32 pixels_width = fontl_get_fit_pixel_width(special_info);
	//左右预留八个
	pixels_width +=8;

    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
    st_attr.u16_width = pixels_width; //480; 

    st_attr.u16_height = 24; //16（16x16的font）+4(上预留)+4（下预留）

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_384){
		st_attr.u16_height = 16;
	}
	else if(osd_type==VOT_MENU_OSD_RES_1280)
	{
		pixels_width = fontl_get_fit_pixel_width(special_info);
		pixels_width +=16;
		st_attr.u16_left = pos_x;
		st_attr.u16_top =  pos_y;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}
	
    { 	
		st_attr.u08_visiable = 1;
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }

	
    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
    ts_lib_fontl_gen_label_info(pst_lbl, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
}


void UpdateSpecialInfoFont16x16(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible)
{
	static ts_u16 last_pos_x = 0, last_pos_y = 0;
	
    ts_fontl_lbl_atts_s_t st_attr;

	// vision_log_debug("UpdateSpecialInfoFont64x64 enter: pos_x=%d, pos_y=%d, is_visible=%d\n", pos_x, pos_y, is_visible);
	if(is_visible==0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
		return;
	}

	if(last_pos_x!=0 && last_pos_y!=0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
	}

	last_pos_x = pos_x; 
	last_pos_y = pos_y;

	ts_u32 pixels_width =  fontl_get_fit_pixel_width(special_info);
	
	//左右预留八个
	pixels_width +=8;
    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;

    st_attr.u16_width = pixels_width; //480; 
    st_attr.u16_height = 24; //16（16x16的font）+4(上预留)+4（下预留）
	
    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	//显示
	// if(is_visible==1)
    { 	
		st_attr.u08_visiable = 1;
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }

	
    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
	ts_lib_fontl_gen_label_info_font16x16(global_sys_ctrl_info.m_special_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr,1);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
}

/**
 * @description: 用于显示64x64字符
 * @param {ts_char} *special_info：需要提示信息
 * @param {ts_u16} pos_x：x坐标
 * @param {ts_u16} pos_y：y坐标
 * @param {int} is_visible：是否显示osd信息，目前暂时没有使用该标志
 * @return {void}
 */
void UpdateSpecialInfoFont64x64(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y, int is_visible)
{
	static ts_u16 last_pos_x = 0, last_pos_y = 0;
	
    ts_fontl_lbl_atts_s_t st_attr;

	// vision_log_debug("UpdateSpecialInfoFont64x64 enter: pos_x=%d, pos_y=%d, is_visible=%d\n", pos_x, pos_y, is_visible);
	if(is_visible==0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
		return;
	}

	if(last_pos_x!=0 && last_pos_y!=0){
		ClearSpecialInfo(special_info,  last_pos_x, last_pos_y);
	}

	last_pos_x = pos_x; 
	last_pos_y = pos_y;

	ts_u32 pixels_width =  fontl_get_fit_pixel_width_font64x64(special_info);
	
	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024) 
	{
		pixels_width +=16;
		st_attr.u16_left = pos_x;
		st_attr.u16_top =  pos_y;
		st_attr.u16_width = pixels_width;
		st_attr.u16_height = 48;////32+8(上预留)+8（下预留）
	}
	else
	{
	// vision_log_debug("pixels_width %u \n", pixels_width);
	//左右预留八个
	pixels_width +=8;

    st_attr.u16_left = pos_x;
    st_attr.u16_top =  pos_y;
    // if(SelectionMenu.MenuState==0x26)//英文菜单属性宽带100，中文菜单宽带144
    st_attr.u16_width = pixels_width; //480; 
    // else
    // 	st_attr.u16_width = 144;
    st_attr.u16_height = 24; //16（16x16的font）+4(上预留)+4（下预留）
	}

    st_attr.u08_visiable = 1;
    st_attr.u08_priority = 50;
    st_attr.s08_alpha = -1;
    st_attr.u08_auto_invert = 0;

	//显示
	// if(is_visible==1)
    { 	
		st_attr.u08_visiable = 1;
    	st_attr.u08_font_color = OSD_COLOR_WHITE;
		st_attr.u08_bg_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;//OSD_COLOR_BLACK;
    	st_attr.u08_rim_color = OSD_COLOR_BLACK;//OSD_COLOR_GRAY;
    }

	
    st_attr.u08_rim_width = 3;
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_special_osd_info_mutex);
	ts_lib_fontl_gen_label_info_font64x64(global_sys_ctrl_info.m_special_info_osd_node, special_info, (ts_fontl_lbl_atts_s_t *)&st_attr);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_special_osd_info_mutex);
}


ts_void init_menu1_parameters(void)
{
	menu1_default_parameters_init();
	Templut_Menu1_LoadDefault();
    ResetDefaultMenuDisplay();
	//参数生效
	reset_usr_isp_cfg();
	userpara_reset();
	// ISP_write_sys_cfg();
	
	UpdatePromptInfoWithTimeMs("user para init completed",2000,VOT_LOG_TYPE_MENU1);
}

ts_void init_menu2_parameters(void)
{
	menu1_default_parameters_init();
	menu2_default_parameters_init();
	Templut_Menu2_LoadDefault();
	reset_usr_isp_cfg();
	userpara_reset();
    testpara_reset();
	// ISP_write_sys_menu2_cfg(SystemParaMenu2);
	UpdatePromptInfoWithTimeMs("menu2 para init completed",2000,VOT_LOG_TYPE_MENU1);
}


ts_void save_menu1_parameters(void)
{
	ISP_write_sys_cfg();
	Write_TempParameter_cfg(&Temp_Parameter);
	UpdatePromptInfoWithTimeMs("save user para completed",2000,VOT_LOG_TYPE_MENU1);
}


ts_void save_menu2_parameters(void)
{
	ISP_write_sys_cfg();
	ISP_write_sys_menu2_cfg(SystemParaMenu2);
	Write_TempParameter_cfg(&Temp_Parameter);
	UpdatePromptInfoWithTimeMs("save menu2 para completed",2000,VOT_LOG_TYPE_MENU1);
}

