/*
 * iomux.h -- tdk 芯片管脚复用配置头文件, 此文件为 T10x 定制
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UAPI_IOMUX_H__
#define __UAPI_IOMUX_H__

/* IOMUX and IOCFG_X */
#define TS_IO_MUX_MASK          (0xF)
#define TS_IO_MUX_BITW          (4)

#define TS_IO_ADC_CH_FLAG       (0xad)


#define TS_IO_GET_MUX_GRP(io_name)      (ts_u32)((io_name >> 24) & 0xFF)
#define TS_IO_GET_MUX_IDX(io_name)      (ts_u32)((io_name >> 16) & 0xFF)
#define TS_IO_GET_CFG_GRP(io_name)      (ts_u32)((io_name >> 8) & 0xFF)
#define TS_IO_GET_CFG_IDX(io_name)      (ts_u32)((io_name >> 0) & 0xFF)

/* 设置 IO 复用和配置序号 */
#define TS_IO_SET_MUX_CFG(iomux_grp, iomux_idx, iocfg_grp, iocfg_idx)      ((((ts_u08)iomux_grp & 0xFF) << 24) \
            | (((ts_u08)iomux_idx & 0xFF) << 16) | ((((ts_u08)iocfg_grp & 0xFF) << 8) | ((ts_u08)iocfg_idx & 0xFF)))

#define TS_IO_MUX_MAX_GRP               (18)
#define TS_IO_CFG_MAX_GRP               (5)
#define TS_IO_MUX_MAX_IDX               (28)
#define TS_IO_CFG_MAX_IDX               (31)

/* 芯片 PORT */
#define TS_IO_GPIO57                    TS_IO_SET_MUX_CFG(0, 16, 0, 4)          /* io_mux_reg_0[19:16]  io_cfg_reg_*_0[4]  GPIO57  PWM2      GPIO57 */
#define TS_IO_GPIO57_FUN0                         (0)
#define TS_IO_GPIO57_FUN1_PWM2                    (1)
#define TS_IO_GPIO57_FUN2_X                       (2)
#define TS_IO_GPIO57_FUN3                         (3)

#define TS_IO_I2C0_SDA                  TS_IO_SET_MUX_CFG(0, 20, 0, 5)          /* io_mux_reg_0[23:20]  io_cfg_reg_*_0[5]  I2C0_SDA  AXITRACE_TXD      GPIO0 */
#define TS_IO_I2C0_SDA_FUN0                       (0)
#define TS_IO_I2C0_SDA_FUN1_AXITRACE_TXD          (1)
#define TS_IO_I2C0_SDA_FUN2_X                     (2)
#define TS_IO_I2C0_SDA_FUN3_GPIO0                 (3)

#define TS_IO_I2C0_SCL                  TS_IO_SET_MUX_CFG(0, 24, 0, 6)          /* io_mux_reg_0[27:24]  io_cfg_reg_*_0[6]  I2C0_SCL  AXITRACE_RXD      GPIO1 */
#define TS_IO_I2C0_SCL_FUN0                       (0)
#define TS_IO_I2C0_SCL_FUN1_AXITRACE_RXD          (1)
#define TS_IO_I2C0_SCL_FUN2_X                     (2)
#define TS_IO_I2C0_SCL_FUN3_GPIO1                 (3)

#define TS_IO_I2C1_SDA                  TS_IO_SET_MUX_CFG(0, 28, 0, 7)          /* io_mux_reg_0[31:28]  io_cfg_reg_*_0[7]  I2C1_SDA      I2C_SLV_SDA  GPIO2 */
#define TS_IO_I2C1_SDA_FUN0                       (0)
#define TS_IO_I2C1_SDA_FUN1_X                     (1)
#define TS_IO_I2C1_SDA_FUN2_I2C_SLV_SDA           (2)
#define TS_IO_I2C1_SDA_FUN3_GPIO2                 (3)

#define TS_IO_I2C1_SCL                  TS_IO_SET_MUX_CFG(1, 0, 0, 8)           /* io_mux_reg_1[3:0]  io_cfg_reg_*_0[8]  I2C1_SCL      I2C_SLV_SCL  GPIO3 */
#define TS_IO_I2C1_SCL_FUN0                       (0)
#define TS_IO_I2C1_SCL_FUN1_X                     (1)
#define TS_IO_I2C1_SCL_FUN2_I2C_SLV_SCL           (2)
#define TS_IO_I2C1_SCL_FUN3_GPIO3                 (3)

#define TS_IO_I2C2_SDA                  TS_IO_SET_MUX_CFG(1, 4, 0, 9)           /* io_mux_reg_1[7:4]  io_cfg_reg_*_0[9]  I2C2_SDA  RGB_DAT0  UART4_TXD  GPIO4 */
#define TS_IO_I2C2_SDA_FUN0                       (0)
#define TS_IO_I2C2_SDA_FUN1_RGB_DAT0              (1)
#define TS_IO_I2C2_SDA_FUN2_UART4_TXD             (2)
#define TS_IO_I2C2_SDA_FUN3_GPIO4                 (3)

