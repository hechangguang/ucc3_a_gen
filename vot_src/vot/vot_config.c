/*
 * @Author: shixiaobo 328494516@qq.com
 * @Date: 2025-10-09 14:29:32
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-04 13:37:11
 * @FilePath: /ucc3_a_gen/vot_src/vot/vot_config.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */


#include <stdint.h>
#include <string.h>

#include <menu/votmenu.h>
#include <api.h>
#include <vot_config.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

extern struct Menu2_Para SystemParaMenu2;
extern SysCtrlInfo_s_t global_sys_ctrl_info;

/* 通过链接脚本符号访问 */
extern const uint8_t __ispcfg_start[];
extern const uint8_t __ispcfg_end[];
ts_u08 m_offset_index[DEVICE_MODE_BUTT]={0};

/* 强制符号保留（防止链接器优化）*/
__attribute__((used)) 
static const void* _keep_ispcfg_ref[] = { __ispcfg_start, __ispcfg_end };

volatile user_cfg_header_s_t usr_cfg_header;
volatile  user_cfg_node_header_s_t usr_cfg_node;

#ifdef CONFIG_SNR_PLANCK1280C
ts_void vot_update_isp_cfg_offset_index(ts_u08* offset_index_ptr)
{
    if(offset_index_ptr==ts_null)
    {
        vision_log_err("offset_index_ptr=%p \n",offset_index_ptr);
        return;
    }

    //更新实际偏移值
    ts_u08 offset_base = 0;
    ts_s32 index=0;

    for(index=0; index<DEVICE_MODE_1280_BUTT; index++){
     
        switch(index)
        {
            case DEVICE_MODE_1280_R1:
            case DEVICE_MODE_1280_R2:
            case DEVICE_MODE_1280_R2_1:
            case DEVICE_MODE_1280_R2_2:
            case DEVICE_MODE_1280_R2_3:
            {
                offset_index_ptr[index] = offset_base;
                offset_base++;
                break;
            }
            default:
            {
                offset_index_ptr[index] = 0;
                break;
            }
        }
        
    }
}
#else
ts_void vot_update_isp_cfg_offset_index(ts_u08* offset_index_ptr)
{
    if(offset_index_ptr==ts_null)
    {
        vision_log_err("offset_index_ptr=%p \n",offset_index_ptr);
        return;
    }

    //更新实际偏移值
    ts_u08 offset_base = 0;
    ts_s32 index=0;

    for(index=0; index<DEVICE_MODE_BUTT; index++){
     
        switch(index)
        {
            case DEVICE_MODE_INDOOR:
            case DEVICE_MODE_OUTDOOR:
            case DEVICE_MODE_OUTDOOR_K1:
            case DEVICE_MODE_OUTDOOR_FLY:
            case DEVICE_MODE_OUTDOOR_FLY_ENH:
            case DEVICE_MODE_OUTDOOR_NUC_MANUAL:
            case DEVICE_MODE_OUTDOOR_RTD6:
            case DEVICE_MODE_OUTDOOR_GUAN_NIAO:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5:
            case DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6:
            case DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT:
            case DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT:
            case DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT:
            case DEVICE_MODE_OUTDOOR_CITY_IRON_HOT:
            case DEVICE_MODE_OUTDOOR_CITY_RED_HOT:
            case DEVICE_MODE_OUTDOOR_BPC:
            case DEVICE_MODE_ISO_MODE:
            case DEVICE_MODE_384_LOW_ATD:
            case DEVICE_MODE_384_LOW_ATD_RB:
            {
                offset_index_ptr[index] = offset_base;
                offset_base++;
                break;
            }
            default:
            {
                offset_index_ptr[index] = 0;
                break;
            }
        }
        
    }
}
#endif


