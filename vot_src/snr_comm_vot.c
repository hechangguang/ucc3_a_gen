/*
 * @Author: git config user.name && git config user.email
 * @Date: 2026-01-13 15:43:12
 * @LastEditors: git config user.name && git config user.email

 * @FilePath: /ucc3_a_gen/vot_src/snr_comm_vot.c
 * @Description: 
 * 
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */

#include <config.h>
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/sfc_v100.h>
#include <uapi/aisp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/upl_v100.h>
#include <uapi/isp_comm.h>
#include <libupload.h>
#include <command.h>
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "sffs_files_vot.h"
#include "sffs_files.h"
#include "snr_comm.h"
#include <uapi/isp_reg.h>
#include <uapi/fw_isp_status.h>


#include "menu/votmenu.h"
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <snr_comm_vot.h>
#include <menu/ispfunction.h>
#include <fontlib_vot.h>
#include <vot_config.h>
#include <uapi/alg_v100.h>


ts_u08 preStartForceShiftFlag = 1;	// 机器启动前N分钟，强制打档时间的标志
extern const ts_u08 sast_iso_green_vuya[][4];
extern const ts_u08 sast_iso_red_vuya[][4];
extern const ts_u08 sast_iso_white_vuya[][4];
extern const ts_u08 sast_iso_black_vuya[][4];

extern  ts_mdev_t shdl_osd_dev;
extern void VotDetParInit(ts_u08 state);
//extern void ts_tdk_extbpc_recfg(void);
ts_void usb_usr_init(ts_void *pv_tmm);
ts_void usb_usr_init_libusb(ts_void *pv_tmm);
ts_void usb_uvc_reset(void);

extern snr_ctx_s_t gst_snr_ctx;
extern SysCtrlInfo_s_t global_sys_ctrl_info;

extern struct Menu2_Para SystemParaMenu2;
extern ts_void *gpv_tmm_usr;

ts_u32 det_gos_switch_force_last = 0;

ts_void cvbs_pod_set(ts_u32 u32_pod_dly_ms)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    if(u32_pod_dly_ms>0){
        pst_attr->u32_pod_dly_ms = u32_pod_dly_ms;
    }
    else{
        pst_attr->u32_pod_dly_ms = 0;
    }
    snr_cfg_vpo(pst_attr);
    special_para_init(UsermenuParameter.Palette);
}


ts_void vpo_cvbs_display_orgi_attr(ts_void)
{
    snr_attr_s_t *pst_snr;
    ts_vpo_chn_attr_s_t st_disp_cvbs;
    snr_cvbs_attr_s_t *pst_cfg;

    pst_snr = &gst_snr_ctx.st_attr;

    ts_tdk_vpo_disable_grp(TS_VPO_GRP0);
    ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
    
    ts_s32 ret = ts_tdk_vpo_get_chn_attr(TS_VPO_CVBS_MDEV, &st_disp_cvbs);
    if(ret!=TS_OK){
        return;
    }

    pst_cfg = (snr_cvbs_attr_s_t *)&pst_snr->st_cvbs;
    if (0 == pst_cfg->u08_en)
        return;

    /* u32_real_size 使能会原始尺寸输出，不够补黑边 */
    if (pst_cfg->u08_orgi_en > 0) {
        st_disp_cvbs.u16_disp_width = (ts_u16)pst_snr->s32_img_w;
        st_disp_cvbs.u16_disp_height = (ts_u16)pst_snr->s32_img_h;
        if (pst_cfg->u08_pal_en > 0) {
            st_disp_cvbs.f32_disp_fps = 50;
        } else {
            st_disp_cvbs.f32_disp_fps = 60;
        }
    } else {
        st_disp_cvbs.u16_disp_width = 720;
        if (pst_cfg->u08_pal_en > 0) {
            st_disp_cvbs.u16_disp_height = 576;
            st_disp_cvbs.f32_disp_fps = 50;
        } else {
            st_disp_cvbs.u16_disp_height = 480;
            st_disp_cvbs.f32_disp_fps = 60;
        }
    }
    ts_tdk_vpo_set_chn_attr(TS_VPO_CVBS_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_cvbs);

    ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
    
}

ts_void cvbs_display_mode_set(ts_u08 cvbs_display_mode)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;
    // vot_snr_set_grp_pod_switch(1);
    ts_cpl_msleep(200);
    vot_vpo_freeze();
    ts_cpl_msleep(300);

    if(cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
        pst_attr->st_cvbs.u08_orgi_en = 0;
    }
    else{
        pst_attr->st_cvbs.u08_orgi_en = 1;
    }

    vpo_cvbs_display_orgi_attr();
    //snr_cfg_vpo(pst_attr);
    if(SystemParaMenu2.Bpc_ext_SW>0)
    {
        //ts_tdk_extbpc_recfg();
        ts_tdk_extbpc_sync();
        ts_tdk_extbpc_run();
    }

    ts_cpl_msleep(300);
    vot_vpo_unfreeze();
    ts_cpl_msleep(300);

    usb_uvc_reset();

    vot_snr_set_grp_pod_switch(0);

    special_para_init(UsermenuParameter.Palette);
    vot_refresh_osd_bind();
}

ts_void vot_upl_get_frm_info(ts_u32 u32_pos, ts_u32 *pu32_frm_size, ts_u32 *pu32_strid_size)
{
    ts_u32 u32_frm_size, u32_width, u32_height, u32_strid;

    if (u32_pos >= TS_UPL_POS_VPO_YUV)
        tdl_mupl_get_frm_info((ts_u32 *)&u32_width, (ts_u32 *)&u32_height, ts_null);
    else
        tdl_isp_get_frm_info(TS_ISP_MDEV0, (ts_u32 *)&u32_width, (ts_u32 *)&u32_height);

    u32_frm_size = u32_width * u32_height;
    if (TS_UPL_POS_VPO_YUV == u32_pos) {
        u32_frm_size = u32_frm_size << 1;
        u32_strid = (u32_width * (u32_height + 4)) << 1;
    } else {
        if (u32_pos < TS_UPL_POS_ISP)
            u32_frm_size = u32_frm_size << 1;

        u32_strid = u32_frm_size;
    }

    if (pu32_frm_size != ts_null)
        *pu32_frm_size = u32_frm_size;

    if (pu32_strid_size != ts_null)
        *pu32_strid_size = u32_strid;
}


ts_u32 vot_isp_get_upl_get_frm_strid(ts_s32 s32_pos)
{
    ts_u32 u32_strid, u32_frm_size;
    vot_upl_get_frm_info(s32_pos, (ts_u32 *)&u32_frm_size, (ts_u32 *)&u32_strid);
    return u32_strid;
}


ts_s32 vot_isp_dump_raw_sigle(ts_s32 s32_pos, ts_void* pv_dst_raw, ts_u32 frm_size) 
{
	ts_u32 i;
    ts_u32 s32_is_dma = 0;
    ts_nodebuf_s_t *pst_node;
    ts_s32 u32_frm_cnt = 1;
    ts_u32 u32_node_cnt = u32_frm_cnt + 1;

    ts_void* m_raw_ad_buff = NULL;
    ts_u32 u32_strid_frame;
    ts_u32 u32_strid;
  
    u32_strid = vot_isp_get_upl_get_frm_strid(s32_pos);
    u32_strid_frame = u32_strid * u32_node_cnt; 

    m_raw_ad_buff = (ts_void*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,u32_strid_frame); 

    if(m_raw_ad_buff==NULL){
        vision_log_warn(" m_raw_ad_buff==NULL \n");
        return TS_ERR;
    }

    tdl_isp_claim_s_chn();
    //实测发现，m_raw_ad_buff必须是32字节对齐，否则会出现越界访问问题。
    ts_lib_up_start(s32_pos, (ts_u32 *)m_raw_ad_buff, u32_frm_cnt, u32_node_cnt, s32_is_dma);
    for (i = 0; i < u32_frm_cnt; i++) {
        pst_node = ts_lib_up_get_node(200);
        if (ts_null == pst_node) {
            break;
        }
    
        if(pv_dst_raw){
            ts_cpl_memcpy((ts_void*)pv_dst_raw, (ts_void*)pst_node->u32_virt_addr, u32_strid);
        }
        ts_lib_up_release_node(pst_node);
    }
    ts_lib_up_stop();
    tdl_isp_release_s_chn();

    if(m_raw_ad_buff){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&m_raw_ad_buff); 
        m_raw_ad_buff = 0; 
    }

    return TS_OK;
}


ts_s32 isp_dump_ad_info(ts_s32 s32_pos ,  ts_u16* ad_avg , ts_u16* ad_max, ts_u16* ad_min, void* frame_ptr) 
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
    ts_u32 u32_img_w,  u32_img_h;
 
    u32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
    u32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h ;
     
    ts_u64  sumPoint = u32_img_w * u32_img_h;
    ts_u64 frame_size = vot_isp_get_upl_get_frm_strid(s32_pos);
 
    
    ts_void *m_dst_ad_buff = NULL;

    if(frame_ptr){
        m_dst_ad_buff = frame_ptr;
    }
    else{
        m_dst_ad_buff =  (ts_void *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,frame_size) ;
    }

    if(m_dst_ad_buff==NULL){
        vision_log_warn(" m_dst_ad_buff %p \n",m_dst_ad_buff);
        return 0;
    }

	ts_u16 *pu16_ptr = (ts_u16*)m_dst_ad_buff;

    if(vot_isp_dump_raw_sigle(s32_pos,pu16_ptr,frame_size )==TS_OK){

        ts_u16  maxTemp=0, minTemp=0x3fff, avgTemp=0;
        ts_u64  sumTemp=0;

        ts_u16 tmp_value=0x0;
        pu16_ptr = (ts_u16*)m_dst_ad_buff;

        ts_u32 y=0;
        ts_u32 j = 0;
        ts_u32 index=0;
        if(ad_avg || ad_max || ad_min ){
            for( y = 0; y<u32_img_h; y++){
                for ( j = 0; j < u32_img_w; j++) {
                
                    index = y*u32_img_w + j;
                    
                    tmp_value = (pu16_ptr[index] & 0x3fff);
                    if(maxTemp<tmp_value){
                        maxTemp = tmp_value;
                    }

                    if(minTemp>tmp_value){
                        minTemp = tmp_value;
                    }

                    sumTemp += tmp_value;
                }

            }

            ts_u64  avg_value=  (sumTemp / sumPoint);
            avgTemp = (ts_u16)avg_value;
            // vision_log_debug(" frameindex %d avgTemp [0x%x]  minTemp [0x%x]   maxTemp [0x%x]\n", i, avgTemp,minTemp,maxTemp );
            if(ad_avg){
                *ad_avg = avgTemp;
            }

            if(ad_max){
                *ad_max = maxTemp;
            }

            if(ad_min){
                *ad_min = minTemp;
            }
        }

    }

    if(m_dst_ad_buff!=frame_ptr){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&m_dst_ad_buff); 
		m_dst_ad_buff = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
    return 0;
    
}


ts_void vot_usb_device_init(void)
{
    vision_log_debug("SystemParaMenu2.m_video_out_mode %d \n", SystemParaMenu2.m_video_out_mode);
#ifdef  DEV_SIM_MODE
    SystemParaMenu2.m_video_out_mode  = VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG;
#endif
    if( !(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601) )
    {

        if( !(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI))
        {
    // #if  !defined(VOT_MIPI_EN)
    
        if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_UVC || SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_CVBS)
        {
            vot_usb_uvc_init(gpv_tmm_usr);
            vision_log_debug("USB_VIDEO_MODE_UVC \n");
        }
        else if( /*SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB || */ SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB_DEBUG  ){
            usb_usr_init_libusb(gpv_tmm_usr);
            vision_log_debug("USB_VIDEO_MODE_LIBUSB \n");
        }

    // #endif
        }

    }

    
    
}


 ts_void vot_isp_dump_bp_k(ts_void)
{
    ts_buffer_s_t st_kbuf;
    ts_u16 *pu16_k_buf = null;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;
    ts_u16 k_value;

    pst_attr = &gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    pu16_k_buf = (ts_u16 *)st_kbuf.pu08_ptr;

    for (ts_s32 i = 0; i < s32_img_w; i++) {
        for (ts_s32 y = 0; y < s32_img_h; y++) {
            k_value =  pu16_k_buf[y * s32_img_w + i];
            if(k_value & 0x8000){
                vision_log_debug(" k_value 0x%#x i,y [%d,%d] \n",k_value, i,y);
            }
        }
       
    }

}


ts_void vot_snr_set_grp_pod(ts_grp_t h_grp)
{
    ts_u32 i, j;
    ts_buffer_s_t st_buf;

    /* Get grp0 power on display buff  */
    ts_tdk_vpo_get_grp_pod_buf(h_grp, &st_buf);

    ts_cpl_sync_dma_for_cpu(ts_null, st_buf.phys_addr, (st_buf.u32_height * st_buf.u32_width * 2));

    if(TS_VPO_GRP0 == h_grp) {
        vision_log_debug("POD addr    :%x\n", (ts_u32)st_buf.pu08_ptr);
        vision_log_debug("POD phy addr:%x\n", (ts_u32)st_buf.phys_addr);
        vision_log_debug("POD height  :%d\n", st_buf.u32_height);
        vision_log_debug("POD width   :%d\n", st_buf.u32_width);

        ts_u08 gray = 120;
        ts_u08 gray_2 = 60;
        if(SystemParaMenu2.m_vot_pod_type == VOT_POD_TYPE_WHITE_GRAY){
            gray = 120;
            gray_2 = 60;
            vision_log_debug("VOT_POD_TYPE_WHITE_GRAY\n");
        }
        else  if(SystemParaMenu2.m_vot_pod_type == VOT_POD_TYPE_BLACK){
            gray = 0;
            gray_2 = 0;
            vision_log_debug("VOT_POD_TYPE_BLACK\n");
        }
        else  if(SystemParaMenu2.m_vot_pod_type == VOT_POD_TYPE_GRAY){
            gray = 120;
            gray_2 = 120;
            vision_log_debug("VOT_POD_TYPE_GRAY\n");
        }
        else{
            gray = 120;
            gray_2 = 60;
        }

        ts_u32 stride_width=40;

        // /* Y8 */
        for (i = 0; i < st_buf.u32_height; i++) {
            for (j = 0; j < st_buf.u32_width; j++) {
                 if (0 == (j/stride_width) % 2){
                    st_buf.pu08_ptr[i * st_buf.u32_width + j] = gray;
                 }
                 else{
                    st_buf.pu08_ptr[i * st_buf.u32_width + j] = gray_2;
                 }
            }
        }
    }
}

ts_void vot_isp_set_sync_bp(ts_void)
{
    ts_buffer_s_t st_kbuf;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
    vot_isp_bp_kb_table_write(pu16_ptr);

}


ts_s32 vot_isp_bp_kb_table_reset(ts_void)
{
    ISP_bpc_off();
    vot_isp_set_sync_bp();
    return 0;
}

void vot_badpixel_pack_fast(const uint16_t *src, uint8_t *dst, uint32_t pixels)
{
    uint32_t i;

    if(src==NULL || dst==NULL || pixels==0){
        return; 
    }

    for(i=0;i+7<pixels;i+=8)
    {
        uint8_t value=0;

        value |= ((src[i+0] & 0x8000) >> 15) << 0;
        value |= ((src[i+1] & 0x8000) >> 15) << 1;
        value |= ((src[i+2] & 0x8000) >> 15) << 2;
        value |= ((src[i+3] & 0x8000) >> 15) << 3;

        value |= ((src[i+4] & 0x8000) >> 15) << 4;
        value |= ((src[i+5] & 0x8000) >> 15) << 5;
        value |= ((src[i+6] & 0x8000) >> 15) << 6;
        value |= ((src[i+7] & 0x8000) >> 15) << 7;

        *dst++=value;
    }

    // 处理不足8个pixel
    if(i < pixels)
    {
        uint8_t value=0;
        uint32_t bit=0;

        while(i < pixels)
        {
            if(src[i] & 0x8000){
                value |= (1<<bit);
            }

            i++;
            bit++;
        }
        *dst=value;
    }
}


void vot_badpixel_unpack( const uint8_t *src, uint16_t *dst, uint32_t pixels)
{
    uint32_t i;

    if(src==NULL || dst==NULL || pixels==0){
        return; 
    }
    
    for(i=0;i<pixels;i++)
    {
        uint8_t value = src[i/8];

        if(value & (1<<(i%8)))
            dst[i] |= 0x8000;
        else
            dst[i] &= (~0x8000);
    }
}


void vot_badpixel_unpack_fast( const uint8_t *src,uint16_t *dst, uint32_t pixels)
{
    uint32_t i;
    uint32_t byte_num;
    uint32_t remain;

    if(src == NULL || dst == NULL || pixels == 0)
    {
        return;
    }

    byte_num = (pixels >> 3);    // pixels / 8

    for(i = 0; i < byte_num; i++)
    {
        uint8_t value = src[i];

        dst[0] = ((value & 0x01) ? 0x8000 : 0);

        dst[1] =  ((value & 0x02) ? 0x8000 : 0);

        dst[2] = ((value & 0x04) ? 0x8000 : 0);

        dst[3] = ((value & 0x08) ? 0x8000 : 0);

        dst[4] = ((value & 0x10) ? 0x8000 : 0);

        dst[5] = ((value & 0x20) ? 0x8000 : 0);

        dst[6] =  ((value & 0x40) ? 0x8000 : 0);

        dst[7] = ((value & 0x80) ? 0x8000 : 0);

        dst += 8;
    }

      /*
     * 处理不足8个pixel
     */
    remain = pixels & 0x07;

    if(remain)
    {
        uint8_t value = src[i];
        for(uint32_t j=0;j<remain;j++)
        {
            dst[j] = (value & (1<<j)) ? 0x8000 : 0;
        }
    }
}

ts_s32 vot_isp_bp_kb_table_init(ts_void)
{
    ts_buffer_s_t st_kbuf;
    // sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, &st_kbuf);
    ts_u16 *pu16_k = (ts_u16*)st_kbuf.pu08_ptr;

    ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
    if (s32_hfile_r > 0) {
        ts_u32 u32_index = 0;
        uint32_t pixels = s32_img_w * s32_img_h;
        uint32_t bpc_buf_size = pixels << 1;

        if(vot_get_flash_type()!=VOT_ASIC_NOR_FLASH_8MB){
            if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) > vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }
        else{
            uint32_t tmp_bpc_buf_size = pixels >> 3;
            if (tmp_bpc_buf_size + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }

        sfc_cfg_file_s_t st_sfc_file;
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        strcpy(st_sfc_file.ac_cfg_name, FN_VOT_NUC_KB_CFG);
        //没有vot kb表，就初始化为k表
        if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
            st_sfc_file.u32_cfg_num = 0;    
            st_sfc_file.u32_valid_flag = 0; 

            st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
            st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
            for (ts_s32 i = 0; i < 32; i++) {
                if ((st_sfc_file.u32_valid_flag >> i) != 0)
                {
                    st_sfc_file.u32_cfg_num++;
                }
            } 
            
            if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
                bpc_buf_size = pixels >> 3;
                ts_u16 *pu16_k_buf = (ts_u16 *)pu16_k;
                ts_u08* dst_bpcbuf = (ts_u08*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size); //
                if(dst_bpcbuf){
                    vot_badpixel_pack_fast(pu16_k_buf, dst_bpcbuf, pixels);
                    ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
                    ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) , dst_bpcbuf, bpc_buf_size);
                    if(dst_bpcbuf){
                        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&dst_bpcbuf); 
                    }
                }
                else{
                    vision_log_warn(" vot_tlsf_safe_malloc dst_bpcbuf %p  bpc_buf_size %d \n",dst_bpcbuf, bpc_buf_size);
                }
                
            }
            else{
                ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
                ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + s32_img_w * s32_img_h  * 2 * u32_index, (ts_u08*)pu16_k, bpc_buf_size);
            }

        }
        ts_lib_sfc_close(s32_hfile_r);  

    }
    else{
        vision_log_debug("%d is not vaild! \n",SFC_FILE_VALID_CODE);
        return -1;
    }
    return 0;
}


