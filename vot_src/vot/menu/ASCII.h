/*
 * ASCII.h
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */

#ifndef SRC_MENU_ASCII_H_
#define SRC_MENU_ASCII_H_

#include <base/ts_type.h>

extern const ts_u08 Font_TAB[][19];
extern const ts_u08 HZ_TAB[][35];
extern const ts_u08 IR_TAB[][35];
extern const ts_u08 Aralm_Icon[][32];

#if (BeiFangGuangXue_BootIcon == 1)
	extern const ts_u08 TurnOnIcon[][64];
#endif

#endif /* SRC_MENU_ASCII_H_ */
