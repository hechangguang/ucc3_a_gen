/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:00:02
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2025-11-26 17:48:52
 * @FilePath: /ucc3_a_gen/vot_src/vot/vot_config.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef  __VOT_CONFIG_H__
#define __VOT_CONFIG_H__
#include <api.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <menu/ispfunction.h>

void load_isp_config_for_bpc(void);
void load_isp_config_for_iso_mode(void);
void load_isp_config(void);
void copy_isp_config(uint8_t *dest, uint8_t *src, size_t cfg_size) ;


#endif