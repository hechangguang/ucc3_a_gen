/*
 * riscv_encoding.h -- tsp riscv_encoding 声明头文件,根据配置引用对应的芯片定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __RISCV_ENCODING_H__
#define __RISCV_ENCODING_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <config.h>

#ifdef CONFIG_E907
#include <asm/e907_encoding.h>
#endif

#ifdef CONFIG_N900
#include <asm/n900_encoding.h>
#endif

#define STR(x)                  XSTR(x)
#define XSTR(x)                 #x
#define __STR(s)                #s
#define STRINGIFY(s)            __STR(s)

#ifdef __cplusplus
}
#endif

#endif /* __RISCV_ENCODING_H__ */
