/*
 * main.c -- solution demo for t10x
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <tsp.h>
#include <uapi/base_v100.h>
#include <uapi/sfc_v100.h>
#include <uapi/wdt_v100.h>
#include <uapi/timer_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/snr_v100.h>
#include <uapi/scpu_v100.h>

/* for 命令行 */
#include <command.h>
#include "sffs_files.h"
#include "api.h"
#include "snr_comm.h"
#include <stdio.h>
#include <menu/votmain.h>
#include <menu/usermenu.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
#include <common/TempCal.h>
#include <menu/testmenufunction.h>

ts_s32 gs32_vision_log_lvl;
extern ts_u32 gu32_rom_size;
extern SysCtrlInfo_s_t global_sys_ctrl_info;
ts_s32 s32_global_flash_type = 2;
/* AISP 和 SR 互斥 */
#if defined(CONFIG_SR_EN) && defined(CONFIG_AISP_EN)
#error "AISP and SR cannot be enabled at the same time"
#endif

/* 开启终端(命令行)线程, 如果无需终端，此函数可以空实现 */
ts_void tsp_start_shell(ts_void)
{
    ts_cpl_task_s_t st_sh_task;
    ts_u32 u32_prio, u32_stack_size;

    /* 初始化命令行 */
    cli_init();

    /* system tools */
    ts_active_cmd(mem);
    ts_active_cmd(cat);
    ts_active_cmd(echo);
    ts_active_cmd(free);
    ts_active_cmd(ps);
    ts_active_cmd(ls);

    /* 重启命令，建议生产环境下关闭 */
    ts_active_cmd(reboot);

    /* vision 命令 */
    // ts_active_cmd(sffs);
    // ts_active_cmd(sht);
    ts_active_cmd(tmm);
    // ts_active_cmd(i2c);
	// ts_active_cmd(mnu);
    // ts_active_cmd(vpo);
    // ts_active_cmd(upl);
    // ts_active_cmd(cvbs);
#if defined(DEV_SIM_MODE) 
    ts_active_cmd(sim);
#endif

    #ifdef CONFIG_LVGL
        ts_active_cmd(lvgl_disp);
    #endif
    #ifdef CONFIG_OP_EN
        ts_active_cmd(op);
    #endif

    /* isp 相关命令 */
    ts_active_cmd(isp);
    // ts_active_cmd(sffc);

    ts_active_cmd(vot);
    // ts_active_cmd(tl);
#ifdef VOT_FIRE_HOT
    ts_active_cmd(fire_hot);
#endif

#ifdef CONFIG_SL_EN
    ts_active_cmd(sl);
#endif

    // ts_active_cmd(iso);
    
    /* 优先级，越大优先级越低 */
    u32_prio = 6;

    /* 栈大小，可以根据实际运行情况(ps 命令查看)来调整 */
    u32_stack_size = 2048*4;

    /* 创建命令线程 */
    ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_sh_task, (ts_cpl_kthread_t)cli_simple_loop, \
        ts_null, "sh", u32_prio, u32_stack_size);
}


/* 调整主线程优先级和栈大小 */
ts_void tsp_start_main(ts_void)
{
    ts_cpl_task_s_t st_main_task;
    ts_s32 s32_prio;
    ts_u32 u32_stack_size;

    /* 数字越大，优先级越低 */
    s32_prio = 9;

    /* 栈大小，可以通过 ps 命令查看栈使用率来进行调整 */
    u32_stack_size = 2048*6;

    ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_main_task, (ts_cpl_kthread_t)main, \
            ts_null, "main", s32_prio, u32_stack_size);
}

/* 调整堆大小, 全部为 0 配置，用满整个 SRAM */
ts_void tsp_get_heap_info(ts_u32 *pu32_heap_base, ts_u32 *pu32_heap_size)
{
    if (pu32_heap_base != ts_null)
        *pu32_heap_base = 0;

    if (pu32_heap_size != ts_null)
        *pu32_heap_size = 0;

    /* 此函数为系统最早调用的用户接口函数，可以在此加入看门狗启动代码，或者直接在 hook 开启看门狗，此处喂狗 */
    ts_lib_wdt_start(6000);
}


