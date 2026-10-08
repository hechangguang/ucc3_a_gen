# 
# Makefile -- uart 顶层 Makefile, 通过调用 tsp 或者 linux 的模块编译 makefile
#			  进行编译
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 
# 自动产生环境
MOD_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

# 小写，便于输入
os ?= freertos
ost ?= rtos
os_root ?= /opt/tdk/$(soc)/release

# 读入工程配置
sinclude $(cfg_root)/otdk.cfg

# 内部指定
TARGET_NAME ?= ts_uart_v100

TARGET_SRC := src


TARGET_FILES_$(CONFIG_OTDK_DRV_UART_V100) = $(TARGET_SRC)/uart_v100.c

# 子模块分割
TARGET_FILES_$(CONFIG_OTDK_DRV_UART_TG_V100) += $(TARGET_SRC)/tg/uart.c


sinclude Makefile.$(ost)

.PHONY: all clean

all: build_$(ost)