#define TS_IO_I2C2_SCL                  TS_IO_SET_MUX_CFG(1, 8, 0, 10)          /* io_mux_reg_1[11:8]  io_cfg_reg_*_0[10]  I2C2_SCL  RGB_DAT1  UART4_RXD  GPIO5 */
#define TS_IO_I2C2_SCL_FUN0                       (0)
#define TS_IO_I2C2_SCL_FUN1_RGB_DAT1              (1)
#define TS_IO_I2C2_SCL_FUN2_UART4_RXD             (2)
#define TS_IO_I2C2_SCL_FUN3_GPIO5                 (3)

#define TS_IO_UART0_TXD                 TS_IO_SET_MUX_CFG(1, 12, 0, 11)         /* io_mux_reg_1[15:12]  io_cfg_reg_*_0[11]  UART0_TXD  UART1_TXD      GPIO6 */
#define TS_IO_UART0_TXD_FUN0                      (0)
#define TS_IO_UART0_TXD_FUN1_UART1_TXD            (1)
#define TS_IO_UART0_TXD_FUN2_X                    (2)
#define TS_IO_UART0_TXD_FUN3_GPIO6                (3)

#define TS_IO_UART0_RXD                 TS_IO_SET_MUX_CFG(1, 16, 0, 12)         /* io_mux_reg_1[19:16]  io_cfg_reg_*_0[12]  UART0_RXD  UART1_RXD      GPIO7 */
#define TS_IO_UART0_RXD_FUN0                      (0)
#define TS_IO_UART0_RXD_FUN1_UART1_RXD            (1)
#define TS_IO_UART0_RXD_FUN2_X                    (2)
#define TS_IO_UART0_RXD_FUN3_GPIO7                (3)

#define TS_IO_UART1_TXD                 TS_IO_SET_MUX_CFG(1, 20, 0, 13)         /* io_mux_reg_1[23:20]  io_cfg_reg_*_0[13]  UART1_TXD  PWM2  I2C2_SDA  GPIO8 */
#define TS_IO_UART1_TXD_FUN0                      (0)
#define TS_IO_UART1_TXD_FUN1_PWM2                 (1)
#define TS_IO_UART1_TXD_FUN2_I2C2_SDA             (2)
#define TS_IO_UART1_TXD_FUN3_GPIO8                (3)

#define TS_IO_UART1_RXD                 TS_IO_SET_MUX_CFG(1, 24, 0, 14)         /* io_mux_reg_1[27:24]  io_cfg_reg_*_0[14]  UART1_RXD  PWM3  I2C2_SCL  GPIO9 */
#define TS_IO_UART1_RXD_FUN0                      (0)
#define TS_IO_UART1_RXD_FUN1_PWM3                 (1)
#define TS_IO_UART1_RXD_FUN2_I2C2_SCL             (2)
#define TS_IO_UART1_RXD_FUN3_GPIO9                (3)

#define TS_IO_UART2_TXD                 TS_IO_SET_MUX_CFG(1, 28, 0, 15)         /* io_mux_reg_1[31:28]  io_cfg_reg_*_0[15]  UART2_TXD  PWM0  I2C3_SDA  GPIO10 */
#define TS_IO_UART2_TXD_FUN0                      (0)
#define TS_IO_UART2_TXD_FUN1_PWM0                 (1)
#define TS_IO_UART2_TXD_FUN2_I2C3_SDA             (2)
#define TS_IO_UART2_TXD_FUN3_GPIO10               (3)

#define TS_IO_UART2_RXD                 TS_IO_SET_MUX_CFG(2, 0, 0, 16)          /* io_mux_reg_2[3:0]  io_cfg_reg_*_0[16]  UART2_RXD  PWM1  I2C3_SCL  GPIO11 */
#define TS_IO_UART2_RXD_FUN0                      (0)
#define TS_IO_UART2_RXD_FUN1_PWM1                 (1)
#define TS_IO_UART2_RXD_FUN2_I2C3_SCL             (2)
#define TS_IO_UART2_RXD_FUN3_GPIO11               (3)

#define TS_IO_SW_CLK                    TS_IO_SET_MUX_CFG(2, 4, 0, 17)          /* io_mux_reg_2[7:4]  io_cfg_reg_*_0[17]  SW_CLK  RGB_DAT2  UART3_TXD  GPIO12 */
#define TS_IO_SW_CLK_FUN0                         (0)
#define TS_IO_SW_CLK_FUN1_RGB_DAT2                (1)
#define TS_IO_SW_CLK_FUN2_UART3_TXD               (2)
#define TS_IO_SW_CLK_FUN3_GPIO12                  (3)

#define TS_IO_SW_DIO                    TS_IO_SET_MUX_CFG(2, 8, 0, 18)          /* io_mux_reg_2[11:8]  io_cfg_reg_*_0[18]  SW_DIO  RGB_DAT3  UART3_RXD  GPIO13 */
#define TS_IO_SW_DIO_FUN0                         (0)
#define TS_IO_SW_DIO_FUN1_RGB_DAT3                (1)
#define TS_IO_SW_DIO_FUN2_UART3_RXD               (2)
#define TS_IO_SW_DIO_FUN3_GPIO13                  (3)

