/* 
 * cpl_tty.h -- linux tty cpl 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_TTY_H__
#define __CPL_TTY_H__

typedef struct ts_cpl_tty_s {
    ts_void *pv_cur_tty;
    ts_s16 s16_type;
    ts_s16 s16_sub_type;
} ts_cpl_tty_s_t;

#define TS_CPL_TTY_TYPE_SERIAL          (0x0003)

extern ts_s32 ts_cpl_get_current_tty(ts_cpl_tty_s_t *);
extern ts_s32 ts_cpl_tty_putc(ts_cpl_tty_s_t *, ts_u08);
extern ts_s32 ts_cpl_tty_flush(ts_cpl_tty_s_t *);

#endif /* __CPL_TTY_H__ */
