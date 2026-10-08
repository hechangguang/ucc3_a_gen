/*
 * gm1215.h
 *
 *  Created on: 2026年5月06日
 *      Author: hechangguang
 */

#ifndef SRC_COMMON_GM1215_H_
#define SRC_COMMON_GM1215_H_

#include "../myheader.h"



void gm1215_Init(void);
int gm1215_set_current(int fd, float current_ua);
int gm1215_get_current(int fd, float *current_ua);
float gm1215_calc_vout(float current_ua, float rset_ohm);
int gm1215_run_task(void);
ts_void create_gm1215_task_pthread(void);
#endif /* SRC_COMMON_GM1215_H_ */