#define TS_IO_SPI0_CLK                  TS_IO_SET_MUX_CFG(2, 12, 0, 19)         /* io_mux_reg_2[15:12]  io_cfg_reg_*_0[19]  CDI_SPI_CLK      SPI0_CLK  GPIO14 */
#define TS_IO_SPI0_CLK_FUN0_CDI_SPI_CLK           (0)
#define TS_IO_SPI0_CLK_FUN1_X                     (1)
#define TS_IO_SPI0_CLK_FUN2                       (2)
#define TS_IO_SPI0_CLK_FUN3_GPIO14                (3)

#define TS_IO_SPI0_MOSI                 TS_IO_SET_MUX_CFG(2, 16, 0, 20)         /* io_mux_reg_2[19:16]  io_cfg_reg_*_0[20]  CDI_SPI_MOSI      SPI0_MOSI  GPIO15 */
#define TS_IO_SPI0_MOSI_FUN0_CDI_SPI_MOSI         (0)
#define TS_IO_SPI0_MOSI_FUN1_X                    (1)
#define TS_IO_SPI0_MOSI_FUN2                      (2)
#define TS_IO_SPI0_MOSI_FUN3_GPIO15               (3)

#define TS_IO_SPI0_MISO                 TS_IO_SET_MUX_CFG(2, 20, 0, 21)         /* io_mux_reg_2[23:20]  io_cfg_reg_*_0[21]  CDI_SPI_MISO      SPI0_MISO  GPIO16 */
#define TS_IO_SPI0_MISO_FUN0_CDI_SPI_MISO         (0)
#define TS_IO_SPI0_MISO_FUN1_X                    (1)
#define TS_IO_SPI0_MISO_FUN2                      (2)
#define TS_IO_SPI0_MISO_FUN3_GPIO16               (3)

#define TS_IO_SPI0_WP                   TS_IO_SET_MUX_CFG(2, 24, 0, 22)         /* io_mux_reg_2[27:24]  io_cfg_reg_*_0[22]  CDI_TXD      SPI0_WP  GPIO17 */
#define TS_IO_SPI0_WP_FUN0_CDI_TXD                (0)
#define TS_IO_SPI0_WP_FUN1_X                      (1)
#define TS_IO_SPI0_WP_FUN2                        (2)
#define TS_IO_SPI0_WP_FUN3_GPIO17                 (3)

#define TS_IO_SPI0_HOLD                 TS_IO_SET_MUX_CFG(2, 28, 0, 23)         /* io_mux_reg_2[31:28]  io_cfg_reg_*_0[23]  CDI_RXD      SPI0_HOLD  GPIO18 */
#define TS_IO_SPI0_HOLD_FUN0_CDI_RXD              (0)
#define TS_IO_SPI0_HOLD_FUN1_X                    (1)
#define TS_IO_SPI0_HOLD_FUN2                      (2)
#define TS_IO_SPI0_HOLD_FUN3_GPIO18               (3)

#define TS_IO_SPI0_CSN0                 TS_IO_SET_MUX_CFG(3, 0, 0, 24)          /* io_mux_reg_3[3:0]  io_cfg_reg_*_0[24]  CDI_SPI_CSN      SPI0_CSN0  GPIO19 */
#define TS_IO_SPI0_CSN0_FUN0_CDI_SPI_CSN          (0)
#define TS_IO_SPI0_CSN0_FUN1_X                    (1)
#define TS_IO_SPI0_CSN0_FUN2                      (2)
#define TS_IO_SPI0_CSN0_FUN3_GPIO19               (3)

#define TS_IO_SPI0_CSN1                 TS_IO_SET_MUX_CFG(3, 4, 0, 25)          /* io_mux_reg_3[7:4]  io_cfg_reg_*_0[25]  CDI_IF_SEL      SPI0_CSN1  GPIO20 */
#define TS_IO_SPI0_CSN1_FUN0_CDI_IF_SEL           (0)
#define TS_IO_SPI0_CSN1_FUN1_X                    (1)
#define TS_IO_SPI0_CSN1_FUN2                      (2)
#define TS_IO_SPI0_CSN1_FUN3_GPIO20               (3)

#define TS_IO_SPI1_CLK                  TS_IO_SET_MUX_CFG(3, 8, 0, 26)          /* io_mux_reg_3[11:8]  io_cfg_reg_*_0[26]  SPI1_CLK  SR_OUT_DAT7  SPI_SLV_CLK  GPIO21 */
#define TS_IO_SPI1_CLK_FUN0                       (0)
#define TS_IO_SPI1_CLK_FUN1_SR_OUT_DAT7           (1)
#define TS_IO_SPI1_CLK_FUN2_SPI_SLV_CLK           (2)
#define TS_IO_SPI1_CLK_FUN3_GPIO21                (3)

#define TS_IO_SPI1_MOSI                 TS_IO_SET_MUX_CFG(3, 12, 0, 27)         /* io_mux_reg_3[15:12]  io_cfg_reg_*_0[27]  SPI1_MOSI  SR_OUT_DAT8  SPI_SLV_MOSI  GPIO22 */
#define TS_IO_SPI1_MOSI_FUN0                      (0)
#define TS_IO_SPI1_MOSI_FUN1_SR_OUT_DAT8          (1)
#define TS_IO_SPI1_MOSI_FUN2_SPI_SLV_MOSI         (2)
#define TS_IO_SPI1_MOSI_FUN3_GPIO22               (3)

