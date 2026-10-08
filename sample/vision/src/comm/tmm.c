/*
 * tmm.c -- tdk tmm 信息展示
 */
#include <config.h>
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <command.h>
#include <stdio.h>


static ts_void tmm_help(ts_void)
{
    printf("tmm -d 0/1 : set debug off or on\n");
    printf("tmm -i : show tmm info\n");
}


static ts_void mem_info(ts_void)
{
    tmm_sys_mem_s_t st_sys_mem;

    ts_cpl_tmm_get_sys_mem((tmm_sys_mem_s_t *)&st_sys_mem);

    printf("\n--- TDK MEMORY USED ---\n");

    printf("SECT      START        END         SIZE\n");
    printf("TEXT   0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_text.u32_start_addr, 
                        st_sys_mem.st_text.u32_end_addr, st_sys_mem.st_text.u32_total_size);

    printf("DATA   0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_data.u32_start_addr, 
                        st_sys_mem.st_data.u32_end_addr, st_sys_mem.st_data.u32_total_size);

    printf("BSS    0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_bss.u32_start_addr, 
                        st_sys_mem.st_bss.u32_end_addr, st_sys_mem.st_bss.u32_total_size);

    printf("ITCM   0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_itcm.u32_start_addr, 
                        st_sys_mem.st_itcm.u32_end_addr, st_sys_mem.st_itcm.u32_total_size);

    printf("DTCM   0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_dtcm.u32_start_addr, 
                        st_sys_mem.st_dtcm.u32_end_addr, st_sys_mem.st_dtcm.u32_total_size);

    printf("HEAP   0x%08x   0x%08x  0x%08x\n", st_sys_mem.st_heap.u32_start_addr, 
                        st_sys_mem.st_heap.u32_end_addr, st_sys_mem.st_heap.u32_total_size);
}


/* 
    tmm d 0/1
    tmm i
*/
TS_DECLARE_CMD(tmm)
{
    ts_u32 u32_on;

    if (argc > 1) {
        if ('d' == argv[1][1]) {
            if (argc > 2)
                u32_on = ts_cpl_str2num(argv[2]);
            else
                u32_on = 0;

            ts_cpl_tmm_set_debug(u32_on);

        } else if ('i' == argv[1][1]) {
            ts_cpl_tmm_info();
            mem_info();
        } else {
            tmm_help();
        }
    } else {
        tmm_help();
    }

    return 0;
}