#ifdef CONFIG_SNR_PLANCK1280C
/* 获取配置数据 */
void load_isp_config(void) 
{

    size_t cfg_size = sizeof(user_cfg_header_s_t) ;
    ts_u08 *pu08_dst_buf = (ts_u08*)&usr_cfg_header;
    ts_u32 rd_offset=0;
    ts_u32 want_rd_size = 4096;
    ts_u32 u32_addr = (ts_u32)__ispcfg_start;

    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,4096*10) ; 
    if(pu08_rd_buf==ts_null){
        vision_log_err("pu08_rd_buf==ts_null \n");
        return;
    }
 
    copy_isp_config(pu08_dst_buf, (uint8_t *)__ispcfg_start,  cfg_size);
    vision_log_debug(" usr_cfg_header.cfg_node_nums=%d \n",usr_cfg_header.cfg_node_nums);
    vot_update_isp_cfg_offset_index(m_offset_index);
    int cnt = 0;   
    for(cnt=0; cnt<usr_cfg_header.cfg_node_nums; cnt++){
        vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name %s \n",cnt , usr_cfg_header.cfg_node_array[cnt].name );
        rd_offset =  cfg_size;
        u32_addr = (ts_u32)__ispcfg_start;
        if(cnt ==  m_offset_index[DEVICE_MODE_1280_R1]  ){
            if( UsermenuParameter.m_dev_env_mode  == DEVICE_MODE_1280_R1){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;
                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x  \n",u32_addr,want_rd_size);
                vision_log_debug(" m_offset_index [%d] \n",m_offset_index[DEVICE_MODE_1280_R1] );
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_1280_R1 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_1280_R2]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_1280_R2){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_1280_R2 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_1280_R2_1]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_1280_R2_1){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_1280_R2_1 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_1280_R2_2]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_1280_R2_2){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_1280_R2_2 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_1280_R2_3]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_1280_R2_3){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_1280_R2_3 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        
    }
    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf);  
    }
   
}