#define TS_IO_SPI1_MISO                 TS_IO_SET_MUX_CFG(3, 16, 0, 28)         /* io_mux_reg_3[19:16]  io_cfg_reg_*_0[28]  SPI1_MISO  SR_OUT_DAT9  SPI_SLV_MISO  GPIO23 */
#define TS_IO_SPI1_MISO_FUN0                      (0)
#define TS_IO_SPI1_MISO_FUN1_SR_OUT_DAT9          (1)
#define TS_IO_SPI1_MISO_FUN2_SPI_SLV_MISO         (2)
#define TS_IO_SPI1_MISO_FUN3_GPIO23               (3)

#define TS_IO_SPI1_CSN0                 TS_IO_SET_MUX_CFG(3, 20, 0, 29)         /* io_mux_reg_3[23:20]  io_cfg_reg_*_0[29]  SPI1_CSN0  SR_OUT_DAT10  SPI_SLV_CSN  GPIO24 */
#define TS_IO_SPI1_CSN0_FUN0                      (0)
#define TS_IO_SPI1_CSN0_FUN1_SR_OUT_DAT10         (1)
#define TS_IO_SPI1_CSN0_FUN2_SPI_SLV_CSN          (2)
#define TS_IO_SPI1_CSN0_FUN3_GPIO24               (3)

#define TS_IO_SPI1_CSN1                 TS_IO_SET_MUX_CFG(3, 24, 0, 30)         /* io_mux_reg_3[27:24]  io_cfg_reg_*_0[30]  SPI1_CSN1      PWM3  GPIO25 */
#define TS_IO_SPI1_CSN1_FUN0                      (0)
#define TS_IO_SPI1_CSN1_FUN1_X                    (1)
#define TS_IO_SPI1_CSN1_FUN2_PWM3                 (2)
#define TS_IO_SPI1_CSN1_FUN3_GPIO25               (3)

#define TS_IO_ADC_CH                    TS_IO_SET_MUX_CFG(TS_IO_ADC_CH_FLAG, 0xff, TS_IO_ADC_CH_FLAG, 0xff) /* ADC_CH0_3, GPIO26_29, 管脚复用用此选项，统一配置 */
#define TS_IO_ADC_CH_FUN0                       (0)
#define TS_IO_ADC_CH_FUN1_GPIO                  (1)

#define TS_IO_ADC_CH0                   TS_IO_SET_MUX_CFG(TS_IO_ADC_CH_FLAG, 0xff, TS_IO_ADC_CH_FLAG, 0)    /* ADC_CH0 GPIO26, 不能用于管脚复用配置, 仅用于上下拉，施密特等控制 */
#define TS_IO_ADC_CH_FUN0                       (0)
#define TS_IO_ADC_CH_FUN1_GPIO26                (1)

#define TS_IO_ADC_CH1                   TS_IO_SET_MUX_CFG(TS_IO_ADC_CH_FLAG, 0xff, TS_IO_ADC_CH_FLAG, 1)    /* ADC_CH1, GPIO27, 不能用于管脚复用配置, 仅用于上下拉，施密特等控制 */
#define TS_IO_ADC_CH_FUN0                       (0)
#define TS_IO_ADC_CH_FUN1_GPIO27                (1)

#define TS_IO_ADC_CH2                   TS_IO_SET_MUX_CFG(TS_IO_ADC_CH_FLAG, 0xff, TS_IO_ADC_CH_FLAG, 2)    /* ADC_CH2, GPIO28, 不能用于管脚复用配置, 仅用于上下拉，施密特等控制 */
#define TS_IO_ADC_CH_FUN0                       (0)
#define TS_IO_ADC_CH_FUN1_GPIO28                (1)

#define TS_IO_ADC_CH3                   TS_IO_SET_MUX_CFG(TS_IO_ADC_CH_FLAG, 0xff, TS_IO_ADC_CH_FLAG, 3)    /* ADC_CH3, GPIO29, 不能用于管脚复用配置, 仅用于上下拉，施密特等控制 */
#define TS_IO_ADC_CH_FUN0                       (0)
#define TS_IO_ADC_CH_FUN1_GPIO29                (1)

#define TS_IO_PWM0                      TS_IO_SET_MUX_CFG(4, 16, 1, 4)              /* io_mux_reg_4[19:16]  io_cfg_reg_*_1[4]  PWM0          GPIO30 */
#define TS_IO_PWM0_FUN0                           (0)
#define TS_IO_PWM0_FUN1_X                         (1)
#define TS_IO_PWM0_FUN2_X                         (2)
#define TS_IO_PWM0_FUN3_GPIO30                    (3)

#define TS_IO_PWM1                      TS_IO_SET_MUX_CFG(4, 20, 1, 5)          /* io_mux_reg_4[23:20]  io_cfg_reg_*_1[5]  PWM1          GPIO31 */
#define TS_IO_PWM1_FUN0                           (0)
#define TS_IO_PWM1_FUN1_X                         (1)
#define TS_IO_PWM1_FUN2_X                         (2)
#define TS_IO_PWM1_FUN3_GPIO31                    (3)

