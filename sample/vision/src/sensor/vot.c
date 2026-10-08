/*
 * vot.c -- vision vot 对接各个探测器入口
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/aisp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/clkm_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/wdt_v100.h>
#include <uapi/base_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/isp_comm.h>
#include <uapi/sfc_v100.h>
#include <uapi/i2c_v100.h>
#include <uapi/snr_v100.h>
#include <stdio.h>
#include "api.h"
#include "snr_comm.h"
#include "./vot/vot_comm.h"
#include "menu/votmenu.h"
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
#include <snr_comm_vot.h>
#include <menu/ispfunction.h>


extern ts_void sffc_process(ts_void);

extern struct Menu2_Para SystemParaMenu2;
extern SysCtrlInfo_s_t global_sys_ctrl_info;
//VL0612VCA02507M9-中科 0001390A3734153B34530F57FDFF1D0C0C135F835DF00080826CDA0C8B8416C5B36DB61B6C410080080451333EC3A6BD1C4000
//VL0612VCB03007B5-老化 0001390A3734153B34490B57FDFF1D0C0E135F827D7000E0826CD80C8B8416C5B36DB61B6C410080080451333EC3A6BD1C4000
//VL0612VCB03807A5-低温 0001390A3734153B342CD357FDFF1D0C0C135F827D7000E0826CD8CC8B8416C5B36DB61B6C410080080451333EC3A6BD1C4000
//#define PLANCK640C_3807A5
#define PLANCK640C_101485
//#define PLANCK640C_101492
//#define PLANCK640C_101555

#ifdef PLANCK640C_101485
// ts_viu_snr_cfg_s_t sast_planck640_vot_cfg[] = {
//     {0x0000001f, 0x0001390A},//[407:376]
//     {0x0000001e, 0x3734153B},//[375:344]
//     {0x0000001d, 0x343AFB57},//[343:312]
//     {0x0000001c, 0xFDFF1D0C},//[311:280]
//     {0x0000001b, 0x0C035F82},//[279:248]
//     {0x0000001a, 0xA1700080},//[247:216]
//     {0x00000019, 0x826CD90C},//[215:184],[202:196] GOF,[18:12]
//     {0x00000018, 0x8B8416C5},//[183:152]
//     {0x00000017, 0xB36DB61B},//[151:120]
//     {0x00000016, 0x6C410080},//[119:88]
//     {0x00000015, 0x08045133},//[87:56]
//     {0x00000014, 0x3EC3A6BD},//[55:24]
//     {0x00000013, 0x1C400000},//[23:-8]
// };


ts_viu_snr_cfg_s_t sast_planck640_c122_vot_cfg[] = {
        {0x0000001f, 0x0000190A}, //407-376
        {0x0000001e, 0x3734153B}, //375-344
        {0x0000001d, 0x343B3B57}, //343-312
        {0x0000001c, 0xFDFF1D0C}, //311-280
        {0x0000001b, 0x0C035F83}, //279-248
        {0x0000001a, 0x5D701FF0}, //247-216
        {0x00000019, 0x826CD90C}, //215-184
        {0x00000018, 0x8B8416C5}, //183-152
        {0x00000017, 0xB36DB61B}, //151-120
        {0x00000016, 0x6C410080}, //119-88
        {0x00000015, 0x08045133}, //87-56
        {0x00000014, 0x3EC3A6BD}, //55-24
        {0x00000013, 0x1C481000}, //23:-8
};

ts_viu_snr_cfg_s_t sast_planck640_9c1_vot_cfg[] = {
        {0x0000001f, 0x0000490A}, //407-376
        {0x0000001e, 0x3734153B}, //375-344
        {0x0000001d, 0x343B3B57}, //343-312
        {0x0000001c, 0xFDFF1D0C}, //311-280
        {0x0000001b, 0x0C035F83}, //279-248
        {0x0000001a, 0x5D701FF0}, //247-216
        {0x00000019, 0x826CD90C}, //215-184
        {0x00000018, 0x8B8416C5}, //183-152
        {0x00000017, 0xB36DB61B}, //151-120
        {0x00000016, 0x6C410080}, //119-88
        {0x00000015, 0x08045133}, //87-56
        {0x00000014, 0x3EC3A6BD}, //55-24
        {0x00000013, 0x1C481000}, //23:-8
};

#if 1
    ts_viu_snr_cfg_s_t sast_planck640_vot_cfg[] = {
        {0x0000001f, 0x0001390A},
        {0x0000001e, 0x3734153B},
        {0x0000001d, 0x343AFB57},
        {0x0000001c, 0xFDFF1D0C},
        {0x0000001b, 0x0C035F82},
        {0x0000001a, 0xA1700080},
        {0x00000019, 0x826CD90C},
        {0x00000018, 0x8B8416C5},
        {0x00000017, 0xB36DB61B},
        {0x00000016, 0x6C410080},
        {0x00000015, 0x08045133},
        {0x00000014, 0x3EC3A6BD},
        {0x00000013, 0x1C400000},
    };
#else
    //17*17 wlp 陶封  编译UAW3 UAW3_LITE 需要用这个配置
    ts_viu_snr_cfg_s_t sast_planck640_vot_cfg[] = {
        {0x0000001f, 0x0001390A},
        {0x0000001e, 0x3734153B},
        {0x0000001d, 0x32491357},
        {0x0000001c, 0xFDFF1D0C},
        {0x0000001b, 0x0C035F82},
        {0x0000001a, 0xA17000FF},
        {0x00000019, 0xF26CD990},
        {0x00000018, 0x9B6416C5},
        {0x00000017, 0xB36DB61B},
        {0x00000016, 0x6C410080},
        {0x00000015, 0x08045133},
        {0x00000014, 0x3EC3A6BD},
        {0x00000013, 0x1C400000},
    };
    #endif
#endif

#ifdef PLANCK640C_101492
ts_viu_snr_cfg_s_t sast_planck640_vot_cfg[] = {
    {0x0000001f, 0x0001390A},
    {0x0000001e, 0x3734153B},
    {0x0000001d, 0x3438FB57},
    {0x0000001c, 0xFDFF1D0C},
    {0x0000001b, 0x0C035F82},
    {0x0000001a, 0xA1700080},
    {0x00000019, 0x826CDA4C},
    {0x00000018, 0x8B8416C5},
    {0x00000017, 0xB36DB61B},
    {0x00000016, 0x6C410080},
    {0x00000015, 0x08045133},
    {0x00000014, 0x3EC3A6BD},
    {0x00000013, 0x1C400000},
};
#endif

#ifdef PLANCK640C_101555
ts_viu_snr_cfg_s_t sast_planck640_cfg[] = {
    {0x0000001f, 0x0001390A},
    {0x0000001e, 0x3734153B},
    {0x0000001d, 0x343CFB57},
    {0x0000001c, 0xFDFF1D0C},
    {0x0000001b, 0x0C035F82},
    {0x0000001a, 0xA1700080},
    {0x00000019, 0x826CD8CC},
    {0x00000018, 0x8B8416C5},
    {0x00000017, 0xB36DB61B},
    {0x00000016, 0x6C410080},
    {0x00000015, 0x08045133},
    {0x00000014, 0x3EC3A6BD},
    {0x00000013, 0x1C400000},
};
#endif

#ifdef PLANCK640C_3807A5
ts_viu_snr_cfg_s_t sast_planck640_vot_cfg[] = {
    {0x0000001f, 0x0001390A},//[407:376]
    {0x0000001e, 0x3734153B},//[375:344],tcc:362
    {0x0000001d, 0x342CD357},//[343:312]
    {0x0000001c, 0xFDFF1D0C},//[311:280]
    {0x0000001b, 0x0C135F82},//[279:248]
    {0x0000001a, 0x7D7000E0},//[247:216]
    {0x00000019, 0x826CD8CC},//[215:184],[202:196] GOF,[18:12]
    {0x00000018, 0x8B8416C5},//[183:152]
    {0x00000017, 0xB36DB61B},//[151:120]
    {0x00000016, 0x6C410080},//[119:88]
    {0x00000015, 0x08045133},//[87:56]
    {0x00000014, 0x3EC3A6BD},//[55:24]
    {0x00000013, 0x1C400000},//[23:-8]
};
#endif

ts_viu_snr_attr_s_t sst_vot_planck640_attr = {
    .u16_uint_len = 0x197,
    .u16_cfg_num = sizeof(sast_planck640_vot_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_planck640_vot_cfg,
};

ts_viu_snr_attr_s_t sst_vot_planck640_9c1_attr = {
    .u16_uint_len = 0x197,
    .u16_cfg_num = sizeof(sast_planck640_9c1_vot_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_planck640_9c1_vot_cfg,
};

ts_viu_snr_attr_s_t sst_vot_planck640_c122_attr = {
    .u16_uint_len = 0x197,
    .u16_cfg_num = sizeof(sast_planck640_c122_vot_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_planck640_c122_vot_cfg,
};

ts_viu_snr_cfg_s_t sast_planck384_vot_cfg[] = {
    {0x0000001f, 0x0001390A},//[407:376]
    {0x0000001e, 0x3734153B},//[375:344],tcc:362
    {0x0000001d, 0x34491357},//[343:312]
    {0x0000001c, 0xFDFF1D0C},//[311:280]
    {0x0000001b, 0x0C035F82},//[279:248]
    {0x0000001a, 0xA17000FF},//[247:216]
    {0x00000019, 0xF26CD918},//[215:184],[202:196] GOF,[18:12]
    {0x00000018, 0x0B8416C5},//[183:152]
    {0x00000017, 0xB36DB61B},//[151:120]
    {0x00000016, 0x6C410080},//[119:88]
    {0x00000015, 0x08045133},//[87:56]
    {0x00000014, 0x3EC3A6BD},//[55:24]
    {0x00000013, 0x1C400000},//[23:-8]
};



ts_viu_snr_attr_s_t sst_vot_planck384_attr = {
    .u16_uint_len = 0x197,
    .u16_cfg_num = sizeof(sast_planck384_vot_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_planck384_vot_cfg,
};


/* 25HZ */
#if 0
ts_viu_snr_cfg_s_t sast_planck1280_vot_cfg[] = {
    {0x0000001f, 0x00000003},//[255:224]
    {0x0000001e, 0x2B80ECC6},//[223:192]
    {0x0000001d, 0x48780653},//[191:160]
    {0x0000001c, 0xB18D3AF4},//[159:128]
    {0x0000001b, 0x5C131F88},//[127:96]
    {0x0000001a, 0xFF025C9B},//[95:64]
    {0x00000019, 0x04E4375C},//[63:32]
    {0x00000018, 0x0BC68AF0},//[31:0]
};
#else
ts_viu_snr_cfg_s_t sast_planck1280_vot_cfg[] = {
    {0x0000001f, 0x00000003},//[255:224]
    {0x0000001e, 0x1B80ECC6},//[223:192]
    {0x0000001d, 0x48780653},//[191:160]
    {0x0000001c, 0xB1653AF4},//[159:128]
    {0x0000001b, 0x5C031F88},//[127:96]
    {0x0000001a, 0xFF025CA4},//[95:64]
    {0x00000019, 0xE4E4375C},//[63:32]
    {0x00000018, 0x0BC68AF0},//[31:0]
};
// ts_viu_snr_cfg_s_t sast_planck1280_vot_cfg[] = {
//     {0x0000001f, 0x00000003},//[255:224]
//     {0x0000001e, 0x2880ECC6},//[223:192]
//     {0x0000001d, 0x48780653},//[191:160]
//     {0x0000001c, 0xB18D3AF4},//[159:128]
//     {0x0000001b, 0x5C031F88},//[127:96]
//     {0x0000001a, 0xFF025C9B},//[95:64]
//     {0x00000019, 0x04E4375C},//[63:32]
//     //{0x00000018, 0x0BC68AF0},//[31:0]
//     {0x00000018, 0x0BC68AD7},//[31:0]
// };
#endif