#else
/* 获取配置数据 */
void load_isp_config(void) 
{

    size_t cfg_size = sizeof(user_cfg_header_s_t) ;
    ts_u08 *pu08_dst_buf = (ts_u08*)&usr_cfg_header;
    ts_u32 rd_offset=0;
    ts_u32 want_rd_size = 4096;
    ts_u32 u32_addr = (ts_u32)__ispcfg_start;


    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,4096*10) ; 
    if(pu08_rd_buf==ts_null){
        vision_log_err("pu08_rd_buf==ts_null \n");
        return;
    }

    /* 带内存屏障的拷贝 */
    // __asm volatile ("fence iorw,iorw" ::: "memory");
    // // memcpy(pu08_dst_buf, header, max_size);
    // memcpy(pu08_dst_buf, __ispcfg_start, cfg_size);
    // __asm volatile ("fence iorw,iorw" ::: "memory");

    copy_isp_config(pu08_dst_buf, (uint8_t *)__ispcfg_start,  cfg_size);

    // vision_log_debug("copy_isp_config\n");
    // vision_log_debug(" usr_cfg_header.base_addr=0x%x \n",usr_cfg_header.base_addr);
    vision_log_debug(" usr_cfg_header.cfg_node_nums=%d \n",usr_cfg_header.cfg_node_nums);
    // vision_log_debug(" usr_cfg_header.cfg_total_bytes=%d \n",usr_cfg_header.cfg_total_bytes);

    vot_update_isp_cfg_offset_index(m_offset_index);
                

    //vision_log_warn("UsermenuParameter.m_dev_env_mode %d \n",UsermenuParameter.m_dev_env_mode);
    int cnt = 0;   
    for(cnt=0; cnt<usr_cfg_header.cfg_node_nums; cnt++){
        // vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name %s \n",cnt , usr_cfg_header.cfg_node_array[cnt].name );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].offset  0x%x \n", cnt ,usr_cfg_header.cfg_node_array[cnt].offset );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].size  0x%x \n",cnt , usr_cfg_header.cfg_node_array[cnt].size );

        rd_offset =  cfg_size;
        u32_addr = (ts_u32)__ispcfg_start;
        // vision_log_debug(" __ispcfg_start=0x%x cfg_size 0x%x \n",__ispcfg_start,cfg_size );
        // vision_log_debug(" cnt %d m_dev_env_mode %d \n",cnt,UsermenuParameter.m_dev_env_mode );
        if(cnt ==  m_offset_index[DEVICE_MODE_INDOOR]  ){
            if( UsermenuParameter.m_dev_env_mode  == DEVICE_MODE_INDOOR){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x  \n",u32_addr,want_rd_size);
                vision_log_debug(" m_offset_index [%d] \n",m_offset_index[DEVICE_MODE_INDOOR] );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_INDOOR \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_K1]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_K1){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_K1 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_FLY]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_FLY \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_FLY_ENH]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_FLY_ENH){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_FLY_ENH \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_NUC_MANUAL]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_NUC_MANUAL \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_RTD6]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_RTD6){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_RTD6 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_GUAN_NIAO]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_GUAN_NIAO){

                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_GRAY_TAB;
                load_palette(UsermenuParameter.Palette);
                
                // UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
                // UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_GUAN_NIAO \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V1 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V2 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V3 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V4 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V5 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6){


                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_LOW_ATD_DDE_V6 \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT){

                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_GRAY_TAB;
                load_palette(UsermenuParameter.Palette);

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT){
                
                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_GRAY_TAB;
                load_palette(UsermenuParameter.Palette);
                // UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
                // UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;

                
                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_CITY_WHITE_HOT \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT){
                
                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_GRAY_INV_TAB;
                load_palette(UsermenuParameter.Palette);
                // UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
                // UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;

                
                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_CITY_BLACK_HOT \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else  if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_CITY_IRON_HOT]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_IRON_HOT){
                
                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_IRON_HOT2_TAB;
                load_palette(UsermenuParameter.Palette);
                // UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
                // UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
                
                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_CITY_IRON_HOT \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }       
        else if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_CITY_RED_HOT]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_CITY_RED_HOT){
                
                UsermenuParameter.Palette = VOT_COLOR_PALETTE_MODE_RED_HOT_TAB;
                load_palette(UsermenuParameter.Palette);

                // UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
                // UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
                
                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                vision_log_debug(" m_offset_index [%d] \n",m_offset_index[DEVICE_MODE_OUTDOOR_CITY_RED_HOT] );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_OUTDOOR_CITY_RED_HOT \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_384_LOW_ATD]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_384_LOW_ATD){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                vision_log_debug(" m_offset_index [%d] \n",m_offset_index[DEVICE_MODE_384_LOW_ATD] );
                
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_384_LOW_ATD \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        else if(cnt ==  m_offset_index[DEVICE_MODE_384_LOW_ATD_RB]  ){
            if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_384_LOW_ATD_RB){

                ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
                rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
                u32_addr += rd_offset;

                vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n",u32_addr,want_rd_size );
                vision_log_debug(" m_offset_index [%d] \n",m_offset_index[DEVICE_MODE_384_LOW_ATD_RB] );
                
                want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
                
                copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
                ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
                ISP_set_usr_cfg(pst_isp_reg);
                vision_log_debug(" DEVICE_MODE_384_LOW_ATD_RB \n");
                
                ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

            }
        }
        
    }

    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf);  
        pu08_rd_buf = 0;
    }

}
#endif


