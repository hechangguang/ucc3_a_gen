// /*
//  * spi_flash.c
//  *
//  *  Created on: 2023年4月20日
//  *      Author: yan
//  */
//  #include <base/ts_type.h>
// #include "spi_flash.h"


// void GlobalLock(void)
// {
// 	WriteEnableLatch();
// 	//spi_select(SPI, 0);
//     //Global lock
// 	//spi_write(SPI, 0x7E);
// 	//spi_diselect(SPI, 0);
// }

// void SectorErase(ts_u32 Addr)
// {
// 	WriteEnableLatch();
// 	//spi_select(SPI, 0);
//     //Erase Sector
// 	//spi_write(SPI, 0x20);
// 	//spi_write(SPI, (Addr>>16)&0xFF);
// 	//spi_write(SPI, (Addr>>8)&0xFF);
// 	//spi_write(SPI, Addr&0xFF);
// 	//spi_diselect(SPI, 0);
// 	WaitBusy();
// }
// void BlockErase(ts_u32 Addr)
// {
// 	WriteEnableLatch();
// 	//spi_select(SPI, 0);

// 	//spi_write(SPI, 0xD8);
// 	//spi_write(SPI, (Addr>>16)&0xFF);
// 	//spi_write(SPI, (Addr>>8)&0xFF);
// 	//spi_write(SPI, Addr&0xFF);
// 	//spi_diselect(SPI, 0);
// 	WaitBusy();
// }
// void WriteEnableLatch(void)
// {
// 	//spi_select(SPI, 0);
//     //Write Enable latch
// 	//spi_write(SPI, 0x06);
// 	//spi_diselect(SPI, 0);
// }
// void GlobalUnlock(void)
// {
// 	WriteEnableLatch();
// 	//spi_select(SPI, 0);
//     //Global unlock
// 	//spi_write(SPI, 0x98);
// 	//spi_diselect(SPI, 0);
// }

// void WaitBusy(void)
// {
// #if 0
// 	ts_u08 out;
// 	ts_u16 timeout=0;
// 	float a;
// 	while(1)
// 	{
// 		bsp_uDelay(1*1000);
// 		//spi_select(SPI, 0);
//         //Write Enable
// 		//spi_write(SPI, 0x05);
// 		out = spi_read(SPI);
// 		//spi_diselect(SPI, 0);
// 		if((out & 0x01) ==0x00)
// 			return;
// 		timeout++;
//         //sector erase max=400ms
// 		if(timeout >=400)
// 		{
// 			bsp_print("Time out");
// 			return;
// 		}
// 	}
// #endif
// }

// void spi_init(){
// #if 0
//     //SPI init
//     Spi_Config spiA;
//     spiA.cpol = 1;
//     spiA.cpha = 1;
//     //Assume full duplex (standard SPI)
//     spiA.mode = 0;  //0
//     spiA.clkDivider = 0;
//     spiA.ssSetup = 5;
//     spiA.ssHold = 5;
//     spiA.ssDisable = 5;
//     spi_applyConfig(SPI, &spiA);
//     APB3_REGW_SPI_CS(0);
//     delay_1ms(50);
// #endif
// }

// void read_flash(ts_u32 flash_addr, ts_u08 read_buff[], int size){
// #if 0
// 	int i;
//     for(i=flash_addr;i<flash_addr+size;i++)
//     {
// 	  //spi_select(SPI, 0);
// 	  //spi_write(SPI, 0x03);
// 	  //spi_write(SPI, (i>>16)&0xFF);
// 	  //spi_write(SPI, (i>>8)&0xFF);
// 	  //spi_write(SPI, i&0xFF);
// 	  read_buff[i-flash_addr]= spi_read(SPI);
// 	  //spi_diselect(SPI, 0);
//     }
//  //   bsp_print("spi 0 flash read end !");
// #endif
// }

// void write_flash(ts_u32 flash_addr, ts_u08 write_buff[], int size){
//     int i;
// 	GlobalUnlock();
// 	SectorErase(flash_addr);
// 	WriteEnableLatch();
//     //spi_select(SPI, 0);
// 	//spi_write(SPI, 0x02);
// 	//spi_write(SPI, (flash_addr>>16)&0xFF);
// 	//spi_write(SPI, (flash_addr>>8)&0xFF);
// 	//spi_write(SPI, flash_addr&0xFF);
//     for(i=0;i<size;i++)
//     {
//     	//spi_write(SPI, write_buff[i]&0xFF);
//     }
//     //spi_diselect(SPI, 0);
//     WaitBusy();
//     GlobalLock();
//   //  bsp_print("spi 0 flash write end !");
// }
// void write_flash_page(ts_u32 flash_addr, ts_u08 write_buff[], int size){
//     int i;

