#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/clkm_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/sfc_v100.h>
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "snr_comm.h"
#include "menu/votmenu.h"
#include "menu/usermenu.h"
#include "planck640c_vot.h"
#include "planck1280c_vot.h"
#include <src/sensor/vot/vot_comm.h>
#include "snr_comm.h"

extern SysCtrlInfo_s_t global_sys_ctrl_info;
extern snr_ctx_s_t gst_snr_ctx ;


ts_u32 planck640_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb)
{

    ts_snr_sd_s_t st_snr_sd;
    st_snr_sd.u32_lsb = u32_lsb;
    st_snr_sd.u32_msb = u32_msb;
    ts_s32 ret = ts_tdk_snr_get_sdb(&st_snr_sd);
    if(ret!=TS_OK){
            vision_log_debug("msb %d lsb %d  is illegal!\n",u32_msb,u32_lsb);
        }
    return st_snr_sd.u32_rwdata;
}


ts_void planck640_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata)
{
    ts_snr_sd_s_t st_snr_sd;
    st_snr_sd.u32_lsb = u32_lsb;
    st_snr_sd.u32_msb = u32_msb;
    st_snr_sd.u32_rwdata = u32_wdata;
    ts_s32 ret =ts_tdk_snr_set_sdb(&st_snr_sd);
    if(ret!=TS_OK){
        vision_log_debug("msb %d lsb %d  u32_wdata %u is illegal!\n",u32_msb,u32_lsb, u32_wdata);
    }
}

// ts_u32 planck640_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb)
// {
//     ts_u32 u32_result = 0;
//     ts_snr_sd_s_t st_snr_sd;
//     st_snr_sd.u32_lsb = u32_lsb;
//     st_snr_sd.u32_msb = u32_msb;
//     st_snr_sd.pu32_rdata = &u32_result;

//     ts_s32 ret = ts_tdk_snr_get_sdb(&st_snr_sd);
//     if(ret!=TS_OK){
//         u32_result = 0;
//         vision_log_debug("msb %d lsb %d  is illegal!\n",u32_msb,u32_lsb);
//     }

//     return u32_result;
// }



// ts_void planck640_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata)
// {

//     ts_snr_sd_s_t st_snr_sd;
//     st_snr_sd.u32_lsb = u32_lsb;
//     st_snr_sd.u32_msb = u32_msb;
//     st_snr_sd.u32_wdata = u32_wdata;

//     ts_s32 ret = ts_tdk_snr_set_sdb(&st_snr_sd);
//     if(ret!=TS_OK){
//         vision_log_debug("msb %d lsb %d  u32_wdata %u is illegal!\n",u32_msb,u32_lsb, u32_wdata);
//     }

// }



ts_u32 vot_detetor_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb)
{
    ts_u32 ret=0;
    #ifdef CONFIG_SNR_PLANCK640C
    ret=planck640_read_sd(u32_msb, u32_lsb);
    #endif
    return ret;
}


ts_void vot_detetor_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata)
{
    //注意：只有640探测器，才有DET写gos值；而使用1280探测器时，没有gos值，强写会花屏异常
    #ifdef CONFIG_SNR_PLANCK640C
    return planck640_write_sd( u32_msb, u32_lsb, u32_wdata);
    #endif
}

board_type_t get_hardware_port_type_with_manual(void) 
{
    // vision_log_warn(" SystemParaMenu2.m_force_hardware_port_type %d   \n",SystemParaMenu2.m_force_hardware_port_type);

    if(SystemParaMenu2.m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3 ){
        vision_log_debug(" BOARD_PORT_UAD3  \n");
        return BOARD_PORT_UAD3;
    }
    else if(SystemParaMenu2.m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAD3_LITE ){
        vision_log_debug(" BOARD_PORT_UAD3_LITE  \n");
        return BOARD_PORT_UAD3_LITE;
    }
    else if(SystemParaMenu2.m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW ){
        vision_log_debug(" BOARD_PORT_UAW  \n");
        return BOARD_PORT_UAW;
    }
    else if(SystemParaMenu2.m_force_hardware_port_type==FORCE_HARDWARE_PORT_TYPE_UAW_LITE ){
        vision_log_debug(" BOARD_PORT_UAW_SE_LITE  \n");
        return BOARD_PORT_UAW_SE_LITE;
    }
    else{
        return BOARD_PORT_UNKNOWN;
    }

}



dector_type_t get_dector_name_type_with_manual(void) 
{
    // vision_log_warn(" SystemParaMenu2.m_force_detector_type %d   \n",SystemParaMenu2.m_force_detector_type);

    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_1280X1024){
        // vision_log_debug(" DECTOR_TYPE_VOT  \n");
        return DECTOR_TYPE_VOT;
    }
    else if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_640X512
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_384X288 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_1280X1024 ){
        // vision_log_debug(" DECTOR_TYPE_RTD  \n");
        return DECTOR_TYPE_RTD;
    }
    else if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_640X512
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_384X288 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024 ){
        // vision_log_debug(" DECTOR_TYPE_VOT_WLP  \n");
        return DECTOR_TYPE_VOT_WLP;
    }
    else{
        return DECTOR_TYPE_UNKNOWN;
    }

}

board_type_t get_board_port_type(void)
{
    vision_log_debug(" HARD_BOARD_PORT_TYPE %s   \n",HARD_BOARD_PORT_TYPE);
    if (strcmp(HARD_BOARD_PORT_TYPE, "UAD3_Lite") == 0) {
        return BOARD_PORT_UAD3_LITE;
    } else if (strcmp(HARD_BOARD_PORT_TYPE, "UAD3") == 0) {
        return BOARD_PORT_UAD3;
    }else if(strcmp(HARD_BOARD_PORT_TYPE, "UAW") == 0) {
        return BOARD_PORT_UAW;
    } else if(strcmp(HARD_BOARD_PORT_TYPE, "UAW_Lite") == 0) {
        return BOARD_PORT_UAW_SE_LITE;
    }
    else if (strcmp(HARD_BOARD_PORT_TYPE, "MANUAL") == 0) {
        return get_hardware_port_type_with_manual() ;
    } 
    else
    {
        return BOARD_PORT_UNKNOWN;
    }
}


