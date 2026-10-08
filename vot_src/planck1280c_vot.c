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
#if 0
//光智1280探测器的256位操作模式数据的组成情况如下表
ts_viu_snr_cfg_s_t sast_rv1280_cfg[] = {
    {0x0000001f, 0x00000003},//[255:224]
    {0x0000001e, 0x1B80ECC6},//[223:192]
    {0x0000001d, 0x48780653},//[191:160]
    {0x0000001c, 0xB1653AF4},//[159:128]
    {0x0000001b, 0x5C031F88},//[127:96]
    {0x0000001a, 0xFF025CA4},//[95:64]
    {0x00000019, 0xE4E4375C},//[63:32]
    {0x00000018, 0x0BC68AF0},//[31:0]
};
// 配置数组长度（自动计算）
#define RV1280_CFG_CNT  (sizeof(sast_rv1280_cfg) / sizeof(sast_rv1280_cfg[0]))
static ts_viu_snr_attr_s_t sst_rv1280_attr = {
    .u16_uint_len = 256 - 1,
    .u16_cfg_num = sizeof(sast_rv1280_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_rv1280_cfg,
};

/**
 * @brief 1字节 高位bit与低位bit互换（8位位序反转）
 * @param byte 输入的1字节数据
 * @return 高低位互换后的1字节数据
 * 1 字节是 8 位二进制，高位低位互换 = bit7 ↔ bit0、bit6 ↔ bit1、bit5 ↔ bit2、bit4 ↔ bit3 对称反转。
比如：
原字节：1011 0101 (0xB5)
互换后：1010 1101 (0xAD)
 */
uint8_t byte_bit_reverse(uint8_t byte)
{
    // 高效位运算实现 8位高低位反转
    byte = ((byte & 0x01) << 7) | ((byte & 0x02) << 5) |
           ((byte & 0x04) << 3) | ((byte & 0x08) << 1) |
           ((byte & 0x10) >> 1) | ((byte & 0x20) >> 3) |
           ((byte & 0x40) >> 5) | ((byte & 0x80) >> 7);
    return byte;
}


/**
 * 从 256 位缓冲区中读取指定位数的数据（≤32位）
 * 
 * @param buf      缓冲区（32 字节）
 * @param bit_pos  起始位位置（0 ~ 255）
 * @param width    要读取的位数（1 ~ 32）
 * @return         读取到的数据（低 width 位有效）
 */
// uint32_t read_bits(const uint8_t buf[BUFFER_BYTES], int bit_pos, int width) 
uint32_t read_bits(const uint8_t *buf, int bit_pos, int width) 
{
    if (width <= 0 || width > 32) return 0;
    if (bit_pos < 0 || bit_pos + width > 256) return 0;
    
    int byte_idx = bit_pos / 8;
    int bit_offset = bit_pos % 8;
    uint32_t result = 0;
    
    if (width <= 8 - bit_offset) {
        // 完全在一个字节内
        result = (buf[byte_idx] >> bit_offset) & ((1U << width) - 1);
    } else {
        // 跨越字节边界
        int bits_from_first = 8 - bit_offset;
        result = (buf[byte_idx] >> bit_offset) & ((1U << bits_from_first) - 1);
        
        int remaining = width - bits_from_first;
        int current_byte = byte_idx + 1;
        
        while (remaining > 0) {
            int bits_this_byte = (remaining < 8) ? remaining : 8;
            result |= ((uint32_t)(buf[current_byte] & ((1U << bits_this_byte) - 1)) << bits_from_first);
            bits_from_first += bits_this_byte;
            remaining -= bits_this_byte;
            current_byte++;
        }
    }
    
    return result;
}

/**
 * 向 256 位缓冲区中写入指定位数的数据（≤32位）
 * 
 * @param buf      缓冲区（32 字节）
 * @param bit_pos  起始位位置（0 ~ 255）
 * @param width    要写入的位数（1 ~ 32）
 * @param value    待写入的数据（只使用低 width 位）
 */
//int write_bits(uint8_t buf[BUFFER_BYTES], int bit_pos, int width, uint32_t value) 
int write_bits(uint8_t *buf, int bit_pos, int width, uint32_t value) 
{
    if (width <= 0 || width > 32) return -1;
    if (bit_pos < 0 || bit_pos + width > 256) return -1;
    
    int byte_idx = bit_pos / 8;
    int bit_offset = bit_pos % 8;
    
    // 只保留低 width 位
    value &= (width == 32) ? 0xFFFFFFFF : ((1U << width) - 1);
    
    if (width <= 8 - bit_offset) {
        // 完全在一个字节内
        uint8_t mask = ((1U << width) - 1) << bit_offset;
        buf[byte_idx] = (buf[byte_idx] & ~mask) | ((value << bit_offset) & mask);
    } else {
        // 跨越字节边界
        int bits_to_first = 8 - bit_offset;
        uint8_t first_mask = ((1U << bits_to_first) - 1) << bit_offset;
        buf[byte_idx] = (buf[byte_idx] & ~first_mask) | ((value << bit_offset) & first_mask);
        
        int remaining = width - bits_to_first;
        int current_byte = byte_idx + 1;
        uint32_t shifted_value = value >> bits_to_first;
        
        while (remaining > 0) {
            int bits_this_byte = (remaining < 8) ? remaining : 8;
            uint8_t this_mask = (1U << bits_this_byte) - 1;
            buf[current_byte] = (buf[current_byte] & ~this_mask) | (shifted_value & this_mask);
            shifted_value >>= bits_this_byte;
            remaining -= bits_this_byte;
            current_byte++;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
// 函数：ts_tdk_snr_get_sdb
// 功能：根据 msb+lsb 地址，读取 sast_rv1280_cfg 中对应的值
// 参数：pst_snr_sd - 传入地址，传出读取结果
// 返回：TS_OK 成功 / 其他 失败
//------------------------------------------------------------------------------
ts_s32 vot_ts_tdk_snr_get_sdb(ts_snr_sd_s_t *pst_snr_sd)
{
    ts_u32 i;
    ts_u08 Data[RV1280_CFG_CNT*4];
    ts_u32 u32_val;

    // 1. 参数合法性检查
    if (pst_snr_sd == NULL ) {
        return -1; // 指针非法
    }
    for (i = 0; i < RV1280_CFG_CNT; i++) 
    {
           Data[4*i] = sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val&0xff;
           Data[4*i+1] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>8)&0xff;
           Data[4*i+2] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>16)&0xff;
           Data[4*i+3] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>24)&0xff;
    }
    if(SystemParaMenu2.SmartData256bitValidFlag==1)
    {
        for (i = 0; i < RV1280_CFG_CNT*4; i++) 
        {
            Data[i] = SystemParaMenu2.SmartData256bit[RV1280_CFG_CNT*4-1-i];
        }
    }
    uint32_t length = pst_snr_sd->u32_msb-pst_snr_sd->u32_lsb+1; 
    uint32_t start_bit = pst_snr_sd->u32_lsb; 
    u32_val =read_bits(Data, start_bit, length);

    // *(pst_snr_sd->u32_rwdata) = u32_val;
    pst_snr_sd->u32_rwdata = u32_val;
    return TS_OK;
}

//------------------------------------------------------------------------------
// 函数：ts_tdk_snr_set_sdb
// 功能：根据 msb+lsb 地址，写入 sast_rv1280_cfg 中对应的值
// 参数：pst_snr_sd - 传入地址 + 待写入数据
// 返回：TS_OK 成功 / 其他 失败
//------------------------------------------------------------------------------
ts_s32 vot_ts_tdk_snr_set_sdb(ts_snr_sd_s_t *pst_snr_sd)
{
    ts_u32 i;
    ts_u08 Data[RV1280_CFG_CNT*4];

    // 1. 参数合法性检查
    if (pst_snr_sd == NULL) {
        return -1; // 指针非法
    }
    for (i = 0; i < RV1280_CFG_CNT; i++) 
    {
           Data[4*i] = sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val&0xff;
           Data[4*i+1] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>8)&0xff;
           Data[4*i+2] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>16)&0xff;
           Data[4*i+3] = (sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val>>24)&0xff;
    }
    if(SystemParaMenu2.SmartData256bitValidFlag==1)
    {
        for (i = 0; i < RV1280_CFG_CNT*4; i++) 
        {
            Data[i] = SystemParaMenu2.SmartData256bit[RV1280_CFG_CNT*4-1-i];
        }
    }
    
    uint32_t start_bit = pst_snr_sd->u32_lsb;
    uint32_t length = pst_snr_sd->u32_msb - pst_snr_sd->u32_lsb+1; 
    if (length == 0 || length > 32)
    {
        return -1; // 长度非法
    }
    if(write_bits(Data, start_bit, length, pst_snr_sd->u32_rwdata) != 0) 
    {
        return -1; // 设置失败
    }
    //SystemParaMenu2.SmartData256bitValidFlag=1;
    if(SystemParaMenu2.SmartData256bitValidFlag==1)
    {
        for (i = 0; i < RV1280_CFG_CNT*4; i++) 
        {
            SystemParaMenu2.SmartData256bit[i] = Data[RV1280_CFG_CNT*4-1-i];
        }
    }
    for(i = 0; i < RV1280_CFG_CNT; i++) 
    {
        sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val = (Data[4*i]) | (Data[4*i+1]<<8) | (Data[4*i+2]<<16) | (Data[4*i+3]<<24);
        sst_rv1280_attr.pst_cfg[RV1280_CFG_CNT-1-i].u32_val =sast_rv1280_cfg[RV1280_CFG_CNT-1-i].u32_val;
    }

    //ts_tdk_viu_disable(TS_VIU_MDEV0);
    ts_tdk_viu_set_snr_attr(TS_VIU_MDEV0, &sst_rv1280_attr);
    //ts_tdk_viu_enable(TS_VIU_MDEV0);
   
    return TS_OK;
}
#endif

