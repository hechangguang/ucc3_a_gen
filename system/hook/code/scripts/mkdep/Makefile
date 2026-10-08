# 
# Makefile -- 生成依赖关系工具 Makefile 模板 
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 

# the Project's name
PRJ_NAME = fixdep

# Configure the Makefile *.h *.hpp search path
HOST_INCLUDE = -I.
HOST_CC 	= gcc
HOST_CFLAGS = -O2

COBJS = 

all: $(PRJ_NAME) install

$(PRJ_NAME): $(PRJ_NAME).c
	$(Q) $(HOST_CC) $(HOST_CFLAGS) $(HOST_INCLUDE) $< -o $@

install:
	$(Q) mv -f $(PRJ_NAME) ../

clean:
	$(Q) rm -f *.o $(PRJ_NAME) $(INSTALL_PATH)/$(PRJ_NAME)