#define TS_IO_GPIO58                    TS_IO_SET_MUX_CFG(4, 24, 1, 6)          /* io_mux_reg_4[27:24]  io_cfg_reg_*_1[6]  GPIO58  RGB_DAT4  PA0_PWM  PWM2 */
#define TS_IO_GPIO58_FUN0                         (0)
#define TS_IO_GPIO58_FUN1_RGB_DAT4                (1)
#define TS_IO_GPIO58_FUN2_PA0_PWM                 (2)
#define TS_IO_GPIO58_FUN3_PWM2                    (3)

#define TS_IO_GPIO59                    TS_IO_SET_MUX_CFG(4, 28, 1, 7)          /* io_mux_reg_4[31:28]  io_cfg_reg_*_1[7]  GPIO59  RGB_DAT5  PB0_PWM  PWM3 */
#define TS_IO_GPIO59_FUN0                         (0)
#define TS_IO_GPIO59_FUN1_RGB_DAT5                (1)
#define TS_IO_GPIO59_FUN2_PB0_PWM                 (2)
#define TS_IO_GPIO59_FUN3_PWM3                    (3)

#define TS_IO_GPIO60                    TS_IO_SET_MUX_CFG(5, 0, 1, 8)           /* io_mux_reg_5[3:0]  io_cfg_reg_*_1[8]  GPIO60  RGB_DAT6  PA1_PWM     */
#define TS_IO_GPIO60_FUN0                         (0)
#define TS_IO_GPIO60_FUN1_RGB_DAT6                (1)
#define TS_IO_GPIO60_FUN2_PA1_PWM                 (2)
#define TS_IO_GPIO60_FUN3_X                       (3)

#define TS_IO_GPIO61                    TS_IO_SET_MUX_CFG(5, 4, 1, 9)           /* io_mux_reg_5[7:4]  io_cfg_reg_*_1[9]  GPIO61  RGB_DAT7  PB1_PWM     */
#define TS_IO_GPIO61_FUN0                         (0)
#define TS_IO_GPIO61_FUN1_RGB_DAT7                (1)
#define TS_IO_GPIO61_FUN2_PB1_PWM                 (2)
#define TS_IO_GPIO61_FUN3_X                       (3)

#define TS_IO_SR_CLK_IN                 TS_IO_SET_MUX_CFG(5, 8, 1, 10)          /* io_mux_reg_5[11:8]  io_cfg_reg_*_1[10]  SR_CLK_IN          GPIO32 */
#define TS_IO_SR_CLK_IN_FUN0                      (0)
#define TS_IO_SR_CLK_IN_FUN1_X                    (1)
#define TS_IO_SR_CLK_IN_FUN2_X                    (2)
#define TS_IO_SR_CLK_IN_FUN3_GPIO32               (3)

#define TS_IO_SR_CLK_OUT                TS_IO_SET_MUX_CFG(5, 12, 1, 11)         /* io_cfg_reg_*_1[11]  SR_CLK_OUT */
#define TS_IO_SR_IN_DAT0                TS_IO_SET_MUX_CFG(5, 16, 1, 12)         /* io_cfg_reg_*_1[12]  SR_IN_DAT0 */
#define TS_IO_SR_IN_DAT1                TS_IO_SET_MUX_CFG(5, 20, 1, 13)         /* io_cfg_reg_*_1[13]  SR_IN_DAT1 */
#define TS_IO_SR_IN_DAT2                TS_IO_SET_MUX_CFG(5, 24, 1, 14)         /* io_cfg_reg_*_1[14]  SR_IN_DAT2 */
#define TS_IO_SR_IN_DAT3                TS_IO_SET_MUX_CFG(5, 28, 1, 15)         /* io_cfg_reg_*_1[15]  SR_IN_DAT3 */
#define TS_IO_SR_IN_DAT4                TS_IO_SET_MUX_CFG(6, 0, 1, 16)          /* io_cfg_reg_*_1[16]  SR_IN_DAT4 */
#define TS_IO_SR_IN_DAT5                TS_IO_SET_MUX_CFG(6, 4, 1, 17)          /* io_cfg_reg_*_1[17]  SR_IN_DAT5 */
#define TS_IO_SR_IN_DAT6                TS_IO_SET_MUX_CFG(6, 8, 1, 18)          /* io_cfg_reg_*_1[18]  SR_IN_DAT6 */
#define TS_IO_SR_IN_DAT7                TS_IO_SET_MUX_CFG(6, 12, 1, 19)         /* io_cfg_reg_*_1[19]  SR_IN_DAT7 */
#define TS_IO_SR_IN_DAT8                TS_IO_SET_MUX_CFG(6, 16, 1, 20)         /* io_cfg_reg_*_1[20]  SR_IN_DAT8 */
#define TS_IO_SR_IN_DAT9                TS_IO_SET_MUX_CFG(6, 20, 1, 21)         /* io_cfg_reg_*_1[21]  SR_IN_DAT9 */

