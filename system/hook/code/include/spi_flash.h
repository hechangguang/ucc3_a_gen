/*
 * spi_flash.h -- spi flash 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <base/ts_bitop.h>

#define SFC_TYPE_NOR                        (0)
#define SFC_TYPE_NAND                       (1)
#define SFC_TYPE_UNKNOW                     (2)

/* io mode */
#define SFC_IO_STANDARD                     (1)
#define SFC_IO_DUAL                         (2)
#define SFC_IO_QUAD                         (4)

#define SFC_OPS_READ                        (0)
#define SFC_OPS_WRITE                       (1)

/* --- SPI FLASH COMM --- */
#define SFC_COMM_CMD_READID                 (0x9f)

/* --- SPI_NOR --- */
#define SFC_NOR_CMD_READ_PAGE               (0x03)
#define SFC_NOR_CMD_READ_4BA_PAGE           (0x13)
#define SFC_NOR_CMD_DUAL_READ_PAGE          (0x3B)
#define SFC_NOR_CMD_DUAL_READ_4BA_PAGE      (0x3C)
#define SFC_NOR_CMD_QUAD_READ_PAGE          (0x6B)
#define SFC_NOR_CMD_QUAD_READ_4BA_PAGE      (0x6C)

#define SFC_NOR_CMD_PROGRAM_PAGE            (0x02)
#define SFC_NOR_CMD_PROGRAM_4BA_PAGE        (0x12)

#define SFC_NOR_CMD_ERASE_SECTOR            (0x20)
#define SFC_NOR_CMD_ERASE_4BA_SECTOR        (0x21)
#define SFC_NOR_CMD_ERASE_BLOCK_32KB        (0x52)
#define SFC_NOR_CMD_ERASE_4BA_BLOCK_32KB    (0x5C)
#define SFC_NOR_CMD_ERASE_BLOCK_64KB        (0xD8)
#define SFC_NOR_CMD_ERASE_4BA_BLOCK_64KB    (0xDC)


#define SFC_NOR_CMD_WRITE_ENABLE            (0x06)

#define SFC_NOR_CMD_WRITE_SR1               (0x01)
#define SFC_NOR_CMD_WRITE_SR2               (0x31)
#define SFC_NOR_CMD_WRITE_SR3               (0x11)
#define SFC_NOR_CMD_READ_SR1                (0x05)
#define SFC_NOR_CMD_READ_SR2                (0x35)
#define SFC_NOR_CMD_READ_SR3                (0x15)

/* READ ID */
#define SFC_NOR_READID_CMD_LEN              (1)
#define SFC_NOR_ID_LEN                      (3)


/* --- SPI_NAND --- */
#define SFC_NAND_CMD_PAGE_PROG              (0x02)
#define SFC_NAND_CMD_PROG_EXC               (0x10)
#define SFC_NAND_CMD_PRELOAD                (0x13)
#define SFC_NAND_CMD_READ_PAGE              (0x03)
#define SFC_NAND_CMD_DUAL_READ_PAGE         (0x3B)
#define SFC_NAND_CMD_QUAD_READ_PAGE         (0x6B)

#define SFC_NAND_CMD_BLK_ERASE              (0xD8)

#define SFC_NAND_CMD_READ_FEATURE           (0x0F)
#define SFC_NAND_CMD_SET_FEATURE            (0x1F)

#define SFC_NAND_STATUS_BP                  (0xA0)
#define SFC_NAND_STATUS_FEATURE0            (0xB0)
#define SFC_NAND_STATUS_STATUS0             (0xC0)
#define SFC_NAND_STATUS_FEATURE1            (0xD0)
#define SFC_NAND_STATUS_STATUS1             (0xF0)

#define SFC_NAND_RESET                      (0xFF)

/* READ ID */
#define SFC_NAND_READID_CMD_LEN             (2)
#define SFC_NAND_ID_LEN                     (3)

/* FLASH ADDRESS */
#define SFC_3BYTE_ADDRESS                   (0)
#define SFC_4BYTE_ADDRESS                   (1)


/* SPI-NAND Feature: A0,B0,C0,D0,F0 */

/* C0H: status */
#define SNAND_C0H_OIP       TS_BIT(0)
#define SNAND_C0H_WEL       TS_BIT(1)
#define SNAND_C0H_E_FAIL    TS_BIT(2)
#define SNAND_C0H_P_FAIL    TS_BIT(3)
#define SNAND_C0H_ECCS0     TS_BIT(4)
#define SNAND_C0H_ECCS1     TS_BIT(5)

#define SNAND_C0H_FAIL      (SNAND_C0H_E_FAIL | SNAND_C0H_P_FAIL)
#define SNAND_C0H_ECCS      (SNAND_C0H_ECCS0 | SNAND_C0H_ECCS1)

/* F0H: status */
#define SNAND_F0H_BPS       TS_BIT(3)
#define SNAND_F0H_ECCSE0    TS_BIT(4)
#define SNAND_F0H_ECCSE1    TS_BIT(5)

#define SNAND_F0H_ECCSE_OFFSET  (4)

/* 仅关心 SNAND_C0H_OIP */
#define SNAND_STATUS_BUSY   (0)

/* 仅关心 擦除或者编程结果，包括 OIP */
#define SNAND_STATUS_PROG   (1)

/* 仅关心 读结果，包括 ECC 和 OIP */
#define SNAND_STATUS_READ   (2)


/* 错误码定义, TS_IOC_MAGIC_SFC = 0x52 */
#define TS_SFC_MODID            (0x52)

#define TS_ESNANDNOTCORR        TS_DEFINE_ERRNO(TS_LOG_ERR, TS_SFC_MODID, 0)
#define TS_ESNANDNOTCORR_STR    "ECC not corrected"

#define TS_ESNANDCORR           TS_DEFINE_ERRNO(TS_LOG_ERR, TS_SFC_MODID, 1)
#define TS_ESNANDCORR_STR       "ECC corrected"

#define TS_ESNANDPROG           TS_DEFINE_ERRNO(TS_LOG_ERR, TS_SFC_MODID, 2)
#define TS_ESNANDPROG_STR       "Prog fail"

#define TS_ESNANDBADBLK         TS_DEFINE_ERRNO(TS_LOG_ERR, TS_SFC_MODID, 3)
#define TS_ESNANDBADBLK_STR     "Bad block"


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SPI_FLASH_H__ */