dector_type_t get_dector_name_type(void) 
{
    // vision_log_debug(" DETECTOR_TYPE_NAME %s   \n",DETECTOR_TYPE_NAME);
    if (strcmp(DETECTOR_TYPE_NAME, "VOT") == 0) {
        return DECTOR_TYPE_VOT;
    } else if (strcmp(DETECTOR_TYPE_NAME, "RTD") == 0) {
        return DECTOR_TYPE_RTD;
    }
    else if (strcmp(DETECTOR_TYPE_NAME, "VOT_WLP") == 0) {
        return DECTOR_TYPE_VOT_WLP;
    }
    else if (strcmp(DETECTOR_TYPE_NAME, "RTD") == 0) {
        return DECTOR_TYPE_RTD;
    }
    else if (strcmp(DETECTOR_TYPE_NAME, "MANUAL") == 0) {
        return get_dector_name_type_with_manual() ;
    }
    else {
        return DECTOR_TYPE_UNKNOWN;
    }
}


dector_resolution_type_t get_dector_resolution_type_with_manual(void) 
{
    // vision_log_warn(" SystemParaMenu2.m_force_detector_type %d   \n",SystemParaMenu2.m_force_detector_type);

    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_640X512
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_640X512){
            // vision_log_debug(" DECTOR_RESOLUTION_640X512  \n");
            return DECTOR_RESOLUTION_640X512;   
    }
    else  if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_384X288
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_384X288){
            // vision_log_debug(" DECTOR_RESOLUTION_384X288  \n");
            return DECTOR_RESOLUTION_384X288;
    }
    else  if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_1280X1024 
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_RTD_1280X1024
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_WLP_1280X1024){
            // vision_log_debug(" DECTOR_RESOLUTION_1280X1024  \n");
            if(global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_A12){
                return DECTOR_RESOLUTION_1280X1024;
            }
            else{
                return DECTOR_RESOLUTION_640X512;
            }
            
    }
    else {
        return DECTOR_RESOLUTION_UNKNOWN;
    }

}


ts_u08 get_powerup_temp_drift_comp_mode(void) 
{
    return global_sys_ctrl_info.m_powerup_temp_drift_comp_mode;
}

ts_void set_powerup_temp_drift_comp_mode(ts_u08 tdc_mode)
{
    global_sys_ctrl_info.m_powerup_temp_drift_comp_mode = tdc_mode;
}

ts_u08 get_powerup_board_port_type(void) 
{
    return global_sys_ctrl_info.m_powerup_board_port_type;
}

ts_void set_powerup_board_port_type(void)
{
    global_sys_ctrl_info.m_powerup_board_port_type = get_board_port_type();
}


ts_u08 get_powerup_dector_name_type(void) 
{
    return global_sys_ctrl_info.m_powerup_det_name_type;
}

ts_void set_powerup_dector_name_type(void)
{
    global_sys_ctrl_info.m_powerup_det_name_type = get_dector_name_type();
}


ts_u08 get_powerup_dector_resolution_type(void) 
{
    return global_sys_ctrl_info.m_powerup_det_res_type;
}

ts_void set_powerup_dector_resolution_type(void)
{
    global_sys_ctrl_info.m_powerup_det_res_type = get_dector_resolution_type();
}

dector_resolution_type_t get_dector_resolution_type(void) 
{
    // vision_log_debug(" RESOLUTION_TYPE %s   \n",RESOLUTION_TYPE);
    if (strcmp(RESOLUTION_TYPE, "RES_640_512") == 0) {
        return DECTOR_RESOLUTION_640X512;
    } 
    else if (strcmp(RESOLUTION_TYPE, "RES_384_288") == 0) {
        return DECTOR_RESOLUTION_384X288;
    }
    else if (strcmp(RESOLUTION_TYPE, "RES_1280_1024") == 0) {
        return DECTOR_RESOLUTION_1280X1024;
    }
    else if (strcmp(RESOLUTION_TYPE, "RES_MANUAL") == 0) {
        return get_dector_resolution_type_with_manual();
    }
    else {
        return DECTOR_RESOLUTION_UNKNOWN;
    }
}


ts_u08 get_customer_type_with_manual(void) 
{
    // vision_log_debug(" SystemParaMenu2.m_force_customer_id %d   \n",SystemParaMenu2.m_force_customer_id);

    if(SystemParaMenu2.m_force_customer_id==CUSTOMER_TYPE_STANDARD ){
        // vision_log_debug(" CUSTOMER_TYPE_STANDARD  \n");
        return CUSTOMER_TYPE_STANDARD;   
    }
    else if(SystemParaMenu2.m_force_customer_id==CUSTOMER_TYPE_GENERAL ){
        // vision_log_debug(" CUSTOMER_TYPE_GENERAL  \n");
        return CUSTOMER_TYPE_GENERAL;   
    }
    else if(SystemParaMenu2.m_force_customer_id==CUSTOMER_TYPE_DSW ){
        // vision_log_debug(" CUSTOMER_TYPE_DSW  \n");
        return CUSTOMER_TYPE_DSW;   
    }
    else if(SystemParaMenu2.m_force_customer_id==CUSTOMER_TYPE_ZM ){
        return CUSTOMER_TYPE_ZM;   //卓目客户定制
    }
    else if(SystemParaMenu2.m_force_customer_id==CUSTOMER_TYPE_HAND ){
        return CUSTOMER_TYPE_HAND;   //手持机定制项目，用于384电子变倍768
    }
    else {
        return CUSTOMER_TYPE_STANDARD;
    }

}


ts_u08 get_customer_type(void) 
{
    // vision_log_warn(" CUSTOMER_TYPE %s   \n",CUSTOMER_TYPE);
    if (strcmp(CUSTOMER_TYPE, "STANDARD") == 0) {
        return CUSTOMER_TYPE_STANDARD;
    } 
    else if (strcmp(CUSTOMER_TYPE, "CTYPE_DSW") == 0) {
        return CUSTOMER_TYPE_DSW;
    } 
    else if (strcmp(CUSTOMER_TYPE, "MANUAL") == 0) {
        return get_customer_type_with_manual() ;
    }
    else {
        return CUSTOMER_TYPE_STANDARD;
    }
}