ts_u32 planck1280_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb)
{
    ts_snr_sd_s_t st_snr_sd;
    st_snr_sd.u32_lsb = u32_lsb;
    st_snr_sd.u32_msb = u32_msb;
    ts_s32 ret =0;
    //ret = vot_ts_tdk_snr_get_sdb(&st_snr_sd);
    ret = ts_tdk_snr_get_sdb(&st_snr_sd);
    if(ret!=TS_OK){
        vision_log_debug("msb %d lsb %d  is illegal!\n",u32_msb,u32_lsb);
    }
    vision_log_debug("planck1280_read_sd: msb %d lsb %d  result: %u\n",u32_msb,u32_lsb,  st_snr_sd.u32_rwdata);
    return  st_snr_sd.u32_rwdata;
}

ts_void planck1280_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata)
{
    ts_snr_sd_s_t st_snr_sd;
    st_snr_sd.u32_lsb = u32_lsb;
    st_snr_sd.u32_msb = u32_msb;
    st_snr_sd.u32_rwdata = u32_wdata;
    ts_s32 ret =0;
    //ret = vot_ts_tdk_snr_set_sdb(&st_snr_sd);
    ret = ts_tdk_snr_set_sdb(&st_snr_sd);
    if(ret!=TS_OK){
        vision_log_debug("msb %d lsb %d  u32_wdata %u is illegal!\n",u32_msb,u32_lsb, u32_wdata);
    }
}

