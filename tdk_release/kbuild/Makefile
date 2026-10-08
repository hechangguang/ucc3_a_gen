# 
# Makefile -- tsp 顶层 Makefile
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 
VERSION = 2024_12
PRJ_NAME = tsp
RELEASE_VERSION = tdk_release

WORK_NOW = $(shell date +%Y%m%d_%H%M%S)

# 目录定义
WORK_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
TDK_DIR ?= $(WORK_DIR)/../../$(RELEASE_VERSION)
SCRIPTS_DIR = $(WORK_DIR)/scripts/
MENU_CFG_DIR = $(SCRIPTS_DIR)/kconfig/
OBJ_DIR = build/
SRC_DIR = src/
INC_DIR = $(WORK_DIR)/include/
INSTALL_DIR ?= $(TDK_DIR)/os

# Kconfig 配置
KCONFIG_AUTOHEADER := $(INC_DIR)/config/autoconf.h
KCONFIG_AUTOCONFIG := $(INC_DIR)/config/auto.conf
KCONFIG_TOPCONFIG := $(WORK_DIR)/Kconfig

# for gen lds
TSP_LDS_FILE := $(OBJ_DIR)tdk.lds
TSP_LDS_S_FILE := $(SCRIPTS_DIR)tdk.lds.S
TSP_LDS_INCLUDE := $(KCONFIG_AUTOHEADER)

# for depend
DEPEND_FILE := $(OBJ_DIR)mkdep
FIXDEP		= $(SCRIPTS_DIR)/mkdep/fixdep

# for dtc
DTC_EXE	:= $(SCRIPTS_DIR)dtc/dtc

# for mkuimg
MKUIMG_EXE := $(SCRIPTS_DIR)mkimage/mkuimg

# config file
CONFIG_FILE := $(WORK_DIR)/.config

export KCONFIG_AUTOHEADER KCONFIG_AUTOCONFIG

# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

# import config
sinclude $(CONFIG_FILE)