#define TS_IO_SR_IN_DAT10               TS_IO_SET_MUX_CFG(6, 24, 1, 22)         /* io_mux_reg_6[27:24]  io_cfg_reg_*_1[22]  SR_IN_DAT10          GPIO33 */
#define TS_IO_SR_IN_DAT10_FUN0                    (0)
#define TS_IO_SR_IN_DAT10_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT10_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT10_FUN3_GPIO33             (3)

#define TS_IO_SR_IN_DAT11               TS_IO_SET_MUX_CFG(6, 28, 1, 23)         /* io_mux_reg_6[31:28]  io_cfg_reg_*_1[23]  SR_IN_DAT11          GPIO34 */
#define TS_IO_SR_IN_DAT11_FUN0                    (0)
#define TS_IO_SR_IN_DAT11_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT11_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT11_FUN3_GPIO34             (3)

#define TS_IO_SR_IN_DAT12               TS_IO_SET_MUX_CFG(7, 0, 1, 24)          /* io_mux_reg_7[3:0]  io_cfg_reg_*_1[24]  SR_IN_DAT12          GPIO35 */
#define TS_IO_SR_IN_DAT12_FUN0                    (0)
#define TS_IO_SR_IN_DAT12_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT12_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT12_FUN3_GPIO35             (3)

#define TS_IO_SR_IN_DAT13               TS_IO_SET_MUX_CFG(7, 4, 1, 25)          /* io_mux_reg_7[7:4]  io_cfg_reg_*_1[25]  SR_IN_DAT13          GPIO36 */
#define TS_IO_SR_IN_DAT13_FUN0                    (0)
#define TS_IO_SR_IN_DAT13_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT13_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT13_FUN3_GPIO36             (3)

#define TS_IO_SR_IN_DAT14               TS_IO_SET_MUX_CFG(7, 8, 1, 26)          /* io_mux_reg_7[11:8]  io_cfg_reg_*_1[26]  SR_IN_DAT14          GPIO37 */
#define TS_IO_SR_IN_DAT14_FUN0                    (0)
#define TS_IO_SR_IN_DAT14_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT14_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT14_FUN3_GPIO37             (3)

#define TS_IO_SR_IN_DAT15               TS_IO_SET_MUX_CFG(7, 12, 1, 27)         /* io_mux_reg_7[15:12]  io_cfg_reg_*_1[27]  SR_IN_DAT15          GPIO38 */
#define TS_IO_SR_IN_DAT15_FUN0                    (0)
#define TS_IO_SR_IN_DAT15_FUN1_X                  (1)
#define TS_IO_SR_IN_DAT15_FUN2_X                  (2)
#define TS_IO_SR_IN_DAT15_FUN3_GPIO38             (3)

#define TS_IO_SR_OUT_DAT0               TS_IO_SET_MUX_CFG(7, 16, 1, 28)         /* io_mux_reg_7[19:16]  io_cfg_reg_*_1[28]  SR_OUT_DAT0          GPIO39 */
#define TS_IO_SR_OUT_DAT0_FUN0                    (0)
#define TS_IO_SR_OUT_DAT0_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT0_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT0_FUN3_GPIO39             (3)

#define TS_IO_SR_OUT_DAT1               TS_IO_SET_MUX_CFG(7, 20, 1, 29)         /* io_mux_reg_7[23:20]  io_cfg_reg_*_1[29]  SR_OUT_DAT1          GPIO40 */
#define TS_IO_SR_OUT_DAT1_FUN0                    (0)
#define TS_IO_SR_OUT_DAT1_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT1_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT1_FUN3_GPIO40             (3)

#define TS_IO_SR_OUT_DAT2               TS_IO_SET_MUX_CFG(7, 24, 1, 30)         /* io_mux_reg_7[27:24]  io_cfg_reg_*_1[30]  SR_OUT_DAT2          GPIO41 */
#define TS_IO_SR_OUT_DAT2_FUN0                    (0)
#define TS_IO_SR_OUT_DAT2_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT2_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT2_FUN3_GPIO41             (3)

#define TS_IO_SR_OUT_DAT3               TS_IO_SET_MUX_CFG(7, 28, 1, 31)         /* io_mux_reg_7[31:28]  io_cfg_reg_*_1[31]  SR_OUT_DAT3          GPIO42 */
#define TS_IO_SR_OUT_DAT3_FUN0                    (0)
#define TS_IO_SR_OUT_DAT3_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT3_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT3_FUN3_GPIO42             (3)

#define TS_IO_SR_OUT_DAT4               TS_IO_SET_MUX_CFG(8, 0, 2, 0)           /* io_mux_reg_8[3:0]  io_cfg_reg_*_2[0]  SR_OUT_DAT4          GPIO43 */
#define TS_IO_SR_OUT_DAT4_FUN0                    (0)
#define TS_IO_SR_OUT_DAT4_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT4_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT4_FUN3_GPIO43             (3)

#define TS_IO_SR_OUT_DAT5               TS_IO_SET_MUX_CFG(8, 4, 2, 1)           /* io_mux_reg_8[7:4]  io_cfg_reg_*_2[1]  SR_OUT_DAT5          GPIO44 */
#define TS_IO_SR_OUT_DAT5_FUN0                    (0)
#define TS_IO_SR_OUT_DAT5_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT5_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT5_FUN3_GPIO44             (3)