ts_u08 get_focus_type_with_manual(void) 
{
    // vision_log_debug(" SystemParaMenu2.m_force_focus_type %d   \n",SystemParaMenu2.m_force_focus_type);

    if(SystemParaMenu2.m_force_focus_type==FOCUS_TYPE_PRIME_LENS ){
        // vision_log_debug(" FOCUS_TYPE_PRIME_LENS  \n");
        return FOCUS_TYPE_PRIME_LENS;   
    }
    else if(SystemParaMenu2.m_force_focus_type==FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER ){
        // vision_log_debug(" FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER  \n");
        return FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER;   
    }
    else if(SystemParaMenu2.m_force_focus_type==FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER ){
        // vision_log_debug(" FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER  \n");
        return FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER;   
    }
    else if(SystemParaMenu2.m_force_focus_type==FOCUS_TYPE_FOCUS_LENS_ENCODER ){
        // vision_log_debug(" FOCUS_TYPE_FOCUS_LENS_ENCODER  \n");
        return FOCUS_TYPE_FOCUS_LENS_ENCODER;   
    }
    else if(SystemParaMenu2.m_force_focus_type==FOCUS_TYPE_ZOOM_LENS_ENCODER ){
        // vision_log_debug(" FOCUS_TYPE_ZOOM_LENS_ENCODER  \n");
        return FOCUS_TYPE_ZOOM_LENS_ENCODER;   
    }
    else {
        return FOCUS_TYPE_PRIME_LENS;
    }

}

ts_u08 get_focus_type(void) 
{
    // vision_log_warn(" FOCUS_TYPE %s   \n",FOCUS_TYPE);
    if (strcmp(FOCUS_TYPE, "PRIME_LENS_TYPE") == 0) {
        return FOCUS_TYPE_PRIME_LENS;
    } 
    else if (strcmp(FOCUS_TYPE, "FOCUS_LENS_POTENTIOMETER") == 0) {
        return FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER;
    } 
    else if (strcmp(FOCUS_TYPE, "ZOOM_LENS_POTENTIOMETER") == 0) {
        return FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER;
    } 
    else if (strcmp(FOCUS_TYPE, "FOCUS_LENS_ENCODER") == 0) {
        return FOCUS_TYPE_FOCUS_LENS_ENCODER;
    } 
    else if (strcmp(FOCUS_TYPE, "ZOOM_LENS_ENCODER") == 0) {
        return FOCUS_TYPE_ZOOM_LENS_ENCODER;
    } 
    else if (strcmp(FOCUS_TYPE, "MANUAL") == 0) {
        return get_focus_type_with_manual() ;
    }
    else {
        return FOCUS_TYPE_PRIME_LENS;
    }
}


ts_u08 get_function_type_with_manual(void) 
{
    vision_log_debug(" SystemParaMenu2.m_force_function_type %d   \n",SystemParaMenu2.m_force_function_type);

    if(SystemParaMenu2.m_force_function_type==FUNCTION_TYPE_INFRARED_IMAGING ){
        vision_log_debug(" FUNCTION_TYPE_INFRARED_IMAGING  \n");
        return FUNCTION_TYPE_INFRARED_IMAGING;   
    }
    else if(SystemParaMenu2.m_force_function_type==FUNCTION_TYPE_INFRARED_THERMOMETRY ){
        vision_log_debug(" FUNCTION_TYPE_INFRARED_THERMOMETRY  \n");
        return FUNCTION_TYPE_INFRARED_THERMOMETRY;   
    }
    else {
        return FUNCTION_TYPE_INFRARED_IMAGING;
    }

}

ts_u08 get_function_type(void) 
{
    vision_log_warn(" FUNCTION_TYPE %s   \n",FUNCTION_TYPE);
    if (strcmp(FUNCTION_TYPE, "INFRARED_IMAGING_TYPE") == 0) {
        return FUNCTION_TYPE_INFRARED_IMAGING;
    } 
    else if (strcmp(FUNCTION_TYPE, "INFRARED_THERMOMETRY_TYPE") == 0) {
        return FUNCTION_TYPE_INFRARED_THERMOMETRY;
    } 
    else if (strcmp(FUNCTION_TYPE, "MANUAL") == 0) {
        return get_function_type_with_manual() ;
    }
    else {
        return FUNCTION_TYPE_INFRARED_IMAGING;
    }
}


ts_void show_dector_name_type(void) 
{
    if(DECTOR_TYPE_VOT_WLP==get_dector_name_type() ){
        vision_log_debug(" DECTOR_TYPE_VOT_WLP  \n");
    }
    else  if(DECTOR_TYPE_VOT==get_dector_name_type() ){
        vision_log_debug(" DECTOR_TYPE_VOT  \n");
    }
    else  if(DECTOR_TYPE_RTD==get_dector_name_type() ){
        vision_log_debug(" DECTOR_TYPE_RTD  \n");
    }
}


ts_void show_dector_resolution_type(void) 
{
    if(DECTOR_RESOLUTION_640X512==get_dector_resolution_type() ){
        vision_log_debug(" DECTOR_RESOLUTION_640X512  \n");
    }
    else  if(DECTOR_RESOLUTION_384X288==get_dector_resolution_type() ){
        vision_log_debug(" DECTOR_RESOLUTION_384X288  \n");
    }
    else  if(DECTOR_RESOLUTION_1280X1024==get_dector_resolution_type() ){
        vision_log_debug(" DECTOR_RESOLUTION_1280X1024  \n");
    }
}

ts_u08 is_dector_type_manual(void) 
{
    vision_log_debug(" DETECTOR_TYPE_NAME %s   \n",DETECTOR_TYPE_NAME);
    vision_log_debug(" RESOLUTION_TYPE %s   \n",RESOLUTION_TYPE);
    if ( strcmp(DETECTOR_TYPE_NAME, "MANUAL")==0 && strcmp(RESOLUTION_TYPE, "RES_MANUAL") == 0 ) {
        return 1;
    } 

    return 0;
}