ts_s32 vot_isp_bp_kb_table_write(ts_u16 * wr_kbuf)
{
    // sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    if(wr_kbuf==NULL){
        vision_log_debug(" error wr_kbuf  %p  \n",wr_kbuf );
        return -1;
    }

    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );
    ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
    if (s32_hfile_r > 0) {
        uint32_t pixels = s32_img_w * s32_img_h;
        uint32_t bpc_buf_size = pixels << 1;
        ts_u32 u32_index = 0;
        if(vot_get_flash_type()!=VOT_ASIC_NOR_FLASH_8MB){
            if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }
        else{
            uint32_t tmp_bpc_buf_size = pixels >> 3;
            if (tmp_bpc_buf_size + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }

        sfc_cfg_file_s_t st_sfc_file;
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        strcpy(st_sfc_file.ac_cfg_name, FN_VOT_NUC_KB_CFG);
        if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
            st_sfc_file.u32_cfg_num = 0;    
            st_sfc_file.u32_valid_flag = 0;            
        }

        st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
        st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
        for (ts_s32 i = 0; i < 32; i++) {
            if ((st_sfc_file.u32_valid_flag >> i) != 0)
            {
                st_sfc_file.u32_cfg_num++;
            }
        }

        if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
            vision_log_warn("VOT_ASIC_NOR_FLASH_8MB \n");
            bpc_buf_size = pixels >> 3;
            ts_u16 *pu16_k_buf = (ts_u16 *)wr_kbuf;
            ts_u08* dst_bpcbuf = (ts_u08*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size); //
            if(dst_bpcbuf){
                vot_badpixel_pack_fast(pu16_k_buf, dst_bpcbuf, pixels);
                ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
                ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) , dst_bpcbuf, bpc_buf_size);
                if(dst_bpcbuf){
                    vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&dst_bpcbuf); 
                }
            }
            else{
                vision_log_warn(" vot_tlsf_safe_malloc dst_bpcbuf %p  bpc_buf_size %d \n",dst_bpcbuf, bpc_buf_size);
            }
            
        }
        else{
            ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
            ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + s32_img_w * s32_img_h  * 2 * u32_index, (ts_u08*)wr_kbuf, bpc_buf_size);
        }


        ts_lib_sfc_close(s32_hfile_r); 
        vision_log_debug("write saved vot kb value...\n"); 

    }
    else{
        vision_log_debug("%d is not vaild! \n",SFC_FILE_VALID_CODE);
        return -1;
    }
    return 0; 

}

ts_s32 vot_isp_bp_kb_table_merge_to_sdk_k_table(void)
{
    ts_buffer_s_t st_kbuf;
    ts_s32 s32_hfile_r;
    ts_u32 u32_cfg_flag;
    sfc_cfg_file_s_t st_sfc_file;
    snr_attr_s_t *pst_attr;
    ts_s32 s32_img_w, s32_img_h;
    ts_s32 i=0;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, &st_kbuf);
    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );

    s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
    ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
    u32_cfg_flag = 0;

    ts_u16* dst_u16_kbuf = NULL;

    if (s32_hfile_r > 0) {
        uint32_t pixels = s32_img_w * s32_img_h;
        uint32_t bpc_buf_size = pixels << 1;
        ts_u32 u32_index = 0;
        if(vot_get_flash_type()!=VOT_ASIC_NOR_FLASH_8MB){
            if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }
        else{
            uint32_t tmp_bpc_buf_size = pixels >> 3;
            if (tmp_bpc_buf_size + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }
        dst_u16_kbuf = (ts_u16*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size);
        if(dst_u16_kbuf==NULL){
            vision_log_warn("wrong dst_u16_kbuf == NULL \n");
            ts_lib_sfc_close(s32_hfile_r);
            return -1;  
        }
        
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));

        if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {

            if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
                vision_log_warn("VOT_ASIC_NOR_FLASH_8MB \n");
                bpc_buf_size = pixels >> 3;
                ts_u08* src_u08_bpcbuf = (ts_u08*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size); //
                if(src_u08_bpcbuf){
                    ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)src_u08_bpcbuf, bpc_buf_size);  
                    vot_badpixel_unpack_fast(src_u08_bpcbuf, dst_u16_kbuf, pixels);
                    if(src_u08_bpcbuf){
                        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&src_u08_bpcbuf); 
                    }
                }
                else{
                    vision_log_warn(" vot_tlsf_safe_malloc dst_bpcbuf %p  bpc_buf_size %d \n",src_u08_bpcbuf, bpc_buf_size);
                }
            }
            else{
                ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)dst_u16_kbuf, bpc_buf_size);  
            }
           
            vision_log_debug("read saved vot kb value...\n");
            u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
        if(u32_cfg_flag == 1){
            for (i = 0; i < pixels; i++){
                if(dst_u16_kbuf[i] & 0x8000){
                    pu16_ptr[i] |= 0x8000;
                }
                
            }
        }
    }
    else{
        vision_log_debug("%d is not vaild! \n",SFC_FILE_VALID_CODE);
        if(dst_u16_kbuf){
            vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&dst_u16_kbuf); 
        }
        return -1;
    }


    if(dst_u16_kbuf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&dst_u16_kbuf); 
    }

    ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    return 0; 

}

ts_s32 vot_isp_bp_kb_table_read(ts_u16 * rd_kbuf)
{
    ts_s32 s32_hfile_r;
    // ts_u32 u32_cfg_flag;
    sfc_cfg_file_s_t st_sfc_file;
    snr_attr_s_t *pst_attr;
    ts_s32 s32_img_w, s32_img_h;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    if(rd_kbuf==NULL){
        vision_log_debug(" error rd_kbuf  %p  \n",rd_kbuf );
        return -1;
    }
    
    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );

    s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
    ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
    // u32_cfg_flag = 0;
    if (s32_hfile_r > 0) {
        uint32_t pixels = s32_img_w * s32_img_h;
        uint32_t bpc_buf_size = pixels << 1;
        ts_u32 u32_index = 0;
        if(vot_get_flash_type()!=VOT_ASIC_NOR_FLASH_8MB){
            if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }
        else{
            uint32_t tmp_bpc_buf_size = pixels >> 3;
            if (tmp_bpc_buf_size + sizeof(sfc_cfg_file_s_t) >  vot_get_fn_vot_nuc_kb_file_size()) {
                vision_log_warn("wrong vot kb tables index \n");
                ts_lib_sfc_close(s32_hfile_r);
                return -1;  
            }
        }

        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));

        if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {
            if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
                vision_log_warn("VOT_ASIC_NOR_FLASH_8MB \n");
                bpc_buf_size = pixels >> 3;
                ts_u08* src_u08_bpcbuf = (ts_u08*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size); //
                if(src_u08_bpcbuf){
                    ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)src_u08_bpcbuf, bpc_buf_size);  
                    vot_badpixel_unpack_fast(src_u08_bpcbuf, rd_kbuf, pixels);
                    if(src_u08_bpcbuf){
                        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&src_u08_bpcbuf); 
                    }
                }
                else{
                    vision_log_warn(" vot_tlsf_safe_malloc dst_bpcbuf %p  bpc_buf_size %d \n",src_u08_bpcbuf, bpc_buf_size);
                }
            }
            else{
                ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)rd_kbuf,bpc_buf_size );
            }

            vision_log_debug("read saved vot kb value...\n");
            // u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
    }
    else{
        vision_log_debug("%d is not vaild! \n",SFC_FILE_VALID_CODE);
        return -1;
    }

    return 0; 
}



ts_s32 vot_isp_sdk_k_table_read(ts_u16 * rd_kbuf)
{
    ts_s32 s32_hfile_r;
    // ts_u32 u32_cfg_flag;
    sfc_cfg_file_s_t st_sfc_file;
    snr_attr_s_t *pst_attr;
    ts_s32 s32_img_w, s32_img_h;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
    

    if(rd_kbuf==NULL){
        vision_log_debug(" error rd_kbuf  %p  \n",rd_kbuf );
        return -1;
    }
    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );

    s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
    ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
    // u32_cfg_flag = 0;
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));

        if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {
            ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)rd_kbuf, s32_img_w * s32_img_h * 2);
            vision_log_debug("read saved sdk kb= value...\n");
            // u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
    }
    else{
        vision_log_debug("%s is not vaild! \n",FN_K_CFG);
        return -1;
    }

    return 0; 
}

ts_s32 vot_isp_sdk_k_table_write(ts_u16 * wr_kbuf)
{
    // sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    if(wr_kbuf==NULL){
        vision_log_debug(" error wr_kbuf  %p  \n",wr_kbuf );
        return -1;
    }

    vision_log_debug(" s32_img_w %d s32_img_h %d  \n", s32_img_w , s32_img_h );
    ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
    if (s32_hfile_r > 0) {
        ts_u32 u32_index = 0;
        if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) > vot_get_fn_k_file_size()) {
            vision_log_debug("wrong sdk k tables index \n");
            ts_lib_sfc_close(s32_hfile_r);
            return -1;  
        }

        sfc_cfg_file_s_t st_sfc_file;
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        strcpy(st_sfc_file.ac_cfg_name, FN_K_CFG);
        if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
            st_sfc_file.u32_cfg_num = 0;    
            st_sfc_file.u32_valid_flag = 0;            
        }

        st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
        st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
        for (ts_s32 i = 0; i < 32; i++) {
            if ((st_sfc_file.u32_valid_flag >> i) != 0)
            {
                st_sfc_file.u32_cfg_num++;
            }
        }

        ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
        ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + s32_img_w * s32_img_h  * 2 * u32_index, (ts_u08*)wr_kbuf, s32_img_w * s32_img_h  * 2);
        ts_lib_sfc_close(s32_hfile_r); 
        vision_log_debug("write  sdk k table value...\n"); 

    }
    else{
        vision_log_debug("%s is not vaild! \n",FN_K_CFG);
        return -1;
    }
    return 0; 

}


ts_void vot_isp_clr_bp(ts_void)
{
    ts_buffer_s_t st_kbuf;
    sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
    for (ts_s32 i = 0; i < s32_img_w * s32_img_h; i++)
    {
        pu16_ptr[i] = pu16_ptr[i] & 0x7fff;
    }
    ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_VOT_NUC_KB_CFG);
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        if (st_sfc_file.u32_valid_code == SFC_FILE_VALID_CODE) {
            ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)st_kbuf.pu08_ptr, s32_img_w * s32_img_h * 2);
        }
        ts_lib_sfc_close(s32_hfile_r);
        vision_log_debug("clr bp done...\n");
    }
    else
    {
        vision_log_debug("open bp file err...\n");
    }
    ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
}



ts_void set_osd_position(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd, ts_u16 u16_left, ts_u16 u16_top)
{
    ts_osd_attr_s_t st_osd_attr;

    // vision_log_debug(" u16_left %d ,u16_top %d \n", u16_left,u16_top );

    if(pst_hosd==NULL){
        return;
    }
    ts_tdk_osd_get_attr(h_osd_dev,  pst_hosd, &st_osd_attr);

    st_osd_attr.u16_left = u16_left;
    st_osd_attr.u16_top = u16_top;

    // vision_log_warn(" u16_left %d ,u16_top %d \n", u16_left,u16_top );

    ts_tdk_osd_set_attr(h_osd_dev, pst_hosd, &st_osd_attr);

    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}


ts_void vot_set_menu_osd_position(void)
{
	if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024) // 1280x1024分辨率，底部信息显示区域适当增加高度
    {
        if(vot_snr_get_cvbs_en()>0){
        set_osd_position(TS_OSD_CVBS_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 28, 700);
        set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 28, 20);
        //printf("TS_OSD_CVBS_MDEV0 set osd position for 1280x1024 \n");
        }
        //
        if(vot_snr_get_mem_en()>0){
            set_osd_position(TS_OSD_DISP_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 10, 700);
            set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 10, 20);
           // printf("TS_OSD_DISP_MDEV0 set osd position for 1280x1024 \n");
        }
    }
    else
    {
    if(vot_snr_get_cvbs_en()>0){
        set_osd_position(TS_OSD_CVBS_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 28, 350);
        set_osd_position(TS_OSD_CVBS_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 28, 10);
        
    }

    if(vot_snr_get_mem_en()>0 || vot_snr_get_bte_en()>0 || vot_snr_get_mipi_csi_en()>0 || vot_snr_get_mipi_dsi_en()>0){
        set_osd_position(TS_OSD_DISP_MDEV0,global_sys_ctrl_info.m_bottom_info_osd_node, 1, 350);
        set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.pst_framebuffer_osd, 1, 10);
    }
	}
}

ts_void bpc_mode_isp_set( ts_u08 ee_en , ts_u08 dde_en)
{
	ts_u32 *pu32_addr, u32_len;
 
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。

    pst_isp_reg->st_ee.u08_en = ee_en;
    pst_isp_reg->st_dde.u08_en = dde_en;

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

}

ts_void update_cvbs_vpo_attr(void)
{
    global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_width = global_sys_ctrl_info.m_sensor_s32_img_w;
    global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_height  = global_sys_ctrl_info.m_sensor_s32_img_h;
}

ts_u08 get_sensor_status(ts_void)
{
    ts_viu_array_info_s_t st_viu_info;
    ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
   
    if (st_viu_info.u32_mean > 0){
        return SENSOR_STATUS_WORKING;
    }
    else{
        vision_log_debug("try get sensor status ! \n");
        ts_cpl_fix_msleep(2000);
        ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
        if (st_viu_info.u32_mean > 0){
            return SENSOR_STATUS_WORKING;
        }
    }

    return SENSOR_STATUS_POWER_DOWN;
}

ts_void wait_sensor_work(ts_void)
{
    ts_u08 cut_try_cnt=0;
    ts_u08 cut_try_cnt_max=3;
    if(get_sensor_status()!=SENSOR_STATUS_WORKING){
        cut_try_cnt_max = 5;
    }
    else{
        cut_try_cnt_max = 2;
    }

    while(1){
        ts_cpl_fix_msleep(1500);

        cut_try_cnt++;

        if(cut_try_cnt>cut_try_cnt_max){
            vision_log_debug(" sensor %d  cut_try_cnt %d \n",get_sensor_status(),cut_try_cnt);
            break;
        }
    }
}

ts_s32 rtd6122_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);


    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 654, 200, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    if (TS_OK == s32_ret) {
        
        f32_fv = (7416.0f-u32_mean)/ (33.0f);
      
    } else {
        u32_mean = 0;
    }

    // s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 654, 200, (ts_u32 *)&u32_fv,  &u32_mean);
    // u32_mean = u32_mean & 0x3fff;
    // if (TS_OK == s32_ret) {
    //     ts_s32 s32_fv = u32_fv;
    //     f32_fv = s32_fv;
    //     f32_fv = 20.0 + (f32_fv / 522.0 - 6867.0) / (0 - 33.8);
       
    // } else {
    //     u32_mean = 0;
    // }
    

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


ts_u16 rtd6_get_tes(ts_void)
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

//uaw温度测试函数，周期性改变温度值，便于验证温度相关功能
ts_void vot_uaw_set_temperature_test(ts_u16* ad_value, ts_f32* temp_value)
{
   static ts_f32 f32_temp = 25.0f;
   static ts_u16 u16_times = 0, u16_dir = 0;

    dector_type_t dector_type = get_powerup_dector_name_type();
    if(dector_type == DECTOR_TYPE_VOT_WLP)
    {
        u16_times++;
        if(u16_times>1)
        {
            u16_times=0;
           
            if(u16_dir==0)
            {
                f32_temp = f32_temp - 5.0f;//for test, limit the temp value between 0-99 degree
            }
            else
            {
                f32_temp = f32_temp + 5.0f;//for test, limit the temp value between 0-99 degree
            }
            if(f32_temp>80.0f)
            {            
                //f32_temp=80.0f; 
                u16_dir = 1 - u16_dir; // Toggle direction
            }
            if(f32_temp<-40.0f)
            {
                //f32_temp=-40.0f; 
                u16_dir = 1 - u16_dir; // Toggle direction
            }
        }
        *temp_value = f32_temp;
        *ad_value = (ts_u16)(7416.0f- (*temp_value)*33.0f);
    }
}


ts_s32 vot_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value)
{
    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();

    dector_type_t dector_type = get_powerup_dector_name_type();
    if(dector_type!=DECTOR_TYPE_RTD){

        if(res_type==DECTOR_RESOLUTION_640X512){
            planck640c_get_temperature_info(ad_value, temp_value);  
        }
        else if(res_type==DECTOR_RESOLUTION_384X288){
            //add for 384 
            planck384c_get_temperature_info(ad_value, temp_value);  
        }
        else if(res_type==DECTOR_RESOLUTION_1280X1024)
        {
            planck1280c_get_temperature_info(ad_value, temp_value);  
        }
    }
    else if(dector_type==DECTOR_TYPE_RTD){
        if(res_type==DECTOR_RESOLUTION_640X512){
            rtd6122_get_temperature_info( ad_value, temp_value);
        }
        else if(res_type==DECTOR_RESOLUTION_1280X1024)
        {
            planck1280c_get_temperature_info(ad_value, temp_value);  
        }
    }
//   vot_uaw_set_temperature_test(ad_value, temp_value);//uaw温度测试函数，周期性改变温度值，便于验证温度相关功能

    return 0;

}

ts_u16 vot_get_tes_info(ts_void)
{
    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    // vision_log_debug("res_type %d \n",res_type);

    dector_type_t dector_type = get_powerup_dector_name_type();
    if(dector_type!=DECTOR_TYPE_RTD){
        if(res_type==DECTOR_RESOLUTION_640X512){
            
            return planck640c_get_tes();  
        }
        else if(res_type==DECTOR_RESOLUTION_384X288){
            //add for 384 
            return planck384c_get_tes();  
        }
        else if(res_type==DECTOR_RESOLUTION_1280X1024)
        {
        	return planck1280c_get_tes(); 	
        }
    }
    else if(dector_type==DECTOR_TYPE_RTD){
        if(res_type==DECTOR_RESOLUTION_640X512){
            return rtd6_get_tes();
        }

    }
    // vision_log_debug("leave: res_type %d \n",res_type);
    return 0;

}


ts_void vot_set_lut_info(ts_u32 u32_enable, ts_u08 u08_lut_update_en, ts_u08 src_sel, ts_u16 u16_mk, ts_u16 u16_mb, ts_u16 u16_b1)
{
    ts_vpo_templut_attr_s_t st_attr;
    ts_s32  ret = TS_OK;

    st_attr.u32_enable = u32_enable;
    st_attr.u08_lut_update_en = u08_lut_update_en;
    st_attr.u08_source_sel = src_sel;
    st_attr.u16_mk = u16_mk;
    st_attr.u16_mb = u16_mb;
    st_attr.u16_b1 = u16_b1;
    st_attr.u32_lut_base_addr = global_sys_ctrl_info.u32_lut_base_addr;

    ts_u16 *pu16_lutaddr = (ts_u16 *) global_sys_ctrl_info.u32_lut_base_addr;
    for(ts_u32 i=0; i<4096; i++){
        pu16_lutaddr[i] = i*4;
    }
    ret = ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0, &st_attr);
    if(ret!=TS_OK){
        vision_log_debug("error: ret %d \n",ret);
    }

    vision_log_debug("leave: u16_b1 %d u16_mk %d  u16_mb %d src_sel %d\n",u16_b1,u16_mk , u16_mb ,src_sel);
}


/**
 * @description: 冻结画面
 * @param {ts_void}
 * @return {ts_s32} 冻结成功返回TS_OK,其他值表示失败
 */
ts_s32 vot_vpo_freeze(ts_void)
{
	ts_s32 s32_ret=0;

	s32_ret = ts_tdk_vpo_freeze(TS_VPO_MDEV0);
	return s32_ret;
}


/**
 * @description: 解冻画面
 * @param {ts_void}
 * @return {ts_s32} 解冻成功返回TS_OK,其他值表示失败
 */
ts_s32 vot_vpo_unfreeze(ts_void)
{
	ts_s32 s32_ret=0;
	s32_ret = ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	return s32_ret;
}

ts_void vot_sun_pt_notify_peer(ts_u08 start_flag)
{
    // vision_log_warn("start_flag %d \n",start_flag);
    if (start_flag == 1 && global_sys_ctrl_info.m_transfer_callback)
    {
        ts_u08 notify_msg[] = {0x56, 0x4F, 0x54 ,0x00, 0x07 ,0x00, 0xCA ,0x4B, 0x06 ,0x03, 0x7D, 0xEA };
        (*global_sys_ctrl_info.m_transfer_callback)(notify_msg, sizeof(notify_msg));
    }
    else  if (start_flag == 0 && global_sys_ctrl_info.m_transfer_callback){
        ts_u08 notify_msg[] = {0x56, 0x4F, 0x54 ,0x00, 0x07 ,0x00, 0xCA ,0x4B, 0x06 ,0x04, 0xBF, 0xAB };
        (*global_sys_ctrl_info.m_transfer_callback)(notify_msg, sizeof(notify_msg));

    }
}


ts_void vot_sun_pt_shutter_notify_peer(ts_u08 open_flag)
{
    // vision_log_warn("open_flag %d \n",open_flag);
    if (open_flag == 1 && global_sys_ctrl_info.m_transfer_callback)
    {
        ts_u08 notify_msg[] = {0x56, 0x4F, 0x54 ,0x00, 0x07 ,0x00, 0xCA ,0x4B, 0x06 ,0x01, 0xBC, 0x6B };
        (*global_sys_ctrl_info.m_transfer_callback)(notify_msg, sizeof(notify_msg));
    }
    else  if (open_flag == 0 && global_sys_ctrl_info.m_transfer_callback){
        ts_u08 notify_msg[] = {0x56, 0x4F, 0x54 ,0x00, 0x06 ,0x00, 0xCA ,0x68, 0x00 ,0x18, 0x0B};
        (*global_sys_ctrl_info.m_transfer_callback)(notify_msg, sizeof(notify_msg));

    }
}


ts_void vot_sun_pt_shutter_close(ts_void)
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
    
	tdk_snrcfg_shutter_close();
    vot_sun_pt_shutter_notify_peer(0);
    
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}