ts_viu_snr_attr_s_t sst_vot_planck1280_attr = {
    .u16_uint_len = 256 - 1,
    .u16_cfg_num = sizeof(sast_planck1280_vot_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_planck1280_vot_cfg,
};


#if 1
//static 
ts_void brd_init(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

#if 0
    /* Sensor misc io mux */
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

    ts_lib_gpio_set_o_strong(TS_IO_BT1120_CLK, 3);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT0, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT1, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT2, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT3, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT4, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT5, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT6, 2);
    ts_lib_gpio_set_o_strong(TS_IO_BT1120_DAT7, 2);

    /* 配置探测器复位、电源、开门的 IO 控制信息 */
    st_snr_io_cfg.s32_pwr_gpio_num = -1;
    st_snr_io_cfg.s32_rst_gpio_num = 46;
    st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.u32_rst_dly = 100000;

    st_snr_io_cfg.s32_sht_p_gpio_num = 45;
    st_snr_io_cfg.s32_sht_n_gpio_num = 24;
    st_snr_io_cfg.u32_sht_dbg = 1;
    st_snr_io_cfg.u32_sht_hold = 60000;
#else
    /* Sensor misc io mux */
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//AVDD_EN
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT5, TS_IO_SR_OUT_DAT5_FUN3_GPIO44);//SHUTTERN
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT4, TS_IO_SR_OUT_DAT4_FUN3_GPIO43);//SHUTTERP
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT3, TS_IO_SR_OUT_DAT3_FUN3_GPIO42);//VDET_EN2
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT2, TS_IO_SR_OUT_DAT2_FUN3_GPIO41);//VDET_EN1
    ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//SHUTTER_CTL

    snr_gpio_cfg2out(42, TS_ELEC_LVL_HI);
    snr_gpio_cfg2out(41, TS_ELEC_LVL_HI);
    //snr_gpio_cfg2out(45, TS_ELEC_LVL_HI);
    snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);

    /*******************
    ** IO DRIVER LEVEL
    *******************/
    ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);

    /* 配置探测器复位、电源、开门的 IO 控制信息 */
    st_snr_io_cfg.s32_pwr_gpio_num = 45;
    st_snr_io_cfg.en_pwr_off_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.s32_rst_gpio_num = -1;
    st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.u32_rst_dly = 200000;

    st_snr_io_cfg.s32_sht_p_gpio_num = 43;
    st_snr_io_cfg.s32_sht_n_gpio_num = 44;
    st_snr_io_cfg.u32_sht_dbg = 1;
    st_snr_io_cfg.u32_sht_hold = 60000;