ts_u08 is_hardware_port_type_manual(void) 
{
    vision_log_debug(" HARD_BOARD_PORT_TYPE %s   \n",HARD_BOARD_PORT_TYPE);
    if ( strcmp(HARD_BOARD_PORT_TYPE, "MANUAL")==0 ) {
        return 1;
    } 

    return 0;
}


ts_u08 is_customer_type_manual(void) 
{
    vision_log_debug(" CUSTOMER_TYPE %s   \n",CUSTOMER_TYPE);
    if ( strcmp(CUSTOMER_TYPE, "MANUAL")==0 ) {
        return 1;
    } 

    return 0;
}

ts_u08 is_focus_type_manual(void) 
{
    vision_log_debug(" FOCUS_TYPE %s   \n",FOCUS_TYPE);
    if ( strcmp(FOCUS_TYPE, "MANUAL")==0 ) {
        return 1;
    } 

    return 0;
}

ts_u08 is_function_type_manual(void) 
{
    vision_log_debug(" FUNCTION_TYPE %s   \n",FUNCTION_TYPE);
    if ( strcmp(FUNCTION_TYPE, "MANUAL")==0 ) {
        return 1;
    } 

    return 0;
}

// 根据产品名称字符串获取对应的项目类型枚举值
project_name_type_t get_project_name_type(void) 
{
    vision_log_debug(" PRODUCTION_NAME %s   \n",PRODUCTION_NAME);
    if (strcmp(PRODUCTION_NAME, "UAD3") == 0) {
        return PROJECT_NAME_TYPE_UAD3;
    } else if (strcmp(PRODUCTION_NAME, "WLP") == 0) {
        return PROJECT_NAME_TYPE_WLP;
    }
    else {
        return PROJECT_NAME_TYPE_UNKNOWN;
    }
}


ts_void planck640c_stop(ts_void)
{
    #ifdef CONFIG_SNR_PLANCK1280C
      if(get_powerup_board_port_type() == BOARD_PORT_UAD3_LITE || get_powerup_board_port_type() == BOARD_PORT_UAW_SE_LITE)
      {
        snr_gpio_cfg2out(5, TS_ELEC_LVL_LO); //VDET_CTRL set to high VDET_EN2
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(4, TS_ELEC_LVL_LO);//VDET_CTRL set to high VDET_EN1
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(25, TS_ELEC_LVL_LO);//LDO set to high DVDD POWER_ON
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);//LDO set to high AVDD
    }
    else  if(get_powerup_board_port_type() == BOARD_PORT_UAD3){
        snr_gpio_cfg2out(31, TS_ELEC_LVL_LO); //VDET_CTRL set to high VDET_EN2
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(30, TS_ELEC_LVL_LO);//VDET_CTRL set to high VDET_EN1
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(25, TS_ELEC_LVL_LO);//LDO set to high DVDD POWER_ON
        ts_cpl_msleep(50);
        snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);//LDO set to high AVDD
    }
    else  if(get_powerup_board_port_type() == BOARD_PORT_UAW){

    }

    #else
    if(get_powerup_board_port_type() == BOARD_PORT_UAD3_LITE || get_powerup_board_port_type() == BOARD_PORT_UAW_SE_LITE){
        // VDET_EN2
        snr_gpio_cfg2out(46, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

        //VDET EN1
        snr_gpio_cfg2out(42, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

        //AVDD
        snr_gpio_cfg2out(41, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);
    }
    else  if(get_powerup_board_port_type() == BOARD_PORT_UAD3){
        //VDET_CTRL set to low VDET_EN2
        snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

        //AVDD&VDET EN1
        snr_gpio_cfg2out(21, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);
    }
    else  if(get_powerup_board_port_type() == BOARD_PORT_UAW){

        snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

        //VDET EN1
        snr_gpio_cfg2out(22, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

        //AVDD
        snr_gpio_cfg2out(21, TS_ELEC_LVL_LO);
        ts_cpl_msleep(50);

    }
    #endif
    //停止喂狗，系统将自动重启
    ISP_set_feeding_flag(0);

}


ts_s32 planck640c_vpo_freeze(ts_void)
{
	ts_s32 s32_ret=0;

	s32_ret = ts_tdk_vpo_freeze(TS_VPO_MDEV0);
	return s32_ret;
}

ts_s32 planck640c_vpo_unfreeze(ts_void)
{
	ts_s32 s32_ret=0;
	s32_ret = ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	return s32_ret;
}
//光智640探测器Det参数408bit写

ts_void planck640c_write_gain_adc(ts_u32 u32_gain_adc)
{//adc
    vot_detetor_write_sd(335,329,u32_gain_adc);
}


/**
 * @description: 配置探测器plk640_9c1的调试模式
 * @param { u32_flag 调试模式值 }
 * @return {ts_void}
 */
ts_void planck640c_write_gain_protect_flag(ts_u32 u32_flag)
{//gain_protect_flag

    vot_detetor_write_sd(392,388,u32_flag);
}

ts_void planck640c_write_ndac(ts_u32 u32_ndac)
{//ndac
    vot_detetor_write_sd(223,211,u32_ndac);
}

ts_void planck640c_write_gof(ts_u32 u32_gof)
{//gof
    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122
        ){
        //gof 其他值，需要设置 gain_protect_flag 为 0x04,非调试模式
        //gof 99 时，需要设置 gain_protect_flag 为 0x13，调试模式，tes值才会变化
        if(u32_gof==99){
            ts_u32 u32_flag = 0x13;
            planck640c_write_gain_protect_flag(u32_flag);
        }
        else {
            ts_u32 u32_flag = planck640c_read_gain_protect_flag();
            if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_C122
                || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122)
            {
                if(u32_flag!=0x01){
                    u32_flag = 0x01;
                    planck640c_write_gain_protect_flag(u32_flag);
                }
            }
            else{
                if(u32_flag!=0x04){
                    u32_flag = 0x04;
                    planck640c_write_gain_protect_flag(u32_flag);
                }
            }
            
        }
    }
    vot_detetor_write_sd(202,196,u32_gof);

}

ts_void planck640c_write_gos(ts_u32 u32_gos)
{//sfb

    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
        ){
        return vot_detetor_write_sd(10,5, u32_gos);
    }
    else{
        vot_detetor_write_sd(195,190,u32_gos);
    }
}