#define TS_IO_SR_OUT_DAT6               TS_IO_SET_MUX_CFG(8, 8, 2, 2)           /* io_mux_reg_8[11:8]  io_cfg_reg_*_2[2]  SR_OUT_DAT6          GPIO45 */
#define TS_IO_SR_OUT_DAT6_FUN0                    (0)
#define TS_IO_SR_OUT_DAT6_FUN1_X                  (1)
#define TS_IO_SR_OUT_DAT6_FUN2_X                  (2)
#define TS_IO_SR_OUT_DAT6_FUN3_GPIO45             (3)

#define TS_IO_SR_SEQ_SYNC               TS_IO_SET_MUX_CFG(8, 12, 2, 3)          /* io_mux_reg_8[15:12]  io_cfg_reg_*_2[3]  SR_SEQ_SYNC_I  SR_SEQ_SYNC_O  PWM2  GPIO46 */
#define TS_IO_SR_SEQ_SYNC_FUN0_SR_SEQ_SYNC_I      (0)
#define TS_IO_SR_SEQ_SYNC_FUN1_SR_SEQ_SYNC_O      (1)
#define TS_IO_SR_SEQ_SYNC_FUN2_PWM2               (2)
#define TS_IO_SR_SEQ_SYNC_FUN3_GPIO46             (3)

#define TS_IO_BT1120_CLK                TS_IO_SET_MUX_CFG(8, 16, 2, 4)          /* io_mux_reg_8[19:16]  io_cfg_reg_*_2[4]  BT1120_CLK  RGB_CLK      GPIO47 */
#define TS_IO_BT1120_CLK_FUN0                     (0)
#define TS_IO_BT1120_CLK_FUN1_RGB_CLK             (1)
#define TS_IO_BT1120_CLK_FUN2_X                   (2)
#define TS_IO_BT1120_CLK_FUN3_GPIO47              (3)

#define TS_IO_BT1120_DAT0               TS_IO_SET_MUX_CFG(8, 20, 2, 5)          /* io_mux_reg_8[23:20]  io_cfg_reg_*_2[5]  BT1120_DAT0  RGB_DAT8      GPIO48 */
#define TS_IO_BT1120_DAT0_FUN0                    (0)
#define TS_IO_BT1120_DAT0_FUN1_RGB_DAT8           (1)
#define TS_IO_BT1120_DAT0_FUN2_X                  (2)
#define TS_IO_BT1120_DAT0_FUN3_GPIO48             (3)

#define TS_IO_BT1120_DAT1               TS_IO_SET_MUX_CFG(8, 24, 2, 6)          /* io_mux_reg_8[27:24]  io_cfg_reg_*_2[6]  BT1120_DAT1  RGB_DAT9      GPIO49 */
#define TS_IO_BT1120_DAT1_FUN0                    (0)
#define TS_IO_BT1120_DAT1_FUN1_RGB_DAT9           (1)
#define TS_IO_BT1120_DAT1_FUN2_X                  (2)
#define TS_IO_BT1120_DAT1_FUN3_GPIO49             (3)

#define TS_IO_BT1120_DAT2               TS_IO_SET_MUX_CFG(8, 28, 2, 7)          /* io_mux_reg_8[31:28]  io_cfg_reg_*_2[7]  BT1120_DAT2  RGB_DAT10      GPIO50 */
#define TS_IO_BT1120_DAT2_FUN0                    (0)
#define TS_IO_BT1120_DAT2_FUN1_RGB_DAT10          (1)
#define TS_IO_BT1120_DAT2_FUN2_X                  (2)
#define TS_IO_BT1120_DAT2_FUN3_GPIO50             (3)

#define TS_IO_BT1120_DAT3               TS_IO_SET_MUX_CFG(9, 0, 2, 8)           /* io_mux_reg_9[3:0]  io_cfg_reg_*_2[8]  BT1120_DAT3  RGB_DAT11      GPIO51 */
#define TS_IO_BT1120_DAT3_FUN0                    (0)
#define TS_IO_BT1120_DAT3_FUN1_RGB_DAT11          (1)
#define TS_IO_BT1120_DAT3_FUN2_X                  (2)
#define TS_IO_BT1120_DAT3_FUN3_GPIO51             (3)

#define TS_IO_BT1120_DAT4               TS_IO_SET_MUX_CFG(9, 4, 2, 9)           /* io_mux_reg_9[7:4]  io_cfg_reg_*_2[9]  BT1120_DAT4  RGB_DAT12      GPIO52 */
#define TS_IO_BT1120_DAT4_FUN0                    (0)
#define TS_IO_BT1120_DAT4_FUN1_RGB_DAT12          (1)
#define TS_IO_BT1120_DAT4_FUN2_X                  (2)
#define TS_IO_BT1120_DAT4_FUN3_GPIO52             (3)