#endif

    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();
}
#else
static ts_void brd_init(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

    /* Sensor misc io mux */
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//AVDD_EN
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT5, TS_IO_SR_OUT_DAT5_FUN3_GPIO44);//SHUTTERN
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT4, TS_IO_SR_OUT_DAT4_FUN3_GPIO43);//SHUTTERP
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT3, TS_IO_SR_OUT_DAT3_FUN3_GPIO42);//VDET_EN2
    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT2, TS_IO_SR_OUT_DAT2_FUN3_GPIO41);//VDET_EN1
    ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//SHUTTER_CTL

    snr_gpio_cfg2out(42, TS_ELEC_LVL_HI);
    snr_gpio_cfg2out(41, TS_ELEC_LVL_HI);
    //snr_gpio_cfg2out(45, TS_ELEC_LVL_HI);
    snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);

    /*******************
    ** IO DRIVER LEVEL
    *******************/
    ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);

    /* 配置探测器复位、电源、开门的 IO 控制信息 */
    st_snr_io_cfg.s32_pwr_gpio_num = 45;
    st_snr_io_cfg.en_pwr_off_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.s32_rst_gpio_num = -1;
    st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.u32_rst_dly = 100000;

    st_snr_io_cfg.s32_sht_p_gpio_num = 43;
    st_snr_io_cfg.s32_sht_n_gpio_num = 44;
    st_snr_io_cfg.u32_sht_dbg = 1;
    st_snr_io_cfg.u32_sht_hold = 60000;

    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();
}

#endif



//读取408bit从Flash
//static 
ts_void ReadSmartData408bitFromFlash(ts_viu_snr_attr_s_t *pst_snr_attr)
{
    struct Menu2_Para SysMenuDefParam;
    ts_u32 *u32_value=NULL;
    ts_u08 data[VOT_RESERVED_DATA_LENGTH+1];
    int i=0;
    int index=VOT_RESERVED_DATA_LENGTH-1;
    data[0] = 0x0;

    if(ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0)//读取系统菜单默认参数配置成功
    {
        if(SysMenuDefParam.SmartData408bitValidFlag == 0x01)
        {
            vision_log_warn(" SmartData408bitValidFlag %u \n ",  SysMenuDefParam.SmartData408bitValidFlag);
            for(i=1; i<VOT_RESERVED_DATA_LENGTH+1; i++){
                data[i]=SysMenuDefParam.SmartData408bit[index];
                // vision_log_debug("index: %d i %d data[i] %x \n ", index, i,data[i]);
                index--;
            }
            u32_value =(ts_u32*) &data[0];
            // for(i=0; i<13; i++){
            //     vision_log_debug(" i %d u32_value[i] %x \n ",  i,u32_value[i]);
            // }

            for(index=0; index<13; index++){
                pst_snr_attr->pst_cfg[index].u32_val = u32_value[12-index];
                // vision_log_debug(" index %d u32_value %x \n ",  index,u32_value[12-index]);
            }
        }
        else{
            vision_log_debug("SmartData408bit data in flash is invalid \n");
        }
    }
}

// 读取256bit从Flash
// static 
ts_void ReadSmartData256bitFromFlash(ts_viu_snr_attr_s_t *pst_snr_attr)
{
    struct Menu2_Para SysMenuDefParam;
    ts_u32 *u32_value=NULL;
    ts_u08 data[32];
    int i=0;
    int index=31;
    data[0] = 0x0;

    if(ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0)//读取系统菜单默认参数配置成功
    {
        if(SysMenuDefParam.SmartData256bitValidFlag == 0x01)
        {
            for(i=0; i<32; i++){
                data[i]=SysMenuDefParam.SmartData256bit[index];
                // vision_log_debug("index: %d i %d data[i] %x \n ", index, i,data[i]);
                index--;
            }
            u32_value =(ts_u32*) &data[0];
            // for(i=0; i<8; i++){
            //     vision_log_debug(" i %d u32_value[i] %x \n ",  i,u32_value[i]);
            // }

            for(index=0; index<8; index++){
                pst_snr_attr->pst_cfg[index].u32_val = u32_value[7-index];
                vision_log_debug("index %d u32_value 0x%08X \n ",  index,u32_value[7-index]);
            }

            // SystemParaMenu2.GAIN1280=planck1280c_read_gain_adc();
            // SystemParaMenu2.GOF1280=planck1280c_read_gof();
            // SystemParaMenu2.Cint1280=planck1280c_read_Cint();
            // SystemParaMenu2.Nint=planck1280c_read_Nint();
            // SystemParaMenu2.Sel_PJ=planck1280c_read_Sel_PJ();
            // SystemParaMenu2.NTCC=planck1280c_read_NTCC();
            // SystemParaMenu2.Voccm=planck1280c_read_Voccm();
            // SystemParaMenu2.detConfig=planck1280c_read_detConfig();
            // SystemParaMenu2.Vfid=planck1280c_read_Vfid();
        }
        else{
            vision_log_debug("SmartData256bit data in flash is invalid \n");
        }
    }
}