// 	WriteEnableLatch();
//     //spi_select(SPI, 0);
// 	//spi_write(SPI, 0x02);
// 	//spi_write(SPI, (flash_addr>>16)&0xFF);
// 	//spi_write(SPI, (flash_addr>>8)&0xFF);
// 	//spi_write(SPI, flash_addr&0xFF);
//     for(i=0;i<size;i++)
//     {
//     	//spi_write(SPI, write_buff[i]&0xFF);
//     }
//     //spi_diselect(SPI, 0);
//     WaitBusy();

// }
// void spi_test(void)
// {
// 	spi_init();
// 	ts_u08 wr_buffer [FLASH_LENGTH] = {0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAa};
// 	ts_u08 wr_buffer2[FLASH_LENGTH] = {0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCa};
// 	ts_u08 rd_buffer[FLASH_LENGTH];
//     write_flash(FLASH_ADDR, wr_buffer2,FLASH_LENGTH);
// 	read_flash(FLASH_ADDR, rd_buffer, FLASH_LENGTH);
// 	int i;
// 	bsp_putString("start read flash data:\r\n");
// 	for(i=0;i<FLASH_LENGTH;i++){
//     	bsp_print("data [");
//     	bsp_printHex(i);
//     	bsp_print("] = ");
//     	bsp_printHex(rd_buffer[i]);
//     	bsp_print("\r\n");
// 	}
// }

// void Read_Flash_NUC(void)
// {
// 	uint8_t  *prt1;
// 	uint8_t  flash_rd_buf256[256];
// 	uint16_t i;
// 	uint16_t j;
// 	uint32_t  N25Q032A_address;

// 	N25Q032A_address = N25Q032A_NUC_addr;
// 	prt1 = (uint8_t *) NUC_BASE_ADDR;

// 	for(j = 0; j < 10; j++)
// 	{
// 		for(i = 0; i < 256; i++)
// 		{
// 			read_flash(N25Q032A_address, flash_rd_buf256, 256);
// 			memcpy(prt1,flash_rd_buf256,256);
// 			N25Q032A_address +=256;
// 			prt1 +=256;
// 		}
// 	}
// //	bsp_print("spi 0 flash read end !");

// }

// void Write_Flash_NUC(void)
// {
// 	uint8_t flash_wr_buf256[256];
// 	uint8_t  *prt1;
// 	uint16_t i;
// 	uint16_t j;
// 	uint32_t  N25Q032A_address;


// 	N25Q032A_address = N25Q032A_NUC_addr;
// 	prt1 = (uint8_t *) NUC_BASE_ADDR;
// 	GlobalUnlock();
// 	for(j = 0; j < 10; j++)
// 	{
// 		BlockErase(N25Q032A_address);
// 		for(i = 0; i < 256; i++)					//y
// 		{
// 			memcpy(flash_wr_buf256,prt1,256);
// 			write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
// 			prt1 += 256;
// 			N25Q032A_address += 256;
// 		}
// 	}
//     GlobalLock();
//  //   bsp_print("spi 0 flash write end !");
// }


// void BNUC_init(void)
// {
//   uint16_t i,j;
//   uint16_t * dout;

//   dout = (uint16_t *) NUC_BASE_ADDR;
// 	for(j=0; j<IR_Y_pixel; j++)
// 	{
// 		for(i=0; i<IR_X_pixel; i++)
// 		{
// 			*dout = 0x4000;
// //		*dout = 0xA2B6;
// 		 dout++;
// 		}
//  }
// /*
// 	dout = (uint16_t *) NUC_BASE_ADDR;
// 	for(j=15; j<16; j++)
// 	{
// 		dout += 640*j;
// 		for(i=50; i<80; i++)
// 		{
// 			bsp_printf("Init NUC_BASE_ADDR(%d,%d) is : 0x%x\n", i,j,*dout);
// 			dout++;
// 		}
// 	}
// */
// }
// void Read_Flash_NUC_init(void)
// {
// 	uint8_t i;
// 	uint8_t	Flash_NUC_had_init;
// 	uint8_t flash_rd_buf[1] = {0x00};
// 	uint32_t  N25Q032A_address;

// 	SetFreeze_img(ON);		//图像冻结
// 	delay_1ms(50);
// 	N25Q032A_address = N25Q032A_NUC_addr;
// 	for(i = 0;i < 8; i++)
// 	{
// 		read_flash(N25Q032A_address, flash_rd_buf, sizeof(flash_rd_buf));
// 		N25Q032A_address += 0x10000;
// 		if(flash_rd_buf[0] != 0xff)
// 		{
// 			Flash_NUC_had_init = 1;
// 			break;
// 		}
// 		else
// 		{
// 			Flash_NUC_had_init = 0;
// 		}
// 	}
// 	if(Flash_NUC_had_init == 1)
// 	{
// 		Read_Flash_NUC();
// 		check1 = 0B00000000;
// 	    check3 = 0B00000000;
// 	}
// 	else
// 	{
// 		BNUC_init();
// 		Write_Flash_NUC();
// 	}
// 	SetFreeze_img(OFF);
// }



