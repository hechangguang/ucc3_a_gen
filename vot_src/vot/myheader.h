/*
 * myheader.h
 *
 *  Created on: 2023年3月21日
 *      Author: yan
 */

#ifndef SRC_MYHEADER_H_
#define SRC_MYHEADER_H_

#include <base/ts_type.h>
typedef ts_u08 u8;
typedef ts_u16 u16;
typedef ts_u32 u32;
typedef ts_s08 s8;
typedef ts_s16 s16;
typedef ts_s32 s32;
typedef ts_u08 uint8_t;
typedef ts_u16 uint16_t;
//typedef ts_u32 uint32_t;

#include <stdint.h>
#include <stdarg.h>
//#include "bsp.h"
//#include "riscv.h"
//#include "gpio.h"
//#include "clint.h"
//#include "plic.h"
//#include "type.h"
#include <stdarg.h>
//#include "string.h"
//#include "print.h"
#include <stdlib.h>
#include <string.h>
#include "userdef.h"
#include "config.h"
//#include "soc.h"
//#include "i2c.h"
#include "./common/common.h"
#include "./uart/uart_opr.h"
#include "./uart/uart_iray.h"
#include "./uart/uart_common.h"
#include "./uart/uart_vot.h"
#include "./menu/IDE_tree.h"
#include "./menu/votmenu.h"
#include "./menu/ASCII.h"
#include "./menu/display.h"
#include "./menu/usermenu.h"
#include "./menu/testmenu.h"
#include "./menu/menu_save_flash.h"
#include "./common/spi_flash.h"
#include "./common/function.h"
#include "./common/IIC_OPR.h"
#include "./common/TempCal.h"
#include "./common/Zoom_Lens.h"
#include "./common/gm1215.h"
#include "./menu/votmain.h"

//#define SUPPORT_CONTINUOUS_ZOOM   1

#endif /* SRC_MYHEADER_H_ */