ts_void vot_sun_pt_shutter_open(ts_void)
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
    //如果用户配置了强制关闭挡片，阳光保护时间到了后，也不弹开挡片
    if(global_sys_ctrl_info.m_shutter_force_close_flag==0){
	    tdk_snrcfg_shutter_open();
        vot_sun_pt_shutter_notify_peer(1);

    }
    else{
         vision_log_debug("m_shutter_force_close_flag: %d  !\n", global_sys_ctrl_info.m_shutter_force_close_flag);
    }
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}

ts_void vot_shutter_force_close(ts_void)
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
    
	tdk_snrcfg_shutter_close();
    global_sys_ctrl_info.m_shutter_force_close_flag = 1;
    
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}

ts_void vot_shutter_force_open(ts_void)
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	tdk_snrcfg_shutter_open();
    global_sys_ctrl_info.m_shutter_force_close_flag = 0;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}

/**
 * @description: 查询满足条件的像素个数
 * @param {ts_u32}  查询的阈值gray_level_value，取值：[0-1025]
 * @return {ts_u32} 大于level_vaule的像素个数
 */
ts_u32 vot_get_sun_pt_targer_pixels(ts_u32 gray_level_value)
{
    return vot_whole_hist_target_pixels(gray_level_value);
}


ts_u32 vot_whole_hist_target_pixels(ts_s32 level_value)
{
    ts_isp_agc_usr_hist_s_t hist;
    memset(&hist, 0, sizeof(ts_isp_agc_usr_hist_s_t));
    
    ts_s32 pixel_nums = 0;

    if(level_value<0 || level_value>1025){
        vision_log_debug("level_value: %d is  illegal !\n", level_value);
        return 0;
    }

    //获取直方图数据
    ts_s32 ret = ts_tdk_isp_get_agc_usr_hist(TS_ISP_MDEV0, &hist, 1000);
    if (ret != TS_OK) {
        vision_log_debug("ts_tdk_isp_get_agc_usr_hist failed, return code: %d\n", ret);
        return pixel_nums;
    }
    
    //验证数据
    if (hist.pv_hist && hist.s32_hist_valid_len > 0) {
        ts_u32* data = (ts_u32*)hist.pv_hist;
        const int valid_len = hist.s32_hist_valid_len; // 有效灰度级数量

        //1. 计算总像素数
        ts_s64 total_pixels = 0;
        pixel_nums = 0;
        for (int i = 0; i < valid_len; i++ ) {
            total_pixels += data[i];
            if(i>=level_value){
                pixel_nums+= data[i];
                // vision_log_debug(" pixel_nums [%d] larger than level_value [%u]  \n",pixel_nums,level_value); 
            }
        }

       
    } 
    else {
        vision_log_debug("Invalid histogram data\n");
    }

    //释放资源
    ret = ts_tdk_isp_release_agc_usr_hist(TS_ISP_MDEV0, &hist);
    if (ret != TS_OK) {
        vision_log_debug("Release failed, code: %d\n", ret);
    }

    return (ts_u32)pixel_nums;
}

/**
 * @description: hist灰度值统计
 * @param {ts_s32 } level_value: 目标灰度值
 * @param {ts_s32 } pixel_num: 目标像素个数
 * @return {ts_void}
 */
ts_void vot_whole_hist_test(ts_s32 level_value, ts_s32 pixel_num)
{
    ts_isp_agc_usr_hist_s_t hist;
    memset(&hist, 0, sizeof(ts_isp_agc_usr_hist_s_t));
    
    //获取直方图数据
    ts_s32 ret = ts_tdk_isp_get_agc_usr_hist(TS_ISP_MDEV0, &hist, 1000);
    if (ret != TS_OK) {
        vision_log_debug("ts_tdk_isp_get_agc_usr_hist failed, return code: %d\n", ret);
        return;
    }
    
    //验证数据
    if (hist.pv_hist && hist.s32_hist_valid_len > 0) {
        ts_u32* data = (ts_u32*)hist.pv_hist;
        const int valid_len = hist.s32_hist_valid_len; // 有效灰度级数量


        //1. 计算总像素数
        ts_s64 total_pixels = 0;
        for (int i = 0; i < valid_len; i++ ) {
            total_pixels += data[i];
            if(i>=level_value){
                vision_log_debug("level_value %d grap level [%u] pixels_num [%d]\n",level_value,i,data[i]); 
            }
            if(data[i]>=pixel_num){
                vision_log_debug("pixel_num %d grap level [%u] pixels_num [%d]\n",pixel_num,i,data[i]); 
            }
        }

        // vision_log_debug("valid_len [%d] total_pixels [%d]\n",valid_len,total_pixels);
        //2. 计算灰度均值
        double mean = 0.0;
        if (total_pixels > 0) {
            for (int i = 0; i < valid_len; i++ ) {
                mean += i * data[i];    // i = 灰度级
            }
            mean /= total_pixels;
        }

        //3. 计算灰度方差
        double variance = 0.0;
        if (total_pixels > 0) {
            for (int i = 0; i < valid_len; i++ ) {
                double diff = i - mean;
                variance += diff * diff * data[i];
            }
            variance /= total_pixels;
        }

        //打印统计结果
        vision_log_debug("Valid gray levels: %d\n", valid_len);
        vision_log_debug("Total pixels: %lld\n", (long long)total_pixels);
        vision_log_debug("Mean gray level: %.2f\n", mean);
        vision_log_debug("Gray variance: %.2f\n", variance);
    } 
    else {
        vision_log_debug("Invalid histogram data\n");
    }

    //释放资源
    ret = ts_tdk_isp_release_agc_usr_hist(TS_ISP_MDEV0, &hist);
    if (ret != TS_OK) {
        vision_log_debug("Release failed, code: %d\n", ret);
    }
}

// ts_void vot_block_hist_test(ts_void)
// {
//     ts_mdev_t h_isp_dev = TS_ISP_MDEV0;
//     ts_isp_statis_usr_s_t statis;
//     memset(&statis, 0, sizeof(ts_isp_statis_usr_s_t));

//     //测试获取统计数据
//     ts_s32 ret = ts_tdk_isp_get_usr_statis(h_isp_dev, &statis, 1000);
//     if (ret != TS_OK) {
//         vision_log_debug("ts_tdk_isp_get_usr_statis failed, return code: %d\n", ret);
//         return;
//     }

//     //验证获取的统计数据
//     if (statis.pu16_aver && statis.pu16_detail && statis.pu16_lum_vari && statis.s32_blk_num_w > 0 && statis.s32_blk_num_h > 0) {
//         vision_log_debug("User statistics data acquired successfully.\n");
//         vision_log_debug("Block count: %d x %d\n", statis.s32_blk_num_w, statis.s32_blk_num_h);
//         vision_log_debug("Block size: %d x %d\n", statis.s32_blk_size_w, statis.s32_blk_size_h);

//         //计算总块数
//         int total_blocks = statis.s32_blk_num_w * statis.s32_blk_num_h;
//         vision_log_debug("Total blocks: %d\n", total_blocks);

//         //提示用户输入起始块索引
//         int start_block = 0;
//         if (start_block < 0 || start_block >= total_blocks) {
//             vision_log_debug("Invalid index! Using default start block 0.\n");
//             start_block = 0;
//         }

//         /// 计算要显示的块数量（最多10个，不超过总块数）
//         int display_count = (start_block + 10 > total_blocks) ?(total_blocks - start_block) : 10;
//         vision_log_debug("\nDisplaying %d blocks starting from index %d:\n", display_count,start_block);

//         //遍历并打印指定范围的块信息
//         for (int i = 0; i < display_count; i++ ) {
//             int block_idx = start_block + i;
//             int x = block_idx % statis.s32_blk_num_w;
//             int y = block_idx / statis.s32_blk_num_w;

//             //计算块对应的图像区域坐标
//             int block_start_x = x * statis.s32_blk_size_w;
//             int block_start_y = y * statis.s32_blk_size_h;
//             int block_end_x = block_start_x + statis.s32_blk_size_w - 1;
//             int block_end_y = block_start_y + statis.s32_blk_size_h - 1;

//             //打印块信息
//             vision_log_debug("Block [%d,%d] (idx=%d): aver=%5u, detail=%5u, var=%5u | x= [%3d,%3d], y=[%3d,%3d]\n",
//                 x, y, block_idx,
//                 statis.pu16_aver[block_idx],
//                 statis.pu16_detail[block_idx],
//                 statis.pu16_lum_vari[block_idx],
//                 block_start_x, block_end_x,
//                 block_start_y, block_end_y
//             );
//         }
//     } 
//     else {
//         vision_log_debug("Invalid statistics data received.\n");
//     }

//     //测试释放统计数据
//     ret = ts_tdk_isp_release_usr_statis(h_isp_dev, &statis);
//     if (ret != TS_OK) {
//         vision_log_debug("ts_tdk_isp_release_usr_statis failed, return code: %d\n", ret);
//     } else {
//         vision_log_debug("User statistics data released successfully.\n");
//     }
// }


ts_s32 planck384c_get_temperature_test(ts_u16* ad_value, ts_f32* temp_value ,  ts_u16 u16_coor_y)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret;
    ts_f32 f32_fv = 0.0f;
	ts_u32 u32_mean=0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, u16_coor_y, 0, (ts_u32 *)&u32_fv, &u32_mean);
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

ts_void vot_check_det_int_para(void)
{

    
}

ts_s32 vot_check_det_all_para(void)
{
    ts_s32 illegal_flag = 0;
     //如果是vot_planck1280c
#ifdef CONFIG_SNR_PLANCK1280C
	{
        if(SystemParaMenu2.GAIN1280>0x7f || SystemParaMenu2.GAIN1280==0){
            vision_log_warn("error: SystemParaMenu2.GAIN1280 %d  \n",SystemParaMenu2.GAIN1280);
            illegal_flag = 1;
        }

        if(SystemParaMenu2.GOF1280>0x7f || SystemParaMenu2.GOF1280==0 ){
            vision_log_warn("error: SystemParaMenu2.GOF1280 %d  \n",SystemParaMenu2.GOF1280);
            illegal_flag = 1;
        }
        
        if(SystemParaMenu2.Cint1280>7 || SystemParaMenu2.Cint1280==0 ){
            vision_log_warn("error: SystemParaMenu2.Cint1280 %d  \n",SystemParaMenu2.Cint1280);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.Nint>2047 || SystemParaMenu2.Nint==0){
            vision_log_warn("error: SystemParaMenu2.Nint  %d  \n",SystemParaMenu2.Nint);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.Sel_PJ>1){
            vision_log_warn("error: SystemParaMenu2.Sel_PJ  %d  \n",SystemParaMenu2.Sel_PJ);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.NTCC>127 || SystemParaMenu2.NTCC==0){
            vision_log_warn("error: SystemParaMenu2.NTCC  %d  \n",SystemParaMenu2.NTCC);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.Voccm>63 || SystemParaMenu2.Voccm==0){
            vision_log_warn("error: SystemParaMenu2.Voccm  %d  \n",SystemParaMenu2.Voccm);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.detConfig>1){
            vision_log_warn("error: SystemParaMenu2.detConfig  %d  \n",SystemParaMenu2.detConfig);
            illegal_flag = 1;
        }
        if(SystemParaMenu2.Vfid>127 || SystemParaMenu2.Vfid==0){
            vision_log_warn("error: SystemParaMenu2.Vfid  %d  \n",SystemParaMenu2.Vfid);
            illegal_flag = 1;
        }
    }
#else
  {
    if(SystemParaMenu2.GAIN>0x7f || SystemParaMenu2.GAIN==0){
        vision_log_warn("error: SystemParaMenu2.GAIN %d  \n",SystemParaMenu2.GAIN);
        illegal_flag = 1;
    }

    if(SystemParaMenu2.GOF>0x7f || SystemParaMenu2.GOF==0 ){
        vision_log_warn("error: SystemParaMenu2.GOF %d  \n",SystemParaMenu2.GOF);
        illegal_flag = 1;
    }
    
    if(SystemParaMenu2.INT_C>7 || SystemParaMenu2.INT_C==0 ){
        vision_log_warn("error: SystemParaMenu2.INT_C %d  \n",SystemParaMenu2.INT_C);
        illegal_flag = 1;
    }

    if(SystemParaMenu2.INT_T>76 || SystemParaMenu2.INT_T==0){
        vision_log_warn("error: SystemParaMenu2.INT_T %d  \n",SystemParaMenu2.INT_T);
        illegal_flag = 1;
    }

    if(SystemParaMenu2.OOC_Par>63 || SystemParaMenu2.OOC_Par==0){
        vision_log_warn("error: SystemParaMenu2.OOC_Par gos %d  \n",SystemParaMenu2.OOC_Par);
        illegal_flag = 1;
    }
 }
#endif
    return illegal_flag;
}

ts_void vot_set_special_osd_position(void)
{
    if(vot_snr_get_cvbs_en()>0){
        set_osd_position(TS_OSD_CVBS_MDEV0,global_sys_ctrl_info.m_special_info_osd_node, 28, 350);
    }

    if(vot_snr_get_mem_en()>0){
        set_osd_position(TS_OSD_DISP_MDEV0, global_sys_ctrl_info.m_special_info_osd_node, 10, 10);
    }

}

ts_void vot_set_sun_pt_osd_refresh(int is_visible )
{
    if(is_visible==0){
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
    }
    else{
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 1);
    }

    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
 

}

ts_void vot_set_sun_pt_notice_info(int is_visible)
{
	// ts_s32 s32_ret=0;
    ts_char special_info[48];


    if(is_visible){
        // vot_set_special_osd_position();
        ts_u16 pos_x,  pos_y;
        
        pos_x =  80;
        pos_y = 190;
        dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    
        if (res_type==DECTOR_RESOLUTION_1280X1024)
        {
            pos_x = 200;
            pos_y = 400;
        }
        else if (res_type==DECTOR_RESOLUTION_384X288)
        {
            pos_x = 130;
            pos_y = 130;
        }

        snprintf(special_info,48 ,"Burn Protection");
        if (res_type==DECTOR_RESOLUTION_384X288){
            UpdateSpecialInfoFont16x16(special_info,  pos_x,  pos_y,  is_visible);
        }
        else{
            UpdateSpecialInfoFont64x64(special_info,  pos_x,  pos_y,  is_visible);
        }
        
        vot_set_sun_pt_osd_refresh(1);

        // vision_log_warn("is_visible %d \n",is_visible);
    }
    else{
        vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
        vot_set_sun_pt_osd_refresh(0);
        // vision_log_warn("is_visible %d \n",is_visible);
    }
    vot_refresh_osd_bind();
}

/**
 * @description: 用于主动强制切换det模式
 * @param {ts_u08 }ts_u08 det_switch_force_flag 0：不切换，1:是强制切换，2：立即切换，其他：无效值
 * @return {ts_void}
 */
ts_void ISP_set_det_switch_force_flag(ts_u08 det_switch_force_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_switch_force_mutex);
	global_sys_ctrl_info.m_det_switch_force_flag = det_switch_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_switch_force_mutex);

    // vision_log_debug("m_det_switch_force_flag %d \n",global_sys_ctrl_info.m_det_switch_force_flag);
}

/**
 * @description: 获取主动切换det模式的标记
 * @param {ts_void}
 * @return {ts_u08} 主动切换det模式的标记值
 */
ts_u08 ISP_get_det_switch_force_flag(ts_void)
{
    
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_switch_force_mutex);
    ts_u08 det_switch_force_flag = global_sys_ctrl_info.m_det_switch_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_switch_force_mutex);
	return det_switch_force_flag;
}


ts_void ISP_set_det_switching_flag(ts_u08 det_switching_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_switch_force_mutex);
	global_sys_ctrl_info.m_det_switching_flag = det_switching_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_switch_force_mutex);

    // vision_log_debug("m_det_switching_flag %d \n",global_sys_ctrl_info.m_det_switching_flag);
}

ts_u08 ISP_get_det_switching_flag(ts_void)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_switch_force_mutex);
    ts_u08 det_switching_flag = global_sys_ctrl_info.m_det_switching_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_switch_force_mutex);
	return det_switching_flag;
}

/**
 * @description: 用于主动强制刷新
 * @param {ts_u08 }ts_u08 bottom_statistic_force_flag： 1:是关闭，2是显示，3是保持，其他：无效值
 * @return {ts_void}
 */
ts_void ISP_set_bottom_statistic_force_flag(ts_u08 bottom_statistic_force_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_status_force_mutex);
	global_sys_ctrl_info.m_bottom_statistic_force_flag = bottom_statistic_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_status_force_mutex);

    // vision_log_warn("m_bottom_statistic_force_flag %d \n",global_sys_ctrl_info.m_bottom_statistic_force_flag);
}

/**
 * @description: 获取主动刷新标志
 * @param {ts_void}
 * @return {ts_u08} 当前的主动刷新标志状态
 */
ts_u08 ISP_get_bottom_statistic_force_flag(ts_void)
{
    
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_bottom_status_force_mutex);
    ts_u08 bottom_statistic_force_flag = global_sys_ctrl_info.m_bottom_statistic_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_bottom_status_force_mutex);
	return bottom_statistic_force_flag;
}


ts_void ISP_set_feeding_flag(ts_u08 feeding_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_feeding_flag_mutex);
	global_sys_ctrl_info.m_run_feeding_flag = feeding_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_feeding_flag_mutex);
}


ts_u08 ISP_get_feeding_flag(ts_void)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_feeding_flag_mutex);
	ts_u08 feeding_flag = global_sys_ctrl_info.m_run_feeding_flag ;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_feeding_flag_mutex);
    return feeding_flag;

}


/**
 * @description: 用于主动强制fire statis刷新
 * @param {ts_u08 }ts_u08 update_fire_statis_force_flag 0：关闭刷新， 1:是定时刷新，2:立即刷新，其他：无效值
 * @return {ts_void}
 */
ts_void ISP_set_update_fire_statis_force_flag(ts_u08 update_fire_statis_force_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);
	global_sys_ctrl_info.m_update_fire_statis_force_flag = update_fire_statis_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);

    // vision_log_warn("m_update_fire_statis_force_flag %d \n",global_sys_ctrl_info.m_update_fire_statis_force_flag);
}


/**
 * @description: 获取主动刷新fire statis标志
 * @param {ts_void}
 * @return {ts_u08} 当前的主动刷新标志状态
 */
ts_u08 ISP_get_update_fire_statis_force_flag(ts_void)
{
    
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);
    ts_u08 update_fire_statis_force_flag = global_sys_ctrl_info.m_update_fire_statis_force_flag;
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_update_fire_statis_force_mutex);
	return update_fire_statis_force_flag;
}

/**
 * @description: 是否开启开机画面
 * @param {ts_u08} is_visible是否显示开机画面， 0：不显示开机画面， 1：显示开机画面
 * @return {ts_void}
 */
ts_void vot_snr_set_grp_pod_switch(ts_u08 is_visible)
{

    ts_tdk_vpo_disable_grp(TS_VPO_GRP0);

    ts_vpo_grp_attr_s_t st_attr;
    ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
    if(is_visible==0){
        st_attr.u08_pod_en = 0;
    }
    else{
        st_attr.u08_pod_en = 1;
    }

    ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, (ts_vpo_grp_attr_s_t *)&st_attr);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);

    if(is_visible > 0){
        vot_snr_set_grp_pod(TS_VPO_GRP0);
    }

}

/**
 * @description: 动态配置cvbs输出的是否放大还是缩小
 * @param {ts_u08} is_enlarge 是否放大， 0：原始大小， 1：放大全屏
 * @return {ts_void}
 */
ts_void vot_snr_set_cvbs_enlarge_switch(ts_u08 is_enlarge)
{
    // ts_vpo_chn_attr_s_t st_disp_cvbs;
 
    // snr_attr_s_t *pst_attr;
    // snr_cvbs_attr_s_t *pst_cfg;

    // pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    
    // pst_cfg = (snr_cvbs_attr_s_t *)&pst_attr->st_cvbs;
    // if (0 == pst_cfg->u08_en)
    //     return;
    // ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
    // ts_tdk_vpo_get_chn_attr(TS_VPO_CVBS_MDEV, &st_disp_cvbs);

    // if (is_enlarge == 0) {
    //     st_disp_cvbs.u16_disp_width = (ts_u16)pst_attr->s32_img_w;
    //     st_disp_cvbs.u16_disp_height = (ts_u16)pst_attr->s32_img_h;
    //     if (pst_cfg->u08_pal_en > 0) {
    //         st_disp_cvbs.f32_disp_fps = 50;
    //     } else {
    //         st_disp_cvbs.f32_disp_fps = 60;
    //     }
    // } else {
    //     st_disp_cvbs.u16_disp_width = 720;
    //     if (pst_cfg->u08_pal_en > 0) {
    //         st_disp_cvbs.u16_disp_height = 576;
    //         st_disp_cvbs.f32_disp_fps = 50;
    //     } else {
    //         st_disp_cvbs.u16_disp_height = 480;
    //         st_disp_cvbs.f32_disp_fps = 60;
    //     }
    // }

    // ts_tdk_vpo_set_chn_attr(TS_VPO_CVBS_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_cvbs);
    
    // ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);

}


