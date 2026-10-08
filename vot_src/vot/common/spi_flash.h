/*
 * spi_flash.h
 *
 *  Created on: 2023年4月20日
 *      Author: yan
 */

#ifndef SRC_COMMON_SPI_FLASH_H_
#define SRC_COMMON_SPI_FLASH_H_

#include "../myheader.h"

#define FLASH_ADDR    0x3A0000
#define FLASH_LENGTH  10

/*NUC write & read*/
#define N25Q032A_NUC_addr        0x3A0000
#define N25Q032A_NUC_addr_P1     0x3A0000
#define N25Q032A_NUC_addr_P2     0x3B0000
#define N25Q032A_NUC_addr_P3     0x3C0000


/*OOC write & read*/
#define N25Q032A_OOC_addr        0x450000

/*Update_Firmware write & read*/
#define N25Q032A_Update_Firmware_addr 0x000000

void GlobalLock(void);
void SectorErase(u32 Addr);
void WriteEnableLatch(void);
void GlobalUnlock(void);
void WaitBusy(void);
// void spi_init();
void BlockErase(u32 Addr);
void read_flash(u32 flash_addr, u8 read_buff[], int size);
void write_flash(u32 flash_addr, u8 write_buff[], int size);
void write_flash_page(u32 flash_addr, u8 write_buff[], int size);

void spi_test(void);

void BNUC_init(void);
void Read_Flash_NUC(void);
void Write_Flash_NUC(void);
void Read_Flash_NUC_init(void);


void Read_Flash_OOC(void);
void Write_Flash_OOC(void);
void Read_Flash_OOC_init(void);


void Write_Flash_BigDate (u8* addr,u32 Len);

#endif /* SRC_COMMON_SPI_FLASH_H_ */
