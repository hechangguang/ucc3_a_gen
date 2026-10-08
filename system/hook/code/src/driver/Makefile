# Makefile -- driver 层 Makefile, 避免过多 Makefile, driver 统一在此管理
# Created by : Top-Silicon Software Team
# Copyright : (C) 2022 Hangzhou JiTai Silicon Co., Ltd. All rights reserved.

DRV_DIR := src/driver/

# clock
CFILES-$(CONFIG_CLKM_T10X) += $(DRV_DIR)/clock/clk_t10x.c

# ddr
CFILES-$(CONFIG_DDR_PRC_V100) += $(DRV_DIR)/ddr/prc_v100.c
CFILES-$(CONFIG_DDR_NULL) += $(DRV_DIR)/ddr/ddr_null.c

# efuse
CFILES-$(CONFIG_EFUSE_V100) += $(DRV_DIR)/efuse/efuse_v100.c
CFILES-$(CONFIG_EFUSE_NULL) += $(DRV_DIR)/efuse/efuse_null.c

# scpu
CFILES-$(CONFIG_SCPU_V100) += $(DRV_DIR)/scpu/scpu_v100.c

# uart
CFILES-$(CONFIG_UART_V100) += $(DRV_DIR)/uart/uart_v100.c