ts_void vot_snr_set_uvc_output_size(ts_u16 u16_disp_width, ts_u16 u16_disp_height)
{
    // snr_attr_s_t *pst_snr;
    // ts_vpo_chn_attr_s_t st_disp_mem;

    // pst_snr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    
    // if (0 == pst_snr->u32_mem_en)
    //     return;

    // ts_tdk_vpo_disable_chn(TS_VPO_MEM_MDEV);
    // ts_tdk_vpo_get_chn_attr(TS_VPO_MEM_MDEV,&st_disp_mem);
    // st_disp_mem.u16_disp_height = (ts_u16)u16_disp_width;
    // st_disp_mem.u16_disp_width = (ts_u16)u16_disp_height;
    
    // ts_tdk_vpo_set_chn_attr(TS_VPO_MEM_MDEV,&st_disp_mem);
    // ts_tdk_vpo_enable_chn(TS_VPO_MEM_MDEV);


    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    cvbs_pod_set(1000);

    ts_cpl_msleep(200);
    vot_vpo_freeze();
    ts_cpl_msleep(300);

    // ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
    // ts_tdk_vpo_disable_chn(TS_VPO_MEM_MDEV);
    
    pst_attr->st_cvbs.u08_orgi_en = 0;
    
    snr_cfg_vpo(pst_attr);

    ts_cpl_msleep(300);
    vot_vpo_unfreeze();
    ts_cpl_msleep(300);
    cvbs_pod_set(0);
    usb_uvc_reset();

}


ts_void ISP_set_det_ad_info(ts_f32 ad_f32, ts_u16 ad_u16)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_update_ad_mutex);

    global_sys_ctrl_info.m_det_temp_ad_value_u16 = ad_u16; //用于auto 模式下，det参数切换
	global_sys_ctrl_info.m_det_temp_value_f32 = ad_f32;

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_update_ad_mutex);

    // vision_log_debug("ad_f32 %f ad_u16 %u \n",ad_f32,ad_u16 );
}


ts_void ISP_get_det_ad_info(ts_f32* ad_f32_ptr, ts_u16* ad_u16_ptr)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_det_update_ad_mutex);

    if(ad_f32_ptr){
        *ad_f32_ptr = global_sys_ctrl_info.m_det_temp_value_f32;
    }

    if(ad_u16_ptr){
        *ad_u16_ptr = global_sys_ctrl_info.m_det_temp_ad_value_u16;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_det_update_ad_mutex);

    // vision_log_warn("m_det_temp_value_f32 %f m_det_temp_ad_value %u \n",global_sys_ctrl_info.m_det_temp_value_f32,global_sys_ctrl_info.m_det_temp_ad_value_u16 );
}


ts_void vot_snr_update_det_ad(ts_void)
{
    ts_u16 temp_ad_value_u16=0;
    ts_f32 temp_value_f32=0;

    vot_get_temperature_info(&temp_ad_value_u16, &temp_value_f32);  
    ISP_set_det_ad_info(temp_value_f32,  temp_ad_value_u16);
 
}

/**************************
//UAW&UAW_Lite机芯 分温度段设置GOS值
在GOS更新逻辑可以需改为低温-40~0℃之间，-10~0为常温参数减一，后续每十摄氏度递减一，以
0~10摄氏度减四为基础，后续10~40摄氏度每十度加一，40~60每十度加三，60~70每十度加二，七十加一，八十以上保持不动!
**************************/
ts_void vot_uaw_get_gos_from_det_temp(ts_void)
{
    if (SystemParaMenu2.Usermenu_SW) {
        return;
    }

    dector_type_t dector_type = get_powerup_dector_name_type();
    //只有探测器类型为UAW&UAW_Lite的WLP下，且DET模式为自动模式（DetMode=DET_TEMP_MODE_AUTO）时，才能温度值发生变化时调整gos值
    if(dector_type == DECTOR_TYPE_VOT_WLP && SystemParaMenu2.DetMode == DET_TEMP_MODE_AUTO)
    {
        static  ts_f32 temp_value = 0;
        vot_read_det_ad();//读取det温度值,并转换为温度值存入global_sys_ctrl_info.m_det_temp_value全局变量中
        //只有当温度值发生变化正负0.3摄氏度时才进行gos值调整，避免频繁调整导致的性能问题
        if(global_sys_ctrl_info.m_det_temp_value<=(temp_value-0.3) || global_sys_ctrl_info.m_det_temp_value >= (temp_value+0.3))
        {  
            temp_value = global_sys_ctrl_info.m_det_temp_value;//获取当前det温度值,单位为摄氏度
           // vision_log_debug("global_sys_ctrl_info.m_det_temp_value =%f\n", global_sys_ctrl_info.m_det_temp_value);
            ts_u08 gos_OOC_Par=SystemParaMenu2.NormalTemp.OOC_Par;//gos_OOC_Par为常温下的gos值，以常温下的gos值为基准，随温度变化调整当前gos值
            if(gos_OOC_Par<4){
                return;//如果参数设置过小，可能导致后续计算出现负数，直接返回，不调整GOS值
            }
            if(-10<=temp_value && temp_value<0){
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-1;//-10~0为常温参数减一
            }
            else if(-20<=temp_value && temp_value<-10){
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-2;//-20~-10为常温参数减二
            }
            else if(-30<=temp_value && temp_value<-20){
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-3;//-30~-20为常温参数减三
            }
            else if(-40<=temp_value && temp_value<-30){
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-4;//-40~-30为常温参数减四
            }
            else if(0<=temp_value && temp_value<10){
                SystemParaMenu2.OOC_Par = gos_OOC_Par-4;//0-10摄氏度减四为基础值
            }
            else if(temp_value>=10 && temp_value<20){//10~40摄氏度每十度加一
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-4+1;
            }
            else if(temp_value>=20 && temp_value<30){//10~40摄氏度每十度加一
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-4+1+1;
            }
            else if(temp_value>=30 && temp_value<40){//10~40摄氏度每十度加一
                SystemParaMenu2.OOC_Par =   gos_OOC_Par-4+1+1+1;
            }
            else if(temp_value>=40 && temp_value<50){//40~60每十度加三
                SystemParaMenu2.OOC_Par = gos_OOC_Par-4+1+1+1+3;
            }
            else if(temp_value>=50 && temp_value<60){
                SystemParaMenu2.OOC_Par =  gos_OOC_Par-4+1+1+1+3+3;
            }
            else if(temp_value>=60 && temp_value<70){//60~70每十度加二
                SystemParaMenu2.OOC_Par = gos_OOC_Par-4+1+1+1+3+3+2;
            }
            else if(temp_value>=70 && temp_value<80){//70~80每十度加一
                SystemParaMenu2.OOC_Par = gos_OOC_Par-4+1+1+1+3+3+2+1;
            }
            else if(temp_value>=80){//八十以上保持不动
                SystemParaMenu2.OOC_Par =   gos_OOC_Par-4+1+1+1+3+3+2+1;
            }
            if (SystemParaMenu2.OOC_Par > 63)SystemParaMenu2.OOC_Par = 63;//参数最大值为63
            // vision_log_debug("vot_uaw_get_gos_from_det_temp SystemParaMenu2.OOC_Par =%d\n", SystemParaMenu2.OOC_Par);
            // planck640c_write_gos(SystemParaMenu2.OOC_Par);
        //     ts_u08 det_temp_mode = DET_TEMP_MODE_AUTO;
        //    if(temp_value<0){
        //        det_temp_mode = DET_TEMP_MODE_LOW;
        //        SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
		// 	   SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
        //     }
        //     else if(temp_value>=0 && temp_value<50){
        //        det_temp_mode = DET_TEMP_MODE_NORMAL;
        //        SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
		// 	   SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
        //     }
        //     else if(temp_value>=50){
        //         det_temp_mode = DET_TEMP_MODE_HIGH;
        //         SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
		// 		SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
        //     }
        //     if(SystemParaMenu2.INT_C>7 ){SystemParaMenu2.INT_C = 7;}
        //     if(SystemParaMenu2.INT_T>76){SystemParaMenu2.INT_T = 76;}
        //     if(SystemParaMenu2.GAIN >0x7f)SystemParaMenu2.GAIN = 0x7f;
        //     if(SystemParaMenu2.GOF >0x7f)SystemParaMenu2.GOF = 0x7f;

        // vot_vpo_freeze();
        // ISP_pause();
        // ts_cpl_fix_msleep(200);
        // //配置生效
        // planck640c_write_gain_adc(SystemParaMenu2.GAIN);
        // planck640c_write_gof(SystemParaMenu2.GOF);
        // planck640c_write_gos(SystemParaMenu2.OOC_Par);
        // planck640c_write_cint(SystemParaMenu2.INT_C);
        // planck640c_write_tint(SystemParaMenu2.INT_T);
        // planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
        // ts_cpl_fix_msleep(300);
        // ISP_run();
        // ISP_cut_new();
        // vot_vpo_unfreeze();

            //如果当前界面在菜单二的det参数设置界面，则刷新界面显示的det参数值
            if(SelectionMenu.MenuState == 0x26 && SelectionMenu.SelectItem->Info.ID >= 0x61&& SelectionMenu.SelectItem->Info.ID <= 0x6A)
            {
				Update_Display_TestMenuItem(SelectionMenu.SelectItem->Parent->Son,SelectionMenu.SelectItem->Parent->Info.SonNum,SelectionMenu.SelectItem->Info.ID);
            }
        }
    }
}

ts_void vot_snr_init_detector(ts_u08 freeze_flag, ts_u08 force_det_mode)
{
    ts_u08 det_temp_mode = DET_TEMP_MODE_AUTO;
    det_temp_mode = force_det_mode;
    
    if(det_temp_mode==DET_TEMP_MODE_AUTO){

        //vot_snr_update_det_ad();
        //ts_cpl_fix_msleep(300);
        
        vot_snr_update_det_ad();
        vot_read_det_ad();
        ts_s32 s32_temp;
        ts_tdk_snr_get_temp(&s32_temp);

        vision_log_warn("s32_temp [%d] m_det_temp_value_f32 %f m_det_temp_ad_value %u \n",s32_temp,global_sys_ctrl_info.m_det_temp_value_f32,global_sys_ctrl_info.m_det_temp_ad_value_u16 );
        ts_f32 m_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
        ts_s32 m_det_temp_value_ten = (ts_s32)(m_det_temp_value*10.0f);

        if(m_det_temp_value_ten<0){
            det_temp_mode = DET_TEMP_MODE_LOW;
        }
        else if(m_det_temp_value_ten>=0 && m_det_temp_value_ten<(ts_s32)500){
            det_temp_mode = DET_TEMP_MODE_NORMAL;
        }
        else if(m_det_temp_value_ten>=(ts_s32)500){
            det_temp_mode = DET_TEMP_MODE_HIGH;
        }
        if(SystemParaMenu2.Usermenu_SW) {
            det_temp_mode = DET_TEMP_MODE_OFF;
        }
        vision_log_warn("m_det_temp_value_ten  %d \n",m_det_temp_value_ten );
    }

    vision_log_warn(" det_temp_mode %d \n",det_temp_mode );

    switch(det_temp_mode) {
		default: break;
		case DET_TEMP_MODE_LOW:
			SystemParaMenu2.INT_C = SystemParaMenu2.LowTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.LowTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.LowTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_LOW %d \n",det_temp_mode);
			break;
		case DET_TEMP_MODE_NORMAL:
			SystemParaMenu2.INT_C = SystemParaMenu2.NormalTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.NormalTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.NormalTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_NORMAL %d \n",det_temp_mode);
			break;
		case DET_TEMP_MODE_HIGH:
			SystemParaMenu2.INT_C = SystemParaMenu2.HighTemp.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.HighTemp.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.HighTemp.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_HIGH %d \n",det_temp_mode);
			break;
		case DET_TEMP_MODE_OFF:
			SystemParaMenu2.INT_C = SystemParaMenu2.offChange.INT_C;
			SystemParaMenu2.INT_T = SystemParaMenu2.offChange.INT_T;
			SystemParaMenu2.OOC_Par = SystemParaMenu2.offChange.OOC_Par;
			vision_log_warn("DET_TEMP_MODE_OFF %d \n",det_temp_mode);
			break;
	}
    if (SystemParaMenu2.Usermenu_SW) {
        if(SystemParaMenu2.DetMode != DET_TEMP_MODE_OFF){
            SystemParaMenu2.DetMode = DET_TEMP_MODE_OFF;
        }
    }
    else {
        //避免上电反复切换det
        if(SystemParaMenu2.DetMode!= DET_TEMP_MODE_AUTO){
            SystemParaMenu2.DetMode = DET_TEMP_MODE_AUTO;
        }
    }

	if(SystemParaMenu2.INT_T>=77){
		SystemParaMenu2.INT_T = 76;
	}

    /*测温参数设置*/
    if(SystemParaMenu2.Usermenu_SW == 1) //一档测温 1
    {
        SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[0];
        SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[0];
        SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[0];
        SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[0];
    }
    else  if(SystemParaMenu2.Usermenu_SW == 2) //二档测温
    {
        SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[1];
        SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[1];
        SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[1];
        SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[1];
    }
    else  if(SystemParaMenu2.Usermenu_SW == 3) //三档测温
    {
        SystemParaMenu2.GOF = SystemParaMenu2.GOF_Val[2];
        SystemParaMenu2.INT_C = SystemParaMenu2.INT_C_Val[2];
        SystemParaMenu2.INT_T = SystemParaMenu2.INT_T_Val[2];
        SystemParaMenu2.OOC_Par = SystemParaMenu2.OOC_Par_Val[2];
    }

    if(freeze_flag==1){
        vot_vpo_freeze();

        ISP_pause();
        ts_cpl_fix_msleep(200);
    }

    //vot_uaw_get_gos_from_det_temp();//UAW&UAW_Lite根据det温度自动调整gos参数
	//配置生效
    // planck640c_write_gain_adc(SystemParaMenu2.GAIN);
	// planck640c_write_gof(SystemParaMenu2.GOF);
	// planck640c_write_gos(SystemParaMenu2.OOC_Par);
 	// planck640c_write_cint(SystemParaMenu2.INT_C);
	// planck640c_write_tint(SystemParaMenu2.INT_T);
	// planck640c_write_rst_data(77-SystemParaMenu2.INT_T);

    //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
    #ifdef CONFIG_SNR_PLANCK1280C
    {
        planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
        planck1280c_write_gof(SystemParaMenu2.GOF1280);
        planck1280c_write_cint(SystemParaMenu2.Cint1280);
        planck1280c_write_Nint(SystemParaMenu2.Nint);
        planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
        planck1280c_write_NTCC(SystemParaMenu2.NTCC);
        planck1280c_write_Voccm(SystemParaMenu2.Voccm);
        planck1280c_write_detConfig(SystemParaMenu2.detConfig);
        planck1280c_write_Vfid(SystemParaMenu2.Vfid);
    }
    #else
    {
        planck640c_write_gain_adc(SystemParaMenu2.GAIN);
        planck640c_write_gof(SystemParaMenu2.GOF);
        planck640c_write_gos(SystemParaMenu2.OOC_Par);
        planck640c_write_cint(SystemParaMenu2.INT_C);
        planck640c_write_tint(SystemParaMenu2.INT_T);
        planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
    }
    #endif

    if(freeze_flag==1){
        ts_cpl_fix_msleep(200);

        ISP_run();

        vot_isp_cut_with_flag(0,1,1);
        vot_vpo_unfreeze();

    }

    det_gos_switch_force_last = SystemParaMenu2.OOC_Par;
	global_sys_ctrl_info.m_det_cur_mode = det_temp_mode;
}

ts_s32 vot_snr_del_ext_bp(ts_s32 s32_index)
{
    
    //坏斑最大支持16个，索引0-15
    if(s32_index>=0&& s32_index<15){
        return ts_tdk_del_extbpc(s32_index);
    }

    return 0;

}


/**
 * @description: 矫正坏斑接口
 * @param {ts_s32} s32_idx : 坏斑的索引标号,取值(0-15)
 * @param {ts_s32} u32_en : s32_idx索引的坏斑是否生效
 * @param {ts_s32} s32_win_x_start :外窗口左上角对应的x坐标
 * @param {ts_s32} s32_win_y_start :外窗口左上角对应的y坐标
 * @param {ts_s32} u16_x_m :坏斑左上角点的x坐标，相对于外窗口左上角点
 * @param {ts_s32} u16_y_m :坏斑左上角点的y坐标，相对于外窗口左上角点
 * @param {ts_u16} u16_size_x :坏斑的像素宽度
 * @param {ts_u16} u16_size_y :斑的像素高度
 * @return {ts_s32} 成功TS_OK,其他失败
 */
ts_s32 vot_snr_set_ext_bp1( ts_s32 s32_idx , ts_u32 u32_en,  ts_s32 s32_win_x_start, ts_s32 s32_win_y_start, ts_u16 u16_x_m,ts_u16 u16_y_m , ts_u16 u16_size_x,ts_u16 u16_size_y)
{

    ts_extbpc_cfg_s_t st_cfg;
    ts_s32 s32_ret;

    if ((s32_idx < 0) || (s32_idx > EXTBPC_POSITION_MAX)) {
        vision_log_debug("s32_idx must be 0~%d \n",EXTBPC_POSITION_MAX);
        return TS_ERR;
    }

    u32_en = u32_en>0?1:0;

    st_cfg.s32_idx = s32_idx;
    st_cfg.u32_en = u32_en;
    st_cfg.u32_cor_win_sz = (ts_u32)5;

    st_cfg.x = s32_win_x_start;
    st_cfg.y = s32_win_y_start;   
    st_cfg.u16_x_m = u16_x_m;
    st_cfg.u16_y_m = u16_y_m;   

    st_cfg.u16_size_x = u16_size_x;
    st_cfg.u16_size_y = u16_size_y;   


    if(vot_check_extbpc_info_vaild( s32_idx ,  s32_win_x_start,  s32_win_y_start,  u16_size_x, u16_size_y)!=TS_OK ){
        return TS_ERR;
    }

    s32_ret = ts_tdk_add_extbpc(&st_cfg);

    return s32_ret;

}

/**
 * @description: 矫正坏斑接口
 * @param {ts_s32} s32_idx : 坏斑的索引标号,取值(0-15)
 * @param {ts_s32} u32_en : s32_idx索引的坏斑是否生效
 * @param {ts_s32} s32_bad_x :坏斑左上角点的x坐标
 * @param {ts_s32} s32_bad_y :坏斑左上角点的y坐标
 * @param {ts_s32} s32_bad_w :坏斑对应的宽度像素
 * @param {ts_s32} s32_bad_h :坏斑对应的高度像素
 * @return {ts_s32} 成功TS_OK,其他失败
 */
ts_s32 vot_snr_set_ext_bp2( ts_s32 s32_idx , ts_u32 u32_en,  ts_s32 s32_bad_x, ts_s32 s32_bad_y, ts_s32 s32_bad_w,ts_s32 s32_bad_h)
{
    ts_extbpc_cfg_s_t st_cfg;
    ts_s32 s32_ret;
    const ts_s32 window_size = 9;
    const ts_s32 window_half = 4;

    ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
    ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;

    if ((s32_idx < 0) || (s32_idx > EXTBPC_POSITION_MAX)) {
        vision_log_debug("s32_idx must be 0~%d \n",EXTBPC_POSITION_MAX);
        return TS_ERR;
    }

    if(vot_check_extbpc_info_vaild( s32_idx ,  s32_bad_x,  s32_bad_y,  s32_bad_w, s32_bad_h)!=TS_OK ){
        return TS_ERR;
    }
    //ext_bp_dbg_off
    //ts_tdk_extbpc_set_bdg(0, 0xffffffff);//debug模式默认关闭

    vision_log_warn("s32_idx %d ,  s32_bad_x %d,  s32_bad_y %d,  s32_bad_w %d, s32_bad_h %d, \n",s32_idx ,  s32_bad_x,  s32_bad_y,  s32_bad_w, s32_bad_h);
    
    u32_en = u32_en>0?1:0;

    st_cfg.s32_idx = s32_idx;
    st_cfg.u32_en = u32_en;
    //sz为4 可以支持大一些的  最大不超过7X7
    st_cfg.u32_cor_win_sz = (ts_u32)4; //5; //范围为3~5，size越大,坏斑校正效果会更好，但延时也会增加

    ts_s32 s32_bad_center_x = s32_bad_x + s32_bad_w / 2;
    ts_s32 s32_bad_center_y = s32_bad_y + s32_bad_h / 2;

    ts_s32 s32_win_x_start = s32_bad_center_x - window_half;
    ts_s32 s32_win_y_start = s32_bad_center_y - window_half;

    s32_win_x_start = (s32_win_x_start < 0) ? 0 : s32_win_x_start;
    s32_win_x_start = (s32_win_x_start > (ts_s32)s32_img_w - window_size) ? (ts_s32)s32_img_w - window_size : s32_win_x_start;

    s32_win_y_start = (s32_win_y_start < 0) ? 0 : s32_win_y_start;
    s32_win_y_start = (s32_win_y_start > (ts_s32)s32_img_h - window_size) ? (ts_s32)s32_img_h - window_size : s32_win_y_start;


    st_cfg.x = (ts_u16)s32_win_x_start;
    st_cfg.y = (ts_u16)s32_win_y_start;

    st_cfg.u16_x_m = (ts_u16)(s32_bad_x - s32_win_x_start);
    st_cfg.u16_y_m = (ts_u16)(s32_bad_y - s32_win_y_start);

    st_cfg.u16_size_x = (ts_u16)s32_bad_w;
    st_cfg.u16_size_y = (ts_u16)s32_bad_h;   

    s32_ret = ts_tdk_add_extbpc(&st_cfg);
    return s32_ret;


}

      