ts_void planck640c_write_cint(ts_u32 u32_cint)
{//int_cap
    vot_detetor_write_sd(188,186,u32_cint);
}

ts_void planck640c_write_rst_data(ts_u32 u32_rst_data)
{//rst_time
    vot_detetor_write_sd(182,176,u32_rst_data);
}

ts_void planck640c_write_tint(ts_u32 u32_tint)
{//int_time
    vot_detetor_write_sd(175,169,u32_tint);
}

ts_void planck640c_write_extdata(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_gsp)
{
    vot_detetor_write_sd(u32_msb,u32_lsb,u32_gsp);
}

ts_u32 planck640c_read_extdata(ts_u32 u32_msb,ts_u32 u32_lsb)
{
    return vot_detetor_read_sd(u32_msb,u32_lsb);
}

ts_u32 planck640c_read_gain_adc(ts_void)
{//adc
    return vot_detetor_read_sd(335,329);
}

/**
 * @description: 读取探测器plk640_9C1的调试模式
 * @param {void}
 * @return {ts_u32} 返回调试模式值
 */
ts_u32 planck640c_read_gain_protect_flag(ts_void)
{//gain_protect_flag
    return vot_detetor_read_sd(392,388);
}

ts_u32 planck640c_read_ndac(ts_void)
{//ndac
    return vot_detetor_read_sd(223,211);
}

ts_u32 planck640c_read_gof(ts_void)
{//gof
    return vot_detetor_read_sd(202,196);
}

ts_u32 planck640c_read_gos(ts_void)
{//sfb

    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1
        || SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288_9C1
        ){
        return vot_detetor_read_sd(10,5);
    }
    return vot_detetor_read_sd(195,190);
}

ts_u32 planck640c_read_cint(ts_void)
{//int_cap
    return vot_detetor_read_sd(188,186);
}

ts_u32 planck640c_read_rst_data(ts_void)
{//rst_time
    return vot_detetor_read_sd(182,176);
}

ts_u32 planck640c_read_tint(ts_void)
{//int_time
    return vot_detetor_read_sd(175,169);
}