ts_u32 vot1280_detetor_read_sd(ts_u32 u32_msb,ts_u32 u32_lsb)
{
    ts_u32 ret=0;
    #ifdef CONFIG_SNR_PLANCK1280C
    ret=planck1280_read_sd(u32_msb, u32_lsb);
    #endif
    return ret;
}


ts_void vot1280_detetor_write_sd(ts_u32 u32_msb,ts_u32 u32_lsb,ts_u32 u32_wdata)
{
    #ifdef CONFIG_SNR_PLANCK1280C
    return planck1280_write_sd( u32_msb, u32_lsb, u32_wdata);
    #endif
}


//光智1280探测器Det参数256bit写
ts_void planck1280c_write_gain_adc(ts_u32 u32_gain_adc)
{//Gain
    vot1280_detetor_write_sd(151, 145, u32_gain_adc);
}

ts_void planck1280c_write_gof(ts_u32 u32_gof)
{//gof
    vot1280_detetor_write_sd(71, 65, u32_gof);
}

ts_void planck1280c_write_cint(ts_u32 u32_cint)
{//Cint
    vot1280_detetor_write_sd(56, 54, u32_cint);
}

ts_void planck1280c_write_Nint(ts_u32 u32_Nint)
{//Nint
    vot1280_detetor_write_sd(22, 12, u32_Nint);
}

ts_void planck1280c_write_Sel_PJ(ts_u32 u32_Sel_PJ)
{//Sel_PJ
    vot1280_detetor_write_sd(171, 171, u32_Sel_PJ);
}