static ts_void start_planck640c(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_u32 u32_npu_en=0;
    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = ts_null;
    pst_attr->s32_img_w = 640;
    pst_attr->s32_img_h = 512;
    pst_attr->s32_fps = ISP_get_output_fps();
    pst_attr->s32_vo_img_w = (ts_s32)640;
    pst_attr->s32_vo_img_h = (ts_s32)512;
    global_sys_ctrl_info.m_sensor_s32_img_w = pst_attr->s32_img_w;
    global_sys_ctrl_info.m_sensor_s32_img_h = pst_attr->s32_img_h;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 3000;

    /* 是否开启 raw 同传 */
    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;
    if(SystemParaMenu2.m_user_metadata_flag ==  USER_METADATA_ENABLE){
        pst_attr->u08_info_en = 1;
        pst_attr->u08_info_line_num = 4;
    }
    else{
        pst_attr->u08_info_en = 0;
    }
    /* npu 设置 */
    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//  超分应用使能
    {
        pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;//  超分应用 TS_NPU_OPS_S
        if (TS_NPU_OPS_SR == pst_attr->st_npu.en_ops) {//640x512@50-->960x768@50超分
            pst_attr->st_npu.u32_clk = 180000000;
            pst_attr->s32_vo_img_w = (ts_s32)(960);
            pst_attr->s32_vo_img_h = (ts_s32)(768);
        } else if (TS_NPU_OPS_AISP == pst_attr->st_npu.en_ops) {
            pst_attr->st_npu.u32_clk = 180000000;
        }
        if (pst_attr->st_npu.u32_clk > 0)
            u32_npu_en = 1;
    }
    else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//640x512@50-->1280x1024@50超分
    {
        pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(1280);
        pst_attr->s32_vo_img_h = (ts_s32)(1024);
        u32_npu_en = 1;
    }
    else
    {
        pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE; //无超分
        pst_attr->st_npu.u32_clk = 0;
    }

    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 1;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;

    if(SystemParaMenu2.m_cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
        pst_attr->st_cvbs.u08_orgi_en = 0;
    }
    else{
        pst_attr->st_cvbs.u08_orgi_en = 1;
    }

    if (get_customer_type() == CUSTOMER_TYPE_ZM && SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
    {
        pst_attr->st_cvbs.u08_en = 0;//关闭CVBS输出
        pst_attr->st_cvbs.u08_sd_en = 0;
        pst_attr->st_cvbs.u08_orgi_en = 0;
        pst_attr->st_cvbs.u08_pal_en = 0;
    }
    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    // if (TS_NPU_OPS_MEM == pst_attr->st_npu.en_ops)
    //     pst_attr->st_npu.u08_mem_type = TS_NPU_MEM_EXTBP;
    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_mipi.u08_en = 0;
    pst_attr->st_mipi.u08_dsi_en = 0;
    pst_attr->st_bte.u08_redefine_timing_en = 0;
    /* 是否开启 raw 同传 raw+yuv */
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        pst_attr->u08_raw_oe = 1;
        SystemParaMenu2.m_raw_node_pos = TS_LUT_POS_FINAL_RAW;
        pst_attr->u08_raw_pos = SystemParaMenu2.m_raw_node_pos; 
        pst_attr->u08_y_oe = 1;

        if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
            pst_attr->u08_raw_seq = 1;
        }
        else{
            pst_attr->u08_raw_seq = 0;
        }
        vision_log_debug("m_raw_node_pos = %d, pst_attr->u08_raw_pos = %d\n",SystemParaMenu2.m_raw_node_pos, pst_attr->u08_raw_pos);
        vision_log_warn(" SystemParaMenu2.m_raw_yuv_mode  %d  \n",SystemParaMenu2.m_raw_yuv_mode);
        //关闭坏斑功能，此功能和raw+yuv冲突
        //pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    }

    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
        // gpio cfg for 
        bt656_bt1120_pin_init();
        //配置bt656//bt1120
        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
            pst_attr->st_bte.u08_external_sync_en = 1;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT601  \n");
        }
        else{
            pst_attr->st_bte.u08_external_sync_en = 0;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT656  \n");
        }
        pst_attr->u32_mem_en = 0;
        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 111;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 640;
            pst_attr->st_bte.u16_output_height = 512;
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//  超分应用使能640x512@50-->960x768@50超分
            {
                pst_attr->st_bte.u16_output_width = 960;
                pst_attr->st_bte.u16_output_height = 768;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//640x512@50-->1280x1024@50超分
            {
                pst_attr->st_bte.u16_output_width = 1280;
                pst_attr->st_bte.u16_output_height = 1024;
            }
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
            vision_log_debug("BT_TIMING_SEQ_MODE_WIN2_25HZ  \n");
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 55;
            pst_attr->st_bte.u16_vsync_per = 8;
            pst_attr->st_bte.u16_vsync_fp = 50;
            pst_attr->st_bte.u16_hsync_bp = 120;
            pst_attr->st_bte.u16_hsync_per = 36;
            pst_attr->st_bte.u16_hsync_fp = 60;
            pst_attr->st_bte.u16_output_width = 640;
            pst_attr->st_bte.u16_output_height = 512;
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//  超分应用使能640x512@50-->960x768@50超分
            {
                pst_attr->st_bte.u16_output_width = 960;
                pst_attr->st_bte.u16_output_height = 768;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//640x512@50-->1280x1024@50超分
            {
                pst_attr->st_bte.u16_output_width = 1280;
                pst_attr->st_bte.u16_output_height = 1024;
            }
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_30HZ){
            vision_log_debug("BT_TIMING_SEQ_MODE_WIN2_30HZ  \n");
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 101;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 640;
            pst_attr->st_bte.u16_output_height = 512;
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//  超分应用使能640x512@50-->960x768@50超分
            {
                pst_attr->st_bte.u16_output_width = 960;
                pst_attr->st_bte.u16_output_height = 768;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//640x512@50-->1280x1024@50超分
            {
                pst_attr->st_bte.u16_output_width = 1280;
                pst_attr->st_bte.u16_output_height = 1024;
            }
        }
    }
    else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI){
            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
            pst_attr->st_mipi.u08_clock_continue = 0;
            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 200;
                if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768 || SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//  超分应用使能640x512@50-->960x768@50超分 或者 640x512@50-->1280x1024@50超分
                {
                    pst_attr->st_mipi.u16_data_rate = 400;//有640超分960或者640超分1280，4lane时的数据率400Mbps/lane
                }
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 400;
                if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768 ||SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)
                {
                   pst_attr->st_mipi.u16_data_rate = 800;//有640超分960或者640超分1280，2lane时的数据率800Mbps/lane
                   vision_log_warn("pst_attr->st_mipi.u16_data_rate %d m_ts_npu_ops_sr_en %d \n",pst_attr->st_mipi.u16_data_rate, SystemParaMenu2.m_ts_npu_ops_sr_en);
                }
            }
            vision_log_warn("SystemParaMenu2.m_mipi_lane_mode  %d pst_attr->st_mipi.u16_data_rate %d \n",SystemParaMenu2.m_mipi_lane_mode ,pst_attr->st_mipi.u16_data_rate);  
    }

    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//  超分应用使能640x512@50-->960x768@50超分 
    {
    /* 关闭 pll 安全模式， vco 计算将不会按照 IP 要求进行，后果不保证 */
    //ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=162000000 pllisp=180000000 pllvpo=180000000 pllsafe=1");
        if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)
        {
            /* 加载时钟模块 */
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=156000000 pllisp=180000000 pllvpo=180000000");
        }
        else{
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=180000000 pllvpo=180000000");
        
        }
    }
    else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)// 超分应用使能640x512@50-->1280x1024@50超分
    {
        if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)
        {
            /* 加载时钟模块 */
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=156000000 pllisp=360000000 pllvpo=360000000");
        }
        else{
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=360000000 pllvpo=360000000");
        
        }
    }
    else
    {
        if(SystemParaMenu2.m_raw_yuv_mode != RAW_YUV_MODE_NONE && (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656))
        {
            /* 加载时钟模块 */
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=156000000 pllisp=180000000 pllvpo=216000000");
        }
        else{
            ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=180000000 pllvpo=216000000");
        
        }
    }
    planck640c_clkm_init();
    snr_insmod_all_tdk(u32_npu_en);
    // brd_init();
    vision_log_warn("pst_attr->st_bte.u08_redefine_timing_en :  %d \n",pst_attr->st_bte.u08_redefine_timing_en);
    vision_log_debug("snr_init_smart init \n");
    if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_9C1 ){
        snr_init_smart(sast_planck640_9c1_vot_cfg, sizeof(sast_planck640_9c1_vot_cfg));
        ReadSmartData408bitFromFlash(&sst_vot_planck640_9c1_attr);
        planck640c_viu_init(pst_attr, &sst_vot_planck640_9c1_attr);
        vision_log_warn("FORCE_DETECTOR_TYPE_VOT_640X512_9C1  \n");
    }
    else if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_640X512_C122){
        snr_init_smart(sast_planck640_c122_vot_cfg, sizeof(sast_planck640_c122_vot_cfg));
        ReadSmartData408bitFromFlash(&sst_vot_planck640_c122_attr);
        planck640c_viu_init(pst_attr, &sst_vot_planck640_c122_attr);
        vision_log_warn(" FORCE_DETECTOR_TYPE_VOT_640X512_C122  \n");
    }
    else{
        snr_init_smart(sast_planck640_vot_cfg, sizeof(sast_planck640_vot_cfg));
        ReadSmartData408bitFromFlash(&sst_vot_planck640_attr);
        planck640c_viu_init(pst_attr, &sst_vot_planck640_attr);
    }

    snr_cfg_vpo(pst_attr);
    pst_attr->get_temperature = planck640c_get_temperature;

    /*配置ISP参数*/
    ts_void *pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_512P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);

    /*使能输入*/
    ts_tdk_viu_enable(TS_VIU_MDEV0);
  
    vot_snr_init_detector(0,DET_TEMP_MODE_AUTO);
  

    if (get_customer_type() == CUSTOMER_TYPE_ZM && SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
    {
       ts_cpl_fix_msleep(500);
       tdk_snrcfg_shutter_open();//打开挡片，方便外部信号触发采集一帧MIPI数据
        ts_viu_corr_rst_s_t st_occ_rst;
        st_occ_rst.u08_reset_only = 1;
        st_occ_rst.u08_reset_val = 0x20;
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_occ_rst);
        tdk_snrcfg_shutter_open();
    }
    else
    {
        snr_wait_ready(6000);
    }

    // ts_viu_corr_rst_s_t st_occ_rst;

    // st_occ_rst.u08_reset_only = 1;
    // st_occ_rst.u08_reset_val = 0x20;
    // ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_occ_rst);
    // tdk_snrcfg_shutter_open();

    /*OCC 初始化*/
    // snr_init_ooc();

    /*K 初始化*/
    ts_isp_nuc_attr_s_t st_nuc_attr;
    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_DELTAB; 
    st_nuc_attr.en_stat_mode = 2;  
    st_nuc_attr.u08_stat_frm_num = 16;//12;//48; //////////
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;
    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);

    ts_isp_sffc_attr_s_t sffc_attr;