ts_u16 planck640c_get_tes(ts_void)
{
    ts_u32 u32_fv=0;
    // ts_u16  u16_fv=0;
    ts_s32 s32_ret;
    ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    //此处需要调用两次ts_tdk_viu_get_stats，才能获取稳定的tes值，具体原因需要进一步分析
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 0x28f, 0, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    // vision_log_debug("first test 0 u32_mean  0x%X  \n",u32_mean);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 0x28f, 0, (ts_u32 *)&u32_fv, &u32_mean);
    if (TS_OK == s32_ret) {
        u32_mean = u32_mean & 0x3fff;
        // vision_log_debug("planck640c_get_tes u32_mean 0x%X \n", u32_mean);
    } else {
        u32_mean = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    return u32_mean;
}


ts_u16 planck384c_get_tes(ts_void)
{
    ts_u32 u32_fv=0;
    // ts_u16  u16_fv=0;
    ts_s32 s32_ret;
    ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    //此处需要调用两次ts_tdk_viu_get_stats，才能获取稳定的tes值，具体原因需要进一步分析
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 399, 0, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    // vision_log_debug("first test 0 u32_mean  0x%X  \n",u32_mean);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 399, 0, (ts_u32 *)&u32_fv, &u32_mean);
    if (TS_OK == s32_ret) {
        u32_mean = u32_mean & 0x3fff;
        // vision_log_debug("planck640c_get_tes u32_mean 0x%X \n", u32_mean);
    } else {
        u32_mean = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    return u32_mean;
}



ts_s32 planck640c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 0x28e, 200, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    if (TS_OK == s32_ret) {
        
        f32_fv = (7416.0f-u32_mean)/ (33.0f);
      
    } else {
        u32_mean = 0;
    }

    if(ad_value){
        *ad_value = u32_mean;
    }

    if(temp_value){
        *temp_value = f32_fv;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    // vision_log_debug("planck640c_get_temperature_info u32_mean 0x%x  f32_fv [%f]\n",u32_mean,f32_fv);

    return s32_ret;
}

ts_s32 planck384c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 398, 0, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    if (TS_OK == s32_ret) {
        
        f32_fv = (7416.0f-u32_mean)/ (33.0f);
      
    } else {
        u32_mean = 0;
    }

    if(ad_value){
        *ad_value = u32_mean;
    }

    if(temp_value){
        *temp_value = f32_fv;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    // vision_log_debug("planck640c_get_temperature_info u32_mean 0x%x  f32_fv [%f]\n",u32_mean,f32_fv);

    return s32_ret;
}



/**
 * @description: 配置探测器plk640的408bit，输入的配置bit407-bit0， 高位在前，低位在后。
 * @param {ts_u08*} config_408bit
 * @return {*}
 */
ts_void planck640c_write_config_408bit(ts_u08* config_408bit)
{
    ts_u32 u32_value=0x0;
    // ts_u08* data = config_408bit;
    ts_u08 data[VOT_RESERVED_DATA_LENGTH+2];

    vision_log_debug("enter: planck640c_write_config_408bit ");
    if(config_408bit==NULL){
        return;
    }
    
    int i=0;
    int index=VOT_RESERVED_DATA_LENGTH-1;
    for(i=0; i<VOT_RESERVED_DATA_LENGTH; i++){
        data[i]=config_408bit[index];
        index--;
    }

    vision_log_debug("index: %d i %d planck640c_write_config_408bit ", index, i);

    u32_value=(data[3]<<24)+(data[2]<<16)+(data[1]<<8)+data[0];
    vot_detetor_write_sd(31,0,u32_value); //bit0 - bit31

    u32_value=(data[7]<<24)+(data[6]<<16)+(data[5]<<8)+data[4];
    vot_detetor_write_sd(63,32,u32_value); //bit32 - bit63

	u32_value = (data[11]<<24)+(data[10]<<16)+(data[9]<<8)+data[8];//bit64 - bit95
    vot_detetor_write_sd(95,64,u32_value); 

	u32_value = (data[15]<<24)+(data[14]<<16)+(data[13]<<8)+data[12];//bit96 - bit127
    vot_detetor_write_sd(127,96,u32_value); 

	u32_value = (data[19]<<24)+(data[18]<<16)+(data[17]<<8)+data[16];//bit128 - bit159
    vot_detetor_write_sd(159,128,u32_value); 

    u32_value = (u32_value&0x00000000) + ((data[21]&0x01)<<8) + data[20];//bit168+bit167~bit160
    vot_detetor_write_sd(168,160,u32_value); 
	
	u32_value = (u32_value&0x00000000) + (((data[22]&0x80)>>7)<<18);//bit183
    vot_detetor_write_sd(183,183,u32_value); 

	u32_value = (u32_value&0x00000000) + (((data[23]&0x20)>>5)<<17);//bit189
    vot_detetor_write_sd(189,189,u32_value); 

	u32_value = (u32_value&0x00000000) + ((((data[25])>>3) + ((data[26])<<5))<<24);//bit203 - bit210
    vot_detetor_write_sd(210,203,u32_value); 

	u32_value = (u32_value&0x00000000) + (((data[30]<<16)+(data[29]<<8)+data[28])<<8);//bit224 - bit247
    vot_detetor_write_sd(247,224,u32_value); 

	u32_value = (data[34]<<24)+(data[33]<<16)+(data[32]<<8)+data[31];//bit248 - bit279
    vot_detetor_write_sd(279,248,u32_value); 

	u32_value = (data[38]<<24)+(data[37]<<16)+(data[36]<<8)+data[35];//bit280 - bit311
    vot_detetor_write_sd(311,280,u32_value); 
	
    u32_value = (u32_value&0x00000000) +((data[41]&0x01)<<16) + (data[40]<<8) + data[39];//bit328 + (bit312 - bit327)
    vot_detetor_write_sd(328,312,u32_value); 
	
	u32_value = (data[45]<<24)+(data[44]<<16)+(data[43]<<8)+data[42];//bit336 - bit367
	vot_detetor_write_sd(367,336,u32_value); 

   	u32_value = (data[49]<<24)+(data[48]<<16)+(data[47]<<8)+data[46];//bit368 - bit399
	vot_detetor_write_sd(399,368,u32_value); 

	//bit400 - bit407
    u32_value = (u32_value&0x00000000)+data[50];
    vot_detetor_write_sd(407,400,u32_value); 

	// APB3_REGW(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44,((APB3_REGR(IO_APB_SLAVE_0_INPUT_VOT_DETECTOR_START,44)&0xffff3fff)|(0x0000c000)));//OOC_SEL

    SystemParaMenu2.GAIN = (data[41]>>1);
	SystemParaMenu2.NDAC_L = (data[26]>>3); //bit211-bit215
	SystemParaMenu2.NDAC_H = data[27]; //bit216-bit223
	
    SystemParaMenu2.GOF = (data[24]>>4)+((data[25]&0x07)<<4);
    SystemParaMenu2.OOC_Par = (data[23]>>6)+((data[24]&0x0f)<<2);
    SystemParaMenu2.INT_C = ((data[23]&0x1f)>>2);
    SystemParaMenu2.INT_T = (data[21]>>1);


    vision_log_debug("leave planck640c_write_config_408bit ");

}

extern ts_viu_snr_attr_s_t sst_vot_planck640_attr;
extern ts_viu_snr_cfg_s_t sast_planck640_vot_cfg;

extern ts_viu_snr_attr_s_t sst_vot_planck640_9c1_attr;
extern ts_viu_snr_cfg_s_t sast_planck640_9c1_vot_cfg;

extern ts_viu_snr_attr_s_t sst_vot_planck640_c122_attr;
extern ts_viu_snr_cfg_s_t sast_planck640_c122_vot_cfg;

ts_void vot_planck640c_write_config_408bit(ts_u08* config_408bit)
{
    ts_u32 *u32_value=NULL;
    // ts_u08* data = config_408bit;
    ts_u08 data[VOT_RESERVED_DATA_LENGTH+1];
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;

    pst_ctx =&gst_snr_ctx;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;

    // vision_log_debug("enter:  ");
    if(config_408bit==NULL){
        return;
    }
    
    int i=0;
    int index=VOT_RESERVED_DATA_LENGTH-1;
    data[0] = 0x0;
    for(i=1; i<VOT_RESERVED_DATA_LENGTH+1; i++){
        data[i]=config_408bit[index];
        // vision_log_debug("index: %d i %d data[i] %x \n ", index, i,data[i]);
        index--;
    }

    u32_value =(ts_u32*) &data[0];
    for(i=0; i<13; i++){
        vision_log_debug(" i %d u32_value[i] %x \n ",  i,u32_value[i]);
    }

    
    for(index=0; index<13; index++){
        dector_type_t dector_type = get_dector_name_type();
        if(dector_type!=DECTOR_TYPE_RTD){
            if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1 ){
                sst_vot_planck640_9c1_attr.pst_cfg[index].u32_val = u32_value[12-index];

            }
            else if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122){
                sst_vot_planck640_c122_attr.pst_cfg[index].u32_val = u32_value[12-index];   
            }
            else{
                sst_vot_planck640_attr.pst_cfg[index].u32_val = u32_value[12-index];
            }

        }
    
        // vision_log_debug(" index %d u32_value %x \n ",  index,u32_value[12-index]);
    }
    ts_tdk_viu_disable(TS_VIU_MDEV0);

    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1 ){
        planck640c_viu_init(pst_attr, &sst_vot_planck640_9c1_attr);
    }
    else  if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122){
        planck640c_viu_init(pst_attr, &sst_vot_planck640_c122_attr);
    }
    else{
        planck640c_viu_init(pst_attr, &sst_vot_planck640_attr);
    }

    ts_tdk_viu_enable(TS_VIU_MDEV0);

    // vision_log_debug("leave:  ");
}

ts_void planck640c_read_config_408bit(ts_u08* config_408bit, ts_u08 DataLength)
{
    int byte_cnt=0;
    ts_u32 u32_value;
    ts_u32 u32_msb, u32_lsb;

    for(byte_cnt=0; byte_cnt<DataLength; byte_cnt++){
        u32_msb = (byte_cnt+1)*8;
        u32_lsb = byte_cnt*8;
        u32_value = vot_detetor_read_sd( u32_msb, u32_lsb);
        config_408bit[byte_cnt] = u32_value&0xff;
    }

}