ts_void planck1280c_write_NTCC(ts_u32 u32_NTCC)
{//NTCC
    vot1280_detetor_write_sd(98, 92, u32_NTCC);
}

ts_void planck1280c_write_Voccm(ts_u32 u32_Voccm)
{//Voccm
    vot1280_detetor_write_sd(64, 59, u32_Voccm);
}
ts_void planck1280c_write_detConfig(ts_u32 u32_detConfig)
{//DET Config
    vot1280_detetor_write_sd(27, 27, u32_detConfig);
}

ts_void planck1280c_write_Vfid(ts_u32 u32_Vfid)
{//Vfid
    vot1280_detetor_write_sd(78, 72, u32_Vfid);
}

//光智1280探测器Det参数256bit读
ts_u32 planck1280c_read_gain_adc(ts_void)
{//gain adc
    return vot1280_detetor_read_sd(151, 145);
}

ts_u32 planck1280c_read_gof(ts_void)
{//gof
    return vot1280_detetor_read_sd(71, 65);
}

ts_u32 planck1280c_read_Cint(ts_void)
{//Cint
    return vot1280_detetor_read_sd(56, 54);
}
ts_u32 planck1280c_read_Nint(ts_void)
{//Nint
    return vot1280_detetor_read_sd(22, 12);
}

ts_u32 planck1280c_read_Sel_PJ(ts_void)
{//Sel_PJ
    return vot1280_detetor_read_sd(171, 171);
}

ts_u32 planck1280c_read_NTCC(ts_void)
{
    return vot1280_detetor_read_sd(98, 92);
}

ts_u32 planck1280c_read_Voccm(ts_void)
{
    return vot1280_detetor_read_sd(64, 59);
}

ts_u32 planck1280c_read_detConfig(ts_void)
{
    return vot1280_detetor_read_sd(27, 27);
}

ts_u32 planck1280c_read_Vfid(ts_void)
{
    return vot1280_detetor_read_sd(78, 72);
}


ts_u16 planck1280c_get_tes(ts_void)
{
    ts_u32 u32_fv=0;
    // ts_u16  u16_fv=0;
    ts_s32 s32_ret;
    ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    //此处需要调用两次ts_tdk_viu_get_stats，才能获取稳定的tes值，具体原因需要进一步分析
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1295, 0, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    // vision_log_debug("first test 0 u32_mean  0x%X  \n",u32_mean);
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1295, 0, (ts_u32 *)&u32_fv, &u32_mean);
    if (TS_OK == s32_ret) {
        u32_mean = u32_mean & 0x3fff;
       // vision_log_debug("planck1280c_get_tes u32_mean 0x%X \n", u32_mean);
    } else {
        u32_mean = 0;
    }
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    return u32_mean;
}

#if 1
ts_s32 planck1280c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1294, 400, (ts_u32 *)&u32_fv, &u32_mean); 
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
    // vision_log_debug("planck1280c_get_temperature_info u32_mean 0x%x  f32_fv [%f]\n",u32_mean,f32_fv);

    return s32_ret;
}
#else
static ts_viu_corr_rst_s_t st_rst_attr;
ts_s32 planck1280c_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;
    static ts_u32 u16_coor_y=0;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1294, u16_coor_y, (ts_u32 *)&u32_fv, &u32_mean); 
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

    if(f32_fv<30 ||f32_fv>38)
    {
       // u16_coor_y++;
        u16_coor_y=u16_coor_y+2;
        if(u16_coor_y>900)
        {
            u16_coor_y=0;
        }
        vision_log_debug("u16_coor_y:%d  u32_mean:0x%x  f32_fv:[%f]\n",u16_coor_y,u32_mean,f32_fv);
    }
    else
    vision_log_debug("planck1280c_get_temperature_info u16_coor_y:%d  u32_mean:0x%x  f32_fv:[%f]\n",u16_coor_y,u32_mean,f32_fv);

  if(u16_coor_y==40)
  {
    // ISP_pause();
    // ts_cpl_msleep(2000);
    // ISP_auto_cut_off();
    //reset occ value only ， not calc occ
    st_rst_attr.u08_reset_val =0x80;// st_rst_attr.u08_reset_val+1;
    if(st_rst_attr.u08_reset_val > 0xfe) {
        st_rst_attr.u08_reset_val = 0;
    }
    vision_log_debug("st_rst_attr.u08_reset_val = 0x%x st_rst_attr.u16_rev = 0x%x \n", st_rst_attr.u08_reset_val, st_rst_attr.u16_rev);
    st_rst_attr.u08_reset_only = 1;
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);  
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    // vision_log_debug("planck1280c_get_temperature_info u32_mean 0x%x  f32_fv [%f]\n",u32_mean,f32_fv);
    return s32_ret;
}
#endif

