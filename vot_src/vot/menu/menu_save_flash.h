#ifndef SRC_MENU_MENU_SAVE_FLASH_H_
#define SRC_MENU_MENU_SAVE_FLASH_H_
#include "../myheader.h"
#include"votmenu.h"
#include"usermenu.h"
#include"testmenu.h"
#include"config.h"

#define N25Q032A_MENU1_ADDRESS 0x4B0000

#define N25Q032A_MENU2_ADDRESS 0x4B1000

#define N25Q032A_COMMON_AREA_ADDRESS 0x4B2000

#define N25Q032A_Quadrilateral_Address 0x4B5000

#define N25Q032A_DetectorInfo_Address 0x4B7000

#define USERMENU_LENGTH 	 		 97
#define COMMON_AREA_LENGTH 	 		 85
#define TESTMENU_LENGTH		 		 174

#if No_Shutter
	#define No_ShutterBaseAddress1 0x0A0000
	#define No_ShutterBaseAddress2 0x140000
	#define No_ShutterBaseAddress3 0x1E0000
	#define No_ShutterBaseAddress4 0x280000
	#define No_ShutterBaseAddress5 0x4E0000
	#define No_ShutterBaseAddress6 0x580000

	#define No_ShutterBaseAddress7 0x650000
	#define No_ShutterBaseAddress8 0x6F0000
	#define No_ShutterBaseAddress9 0x790000
	#define No_ShutterBaseAddress10 0x830000
	#define No_ShutterBaseAddress11 0x8D0000
	#define No_ShutterBaseAddress12 0x970000

	#define No_ShutterBaseAddress13 0xA10000
	#define No_ShutterBaseAddress14 0xAB0000
	#define No_ShutterBaseAddress15 0xB50000
	#define No_ShutterBaseAddress16 0xBF0000
	#define No_ShutterBaseAddress17 0xC90000
	#define No_ShutterBaseAddress18 0xD30000

	#define No_ShutterBaseAddress19 0xDD0000
	#define No_ShutterBaseAddress20 0xE70000
	#define No_ShutterBaseAddress21 0xF10000
//	#define No_ShutterBaseAddress22 0xFB0000
	#define No_ShutterBaseAddress22 0x1190000
	#define No_ShutterBaseAddress23 0x1050000
	#define No_ShutterBaseAddress24 0x10F0000

#endif

#ifdef  TEST_SAVE_Image
	#define No_ShutterBaseAddress1 0x0A0000
	#define No_ShutterBaseAddress2 0x140000
	#define No_ShutterBaseAddress3 0x1E0000
	#define No_ShutterBaseAddress4 0x280000
	#define No_ShutterBaseAddress5 0x4E0000
	#define No_ShutterBaseAddress6 0x580000
	void WriteToFlash_ImageTest(void);
	void ReadFromFlash_ImageTest(void);
#endif


extern ts_u08 ImagePartSave_flag;
extern ts_u08 Menu1ParaInit_flag;
extern unsigned char DetectorID[9];

void Write_CommonAreaParameterToFlash(void);
void CommonAreaParameterInit(void);
void Write_CommonAreaParameterToFPGA(void);
void Read_CommonAreaParameterFromFlash(void);

void Write_TestmenuParameterToFlash(void);
void TestmenuParameterInit(void);
void Write_TestmenuParameterToFPGA(void);
void Read_TestmenuParameterFromFlash(void);

void Write_UsermenuParameterToFlash(void);
void UsermenuParameterInit(void);
void Write_UserParameterToFPGA(void);
void Read_UsermenuParameterFromFlash(void);

void Menu2NewParaSet(void);
void Menu1NewParaSet(void);

ts_u08 PercentageToHex(ts_u08 value);
void ImageAdjustSet(ts_u08 ImageAdjustVal);
void ImageManualParaRefresh(void);
void UpdataImagePara(void);
void TestmenuParRangeLmit(void);
void DetParInit(ts_u08 state);
void FocusParInit(ts_u08 state);
#if(No_Shutter == 1)
	void Write_NoShutterDataToFlash(void);
	void Read_NoShutterDataFromFlash(void);
#endif

void QuadPosInit(void);
void Write_QuadPosToFlash(void);
void Read_QuadPosFromFlash(void);
void Write_SysTimeAndSysShutToFlash(void);
void Read_SysTimeAndSysShutFromFlash(void);
void SysTimeAndSysShutInit(void);

void Write_DetectorInfoToFlash(void);
void Read_DetectorInfoFromFlash(void);
#endif