#ifndef VOT_NUC_SFFC_TEST
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }

    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 6000;
#else
    gst_snr_ctx.s32_sffc_alg = 1;
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }

    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 3000;
#endif
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);

    //增加多一次读取温度，因为当前第一次读取有异常
    vot_snr_init_detector(0,DET_TEMP_MODE_AUTO);


}

static ts_void start_planck384c(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_u32 u32_npu_en=0;
    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = planck384c_get_temperature;
    pst_attr->s32_img_w = 384;
    pst_attr->s32_img_h = 288;
    pst_attr->s32_fps = ISP_get_output_fps();
    pst_attr->s32_vo_img_w = (ts_s32)384;
    pst_attr->s32_vo_img_h = (ts_s32)288;
    global_sys_ctrl_info.m_sensor_s32_img_w = pst_attr->s32_img_w;
    global_sys_ctrl_info.m_sensor_s32_img_h = pst_attr->s32_img_h;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 3000;

    /* 是否开启 raw 同传 */
    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;
    if(SystemParaMenu2.m_user_metadata_flag ==  USER_METADATA_ENABLE){
        pst_attr->u08_info_en = 1;
        pst_attr->u08_info_line_num = 4;
    }
    else{
        pst_attr->u08_info_en = 0;
    }
    //手持机定制，开启384超分768且输出RAW+YUV T8模式,768x576YUV超分图像+768x144测温AD 上下拼接
    if (get_customer_type() == CUSTOMER_TYPE_HAND && ts_cpl_readl(0x001e0014)!=0)//A6芯片 ,非N6
    {
        SystemParaMenu2.m_ts_npu_ops_sr_en=NPU_SR_384X288_768X576;
        SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_RAW_YUV;//RAW_YUV_MODE_YUV_RAW;
    }
  /* npu 设置 */
    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)// 超分使能 384x288@50-->640x512@50超分
    {
        pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(640);
        pst_attr->s32_vo_img_h = (ts_s32)(512);
        u32_npu_en = 1;
    }
    else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
    {
        pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(768);
        pst_attr->s32_vo_img_h = (ts_s32)(576);
        u32_npu_en = 1;
    }
    else
    {
        pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE; //无超分
        pst_attr->st_npu.u32_clk = 0;
    }
    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 1;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;
    if(SystemParaMenu2.m_cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
        pst_attr->st_cvbs.u08_orgi_en = 0;
    }
    else{
        pst_attr->st_cvbs.u08_orgi_en = 1;
    }

    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    // if (TS_NPU_OPS_MEM == pst_attr->st_npu.en_ops)
    //     pst_attr->st_npu.u08_mem_type = TS_NPU_MEM_EXTBP;
    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_mipi.u08_en = 0;
    pst_attr->st_mipi.u08_dsi_en = 0;
    pst_attr->st_bte.u08_redefine_timing_en = 0;
    /* 是否开启 raw 同传 raw+yuv */
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        pst_attr->u08_raw_oe = 1;
        SystemParaMenu2.m_raw_node_pos = TS_LUT_POS_FINAL_RAW;
        pst_attr->u08_raw_pos = SystemParaMenu2.m_raw_node_pos; 
        pst_attr->u08_y_oe = 1;

        if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
            pst_attr->u08_raw_seq = 1;
        }
        else{
            pst_attr->u08_raw_seq = 0;
        }
        vision_log_warn("m_raw_node_pos %d  \n",SystemParaMenu2.m_raw_node_pos);
        vision_log_warn(" SystemParaMenu2.m_raw_yuv_mode  %d  \n",SystemParaMenu2.m_raw_yuv_mode);
        //关闭坏斑功能，此功能和raw+yuv冲突
        //pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    }
    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
        // gpio cfg for 
        bt656_bt1120_pin_init();
        //配置bt656//bt1120
        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
            pst_attr->st_bte.u08_external_sync_en = 1;
            vision_log_warn("VIDEO_OUTPUT_MODE_ENUM_BT601  \n");
        }
        else{
            pst_attr->st_bte.u08_external_sync_en = 0;
            vision_log_warn("VIDEO_OUTPUT_MODE_ENUM_BT656  \n");
        }
        pst_attr->u32_mem_en = 0;

        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 60;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 384;
            pst_attr->st_bte.u16_output_height = 288;   
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)// 超分使能 384x288@50-->640x512@50超分
            {
                pst_attr->st_bte.u16_output_width = 640;
                pst_attr->st_bte.u16_output_height = 512;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
            {
                pst_attr->st_bte.u16_output_width = 768;
                pst_attr->st_bte.u16_output_height = 576;
            }
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
            vision_log_warn("BT_TIMING_SEQ_MODE_WIN2_25HZ \n");
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 50;
            pst_attr->st_bte.u16_vsync_per = 22;
            pst_attr->st_bte.u16_vsync_fp = 40;
            pst_attr->st_bte.u16_hsync_bp = 60;
            pst_attr->st_bte.u16_hsync_per = 18;
            pst_attr->st_bte.u16_hsync_fp = 30;
            pst_attr->st_bte.u16_output_width = 384;
            pst_attr->st_bte.u16_output_height = 288;          
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)// 超分使能 384x288@50-->640x512@50超分
            {
                pst_attr->st_bte.u16_output_width = 640;
                pst_attr->st_bte.u16_output_height = 512;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
            {
                pst_attr->st_bte.u16_output_width = 768;
                pst_attr->st_bte.u16_output_height = 576;
            }
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_30HZ){
            vision_log_warn("BT_TIMING_SEQ_MODE_WIN2_30HZ \n");
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 60;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 384;
            pst_attr->st_bte.u16_output_height = 288;            
            if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)// 超分使能 384x288@50-->640x512@50超分
            {
                pst_attr->st_bte.u16_output_width = 640;
                pst_attr->st_bte.u16_output_height = 512;
            }
            else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
            {
                pst_attr->st_bte.u16_output_width = 768;
                pst_attr->st_bte.u16_output_height = 576;
            }
        }
    }
    else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI){
            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 200;
                if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)//  超分使能 384x288@50-->768x576@50超分
                {
                    pst_attr->st_mipi.u16_data_rate = 400;//2lane时的数据率400Mbps/lane
                }
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 400;
                if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)//超分使能 384x288@50-->768x576@50超分，则2lane时的数据率800Mbps/lane
                {
                   pst_attr->st_mipi.u16_data_rate = 800;//2lane时的数据率800Mbps/lane
                }
            }
            vision_log_warn("SystemParaMenu2.m_mipi_lane_mode  %d  \n",SystemParaMenu2.m_mipi_lane_mode );
            pst_attr->st_mipi.u08_clock_continue = 0;
            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
    }

    // if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)// 超分使能 384x288@50-->640x512@50超分
    // {
    //    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=174000000 pllisp=180000000 pllvpo=180000000");
    // }
    // else
    // {
        ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=174000000 pllisp=180000000 pllvpo=240000000");
   // }
    
    planck384c_clkm_init();
    snr_insmod_all_tdk(u32_npu_en);
    // brd_init();
    snr_init_smart(sast_planck384_vot_cfg, sizeof(sast_planck384_vot_cfg));
    ReadSmartData408bitFromFlash(&sst_vot_planck384_attr);
    planck384c_viu_init(pst_attr, &sst_vot_planck384_attr);
    snr_cfg_vpo(pst_attr);
    /*配置ISP参数*/
    ts_void *pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_512P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);
    /*使能输入*/
    ts_tdk_viu_enable(TS_VIU_MDEV0);


    vot_snr_init_detector(0,DET_TEMP_MODE_AUTO); 
    // tdk_snrcfg_shutter_close();
    snr_wait_ready(6000);
 
    // ts_viu_corr_rst_s_t st_occ_rst;

    // st_occ_rst.u08_reset_only = 1;
    // st_occ_rst.u08_reset_val = 0x20;
    // ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_occ_rst);
    // tdk_snrcfg_shutter_open();

    /*OCC 初始化*/
    // snr_init_ooc();
    

    /*K 初始化*/
    ts_isp_nuc_attr_s_t st_nuc_attr;
    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_DELTAB; 
    st_nuc_attr.en_stat_mode = 2;  
    st_nuc_attr.u08_stat_frm_num = 16;//12;//48; //////////
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;
    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);

    ts_isp_sffc_attr_s_t sffc_attr;