extern ts_viu_snr_attr_s_t sst_vot_planck1280_attr;
extern ts_viu_snr_cfg_s_t sast_planck1280_vot_cfg;

ts_void vot_planck1280c_write_config_256bit(ts_u08* config_256bit)
{
    ts_u32 *u32_value=NULL;
    // ts_u08* data = config_408bit;
    ts_u08 data[32];
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;

    pst_ctx =&gst_snr_ctx;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;

    // vision_log_debug("enter:  ");
    if(config_256bit==NULL){
        return;
    }
    
    int i=0;
    int index=31;
    data[0] = 0x0;
    for(i=0; i<32; i++){
        data[i]=config_256bit[index];
        // vision_log_debug("index: %d i %d data[i] %x \n ", index, i,data[i]);
        index--;
    }
    u32_value =(ts_u32*) &data[0];
    // for(i=0; i<8; i++){
    //     vision_log_debug(" i %d u32_value[i] %x \n ",  i,u32_value[i]);
    // }
    for(index=0; index<8; index++){
        sst_vot_planck1280_attr.pst_cfg[index].u32_val = u32_value[7-index];
        // vision_log_debug(" index %d u32_value %x \n ",  index,u32_value[7-index]);
    }
    ts_tdk_viu_disable(TS_VIU_MDEV0);
    planck1280c_viu_init(pst_attr, &sst_vot_planck1280_attr);
    ts_tdk_viu_enable(TS_VIU_MDEV0);
    // vision_log_debug("leave:  ");
}