/**
 * @description: 矫正坏斑接口
 * @param {ts_s32} s32_idx : 坏斑的索引标号,取值(0-15)
 * @param {ts_s32} u32_en : s32_idx索引的坏斑是否生效
 * @param {ts_s32} s32_bad_cx :坏斑中心的x坐标
 * @param {ts_s32} s32_bad_cy :坏斑中心的y坐标
 * @param {ts_s32} s32_diameter :坏斑直径：[1-5]
 * @return {ts_s32} 成功TS_OK,其他失败
 */
ts_s32 vot_snr_set_ext_bp3( ts_s32 s32_idx , ts_u32 u32_en, ts_s32 s32_bad_cx, ts_s32 s32_bad_cy, ts_s32 s32_diameter)
{
    ts_extbpc_cfg_s_t st_cfg;
    ts_s32 s32_ret;
    const ts_s32 window_size = 9;
    const ts_s32 window_half = 4;

    ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
    ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;

   if ((s32_idx < 0) || (s32_idx > EXTBPC_POSITION_MAX)) {
        vision_log_debug("s32_idx must be 0~%d \n",EXTBPC_POSITION_MAX);
        return TS_ERR;
    }

    u32_en = u32_en>0?1:0;

    if ((s32_diameter < 1) || (s32_diameter > 7)) {
        vision_log_debug("diameter must be 1~7 \n");
        return TS_ERR;
    }
    //ext_bp_dbg_off
   // ts_tdk_extbpc_set_bdg(0, 0xffffffff);
    st_cfg.s32_idx = s32_idx;
    st_cfg.u32_en = u32_en;
    st_cfg.u32_cor_win_sz = (ts_u32)5;

    ts_s32 s32_bad_size = s32_diameter;  

    ts_s32 s32_bad_x = s32_bad_cx - (s32_diameter - 1) / 2;
    ts_s32 s32_bad_y = s32_bad_cy - (s32_diameter - 1) / 2;

    ts_s32 s32_bad_center_x = s32_bad_cx;
    ts_s32 s32_bad_center_y = s32_bad_cy;

    ts_s32 s32_win_x_start = s32_bad_center_x - window_half;
    ts_s32 s32_win_y_start = s32_bad_center_y - window_half;
    
    if(vot_check_extbpc_info_vaild( s32_idx ,  s32_win_x_start,  s32_win_y_start,  s32_diameter, s32_diameter)!=TS_OK ){
        return TS_ERR;
    }

    s32_win_x_start = (s32_win_x_start < 0) ? 0 : s32_win_x_start;
    s32_win_x_start = (s32_win_x_start > (ts_s32)s32_img_w - window_size) ? (ts_s32)s32_img_w - window_size : s32_win_x_start;
    s32_win_y_start = (s32_win_y_start < 0) ? 0 : s32_win_y_start;
    s32_win_y_start = (s32_win_y_start > (ts_s32)s32_img_h - window_size) ? (ts_s32)s32_img_h - window_size : s32_win_y_start;

    st_cfg.x = (ts_u16)s32_win_x_start;
    st_cfg.y = (ts_u16)s32_win_y_start;

    st_cfg.u16_x_m = (ts_u16)(s32_bad_x - s32_win_x_start);
    st_cfg.u16_y_m = (ts_u16)(s32_bad_y - s32_win_y_start);
    st_cfg.u16_size_x = (ts_u16)s32_bad_size;
    st_cfg.u16_size_y = (ts_u16)s32_bad_size;

    s32_ret = ts_tdk_add_extbpc(&st_cfg);

    return s32_ret;
}

ts_void vot_check_bpc_ext_para(void)
{
    ts_s32 i;

    if(SystemParaMenu2.Bpc_ext_SW==0){
        return;
    }

    if(SystemParaMenu2.Bpc_ext_SW>0){
        ts_tdk_extbpc_run();
    }
    else{
        ts_tdk_extbpc_pause();
    }

    if(SystemParaMenu2.Bpc_ext_SW >1 ){
        SystemParaMenu2.Bpc_ext_SW = 0;
    }
    for(i = 0;i <= EXTBPC_POSITION_MAX; i++){
        //做范围限制，默认都是关闭状态
        if(SystemParaMenu2.Extbpc_para[i].enable> 1){
           SystemParaMenu2.Extbpc_para[i].enable = 0;
        }
        // if(SystemParaMenu2.Extbpc_para[i].enable == 1){
        //     if(SystemParaMenu2.Extbpc_para[i].bad_x > 639)  SystemParaMenu2.Extbpc_para[i].bad_x = 639;
        //     if(SystemParaMenu2.Extbpc_para[i].bad_y > 511)  SystemParaMenu2.Extbpc_para[i].bad_y = 511;
        //     if(SystemParaMenu2.Extbpc_para[i].bad_w > 5)   SystemParaMenu2.Extbpc_para[i].bad_w = 5;
        //     if(SystemParaMenu2.Extbpc_para[i].bad_h > 5)  SystemParaMenu2.Extbpc_para[i].bad_h = 5;
        // }
        vot_snr_set_ext_bp2(i,SystemParaMenu2.Extbpc_para[i].enable,SystemParaMenu2.Extbpc_para[i].bad_x,SystemParaMenu2.Extbpc_para[i].bad_y,\
            SystemParaMenu2.Extbpc_para[i].bad_w,SystemParaMenu2.Extbpc_para[i].bad_h);

        vision_log_debug("extbp idx[%d] enable[%d] x[%d] y[%d] w[%d] h[%d] \n",i,SystemParaMenu2.Extbpc_para[i].enable,SystemParaMenu2.Extbpc_para[i].bad_x,\
            SystemParaMenu2.Extbpc_para[i].bad_y,SystemParaMenu2.Extbpc_para[i].bad_w,SystemParaMenu2.Extbpc_para[i].bad_h);
    }
}


ts_u08  vot_snr_get_cvbs_en(void)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    return pst_attr->st_cvbs.u08_en;
}



ts_u32  vot_snr_get_mem_en(void)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    return pst_attr->u32_mem_en;
}



ts_u32  vot_snr_get_bte_en(void)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    return pst_attr->st_bte.u08_en ;
}



ts_u32  vot_snr_get_mipi_csi_en(void)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    return pst_attr->st_mipi.u08_en;
}


ts_u32  vot_snr_get_mipi_dsi_en(void)
{
    snr_attr_s_t *pst_attr;

    pst_attr = &gst_snr_ctx.st_attr;

    return pst_attr->st_mipi.u08_dsi_en;
}


// ts_u08  vot_snr_get_t8_mode_raw_en(void)
// {
//     snr_attr_s_t *pst_attr;

//     pst_attr = &gst_snr_ctx.st_attr;
//     ts_u08 u08_raw_oe = 0;
   
//     if( pst_attr->u08_raw_oe == 0 ){
//         if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
//             u08_raw_oe = 1;
//         }
//     }
//     else{
//         u08_raw_oe = 1;
//     }

//     //强制开启 T8模式下的坏点矫正
//     if(vot_get_t8_mode_force_bpc()==VOT_SWITCH_ON){
//         u08_raw_oe = 0;
//     }

//     vision_log_warn(" pst_attr->u08_raw_oe %d u08_raw_oe %d \n", pst_attr->u08_raw_oe,u08_raw_oe);
//     return u08_raw_oe;
// }




ts_u08  vot_check_upl_raw_pos(ts_u08 index)
{
    ts_u08 u08_sub = index;
    if(TS_UPL_POS_VPO_Y8==index || index==TS_UPL_POS_VPO_YUV 
        || (index>=TS_UPL_POS_RAW && index<=TS_UPL_POS_ISP) ){
        u08_sub = index;
    }
    else{
        u08_sub = TS_UPL_POS_VPO_YUV; 
        vision_log_debug("error set default : index %d \n",index);
    }
    return u08_sub;
}

ts_u08  vot_check_lut_raw_pos(ts_u08 index)
{
    ts_u08 u08_sub = index;
    if(index>=TS_LUT_POS_BEFORE_AGC && index<TS_LUT_POS_BUTT){
        u08_sub = index;
    }
    else{
        u08_sub = TS_LUT_POS_BEFORE_AGC; 
        vision_log_debug("error set default : index %d \n",index);
    }
    return u08_sub;
}



ts_s32 vot_check_extbpc_info_vaild( ts_s32 s32_idx , ts_s32 s32_bad_x, ts_s32 s32_bad_y, ts_s32 s32_bad_w,ts_s32 s32_bad_h)
{

    ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
    ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;

    if ((s32_idx < 0) || (s32_idx > EXTBPC_POSITION_MAX)) {
        vision_log_debug("s32_idx must be 0~%d \n",EXTBPC_POSITION_MAX);
        return TS_ERR;
    }

    // if(s32_bad_x<0 || s32_bad_x>=s32_img_w || s32_bad_y <0 || s32_bad_y >= s32_img_h){
    //     return TS_ERR;
    // }
    
    ts_s32 s32_x_left=0, s32_x_right=0,s32_y_top=0, s32_y_bottom = 0;
    s32_x_left=s32_bad_x; 
    s32_x_right=s32_bad_x+s32_bad_w;
    s32_y_top=s32_bad_y;
    s32_y_bottom = s32_bad_y+s32_bad_h;
    if(s32_x_left<0 || s32_x_right>s32_img_w ||  s32_y_top < 0 ||   s32_y_bottom > s32_img_h){
        return TS_ERR;
    }
    

    return TS_OK;   
}



ts_void  vot_auto_calc_gos_for_det(ts_u08 DetMode)
{

    dector_type_t dector_type = get_powerup_dector_name_type();
    if(dector_type!=DECTOR_TYPE_RTD){
    
        if(DetMode == DET_MODE_OFF) {
            SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
        }else if(DetMode == DET_MODE_NORMAL){
            SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
            SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6;
            SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2;

            vision_log_warn(" enter: dector_type_t %d \n", dector_type);
            vision_log_warn(" enter: SystemParaMenu2.OOC_Par %d \n", SystemParaMenu2.OOC_Par);
            if(dector_type==DECTOR_TYPE_VOT_WLP){
                SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par - 6+2;
                SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
                SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par - 2+10;
            }
        }
    }
    else{ //rtd

        SystemParaMenu2.LowTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.NormalTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.HighTemp.OOC_Par = SystemParaMenu2.OOC_Par;
        SystemParaMenu2.offChange.OOC_Par = SystemParaMenu2.OOC_Par;
    }
}


ts_void  vot_add_user_metadata_info(ts_u08* user_metadata, ts_u32 metadata_size)
{

    if(user_metadata==0 || metadata_size==0){
        return;
    }

    if(global_sys_ctrl_info.m_user_metadata_addr!=0 && global_sys_ctrl_info.m_user_metadata_size>0){

        if(metadata_size>global_sys_ctrl_info.m_user_metadata_size){
            metadata_size = global_sys_ctrl_info.m_user_metadata_size;
        }
        ts_cpl_memcpy((ts_void *)global_sys_ctrl_info.m_user_metadata_addr,user_metadata,metadata_size);
        // ts_cpl_dma_map_single(ts_null, (void *)global_sys_ctrl_info.m_user_metadata_addr, global_sys_ctrl_info.m_user_metadata_size, TS_CPL_DMA_TO_DEVICE);

    }

}


/**
 * @description: 亮度配置兼容接口
 * @param {ts_void}
 * @return {ts_void}
 */
ts_void vot_isp_luma_set(ts_void)
{
    if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
        ISP_luma_set_by_percent(UsermenuParameter.m_luma_percent_mode);
	}
    else{

        if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL) //如果场景是室外手动nuc模式，强制使用用户菜单默认参数中的亮度和对比度参数
        {
            ISP_luma_set(2 * UsermenuParameter.manual_ImagePara.Bright);
            // vision_log_warn("UsermenuParameter.manual_ImagePara.Bright %d \n",UsermenuParameter.manual_ImagePara.Bright);
        }
        else
        {
            //非indoor模式才可以改变亮度和对比
            if(UsermenuParameter.m_dev_env_mode != DEVICE_MODE_INDOOR){
                ISP_luma_set(2 * UsermenuParameter.Bright);
            }
            // vision_log_warn("UsermenuParameter.Bright %d \n",UsermenuParameter.Bright);
        }
    
    }

}

/**
 * @description: 对比度配置兼容接口
 * @param {ts_void}
 * @return {ts_void}
 */
ts_void vot_isp_coef_set(ts_void)
{
    if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{

        ISP_ctr_set_by_percent(UsermenuParameter.m_contrast_percent_mode);
	}
    else{
        if(UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR_NUC_MANUAL) //如果场景是室外手动nuc模式，强制使用用户菜单默认参数中的亮度和对比度参数
        {
            ISP_coef_set(8 * UsermenuParameter.manual_ImagePara.Contrast);
        }
        else
        {
            //非indoor模式才可以改变亮度和对比
            if(UsermenuParameter.m_dev_env_mode != DEVICE_MODE_INDOOR){
                ISP_coef_set(8 * UsermenuParameter.Contrast);
            }
            
        }
    }

}


ts_void vot_pip_mode_set(ts_u08 pip_mode, ts_u08 rz_ratio, ts_u08 rz_en)
{
    ts_vpo_grp_attr_s_t st_attr;

    ts_u16 sensor_width = global_sys_ctrl_info.m_sensor_s32_img_w;
    ts_u16 sensor_height = global_sys_ctrl_info.m_sensor_s32_img_h;

    ts_u16 src_center_px = sensor_width/2;
    ts_u16 src_center_py = sensor_height/2;

    ts_u16 src_start_px = src_center_px-32;
    ts_u16 src_start_py = src_center_py-32;

    ts_u16 src_width = 64;
    ts_u16 src_height = 64;

    ts_u16 dst_start_px = 0;
    ts_u16 dst_start_py = 0;

    ts_u16 cell_width = sensor_width/3;
    ts_u16 cell_height = sensor_height/3;

    ts_u16 offset_px = 0;
    ts_u16 offset_py = 0;

    ts_u08 rz_ratio_value = rz_ratio + 1;

    //考虑放大后的图像在宫格内居中
    if(cell_width>(rz_ratio_value*src_width) && cell_height>((rz_ratio_value*src_height))){
        offset_px = (cell_width-(rz_ratio_value*src_width))/2;
        offset_py = (cell_height-(rz_ratio_value*src_height))/2;
    }


    if (pip_mode != PIP_MODE_NONE && UsermenuParameter.Enlarge != 10)
    {
        UsermenuParameter.Enlarge = 10; //1.0变倍
        ISP_vot_zoom_set(UsermenuParameter.Enlarge);
        // ts_cpl_msleep(200);
        // vision_log_warn(" UsermenuParameter.Enlarge %d \n", UsermenuParameter.Enlarge);
    }

    if(rz_ratio >=RZ_RATIO_MODE_BUTT){
        rz_ratio = RZ_RATIO_MODE_1X;
    }

    if(pip_mode==PIP_MODE_NONE){
        rz_en = 0;
        rz_ratio = 0;
    }
    else if(pip_mode==PIP_MODE_P1){
        dst_start_px = 0;
        dst_start_py = 0;
    }
    else if(pip_mode==PIP_MODE_P2){
        dst_start_px = cell_width;
        dst_start_py = 0;
    }
    else if(pip_mode==PIP_MODE_P3){
        dst_start_px = cell_width*2;
        dst_start_py = 0;
    }
    else if(pip_mode==PIP_MODE_P4){
        dst_start_px = 0;
        dst_start_py = cell_height;
    }
    else if(pip_mode==PIP_MODE_P5){
        dst_start_px = cell_width;
        dst_start_py = cell_height;
    }
    else if(pip_mode==PIP_MODE_P6){
        dst_start_px = cell_width*2;
        dst_start_py = cell_height;
    }
    else if(pip_mode==PIP_MODE_P7){
        dst_start_px = 0;
        dst_start_py = cell_height*2;
    }
    else if(pip_mode==PIP_MODE_P8){
        dst_start_px = cell_width;
        dst_start_py = cell_height*2;
    }
    else if(pip_mode==PIP_MODE_P9){
        dst_start_px = cell_width*2;
        dst_start_py = cell_height*2;
    }


    dst_start_px += offset_px;
    dst_start_py += offset_py;

    if(pip_mode==PIP_MODE_P1 || pip_mode==PIP_MODE_P2 || pip_mode==PIP_MODE_P3){
        dst_start_py -= offset_py;
    }
    else  if(pip_mode==PIP_MODE_P7 || pip_mode==PIP_MODE_P8 || pip_mode==PIP_MODE_P9){
        dst_start_py += offset_py;
    }

    ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
    st_attr.u08_rz_en = rz_en;          
    st_attr.u16_rz_src_start_h = src_start_px; // horizontal src start position
    st_attr.u16_rz_src_start_v = src_start_py; // vertical src start position
    st_attr.u16_rz_src_width = src_width;   // horizontal mirror
    st_attr.u16_rz_src_height = src_height;  // vertical mirror
    st_attr.u16_rz_dst_start_h = dst_start_px; // horizontal dst start position
    st_attr.u16_rz_dst_start_v = dst_start_py; // vertical dst start position
    st_attr.u08_rz_ratio = rz_ratio;

    ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, &st_attr);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);

}

ts_void vot_get_fire_statis(ts_isp_fire_statis_s_t* out_fire_statis)
{
  if(out_fire_statis){
        ts_cpl_mutex_lock(&global_sys_ctrl_info.m_update_fire_statis_mutex);
        memcpy(out_fire_statis,&global_sys_ctrl_info.m_isp_fire_statis,sizeof(ts_isp_fire_statis_s_t));
        ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_update_fire_statis_mutex);
    }
    else{
        vision_log_warn("out_fire_statis %p \n",out_fire_statis);   
    }
}

static void _vot_hst_update_statistic(void)
{
    ts_s32 s32_ret;
    ts_isp_fire_attr_s_t set_attr = {0};
    ts_isp_fire_attr_s_t get_attr = {0};

    // 设置自动分块使能和尺寸
    set_attr.u08_enable = 1;            /*使能，0 关闭， 1，开启*/  
    set_attr.u08_blk_size_w = 32;       /*横向块大小*/
    set_attr.u08_blk_size_h = 32;       /*纵向块大小*/

    set_attr.ast_rect[0].b_enable = 0;
    set_attr.ast_rect[1].b_enable = 0;
    set_attr.ast_rect[2].b_enable = 0;
    if (SystemParaMenu2.Usermenu_SW) {
        set_attr.ast_pos[0].u16_pos_x = SpotInit[0].spot_X_Pos;
        set_attr.ast_pos[0].u16_pos_y = SpotInit[0].spot_Y_Pos;
    }
    else {
        set_attr.ast_pos[0].u16_pos_x = 0;
        set_attr.ast_pos[0].u16_pos_y = 0;
    }    
    // 调用 set 函数
    s32_ret = ts_tdk_isp_set_fire_attr(TS_ISP_MDEV0, &set_attr);
    if (s32_ret != 0) {
        vision_log_debug("Set function failed! Error: %d\n", s32_ret);
    }

    // 调用 get 函数
    s32_ret = ts_tdk_isp_get_fire_attr(TS_ISP_MDEV0, &get_attr);
    if (s32_ret != 0) {
        vision_log_debug("Get function failed! Error: %d\n", s32_ret);
    }

    //验证配置是否生效
    if (memcmp(&set_attr, &get_attr, sizeof(ts_isp_fire_attr_s_t)) != 0) {
        vision_log_debug("Data mismatch between set and get!\n");
    }

    ts_isp_fire_statis_s_t fire_statis;
    memset(&fire_statis, 0, sizeof(ts_isp_fire_statis_s_t));
    
    // 测试获取数据
    s32_ret = ts_tdk_isp_get_fire_statis(TS_ISP_MDEV0, &fire_statis, 1000);
    if (s32_ret != 0) {
        vision_log_debug("Error: Get fire statis failed! (ret=%d)\n", s32_ret);
    }else{
        // vision_log_debug(" Get fire statis success\n");
        memcpy(&global_sys_ctrl_info.m_isp_fire_statis, &fire_statis, sizeof(ts_isp_fire_statis_s_t));
        if (SystemParaMenu2.Usermenu_SW) {
            global_sys_ctrl_info.m_uart_spot1_raw_ad = fire_statis.au16_pos_value[0];
            // vision_log_debug("au16_pos_value = 0x%x\n", fire_statis.au16_pos_value[0]);
        }
    }

    //打印整帧信息
    // vision_log_debug("\n[Frame Statistics]:\n");
    // vision_log_debug("  Min: val=%u @(%u,%u)\n", 
    //     fire_statis.st_frm_info.u16_min_val,
    //     fire_statis.st_frm_info.u16_min_pos_x,
    //     fire_statis.st_frm_info.u16_min_pos_y);
        
    // vision_log_debug("  Max: val=%u @(%u,%u)\n", 
    //     fire_statis.st_frm_info.u16_max_val,
    //     fire_statis.st_frm_info.u16_max_pos_x,
    //     fire_statis.st_frm_info.u16_max_pos_y);
        
    // vision_log_debug("  Mean: %u\n", fire_statis.st_frm_info.u16_mean);

    // vision_log_debug("  sizeof(fire_statis): %u\n", sizeof(fire_statis));

    // 释放资源
    s32_ret = ts_tdk_isp_free_fire_statis(TS_ISP_MDEV0, &fire_statis);
    if (s32_ret != 0) {
        vision_log_debug("Error: Free fire statis failed! (ret=%d)\n", s32_ret);
    }
}
ts_void vot_hst_update_attr_safe(void)
{
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_update_fire_statis_mutex);
    _vot_hst_update_statistic();
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_update_fire_statis_mutex);
}
ts_void vot_osd_node_visible(ts_nodebuf_s_t *osd_node, int is_visible )
{
    if(osd_node==ts_null)
    {
        return;
    }

    if(is_visible==0){
        ts_lib_osd_set_visible_vot(osd_node, 0);
    }
    else{
        ts_lib_osd_set_visible_vot(osd_node, 1);
    }

    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
 

}

