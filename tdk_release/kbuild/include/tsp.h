/*
 * tsp.h -- tsp 系统接口声明头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TSP_H__
#define __TSP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <config.h>
#include <base/ts_type.h>

#if (CONFIG_ITCM_SIZE > 0)
/* 代码定义到 itcm 段,类似与 ICache 永驻 */
#define TS_RELOC_ITCM       __attribute__((section(".itcm")))
#else
#define TS_RELOC_ITCM
#endif

#if (CONFIG_DTCM_SIZE > 0)
#define TS_RELOC_DTCM       __attribute__((section(".dtcm")))
#else
#define TS_RELOC_DTCM
#endif

extern int main(int argc, char *argv[]);

/* 启动初始化函数， tsp内全部是 weak 定义，用户可以自定义实现 */

/* */
ts_void tsp_get_heap_info(ts_u32 *pu32_heap_base, ts_u32 *pu32_heap_size);

/*
加载串口驱动: 系统控制台驱动需要调用串口, tsp 内实现如下:
    TS_WEAK ts_void tsp_insmod_uart(ts_void)
    {
        ts_cpl_insmod(uart, "uart1_rxsz=64 uart2_rxsz=128");
    }
*/
ts_void tsp_insmod_uart(ts_void);

/*
开启命令行线程, tsp 内实现如下:
    TS_WEAK ts_void tsp_start_shell(ts_void)
    {
        ts_cpl_task_s_t st_sh_task;

        cli_init();

        //激活命令
        ts_active_cmd(mem);

        //创建命令线程
        ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_sh_task, (ts_cpl_kthread_t)cli_simple_loop,
                 ts_null, "sh", 6, 2048);
    }
*/
ts_void tsp_start_shell(ts_void);


/*
开启主线程, tsp 内实现如下:
    TS_WEAK ts_void tsp_start_main(ts_void)
    {
        ts_cpl_task_s_t st_main_task;

        ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_main_task, (ts_cpl_kthread_t)main, \
                ts_null, "main", 6, 2048);
    }
*/
ts_void tsp_start_main(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TSP_H__ */