#ifndef VOT_NUC_SFFC_TEST
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }
    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 6000;
#else
    gst_snr_ctx.s32_sffc_alg = 1;
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }
    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 3000;
#endif
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);


    //增加多一次读取温度，因为当前第一次读取有异常
    vot_snr_init_detector(0,DET_TEMP_MODE_AUTO);

}


static ts_void start_planck1280c(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_u32 u32_npu_en=0;
    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = ts_null;
    pst_attr->s32_img_w = 1280;
    pst_attr->s32_img_h = 1024;
    pst_attr->s32_fps = (ts_f32)25;// ISP_get_output_fps();
    pst_attr->s32_vo_img_w = (ts_s32)1280;
    pst_attr->s32_vo_img_h = (ts_s32)1024;
    global_sys_ctrl_info.m_sensor_s32_img_w = pst_attr->s32_img_w;
    global_sys_ctrl_info.m_sensor_s32_img_h = pst_attr->s32_img_h;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 3000;
    /* 是否开启 raw 同传 */
    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;//TS_LUT_POS_AFTER_BPC;
    if(SystemParaMenu2.m_user_metadata_flag ==  USER_METADATA_ENABLE){
        pst_attr->u08_info_en = 1;
        pst_attr->u08_info_line_num = 4;
    }
    else{
        pst_attr->u08_info_en = 0;
    }
    /* npu 设置 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE; //TS_NPU_OPS_MEM;
    pst_attr->st_npu.u32_clk = 0;
    if (TS_NPU_OPS_SR == pst_attr->st_npu.en_ops) {
        pst_attr->st_npu.u32_clk = 300000000;
        pst_attr->s32_vo_img_w = pst_attr->s32_vo_img_w << 1;
        pst_attr->s32_vo_img_h = pst_attr->s32_vo_img_h << 1;
    } else if (TS_NPU_OPS_AISP == pst_attr->st_npu.en_ops) {
        pst_attr->st_npu.u32_clk = 300000000;
    }
    if (pst_attr->st_npu.u32_clk > 0)
        u32_npu_en = 1;
    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 0;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;
    if(SystemParaMenu2.m_cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
        pst_attr->st_cvbs.u08_orgi_en = 0;
    }
    else{
        pst_attr->st_cvbs.u08_orgi_en = 1;
    }
    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;
    // if (TS_NPU_OPS_MEM == pst_attr->st_npu.en_ops)
    //     pst_attr->st_npu.u08_mem_type = TS_NPU_MEM_EXTBP;
    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_mipi.u08_en = 0;
    pst_attr->st_mipi.u08_dsi_en = 0;
    pst_attr->st_bte.u08_redefine_timing_en = 1;
    /* 是否开启 raw 同传 raw+yuv */
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        pst_attr->u08_raw_oe = 1;
        pst_attr->u08_raw_pos = SystemParaMenu2.m_raw_node_pos; 
        pst_attr->u08_y_oe = 1;
        if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
            pst_attr->u08_raw_seq = 1;
        }
        else{
            pst_attr->u08_raw_seq = 0;
        }
        vision_log_debug("m_raw_node_pos %d  \n",SystemParaMenu2.m_raw_node_pos);
        vision_log_debug(" SystemParaMenu2.m_raw_yuv_mode  %d  \n",SystemParaMenu2.m_raw_yuv_mode);
        //关闭坏斑功能，此功能和raw+yuv冲突
        pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    }
    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
        // gpio cfg for 
        bt656_bt1120_pin_init();
        //配置bt656//bt1120
        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
            pst_attr->st_bte.u08_external_sync_en = 1;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT601  \n");
        }
        else{
            pst_attr->st_bte.u08_external_sync_en = 0;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT656  \n");
        }
        pst_attr->u32_mem_en = 0;
        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 111;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 1280;
            pst_attr->st_bte.u16_output_height = 1024;
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;
            pst_attr->st_bte.u16_hsync_bp = 101;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;
            pst_attr->st_bte.u16_output_width = 1280;
            pst_attr->st_bte.u16_output_height = 1024;
        }
    }
    else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI){
            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
            pst_attr->st_mipi.u08_clock_continue = 0;
            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 400;
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 800;//2lane时的数据率800Mbps/lane
            }
            vision_log_warn("SystemParaMenu2.m_mipi_lane_mode  %d  \n",SystemParaMenu2.m_mipi_lane_mode );
    }

    /* 加载时钟模块 */
    #if 1 
    //设置viu、isp时钟50MHz  
    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=300000000 pllisp=300000000 pllvpo=300000000");
    #else
    //设置viu、isp时钟42MHz  
    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=168000000 pllisp=168000000 pllvpo=300000000");
    #endif
    planck1280c_clkm_init();
    snr_insmod_all_tdk(u32_npu_en);
    // brd_init();
    tdk_snrcfg_reset();
    snr_init_smart(sast_planck1280_vot_cfg, sizeof(sast_planck1280_vot_cfg));
    //ReadSmartData256bitFromFlash(&sst_vot_planck1280_attr);
    planck1280c_viu_init(pst_attr, &sst_vot_planck1280_attr);
    pst_attr->get_temperature = planck1280c_get_temperature;
    snr_cfg_vpo(pst_attr);
    /*配置ISP参数*/
    ts_void *pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_1024P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);

    /* 使能输入 */
    ts_tdk_viu_enable(TS_VIU_MDEV0);
    vot_snr_init_detector(0,DET_TEMP_MODE_AUTO);
    ts_cpl_fix_msleep(1000);  
    tdk_snrcfg_shutter_close();
    snr_wait_ready(6000);
    ts_viu_corr_rst_s_t st_occ_rst;
    st_occ_rst.u08_reset_only = 0;
    st_occ_rst.u08_reset_val = 0x80;
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_occ_rst);
    tdk_snrcfg_shutter_open();
      /*OCC 初始化*/
    snr_init_ooc();
   
    /*K 初始化*/
    ts_isp_nuc_attr_s_t st_nuc_attr;
    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    #if 0
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_GAIN_OFFSET; 
    st_nuc_attr.en_stat_mode = 1;  
    #else
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_DELTAB; 
    st_nuc_attr.en_stat_mode = 2;  
    #endif
    st_nuc_attr.u08_stat_frm_num = 31;
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;
    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);
   
    ts_isp_sffc_attr_s_t sffc_attr;
    #ifndef VOT_NUC_SFFC_TEST
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 8500;
    #else
    gst_snr_ctx.s32_sffc_alg = 1;
    if (0 == gst_snr_ctx.s32_sffc_alg) {
        sffc_attr.u08_enable = 1;
    } else {
        sffc_attr.u08_enable = 0;
    }

    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 8500;
    #endif
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);


    // if(SystemParaMenu2.Bpc_ext_SW>0){
    //     //ts_tdk_extbpc_init(pst_attr->s32_img_w, pst_attr->s32_img_h, 0x00160234, 3, 1);
    //     //ts_tdk_extbpc_init(pst_attr->s32_img_w, pst_attr->s32_img_h, 0x00160234, 0, 1);
    //     ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150);
    // }
    //增加多一次读取温度，因为当前第一次读取有异常
     vot_snr_init_detector(0,DET_TEMP_MODE_AUTO);
 
}

