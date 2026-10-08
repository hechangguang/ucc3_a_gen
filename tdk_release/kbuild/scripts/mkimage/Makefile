# 
# Makefile -- 极简 makefile ，用于小工具编译
# Created by : Top-Silicon Software Team
# Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
# 
# This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
# 
EXEC_NAME = mkuimg
EXEC_SOURCES = mkimage.c crc32.c


HOST_CPPFLAGS = -I.

HOST_CFLAGS = -Wall -O2 -fomit-frame-pointer

HOST_CC = gcc

.PHONY: all clean distclean

all:
	$(Q) $(HOST_CC) $(HOST_CPPFLAGS) $(HOST_CFLAGS) -o $(EXEC_NAME) $(EXEC_SOURCES)

clean:
	$(Q) rm -f *.o

distclean: clean
	$(Q) rm -f $(EXEC_NAME)