/**
 * @description: 刷新光标的位置
 * @param {ts_nodebuf_s_t} *pst_osd_node
 * @param {ts_u32} u32_width,最大是整个cvbs设备宽度
 * @param {ts_u32} u32_height，最大是整个cvbs设备高度
 * @return {ts_void}
 */
ts_void vot_set_hst_cursor_position(ts_nodebuf_s_t *cursor_node, ts_u16 pos_x, ts_u16 pos_y)
{
    if(cursor_node==NULL){
        return;
    }
    // vision_log_debug("vot_set_hst_cursor_position enter pos_x %u pox_y %u \n", pos_x,pos_y );
 
    ts_u16 u16_left, u16_top;
    ts_osd_attr_s_t st_attr;
    // ts_s32 ret;

    ts_tdk_osd_get_attr(shdl_osd_dev,cursor_node, (ts_osd_attr_s_t *)&st_attr);
	/* 增加384兼容 */
	ts_u16 max_width;
	ts_u16 max_heigth;
	vGet_BPC_MODE_CurrentResolution(&max_width, &max_heigth);

    u16_left = pos_x % max_width;
    u16_top = pos_y % max_heigth;
    // vision_log_debug(" before u16_left %u u16_top %u \n", u16_left,u16_top );

    u16_left =(ts_u16)((u16_left>>1)<<1);//偶数倍
    u16_top = (ts_u16)((u16_top>>1)<<1);
    st_attr.u16_left = (ts_u16)u16_left; 
    st_attr.u16_top = (ts_u16)u16_top;
    //vision_log_debug(" max_width %u max_heigth %u \n", max_width,max_heigth );
    // vision_log_debug(" after u16_left %u u16_top %u \n", u16_left,u16_top );

    ts_tdk_osd_set_attr(shdl_osd_dev,cursor_node, (ts_osd_attr_s_t *)&st_attr);
  
    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

}


void vot_hst_mode_set(ts_u08 hst_mode)
{

    if(hst_mode>=HOT_SPOT_TRACKING_MODE_BUTT){
        return;
    }

    if(hst_mode==HOT_SPOT_TRACKING_MODE_NONE){
        vot_osd_node_visible(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,0 );
        vot_osd_node_visible(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,0 );
    }

    if( hst_mode!=UsermenuParameter.m_hst_mode){

        UsermenuParameter.m_hst_mode = hst_mode;
        
        ISP_set_update_fire_statis_force_flag(2);
        if(hst_mode!=HOT_SPOT_TRACKING_MODE_NONE){
            ts_isp_fire_statis_s_t fire_statis;
            vot_get_fire_statis(&fire_statis);
            if(hst_mode==HOT_SPOT_TRACKING_MODE_MAX){
                
                ts_u16 max_pos_x = fire_statis.st_frm_info.u16_max_pos_x;
                ts_u16 max_pos_y = fire_statis.st_frm_info.u16_max_pos_y;
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,1 );
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,0 );
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,  max_pos_x,  max_pos_y);
            }
            else if(hst_mode==HOT_SPOT_TRACKING_MODE_MIN){
            
                ts_u16 min_pos_x = fire_statis.st_frm_info.u16_min_pos_x;
                ts_u16 min_pos_y = fire_statis.st_frm_info.u16_min_pos_y;
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,1 );
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,0 );

                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,  min_pos_x,  min_pos_y);
            
            }
            else if(hst_mode==HOT_SPOT_TRACKING_MODE_MAX_MIN){

                ts_u16 max_pos_x = fire_statis.st_frm_info.u16_max_pos_x;
                ts_u16 max_pos_y = fire_statis.st_frm_info.u16_max_pos_y;
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,1 );

                ts_u16 min_pos_x = fire_statis.st_frm_info.u16_min_pos_x;
                ts_u16 min_pos_y = fire_statis.st_frm_info.u16_min_pos_y;
                vot_osd_node_visible(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,1 );

                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,  max_pos_x,  max_pos_y);
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,  min_pos_x,  min_pos_y);
            }
        }

    }
    
}

ts_void vot_roi_zoom_set(ts_u16 zoom, ts_u16 u16_roi_x, ts_u16 u16_roi_y ,ts_u16 u16_roi_width , ts_u16 u16_roi_height)
{
	ts_vpo_zoom_attr_s_t st_attr;

	vision_log_debug("vot_roi_zoom_set  zoom=%d\n", zoom);
	if(zoom<10 || zoom>100){
		return ;
	}
	ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    st_attr.en_type = TS_VPO_ZOOM_ROI;
	st_attr.u16_ratio = zoom;
	st_attr.u08_hmirr_en = UsermenuParameter.Mirror&0x1;
	st_attr.u08_vmirr_en = (UsermenuParameter.Mirror&0x2)>>1;


    /* 感兴趣的区域起始 y 坐标, 仅对 TS_VPO_ZOOM_ROI 生效 */
    st_attr.u16_roi_x =  u16_roi_x;

    /* 感兴趣的区域的宽,高； 仅对 TS_VPO_ZOOM_ROI 生效 */
    st_attr.u16_roi_y =  u16_roi_y;

    st_attr.u16_roi_width =  u16_roi_width;
    st_attr.u16_roi_height =  u16_roi_height;

	ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);

}


ts_void vot_zoom_out_set(ts_u16 zoom, ts_u08 zoom_type)
{
	ts_vpo_zoom_attr_s_t st_attr;

	vision_log_debug("vot_roi_zoom_set  zoom=%d\n", zoom);
	if(zoom<10 || zoom>100){
		return ;
	}
	ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    st_attr.en_type = zoom_type;
	st_attr.u16_ratio = zoom;
	st_attr.u08_hmirr_en = UsermenuParameter.Mirror&0x1;
	st_attr.u08_vmirr_en = (UsermenuParameter.Mirror&0x2)>>1;


	ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);

}

ts_void vot_m_dev_env_mode_set(ts_u08 dev_env_mode)
{
    
	dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    if(res_type==DECTOR_RESOLUTION_384X288){
        if(!(dev_env_mode>=DEVICE_MODE_384_LOW_ATD && dev_env_mode<DEVICE_MODE_384_END)){
            dev_env_mode = DEVICE_MODE_384_LOW_ATD;
        }
        
    } 
    else if(res_type==DECTOR_RESOLUTION_640X512){
        if(!(dev_env_mode>=DEVICE_MODE_INDOOR && dev_env_mode<DEVICE_MODE_OUTDOOR_BPC)){
            dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
        }
    }
    else if(res_type==DECTOR_RESOLUTION_1280X1024){
        if(!(dev_env_mode>=DEVICE_MODE_INDOOR && dev_env_mode<DEVICE_MODE_OUTDOOR_BPC)){
            dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
        }
    }
    else{
        return;
    }
     
    UsermenuParameter.m_dev_env_mode = dev_env_mode;
    ISP_write_sys_cfg();
	reset_usr_isp_cfg();
    
}

ts_u08 vot_get_force_debug_flag(ts_void)
{

//优先判断宏，其次才是用户定义配置
#if defined(DEV_SIM_MODE) || defined(VOT_DEBUG_EN)
    vision_log_warn("define m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
    vision_log_warn("DEV_SIM_MODE or  VOT_DEBUG_EN \n");
    return 1;
#else

    if( global_vot_misc_cfg.m_force_debug_confirm == VOT_DEBUG_OUTPUT_CONFIRM_MAGIC ){
        return 1;
    }
#endif

    return 0;
}



ts_u08 vot_get_u08_step_frame_num(void)
{
    ts_u08 u08_fps = ISP_get_output_fps();
    ts_u08 u08_step_frame_num = 0;
    // if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI 
	// 	||  SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656
	// 	||  SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601){

    //      u08_step_frame_num = (50/s32_fps)-1;
    // }

    u08_step_frame_num = (50/u08_fps)-1;

    // if( !(u08_step_frame_num>0 &&u08_step_frame_num<=7)){
    //     vision_log_warn("u08_fps %u \n",u08_fps);
    //     u08_step_frame_num = 0;
    // }
    vision_log_warn("u08_step_frame_num %u \n",u08_step_frame_num);
    return u08_step_frame_num;
}


ts_void vot_set_iso_mode(ts_u08 switch_flag,ts_u08 iso_position_mode, ts_u08 iso_color_mode, ts_u16 u16_iso_h,ts_u16 u16_iso_l )
{
    if(switch_flag==VOT_SWITCH_OFF){
        return;
    }

    ts_isp_isotherm_attr_s_t st_iso;
    ts_vpo_pcolor_attr_s_t st_pcolor_attr;
    
    u16_iso_h = u16_iso_h << 2;
    u16_iso_l = u16_iso_l << 2;

    switch(iso_position_mode)
    {
        case VOT_ISO_POSITION_MODE_INSIDE:
        {
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 0;
            st_iso.u16_isotherm_high = u16_iso_h; 
            st_iso.u16_isotherm_low = u16_iso_l; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);
            vision_log_debug("  VOT_ISO_POSITION_MODE_INSIDE u16_iso_h %u u16_iso_l %u \n", u16_iso_h,u16_iso_l );;
            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            break;
        }
        case VOT_ISO_POSITION_MODE_OUTSIDE:
        {

            st_iso.u08_enable = 1;
            st_iso.u08_mode = 1;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 0;
            st_iso.u16_isotherm_high = u16_iso_h; 
            st_iso.u16_isotherm_low = u16_iso_l; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);
            vision_log_debug("  VOT_ISO_POSITION_MODE_OUTSIDE u16_iso_h %u u16_iso_l %u \n", u16_iso_h,u16_iso_l );
            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            break;
        }
        case VOT_ISO_POSITION_MODE_ABOVE:
        {
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 255;
            st_iso.u16_isotherm_high = 65535; 
            st_iso.u16_isotherm_low = u16_iso_h; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;
            vision_log_debug("  VOT_ISO_POSITION_MODE_ABOVE u16_iso_h %u \n",u16_iso_h);

            break;
        }
        case VOT_ISO_POSITION_MODE_BELOW:
        {
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 255;
            st_iso.u16_isotherm_high = u16_iso_l; 
            st_iso.u16_isotherm_low = 0; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            vision_log_debug("  VOT_ISO_POSITION_MODE_BELOW u16_iso_l %u \n", u16_iso_l);
            break;
        }
        default:
        {
            break;
        }

    }

    switch(iso_color_mode)
    {
        case VOT_ISO_COLOR_MODE_GREEN:
        {   
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_green_vuya;
            vision_log_debug("  VOT_ISO_COLOR_MODE_GREEN\n");
            break;
        }
        case VOT_ISO_COLOR_MODE_RED:
        {
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_red_vuya;
            vision_log_debug("  VOT_ISO_COLOR_MODE_RED\n");
            break;
        }
        case VOT_ISO_COLOR_MODE_BLACK:
        {
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_black_vuya;
            vision_log_debug("  VOT_ISO_COLOR_MODE_BLACK\n");
            break;
        }
        case VOT_ISO_COLOR_MODE_WHITE:
        {
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_white_vuya;
            vision_log_debug("  VOT_ISO_COLOR_MODE_WHITE\n");
            break;
        }
        default:
        {
            break;
        }
    }

	if(vot_snr_get_cvbs_en()>0){
		ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
	}
	if(vot_snr_get_mem_en()>0){
		ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
	}

	if(vot_snr_get_bte_en()>0){
		ts_tdk_vpo_set_pcolor_attr(TS_VPO_BTENC_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
	}

	if(vot_snr_get_mipi_csi_en()>0){
		ts_tdk_vpo_set_pcolor_attr(TS_VPO_MIPI_CSI_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
	}

	if(vot_snr_get_mipi_dsi_en()>0){
		ts_tdk_vpo_set_pcolor_attr(TS_VPO_MIPI_DSI_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
	}

    special_para_init(1);

}


ts_void vot_set_iso_switch_parameters(ts_u08 switch_flag )
{
    if(switch_flag>=VOT_SWITCH_BUTT){
        switch_flag = VOT_SWITCH_ON;
    }

    if(SystemParaMenu2.m_iso_switch!=switch_flag){
        SystemParaMenu2.m_iso_switch =  switch_flag;
    }
}

ts_void vot_set_iso_position_mode_parameters(ts_u08 iso_position_mode )
{
    if(iso_position_mode>=VOT_ISO_POSITION_MODE_BUTT){
        iso_position_mode = VOT_ISO_POSITION_MODE_INSIDE;
    }

    if(SystemParaMenu2.m_iso_position_mode!=iso_position_mode){
        SystemParaMenu2.m_iso_position_mode =  iso_position_mode;
    }
}

ts_void vot_set_iso_color_mode_parameters(ts_u08 iso_color_mode )
{
    if(iso_color_mode>=VOT_ISO_COLOR_MODE_BUTT){
        iso_color_mode = VOT_ISO_COLOR_MODE_GREEN;
    }

    if(SystemParaMenu2.m_iso_color_mode!=iso_color_mode){
        SystemParaMenu2.m_iso_color_mode =  iso_color_mode;
    }
}

ts_void vot_set_iso_u16_iso_h_parameters(ts_u16 u16_iso_h )
{
    if(u16_iso_h>16383){
        u16_iso_h = 16383;
    }

    if(SystemParaMenu2.m_iso_ad_high!=u16_iso_h){
        SystemParaMenu2.m_iso_ad_high =  u16_iso_h;
    }
}

ts_void vot_set_iso_u16_iso_l_parameters(ts_u16 u16_iso_l )
{
    if(u16_iso_l>16383){
        u16_iso_l = 16383;
    }

    if(SystemParaMenu2.m_iso_ad_low!=u16_iso_l){
        SystemParaMenu2.m_iso_ad_low =  u16_iso_l;
    }
}


ts_void vot_set_iso_parameters(ts_u08 switch_flag,ts_u08 iso_position_mode, ts_u08 iso_color_mode, ts_u16 u16_iso_h,ts_u16 u16_iso_l )
{
    // vision_log_debug("  switch_flag %u    iso_position_mode %u,iso_color_mode  %u u16_iso_h %u, u16_iso_l %u \n", switch_flag ,iso_position_mode,iso_color_mode,u16_iso_h ,u16_iso_l );

    vot_set_iso_position_mode_parameters( iso_position_mode );
    vot_set_iso_color_mode_parameters( iso_color_mode );
    vot_set_iso_u16_iso_h_parameters( u16_iso_h );
    vot_set_iso_u16_iso_l_parameters( u16_iso_l );
    vot_set_iso_switch_parameters(switch_flag);
    // vision_log_debug("  SystemParaMenu2.m_iso_switch %u   SystemParaMenu2.m_iso_position_mode %u,  SystemParaMenu2.m_iso_color_mode %u SystemParaMenu2.m_manual_ad_high  %u SystemParaMenu2.m_manual_ad_low %u\n", 
        // SystemParaMenu2.m_iso_switch ,SystemParaMenu2.m_iso_position_mode, SystemParaMenu2.m_iso_color_mode ,SystemParaMenu2.m_manual_ad_high,SystemParaMenu2.m_manual_ad_low);

    if(SystemParaMenu2.m_iso_switch==VOT_SWITCH_ON ){
        // vision_log_warn("enter SystemParaMenu2.m_iso_switch %d \n",SystemParaMenu2.m_iso_switch);
        //避免伪彩边沿显示异常问题
        vot_set_iso_mode(SystemParaMenu2.m_iso_switch,SystemParaMenu2.m_iso_position_mode, SystemParaMenu2.m_iso_color_mode, SystemParaMenu2.m_iso_ad_high, SystemParaMenu2.m_iso_ad_low );
    }
    else{
        load_palette(UsermenuParameter.Palette);
    }

}

ts_void vot_set_ad_filter_mode_parameters(ts_u08 filter_ad_mode)
{
    if(filter_ad_mode>=VOT_MODE_BUTT){
        filter_ad_mode = VOT_MODE_MANUAL;
    }
    if(filter_ad_mode!=SystemParaMenu2.m_filter_ad_mode){
        SystemParaMenu2.m_filter_ad_mode = filter_ad_mode;
    }
}


ts_void vot_set_ad_filter_ad_high_parameters(ts_u16 manual_ad_high )
{
    if(manual_ad_high>16383){
        manual_ad_high = 16383;
    }

    if(SystemParaMenu2.m_manual_ad_high!=manual_ad_high){
        SystemParaMenu2.m_manual_ad_high =  manual_ad_high;
    }
}

ts_void vot_set_ad_filter_ad_low_parameters(ts_u16 manual_ad_low )
{
    if(manual_ad_low>16383){
        manual_ad_low = 16383;
    }

    if(SystemParaMenu2.m_manual_ad_low!=manual_ad_low){
        SystemParaMenu2.m_manual_ad_low =  manual_ad_low;
    }
}


ts_void vot_set_ad_filter_mode(ts_u08 filter_ad_mode, ts_u16 manual_ad_high,ts_u16 manual_ad_low )
{
    static ts_u08 loadcRet = 0;
    vot_set_ad_filter_ad_high_parameters( manual_ad_high );
    vot_set_ad_filter_ad_low_parameters( manual_ad_low );
    vot_set_ad_filter_mode_parameters( filter_ad_mode);

    // vision_log_debug("  filter_ad_mode %u    manual_ad_high %u,manual_ad_low  %u\n", filter_ad_mode ,manual_ad_high,manual_ad_low);
    // vision_log_debug("  SystemParaMenu2.m_filter_ad_mode %u    SystemParaMenu2.m_filter_ad_mode %u,SystemParaMenu2.m_manual_ad_high  %u SystemParaMenu2.m_manual_ad_low %u\n", SystemParaMenu2.m_filter_ad_mode ,SystemParaMenu2.m_filter_ad_mode,SystemParaMenu2.m_manual_ad_high,SystemParaMenu2.m_manual_ad_low);
    if(VOT_MODE_MANUAL==SystemParaMenu2.m_filter_ad_mode ){
        if (loadcRet == 0) {
            load_isp_config_for_iso_mode();
            loadcRet = 1;
        }
        // vision_log_debug("  filter_ad_mode %u    manual_ad_high %u,manual_ad_low  %u\n", filter_ad_mode ,manual_ad_high,manual_ad_low);
        Isp_set_agc_loc_min_max(manual_ad_low, manual_ad_high);
    }
    else  if(VOT_MODE_AUTO==SystemParaMenu2.m_filter_ad_mode ){
        reset_usr_isp_cfg();
        loadcRet = 0;
    }

}

ts_void vot_check_osd_dev_attr(ts_nodebuf_s_t *pst_hosd,ts_mdev_t h_vpo,ts_vpo_chn_attr_s_t *vpo_st_attr)
{

   ts_osd_attr_s_t st_osd_attr;
   ts_s32 ret=TS_OK;

    ret=ts_tdk_osd_get_attr(shdl_osd_dev, pst_hosd, &st_osd_attr);
    if(ret==TS_OK){
        ts_mdev_t h_osd_dev = TS_OSD_CVBS_MDEV0;
        if(h_vpo==TS_VPO_CVBS_MDEV){
            h_osd_dev = TS_OSD_CVBS_MDEV0;
        }
        else{
            h_osd_dev = TS_OSD_DISP_MDEV0;
        }
        
        // vision_log_debug("st_osd_attr.u16_height %u, st_osd_attr.u16_width %u\n",st_osd_attr.u16_height, st_osd_attr.u16_width);
        if(st_osd_attr.u16_height>vpo_st_attr->u16_disp_height ||  st_osd_attr.u16_width>vpo_st_attr->u16_disp_width ){
            ts_tdk_osd_unbind(h_osd_dev, pst_hosd);
        }
        else{
            ts_tdk_osd_bind(h_osd_dev, pst_hosd,  1);
            ts_u08 osd_type  =  vot_menu_osd_type();
            if(osd_type==VOT_MENU_OSD_RES_384){
                ts_u16 x_center = vpo_st_attr->u16_disp_width/2;
                ts_u16 y_center = vpo_st_attr->u16_disp_height/2;

                ts_u16 osd_x_left = x_center - (st_osd_attr.u16_width/2);
                ts_u16 osd_y_top = y_center - (st_osd_attr.u16_height/2);

                //底部osd应该放底部
                if(pst_hosd==global_sys_ctrl_info.m_bottom_info_osd_node){
                    osd_y_top = vpo_st_attr->u16_disp_height  - st_osd_attr.u16_height;
                    // vision_log_debug("m_bottom_info_osd_node  \n");

                }

                // vision_log_debug(" vpo_st_attr->u16_disp_width %u  : vpo_st_attr->u16_disp_height %u \n" , vpo_st_attr->u16_disp_width,vpo_st_attr->u16_disp_height);
                // vision_log_debug(" st_osd_attr.u16_height %u  : st_osd_attr.u16_width %u \n" , st_osd_attr.u16_height,st_osd_attr.u16_width);
                set_osd_position(h_osd_dev, pst_hosd, osd_x_left, osd_y_top);
                // vision_log_debug(" osd_x_left %u  : osd_y_top %u \n" , osd_x_left,osd_y_top);
            }
            else{

                ts_u16 x_center = vpo_st_attr->u16_disp_width/2;
                ts_u16 y_center = vpo_st_attr->u16_disp_height/2;

                ts_u16 osd_x_left = x_center - (st_osd_attr.u16_width/2);
                ts_u16 osd_y_top = y_center - (st_osd_attr.u16_height/2);

                // vision_log_debug(" osd_x_left %u  : osd_y_top %u \n" , osd_x_left,osd_y_top);
                //底部osd应该放底部
                if(pst_hosd==global_sys_ctrl_info.m_bottom_info_osd_node){
                    
                    osd_y_top = vpo_st_attr->u16_disp_height  - st_osd_attr.u16_height;
                    if(osd_y_top>64){
                        osd_y_top -=64;
                    }

                    if(vpo_st_attr->u16_disp_width==640){
                        if(osd_x_left>16){
                            osd_x_left -=16;
                        }  
                    }
                    // else{
                    //     if(osd_x_left>32){
                    //         osd_x_left -=32;
                    //     }
                    // }
                    
                    // vision_log_debug("m_bottom_info_osd_node  \n");

                }

                if(pst_hosd==global_sys_ctrl_info.pst_framebuffer_osd){
                    osd_y_top = 0;
                    if(vpo_st_attr->u16_disp_width==720){
                        if(osd_x_left>16){
                            osd_x_left -=16;
                        }  
                    }
                    else{
                        if(osd_x_left>64){
                            osd_x_left -=64;
                        }
                    }
                    
                    // vision_log_debug("pst_framebuffer_osd  \n");
                }

                // vision_log_debug(" vpo_st_attr->u16_disp_width %u  : vpo_st_attr->u16_disp_height %u \n" , vpo_st_attr->u16_disp_width,vpo_st_attr->u16_disp_height);
                // vision_log_debug(" st_osd_attr.u16_height %u  : st_osd_attr.u16_width %u \n" , st_osd_attr.u16_height,st_osd_attr.u16_width);
                set_osd_position(h_osd_dev, pst_hosd, osd_x_left, osd_y_top);
                // vision_log_debug(" osd_x_left %u  : osd_y_top %u \n" , osd_x_left,osd_y_top);

            }
            
        }
    }
     
}

ts_void vot_check_osd_and_mdev_attr(ts_mdev_t h_vpo)
{
   ts_vpo_chn_attr_s_t vpo_st_attr;
   ts_s32 ret=TS_OK;

    ret=ts_tdk_vpo_get_chn_attr(h_vpo, &vpo_st_attr);
    if(ret==TS_OK){
        // vision_log_debug("vpo_st_attr.u16_disp_width %u,  vpo_st_attr.u16_disp_height %u\n",vpo_st_attr.u16_disp_width, vpo_st_attr.u16_disp_height);
        // vision_log_debug("m_bottom_info_osd_node :");
        vot_check_osd_dev_attr(global_sys_ctrl_info.m_bottom_info_osd_node, h_vpo,&vpo_st_attr);
        // vision_log_debug("m_special_info_osd_node :");
        vot_check_osd_dev_attr(global_sys_ctrl_info.m_special_info_osd_node, h_vpo,&vpo_st_attr);
        // vision_log_debug("pst_framebuffer_osd :");
        vot_check_osd_dev_attr(global_sys_ctrl_info.pst_framebuffer_osd, h_vpo,&vpo_st_attr);
    }
}


ts_void vot_refresh_osd_bind(ts_void)
{
    if(vot_snr_get_cvbs_en()>0){
        // vision_log_debug("TS_VPO_CVBS_MDEV \n");
        vot_check_osd_and_mdev_attr(TS_VPO_CVBS_MDEV);
    }

    if(vot_snr_get_mem_en()>0){
        // vision_log_debug("TS_VPO_MEM_MDEV \n");
        vot_check_osd_and_mdev_attr(TS_VPO_MEM_MDEV);
    }

    if(vot_snr_get_bte_en()>0){
        // vision_log_debug("TS_VPO_BTENC_MDEV \n");
        vot_check_osd_and_mdev_attr(TS_VPO_BTENC_MDEV);
    }

    if(vot_snr_get_mipi_csi_en()>0){
        // vision_log_debug("TS_VPO_MIPI_CSI_MDEV \n");
        vot_check_osd_and_mdev_attr(TS_VPO_MIPI_CSI_MDEV);
    }

    if(vot_snr_get_mipi_dsi_en()>0){
        // vision_log_debug("TS_VPO_MIPI_DSI_MDEV \n");
        vot_check_osd_and_mdev_attr(TS_VPO_MIPI_DSI_MDEV);
    } 

}


ts_u08 vot_menu_osd_type(ts_void)
{
    ts_u08 menu_osd_type = VOT_MENU_OSD_RES_640;
    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    //只有在384模式，分辨率才是384的菜单
    if(res_type==DECTOR_RESOLUTION_384X288 ){
        menu_osd_type = VOT_MENU_OSD_RES_384;
    }
    else if(res_type==DECTOR_RESOLUTION_1280X1024 ){
        menu_osd_type = VOT_MENU_OSD_RES_1280;
    }
    return menu_osd_type;
}

ts_void vot_temp_drift_calibration_enter_or_exit(ts_u08 exit_flag)
{
    static ts_u08 sun_pt_sw_pre =0; 
    vision_log_warn("exit_flag %d \n",exit_flag);
    if(exit_flag==0){
        shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_BACKUP);
        sun_pt_sw_pre = SystemParaMenu2.SunPT_SW;
    }
    else{
        shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_RESTORE);
        SystemParaMenu2.SunPT_SW = sun_pt_sw_pre;
    }
}