ts_void brd_planck640_vot_init(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

    board_type_t board_name =  get_board_port_type();
    FOCUS_TYPE_ENUM focusType = get_focus_type();

    vision_log_debug(" board_name %d, focusType %d \n",board_name, focusType);

    if(board_name == BOARD_PORT_UAD3_LITE){

        vision_log_debug(" normal 640  2 board \n");
        /* Sensor misc io mux */
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT5, TS_IO_SR_OUT_DAT5_FUN3_GPIO44);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT2, TS_IO_SR_OUT_DAT2_FUN3_GPIO41);//AVDD_EN
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT3, TS_IO_SR_OUT_DAT3_FUN3_GPIO42);//VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT4, TS_IO_SR_OUT_DAT4_FUN3_GPIO43);//Motor_EN
        if (get_customer_type() == CUSTOMER_TYPE_ZM )//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
        {
            if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)
            {
                ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN0_SR_SEQ_SYNC_I);//功能复用成TS_IO_SR_SEQ_SYNC_FUN0_SR_SEQ_SYNC_I，VI配置成外同步触发方式。
            }
            ts_lib_gpio_set_mux(TS_IO_SR_IN_DAT10, TS_IO_SR_IN_DAT10_FUN3_GPIO33);//POWER_ON //VDET_EN2, 用SR_CAL_DAT7替换SR_SEQ_SYNC在前置板上的功能
        }
        else
        {
            ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//POWER_ON //VDET_EN2
        }

        //用于自动调焦相关功能
        ts_lib_gpio_set_mux(TS_IO_PWM0, TS_IO_PWM0_FUN0);
        ts_lib_gpio_set_mux(TS_IO_PWM1, TS_IO_PWM1_FUN0);

        if(focusType == FOCUS_TYPE_ZOOM_LENS_ENCODER){
        ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN2_PA0_PWM);
        ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN2_PB0_PWM);
        /* 变倍电机*/
        ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
        ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
        ts_lib_gpio_set_mux(TS_IO_GPIO60,TS_IO_GPIO60_FUN2_PA1_PWM);
        ts_lib_gpio_set_mux(TS_IO_GPIO61,TS_IO_GPIO61_FUN2_PB1_PWM);
        }else if(focusType == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
            if(SystemParaMenu2.ADDR_Sel == 0 || SystemParaMenu2.ADDR_Sel == 1)    
            {
                ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN3_PWM2);
                ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN3_PWM3);
            }
            else
            {
                ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
                ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
            }   
        }
        //根据以往的经验，探测器上电必须要先使能3.3v，再是能11v，否则，探测器可能出现问题
        //AVDD
        snr_gpio_cfg2out(41, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);
        //VDET EN1
        snr_gpio_cfg2out(42, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);

        //VDET_CTRL set to high VDET_EN2, 观测板：需要拉高，电压11V
        // 高温三档测温：需要拉低，电压会5.6v
        if (get_customer_type() == CUSTOMER_TYPE_ZM )//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
        {
           if(SystemParaMenu2.Usermenu_SW <= 1)
            snr_gpio_cfg2out(33, TS_ELEC_LVL_HI);
            else
            snr_gpio_cfg2out(33, TS_ELEC_LVL_LO);//POWER_ON //VDET_EN2, 用SR_CAL_DAT7替换SR_SEQ_SYNC在前置板上的功能
        }
        else
        {
            if(SystemParaMenu2.Usermenu_SW <= 1)
            snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);
            else
            snr_gpio_cfg2out(46, TS_ELEC_LVL_LO);
        }
       
        
        // ts_cpl_msleep(100);

        //Motor_EN
        snr_gpio_cfg2out(43, TS_ELEC_LVL_HI);

        /*******************
        ** IO DRIVER LEVEL
        *******************/
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
      //ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);
        if (get_customer_type() == CUSTOMER_TYPE_ZM )
        {
             ts_lib_gpio_set_o_strong(TS_IO_SR_IN_DAT10, 3);
        }
        else
        {
            ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);
        }
       

        /* 配置探测器复位、电源、开门的 IO 控制信息 */
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        st_snr_io_cfg.s32_rst_gpio_num = -1;
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;

        st_snr_io_cfg.s32_sht_p_gpio_num = 45;
        st_snr_io_cfg.s32_sht_n_gpio_num = 44;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;
    }
    else if( get_board_port_type()  == BOARD_PORT_UAD3){

        vision_log_debug(" normal 640  3 board\n");
        /* Sensor misc io mux */
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SPI1_CSN0, TS_IO_SPI1_CSN0_FUN3_GPIO24);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//AVDD_EN/VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//VDET_EN2
        ts_lib_gpio_set_mux(TS_IO_SPI1_MOSI, TS_IO_SPI1_MOSI_FUN3_GPIO22);//Motor_EN
       if (get_customer_type() == CUSTOMER_TYPE_ZM)//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
        {
            if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)
            {
                ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN0_SR_SEQ_SYNC_I);//功能复用成TS_IO_SR_SEQ_SYNC_FUN0_SR_SEQ_SYNC_I，VI配置成外同步触发方式。
            }
            ts_lib_gpio_set_mux(TS_IO_SR_IN_DAT10, TS_IO_SR_IN_DAT10_FUN3_GPIO33);//POWER_ON, 用SR_CAL_DAT7替换SR_SEQ_SYNC在前置板上的功能
        }
        else
        {
            ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//POWER_ON 
        }

        //用于自动调焦相关功能
        ts_lib_gpio_set_mux(TS_IO_PWM0, TS_IO_PWM0_FUN0);
        ts_lib_gpio_set_mux(TS_IO_PWM1, TS_IO_PWM1_FUN0);

        if(focusType == FOCUS_TYPE_ZOOM_LENS_ENCODER){
        ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN2_PA0_PWM);
        ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN2_PB0_PWM);
        /* 变倍电机*/
        ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
        ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
        ts_lib_gpio_set_mux(TS_IO_GPIO60,TS_IO_GPIO60_FUN2_PA1_PWM);
        ts_lib_gpio_set_mux(TS_IO_GPIO61,TS_IO_GPIO61_FUN2_PB1_PWM);
        }else if(focusType == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
            if(SystemParaMenu2.ADDR_Sel == 0 || SystemParaMenu2.ADDR_Sel == 1)    
            {
                ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN3_PWM2);
                ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN3_PWM3);
            }
            else
            {
                ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
                ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
            }
    }
        //根据以往的经验，探测器上电必须要先使能3.3v，再是能11v，否则，探测器可能出现问题
        //AVDD&VDET EN1
        snr_gpio_cfg2out(21, TS_ELEC_LVL_HI);

        // ts_cpl_msleep(100);
        
        //VDET_CTRL set to high VDET_EN2
        if(SystemParaMenu2.Usermenu_SW <= 1)
            snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);
        else
            snr_gpio_cfg2out(23, TS_ELEC_LVL_LO);

        // ts_cpl_msleep(100);

        snr_gpio_cfg2out(22, TS_ELEC_LVL_HI);

        /*******************
        ** IO DRIVER LEVEL
        *******************/
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
        //ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);
        if (get_customer_type() == CUSTOMER_TYPE_ZM)
        {
            ts_lib_gpio_set_o_strong(TS_IO_SR_IN_DAT10, 3);
        }
        else
        {
            ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);
        }
        /* 配置探测器复位、电源、开门的 IO 控制信息 */
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        if (get_customer_type() == CUSTOMER_TYPE_ZM )//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
        {
            st_snr_io_cfg.s32_rst_gpio_num = 33;
        }
        else
        {
            st_snr_io_cfg.s32_rst_gpio_num = 46;
        }
        
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;

        st_snr_io_cfg.s32_sht_p_gpio_num = 45;
        st_snr_io_cfg.s32_sht_n_gpio_num = 24;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;
    }
    else if(get_board_port_type() == BOARD_PORT_UAW) {//wlp
        //wlp640
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT5, TS_IO_SR_OUT_DAT5_FUN3_GPIO44);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//AVDD_EN
        ts_lib_gpio_set_mux(TS_IO_SPI1_MOSI, TS_IO_SPI1_MOSI_FUN3_GPIO22);//VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//VDET_EN2
        ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//POWER_ON
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT4, TS_IO_SR_OUT_DAT4_FUN3_GPIO43);//Motor_EN
        snr_gpio_cfg2out(21, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);
        snr_gpio_cfg2out(22, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);
        snr_gpio_cfg2out(43, TS_ELEC_LVL_HI);
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);

        /* 配置探测器复位、电源、开门的 IO 控制信息 */
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        st_snr_io_cfg.s32_rst_gpio_num = 46;
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;

        st_snr_io_cfg.s32_sht_p_gpio_num = 44;
        st_snr_io_cfg.s32_sht_n_gpio_num = 45;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;
    }else if(get_board_port_type() == BOARD_PORT_UAW_SE_LITE) {
            //wlp 17*17 陶封机芯
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT5, TS_IO_SR_OUT_DAT5_FUN3_GPIO44);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT2, TS_IO_SR_OUT_DAT2_FUN3_GPIO41);//AVDD_EN
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT3, TS_IO_SR_OUT_DAT3_FUN3_GPIO42);//VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//VDET_EN2
        ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT4, TS_IO_SR_OUT_DAT4_FUN3_GPIO43);//Motor_EN
        snr_gpio_cfg2out(41, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);
        snr_gpio_cfg2out(42, TS_ELEC_LVL_HI);
        snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);
        // ts_cpl_msleep(100);
        snr_gpio_cfg2out(43, TS_ELEC_LVL_HI);
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        st_snr_io_cfg.s32_rst_gpio_num = -1;
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;
        st_snr_io_cfg.s32_sht_p_gpio_num = 44;
        st_snr_io_cfg.s32_sht_n_gpio_num = 45;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;   
    }
    
    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();

} 