/****
 * //函数功能：超分使能切换,无需设备重启，参数直接生效
 * 参数：无
 * 返回值：无
 ******/
extern ts_void vpo_disable_all(ts_void);
void NPU_SR_ParamAvailable(void)
{
    ts_vpo_grp_attr_s_t st_attr;
    snr_attr_s_t *pst_attr;
    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
    vpo_disable_all();
    // /* 是否关闭 CVBS */
    // if(pst_attr->st_cvbs.u08_sd_en > 0)
    // {
    //     ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
    // }
    // /* 是否关闭 MEM */
    // if (pst_attr->u32_mem_en > 0)
    //     ts_tdk_vpo_disable_chn(TS_VPO_MEM_MDEV);

    // /* 是否关闭 MIPI */
    // if (pst_attr->st_mipi.u08_en > 0) {
    //     // snr_chk_ch1_mdev(TS_VPO_NULL_MDEV);
    //     /* CSI or DSI */
    //     if (0 == pst_attr->st_mipi.u08_dsi_en)
    //         ts_tdk_vpo_disable_chn(TS_VPO_MIPI_CSI_MDEV);
    //     else
    //         ts_tdk_vpo_disable_chn(TS_VPO_MIPI_DSI_MDEV);
    // }
    // /* 是否关闭 BTE */
    // if (pst_attr->st_bte.u08_en > 0) {
    //     ts_tdk_vpo_disable_chn(TS_VPO_BTENC_MDEV);
    // }
    ts_cpl_msleep(100);
    ts_u32 u32_npu_en=0;
	if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//640分辨率下，仅A12芯片 NPU超分为640->960
	{	
		pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(960);
        pst_attr->s32_vo_img_h = (ts_s32)(768);
        u32_npu_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
        {
            pst_attr->st_bte.u16_output_width = 960;
            pst_attr->st_bte.u16_output_height = 768;
        }
        else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)
        {
            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
            pst_attr->st_mipi.u08_clock_continue = 0;
            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 400;
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 800;
            }
        }
	}
	else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//640分辨率下，仅A12芯片 NPU超分为640->1280
	{
		pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(1280);
        pst_attr->s32_vo_img_h = (ts_s32)(1024);
        u32_npu_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
        {
            pst_attr->st_bte.u16_output_width = 1280;
            pst_attr->st_bte.u16_output_height = 1024;
        }
        else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)
        {
            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
            pst_attr->st_mipi.u08_clock_continue = 0;
            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 400;
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 800;
            }
        }
	}
	else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_640X512)//384分辨率下，A6和A12芯片 NPU超分为384->640
	{	
		pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(640);
        pst_attr->s32_vo_img_h = (ts_s32)(512);
        u32_npu_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
        {
            pst_attr->st_bte.u16_output_width = 640;  
            pst_attr->st_bte.u16_output_height = 512;
        }
	}
    else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)//384分辨率下，A6和A12芯片 NPU超分为384->768
    {
        pst_attr->st_npu.en_ops =TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 180000000;
        pst_attr->s32_vo_img_w = (ts_s32)(768);
        pst_attr->s32_vo_img_h = (ts_s32)(576);
        u32_npu_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
        {
            pst_attr->st_bte.u16_output_width = 768;  
            pst_attr->st_bte.u16_output_height = 576;
        }       
    }
	else if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_OFF)// NPU超分关闭
	{	
        ts_tdk_aisp_set_npu_status(0);
        ts_tdk_aisp_set_sr_status(0);
        pst_attr->st_npu.en_ops =TS_NPU_OPS_DISABLE;
        pst_attr->st_npu.u32_clk = 0;//180000000;
        u32_npu_en = 0;
        if(get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
        {
            pst_attr->s32_vo_img_w = (ts_s32)(640);   
            pst_attr->s32_vo_img_h = (ts_s32)(512);
            if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
            {
                pst_attr->st_bte.u16_output_width = 640;
                pst_attr->st_bte.u16_output_height = 512;
            }
        }
        else if(get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_384X288)
        {
            pst_attr->s32_vo_img_w = (ts_s32)(384);
            pst_attr->s32_vo_img_h = (ts_s32)(288);
            if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 )
            {
                pst_attr->st_bte.u16_output_width = 384;
                pst_attr->st_bte.u16_output_height = 288;
            }
        }
	}
    if(u32_npu_en>0)
    {
        // insmod_parse_param("aisp", ts_null);
        // tdl_isp_upload_init();
        ts_tdk_aisp_set_sr_status(1);
    }
    if(get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
    {

        
    }
    else if(get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_384X288)
    {
        //手持机定制，开启384超分768且输出RAW+YUV T8模式,768x576YUV超分图像+768x144测温AD 上下拼接
        if (get_customer_type() == CUSTOMER_TYPE_HAND)
        {   
            pst_attr->s32_img_w = 384;
            pst_attr->s32_img_h = 288;
            pst_attr->s32_fps = ISP_get_output_fps();
            global_sys_ctrl_info.m_sensor_s32_img_w = pst_attr->s32_img_w;
            global_sys_ctrl_info.m_sensor_s32_img_h = pst_attr->s32_img_h;

            /* 是否开启开机画面 */
            pst_attr->u32_pod_dly_ms = 0;

            /* 是否开启 raw 同传 */
            pst_attr->u08_y_oe = 1;
            pst_attr->u08_raw_oe = 0;
            pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;
            if(SystemParaMenu2.m_user_metadata_flag ==  USER_METADATA_ENABLE){
                pst_attr->u08_info_en = 1;
                pst_attr->u08_info_line_num = 4;
            }
            else{
                pst_attr->u08_info_en = 0;
            }

            /* cvbs 通道配置 */
            pst_attr->st_cvbs.u08_en = 1;
            pst_attr->st_cvbs.u08_sd_en = 1;
            pst_attr->st_cvbs.u08_orgi_en = 0;
            pst_attr->st_cvbs.u08_pal_en = 1;
            if(SystemParaMenu2.m_cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
                pst_attr->st_cvbs.u08_orgi_en = 0;
            }
            else{
                pst_attr->st_cvbs.u08_orgi_en = 1;
            }

            /* mem 通道配置 */
            pst_attr->u32_mem_en = 1;
            /* bt601/bt656 通道配置 */
            pst_attr->st_bte.u08_en = 0;
            pst_attr->st_bte.u08_redefine_timing_en = 0;
             /* mipi 通道配置 */
            pst_attr->st_mipi.u08_en = 0;
            pst_attr->st_mipi.u08_dsi_en = 0;
            SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_RAW_YUV;//RAW_YUV_MODE_YUV_RAW;
            /* 是否开启 raw 同传 raw+yuv */
            if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
                pst_attr->u08_raw_oe = 1;
                SystemParaMenu2.m_raw_node_pos = TS_LUT_POS_FINAL_RAW;
                pst_attr->u08_raw_pos = SystemParaMenu2.m_raw_node_pos; 
                pst_attr->u08_y_oe = 1;

                if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
                    pst_attr->u08_raw_seq = 1;
                }
                else{
                    pst_attr->u08_raw_seq = 0;
                }
                vision_log_warn("m_raw_node_pos %d  \n",SystemParaMenu2.m_raw_node_pos);
                vision_log_warn(" SystemParaMenu2.m_raw_yuv_mode  %d  \n",SystemParaMenu2.m_raw_yuv_mode);
                //关闭坏斑功能，此功能和raw+yuv冲突
                //pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
            }
            if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
                // gpio cfg for 
                // bt656_bt1120_pin_init();
                //配置bt656//bt1120
                pst_attr->st_bte.u08_en = 1;
                pst_attr->st_bte.u08_8bit_en = 1;
                if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
                    pst_attr->st_bte.u08_external_sync_en = 1;
                    vision_log_warn("VIDEO_OUTPUT_MODE_ENUM_BT601  \n");
                }
                else{
                    pst_attr->st_bte.u08_external_sync_en = 0;
                    vision_log_warn("VIDEO_OUTPUT_MODE_ENUM_BT656  \n");
                }
                pst_attr->u32_mem_en = 0;

                if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
                    pst_attr->st_bte.u08_redefine_timing_en = 1; 
                    pst_attr->st_bte.u16_vsync_bp = 20;
                    pst_attr->st_bte.u16_vsync_per = 4;
                    pst_attr->st_bte.u16_vsync_fp = 16;
                    pst_attr->st_bte.u16_hsync_bp = 60;
                    pst_attr->st_bte.u16_hsync_per = 32;
                    pst_attr->st_bte.u16_hsync_fp = 32;
                    pst_attr->st_bte.u16_output_width = 384;
                    pst_attr->st_bte.u16_output_height = 288;   
                    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
                    {
                        pst_attr->st_bte.u16_output_width = 768;
                        pst_attr->st_bte.u16_output_height = 576;
                    }
                }
                else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
                    vision_log_warn("BT_TIMING_SEQ_MODE_WIN2_25HZ \n");
                    pst_attr->st_bte.u08_redefine_timing_en = 1; 
                    pst_attr->st_bte.u16_vsync_bp = 50;
                    pst_attr->st_bte.u16_vsync_per = 22;
                    pst_attr->st_bte.u16_vsync_fp = 40;
                    pst_attr->st_bte.u16_hsync_bp = 60;
                    pst_attr->st_bte.u16_hsync_per = 18;
                    pst_attr->st_bte.u16_hsync_fp = 30;
                    pst_attr->st_bte.u16_output_width = 384;
                    pst_attr->st_bte.u16_output_height = 288;          
                    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
                    {
                        pst_attr->st_bte.u16_output_width = 768;
                        pst_attr->st_bte.u16_output_height = 576;
                    }
                }
                else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_30HZ){
                    pst_attr->st_bte.u08_redefine_timing_en = 1; 
                    pst_attr->st_bte.u16_vsync_bp = 20;
                    pst_attr->st_bte.u16_vsync_per = 4;
                    pst_attr->st_bte.u16_vsync_fp = 16;
                    pst_attr->st_bte.u16_hsync_bp = 60;
                    pst_attr->st_bte.u16_hsync_per = 32;
                    pst_attr->st_bte.u16_hsync_fp = 32;
                    pst_attr->st_bte.u16_output_width = 384;
                    pst_attr->st_bte.u16_output_height = 288;            
                    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)// 超分使能 384x288@50-->768x576@50超分
                    {
                        pst_attr->st_bte.u16_output_width = 768;
                        pst_attr->st_bte.u16_output_height = 576;
                    }
                }
            }
            else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI){
                    pst_attr->st_mipi.u08_en = 1;
                    pst_attr->st_mipi.u08_dsi_en = 0;
                    pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
                    if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                        pst_attr->st_mipi.u08_dphy_lane_num = 4;
                        pst_attr->st_mipi.u16_data_rate = 200;
                        if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)//  超分使能 384x288@50-->768x576@50超分
                        {
                            pst_attr->st_mipi.u16_data_rate = 400;//2lane时的数据率400Mbps/lane
                        }
                    }
                    else{
                        pst_attr->st_mipi.u08_dphy_lane_num = 2;
                        pst_attr->st_mipi.u16_data_rate = 400;
                        if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_384X288_768X576)//超分使能 384x288@50-->768x576@50超分，则2lane时的数据率800Mbps/lane
                        {
                        pst_attr->st_mipi.u16_data_rate = 800;//2lane时的数据率800Mbps/lane
                        }
                    }
                    pst_attr->st_mipi.u08_clock_continue = 0;
                    pst_attr->st_bte.u08_en = 0;
                    pst_attr->u32_mem_en = 0;
            }
        }
    }
    if(snr_cfg_vpo(pst_attr)==TS_OK)
    {
        ts_cpl_msleep(1000);
        vot_uvc_usr_reset();
        vot_refresh_osd_bind();
    }

}


snr_register(planck640c, start_planck640c);
snr_register(planck384c, start_planck384c);
snr_register(planck1280c, start_planck1280c);


