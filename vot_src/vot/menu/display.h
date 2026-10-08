/*
 * display.h
 *
 *  Created on: 2024年3月12日
 *      Author: Administrator
 */

#ifndef SRC_MENU_DISPLAY_H_
#define SRC_MENU_DISPLAY_H_

#include "../myheader.h"
// #include <base/ts_type.h>

// #include "ASCII.h"
// #include "usermenu.h"
// #include "testmenu.h"
// #include "menu/votmenu.h"

/* forward declarations */
struct Spot;
struct Area;

extern ts_u08 SOC_WR_Hyram_Flag;
extern ts_mdev_t shdl_osd_dev;

void SOC_ReadAndWrite_Hyram(ts_u08 Switch);
ts_u16 FindCharactersID(ts_u08 identify_1, ts_u08 identify_2,ts_u08 identify_3);
void DisPlayNewStr(ts_u32 X_LINE,ts_u32 Y_LINE,const ts_char *pData,ts_u08 layer,ts_u08 color);
void DisPlaySize16X8(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 layer,ts_u08 color);
void DisPlayNewFont(ts_u08 *pData,ts_u08 offset);
void ClearFont(ts_u08 xstart,ts_u08 xend);
void ClearSize16X22(ts_u16 X_start,ts_u16 X_offset,ts_u16 Y_start);
void ClearSize16X16(ts_u16 X_start,ts_u16 X_offset,ts_u16 Y_start);
void DisplayMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color);
void DisplaySelectionMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color);
void ClearSelectionMenuFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 layer,ts_u08 color);
void DisplayBackgroundColor(ts_u16 x, ts_u16 y, ts_u16 len, ts_u08 layer,ts_u08 color);
ts_u08 GetStringSize(const  char *Str);
void DisplayBottomPromptInformation(void);
void DisPlayIcon(ts_u16 Sport_id,ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 visible);
void ClearSportsInfo(void);
void ClearAreasInfo(void);
void Display_HyperRAMHexData(ts_u16 pData,ts_u08 start);
void Display_HyperRAMDataUShort(ts_u16 pData,ts_u08 start);
void Display_HyperRAMDataUint(ts_u08 pData,ts_u08 start);
void DisplayTriangleFrame(ts_u16 x, ts_u16 y, ts_u08 len, ts_u08 Type,ts_u08 color);
void Display_MatrixIcon(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 size,ts_u08 color);
void Display_MatrixIconX2(ts_u08 buf[],ts_u32 X_LINE,ts_u32 Y_LINE,ts_u08 size,ts_u08 color);
void DisplayCenterSpot_Init(void);
void Display_CenterSpot(ts_u08 Spot_way);

ts_void templutCursor_Set_position(ts_nodebuf_s_t *pst_osd_node, ts_u16 pos_x, ts_u16 pox_y);
void Clear_templut_osdNode_Info(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y);
void Update_templut_osdNode_Info(ts_char *special_info, ts_u16 pos_x, ts_u16 pos_y);
void Refresh_templutSpot_Captions(ts_char *special_Mess, struct Spot* vSelect_Spots, ts_u08 visible);
void Refresh_templut_AreaCaptions(ts_char *special_Mess, struct Area* vSelect_Areas, ts_u08 visible);
#endif /* SRC_MENU_DISPLAY_H_ */
