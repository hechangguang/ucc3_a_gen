# 
# Makefile -- tdk 单独模块完成从驱动、 API 、 DEMO 一键编译
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 
# freertos rt-thread
os ?= rt-thread
ost ?= rtos
scpu ?= n
release_version ?= tdk_release
WORK_NOW = $(shell date +%Y%m%d_%H%M%S)

WORK_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

tdk_root := $(WORK_DIR)/$(release_version)
cfg_root := $(WORK_DIR)/project
os_root := $(tdk_root)/kbuild

IMG_DIR := $(tdk_root)/images/
LIB_DIR	:= $(tdk_root)/lib/
KO_DIR	:= $(tdk_root)/drv/

ROOT_LIB_DIR := $(os_root)/lib/
ROOT_KO_DIR	:= $(os_root)/drv/

API_DIR := libs/api_v100/code
TDK_DIR := tlibs/tdk_v100/code

BURN_DIR := $(IMG_DIR)
MKUIMG := $(tdk_root)/kbuild/scripts/mkimage/mkuimg

# 读入编译配置
sinclude project/mk.cfg

# 指定编译器
#TARGET_CC := 'BUILD_CC=/opt/toolchain/riscv/thead/v2.8.0/bin/riscv64-unknown-elf-'
TARGET_CC := 'BUILD_CC=/opt/toolchain/bin/riscv64-unknown-elf-'

export os ost tdk_root os_root cfg_root KO_DIR LIB_DIR WORK_DIR ROOT_LIB_DIR ROOT_KO_DIR