ts_void vot_set_temp_drift_calibration_mode(ts_u08 temp_drift_cali_mode, ts_u08 init_flag )
{
    static ts_u08 temp_drift_cali_mode_pre = TEMP_DRIFT_COMPENSATION_MODE_OFF;
    if(temp_drift_cali_mode>=TEMP_DRIFT_COMPENSATION_MODE_BUTT){
		temp_drift_cali_mode = TEMP_DRIFT_COMPENSATION_MODE_OFF;
	}

    temp_drift_cali_mode_pre =  SystemParaMenu2.m_temp_drift_comp_mode;

    vision_log_debug("temp_drift_cali_mode_pre %d temp_drift_cali_mode %d  \n",temp_drift_cali_mode_pre,temp_drift_cali_mode);
    if(temp_drift_cali_mode_pre!=temp_drift_cali_mode){
        //进入标定温补
        if(temp_drift_cali_mode_pre!=TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION && temp_drift_cali_mode==TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){
            vot_temp_drift_calibration_enter_or_exit(0);
        } //退出标定温补
        else if(temp_drift_cali_mode_pre==TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION && temp_drift_cali_mode!=TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){
            vot_temp_drift_calibration_enter_or_exit(1);
        }
    }


    if(temp_drift_cali_mode==TEMP_DRIFT_COMPENSATION_MODE_OFF){
        gst_snr_ctx.s32_run_auto_b = 0;
    }
    else if(temp_drift_cali_mode==TEMP_DRIFT_COMPENSATION_MODE_ON){
        gst_snr_ctx.s32_run_auto_b = 1;
        
    }
    else if(temp_drift_cali_mode==TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){ 
        gst_snr_ctx.s32_run_auto_b = 0;  
    }

    if(temp_drift_cali_mode!=SystemParaMenu2.m_temp_drift_comp_mode){
        SystemParaMenu2.m_temp_drift_comp_mode = temp_drift_cali_mode;
    }

    if(init_flag==0){
        ISP_force_cut(1);
    }

}

ts_void check_aisp_create_status(ts_void)// 监控超分创建状态
{
    static ts_u32 u32_aisp_create_cnt = 0;
    // ts_tdk_aisp_create失败则关闭npu超分,A6开启640->960超分，会一直报错Permission denied:illegal TDK for AISP，导致系统无法启动，此时需要关闭超分，系统自动重启
    if (global_sys_ctrl_info.m_npu_aisp_flag == TS_ERR && SystemParaMenu2.m_ts_npu_ops_sr_en > NPU_SR_OFF) 
    {
        u32_aisp_create_cnt++;
        if (u32_aisp_create_cnt >= 100) // 超过10秒ts_tdk_aisp_create创建失败，则关闭npu超分，系统自动重启
        {
            SystemParaMenu2.m_ts_npu_ops_sr_en = NPU_SR_OFF;// ts_tdk_aisp_create 失败则关闭npu超分
	        ISP_write_sys_menu2_cfg(SystemParaMenu2);    
            vision_log_err("ts_tdk_aisp_create :%d\n", global_sys_ctrl_info.m_npu_aisp_flag);
            //停止喂狗，系统将自动重启
            ISP_set_feeding_flag(0);
        }
    }
}

ts_s32 change_ooc_idx(ts_u08 u08_idx)
{
    ts_buffer_s_t st_occ_buf;
    sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_hfile_r, s32_ret;
    ts_u32 u32_size;

    s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
    s32_ret = TS_ERR;
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code)  && (st_sfc_file.u32_valid_flag & (1 << u08_idx))) {
            ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);

            u32_size = st_occ_buf.u32_width * st_occ_buf.u32_height;
            ts_u32 u32_offset = sizeof(sfc_cfg_file_s_t) + st_occ_buf.sz_size * u08_idx;
            if ((u32_offset + u32_size) <= FN_OCC_FILE_SIZE) {
                ts_lib_sfc_read(s32_hfile_r, u32_offset, (ts_u08 *)st_occ_buf.pu08_ptr, st_occ_buf.sz_size);
                s32_ret = TS_OK;
                vision_log_debug("use saved occ value from idx %d\n", u08_idx);
            } else {
                vision_log_debug("idx %d out of file range\n", u08_idx);
            }
        }

        ts_lib_sfc_close(s32_hfile_r);
    }

    return s32_ret;
}


/** @note 观测版自动打挡线程 */
ts_void observation_auto_cut_process(ts_void)
{
    ts_s32 s32_temp_cur=0;
    ts_s32 s32_cut_cnt=0;
    ts_cpl_timeval_s_t st_time_start, st_time_cur, st_time_last_cut;   
    ts_cpl_gettimeofday(&st_time_start);
    ts_cpl_gettimeofday(&st_time_last_cut);

    ts_u32 u32_cut_status = 0;

    ts_u08 start_force_shift_st0_en = 0;
    ts_u08 start_force_shift_st1_en = 0;

    //开机第一段打档时间
    ts_s32 s32_cut_time_t0_gap_second_start = 0;
    ts_s32 s32_cut_time_t0_gap_second_end = 0;

    //开机第二段打档时间
    ts_s32 s32_cut_time_t1_gap_second_start = 0;
    ts_s32 s32_cut_time_t1_gap_second_end =0;
    
    ts_s32 s32_cut_time_gap_static = 60 * 1000; //60秒


    ts_s32 s32_auto_b_flag=0;
  
    ts_s32 s32_frm=16;
    ts_isp_nuc_attr_s_t st_nuc_attr;


    //ST0执行完后，ST1开始, ST0可以为空
    if(SystemParaMenu2.m_st0_total_duration_s == 0){
        start_force_shift_st0_en = 0;
    }
    else{
        start_force_shift_st0_en = 1;  
        s32_cut_time_t0_gap_second_start = global_sys_ctrl_info.m_tv_start.tv_sec;
        s32_cut_time_t0_gap_second_end = s32_cut_time_t0_gap_second_start + SystemParaMenu2.m_st0_total_duration_s;
        vision_log_debug(" s32_cut_time_t0_gap_second_end %d s32_cut_time_t0_gap_second_start %d !\n",s32_cut_time_t0_gap_second_end,s32_cut_time_t0_gap_second_start);
    }

    //m_st1_m为0时，无效
    if(SystemParaMenu2.m_st1_m == 0){
        start_force_shift_st1_en = 0;
    }
    else{
        start_force_shift_st1_en = 1;  
        //ST0有效时，需要累加ST0的时间间隔
        if(start_force_shift_st0_en==1){
            s32_cut_time_t1_gap_second_start = s32_cut_time_t0_gap_second_end ;
            s32_cut_time_t1_gap_second_end =s32_cut_time_t1_gap_second_start +  (SystemParaMenu2.m_st1_m * 60); 
        }
        else{
            s32_cut_time_t1_gap_second_start = global_sys_ctrl_info.m_tv_start.tv_sec;
            s32_cut_time_t1_gap_second_end = s32_cut_time_t1_gap_second_start +  (SystemParaMenu2.m_st1_m * 60);
        }
        vision_log_debug(" s32_cut_time_t1_gap_second_end %d s32_cut_time_t1_gap_second_start %d !\n",s32_cut_time_t1_gap_second_end,s32_cut_time_t1_gap_second_start);
    }
 

    if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {
   
        ///////////////////////////////////////////////////////
        //刚启动，帧数设置为32帧
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        s32_frm = st_nuc_attr.u08_stat_frm_num;
        st_nuc_attr.u08_stat_frm_num = 32;
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    }

    ts_u32 temp_cnt=0;
    ts_f32 last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
    ts_u16 diff_temp_value=0;

    vision_log_warn("gst_snr_ctx.s32_run_auto_cut %d \n",gst_snr_ctx.s32_run_auto_cut);

    static int test_cnt = 0;
    while(1) {
        u32_cut_status = 0;

        if ((0 == gst_snr_ctx.s32_run_auto_b) && 1 != s32_auto_b_flag && gst_snr_ctx.u32_sl_en) {
            ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
            st_nuc_attr.u16_coef = 0;
            ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);

            s32_auto_b_flag = 1;
        }

        if (start_force_shift_st1_en || start_force_shift_st0_en) {
            ts_cpl_gettimeofday(&st_time_cur);
            ts_s32 s32_time_diff;
            //简化判断，只看秒级时间
            if(start_force_shift_st0_en==1 && st_time_cur.tv_sec > s32_cut_time_t0_gap_second_start){
                if (st_time_cur.tv_sec >= s32_cut_time_t0_gap_second_end) {
                    vision_log_debug(" s32_cut_time_t0_gap_second_end %d  st_time_cur.tv_sec %d !\n",s32_cut_time_t0_gap_second_end,st_time_cur.tv_sec);
                    start_force_shift_st0_en = 0;
                    
                }
                else{
                    s32_time_diff = st_time_cur.tv_sec - st_time_last_cut.tv_sec;
                    if (s32_time_diff >= SystemParaMenu2.m_st0_interval_s) {
                        u32_cut_status |= 0x1;
                        ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
                    }
                }

            }
            
            if(start_force_shift_st1_en==1 && st_time_cur.tv_sec > s32_cut_time_t1_gap_second_start){
                if (st_time_cur.tv_sec >= s32_cut_time_t1_gap_second_end) {
                    vision_log_debug(" s32_cut_time_t1_gap_second_end %d  st_time_cur.tv_sec %d !\n",s32_cut_time_t1_gap_second_end,st_time_cur.tv_sec);
                    start_force_shift_st1_en = 0;
                    
                }
                else{
                    s32_time_diff = st_time_cur.tv_sec - st_time_last_cut.tv_sec;
                    if (s32_time_diff >= SystemParaMenu2.m_st1_s) {
                        u32_cut_status |= 0x20;
                        ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
                    }
                }
            }
        }

        if (0 == gst_snr_ctx.s32_run_auto_cut) {
            ts_cpl_msleep(100);
            ts_cpl_gettimeofday(&st_time_last_cut);  
            continue;
        }
        
        //矫正的时候不进行自动打档
        if(ISP_get_det_calibrationing_flag()>0) {
            // vision_log_debug(" det calibrationing ! stop update statics \n");
            ts_cpl_fix_msleep(500);
            continue;
        }

        //GOS需要调整时打一次快门
        if (det_gos_switch_force_last != SystemParaMenu2.OOC_Par) { 
            u32_cut_status |= 0x10; 
        }

        //更新统计信息
        {
            global_sys_ctrl_info.isp_auto_cut_run_cnt_u64++;
            ts_cpl_gettimeofday(&st_time_cur);
            ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000000;
            global_sys_ctrl_info.isp_cut_time_sec_u32 = s32_time_diff;
            global_sys_ctrl_info.isp_temp_ad_value_u16 = ISP_get_det_ad_value();
        }
        
        if(ISP_get_ShutterMode() != SHUTTER_MODE_OFF) {
            //ST0,ST1两段打档结束后，才进行定时打档
            if (start_force_shift_st1_en==0 && start_force_shift_st0_en==0) {
                //获取实际的打档间隔
                s32_cut_time_gap_static  = ISP_get_shutter_timer_gap();
                {
                    ts_cpl_gettimeofday(&st_time_cur);
                    ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
                    if (s32_time_diff >= s32_cut_time_gap_static){
                        u32_cut_status |= 0x2;
                    }
                    
                }
            }

            temp_cnt++;
            if(temp_cnt>10) {
                temp_cnt = 0;
                if(SystemParaMenu2.m_det_st != 0) {
                    vot_read_det_ad();

                    diff_temp_value = (global_sys_ctrl_info.m_det_temp_value - last_det_temp_value) * 10;
                    
                    if(abs(diff_temp_value) >= SystemParaMenu2.m_det_st) {
                        u32_cut_status |= 0x4; 
                        last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
                    }                   
                }            
            }
        }
 
        if (gst_snr_ctx.s32_force_cut) {
            u32_cut_status |= 0x8;
            gst_snr_ctx.s32_force_cut = 0;
        }

        if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {
            if (6 == s32_cut_cnt) { 
                // vision_log_warn("s32_force_inter = %d s32_frm %d \n", s32_force_inter,s32_frm);
                st_nuc_attr.u08_stat_frm_num = s32_frm;
                ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
                // s32_force_inter=0;
            }
        }

        if ((u32_cut_status != 0) && gst_snr_ctx.s32_run_auto_cut) {
            //兼容手动打档优先
            if( (u32_cut_status & 0x20 ) ||  (u32_cut_status & 0x10 ) || (u32_cut_status & 0x8) || (u32_cut_status & 0x1)|| (ISP_get_ShutterMode() != SHUTTER_MODE_OFF)){
    
                vision_log_warn("to cut, status = 0x%x, diff_temp_value = %d \n", u32_cut_status, diff_temp_value);
                ISP_cut_new();
                s32_temp_cur = global_sys_ctrl_info.m_cali_temp_value;
                last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
                temp_cnt = 0;
                s32_cut_cnt++;
                
            }
            ts_cpl_gettimeofday(&st_time_last_cut);    
            
            u32_cut_status = 0;
        }
        else //没有开机/强制/定时打档，则进行判断是否需要温漂打档
        {
            if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {



                if (gst_snr_ctx.s32_run_auto_b && gst_snr_ctx.s32_run_auto_cut) {


                    if (gst_snr_ctx.s32_run_auto_b > 0 && gst_snr_ctx.u32_sl_en) {
                        s32_auto_b_flag = 0;
                        s32_temp_cur = sns_get_temperature();
                        ts_tdk_sl_process(gst_snr_ctx.h_sl,  s32_temp_cur, 0);
                    }
                    if (0 == test_cnt % 50) {
                        vision_log_debug(" temp = [%d]  \n", s32_temp_cur);
                    }
                }
            }
        }
        
        test_cnt++;
        ts_cpl_fix_msleep(100);
        check_aisp_create_status();  
    }
}
/** @note 测温版自动打挡线程 */
ts_void measurement_auto_cut_process(ts_void) {
    ts_s32 s32_temp_cur=0;
    ts_s32 s32_cut_cnt=0;
    ts_cpl_timeval_s_t st_time_start, st_time_cur, st_time_last_cut;   
    ts_cpl_gettimeofday(&st_time_start);
    ts_cpl_gettimeofday(&st_time_last_cut);

    ts_u32 u32_cut_status = 0;

    ts_u08 start_force_shift_st0_en = 0;
    ts_u08 start_force_shift_st1_en = 0;

    //开机第一段打档时间
    ts_s32 s32_cut_time_t0_gap_second_start = 0;
    ts_s32 s32_cut_time_t0_gap_second_end = 0;

    //开机第二段打档时间
    ts_s32 s32_cut_time_t1_gap_second_start = 0;
    ts_s32 s32_cut_time_t1_gap_second_end =0;
    
    ts_s32 s32_cut_time_gap_static = 60 * 1000; //60秒

    ts_s32 s32_auto_b_flag=0;
    ts_s32 s32_frm=16;
    ts_isp_nuc_attr_s_t st_nuc_attr;

    //ST0执行完后，ST1开始, ST0可以为空
    if(SystemParaMenu2.m_st0_total_duration_s == 0){
        start_force_shift_st0_en = 0;
    }
    else{
        start_force_shift_st0_en = 1;  
        s32_cut_time_t0_gap_second_start = st_time_start.tv_sec;
        s32_cut_time_t0_gap_second_end = s32_cut_time_t0_gap_second_start + SystemParaMenu2.m_st0_total_duration_s;
        vision_log_debug(" s32_cut_time_t0_gap_second_end %d s32_cut_time_t0_gap_second_start %d !\n",s32_cut_time_t0_gap_second_end,s32_cut_time_t0_gap_second_start);
    }

    //m_st1_m为0时，无效
    if(SystemParaMenu2.m_st1_m == 0){
        start_force_shift_st1_en = 0;
    }
    else{
        start_force_shift_st1_en = 1;  
        //ST0有效时，需要累加ST0的时间间隔
        if(start_force_shift_st0_en==1){
            s32_cut_time_t1_gap_second_start = s32_cut_time_t0_gap_second_end ;
            s32_cut_time_t1_gap_second_end =s32_cut_time_t1_gap_second_start +  (SystemParaMenu2.m_st1_m * 60); 
        }
        else{
            s32_cut_time_t1_gap_second_start = st_time_start.tv_sec;
            s32_cut_time_t1_gap_second_end = s32_cut_time_t1_gap_second_start +  (SystemParaMenu2.m_st1_m * 60);
        }
        vision_log_debug(" s32_cut_time_t1_gap_second_end %d s32_cut_time_t1_gap_second_start %d !\n",s32_cut_time_t1_gap_second_end,s32_cut_time_t1_gap_second_start);
    }

   if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {
        ///////////////////////////////////////////////////////
        //刚启动，帧数设置为32帧
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        s32_frm = st_nuc_attr.u08_stat_frm_num;
        st_nuc_attr.u08_stat_frm_num = 32;
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    }


    ts_u32 temp_cnt=0;
    ts_f32 last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
    ts_u16 diff_temp_value=0;
    vision_log_warn("gst_snr_ctx.s32_run_auto_cut %d \n",gst_snr_ctx.s32_run_auto_cut);

    static int test_cnt = 0;
    while(1) {
        u32_cut_status = 0;


        if ((0 == gst_snr_ctx.s32_run_auto_b) && 1 != s32_auto_b_flag && gst_snr_ctx.u32_sl_en) {
            ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
            st_nuc_attr.u16_coef = 0;
            ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);

            s32_auto_b_flag = 1;
        }

        // 切换挡位和测温标定状态下禁止打挡
        if (global_sys_ctrl_info.Shifting_Gears_Flag || SystemParaMenu2.m_temperCalibra_Flag || SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
            ts_cpl_msleep(300);
            continue;
        }

        if (start_force_shift_st1_en || start_force_shift_st0_en) {
            ts_cpl_gettimeofday(&st_time_cur);
            ts_s32 s32_time_diff;
            //简化判断，只看秒级时间
            if(start_force_shift_st0_en==1 && st_time_cur.tv_sec > s32_cut_time_t0_gap_second_start){
                if (st_time_cur.tv_sec >= s32_cut_time_t0_gap_second_end) {
                    vision_log_debug(" s32_cut_time_t0_gap_second_end %d  st_time_cur.tv_sec %d !\n",s32_cut_time_t0_gap_second_end,st_time_cur.tv_sec);
                    start_force_shift_st0_en = 0;
                    
                }
                else{
                    s32_time_diff = st_time_cur.tv_sec - st_time_last_cut.tv_sec;
                    if (s32_time_diff >= SystemParaMenu2.m_st0_interval_s) {
                        u32_cut_status |= 0x1;
                        ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
                    }
                }

            }
            
            if(start_force_shift_st1_en==1 && st_time_cur.tv_sec > s32_cut_time_t1_gap_second_start){
                if (st_time_cur.tv_sec >= s32_cut_time_t1_gap_second_end) {
                    vision_log_debug(" s32_cut_time_t1_gap_second_end %d  st_time_cur.tv_sec %d !\n",s32_cut_time_t1_gap_second_end,st_time_cur.tv_sec);
                    start_force_shift_st1_en = 0;
                    
                }
                else{
                    s32_time_diff = st_time_cur.tv_sec - st_time_last_cut.tv_sec;
                    if (s32_time_diff >= SystemParaMenu2.m_st1_s) {
                        u32_cut_status |= 0x20;
                        ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
                    }
                }
            }
        }

        if (0 == gst_snr_ctx.s32_run_auto_cut) {
            ts_cpl_msleep(100);
            ts_cpl_gettimeofday(&st_time_last_cut);  
            continue;
        }
        //矫正的时候不进行自动打档
        if(ISP_get_det_calibrationing_flag()>0) {
            // vision_log_debug(" det calibrationing ! stop update statics \n");
            ts_cpl_fix_msleep(500);
            continue;
        }
        //更新统计信息
        {
            global_sys_ctrl_info.isp_auto_cut_run_cnt_u64++;
            ts_cpl_gettimeofday(&st_time_cur);
            ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000000;
            global_sys_ctrl_info.isp_cut_time_sec_u32 = s32_time_diff;
            global_sys_ctrl_info.isp_temp_ad_value_u16 = ISP_get_det_ad_value();
        }
        
        if(ISP_get_ShutterMode() != SHUTTER_MODE_OFF) {
            
            //ST0,ST1两段打档结束后，才进行定时打档
            if (start_force_shift_st1_en==0 && start_force_shift_st0_en==0) {
                //获取实际的打档间隔
                s32_cut_time_gap_static  = ISP_get_shutter_timer_gap();
                {
                    ts_cpl_gettimeofday(&st_time_cur);
                    ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
                    if (s32_time_diff >= s32_cut_time_gap_static){
                        u32_cut_status |= 0x2;
                    }
                    
                }
            }

            temp_cnt++;
            if(temp_cnt>10) {
                temp_cnt = 0;
                if(SystemParaMenu2.m_det_st != 0) {
                    vot_read_det_ad();
                    diff_temp_value = (global_sys_ctrl_info.m_det_temp_value - last_det_temp_value) * 10;
                    if(abs(diff_temp_value) >= SystemParaMenu2.m_det_st) {
                        u32_cut_status |= 0x4; 
                        last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
                    }                   
                }            
            }
        }

        if (gst_snr_ctx.s32_force_cut) {
            u32_cut_status |= 0x8;
            gst_snr_ctx.s32_force_cut = 0;
        }

        if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {
            if (6 == s32_cut_cnt) { 
                // vision_log_warn("s32_force_inter = %d s32_frm %d \n", s32_force_inter,s32_frm);
                st_nuc_attr.u08_stat_frm_num = s32_frm;
                ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
                // s32_force_inter=0;
            }
        }

        if ((u32_cut_status != 0) && gst_snr_ctx.s32_run_auto_cut) {     
            //兼容手动打档优先
            if(  (u32_cut_status & 0x20 ) || (u32_cut_status & 0x8) || (u32_cut_status & 0x1)|| (ISP_get_ShutterMode() != SHUTTER_MODE_OFF)) {
                vision_log_debug("to cut, status = 0x%x, diff_temp_value = %d \n", u32_cut_status, diff_temp_value);
                ISP_cut_new();
                s32_temp_cur = global_sys_ctrl_info.m_cali_temp_value;
                last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
                temp_cnt = 0;
                s32_cut_cnt++;
            }
            ts_cpl_gettimeofday(&st_time_last_cut);
            u32_cut_status = 0;
        }
        else //没有开机/强制/定时打档，则进行判断是否需要温漂打档
        {
            if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {
                if (gst_snr_ctx.s32_run_auto_b && gst_snr_ctx.s32_run_auto_cut) {

                    if (gst_snr_ctx.s32_run_auto_b > 0 && gst_snr_ctx.u32_sl_en) {
                        s32_auto_b_flag = 0;
                        s32_temp_cur = sns_get_temperature();
                        ts_tdk_sl_process(gst_snr_ctx.h_sl,  s32_temp_cur, 0);
                    }
                    if (0 == test_cnt % 50) {
                        vision_log_debug("temp = [%d]  \n",  s32_temp_cur);
                    }
                }
            }
        }
        
        test_cnt++;
        ts_cpl_fix_msleep(100);
        check_aisp_create_status();  
    }
}

