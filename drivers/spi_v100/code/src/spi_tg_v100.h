/* 
 * spi_tg_v100.h -- spi_tg_v100 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __SPI_TG_V100_H__
#define __SPI_TG_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

/* register offset */
#define SPI_TG_OFFSET_CTRLR0                (0x0000)
#define SPI_TG_OFFSET_CTRLR1                (0x0004)
#define SPI_TG_OFFSET_ENR                   (0x0008)
#define SPI_TG_OFFSET_SER                   (0x0010)
#define SPI_TG_OFFSET_CLKDIV                (0x0014)
#define SPI_TG_OFFSET_TXFTLR                (0x0018)
#define SPI_TG_OFFSET_RXFTLR                (0x001C)
#define SPI_TG_OFFSET_TXFLR                 (0x0020)
#define SPI_TG_OFFSET_RXFLR                 (0x0024)
#define SPI_TG_OFFSET_SR                    (0x0028)
#define SPI_TG_OFFSET_IMR                   (0x002C)
#define SPI_TG_OFFSET_ISR                   (0x0030)
#define SPI_TG_OFFSET_RISR                  (0x0034)
#define SPI_TG_OFFSET_TXOICR                (0x0038)
#define SPI_TG_OFFSET_RXOICR                (0x003C)
#define SPI_TG_OFFSET_RXUICR                (0x0040)
#define SPI_TG_OFFSET_ICR                   (0x0048)
#define SPI_TG_OFFSET_IDR                   (0x0058)
#define SPI_TG_OFFSET_VERSION               (0x005C)
#define SPI_TG_OFFSET_DR                    (0x0060)
#define SPI_TG_OFFSET_RX_SAMPLE_DLY         (0x00F0)

/* bitmap :  CTRLR0 */
#define SPI_TG_CTRL0_SCPHA              (6)
#define SPI_TG_CTRL0_SCPOL              (7)
#define SPI_TG_CTRL0_TMOD               (8)
#define SPI_TG_CTRL0_SLV_OE             (10)
#define SPI_TG_CTRL0_SRL                (11)
#define SPI_TG_CTRL0_DFS                (16)
#define SPI_TG_CTRL0_FRF                (21)
#define SPI_TG_CTRL0_CS_CTL             (24)
#define SPI_TG_CTRL0_ENDIAN_CTL         (25)

#define SPI_TG_MODE0                    (0)
#define SPI_TG_MODE1                    (1 << SPI_TG_CTRL0_SCPHA)
#define SPI_TG_MODE2                    (1 << SPI_TG_CTRL0_SCPOL)
#define SPI_TG_MODE3                    (SPI_TG_MODE1 | SPI_TG_MODE2)

#define SPI_TG_CTRL0_TMOD_MASK          (0x03 << SPI_TG_CTRL0_TMOD)
#define SPI_TG_CTRL0_TMOD_RXTX          (0 << SPI_TG_CTRL0_TMOD)
#define SPI_TG_CTRL0_TMOD_TX            (1 << SPI_TG_CTRL0_TMOD)
#define SPI_TG_CTRL0_TMOD_RX            (2 << SPI_TG_CTRL0_TMOD)
#define SPI_TG_CTRL0_TMOD_EEPROM        (3 << SPI_TG_CTRL0_TMOD)

#define SPI_TG_CTRL0_DFS_MASK           (0x1f << SPI_TG_CTRL0_DFS)
#define SPI_TG_CTRL0_DFS_8              (7 << SPI_TG_CTRL0_DFS)
#define SPI_TG_CTRL0_DFS_16             (15 << SPI_TG_CTRL0_DFS)
#define SPI_TG_CTRL0_DFS_24             (23 << SPI_TG_CTRL0_DFS)
#define SPI_TG_CTRL0_DFS_32             (31 << SPI_TG_CTRL0_DFS)

#define SPI_TG_CTRL0_CS_KEEP            (1 << SPI_TG_CTRL0_CS_CTL)
#define SPI_TG_CTRL0_CS_ROLL            (1 << SPI_TG_CTRL0_CS_CTL)

#define SPI_TG_CTRL0_ENDIAN_LE          (0 << SPI_TG_CTRL0_ENDIAN_CTL)
#define SPI_TG_CTRL0_ENDIAN_BE          (1 << SPI_TG_CTRL0_ENDIAN_CTL)

/* bitmap : SR */
#define SPI_TG_SR_BUSY                  (0)
#define SPI_TG_SR_TFNF                  (1)
#define SPI_TG_SR_TFE                   (2)
#define SPI_TG_SR_RFNE                  (3)
#define SPI_TG_SR_RFF                   (4)
#define SPI_TG_SR_TXE                   (5)
#define SPI_TG_SR_DCOL                  (6)

/* fifo not full */
#define SPI_TG_SR_TFNF_CHK              (1 << SPI_TG_SR_TFNF)

/* check busy and tfe */
#define SPI_TG_SR_IDLE_MASK             ((1 << SPI_TG_SR_BUSY) | (1 << SPI_TG_SR_TFE))

/*spi idle : busy = 0 and  tfe = 1 */
#define SPI_TG_SR_IS_IDLE               (1 << SPI_TG_SR_TFE)



#define SPI_TG_DEFAULT_LCK_DIV          (8)

#define SPI_TG_DISABLE                  (0)
#define SPI_TG_ENABLE                   (1)

#define SPI_TG_DFS_08                   (8)
#define SPI_TG_DFS_16                   (16)
#define SPI_TG_DFS_32                   (32)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SPI_TG_V100_H__ */
