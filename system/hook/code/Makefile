# 
# Makefile -- Bootup 顶层 Makefile
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 
VERSION = 2023_03
PRJ_NAME = hook

soc ?= doppler
os_root ?= /opt/tdk/$(soc)/release

WORK_NOW = $(shell date +%Y%m%d_%H%M%S)

# 目录定义
WORK_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
SCRIPTS_DIR = $(WORK_DIR)/scripts/
MENU_CFG_DIR = $(SCRIPTS_DIR)/kconfig/
OBJ_DIR = build/
SRC_DIR = src/
INC_DIR = include/
LIB_DIR = lib/

# Kconfig 配置
KCONFIG_AUTOHEADER := $(INC_DIR)/config/autoconf.h
KCONFIG_AUTOCONFIG := $(INC_DIR)/config/auto.conf
KCONFIG_TOPCONFIG := $(WORK_DIR)/src/Kconfig

# for gen lds
BOOTUP_LDS_FILE := $(OBJ_DIR)bootup.lds
BOOTUP_LDS_S_FILE := $(SCRIPTS_DIR)bootup.lds.S
BOOTUP_LDS_INCLUDE := $(KCONFIG_AUTOHEADER)

# for depend
DEPEND_FILE := $(OBJ_DIR)mkdep

# for tdk inclue
TDK_INC ?= $(tdk_root)/include