define build_drv
	make -C $(1) $(TARGET_CC) clean;make -C $(1) $(TARGET_CC) V=1
	mkdir -p $(2)
	mv $(1)/*.ko $(2)
endef

define build_adv_drv
	make -C $(1) $(TARGET_CC) $(2) clean;make -C $(1) $(TARGET_CC) $(2) V=1
	mkdir -p $(3)
	mv $(1)/*.ko $(3)
endef

define build_api
	make -C $(API_DIR) $(TARGET_CC) clean;make -C $(API_DIR) $(TARGET_CC) V=1
	mkdir -p $(LIB_DIR)
	mv $(API_DIR)/ts_otdk.a $(LIB_DIR)
endef

define build_tdk
	make -C $(TDK_DIR) $(TARGET_CC) clean;make -C $(TDK_DIR) $(TARGET_CC) V=1
	mkdir -p $(LIB_DIR)
	mv $(TDK_DIR)/ts_tdk_v100.a $(LIB_DIR)
endef

define build_lib
	make -C $(1) $(TARGET_CC) clean;make -C $(1) $(TARGET_CC) V=1
	mkdir -p $(LIB_DIR)
	mv $(1)/*.a $(LIB_DIR)
endef

define build_adv_lib
	make -C $(1) $(TARGET_CC) $(2) clean;make -C $(1) $(TARGET_CC) $(2) V=1
	mkdir -p $(3)
	mv $(1)/*.a $(3)
endef

define build_sample
	$(Q) if [ ! -f $(MKUIMG) ]; then \
		make -C $(tdk_root)/kbuild/scripts/mkimage; \
	fi
	make -C $(1) $(TARGET_CC) clean;make -C $(1) $(TARGET_CC) V=1
	mkdir -p $(2)
	cp $(1)/build/*.uimg $(BURN_DIR)
#	mv $(1)/build/*.uimg $(2)
	cp tdk_release/images/ts_vision.uimg /mnt/hgfs/2025/vmshare/
endef

define build_sample_scpu
	make -C $(1) $(TARGET_CC) clean;make -C $(1) $(TARGET_CC) scpu=y V=1
	mkdir -p $(2)
	mv $(1)/build/*.uimg $(BURN_DIR)
endef

# system
HOOK_DIR ?= system/hook/code
HOOK_CFG ?= T10x_ff_defconfig

TSP_DIR ?= system/tsp
TSP_CFG ?= caribe_ff_defconfig
STSP_CFG ?= caribe_slv_fpga_defconfig

KCPL_DIR ?= system/cpl/kernel

ispcfg:
	$(CC_OPT)objcopy -I binary -O elf32-littleriscv -B riscv $(binf) $(binf).o
	$(CC_OPT)ar r $(binf).a $(binf).o


# for cpl
kcpl:
	make -C $(KCPL_DIR) $(TARGET_CC) clean;make -C $(KCPL_DIR) $(TARGET_CC) V=1
	$(Q) mkdir -p $(KO_DIR)
	$(Q) cp -vf $(KCPL_DIR)/ts_cpl.ko $(KO_DIR)

.PHONY: env
env:
	mkdir -p bak

hook:
	make -C $(HOOK_DIR) $(HOOK_CFG)
	make -C $(HOOK_DIR) $(TARGET_CC) clean;make -C $(HOOK_DIR) $(TARGET_CC) V=1
	rm -f $(IMG_DIR)/hook.img $(IMG_DIR)/hook01.img
	mv $(HOOK_DIR)/build/hook.img $(IMG_DIR)
	cp -f $(IMG_DIR)/hook.img $(IMG_DIR)/hook01.img
#	cp -f $(IMG_DIR)/hook.img $(BURN_DIR)


# otdk
uapi:
	$(call build_api)

dmac:
	$(call build_drv, $(DMAC_DRV), $(KO_DIR))

uart:
	$(call build_drv, $(UART_DRV), $(KO_DIR))

pwm:
	$(call build_drv, $(PWM_DRV), $(KO_DIR))

timer:
	$(call build_drv, $(TIMER_DRV), $(KO_DIR))

gpio:
	$(call build_drv, $(GPIO_DRV), $(KO_DIR))

i2c:
	$(call build_drv, $(I2C_DRV), $(KO_DIR))

sfc:
	$(call build_drv, $(SFC_DRV), $(KO_DIR))

odrv: dmac uart pwm timer gpio i2c sfc uapi

# demo
# vision:
# 	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))

bringup:
	$(call build_sample, $(BRINGUP_DEMO), $(IMG_DIR))

app: vision

# custom:
ball: odrv bringup


vot_all: vot_prepare
	make vision_manual

vot_prepare:
	make uapi
	make vot_config


vision_manual:
	make vision_Manual_prepare
	make vision_def_Det_MANUAL
	make vision_def_RES_MANUAL
	make vision_def_CUSTOMER_TYPE_MANUAL
#	make vision_def_CUSTOMER_TYPE_STANDARD
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_wlp:
	make vision_wlp_prepare
	make vision_def_Det_VOT_WLP
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_uaw:
	make vision_uaw_prepare
	make vision_def_Det_VOT_WLP
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package
	
vision_uaw_lite:
	make vision_uaw_lite_prepare
	make vision_def_Det_VOT_WLP
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_rtd:
	make vision_rtd_prepare
	make vision_def_Det_RTD
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package


vision_UAD3_MANUAL:
	make vision_UAD3_prepare
	make vision_def_Det_MANUAL
	make vision_def_RES_MANUAL
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package


vision_UAD3_Lite_MANUAL:
	make vision_UAD3_Lite_prepare
	make vision_def_Det_MANUAL
	make vision_def_RES_MANUAL
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3_384:
	make vision_UAD3_prepare
	make vision_def_Det_VOT
	make vision_def_384
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3_1280:
	make vision_UAD3_prepare
	make vot_config_1280
	make vision_def_Det_VOT
	make vision_def_1280
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3:
	make vision_UAD3_prepare
	make vision_def_Det_VOT
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3_Lite_384:
	make vision_UAD3_Lite_prepare
	make vision_def_Det_VOT
	make vision_def_384
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3_Lite_1280:
	make vision_UAD3_Lite_prepare
	make vot_config_1280
	make vision_def_Det_VOT
	make vision_def_1280
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package

vision_UAD3_Lite:
	make vision_UAD3_Lite_prepare
	make vision_def_Det_VOT
	$(call build_sample, $(VISION_DEMO), $(IMG_DIR))
	make vision_package


vision_Manual_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="ALL" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="MANUAL" >> vot_config.mk 


vision_UAD3_Lite_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="UAD3" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="UAD3_Lite" >> vot_config.mk 


vision_UAD3_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="UAD3" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="UAD3" >> vot_config.mk 

vision_wlp_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="WLP" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="UAD3" >> vot_config.mk 

vision_uaw_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="WLP" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="UAW" >> vot_config.mk 

vision_uaw_lite_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME="WLP" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE="UAW_Lite" >> vot_config.mk 

vision_rtd_prepare:
	$(Q) if [ ! -f $(WORK_DIR)/isp_config.bin ]; then \
		make vot_config ; \
	fi
	@echo PRODUCTION_NAME:="UAD3" > vot_config.mk 
	@echo HARD_BOARD_PORT_TYPE:="UAD3" >> vot_config.mk 


vision_def_CUSTOMER_TYPE_STANDARD:
	@echo CUSTOMER_TYPE="STANDARD" >> vot_config.mk 

vision_def_CUSTOMER_TYPE_MANUAL:
	@echo CUSTOMER_TYPE="MANUAL" >> vot_config.mk 

vision_def_CUSTOMER_TYPE_DSW:
	@echo CUSTOMER_TYPE="CTYPE_DSW" >> vot_config.mk 

vision_def_Det_MANUAL:
	@echo DETECTOR_TYPE="MANUAL" >> vot_config.mk 

vision_def_Det_VOT:
	@echo DETECTOR_TYPE="VOT" >> vot_config.mk 

vision_def_Det_VOT_WLP:
	@echo DETECTOR_TYPE="VOT_WLP" >> vot_config.mk 

vision_def_Det_RTD:
	@echo DETECTOR_TYPE="RTD" >> vot_config.mk 

vision_def_RES_MANUAL:
	@echo RESOLUTION_TYPE="RES_MANUAL" >> vot_config.mk 


vision_def_384:
	@echo RESOLUTION_TYPE="RES_384_288" >> vot_config.mk 

vision_def_1280:
	@echo RESOLUTION_TYPE="RES_1280_1024" >> vot_config.mk 

vision_def_UVC:
	@echo VIDEO_OUTPUT_TYPE="UVC" >> vot_config.mk 

vision_def_Calc:
	@echo FUNCTION_TYPE="TEMP_CALC_TYPE" >> vot_config.mk 

vision_def_Focus_Auto:
	@echo FOCUS_TYPE="AUTO_TYPE" >> vot_config.mk 

vision_def_Focus_Fixed:
	@echo FOCUS_TYPE="FIXED_TYPE" >> vot_config.mk 

vot_config:
	@python3 generate_vot_config.py  $(WORK_DIR)/isp_config.bin
#	@python3 generate_vot_config_1280.py  $(WORK_DIR)/isp_config.bin
	
vot_config_1280:
	@python3 generate_vot_config_1280.py  $(WORK_DIR)/isp_config.bin

vision_package:
	make -f Makefile_firmware 