#ifdef CONFIG_SNR_PLANCK1280C
/* 获取配置数据 */
void load_isp_config_for_bpc(void) 
{

    size_t cfg_size = sizeof(user_cfg_header_s_t) ;
    ts_u08 *pu08_dst_buf = (ts_u08*)&usr_cfg_header;
    ts_u32 rd_offset=0;
    ts_u32 want_rd_size = 4096;
    ts_u32 u32_addr = (ts_u32)__ispcfg_start;

    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,4096*10) ; 
    if(pu08_rd_buf==ts_null){
        vision_log_err("pu08_rd_buf==ts_null \n");
        return;
    }

    copy_isp_config(pu08_dst_buf, (uint8_t *)__ispcfg_start,  cfg_size);

    vision_log_debug("copy_isp_config\n");
    // vision_log_debug(" usr_cfg_header.base_addr=0x%x \n",usr_cfg_header.base_addr);
    vision_log_debug(" usr_cfg_header.cfg_node_nums=%d \n",usr_cfg_header.cfg_node_nums);
    // vision_log_debug(" usr_cfg_header.cfg_total_bytes=%d \n",usr_cfg_header.cfg_total_bytes);

    //更新实际偏移值
    ts_u08 m_offset_index[DEVICE_MODE_BUTT]={0};
 
    vot_update_isp_cfg_offset_index(m_offset_index);

    int cnt = 0;   
    for(cnt=0; cnt<usr_cfg_header.cfg_node_nums; cnt++){
        vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name %s \n",cnt , usr_cfg_header.cfg_node_array[cnt].name );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].offset  0x%x \n", cnt ,usr_cfg_header.cfg_node_array[cnt].offset );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].size  0x%x \n",cnt , usr_cfg_header.cfg_node_array[cnt].size );

        rd_offset =  cfg_size;
        u32_addr = (ts_u32)__ispcfg_start;


        if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_BPC]){

            ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
            rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
            u32_addr += rd_offset;

            vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x m_offset_index [%d] \n",u32_addr,want_rd_size,m_offset_index[DEVICE_MODE_OUTDOOR_BPC] );
            
            want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
            
            copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
            ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
            ISP_set_usr_cfg(pst_isp_reg);
            vision_log_debug(" DEVICE_MODE_OUTDOOR_BPC \n");
            
            ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

        }  
    }
  
    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf);  
    }
    // read_basic_fields();
    // traverse_config_nodes();
}
#else
/* 获取配置数据 */
void load_isp_config_for_bpc(void) 
{

    size_t cfg_size = sizeof(user_cfg_header_s_t) ;
    ts_u08 *pu08_dst_buf = (ts_u08*)&usr_cfg_header;
    ts_u32 rd_offset=0;
    ts_u32 want_rd_size = 4096;
    ts_u32 u32_addr = (ts_u32)__ispcfg_start;


    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,4096*10) ; 
    if(pu08_rd_buf==ts_null){
        vision_log_err("pu08_rd_buf==ts_null \n");
        return;
    }

    copy_isp_config(pu08_dst_buf, (uint8_t *)__ispcfg_start,  cfg_size);

    vision_log_debug("copy_isp_config\n");
    // vision_log_debug(" usr_cfg_header.base_addr=0x%x \n",usr_cfg_header.base_addr);
    vision_log_debug(" usr_cfg_header.cfg_node_nums=%d \n",usr_cfg_header.cfg_node_nums);
    // vision_log_debug(" usr_cfg_header.cfg_total_bytes=%d \n",usr_cfg_header.cfg_total_bytes);

    //更新实际偏移值
    vot_update_isp_cfg_offset_index(m_offset_index);

    int cnt = 0;   
    for(cnt=0; cnt<usr_cfg_header.cfg_node_nums; cnt++){
        vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name %s \n",cnt , usr_cfg_header.cfg_node_array[cnt].name );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].offset  0x%x \n", cnt ,usr_cfg_header.cfg_node_array[cnt].offset );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].size  0x%x \n",cnt , usr_cfg_header.cfg_node_array[cnt].size );

        rd_offset =  cfg_size;
        u32_addr = (ts_u32)__ispcfg_start;


        if(cnt ==  m_offset_index[DEVICE_MODE_OUTDOOR_BPC]){

            ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
            rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
            u32_addr += rd_offset;

            vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x m_offset_index [%d] \n",u32_addr,want_rd_size,m_offset_index[DEVICE_MODE_OUTDOOR_BPC] );
            
            
            want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
            
            copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
            ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
            ISP_set_usr_cfg(pst_isp_reg);
            vision_log_debug(" DEVICE_MODE_OUTDOOR_BPC \n");
            
            ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

        }
    }

    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf);  
    }

}
#endif