# config file
CONFIG_FILE := $(WORK_DIR)/.config
IH_CFG_FILE := $(SRC_DIR)/ih/ih.c

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
LD_EXTRA_FLAG := $(shell echo $(CONFIG_EXT_LD_FLAGS) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
BUILD_OPTIMIZE := $(shell echo $(CONFIG_OPTIMIZE) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
GEN_SVR_ADDR := $(shell echo $(CONFIG_GEN_SIGN_SVR) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
GEN_KEY_NAME := $(shell echo $(CONFIG_GEN_KEY_NAME) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')
SOC_DIR := $(shell echo $(CONFIG_BOARD_NAME) | sed -e 's/^[\"]*//g' | sed -e 's/[\"]$$//g')

FIXDEP		= $(SCRIPTS_DIR)/fixdep
GEN_IMG		= $(SCRIPTS_DIR)/gen_img
FIXNAME		= $(PRJ_NAME).bin
PLATSYM		= $(SRC_DIR)platform/$(SOC_DIR)/platform.sym
PLATSOC		= $(SRC_DIR)platform/$(SOC_DIR)/soc_main.c

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

export Q BUILD_VERBOSE OBJ_DIR SRC_DIR TDK_INC

# for generate image header
ifeq ($(CONFIG_GEN_SIGN_EN),y)
GI_SIGN_OPTS := --keyname=$(GEN_KEY_NAME)
GI_SIGN_OPTS += --svr=$(GEN_SVR_ADDR) --port=$(CONFIG_GEN_SIGN_PORT) --sign=y
else
GI_SIGN_OPTS :=
endif

GI_HOOK_OPTS := --config=$(SRC_DIR)ih/ih.c.cfg --hook=$(OBJ_DIR)$(FIXNAME) --outf=$(OBJ_DIR)hook.img $(GI_SIGN_OPTS)
GI_INF_OPTS := --config=$(SRC_DIR)ih/ih.c.cfg --inf=$(OBJ_DIR)demo.bin --outf=$(OBJ_DIR)app.img $(GI_SIGN_OPTS)

GI_BOTH_OPTS := $(GI_HOOK_OPTS) --inf=$(OBJ_DIR)demo.bin

GI_SIGN_ONLY_OPTS := --offset=64 $(GI_SIGN_OPTS)
GI_SIGN_ONLY_OPTS += --inf=$(OBJ_DIR)demo.uimg --outf=$(OBJ_DIR)sign_only.img

# for include
INCLUDE = -I. -I$(INC_DIR) -I$(TDK_INC)

VPATH := $(OBJ_DIR)

# auto get libgcc.a
LIBGCC_A := $(shell $(BUILD_CC)gcc $(BUILD_CPU_CFLAGS) -print-libgcc-file-name)
LIBS := $(LIBGCC_A)

# for CFLAGS
NOSTDINC_FLAGS := -nostdlib -nostdinc -isystem $(shell $(BUILD_CC)gcc -print-file-name=include)
CSTD_FLAG := -std=gnu11
CFLAGS = -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding $(BUILD_OPTIMIZE)
CFLAGS += -fno-strict-aliasing -ffunction-sections -fdata-sections $(NOSTDINC_FLAGS)
CFLAGS += -fno-pic $(BUILD_CPU_CFLAGS) $(BUILD_USER_CFLAGS)
CFLAGS += $(INCLUDE)

# for LDFLAGS
LDFLAGS := -Bstatic -m elf32lriscv -T$(BOOTUP_LDS_FILE) -Ttext $(CONFIG_TEXT_BASE_ADDR) $(LD_EXTRA_FLAG)

# for cflag or aflag depend
sinclude build/ccmd

DEP_CFLAG_F = $(OBJ_DIR)/ccmd

ifeq ($(CCMD),)
DEP_CFLAG =
else
DEP_CFLAG = $(DEP_CFLAG_F)
endif

# for c, cpp source list
CFILES-y := src/hook.c
CFILES-y += $(PLATSOC)

SUB_MKFILE-y = $(SRC_DIR)platform/Makefile
SUB_MKFILE-y += $(SRC_DIR)driver/Makefile
SUB_MKFILE-y += $(SRC_DIR)lib/Makefile
SUB_MKFILE-y += $(SRC_DIR)cmd/Makefile
SUB_MKFILE-y += $(SRC_DIR)app/Makefile

sinclude $(SUB_MKFILE-y)

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

.PHONY: all

all: $(DEPEND_FILE) $(BOOTUP_LDS_FILE) $(FIXNAME) ih genpkt


$(BOOTUP_LDS_FILE): $(BOOTUP_LDS_INCLUDE) $(BOOTUP_LDS_S_FILE)
	$(Q) if [ -f $(BOOTUP_LDS_S_FILE) ]; then \
		echo "Gen lds file";$(CC) -E -D__ASSEMBLY__  -include $(BOOTUP_LDS_INCLUDE) -P -C -o $(BOOTUP_LDS_FILE) $(BOOTUP_LDS_S_FILE); \
	fi

$(FIXNAME): $(AOBJS) $(COBJS)
	$(Q) $(LD) $(LDFLAGS) -Map $(OBJ_DIR)$(PRJ_NAME).map -o $(OBJ_DIR)$(PRJ_NAME).elf $^ --start-group $(LIBS) --end-group $(PLATSYM)
	$(Q) $(OBJCOPY) -O binary -R .note -R .comment -S $(OBJ_DIR)$(PRJ_NAME).elf $(OBJ_DIR)$@
	$(Q) $(OBJDUMP) -d $(OBJ_DIR)$(PRJ_NAME).elf > $(OBJ_DIR)$(PRJ_NAME).asm
	$(Q) ls -l $(OBJ_DIR)$@

genpkt: $(GEN_IMG)/gen_img ih
	$(Q) ./scripts/gen_img/gen_img $(GI_HOOK_OPTS)

geninf: $(GEN_IMG)/gen_img
	$(Q) ./scripts/gen_img/gen_img $(GI_INF_OPTS)

genboth: $(GEN_IMG)/gen_img
	$(Q) ./scripts/gen_img/gen_img $(GI_BOTH_OPTS)

gen_sign_only: $(GEN_IMG)/gen_img
	$(Q) ./scripts/gen_img/gen_img $(GI_SIGN_ONLY_OPTS)

clean:
	$(Q) echo " CLEAN ALL";
	$(Q) if [ -d $(OBJ_DIR) ]; then \
		rm -rf $(OBJ_DIR)/*; \
	fi
	$(Q)  echo " CLEAN DONE"; 

distclean: clean
	$(Q) echo " REMOVE FIXDEP"; rm -f $(FIXDEP)
	$(Q) echo " MAKE DISTCLEAN Kconfig"; make -C $(MENU_CFG_DIR) distclean
	$(Q) rm -f .config .config.old
	$(Q) if [ -d $(INC_DIR)/config ]; then \
		rm -rf $(INC_DIR)/config/*; \
	fi
	$(Q)  echo " DISTCLEAN DONE"; 


.PHONY: openssl
./openssl/release/lib/libssl.a:
	$(Q) echo "make openssl"
	$(Q) mkdir -p openssl
	$(Q) mkdir -p openssl/release
	$(Q) cd openssl;tar xf openssl-OpenSSL_1_0_2k.tar.xz;
	$(Q) cd openssl/openssl-OpenSSL_1_0_2k;./config --prefix=$(WORK_DIR)/openssl/release;make;make install
	

mconf_prepare:
	chmod 777 $(MENU_CFG_DIR)/lxdialog/check-lxdialog.sh
	mkdir -p include/config

%_defconfig: mconf_prepare $(MENU_CFG_DIR)/conf
	@$(MENU_CFG_DIR)/conf --defconfig=configs/$@ $(KCONFIG_TOPCONFIG)
	@$(MENU_CFG_DIR)/conf --syncconfig $(KCONFIG_TOPCONFIG)
	
mcfg: mconf_prepare $(MENU_CFG_DIR)/mconf
	$(MENU_CFG_DIR)/mconf $(KCONFIG_TOPCONFIG)

$(MENU_CFG_DIR)/mconf:
	make -C $(MENU_CFG_DIR) mconf

$(MENU_CFG_DIR)/conf:
	make -C $(MENU_CFG_DIR) -f $(MENU_CFG_DIR)/Makefile conf

#$(GEN_IMG):
$(GEN_IMG)/gen_img:
	$(Q) make -C $(SCRIPTS_DIR)/gen_img CALL_DIR=$(WORK_DIR) all
	$(Q) chmod +x $(SCRIPTS_DIR)/gen_img

sign_svr: ./openssl/release/lib/libssl.a
	$(Q) make -C $(SCRIPTS_DIR)/sign_server CALL_DIR=$(WORK_DIR) all

$(FIXDEP):
	$(Q) make -C $(SCRIPTS_DIR)/mkdep

ih: $(CONFIG_FILE)
	$(Q) $(CC) $(CFLAGS) -c $(IH_CFG_FILE) -o $(IH_CFG_FILE).o
	$(Q) $(OBJCOPY) -O binary $(IH_CFG_FILE).o $(IH_CFG_FILE).cfg

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