ts_void brd_planck1280_vot_init(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

    board_type_t board_name =  get_board_port_type();

    FOCUS_TYPE_ENUM focusType = get_focus_type();

    vision_log_debug(" board_name %d, focusType %d \n",board_name, focusType);


    if(board_name == BOARD_PORT_UAD3_LITE){

        vision_log_debug(" normal 1280  1 board \n");
        /* Sensor misc io mux */
        ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SPI1_MOSI, TS_IO_SPI1_MOSI_FUN3_GPIO22);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//AVDD_EN
        ts_lib_gpio_set_mux(TS_IO_SPI1_CSN1, TS_IO_SPI1_CSN1_FUN3_GPIO25);//DVDD_EN
        ts_lib_gpio_set_mux(TS_IO_I2C2_SDA, TS_IO_I2C2_SDA_FUN3_GPIO4);//VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_I2C2_SCL, TS_IO_I2C2_SCL_FUN3_GPIO5);//VDET_EN2
        ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//Motor_EN

        //用于自动调焦相关功能
        ts_lib_gpio_set_mux(TS_IO_PWM0, TS_IO_PWM0_FUN0);
        ts_lib_gpio_set_mux(TS_IO_PWM1, TS_IO_PWM1_FUN0);

        if(focusType == FOCUS_TYPE_ZOOM_LENS_ENCODER)
        {
            ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN2_PA0_PWM);
            ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN2_PB0_PWM);
            /* 变倍电机*/
            ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
            ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
            ts_lib_gpio_set_mux(TS_IO_GPIO60,TS_IO_GPIO60_FUN2_PA1_PWM);
            ts_lib_gpio_set_mux(TS_IO_GPIO61,TS_IO_GPIO61_FUN2_PB1_PWM);
        }
        else if(focusType == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER)
        {
            ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN3_PWM2);
            ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN3_PWM3);
        }
        //I2C0 总线共用 测温和电机
        // ts_lib_gpio_set_mux(TS_IO_I2C0_SDA, TS_IO_I2C0_SDA_FUN0);
        // ts_lib_gpio_set_mux(TS_IO_I2C0_SCL, TS_IO_I2C0_SCL_FUN0);

        //根据以往的经验，探测器上电必须要先使能3.3v，再是能11v，否则，探测器可能出现问题
        //AVDD_EN&DVDD_EN
        ts_cpl_msleep(1000);
        snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);//LDO set to high AVDD
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(25, TS_ELEC_LVL_HI);//LDO set to high DVDD POWER_ON
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(4, TS_ELEC_LVL_HI);//VDET_CTRL set to high VDET_EN1
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(5, TS_ELEC_LVL_HI); //VDET_CTRL set to high VDET_EN2

        //Motor_EN
        snr_gpio_cfg2out(46, TS_ELEC_LVL_HI);

        /*******************
        ** IO DRIVER LEVEL
        *******************/
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT2, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT3, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT4, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT5, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3); 

        /* 配置探测器复位、电源、开门的 IO 控制信息 */
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        st_snr_io_cfg.s32_rst_gpio_num = -1;
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;

        st_snr_io_cfg.s32_sht_p_gpio_num = 21;
        st_snr_io_cfg.s32_sht_n_gpio_num = 22;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;
    }
    else if( get_board_port_type()  == BOARD_PORT_UAD3){

 //如果是vot_planck1280c，则按照1280探测器的方式配置IO，否则按照640探测器的方式配置IO
        vision_log_debug(" normal 1280  2 board\n");
        /* Sensor misc io mux */
        ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//SHTCTL0
        ts_lib_gpio_set_mux(TS_IO_SPI1_MOSI, TS_IO_SPI1_MOSI_FUN3_GPIO22);//SHTCTL1
        ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//AVDD_EN
        ts_lib_gpio_set_mux(TS_IO_SPI1_CSN1, TS_IO_SPI1_CSN1_FUN3_GPIO25);//DVDD_EN
        ts_lib_gpio_set_mux(TS_IO_PWM0, TS_IO_PWM0_FUN3_GPIO30);//VDET_EN1
        ts_lib_gpio_set_mux(TS_IO_PWM1, TS_IO_PWM1_FUN3_GPIO31);//VDET_EN2

        //用于自动调焦相关功能
        if(focusType == FOCUS_TYPE_ZOOM_LENS_ENCODER)
        {
            ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN2_PA0_PWM);
            ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN2_PB0_PWM);
            /* 变倍电机*/
            ts_lib_gpio_set_mux(TS_IO_UART1_TXD,TS_IO_UART1_TXD_FUN1_PWM2);
            ts_lib_gpio_set_mux(TS_IO_UART1_RXD,TS_IO_UART1_RXD_FUN1_PWM3);
            ts_lib_gpio_set_mux(TS_IO_GPIO60,TS_IO_GPIO60_FUN2_PA1_PWM);
            ts_lib_gpio_set_mux(TS_IO_GPIO61,TS_IO_GPIO61_FUN2_PB1_PWM);
        }
        else if(focusType == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER)
        {
            ts_lib_gpio_set_mux(TS_IO_GPIO58, TS_IO_GPIO58_FUN3_PWM2);
            ts_lib_gpio_set_mux(TS_IO_GPIO59, TS_IO_GPIO59_FUN3_PWM3);
        }
         //根据以往的经验，探测器上电必须要先使能3.3v，再是能11v，否则，探测器可能出现问题
        //AVDD_EN&DVDD_EN
        ts_cpl_msleep(1000);
        snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);//LDO set to high AVDD
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(25, TS_ELEC_LVL_HI);//LDO set to high DVDD POWER_ON
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(30, TS_ELEC_LVL_HI);//VDET_CTRL set to high VDET_EN1
        ts_cpl_msleep(100);
        snr_gpio_cfg2out(31, TS_ELEC_LVL_HI); //VDET_CTRL set to high VDET_EN2
        
        /* 配置探测器复位、电源、开门的 IO 控制信息 */
        st_snr_io_cfg.s32_pwr_gpio_num = -1;
        st_snr_io_cfg.s32_rst_gpio_num = -1;
        st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
        st_snr_io_cfg.u32_rst_dly = 100000;
        st_snr_io_cfg.s32_sht_p_gpio_num = 21;
        st_snr_io_cfg.s32_sht_n_gpio_num = 22;
        st_snr_io_cfg.u32_sht_dbg = 0;
        st_snr_io_cfg.u32_sht_hold = 60000;
        
        /*******************
        ** IO DRIVER LEVEL
        *******************/
        ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT2, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT3, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT4, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT5, 3);
        ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3); 
    }
    else if(get_board_port_type() == BOARD_PORT_UAW) 
    {
   
    }
    else if(get_board_port_type() == BOARD_PORT_UAW_SE_LITE) 
    {
     
    }
    
    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();

} 






