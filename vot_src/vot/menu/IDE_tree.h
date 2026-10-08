/*
 * IDE_tree.h
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

#ifndef SRC_MENU_IDE_TREE_H_
#define SRC_MENU_IDE_TREE_H_

//#include "../myheader.h"
#include "ASCII.h"
#include "votmenu.h"

#define	PaleTAB		//(*((volatile ts_u32 *) (PALETTE_BASE_ADDR)))
//#define BadAreaPos(ID)		//确定哪一个测温区域
#define BadAreaPos(ID) 0

#define ReadBadADMax()  	1//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MaxAD)
#define ReadBadADMin()  	2//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MinAD)
#define ReadBadADMax_X()	3//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MaxAD_X)
#define ReadBadADMax_Y()	4//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MaxAD_Y)
#define ReadBadADMin_X()	5//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MinAD_X)
#define ReadBadADMin_Y()	6//read_ts_u16(IO_APB_SLAVE_0_INPUT_Area5_MinAD_Y)

#define   IDC_ROOT		1

#define  AreaBad_XY_Normal     1
#define  AreaBad_XY_Expand     2
#define  AreaBad_XY_Shrink     3
#define  AreaBad_X_Expand      4
#define  AreaBad_X_Shrink      5
#define  AreaBad_Y_Expand      6
#define  AreaBad_Y_Shrink      7


#define SoptOffset 8       //8
#define AreaOffset 32

typedef enum croOpr{
	badCroUp,
	badCroDown,
	badCroLeft,
	badCroRight,
	badCroOprSum
}CroOpr;

typedef struct  node
{
	struct node  *root;
	struct node  *left;
	struct node  *right;
	struct node  *son;
	ts_u08 idc;
	struct node  *up;
	struct node  *down;
	void    (*CurrentOperate)(void);
}menu_node;

typedef struct IDC_Def_Struct
{
	struct node  * p_node;
	struct node  * p_oldnode;
	ts_u08 bnew_node;
	ts_u08 old_idc;
}IDC_Def;

typedef struct BadMes
{
	ts_u16 Area_Lx;
	ts_u16 Area_Ly;
	ts_u16 Area_Rx;
	ts_u16 Area_Ry;

	ts_u16 Spot_x;
	ts_u16 Spot_y;

	ts_u16 MaxAD_X;
	ts_u16 MinAD_X;

	ts_u16 MaxAD_Y;
	ts_u16 MinAD_Y;

}BAD_Def;

typedef struct BadCro{
	ts_u16 Spot_x;
	ts_u16 Spot_y;
}badCro_Def;

extern struct BadCro badCro;
extern BAD_Def BadMes;
extern menu_node NodeIDC_ROOT;
void DisPlay_BAD_Opr(BAD_Def *pos,ts_u08 layer,ts_u08 color);
void load_palette(ts_u08 slt);
void DisPlaySpot(ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 layer,ts_u08 color,ts_u08 type);

void Area_LxLy(ts_u16 x,ts_u16 y);
void Area_RxRy(ts_u16 x,ts_u16 y);
void Area_XY_Normal(ts_u08 state);
void Area_XY_Expand(void);
void Area_XY_Shrink(void);
void Area_X_Expand(void);		//Area拉长
void Area_X_Shrink(void);		//Area变短
void Area_Y_Expand(void);		//Area拉高
void Area_Y_Shrink(void);		//Area变矮

void DisPlay_BAD_Cro(struct BadCro *pos,ts_u08 layer,ts_u08 color);
ts_void BadAreaXY_Move(ts_u16 *LxPos,ts_u08 Lx_Status,ts_u16 *LyPos,ts_u08 Ly_Status,ts_u16 *RxPos,ts_u08 Rx_Status,ts_u16 *RyPos,ts_u08 Ry_Status); 

#endif /* SRC_MENU_IDE_TREE_H_ */
