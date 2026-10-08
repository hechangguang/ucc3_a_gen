# 
# Makefile -- TDK API 编译脚本
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
mklib ?= y
os_root ?= /opt/tdk/$(soc)/release

# 读入工程配置
sinclude $(cfg_root)/otdk.cfg

# 内部指定
TARGET_NAME ?= ts_otdk

TARGET_SRC := src

TARGET_FILES_y = $(TARGET_SRC)/base/base.c
TARGET_FILES_y += $(TARGET_SRC)/hwlock/hwlock.c

TARGET_FILES_$(CONFIG_OTDK_DRV_UART) += $(TARGET_SRC)/uart/uart.c
TARGET_FILES_$(CONFIG_OTDK_DRV_HBP) += $(TARGET_SRC)/uart/hbp/hbproto.c

TARGET_FILES_$(CONFIG_OTDK_DRV_I2C) += $(TARGET_SRC)/i2c/i2c.c
#TARGET_FILES_$(CONFIG_OTDK_DRV_RTC) += $(TARGET_SRC)/rtc/rtc.c
#TARGET_FILES_$(CONFIG_OTDK_DRV_MBOX) += $(TARGET_SRC)/mbox/mbox.c
TARGET_FILES_$(CONFIG_OTDK_DRV_DMAC) += $(TARGET_SRC)/dmac/dmac.c
TARGET_FILES_$(CONFIG_OTDK_DRV_TIMER) += $(TARGET_SRC)/timer/timer.c
#TARGET_FILES_$(CONFIG_OTDK_DRV_RTC) += $(TARGET_SRC)/rtc/rtc.c
TARGET_FILES_$(CONFIG_OTDK_DRV_GPIO) += $(TARGET_SRC)/gpio/gpio.c
TARGET_FILES_$(CONFIG_OTDK_DRV_PWM) += $(TARGET_SRC)/pwm/pwm.c
#TARGET_FILES_$(CONFIG_OTDK_DRV_RNG) += $(TARGET_SRC)/rng/rng.c
TARGET_FILES_$(CONFIG_OTDK_DRV_WDT) += $(TARGET_SRC)/wdt/wdt.c
TARGET_FILES_$(CONFIG_OTDK_DRV_ADC) += $(TARGET_SRC)/adc/adc.c
TARGET_FILES_y += $(TARGET_SRC)/spi/spi.c
TARGET_FILES_y += $(TARGET_SRC)/spis/spis.c
TARGET_FILES_y += $(TARGET_SRC)/log/log.c


sinclude Makefile.$(ost)

.PHONY: all clean

all: build_$(ost)