/* 控制初始化需要首先加载串口 */
ts_void tsp_insmod_uart(ts_void)
{
    ts_cpl_insmod(uart, "uart0_rxsz=1024 uart1_rxsz=1024 uart2_rxsz=64 uart0_tm=100 uart1_tm=100");
}


int main(int argc, char *argv[])
{
    ts_s32 s32_hfile_r;
    sfc_cfg_file_s_t st_sfc_file;
    ts_u32 u32_tmp, u32_usb_en, u32_auto_en, u32_isp_en, u32_rom_sz;
    uint32_t heartbeat_counter = 0;

    TS_UNUSED(u32_usb_en);
    TS_UNUSED(u32_auto_en);
    TS_UNUSED(u32_isp_en);
    vot_easylog_init();

    // 重定位终端 /
    tsp_kcon_init("/dev/ttyS3");

    // 关闭日志打印
    ts_cpl_set_log_en(0);
    /* 配置日志等级 */
    gs32_vision_log_lvl = TS_LOG_DEBUG;
    /* 加载基础驱动,初始化 */
    u32_rom_sz = (ts_u32)&gu32_rom_size;

	memset(&global_sys_ctrl_info, 0, sizeof(SysCtrlInfo_s_t));

    /* 检测内存 */
    u32_tmp = ts_cpl_readl(0x001e0014);
    if (0 == u32_tmp) {
        vision_log_debug("init to sbank\n");
        ts_tdk_init_tmm(TS_TMM_N6_MODE, 0, 96, 0, u32_rom_sz);
        global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_N6;
    } else {
        vision_log_debug("init to dbank\n");
        ts_tdk_init_tmm(TS_TMM_A12_MODE, 0, 96, 0, u32_rom_sz);
        global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_A12;
    }

    ts_lib_hwlock_init(0x00160200, 6);

    ts_cpl_insmod(base, ts_null);
    ts_cpl_insmod(gpio, ts_null);
    ts_cpl_insmod(sfc, ts_null);
    ts_cpl_insmod(spi, ts_null);
    ts_cpl_insmod(timer, ts_null);

    ts_lib_gpio_init();
    ts_lib_timer_init();

    ts_s32 s32_global_flash_type = ts_cpl_readl(0x00160220);
    vision_log_warn("s32_global_flash_type %d \n",s32_global_flash_type);
    if(s32_global_flash_type==2){
        vision_sffs_init(SFFS_INNOR_ONULL);
        vision_log_warn("nor flash  \n");
        global_sys_ctrl_info.m_flash_type = VOT_ASIC_NOR_FLASH_8MB;
        if(global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_N6){
            global_sys_ctrl_info.m_t10_tpu_type = T10_CPU_TYPE_N6E;
        }
    }
    else {
        vision_sffs_init(SFFS_INAND_ONULL);
        vision_log_warn("nand flash  \n");
        global_sys_ctrl_info.m_flash_type = VOT_ASIC_NAND_FLASH_128MB;
    }

    vision_log_debug("\n%s: %s %s\n", CONFIG_APP_NAME, __DATE__, __TIME__);

    /* 加载配置文件 */
    s32_hfile_r = ts_lib_sfc_open(FN_SUPER_USR);
    vision_log_debug("Open %s done, handle = %d\n", FN_SUPER_USR, s32_hfile_r);
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        ts_lib_sfc_close(s32_hfile_r);
    } else {
        st_sfc_file.u32_valid_code = 0;
    }

    u32_usb_en = 1;
    u32_auto_en = 1;
    u32_isp_en = 1;


    vot_misc_parameters_init();

    if(vot_get_force_debug_flag())
    {
        // 重定位终端 /
        tsp_kcon_init("/dev/ttyS0");
        ts_cpl_set_log_en(1);
        /* 配置日志等级 */
        gs32_vision_log_lvl = TS_LOG_DEBUG;
    }

    vision_log_warn("start log output ! \n");
#if   !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
    init_sys_ext_config();

    menu2_parameters_init();
    menu1_parameters_init();
    TEMP_Parameters_init();
#endif

    // #ifdef CONFIG_SNR_PLANCK1280C
    // 光智1280探测器planck1280c
    // SystemParaMenu2.m_force_hardware_port_type=FORCE_HARDWARE_PORT_TYPE_UAD3;//光智UAD3硬件
    // SystemParaMenu2.m_force_detector_type=FORCE_DETECTOR_TYPE_VOT_1280X1024;//目前只强制调试光智1280X1024探头
    // #endif