void vot_occ_update_with_only_shutter(void )
{
    ts_viu_corr_rst_s_t st_rst_attr;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);  

    tdk_snrcfg_shutter_close();

    st_rst_attr.u08_reset_only = 0;
    st_rst_attr.u08_reset_val = (ts_u08)0x80;
    
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
    snr_init_ooc();
    
    tdk_snrcfg_shutter_open();

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

}

void vot_occ_update_with_shutter(void)
{
    ts_u32 u32_ooc_val = 0x20;
    ts_viu_corr_rst_s_t st_rst_attr;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
    ts_cpl_msleep(300);        

    tdk_snrcfg_shutter_close();

    st_rst_attr.u08_reset_only = 1;
    st_rst_attr.u08_reset_val = (ts_u08)u32_ooc_val;

    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);

    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, ts_null);
    ts_cpl_msleep(40); 
    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    tdk_snrcfg_shutter_open();

    ts_tdk_vpo_freeze(TS_VPO_MDEV0);

    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
    ts_cpl_msleep(40);       
    ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst
    ts_cpl_msleep(40);       
    ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

}


void vot_occ_update_with_calibration(ts_u08 force_cut_flag)
{
    ts_u32 u32_ooc_val = 0x80;
    ts_viu_corr_rst_s_t st_rst_attr;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
    ts_cpl_msleep(50);  
    tdk_snrcfg_shutter_close();

    st_rst_attr.u08_reset_only = 0;
    st_rst_attr.u08_reset_val = (ts_u08)u32_ooc_val;

    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
    snr_init_ooc();

    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    tdk_snrcfg_shutter_open();

    ts_tdk_vpo_freeze(TS_VPO_MDEV0);

    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
    ts_cpl_msleep(40);       
    ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst
    ts_cpl_msleep(150);       
    ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);


    global_sys_ctrl_info.m_powerup_inited_flag = 1;
    vision_log_debug("leave: \n");

}


void vot_cut_with_option(ts_u08 dde_mode)
{
    snr_attr_s_t *pst_attr;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    pst_attr = &gst_snr_ctx.st_attr;
	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间

	ts_u32 u32_msecs_3frame = 3*u32_msecs;

	// vision_log_warn("new shutter test1 ,u32_msecs %d !\n",u32_msecs);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
   
	ts_cpl_fix_msleep(u32_msecs_3frame);  //延时三帧时间 ,延时时间太短,打挡会闪屏  

	tdk_snrcfg_shutter_close();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	tdk_snrcfg_shutter_open();
	
	ts_tdk_vpo_freeze(TS_VPO_MDEV0);

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

    if(dde_mode==0){
        ISP_dde_set_en(0);
    }
    else{
        ISP_dde_set_en(1);
    }
	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(u32_msecs_3frame); ///延时三帧时间              
	ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    vision_log_debug("leave: \n");
}

void vot_reset_sence_env(void)
{

    // dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    // if(res_type==DECTOR_RESOLUTION_384X288){
        ts_cpl_fix_msleep(2000);
        ISP_set_auto_cut_flag(0);
        ts_cpl_fix_msleep(1000);
        vot_cut_with_option(1);

        ISP_set_auto_cut_flag(1);
        // ISP_dde_set_mode(2); 
    // }

    // vision_log_warn("leave: \n");
    global_sys_ctrl_info.m_powerup_inited_flag = 1;
}

void vot_set_vot_pod_type(ts_u08 vot_pod_type)
{
    if( vot_pod_type >= VOT_POD_TYPE_BUTT){ 
        vot_pod_type = VOT_POD_TYPE_WHITE_GRAY;
    }
    SystemParaMenu2.m_vot_pod_type = vot_pod_type;
}

void vot_set_t8_mode_force_bpc(ts_u08 t8_mode_force_bpc)
{
    if( t8_mode_force_bpc >= VOT_SWITCH_BUTT){ 
        t8_mode_force_bpc = VOT_SWITCH_OFF;
    }
    SystemParaMenu2.m_t8_mode_force_bpc_enable = t8_mode_force_bpc;
}

ts_u08 vot_get_t8_mode_force_bpc(void)
{
   return SystemParaMenu2.m_t8_mode_force_bpc_enable;
}

ts_u32 vot_get_fn_vot_nuc_kb_table_size(ts_s32 width, ts_s32 height)
{
    uint32_t pixels = width * height;
    
    uint32_t bpc_buf_size = pixels << 1;
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        bpc_buf_size = pixels >> 3;
    }
   
   return bpc_buf_size;
}


ts_u32 vot_get_fn_vot_nuc_kb_file_size(void)
{
    ts_u32 u32_file_size = FN_VOT_NUC_KB_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_VOT_NUC_KB_FILE_SIZE;
    }
    else{
        u32_file_size = FN_VOT_NUC_KB_FILE_SIZE;
    }
   
   return u32_file_size;
}


ts_u32 vot_get_fn_occ_file_size(void)
{
    ts_u32 u32_file_size = FN_OCC_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_OCC_FILE_SIZE;
    }
    else{
        u32_file_size = FN_OCC_FILE_SIZE;
    }
   
   return u32_file_size;
}

ts_u32 vot_get_fn_k_file_size(void)
{
    ts_u32 u32_file_size = FN_K_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_K_FILE_SIZE;
    }
    else{
        u32_file_size = FN_K_FILE_SIZE;
    }
   
   return u32_file_size;
}

ts_u32 vot_get_fn_b_file_size(void)
{
    ts_u32 u32_file_size = FN_B_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_B_FILE_SIZE;
    }
    else{
        u32_file_size = FN_B_FILE_SIZE;
    }
   
   return u32_file_size;
}


ts_u32 vot_get_fn_sffc_file_size(void)
{
    ts_u32 u32_file_size = FN_SFFC_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_OCC_FILE_SIZE;
    }
    else{
        u32_file_size = FN_SFFC_FILE_SIZE;
    }
   
   return u32_file_size;
}

ts_u32 vot_get_fn_aisp_file_size(void)
{
    ts_u32 u32_file_size = FN_AISP_FILE_SIZE;
    //nor flash
    if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
        u32_file_size = N6E_FN_AISP_FILE_SIZE;
    }
    else{
        u32_file_size = FN_AISP_FILE_SIZE;
    }
   
   return u32_file_size;
}


ts_u08 vot_get_flash_type(void)
{
    return global_sys_ctrl_info.m_flash_type;
}

void vot_easylog_init(void)
{
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LINE | ELOG_FMT_FUNC | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LINE | ELOG_FMT_FUNC  | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO,ELOG_FMT_LINE |  ELOG_FMT_FUNC  | ELOG_FMT_TIME);
    // elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    // elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);

    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LINE |  ELOG_FMT_FUNC  | ELOG_FMT_TIME );
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LINE |  ELOG_FMT_FUNC  | ELOG_FMT_TIME);
    /* start EasyLogger */
    elog_start();
}


int vot_get_hot_statistics(void)
{
    ts_s32 ret;
    ts_isp_hot_attr_s_t attr;       
    ts_isp_hot_statis_s_t statis;   

    // 1. 配置热点检测参数
    memset(&attr, 0, sizeof(attr));
    attr.u08_enable = 1;            
    attr.u08_samp_step = 0;         
    attr.u16_hot_thr = 16000;        
    attr.u16_edge_thr = 13000;        
    ret = ts_tdk_isp_set_hot_attr(TS_ISP_MDEV0, &attr);
    if (ret != 0) {
        vision_log_debug("Failed to set hot attributes. Error code: %d\n", ret);
        return 0;
    }

    // 2. 验证是否设置成功
    ts_isp_hot_attr_s_t get_attr;
    memset(&get_attr, 0, sizeof(get_attr));
    ret = ts_tdk_isp_get_hot_attr(TS_ISP_MDEV0, &get_attr);
    if (ret != 0) {
        vision_log_debug("Failed to get hot attributes. Error code: %d\n", ret);
        return 0;
    }
    vision_log_debug("u08_enable = %d\n", get_attr.u08_enable);
    vision_log_debug("u08_samp_step = %d\n", get_attr.u08_samp_step);
    vision_log_debug("u16_hot_thr = %d\n", get_attr.u16_hot_thr);
    vision_log_debug("u16_edge_thr = %d\n", get_attr.u16_edge_thr);

    // 3. 获取热点检测统计数据
    memset(&statis, 0, sizeof(statis));
    ret = ts_tdk_isp_get_hot_statis(TS_ISP_MDEV0, &statis, 1000);
    if (ret != 0) {
        vision_log_debug("Failed to get hot statistics. Error code: %d\n", ret);
        return 0;
    }

    // 4. 打印统计信息
    vision_log_debug("Hot spot count: %d\n", statis.u32_hot_cnt);
    vision_log_debug("Edge point count: %d\n", statis.u32_edge_cnt);
    

    // 5. 释放统计数据资源
    ret = ts_tdk_isp_free_hot_statis(TS_ISP_MDEV0, &statis);
    if (ret != 0) {
        vision_log_debug("Failed to free hot statistics. Error code: %d\n", ret);
    }

    return ret;
    
}

void vot_modbus_crc16_data_verification(const u8 *Array, u32 Len, ts_u16 *crc16_code)
{
	ts_u32 IX;
	ts_u16 IY;
	ts_u16 CRC;
	if (Len <= 0)
	{
		return;
	}
	CRC = 0xFFFF;
	for (IX = 0; IX < Len; IX++)
	{
		CRC = CRC ^ (ts_u16)(Array[IX]);
		for (IY = 0; IY <= 7; IY++)
		{
			if ((CRC & 1) != 0)
			{
				CRC = (CRC >> 1) ^ 0xA001;
			}
			else
			{
				CRC = CRC >> 1;
			}
		}
	}
    if(crc16_code){
        *crc16_code = CRC;
    }
	
}

int vot_check_firmware_crc32_from_flash(ts_u16 u16_crc_src, ts_u32 u32_addr, ts_u32 u32_want_rd_size)
{
    ts_u32 rd_cnt = 0;
    ts_u16 crc16_code=0;
    ts_s32 ret=-1;

    ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,(u32_want_rd_size+2048));
    rd_cnt = ts_lib_sfc_raw_read(u32_addr, pu08_rd_buf, u32_want_rd_size);
    if (rd_cnt > 0){
        vision_log_debug("read %d u32_want_rd_size %d bytes from flash [%x]! \n", rd_cnt,u32_want_rd_size,u32_addr);
        if(rd_cnt == u32_want_rd_size){
            vision_log_debug("read %d bytes from flash success ! \n", rd_cnt);
            vot_modbus_crc16_data_verification(pu08_rd_buf, rd_cnt, &crc16_code);
            if(u16_crc_src==crc16_code){
                vision_log_debug("crc16_code match ! \n");
               ret =  0;
            }
            else{
                vision_log_debug("crc16_code not match ! \n");
            }
        }
        else{
            vision_log_debug("read %d bytes from flash failed ! \n", rd_cnt);
        }
    }

    if(pu08_rd_buf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo, (ts_void **)&pu08_rd_buf);
        pu08_rd_buf = NULL;
    }

    return ret;
}



ts_u08 vot_get_temp_drift_aisp_enable(void)
{
    ts_u08 enable=0;
    if(global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_A12 || global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_A6){
        enable = 1;
    }
    else if(global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_N6E || global_sys_ctrl_info.m_t10_tpu_type == T10_CPU_TYPE_N6){
        enable = 0;
    }
    else{
        enable = 0;
    }

    return enable;
}

ts_void vot_set_temp_drift_aisp_mode(ts_u08 u8_mode)
{

    if(u8_mode >= TEMP_DRIFT_COMPENSATION_AI_APP_BUTT){
        u8_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
    }

	if(vot_get_temp_drift_aisp_enable() == 0){
       u8_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
    }

    SystemParaMenu2.m_temp_drift_aisp_mode = u8_mode;
}