PRE_CONFIG_CC := $(shell echo $(CONFIG_TOOLCHAIN) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
BUILD_CC ?= $(PRE_CONFIG_CC)
BUILD_CPU_CFLAGS := $(shell echo $(CONFIG_CPU_CFLAGS) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
BUILD_USER_CFLAGS := $(shell echo $(CONFIG_USER_CFLAGS) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
BUILD_OPTIMIZE := $(shell echo $(CONFIG_OPTIMIZE) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')

BUILD_ARCH := $(shell echo $(CONFIG_ARCH_NAME) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
BUILD_SOC := $(shell echo $(CONFIG_SOC_NAME) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')


FIXNAME		= $(PRJ_NAME).bin
LIBTSP_NAME = $(PRJ_NAME).ko

ifeq ($(BUILD_VERBOSE),1)
Q =
AS			= $(BUILD_CC)as
# Always use GNU ld
ifneq ($(shell $(BUILD_CC)ld.bfd -v 2> /dev/null),)
LD			= $(BUILD_CC)ld.bfd
else
LD			= $(BUILD_CC)ld
endif
CC			= $(BUILD_CC)gcc
CPP			= $(CC) -E
AR			= $(BUILD_CC)ar
NM			= $(BUILD_CC)nm
LDR			= $(BUILD_CC)ldr
STRIP		= $(BUILD_CC)strip
OBJCOPY		= $(BUILD_CC)objcopy
OBJDUMP		= $(BUILD_CC)objdump
GENDEP		= $(FIXDEP)
else
Q = @
AS			= echo " AS	$@"; $(BUILD_CC)as
ifneq ($(shell $(BUILD_CC)ld.bfd -v 2> /dev/null),)
LD			= echo " LD	$@"; $(BUILD_CC)ld.bfd
else
LD			= echo " LD	$@"; $(BUILD_CC)ld
endif
CC			= echo " CC	$@"; $(BUILD_CC)gcc
CPP			= echo " CPP	$@"; $(CC) -E
AR			= echo " AR	$@"; $(BUILD_CC)ar
NM			= echo " NM	$@"; $(BUILD_CC)nm
LDR			= echo " LDR	$@"; $(BUILD_CC)ldr
STRIP		= echo " STRIP	$@"; $(BUILD_CC)strip
OBJCOPY		= echo " OBJCOPY	$@"; $(BUILD_CC)objcopy
OBJDUMP		= echo " OBJDUMP	$@"; $(BUILD_CC)objdump
GENDEP		= echo " GENDEP	$@"; $(FIXDEP)
endif

export Q BUILD_VERBOSE OBJ_DIR SRC_DIR

# for include -I$(TDK_INC)
INCLUDE = -I. -I$(INC_DIR) -I$(TDK_DIR)/include
#INCLUDE += -I$(PORTABLE_INC)
INCLUDE += -I$(RTOS_INC)
INCLUDE += -I$(PLT_INC)

VPATH := $(OBJ_DIR)

# for CFLAGS


CSTD_FLAG := -std=gnu11
CFLAGS = -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -fno-omit-frame-pointer -ffreestanding

ifdef BUILD_OPTIMIZE
CFLAGS += $(BUILD_OPTIMIZE)
else
CFLAGS += -os
endif

CFLAGS += -fno-strict-aliasing -ffunction-sections -fdata-sections

ifeq ($(CONFIG_TDKLIBC),y)
CFLAGS += -nostdlib -nostdinc -isystem $(shell $(BUILD_CC)gcc -print-file-name=include)
endif


ifeq ($(CONFIG_NEWLIBC_NANO),y)
LIBC_EXTRA_FLAGS := --specs=nano.specs
CFLAGS += $(LIBC_EXTRA_FLAGS)
endif

ifeq ($(CONFIG_NEWLIBC_NOSYS),y)
LIBC_EXTRA_FLAGS := --specs=nosys.specs
CFLAGS += $(LIBC_EXTRA_FLAGS)
endif


CFLAGS += -fno-pic $(BUILD_CPU_CFLAGS) $(BUILD_USER_CFLAGS)
CFLAGS += $(INCLUDE)

# for cflag or aflag depend
sinclude build/ccmd

DEP_CFLAG_F = $(OBJ_DIR)/ccmd

ifeq ($(CCMD),)
DEP_CFLAG =
else
DEP_CFLAG = $(DEP_CFLAG_F)
endif

# for dts
MK_DTB_CFLAGS := -nostdinc -I. -Iinclude -undef -D__DTS__  -x assembler-with-cpp
MK_DTC_FLAGS := -Wno-unit_address_vs_reg -Wno-unit_address_format -Wno-avoid_unnecessary_addr_size
MK_DTC_FLAGS += -Wno-alias_paths -Wno-graph_child_address -Wno-graph_port -Wno-simple_bus_reg
MK_DTC_FLAGS += -Wno-unique_unit_address -Wno-pci_device_reg

MK_DTB_DIR := $(OBJ_DIR)dts/
MK_DTB_DTS := ./arch/dts/caribe_fpga.dts
MK_DTB_OBJ := $(MK_DTB_DIR)board.dtb.o

# for c, cpp source list
CFILES-y :=
AFILES-y :=

#SUB_MKFILE-y += platform/Makefile
#SUB_MKFILE-y += libc/Makefile
#SUB_MKFILE-y += core/drv/Makefile
#SUB_MKFILE-y += core/fdt/Makefile
#SUB_MKFILE-y += core/fs/Makefile
SUB_MKFILE-y += os/Makefile
SUB_MKFILE-y += fs/Makefile
SUB_MKFILE-y += kdrv/Makefile
SUB_MKFILE-y += klib/Makefile
SUB_MKFILE-y += klib/sh/Makefile

sinclude $(SUB_MKFILE-y)

LIBTSP_COBJS := $(patsubst %.c, $(OBJ_DIR)%.o, $(wildcard $(LIBTSP_C_y)))
LIBTSP_AOBJS := $(patsubst %.S, $(OBJ_DIR)%.o, $(wildcard $(LIBTSP_A_y)))

COBJS := $(patsubst %.c, $(OBJ_DIR)%.o, $(wildcard $(CFILES-y)))
AOBJS := $(patsubst %.S, $(OBJ_DIR)%.o, $(wildcard $(AFILES-y)))

$(OBJ_DIR)%.o:    %.c $(DEP_CFLAG) $(CONFIG_FILE)
	$(Q) if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(Q) $(CC) $(CFLAGS) -o $@ $< -c

$(OBJ_DIR)%.o:    %.S $(DEP_CFLAG) $(CONFIG_FILE)
	$(Q) if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(Q) $(CC) -D__ASSEMBLY__ $(CFLAGS) -o $@ $< -c

.PHONY: alld

all: $(DTC_EXE) $(MKUIMG_EXE) $(DEPEND_FILE) $(TSP_LDS_FILE)  $(LIBTSP_NAME)


$(TSP_LDS_FILE): $(TSP_LDS_INCLUDE) $(TSP_LDS_S_FILE)
	$(Q) if [ -f $(TSP_LDS_S_FILE) ]; then \
		echo "Gen lds file";$(CC) -E -D__ASSEMBLY__  -include $(TSP_LDS_INCLUDE) -P -C -o $(TSP_LDS_FILE) $(TSP_LDS_S_FILE); \
	fi

$(LIBTSP_NAME): $(LIBTSP_COBJS) $(LIBTSP_AOBJS)
	$(AR) csr $(OBJ_DIR)$@ $^
	$(Q) $(OBJDUMP) -d $(OBJ_DIR)$@ > $(OBJ_DIR)$@.asm
	$(Q) ls -l $(OBJ_DIR)$@

gdtb: $(DTC_EXE)
	$(Q) mkdir -p $(MK_DTB_DIR)
	$(Q) $(CC) -E -Wp,-MD,$(MK_DTB_DIR)board.pre.tmp $(MK_DTB_CFLAGS) -o $(MK_DTB_DIR)board.dtb.tmp $(MK_DTB_DTS)
	$(Q) $(DTC_EXE) $(MK_DTC_FLAGS) -I dts -O dtb -o $(MK_DTB_DIR)board.dtb $(MK_DTB_DIR)board.dtb.tmp
	$(Q) $(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $(MK_DTB_DIR)board.dtb $(MK_DTB_DIR)board.dtb.o
	$(Q) $(AR) r $(MK_DTB_DIR)board.dtb.a $(MK_DTB_DIR)board.dtb.o

clean:
	$(Q) echo " CLEAN ALL";
	$(Q) if [ -d $(OBJ_DIR) ]; then \
		rm -rf $(OBJ_DIR)/*; \
	fi
	$(Q)  echo " CLEAN DONE"; 

distclean: clean
	$(Q) echo " REMOVE FIXDEP"; rm -f $(FIXDEP)
	$(Q) echo " MAKE DISTCLEAN Kconfig"; make -C $(MENU_CFG_DIR) distclean
	$(Q) echo " MAKE DISTCLEAN mkdep"; make -C $(SCRIPTS_DIR)mkdep distclean
	$(Q) echo " MAKE DISTCLEAN mkimg"; make -C $(SCRIPTS_DIR)mkimage distclean
	$(Q) rm -f .config .config.old
	$(Q) if [ -d $(INC_DIR)/config ]; then \
		rm -rf $(INC_DIR)/config/*; \
	fi
	$(Q)  echo " DISTCLEAN DONE"; 

.PHONY: install
install:
	$(Q) echo "Install tsp";
	$(Q) if [ -d $(INSTALL_DIR) ]; then \
		mv $(INSTALL_DIR) $(TDK_DIR)/bk_os_$(WORK_NOW); \
	fi
	$(Q) mkdir -p $(INSTALL_DIR)
	$(Q) mkdir -p $(INSTALL_DIR)/release
	$(Q) mkdir -p $(INSTALL_DIR)/scripts/mkdep
	$(Q) mkdir -p $(INSTALL_DIR)/scripts/mkimage
	$(Q) cp -rf include $(INSTALL_DIR)
	$(Q) cp -rf $(PLT_INC)/asm $(INSTALL_DIR)/include/
	$(Q) cp -rf $(RTOS_INC) $(INSTALL_DIR)/include/rtos
	$(Q) cp -rf .config $(INSTALL_DIR)
	$(Q) cp -rf Makefile $(INSTALL_DIR)
	$(Q) cp -rf Makefile_app $(INSTALL_DIR)
	$(Q) cp -rf Makefile_modules $(INSTALL_DIR)
	$(Q) cp -rf scripts/mkdep/fixdep $(INSTALL_DIR)//scripts/mkdep
	$(Q) cp -rf scripts/mkimage/mkuimg $(INSTALL_DIR)/scripts/mkimage
#	$(Q) cp -rf $(OBJ_DIR)$(LIBTSP_NAME) $(INSTALL_DIR)/release
#	$(Q) cp -rf $(OBJ_DIR)tdk.lds $(INSTALL_DIR)/release
#	$(Q) cp -rf $(MK_DTB_DIR)board.dtb.a $(INSTALL_DIR)/release

mconf_prepare:
	chmod 777 $(MENU_CFG_DIR)/lxdialog/check-lxdialog.sh
	mkdir -p include/config

%_defconfig:mconf_prepare $(MENU_CFG_DIR)/conf
	@$(MENU_CFG_DIR)/conf --defconfig=configs/$@ $(KCONFIG_TOPCONFIG)
	@$(MENU_CFG_DIR)/conf --syncconfig $(KCONFIG_TOPCONFIG)
	
mcfg:mconf_prepare $(MENU_CFG_DIR)/mconf
	$(MENU_CFG_DIR)/mconf $(KCONFIG_TOPCONFIG)

$(MENU_CFG_DIR)/mconf:
	make -C $(MENU_CFG_DIR) mconf

$(MENU_CFG_DIR)/conf:
	make -C $(MENU_CFG_DIR) -f $(MENU_CFG_DIR)/Makefile conf

$(FIXDEP):
	$(Q) make -C $(SCRIPTS_DIR)/mkdep

$(DTC_EXE):
	$(Q) make -C $(SCRIPTS_DIR)dtc

$(MKUIMG_EXE):
	$(Q) make -C $(SCRIPTS_DIR)mkimage

.PHONY: savecfg
savecfg:
	$(Q) echo cp $(CONFIG_SAVE_NAME) to configs
	$(Q) cp -f .config configs/$(CONFIG_SAVE_NAME)

$(DEPEND_FILE): Makefile $(FIXDEP) $(CONFIG_FILE)
	$(Q) if [ ! -d $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi

	$(Q) if [ "$(CCMD)" != "$(CFLAGS)" ]; then \
		echo "CCMD=$(CFLAGS)" > $(DEP_CFLAG_F); \
	fi

	$(Q) $(GENDEP) $(INCLUDE) -- $(CFILES-y) $(AFILES-y) > $@

sinclude $(DEPEND_FILE)