#if   !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
    create_vot_wdt_feed_pthread();
#endif

    brd_gpio_mux_vot_init();

#if   !defined(DEV_TESTBAR_MODE) 
    if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {
        vision_log_debug("Read info ok, snr is %s\n", st_sfc_file.ac_cfg_name);
        start_isp(st_sfc_file.ac_cfg_name, u32_usb_en, u32_auto_en, u32_isp_en);
    } else {
        vision_log_debug("Read info fail, start to default\n");
#ifndef  DEV_SIM_MODE

#if   !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
   //vot_uart_pthread_init();//创建串口线程必须在开始start_isp之前，串口线程才能正常，否则start_isp启动异常，串口线程无法创建。
   vot_routine_pthread_init();
#endif

    show_dector_name_type();
    show_dector_resolution_type();
    dector_type_t dector_type = get_dector_name_type();
    set_powerup_dector_resolution_type();
    set_powerup_dector_name_type();
    set_powerup_board_port_type();
    if(dector_type!=DECTOR_TYPE_RTD){
        dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
        // vision_log_debug("res_type %d \n",res_type);
        if(res_type==DECTOR_RESOLUTION_640X512){
            start_isp("planck640c", u32_usb_en, u32_auto_en, u32_isp_en);
        }
        else if(res_type==DECTOR_RESOLUTION_384X288){
            start_isp("planck384c", u32_usb_en, u32_auto_en, u32_isp_en);
        }
        else if(res_type==DECTOR_RESOLUTION_1280X1024){
            //vision_log_debug("res_type= DECTOR_RESOLUTION_1280X1024\n");
           start_isp("planck1280c", u32_usb_en, u32_auto_en, u32_isp_en);
           //start_isp("rv1280", u32_usb_en, u32_auto_en, u32_isp_en); 
        } 
    }
    else  if(dector_type==DECTOR_TYPE_RTD){
         dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
        
        if(res_type==DECTOR_RESOLUTION_640X512){
            start_isp("rtd6122c", u32_usb_en, u32_auto_en, u32_isp_en);
        }
        else if(res_type==DECTOR_RESOLUTION_1280X1024 ){
            start_isp("rtds123c", u32_usb_en, u32_auto_en, u32_isp_en);
        }
        else{
            start_isp("rtd6122c", u32_usb_en, u32_auto_en, u32_isp_en);
        }
    }


#else
        vision_log_debug("sim mode ! DEV_SIM_MODE \n");
#endif   
    }
#else
    start_isp("testbar", 1, 0, 0);
    vision_log_debug("testbar mode ! DEV_TESTBAR_MODE \n");
#endif

#if !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
    if(vot_get_force_debug_flag()){
        ts_lib_uart_hbp_task(ts_h_stdin, 25, 1024,1);
    }
#endif

#if   !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
    display_init();
    Sensor_Init(); //Sensor初始化设置
    MoterFun_Init(); //电机相关功能
    //vot_routine_pthread_init();
    getDisplay_Crosshair_Switch();
#endif


#if   !defined(DEV_SIM_MODE)
    if(SystemParaMenu2.m_temp_drift_comp_mode!=TEMP_DRIFT_COMPENSATION_MODE_ON){
        vot_reset_sence_env();
    }
    ISP_nr_set(UsermenuParameter.SFilter * 8);
#endif


#ifdef VOT_MIPI_RESET
    cvbs_pod_set(0);
#endif

    /* 主线程 */
    while(1) {
    #if defined(DEV_SIM_MODE)
        ts_lib_wdt_feeding();
    #endif

        ts_cpl_msleep(2000);
        /* 心跳 */
        heartbeat_counter++;
        if (heartbeat_counter >= 5) {
            heartbeat_counter = 0;
            global_sys_ctrl_info.Heartbeat_Flag = ! global_sys_ctrl_info.Heartbeat_Flag;  // 翻转心跳
        }
    }

#if   !(defined(DEV_SIM_MODE) || defined(DEV_TESTBAR_MODE))
    deinit_sys_ext_config();
#endif

}
