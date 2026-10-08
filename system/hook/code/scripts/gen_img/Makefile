# 
# Makefile -- Bootup 打包镜像工具 Makefile
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 

# 获取当前路径和父级路径
PARENT_DIR := $(CALL_DIR)
CURRENT_DIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

HOST_CC := gcc

CURRENT_NAME := gen_img

# 目录设置
CURRENT_INCDIR := ./include/
CURRENT_SRCDIR := src/
CURRENT_BUILDDIR := build
SSL_SET ?= n

# 用户头文件搜索路径 : -I.
INCLUDE_PATH := -I. -I$(CURRENT_INCDIR) -I$(TDK_INC) -I$(PARENT_DIR)/include

CFLAGS := -O2 -DCONFIG_HOST $(INCLUDE_PATH)

LIBS := $(USER_LIB) $(SYS_LIB)


# 自动把C文件转换成.o文件
CFILES-y			= $(CURRENT_SRCDIR)gen_img.c
CFILES-y			+= $(PARENT_DIR)

CFILES-$(SSL_SET)	+= $(CURRENT_SRCDIR)lib/rsa-sign.c

$(CURRENT_BUILDDIR)/checksum.o : $(PARENT_DIR)/src/verify/checksum.c
	$(HOST_CC) $(CFLAGS) -c $< -o $@

$(CURRENT_BUILDDIR)/crc8.o : $(PARENT_DIR)/src/verify/crc8.c
	$(HOST_CC) $(CFLAGS) -c $< -o $@

$(CURRENT_BUILDDIR)/crc32.o : $(PARENT_DIR)/src/verify/crc32.c
	$(HOST_CC) $(CFLAGS) -c $< -o $@

$(CURRENT_BUILDDIR)/sha256.o : $(PARENT_DIR)/src/verify/sha256.c
	$(HOST_CC) $(CFLAGS) -c $< -o $@

$(CURRENT_BUILDDIR)/gen_img.o : $(CURRENT_SRCDIR)gen_img.c
	$(HOST_CC) $(CFLAGS) -c $< -o $@

OBJS := $(CURRENT_BUILDDIR)/checksum.o $(CURRENT_BUILDDIR)/crc8.o $(CURRENT_BUILDDIR)/crc32.o
OBJS += $(CURRENT_BUILDDIR)/sha256.o
OBJS += $(CURRENT_BUILDDIR)/gen_img.o

.PHONY: all
all:  prepare demo $(CURRENT_NAME)

$(CURRENT_NAME):  $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

prepare:
	mkdir -p $(CURRENT_BUILDDIR)

demo:
	echo $(OBJS)
	echo $(CFILES-y)
	echo $(TDK_INC)

clean:
	rm -f  $(CURRENT_NAME)
	rm -rf $(CURRENT_SRCDIR)*.o

release:
	@if [ ! -d $(RELEASE_ARCH_DIR) ]; then \
		mkdir -p $(RELEASE_ARCH_DIR); \
	fi
	cp -vf $(CONFIG_NAME) $(RELEASE_ARCH_DIR)