void load_isp_config_for_iso_mode(void) 
{

    size_t cfg_size = sizeof(user_cfg_header_s_t) ;
    ts_u08 *pu08_dst_buf = (ts_u08*)&usr_cfg_header;
    ts_u32 rd_offset=0;
    ts_u32 want_rd_size = 4096;
    ts_u32 u32_addr = (ts_u32)__ispcfg_start;
 
    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,4096*10) ; 
    if(pu08_rd_buf==ts_null){
        vision_log_err("pu08_rd_buf==ts_null \n");
        return;
    }

    copy_isp_config(pu08_dst_buf, (uint8_t *)__ispcfg_start,  cfg_size);

    vision_log_debug("copy_isp_config\n");
    vision_log_debug(" usr_cfg_header.cfg_node_nums=%d \n",usr_cfg_header.cfg_node_nums);

    vot_update_isp_cfg_offset_index(m_offset_index);

    int cnt = 0;   
    for(cnt=0; cnt<usr_cfg_header.cfg_node_nums; cnt++){
        vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name %s \n",cnt , usr_cfg_header.cfg_node_array[cnt].name );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].offset  0x%x \n", cnt ,usr_cfg_header.cfg_node_array[cnt].offset );
        // vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].size  0x%x \n",cnt , usr_cfg_header.cfg_node_array[cnt].size );

        rd_offset =  cfg_size;
        u32_addr = (ts_u32)__ispcfg_start;


        if(cnt ==  m_offset_index[DEVICE_MODE_ISO_MODE]){

            ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
            rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
            u32_addr += rd_offset;

            vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x m_offset_index [%d] \n",u32_addr,want_rd_size,m_offset_index[DEVICE_MODE_ISO_MODE] );
            
            
            want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
            
            copy_isp_config(pu08_rd_buf, (uint8_t *)u32_addr,  want_rd_size);
            ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu08_rd_buf;
            ISP_set_usr_cfg(pst_isp_reg);
            vision_log_debug(" DEVICE_MODE_ISO_MODE \n");
            
            ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);

        }
        
        
        
    }

    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf);  
    }

}



void copy_isp_config(uint8_t *dest, uint8_t *src, size_t cfg_size) 
{
    
    /* 安全检查 */
    if(cfg_size == 0 ) {
        vision_log_debug("Invalid ISP config size [%d] \n",cfg_size);
    }
 
    /* 带内存屏障的拷贝 */
    __asm volatile ("fence iorw,iorw" ::: "memory");
    memcpy(dest, src, cfg_size);
    __asm volatile ("fence iorw,iorw" ::: "memory");
    
    /* 验证拷贝结果 */
    if(memcmp(dest, src, cfg_size) != 0) {
        vision_log_debug("ISP config copy verification failed \n");
    }
}

void read_basic_fields(void) 
{
    /* 强制类型转换时添加volatile防止编译器优化 */
    volatile const user_cfg_header_s_t *header = (volatile const user_cfg_header_s_t *)__ispcfg_start;
    
    /* 地址对齐检查 */
    if((uintptr_t)header % 4 != 0) {
        vision_log_debug("Unaligned config header!");
        return;
    }
 
    vision_log_debug("Base Address: 0x%08X \n", header->base_addr);
    vision_log_debug("Node Count: %u \n ", header->cfg_node_nums);
    vision_log_debug("Total Size: %u bytes \n ", header->cfg_total_bytes);
}

int traverse_config_nodes(void) 
{
    // const user_cfg_header_s_t *header = (user_cfg_header_s_t *)__ispcfg_start;
    // const uint8_t *current = __ispcfg_start + 16;
    // uint8_t *pu08_dst_buf;
    // const uint8_t *pu08_src_buf;
    // int cnt=0;
    
    // /* 边界检查 */
    // if((__ispcfg_start + header->cfg_total_bytes) > __ispcfg_end) {
    //     vision_log_debug("Config overflow detected! \n");
    //     return -1;
    // }
 
    // for(uint32_t i = 0; i < header->cfg_node_nums; i++) {
    //     const user_cfg_node_header_s_t *node = (const user_cfg_node_header_s_t *)current;
        
 
    //     vision_log_debug("Node[%u]: offset=0x%X, size=%u Name=%s \n", i, node->offset, node->size , node->name);
        
    //     /* 处理节点数据 */
    //     // process_node_data(node->data, node->data_len);
        
    //     /* 移动到下一个节点 */
    //     // current += sizeof(user_cfg_node_header_s_t) + node->size;
    //     current += sizeof(user_cfg_node_header_s_t);
    // }
    
    return 0;
}


