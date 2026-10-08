/*
 * command.h -- 用户命令头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <base/ts_type.h>
#include <base/ts_errno.h>

typedef int (*cmd_main_t)(int argc, char * argv[]);

struct tsp_cmd_s;
typedef struct tsp_cmd_s tsp_cmd_s_t;

typedef struct tsp_cmd_s {
    tsp_cmd_s_t *pst_next;
    char *pc_name;
    cmd_main_t fp_func;
} tsp_cmd_s_t;

/* 标准输出输入 */
extern ts_s32 ts_h_stdin;
extern ts_s32 ts_h_stdout;
extern ts_s32 ts_h_stderr;

void cmd_active(tsp_cmd_s_t *pst_cmd);

#define ts_active_cmd(name) 	\
        extern const tsp_cmd_s_t cmd_tbl_##name; \
        cmd_active((tsp_cmd_s_t *)&cmd_tbl_##name)


#define TS_DECLARE_CMD(name) 	\
        static int main_##name(int argc, char * argv[]);	\
        const tsp_cmd_s_t cmd_tbl_##name = { \
            .pst_next = ts_null,    \
            .pc_name = #name,       \
            .fp_func = main_##name, \
        };    \
		static int main_##name(int argc, char * argv[])


void cli_init(void);
void cli_simple_loop(void);

#endif /* __COMMAND_H__ */