#define TS_IO_BT1120_DAT5               TS_IO_SET_MUX_CFG(9, 8, 2, 10)          /* io_mux_reg_9[11:8]  io_cfg_reg_*_2[10]  BT1120_DAT5  RGB_DAT13      GPIO53 */
#define TS_IO_BT1120_DAT5_FUN0                    (0)
#define TS_IO_BT1120_DAT5_FUN1_RGB_DAT13          (1)
#define TS_IO_BT1120_DAT5_FUN2_X                  (2)
#define TS_IO_BT1120_DAT5_FUN3_GPIO53             (3)

#define TS_IO_BT1120_DAT6               TS_IO_SET_MUX_CFG(9, 12, 2, 11)         /* io_mux_reg_9[15:12]  io_cfg_reg_*_2[11]  BT1120_DAT6  RGB_DAT14      GPIO54 */
#define TS_IO_BT1120_DAT6_FUN0                    (0)
#define TS_IO_BT1120_DAT6_FUN1_RGB_DAT14          (1)
#define TS_IO_BT1120_DAT6_FUN2_X                  (2)
#define TS_IO_BT1120_DAT6_FUN3_GPIO54             (3)

#define TS_IO_BT1120_DAT7               TS_IO_SET_MUX_CFG(9, 16, 2, 12)         /* io_mux_reg_9[19:16]  io_cfg_reg_*_2[12]  BT1120_DAT7  RGB_DAT15      GPIO55 */
#define TS_IO_BT1120_DAT7_FUN0                    (0)
#define TS_IO_BT1120_DAT7_FUN1_RGB_DAT15          (1)
#define TS_IO_BT1120_DAT7_FUN2_X                  (2)
#define TS_IO_BT1120_DAT7_FUN3_GPIO55             (3)

#define TS_IO_BT1120_DAT8               TS_IO_SET_MUX_CFG(9, 20, 2, 13)         /* io_mux_reg_9[23:20]  io_cfg_reg_*_2[13]  BT1120_DAT8  RGB_DAT16      GPIO56 */
#define TS_IO_BT1120_DAT8_FUN0                    (0)
#define TS_IO_BT1120_DAT8_FUN1_RGB_DAT16          (1)
#define TS_IO_BT1120_DAT8_FUN2_X                  (2)
#define TS_IO_BT1120_DAT8_FUN3_GPIO56             (3)

#define TS_IO_MIPI                      TS_IO_SET_MUX_CFG(0x4D, 0xff, 0x4D, 0xff)   /* MIPI BT1120_DAT9_BT1120_VLD RGB_DAT17_RGB_VLD */  
#define TS_IO_MIPI_FUN0                         (0)
#define TS_IO_MIPI_FUN1_BT1120_DAT9_VLD         (1)
#define TS_IO_MIPI_FUN2_RGB_DAT17_VLD           (2)
#define TS_IO_MIPI_FUN3_X                       (3)

#define TS_IO_USB_DP                    TS_IO_SET_MUX_CFG(0xff, 0xff, 2, 24)    /* io_cfg_reg_*_2[24]  USB_DP */       
#define TS_IO_USB_DM                    TS_IO_SET_MUX_CFG(0xff, 0xff, 2, 25)    /* io_cfg_reg_*_2[25]  USB_DM */    
#define TS_IO_CVBS_OUT_P                TS_IO_SET_MUX_CFG(0xff, 0xff, 2, 26)    /*  io_cfg_reg_*_2[26]  CVBS_OUT_P */
#define TS_IO_CVBS_OUT_N                TS_IO_SET_MUX_CFG(0xff, 0xff, 2, 27)    /*  io_cfg_reg_*_2[27]  CVBS_OUT_N */
#define TS_IO_FLASH_CLK                 TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 8)     /*  io_cfg_reg_*_4[8]  FLASH_CLK */
#define TS_IO_FLASH_MOSI                TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 9)     /*  io_cfg_reg_*_4[9]  FLASH_MOSI */
#define TS_IO_FLASH_MISO                TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 10)    /*  io_cfg_reg_*_4[10]  FLASH_MISO */
#define TS_IO_FLASH_WP                  TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 11)    /*  io_cfg_reg_*_4[11]  FLASH_WP */
#define TS_IO_FLASH_HOLD                TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 12)    /*  io_cfg_reg_*_4[12]  FLASH_HOLD */
#define TS_IO_FLASH_CSN                 TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 13)    /*  io_cfg_reg_*_4[13]  FLASH_CSN */
#define TS_IO_FLASH1_CLK                TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 17)    /*  io_cfg_reg_*_4[17]  FLASH1_CLK */
#define TS_IO_FLASH1_MOSI               TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 18)    /*  io_cfg_reg_*_4[18]  FLASH1_MOSI */
#define TS_IO_FLASH1_MISO               TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 19)    /*  io_cfg_reg_*_4[19]  FLASH1_MISO */
#define TS_IO_FLASH1_WP                 TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 20)    /*  io_cfg_reg_*_4[20]  FLASH1_WP */
#define TS_IO_FLASH1_HOLD               TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 21)    /*  io_cfg_reg_*_4[21]  FLASH1_HOLD */
#define TS_IO_FLASH1_CSN                TS_IO_SET_MUX_CFG(0xff, 0xff, 4, 22)    /*  io_cfg_reg_*_4[22]  FLASH1_CSN */

#endif /* __UAPI_IOMUX_H__ */