// void Read_Flash_OOC(void)
// {
// 	uint8_t  *prt1;
// 	uint8_t  flash_rd_buf256[256];
// 	uint16_t i;
// 	uint16_t j;
// 	uint32_t  N25Q032A_address;

// 	N25Q032A_address = N25Q032A_OOC_addr;
// 	prt1 = (uint8_t *) OOC_BASE_ADDR;

// 	for(j = 0; j < 5; j++)
// 	{
// 		for(i = 0; i < 256; i++)
// 		{
// 			read_flash(N25Q032A_address, flash_rd_buf256, 256);
// 			memcpy(prt1,flash_rd_buf256,256);
// 			N25Q032A_address +=256;
// 			prt1 +=256;
// 		}
// 	}
// //	bsp_print("spi 0 flash read end !");

// }

// void Write_Flash_OOC(void)
// {
// 	uint8_t flash_wr_buf256[256];
// 	uint8_t  *prt1;
// 	uint16_t i;
// 	uint16_t j;
// 	uint32_t  N25Q032A_address;
// //	DisplayBackground(32,Y_START+400,7,BACKGROUND_LAYER,0);
// //	DisPlayNewStr(32,Y_START+400,"Saving",4,FONT_COLOR);

// 	N25Q032A_address = N25Q032A_OOC_addr;
// 	prt1 = (uint8_t *) OOC_BASE_ADDR;
// 	GlobalUnlock();
// 	for(j = 0; j < 5; j++)
// 	{
// 		BlockErase(N25Q032A_address);
// 		for(i = 0; i < 256; i++)					//y
// 		{
// 			memcpy(flash_wr_buf256,prt1,256);
// 			write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
// 			prt1 += 256;
// 			N25Q032A_address += 256;
// 		}
// 	}
//     GlobalLock();

// //	DisplayBackground(32,Y_START+400,7,BACKGROUND_LAYER,0);
// //	DisPlayNewStr(32,Y_START+400,"Done",4,FONT_COLOR);
// //	delay_1s(1);
// //	menu_clear_oneline(32,8*16,Y_START+400);

//  //   bsp_print("spi 0 flash write end !");
// }

// void Read_Flash_OOC_init(void)
// {
// 	uint8_t i;
// 	uint8_t	Flash_OOC_had_init;
// 	uint8_t flash_rd_buf[1] = {0x00};
// 	uint32_t  N25Q032A_address;

// 	N25Q032A_address = N25Q032A_OOC_addr;
// 	for(i = 0;i < 8; i++)
// 	{
// 		read_flash(N25Q032A_address, flash_rd_buf, sizeof(flash_rd_buf));
// 		N25Q032A_address += 0x10000;
// 		if(flash_rd_buf[0] != 0xff)
// 		{
// 			Flash_OOC_had_init = 1;
// 			break;
// 		}
// 		else
// 		{
// 			Flash_OOC_had_init = 0;
// 		}
// 	}

// 	if(Flash_OOC_had_init == 1)
// 	{
// 		Read_Flash_OOC();
// 		//delay_25ms(100);
// 	}
// 	else
// 	{
// 		OOC_init();
// 		Write_Flash_OOC();
// 		//delay_25ms(100);
// 	}
// }


// void Write_Flash_BigDate (ts_u08* addr,ts_u32 Len)
// {
// 	uint16_t i,j,len;
// 	uint8_t * prt1;
// 	uint32_t  N25Q032A_address;
// 	uint8_t flash_wr_buf256[256];

// 	displayReminderInfo(Save);

// //	APB3_REGW_SPI_CS(0);
// //	delay_1ms(50);

// 	N25Q032A_address = N25Q032A_Update_Firmware_addr;
// 	prt1 = addr;
// 	len = Len / (256 * 256);

// 	GlobalUnlock();
// 	for(i = 0; i < len; i++)
// 	{
// 		BlockErase(N25Q032A_address);
// 		for(j = 0; j < 256; j++)
// 		{
// 			memcpy(flash_wr_buf256,prt1,256);
// 			write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
// 			prt1 += 256;
// 			N25Q032A_address += 256;
// 		}
// 	}

// 	BlockErase(N25Q032A_address);

// 	len = Len % (256 * 256);
// 	len = len / 256;

// 	for(i = 0; i < len; i++)
// 	{
// 		memcpy(flash_wr_buf256,prt1,256);
// 		write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
// 		prt1 += 256;
// 		N25Q032A_address += 256;
// 	}

// 	len = Len % (256 * 256);
// 	len = len % 256;

// 	memcpy(flash_wr_buf256,prt1,len);
// 	write_flash_page(N25Q032A_address,flash_wr_buf256, len);

//     GlobalLock();

//     displayReminderInfo(Done);
// }