ts_void bt656_bt1120_pin_init(void)
{
    // vision_log_debug(" enter: VOT_BT656_EN \n");
	ts_lib_gpio_set_mux(TS_IO_BT1120_CLK,TS_IO_BT1120_CLK_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT0,TS_IO_BT1120_DAT0_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT1,TS_IO_BT1120_DAT1_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT2,TS_IO_BT1120_DAT2_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT3,TS_IO_BT1120_DAT3_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT4,TS_IO_BT1120_DAT4_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT5,TS_IO_BT1120_DAT5_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT6,TS_IO_BT1120_DAT6_FUN0);
    ts_lib_gpio_set_mux(TS_IO_BT1120_DAT7,TS_IO_BT1120_DAT7_FUN0);

	ts_lib_gpio_set_o_strong(TS_IO_BT1120_CLK,3);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT0,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT1,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT2,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT3,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT4,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT5,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT6,2);
	ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT7,2);
    // vision_log_debug(" leave: VOT_BT656_EN \n");
}


ts_void brd_planck640c_init_org(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
    ts_lib_gpio_set_mux(TS_IO_SPI1_CSN0, TS_IO_SPI1_CSN0_FUN3_GPIO24);//SHTCTL1
    ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//AVDD_EN/VDET_EN1
    ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//VDET_EN2
    ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//POWER_ON

    //VDET_CTRL set to high
    snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);

    //AVDD&VDET EN
    snr_gpio_cfg2out(21, TS_ELEC_LVL_HI);

    /*******************
    ** IO DRIVER LEVEL
    *******************/
    ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);

    /* 配置探测器复位、电源、开门的 IO 控制信息 */
    st_snr_io_cfg.s32_pwr_gpio_num = -1;
    st_snr_io_cfg.s32_rst_gpio_num = 46;
    st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.u32_rst_dly = 100000;

    st_snr_io_cfg.s32_sht_p_gpio_num = 45;
    st_snr_io_cfg.s32_sht_n_gpio_num = 24;
    st_snr_io_cfg.u32_sht_dbg = 1;
    st_snr_io_cfg.u32_sht_hold = 60000;

    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();
}


ts_void brd_gpio_mux_vot_init(ts_void)
{
    // brd_planck640c_init_org();
    #ifdef CONFIG_SNR_PLANCK1280C
    brd_planck1280_vot_init();
    #else
    brd_planck640_vot_init();
    #endif
}



