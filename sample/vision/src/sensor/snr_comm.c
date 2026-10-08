/*
 * snr_comm.c -- vision 探测器公共部分代码
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_compiler.h>
#include <base/ts_comm.h>
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


#include <command.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "sffs_files.h"
#include "api.h"
#include "libupload.h"
#include "snr_comm.h"
#include "f_tdk.h"
#include "pcolor.h"
#include <snr_comm_vot.h>
#include <menu/votmain.h>
#include <menu/ispfunction.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
#include <sl.h>

extern const ts_u08 sast_iso_green_vuya[][4];
extern const ts_u08 sast_iso_red_vuya[][4];
extern const ts_u08 sast_iso_white_vuya[][4];
extern const ts_u08 sast_iso_black_vuya[][4];

extern SysCtrlInfo_s_t global_sys_ctrl_info;

typedef struct i2c_comm_ctx_s {
    ts_u32 u32_opened;
    ts_s32 s32_i2c_hdl;
    ts_u32 u32_dev_addr;
} i2c_comm_ctx_s_t;

static i2c_comm_ctx_s_t sst_i2c_ctx = {0};

snr_ctx_s_t gst_snr_ctx = {0};
bp_pos_info_s_t gst_bp_info = {0};

static ts_void snr_start_npu(snr_attr_s_t *pst_attr);
ts_s32 snr_get_cut_force(ts_void);

/* nuc 计算相关 */
#define NUC_SW_FRM_CNT  (32)
#define NUC_UPL_ID      (0)
typedef struct nuc_ctx_s {
    ts_u32 u32_nuc_1_addr;
    ts_u32 u32_nuc_2_addr;
    ts_u32 u32_nuc_3_addr;
    ts_u32 u32_nuc_4_addr;

    ts_s64 s64_sum_s1;
    ts_s64 s64_sum_s2;
    ts_s32 s32_cnt_nuc_1;
    ts_s32 s32_cnt_nuc_2;
    ts_u32 u32_raw_addr;
    ts_u32 u32_sum_addr;

    ts_u32 u32_sum_sffc_addr;
    ts_u32 u32_sum_sffc_flt;

    ts_s32 s32_nuc_bp_thr;
    ts_s32 s32_nuc_bp_k_min;
    ts_s32 s32_nuc_bp_k_max;
} nuc_ctx_s_t;

static nuc_ctx_s_t sst_nuc_ctx = 
{
    .u32_nuc_1_addr       = 0,
    .u32_nuc_2_addr       = 0,
    .u32_nuc_3_addr       = 0,
    .u32_nuc_4_addr       = 0,
    .s64_sum_s1           = 0,
    .s64_sum_s2           = 0,
    .s32_cnt_nuc_1        = 0,
    .s32_cnt_nuc_2        = 0,
    .u32_raw_addr         = 0,
    .u32_sum_addr         = 0,
    .u32_sum_sffc_addr    = 0, 
    .u32_sum_sffc_flt     = 0,
    .s32_nuc_bp_thr       = 600,
    .s32_nuc_bp_k_min     = 0,
    .s32_nuc_bp_k_max     = 32767,
};


static TS_FUNC_UNUSED ts_void snr_set_cvbs_pod(ts_void)
{
    ts_u32 i, j;
    ts_u08 pod_y;
    ts_buffer_s_t st_buf;

    /* Get cvbs power on display buff  */
    ts_tdk_vpo_get_pod_buf(TS_VPO_CVBS_MDEV, &st_buf);

    /* UYVY */
    /* Generate test power on display value */
    for (i = 0; i < st_buf.u32_height; i++) {
        for (j = 0; j < st_buf.u32_width; j++) {
            pod_y = 0x80;
            if (0 == j % 2)
                st_buf.pu08_ptr[i * st_buf.u32_width * 2 + j * 2] = 0x80;
            else
                st_buf.pu08_ptr[i * st_buf.u32_width * 2 + j * 2] = 0x80;

            st_buf.pu08_ptr[i * st_buf.u32_width * 2 + j * 2 + 1] = pod_y;
        }
    }
}


//static 
ts_void snr_set_grp_pod(ts_grp_t h_grp)
{
    ts_u32 i, j;
    ts_buffer_s_t st_buf;

    /* Get grp0 power on display buff  */
    ts_tdk_vpo_get_grp_pod_buf(h_grp, &st_buf);

    ts_cpl_sync_dma_for_cpu(ts_null, st_buf.phys_addr, (st_buf.u32_height * st_buf.u32_width * 2));

    if(TS_VPO_GRP0 == h_grp) {
        vision_log_debug("POD addr(phy),w,h:%x(%x),%d,%d\n", (ts_u32)st_buf.pu08_ptr, (ts_u32)st_buf.phys_addr, st_buf.u32_height, st_buf.u32_width);

        /* Y8 */
        for (i = 0; i < st_buf.u32_height; i++) {
            for (j = 0; j < st_buf.u32_width; j++) {
                st_buf.pu08_ptr[i * st_buf.u32_width + j] = 0x80;
            }
        }
    }
}


static ts_void vpo_cfg_grp0(ts_mdev_t h_yuv_ch0, ts_mdev_t h_yuv_ch1, ts_mdev_t h_yuv_ch2, ts_mdev_t h_raw_ch, snr_attr_s_t *pst_attr)
{
    ts_vpo_grp_attr_s_t st_grp_atr;
    ts_s32 i;
    ts_u08 *pu08_info_addr;

    ts_cpl_memset32((ts_void *)&st_grp_atr, 0, (sizeof(ts_vpo_grp_attr_s_t) >> 2));

    /* 对原图不进行剪切 */
    st_grp_atr.u16_cut_width = 0;
    st_grp_atr.u16_cut_height = 0;
    st_grp_atr.u16_cut_start_v = 0;
    st_grp_atr.u16_cut_start_h = 0;

    st_grp_atr.u08_hmirr_en = pst_attr->u08_hmirr_en;
    st_grp_atr.u08_vmirr_en = pst_attr->u08_vmirr_en;
    st_grp_atr.u08_bank_num = 3;

    st_grp_atr.u08_head_en = 0;
    st_grp_atr.u08_fire_en = 0;

    if (pst_attr->u08_info_en > 0) {
        vision_log_debug("pst_attr->u08_info_line_num %d, pst_attr->u08_info_en = %d \n", pst_attr->u08_info_line_num, pst_attr->u08_info_en);
        if(pst_attr->u08_raw_oe){
            st_grp_atr.u16_info_size = pst_attr->u08_info_line_num * pst_attr->s32_img_w * 2 *2;
        }
        else{
            st_grp_atr.u16_info_size = pst_attr->u08_info_line_num * pst_attr->s32_img_w * 2;
        }
        if (ts_null == pst_attr->pv_info_buf) {
            if (st_grp_atr.u16_info_size > 0) {
				/* 多申请一倍，用于双缓冲切换 */
                pu08_info_addr = (ts_u08 *)ts_cpl_tmm_malloc(ts_null, (st_grp_atr.u16_info_size * 2), TS_HW_ALIGN_SIZE, "vpo_info");
                if (ts_null == pu08_info_addr) {
                    printf("%s : tmm malloc for vpo info fail!\n", TS_ENOMEM_STR);
                } else {
                    printf("info addr:%p\n", pu08_info_addr);
                    for (i = 0; i < st_grp_atr.u16_info_size; i++) {
                        pu08_info_addr[i] = i;
                    }
                }
                ts_cpl_dma_map_single(ts_null, (void *)pu08_info_addr, st_grp_atr.u16_info_size, TS_CPL_DMA_TO_DEVICE);
                global_sys_ctrl_info.m_user_metadata_addr = (ts_u32)pu08_info_addr;
                global_sys_ctrl_info.m_user_metadata_size = st_grp_atr.u16_info_size;
                
            } else {
                pu08_info_addr = ts_null;
                st_grp_atr.u16_info_size = 0;
            }

            pst_attr->pv_info_buf = (ts_void *)pu08_info_addr;
        } else {
            pu08_info_addr = (ts_u08 *)pst_attr->pv_info_buf;
        }

        st_grp_atr.u32_info_addr = (ts_u32)pu08_info_addr;
    } else {
        st_grp_atr.u32_info_addr = 0;
        st_grp_atr.u16_info_size = 0;
    }

    st_grp_atr.u08_ch0_filter_mode = 0;
    st_grp_atr.u08_ch1_filter_mode = 0;

    st_grp_atr.u32_yuv_out_ch0_sel = h_yuv_ch0;
    st_grp_atr.u32_yuv_out_ch1_sel = h_yuv_ch1;
    st_grp_atr.u32_yuv_out_ch2_sel = h_yuv_ch2;
    st_grp_atr.u32_raw_out_ch_sel = h_raw_ch;
    st_grp_atr.u08_digital_ch_yuv2y_en = 0;
    st_grp_atr.u08_digital_ch_yuv_raw_seq = 0;

    st_grp_atr.pv_tmm_zone = gpv_tmm_vpo;

    if (h_raw_ch != TS_VPO_NULL_MDEV) {
        st_grp_atr.u08_raw_pos = pst_attr->u08_raw_pos;
        st_grp_atr.u08_digital_ch_yuv_raw_seq = pst_attr->u08_raw_seq;
    }

    /* 是否开启放大功能,或者选择功能 */
    if ((pst_attr->st_npu.en_ops >= TS_NPU_OPS_SR) && (pst_attr->st_npu.en_ops < TS_NPU_OPS_BUTT)) {
        st_grp_atr.u08_super_resolution_en = 1;
        st_grp_atr.u08_sr_type = (ts_u08)pst_attr->st_npu.en_ops;
    } else {
        st_grp_atr.u08_super_resolution_en = 0;
        st_grp_atr.u08_sr_type = 0;
    }


   if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024){
        st_grp_atr.u08_step_frame_num = 0;
   }
   else{
        st_grp_atr.u08_step_frame_num = vot_get_u08_step_frame_num();
   }

   

    /* grp 仅支持 Y8 */
    if (pst_attr->u32_pod_dly_ms > 0)
        st_grp_atr.u08_pod_en = 1;
    else
        st_grp_atr.u08_pod_en = 0;

    st_grp_atr.pv_tmm_zone = gpv_tmm_vpo;

    ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, (ts_vpo_grp_attr_s_t *)&st_grp_atr);
}


static ts_void vpo_cfg_grp0_cvbs(snr_attr_s_t *pst_snr)
{
    ts_vpo_chn_attr_s_t st_disp_cvbs;
    ts_vpo_cvbs_attr_s_t *pst_cvbs_attr;
    snr_cvbs_attr_s_t *pst_cfg;

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
    if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        // st_disp_cvbs.u16_disp_width = 1280;
        // st_disp_cvbs.u16_disp_height = 1024;
        st_disp_cvbs.f32_disp_fps = 25;
    }

#if 0
    /* 兼容老 sdk， cvbs 单独可设置开机画面 */
    if (pst_snr->u32_pod_dly_ms > 0)
        st_disp_cvbs.u08_pod_en = 1;
    else
        st_disp_cvbs.u08_pod_en = 0;
#else
    /* 推荐当前整组 grp 统一设置开机画面，仅支持 Y8 */
    st_disp_cvbs.u08_pod_en = 0;
#endif

    pst_cvbs_attr = (ts_vpo_cvbs_attr_s_t *)&st_disp_cvbs.un_chn_attr.st_cvbs_attr;
    pst_cvbs_attr->u08_test_bar = 0;
    pst_cvbs_attr->u08_dac_drv_strength = 13;

    if (pst_cfg->u08_sd_en > 0) {
        pst_cvbs_attr->u08_bank_num = 1;
        pst_cvbs_attr->u08_short_delay_en = 1;
    } else {
        pst_cvbs_attr->u08_bank_num = 3;
        pst_cvbs_attr->u08_short_delay_en = 0;
    }

    ts_tdk_vpo_set_chn_attr(TS_VPO_CVBS_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_cvbs);
}


static ts_void vpo_cfg_grp0_mem(snr_attr_s_t *pst_snr)
{
    ts_vpo_chn_attr_s_t st_disp_mem;
    ts_vpo_mem_attr_s_t *pst_mem_attr;

    if (0 == pst_snr->u32_mem_en)
        return;

    /* 另一显示通道配置成 mem 输出 */
    st_disp_mem.u16_disp_height =(ts_u16)pst_snr->s32_vo_img_h;
    st_disp_mem.u16_disp_width = (ts_u16)pst_snr->s32_vo_img_w;
    st_disp_mem.f32_disp_fps = (ts_f32)pst_snr->s32_fps;
    st_disp_mem.u08_pod_en = 0;


    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    vision_log_warn("res_type %d \n",res_type);
    vision_log_warn("global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc %d \n",global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc);
    vision_log_warn("SystemParaMenu2.m_uvc_enlarger_mode %d \n",SystemParaMenu2.m_uvc_enlarger_mode);
    if(SystemParaMenu2.m_bpc_ref_mode != BPC_MODE_ON_UVC && SystemParaMenu2.m_uvc_enlarger_mode==OUTPUT_MODE_ENLARGE_ON  && 
        global_sys_ctrl_info.m_uvc_enlarger_mode_in_bpc!=OUTPUT_MODE_ENLARGE_ON )
    {
        if(res_type==DECTOR_RESOLUTION_384X288){
            ts_u16 u16_disp_width=640,  u16_disp_height=512;
            if (get_customer_type() == CUSTOMER_TYPE_HAND)//手持机定制，用于384电子变倍768
            {
                u16_disp_width=768,  u16_disp_height=576;
            }
            st_disp_mem.u16_disp_height = u16_disp_height;
            st_disp_mem.u16_disp_width = u16_disp_width;
            vision_log_warn("res_type %d u16_disp_width %u u16_disp_height %u \n",res_type,u16_disp_width,u16_disp_height );
        }
    }
     
     //开启T8模式后，输出宽高只能和输入一致
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        st_disp_mem.u16_disp_height = (ts_u32)pst_snr->s32_vo_img_h;
        st_disp_mem.u16_disp_width =(ts_u32)pst_snr->s32_vo_img_w;
        
    }

    vision_log_warn(" u16_disp_height %u u16_disp_width %u \n",st_disp_mem.u16_disp_height,st_disp_mem.u16_disp_width );
    

    pst_mem_attr = (ts_vpo_mem_attr_s_t *)&st_disp_mem.un_chn_attr.st_mem_attr;
 /* 数字通道需要保持一致 */
    pst_mem_attr->u08_mode = 1;
    if (pst_snr->st_mipi.u08_en > 0) {
        if (pst_snr->st_mipi.u08_dsi_en > 0) {
            st_disp_mem.u16_disp_height = (ts_u16)pst_snr->st_mipi.u16_dsi_disp_height;
            st_disp_mem.u16_disp_width = (ts_u16)pst_snr->st_mipi.u16_dsi_disp_width;
            pst_mem_attr->u08_mode = 0;
        }
    } else if (pst_snr->st_bte.u08_en > 0) {
        st_disp_mem.u16_disp_height = pst_snr->st_bte.u16_output_height;
        st_disp_mem.u16_disp_width = pst_snr->st_bte.u16_output_width;
    } else {
        pst_mem_attr->u08_mode = 0;
    }
    pst_mem_attr->u08_bank_num = 2;
    //pst_mem_attr->u08_mode = 0;
    pst_mem_attr->u16_fill_size = 0;
    pst_mem_attr->u32_frm_size = 0;

    if (SystemParaMenu2.m_uvc_output_format == UVC_YUV_FORMAT_YUYV) {
        pst_mem_attr->u08_data_endian = 0;
    }
    else {
        pst_mem_attr->u08_data_endian = 1;
    }
    ts_tdk_vpo_set_chn_attr(TS_VPO_MEM_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_mem);
}

static ts_void vpo_cfg_grp0_bte(snr_attr_s_t *pst_snr)
{
    ts_vpo_bte_attr_s_t *pst_bte_attr;
    ts_vpo_bte_exsync_s_t *pst_bte_exsync_attr;
    ts_vpo_chn_attr_s_t st_disp_btenc;
    snr_bte_attr_s_t *pst_snr_bte;

    pst_snr_bte = (snr_bte_attr_s_t *)&pst_snr->st_bte;
    if (0 == pst_snr_bte->u08_en)
        return;

    /* 另一显示通道配置成 bt 输出 */
    st_disp_btenc.u16_disp_height = pst_snr->s32_vo_img_h;
    st_disp_btenc.u16_disp_width = pst_snr->s32_vo_img_w;
    st_disp_btenc.f32_disp_fps = (ts_f32)pst_snr->s32_fps;
    st_disp_btenc.u08_pod_en = 0;


    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    vision_log_warn("res_type %d \n",res_type);
    if( SystemParaMenu2.m_bt_enlarger_mode == OUTPUT_MODE_ENLARGE_ON )
    {
        if(res_type==DECTOR_RESOLUTION_384X288){
            ts_u16 u16_disp_width=640,  u16_disp_height=512;
            if (get_customer_type() == CUSTOMER_TYPE_HAND)//手持机定制，用于384电子变倍768
            {
                u16_disp_width=768,  u16_disp_height=576;
            }
            st_disp_btenc.u16_disp_height = u16_disp_height;
            st_disp_btenc.u16_disp_width = u16_disp_width;
            vision_log_warn("res_type %d u16_disp_width %u u16_disp_height %u \n",res_type,u16_disp_width,u16_disp_height );
        }
    }

    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        st_disp_btenc.u16_disp_height = (ts_u32)pst_snr->s32_vo_img_h;
        st_disp_btenc.u16_disp_width =(ts_u32)pst_snr->s32_vo_img_w;
    }

    pst_bte_attr = (ts_vpo_bte_attr_s_t *)&st_disp_btenc.un_chn_attr.st_bte_attr;
    pst_bte_exsync_attr = (ts_vpo_bte_exsync_s_t *)&pst_bte_attr->st_sync_cfg.st_ex_sync;

    pst_bte_attr->u08_test_bar = 0;
    pst_bte_attr->st_sync_cfg.st_in_sync.en_thr = TS_VPO_BTE_THR_NULL;
    pst_bte_attr->st_sync_cfg.st_in_sync.u08_bit8_scode = 0;
#if 1
    if (pst_snr_bte->u08_8bit_en > 0)
        pst_bte_attr->u08_bit_mode = TS_VPO_BTE_BITMODE_LO_8B_LSB;
    else
        pst_bte_attr->u08_bit_mode = TS_VPO_BTE_BITMODE_16B_LSB;
#else
    // 刘明坦提出测试BT接口输出YUV格式数据，不用后可删除
    if (pst_snr_bte->u08_8bit_en > 0)
        pst_bte_attr->u08_bit_mode = TS_VPO_BTE_BITMODE_LO_8B_MSB;
    else
        pst_bte_attr->u08_bit_mode = TS_VPO_BTE_BITMODE_16B_LSB;
#endif

    if(pst_snr_bte->u08_external_sync_en > 0)
        pst_bte_attr->u08_sync_mode = TS_SYNC_MODE_EXTERNAL;
    else
        pst_bte_attr->u08_sync_mode = TS_SYNC_MODE_INTERNAL;

    //pst_bte_exsync_attr->en_def_lvl = TS_ELEC_LVL_HI;
    pst_bte_exsync_attr->en_def_lvl_v = TS_ELEC_LVL_HI;
    pst_bte_exsync_attr->en_def_lvl_h = TS_ELEC_LVL_HI;
    pst_bte_exsync_attr->en_def_lvl_de = TS_ELEC_LVL_HI;

    pst_bte_attr->st_sync_cfg.u08_redefine_timing_en = 0; //此处需要初始化st_sync_cfg.u08_redefine_timing_en为0，系统默认是为1，否则运行失败
    if (pst_snr_bte->u08_redefine_timing_en > 0) {
        vision_log_warn("pst_snr_bte->u08_redefine_timing_en :  %d \n",pst_snr_bte->u08_redefine_timing_en);
        pst_bte_attr->st_sync_cfg.u08_redefine_timing_en = 1;
        pst_bte_attr->st_sync_cfg.u08_padding_en = (st_disp_btenc.u16_disp_height < pst_snr->st_bte.u16_output_height) || (st_disp_btenc.u16_disp_width < pst_snr->st_bte.u16_output_width);
        pst_bte_attr->st_sync_cfg.u16_output_width = pst_snr_bte->u16_output_width;
        pst_bte_attr->st_sync_cfg.u16_output_height = pst_snr_bte->u16_output_height;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_bp = pst_snr_bte->u16_vsync_bp;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_per = pst_snr_bte->u16_vsync_per;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_fp= pst_snr_bte->u16_vsync_fp;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_bp = pst_snr_bte->u16_hsync_bp;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_per = pst_snr_bte->u16_hsync_per;
        pst_bte_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_fp= pst_snr_bte->u16_hsync_fp;
    }

    ts_tdk_vpo_set_chn_attr(TS_VPO_BTENC_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_btenc);
}

static ts_void vpo_cfg_grp0_mipi_csi(snr_attr_s_t *pst_snr)
{
    ts_vpo_chn_attr_s_t st_disp_mipi_csi2;
    ts_vpo_mipi_csi2_attr_s_t *pst_csi_attr;

    if (0 == pst_snr->st_mipi.u08_en)
        return;
    
    if (pst_snr->st_mipi.u16_data_rate < 100) {
        printf("mipi data_rate must >= 100\n");

        return;
    }

    ts_cpl_memset32((ts_void *)&st_disp_mipi_csi2, 0, (sizeof(ts_vpo_chn_attr_s_t) >> 2));

    st_disp_mipi_csi2.u16_disp_height =(ts_u32)pst_snr->s32_vo_img_h;
    st_disp_mipi_csi2.u16_disp_width = (ts_u32)pst_snr->s32_vo_img_w;
    st_disp_mipi_csi2.f32_disp_fps = (ts_f32)pst_snr->s32_fps;
    st_disp_mipi_csi2.u08_pod_en = 0;

    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    vision_log_warn("res_type %d \n",res_type);
    vision_log_warn("pst_snr->s32_fps %d \n",pst_snr->s32_fps);
    if( SystemParaMenu2.m_mipi_enlarger_mode == OUTPUT_MODE_ENLARGE_ON )
    {
        if(res_type==DECTOR_RESOLUTION_384X288){
            ts_u16 u16_disp_width=640,  u16_disp_height=512;
            if (get_customer_type() == CUSTOMER_TYPE_HAND)//手持机定制，用于384电子变倍768
            {
                u16_disp_width=768,  u16_disp_height=576;
            }
            st_disp_mipi_csi2.u16_disp_height = u16_disp_height;
            st_disp_mipi_csi2.u16_disp_width = u16_disp_width;
            vision_log_warn("res_type %d u16_disp_width %u u16_disp_height %u \n",res_type,u16_disp_width,u16_disp_height );
        }
    }
    
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        st_disp_mipi_csi2.u16_disp_height = (ts_u32)pst_snr->s32_vo_img_h;
        st_disp_mipi_csi2.u16_disp_width =(ts_u32)pst_snr->s32_vo_img_w;
    }

    pst_csi_attr = (ts_vpo_mipi_csi2_attr_s_t *)&st_disp_mipi_csi2.un_chn_attr.st_mipi_csi2_attr;

    pst_csi_attr->u08_clock_continue = pst_snr->st_mipi.u08_clock_continue;
    pst_csi_attr->u08_data_type = pst_snr->st_mipi.u08_data_type;
    pst_csi_attr->u08_dphy_lane_num = pst_snr->st_mipi.u08_dphy_lane_num;
    pst_csi_attr->u16_data_rate = pst_snr->st_mipi.u16_data_rate;
    pst_csi_attr->u08_dlane_o_strong = pst_snr->st_mipi.u08_dlane_o_strong;
    pst_csi_attr->u08_clane_o_strong = pst_snr->st_mipi.u08_clane_o_strong;

    vision_log_warn("pst_csi_attr->u16_data_rate %d \n",pst_csi_attr->u16_data_rate);
    vision_log_warn("pst_csi_attr->u08_dphy_lane_num %d \n",pst_csi_attr->u08_dphy_lane_num);
    vision_log_warn("pst_csi_attr->u08_clock_continue %d \n",pst_csi_attr->u08_clock_continue);
    vision_log_warn("pst_csi_attr->u08_data_type %d \n",pst_csi_attr->u08_data_type);


    ts_tdk_vpo_set_chn_attr(TS_VPO_MIPI_CSI_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_mipi_csi2);
}

static ts_void vpo_cfg_grp0_mipi_dsi(snr_attr_s_t *pst_snr)
{
    ts_vpo_chn_attr_s_t st_mipi_dsi;
    ts_vpo_mipi_dsi_attr_s_t *pst_dsi_attr;

    if (0 == pst_snr->st_mipi.u08_en)
        return;

    if (0 == pst_snr->st_mipi.u08_dsi_en)
        return;
    
    if (pst_snr->st_mipi.u16_data_rate < 100) {
        printf("mipi data_rate must >= 100\n");

        return;
    }

    ts_cpl_memset32((ts_void *)&st_mipi_dsi, 0, (sizeof(ts_vpo_chn_attr_s_t) >> 2));

    st_mipi_dsi.u16_disp_height = (ts_u32)pst_snr->st_mipi.u16_dsi_disp_height;
    st_mipi_dsi.u16_disp_width = (ts_u32)pst_snr->st_mipi.u16_dsi_disp_width;
    st_mipi_dsi.f32_disp_fps = (ts_f32)pst_snr->st_mipi.f32_dsi_disp_fps;
    st_mipi_dsi.u08_pod_en = 0;

    pst_dsi_attr = (ts_vpo_mipi_dsi_attr_s_t *)&st_mipi_dsi.un_chn_attr.st_mipi_dsi_attr;

    pst_dsi_attr->u08_padding_en = (pst_snr->st_mipi.u16_dsi_disp_height < pst_snr->st_mipi.u16_dsi_output_height) || (pst_snr->st_mipi.u16_dsi_disp_width < pst_snr->st_mipi.u16_dsi_output_width);
    pst_dsi_attr->u08_mem_endian_inv = pst_snr->st_mipi.u08_mem_endian;
    pst_dsi_attr->u16_output_width = pst_snr->st_mipi.u16_dsi_output_width;
    pst_dsi_attr->u16_output_height = pst_snr->st_mipi.u16_dsi_output_height;
    pst_dsi_attr->u08_data_type = pst_snr->st_mipi.u08_data_type;
    pst_dsi_attr->u08_dphy_lane_num = pst_snr->st_mipi.u08_dphy_lane_num;
    pst_dsi_attr->u16_data_rate = pst_snr->st_mipi.u16_data_rate;
    pst_dsi_attr->u08_clock_continue = pst_snr->st_mipi.u08_clock_continue;
    pst_dsi_attr->u08_dlane_o_strong = pst_snr->st_mipi.u08_dlane_o_strong;
    pst_dsi_attr->u08_clane_o_strong = pst_snr->st_mipi.u08_clane_o_strong;

    /* 确认补黑边的位置 */
    if (pst_dsi_attr->u08_padding_en > 0) {
        pst_dsi_attr->u08_padding_left = pst_snr->st_mipi.u08_padding_left;
        pst_dsi_attr->u08_padding_top = pst_snr->st_mipi.u08_padding_top;
    } else {
        pst_dsi_attr->u08_padding_left = 0;
        pst_dsi_attr->u08_padding_top = 0;
    }

    pst_dsi_attr->st_blanking_cfg.u16_vsync_bp = pst_snr->st_mipi.u16_dsi_vsync_bp;
    pst_dsi_attr->st_blanking_cfg.u16_vsync_per = pst_snr->st_mipi.u16_dsi_vsync_per;
    pst_dsi_attr->st_blanking_cfg.u16_vsync_fp= pst_snr->st_mipi.u16_dsi_vsync_fp;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_bp = pst_snr->st_mipi.u16_dsi_hsync_bp;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_per = pst_snr->st_mipi.u16_dsi_hsync_per;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_fp= pst_snr->st_mipi.u16_dsi_hsync_fp;

    ts_tdk_vpo_set_chn_attr(TS_VPO_MIPI_DSI_MDEV, (ts_vpo_chn_attr_s_t *)&st_mipi_dsi);
}

#if 0
ts_void vpo_cfg_bte(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH, ts_u32 yOutEn, ts_u32 rawOutEn, ts_u08 u08_bt_8bit_en, ts_u08 u08_external_sync_en)
{
    vpo_cfg_grp0_no_mirr(TS_VPO_NULL_MDEV, TS_VPO_BTENC_MDEV, TS_VPO_NULL_MDEV, 0, 0);
    vpo_cfg_grp0_bte(imgW,imgH,f32_fps,u08_bt_8bit_en,u08_external_sync_en);

    ts_tdk_vpo_enable_chn(TS_VPO_BTENC_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}

ts_void vpo_cfg_cvbs_bte(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH, ts_u08 u08_bt_8bit_en, ts_u08 u08_external_sync_en,ts_u32 yOutEn,ts_u32 rawOutEn)
{
    vpo_cfg_grp0_no_mirr(TS_VPO_CVBS_MDEV, TS_VPO_BTENC_MDEV, TS_VPO_NULL_MDEV, 0 ,0);
    vpo_cfg_grp0_cvbs(0, 0, 1, 0);
    vpo_cfg_grp0_bte(imgW, imgH, f32_fps, u08_bt_8bit_en, u08_external_sync_en);

    ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);
    ts_tdk_vpo_enable_chn(TS_VPO_BTENC_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}

ts_void vpo_cfg_mipiCsi2(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH,ts_u32 yOutEn,ts_u32 rawOutEn,ts_u08 u08_data_type, ts_u08 u08_dphy_lane_num, ts_u16 u16_data_rate)
{
    vpo_cfg_grp0_no_mirr(TS_VPO_NULL_MDEV, yOutEn ? TS_VPO_MIPI_CSI_MDEV:TS_VPO_NULL_MDEV, rawOutEn ? TS_VPO_MIPI_CSI_MDEV:TS_VPO_NULL_MDEV, 0, 0);
    vpo_cfg_grp0_mipi(imgW,imgH,f32_fps,u08_data_type,u08_dphy_lane_num,u16_data_rate);

    ts_tdk_vpo_enable_chn(TS_VPO_MIPI_CSI_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}

ts_void vpo_cfg_cvbs_mipiCsi2(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH,ts_u32 yOutEn,ts_u32 rawOutEn)
{
    vpo_cfg_grp0_no_mirr(TS_VPO_CVBS_MDEV, TS_VPO_MIPI_CSI_MDEV, TS_VPO_NULL_MDEV, 0 ,0);
    vpo_cfg_grp0_cvbs(0, 0, 1, 0);
    vpo_cfg_grp0_mipi(imgW,imgH,f32_fps,TS_VPO_CSI2_DT_YUV422,4,500);

    ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);
    ts_tdk_vpo_enable_chn(TS_VPO_MIPI_CSI_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}
#endif


#define snr_chk_ch1_mdev(mdev)  TS_CPL_BUG_ON(mdev != TS_VPO_NULL_MDEV)
/** @note 用于测温功能，仅对CVBS生效 */
ts_s32 vot_snr_cfg_vpo(snr_attr_s_t *pst_snr) {

    ts_mdev_t h_vpo_ch0;

    /* 确认输出设备 */
    if (pst_snr->st_cvbs.u08_en > 0) {
        /* 确认短延时合法性, P 制需要 50HZ，N 制需要 60HZ */
        if (pst_snr->st_cvbs.u08_sd_en > 0) {
            if (pst_snr->st_cvbs.u08_pal_en > 0) {
                if (pst_snr->s32_fps != 50) {
                    vision_log_err("pal sd is invalid!\n");

                    pst_snr->st_cvbs.u08_sd_en = 0;
                }
            } else {
                if (pst_snr->s32_fps != 60) {
                    vision_log_err("ntsc sd is invalid!\n");

                    pst_snr->st_cvbs.u08_sd_en = 0;
                }
            }
        }

        h_vpo_ch0 = TS_VPO_CVBS_MDEV;
    } else {
        h_vpo_ch0 = TS_VPO_NULL_MDEV;
    } 
    if (pst_snr->st_cvbs.u08_en > 0)
        vpo_cfg_grp0_cvbs(pst_snr);
        
    /* 开始使能通道 */
    if (pst_snr->st_cvbs.u08_en > 0)
        ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);

    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
    printf("h_vpo_ch0:%d\n", h_vpo_ch0);
    return TS_OK;
}

ts_s32 snr_cfg_vpo(snr_attr_s_t *pst_snr)
{
    snr_attr_s_t *pst_attr;
    ts_mdev_t h_vpo_ch0, h_vpo_ch1, h_vpo_ch2, h_vpo_raw;
    ts_u32 u32_vo_cnt;

    pst_attr = (snr_attr_s_t *)pst_snr;

    /* 确认输出设备 */
    if (pst_snr->st_cvbs.u08_en > 0) {
        /* 确认短延时合法性, P 制需要 50HZ，N 制需要 60HZ */
        if (pst_snr->st_cvbs.u08_sd_en > 0) {
            if (pst_snr->st_cvbs.u08_pal_en > 0) {
            //如果是vot_planck1280c，PAL制是25帧，否则是50帧
               #ifdef CONFIG_SNR_PLANCK1280C
                {
                    if (pst_snr->s32_fps != 25) {
                            vision_log_err("pal sd is invalid!\n");
                            pst_snr->st_cvbs.u08_sd_en = 0;
                        }
                }
                #else
                {
                    if (pst_snr->s32_fps != 50) {
                            vision_log_err("pal sd is invalid!\n");
                            pst_snr->st_cvbs.u08_sd_en = 0;
                        }
                }
                #endif    
               
            } else {
                if (pst_snr->s32_fps != 60) {
                    vision_log_err("ntsc sd is invalid!\n");

                    pst_snr->st_cvbs.u08_sd_en = 0;
                }
            }
        }

        h_vpo_ch0 = TS_VPO_CVBS_MDEV;
    } else {
        h_vpo_ch0 = TS_VPO_NULL_MDEV;
    }

    h_vpo_ch1 = TS_VPO_NULL_MDEV;
    h_vpo_ch2 = TS_VPO_NULL_MDEV;

    /* 是否开启 MEM */
    if (pst_snr->u32_mem_en > 0)
        h_vpo_ch1 = TS_VPO_MEM_MDEV;


    /* 是否开启 MIPI */
    if (pst_snr->st_mipi.u08_en > 0) {
        snr_chk_ch1_mdev(h_vpo_ch1);

        /* CSI or DSI */
        if (0 == pst_snr->st_mipi.u08_dsi_en)
            h_vpo_ch1 = TS_VPO_MIPI_CSI_MDEV;
        else
            h_vpo_ch1 = TS_VPO_MIPI_DSI_MDEV;
    }

    /* 是否开启 BTE */
    if (pst_snr->st_bte.u08_en > 0) {
        if (pst_snr->st_mipi.u08_en > 0) {
            snr_chk_ch1_mdev(h_vpo_ch2);
            h_vpo_ch2 = TS_VPO_BTENC_MDEV;
        } else {
            snr_chk_ch1_mdev(h_vpo_ch1);
            h_vpo_ch1 = TS_VPO_BTENC_MDEV;
        }
    }
   /* 检测是否超过芯片能力 */
    u32_vo_cnt = 0;
    if (pst_snr->u32_mem_en > 0)
        u32_vo_cnt++;

    if (pst_snr->st_mipi.u08_en > 0)
        u32_vo_cnt++;

    if (pst_snr->st_bte.u08_en > 0)
        u32_vo_cnt++;

    if (u32_vo_cnt > 2) {
        vision_log_err("only support 2 chs, but set to 3\n");

        return TS_ERR;
    }

    /* 输出合法性检测 
    cvbs 单独一个通道
    支持以下几种配置 ： bt+mem;csi+mem;mem+dsi;bt+dsi
    */
    if (pst_snr->u32_mem_en > 0) {
        if (pst_snr->st_bte.u08_en > 0) {
            /* bt + mem */
            h_vpo_ch1 = TS_VPO_BTENC_MDEV;
            h_vpo_ch2 = TS_VPO_MEM_MDEV;
        } else if (pst_snr->st_mipi.u08_en > 0) {
            /* CSI or DSI + mem */
            if (0 == pst_snr->st_mipi.u08_dsi_en) {
                h_vpo_ch1 = TS_VPO_MIPI_CSI_MDEV;
                h_vpo_ch2 = TS_VPO_MEM_MDEV;
            } else {
                h_vpo_ch1 = TS_VPO_MIPI_DSI_MDEV;
                h_vpo_ch2 = TS_VPO_MEM_MDEV;
            }
        } else {
            h_vpo_ch1 = TS_VPO_MEM_MDEV;
        }
    } else if (pst_snr->st_mipi.u08_en > 0) {
        /* CSI or DSI */
        if (pst_snr->st_mipi.u08_dsi_en > 0) {
            h_vpo_ch1 = TS_VPO_MIPI_DSI_MDEV;
            if (pst_snr->st_bte.u08_en > 0) {
                /* mipi 与 bt1120 管脚部分冲突 */
                if (0 == pst_snr->st_bte.u08_8bit_en) {
                    vision_log_err("when mipi is enable, only bt656 or bt601 is supported!\n");

                    return TS_ERR;
                }

                h_vpo_ch2 = TS_VPO_BTENC_MDEV;
            }
        } else {
            h_vpo_ch1 = TS_VPO_MIPI_CSI_MDEV;
            if (pst_snr->st_bte.u08_en > 0) {
                vision_log_err("unsupport csi+bt!\n");

                return TS_ERR;
            }
        }
    } else if (pst_snr->st_bte.u08_en > 0) {
        h_vpo_ch1 = TS_VPO_BTENC_MDEV;
    }
    /* 是否开启 raw */
    if (pst_snr->u08_raw_oe > 0)
        h_vpo_raw = h_vpo_ch1;
    else
        h_vpo_raw = TS_VPO_NULL_MDEV;

    /* 最终是否决定输出 y 流 */
    if (0 == pst_snr->u08_y_oe)
        vpo_cfg_grp0(h_vpo_ch0, TS_VPO_NULL_MDEV, TS_VPO_NULL_MDEV, h_vpo_raw, pst_attr);
    else
        vpo_cfg_grp0(h_vpo_ch0, h_vpo_ch1, h_vpo_ch2, h_vpo_raw, pst_attr);
    
    if (pst_snr->st_cvbs.u08_en > 0)
        vpo_cfg_grp0_cvbs(pst_snr);

    switch (h_vpo_ch1) {
        case TS_VPO_MEM_MDEV:
            vpo_cfg_grp0_mem(pst_snr);

            break;

        case TS_VPO_BTENC_MDEV:
            vpo_cfg_grp0_bte(pst_snr);

            break;

        case TS_VPO_MIPI_CSI_MDEV:
            vpo_cfg_grp0_mipi_csi(pst_snr);

            break;

        case TS_VPO_MIPI_DSI_MDEV:
            vpo_cfg_grp0_mipi_dsi(pst_snr);

            break;

        default:
            vision_log_warn("ch1 mdev = %#x\n", h_vpo_ch1);

            break;
    }


    switch (h_vpo_ch2) {
        case TS_VPO_BTENC_MDEV:
            vpo_cfg_grp0_bte(pst_snr);

            break;

        case TS_VPO_MEM_MDEV:
            vpo_cfg_grp0_mem(pst_snr);

            break;

        default:
            vision_log_warn("ch2 mdev = %#x\n", h_vpo_ch2);

            break;
    }

    /* 开始使能通道 */
    if (pst_snr->st_cvbs.u08_en > 0)
        ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);
    
    switch (h_vpo_ch1) {
        case TS_VPO_MEM_MDEV:
            /* mem 由后续流程单独启动 */
            break;

        case TS_VPO_BTENC_MDEV:
            ts_tdk_vpo_enable_chn(TS_VPO_BTENC_MDEV);

            break;

        case TS_VPO_MIPI_CSI_MDEV:
            ts_tdk_vpo_enable_chn(TS_VPO_MIPI_CSI_MDEV);

            break;

        case TS_VPO_MIPI_DSI_MDEV:
            ts_tdk_vpo_enable_chn(TS_VPO_MIPI_DSI_MDEV);
            break;

        default:
            break;
    }

    switch (h_vpo_ch2) {
        case TS_VPO_BTENC_MDEV:
            ts_tdk_vpo_enable_chn(TS_VPO_BTENC_MDEV);

            break;

        default:
            break;
    }

    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);

    /* 更换开机画面显示 */
    if (pst_attr->u32_pod_dly_ms > 0)
        vot_snr_set_grp_pod(TS_VPO_GRP0);

    return TS_OK;    
}


ts_void vpo_cfg_dsi_core(ts_u32 u32_img_w, ts_u32 u32_img_h, ts_f32 f32_img_fps)
{
#if 0
    ts_vpo_chn_attr_s_t st_disp;
    ts_vpo_mipi_dsi_attr_s_t *pst_dsi_attr;
    vpo_cfg_grp0_no_mirr(TS_VPO_NULL_MDEV, TS_VPO_MIPI_DSI_MDEV, TS_VPO_NULL_MDEV);

    /* 另一显示通道配置成 lcd 输出 */
    st_disp.u16_disp_height = u32_img_h;
    st_disp.u16_disp_width = u32_img_w;
    st_disp.f32_disp_fps = f32_img_fps;
    st_disp.u08_pod_en = 0;

    pst_dsi_attr = (ts_vpo_mipi_dsi_attr_s_t *)&st_disp.un_chn_attr.st_mipi_dsi_attr;

    pst_dsi_attr->u08_data_type = TS_VPO_DSI_DT_24BIT;
    pst_dsi_attr->u08_dphy_lane_num = 2;
    pst_dsi_attr->u16_data_rate = 700;
    pst_dsi_attr->u08_clock_continue = 1;
    pst_dsi_attr->st_blanking_cfg.u16_vsync_bp = 15;
    pst_dsi_attr->st_blanking_cfg.u16_vsync_per = 12;
    pst_dsi_attr->st_blanking_cfg.u16_vsync_fp= 12;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_bp = 80;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_per = 12;
    pst_dsi_attr->st_blanking_cfg.u16_hsync_fp = 265;

    ts_tdk_vpo_set_chn_attr(TS_VPO_MIPI_DSI_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp);

    ts_tdk_vpo_enable_chn(TS_VPO_MIPI_DSI_MDEV);
    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
#endif
}



ts_void vpo_cfg_lcd_d0601280pfd40(ts_u32 fps)
{
    ts_vpo_grp_attr_s_t st_grp_atr;
    ts_vpo_chn_attr_s_t st_disp_lcd;
    ts_vpo_lcd_attr_s_t *pst_lcd_attr;

    /* 对原图不进行剪切 */
    st_grp_atr.u16_cut_width = 0;
    st_grp_atr.u16_cut_height = 0;
    st_grp_atr.u16_cut_start_v = 0;
    st_grp_atr.u16_cut_start_h = 0;

    st_grp_atr.u08_hmirr_en = 0;
    st_grp_atr.u08_vmirr_en = 0;
    st_grp_atr.u08_bank_num = 3;

    st_grp_atr.u08_head_en = 0;
    st_grp_atr.u08_fire_en = 0;
    st_grp_atr.u16_info_size = 0;
    st_grp_atr.u32_info_addr = 0;

    st_grp_atr.u08_ch0_filter_mode = 1;
    st_grp_atr.u08_ch1_filter_mode = 1;

    st_grp_atr.u32_yuv_out_ch0_sel = TS_VPO_NULL_MDEV;
    st_grp_atr.u32_yuv_out_ch1_sel = TS_VPO_LCD_MDEV;
    st_grp_atr.u32_yuv_out_ch2_sel = TS_VPO_NULL_MDEV;
    st_grp_atr.u32_raw_out_ch_sel = TS_VPO_NULL_MDEV;
    st_grp_atr.u08_digital_ch_yuv2y_en = 0;

    st_grp_atr.pv_tmm_zone = gpv_tmm_vpo;
    ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, (ts_vpo_grp_attr_s_t *)&st_grp_atr);

    /* 另一显示通道配置成 lcd 输出 */
    st_disp_lcd.u16_disp_height = 1024;
    st_disp_lcd.u16_disp_width = 1280;
    st_disp_lcd.f32_disp_fps = fps;
    st_disp_lcd.u08_pod_en = 0;

    pst_lcd_attr = (ts_vpo_lcd_attr_s_t *)&st_disp_lcd.un_chn_attr.st_lcd_attr;

    pst_lcd_attr->u08_data_type = TS_VPO_LCD_DT_RGB888;
    pst_lcd_attr->u08_test_bar = 0;
    pst_lcd_attr->u08_valid_en = 1;
    pst_lcd_attr->st_sync_cfg.vsync_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.hsync_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.valid_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_bp = 5;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_per = 2;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_bp = 16;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_per = 1;

    ts_tdk_vpo_set_chn_attr(TS_VPO_LCD_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_lcd);
    ts_tdk_vpo_enable_chn(TS_VPO_LCD_MDEV);

    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}

ts_void vpo_cfg_cvbs_lcd_gz039pbc05(ts_u32 fps)
{
    ts_vpo_grp_attr_s_t st_grp_atr;
    ts_vpo_chn_attr_s_t st_disp_lcd;
    ts_vpo_chn_attr_s_t st_disp_cvbs;
    ts_vpo_lcd_attr_s_t *pst_lcd_attr;
    ts_vpo_cvbs_attr_s_t *pst_cvbs_attr;

    /* 对原图不进行剪切 */
    st_grp_atr.u16_cut_width = 0;
    st_grp_atr.u16_cut_height = 0;
    st_grp_atr.u16_cut_start_v = 0;
    st_grp_atr.u16_cut_start_h = 0;

    st_grp_atr.u08_hmirr_en = 0;
    st_grp_atr.u08_vmirr_en = 0;
    st_grp_atr.u08_bank_num = 3;

    st_grp_atr.u08_head_en = 0;
    st_grp_atr.u08_fire_en = 0;
    st_grp_atr.u16_info_size = 0;
    st_grp_atr.u32_info_addr = 0;

    st_grp_atr.u08_ch0_filter_mode = 1;
    st_grp_atr.u08_ch1_filter_mode = 0;

    st_grp_atr.u32_yuv_out_ch0_sel = TS_VPO_CVBS_MDEV;
    st_grp_atr.u32_yuv_out_ch1_sel = TS_VPO_LCD_MDEV;
    st_grp_atr.u32_yuv_out_ch2_sel = TS_VPO_NULL_MDEV;
    st_grp_atr.u32_raw_out_ch_sel = TS_VPO_NULL_MDEV;
    st_grp_atr.u08_digital_ch_yuv2y_en = 0;

    st_grp_atr.pv_tmm_zone = gpv_tmm_vpo;
    ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, (ts_vpo_grp_attr_s_t *)&st_grp_atr);

    /* CVBS 只支持 PAL 和 NTSC ，只需要配置高度， 宽度和帧率无需设置 */
    st_disp_cvbs.u16_disp_height = 576;
    st_disp_cvbs.u16_disp_width = 720;
    st_disp_cvbs.f32_disp_fps = 0;
    st_disp_cvbs.u08_pod_en = 0;

    // if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    // {
    //     st_disp_cvbs.u16_disp_width = 1280;
    //     st_disp_cvbs.u16_disp_height = 1024;
    // }
    pst_cvbs_attr = (ts_vpo_cvbs_attr_s_t *)&st_disp_cvbs.un_chn_attr.st_cvbs_attr;
    pst_cvbs_attr->u08_test_bar = 0;
    pst_cvbs_attr->u08_dac_drv_strength = 13;
    pst_cvbs_attr->u08_bank_num = 3;
    pst_cvbs_attr->u08_short_delay_en = 0;

    ts_tdk_vpo_set_chn_attr(TS_VPO_CVBS_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_cvbs);


    /* 另一显示通道配置成 lcd 输出 */
    st_disp_lcd.u16_disp_height = 768;
    st_disp_lcd.u16_disp_width = 1024;
    st_disp_lcd.f32_disp_fps = fps;
    st_disp_lcd.u08_pod_en = 0;
    
    // if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    // {
    //     st_disp_lcd.u16_disp_width = 1280;
    //     st_disp_lcd.u16_disp_height = 1024;
    //     st_disp_lcd.f32_disp_fps = 25;
    // }

    pst_lcd_attr = (ts_vpo_lcd_attr_s_t *)&st_disp_lcd.un_chn_attr.st_lcd_attr;

    pst_lcd_attr->u08_data_type = TS_VPO_LCD_DT_RGB888;
    pst_lcd_attr->u08_test_bar = 0;
    pst_lcd_attr->u08_valid_en = 1;
    pst_lcd_attr->st_sync_cfg.vsync_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.hsync_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.valid_en_def_lvl = TS_ELEC_LVL_LO;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_bp = 5;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_vsync_per = 2;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_bp = 16;
    pst_lcd_attr->st_sync_cfg.st_blanking_cfg.u16_hsync_per = 1;

    ts_tdk_vpo_set_chn_attr(TS_VPO_LCD_MDEV, (ts_vpo_chn_attr_s_t *)&st_disp_lcd);
    ts_tdk_vpo_enable_chn(TS_VPO_LCD_MDEV);
    ts_tdk_vpo_enable_chn(TS_VPO_CVBS_MDEV);

    ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}


ts_void isp_demo(ts_void)
{
    ts_isp_comm_attr_s_t st_comm;

    st_comm.b_module_en = 0;
    st_comm.u32_start_offset = 20;

    ts_tdk_isp_set_comm_attr(TS_ISP_MDEV0, (const ts_isp_comm_attr_s_t *)&st_comm);
}


ts_void snr_isp_init(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_fps, ts_void *pv_isp_cfg)
{
    ts_isp_comm_attr_s_t st_comm;

    st_comm.b_module_en = 1;
    st_comm.u32_start_offset = 20;
    ts_s32 s32_ret = ts_tdk_isp_set_comm_attr(TS_ISP_MDEV0, (const ts_isp_comm_attr_s_t *)&st_comm);
    if (s32_ret != TS_OK)
        TS_CPL_ASSERT(0);

    ts_isp_create_s_t st_create;
    st_create.pv_cfg_addr = pv_isp_cfg;
    st_create.st_img_size.u32_width = s32_img_w;
    st_create.st_img_size.u32_height = s32_img_h;
    st_create.u32_fps = s32_fps;
    s32_ret = ts_tdk_isp_init(TS_ISP_MDEV0, &st_create); 
    if (s32_ret != TS_OK)
        TS_CPL_ASSERT(0);
}

ts_s32 snr_gpio_cfg2out(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_def)
{
    ts_gpio_attr_s_t st_attr;

    /* 无需程序控制，直接返回成功 */
    if (-1 == s32_gpio_num)
        return TS_OK;

    st_attr.u32_pin_no = (ts_u32)s32_gpio_num;
    st_attr.en_dir = TS_GPIO_DIR_OUTPUT;
    if (TS_ELEC_LVL_LO == en_o_def)
        st_attr.en_lvl = TS_ELEC_LVL_LO;
    else
        st_attr.en_lvl = TS_ELEC_LVL_HI;

    return ts_lib_gpio_set_attr((ts_gpio_attr_s_t *)&st_attr);
}

ts_void snr_gpio_set_out_lvl(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_lvl)
{
    ts_elec_level_en_t en_lvl;

    /* 无需程序控制，直接返回成功 */
    if (-1 == s32_gpio_num)
        return;

    if (TS_ELEC_LVL_LO == en_o_lvl)
        en_lvl = TS_ELEC_LVL_LO;
    else
        en_lvl = TS_ELEC_LVL_HI;
    
    ts_lib_gpio_write_pin((ts_u32)s32_gpio_num, en_lvl);
}


ts_void tdk_snrcfg_init(tdk_sensor_cfg_s_t *pst_cfg)
{
    tdk_sensor_cfg_s_t *pst_ctx_cfg;

    if (ts_null == pst_cfg) {
        vision_log_debug("pst_cfg is ts_null\n");

        return;
    }

    pst_ctx_cfg = &gst_snr_ctx.st_io_cfg;
    ts_cpl_memcpy((ts_void *)pst_ctx_cfg, (ts_void *)pst_cfg, sizeof(tdk_sensor_cfg_s_t));

    /* 按需对 GPIO 做复用关系配置 */

    /* 管脚配置成输出 */
    snr_gpio_cfg2out(pst_ctx_cfg->s32_pwr_gpio_num, pst_ctx_cfg->en_pwr_off_lvl);
    snr_gpio_cfg2out(pst_ctx_cfg->s32_rst_gpio_num, pst_ctx_cfg->en_reset_lvl);
    snr_gpio_cfg2out(pst_ctx_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_LO);
    snr_gpio_cfg2out(pst_ctx_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_LO);
    if ((-1 == pst_ctx_cfg->s32_sht_p_gpio_num) && (-1 == pst_ctx_cfg->s32_sht_n_gpio_num)) {
        snr_gpio_cfg2out(pst_ctx_cfg->s32_sht_en_gpio_num, TS_ELEC_LVL_LO);
        snr_gpio_cfg2out(pst_ctx_cfg->s32_sht_ctrl_gpio_num, TS_ELEC_LVL_LO);
    }
}


ts_void tdk_snrcfg_reset(ts_void)
{
    ts_elec_level_en_t en_release_lvl;
    tdk_sensor_cfg_s_t *pst_cfg;

    pst_cfg = &gst_snr_ctx.st_io_cfg;

    if ((ts_null == pst_cfg) || (0 == pst_cfg->u32_rst_dly) || (-1 == pst_cfg->s32_rst_gpio_num)) {
        vision_log_debug("input param invalid\n");

        return;
    }

    if (TS_ELEC_LVL_LO == pst_cfg->en_reset_lvl)
        en_release_lvl = TS_ELEC_LVL_HI;
    else
        en_release_lvl = TS_ELEC_LVL_LO;

    snr_gpio_set_out_lvl(pst_cfg->s32_rst_gpio_num, pst_cfg->en_reset_lvl);
    ts_cpl_udelay(pst_cfg->u32_rst_dly);
    snr_gpio_set_out_lvl(pst_cfg->s32_rst_gpio_num, en_release_lvl);
}


static ts_void snrcfg_shutter_sw(tdk_sensor_cfg_s_t *pst_cfg, ts_u32 u32_open)
{
    //if ((ts_null == pst_cfg) || (-1 == pst_cfg->s32_sht_n_gpio_num) || (-1 == pst_cfg->s32_sht_p_gpio_num)) {
    if ((ts_null == pst_cfg) || 
        (((-1 == pst_cfg->s32_sht_n_gpio_num) && (-1 == pst_cfg->s32_sht_p_gpio_num)) && 
         ((-1 == pst_cfg->s32_sht_en_gpio_num) || (-1 == pst_cfg->s32_sht_ctrl_gpio_num) || ((TS_ELEC_LVL_HI != pst_cfg->en_sht_open_lvl) && (TS_ELEC_LVL_LO != pst_cfg->en_sht_open_lvl))))
        ) {
        vision_log_debug("snrcfg_reset input param invalid\n");

        return;
    }

    if((-1 == pst_cfg->s32_sht_n_gpio_num) && (-1 == pst_cfg->s32_sht_p_gpio_num)) {
        /* shutter use en and open/close pin */
        snr_gpio_set_out_lvl(pst_cfg->s32_sht_en_gpio_num, TS_ELEC_LVL_LO);

        if (u32_open > 0)
            snr_gpio_set_out_lvl(pst_cfg->s32_sht_ctrl_gpio_num, pst_cfg->en_sht_open_lvl);
        else
            snr_gpio_set_out_lvl(pst_cfg->s32_sht_ctrl_gpio_num, (~ pst_cfg->en_sht_open_lvl) & 0x1);

        snr_gpio_set_out_lvl(pst_cfg->s32_sht_en_gpio_num, TS_ELEC_LVL_HI);

        ts_cpl_fix_msleep(pst_cfg->u32_sht_hold / 1000);

        snr_gpio_set_out_lvl(pst_cfg->s32_sht_en_gpio_num, TS_ELEC_LVL_LO);
    } else if ((-1 == pst_cfg->s32_sht_n_gpio_num) || (-1 == pst_cfg->s32_sht_p_gpio_num)) {
        /* shutter use 1 pin ctrl */
        if((-1 == pst_cfg->s32_sht_n_gpio_num)){
            if (u32_open > 0)
                snr_gpio_set_out_lvl(pst_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_HI);
            else
                snr_gpio_set_out_lvl(pst_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_LO);
        } else {
            if (u32_open > 0)
                snr_gpio_set_out_lvl(pst_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_LO);
            else
                snr_gpio_set_out_lvl(pst_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_HI);
        }
        ts_cpl_fix_msleep(pst_cfg->u32_sht_hold / 1000);
    } else if ((-1 != pst_cfg->s32_sht_n_gpio_num) && (-1 != pst_cfg->s32_sht_p_gpio_num)){
        /* shutter use 2 pin ctrl */
        snr_gpio_set_out_lvl(pst_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_LO);
        snr_gpio_set_out_lvl(pst_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_LO);

        if (u32_open > 0)
            snr_gpio_set_out_lvl(pst_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_HI);
        else
            snr_gpio_set_out_lvl(pst_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_HI);

        ts_cpl_fix_msleep(pst_cfg->u32_sht_hold / 1000);

        snr_gpio_set_out_lvl(pst_cfg->s32_sht_p_gpio_num, TS_ELEC_LVL_LO);
        snr_gpio_set_out_lvl(pst_cfg->s32_sht_n_gpio_num, TS_ELEC_LVL_LO);
    }

    if (pst_cfg->u32_sht_dbg > 0) {
        if (u32_open > 0)
            vision_log_debug("Open Shutter...\n");
        else
            vision_log_debug("Close Shutter...\n");
    }
}
/** @note 测温版-开挡片 */
ts_void vot_templut_shutter_open(ts_void) {
    if (SystemParaMenu2.m_temperCalibra_Flag || SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
        vision_log_debug("shutter_open: blocked, calibration active\n");
        return;
    }
    /* 打开挡片 */
    tdk_sensor_cfg_s_t *pst_cfg = &gst_snr_ctx.st_io_cfg;
    snrcfg_shutter_sw(pst_cfg, 1);
    global_sys_ctrl_info.Iris_Current_State = 1;
}
/** @note 观测版-开挡片 */
ts_void tdk_snrcfg_shutter_open(ts_void)
{
    tdk_sensor_cfg_s_t *pst_cfg = &gst_snr_ctx.st_io_cfg;
    snrcfg_shutter_sw(pst_cfg, 1);
}
/** @note 测温版-关挡片 */
ts_void vot_templut_shutter_close(ts_void) {
    snr_attr_s_t *pst_attr;
    tdk_sensor_cfg_s_t *pst_cfg;
    ts_u32 u32_frame_ms;

    if (SystemParaMenu2.m_temperCalibra_Flag || SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
        // 进入标定状态
        vision_log_debug("shutter_close: blocked, calibration active\n");
        return;
    }
    // 非标定状态
    pst_cfg = &gst_snr_ctx.st_io_cfg;
    snrcfg_shutter_sw(pst_cfg, 0);  /* 0: 关闭挡片 */
    /* 等待传感器稳定：延时 3 帧 */
    pst_attr = &gst_snr_ctx.st_attr;
    if (pst_attr->s32_fps <= 0) {
        vision_log_debug("shutter_close: invalid fps=%d, use default 25\n", pst_attr->s32_fps);
        u32_frame_ms = 1000 / 25;
    } else {
        u32_frame_ms = 1000 / (ts_u32)(pst_attr->s32_fps);
    }
    ts_cpl_fix_msleep(3 * u32_frame_ms);
    global_sys_ctrl_info.Iris_Current_State = 0;
}
/** @note 观测版-关挡片 */
ts_void tdk_snrcfg_shutter_close(ts_void) {
    tdk_sensor_cfg_s_t *pst_cfg = &gst_snr_ctx.st_io_cfg;
    snr_attr_s_t *pst_attr = &gst_snr_ctx.st_attr;
    ts_u32 u32_frame_ms;

    snrcfg_shutter_sw(pst_cfg, 0);
    if (pst_attr->s32_fps <= 0) {
        vision_log_debug("shutter_close: invalid fps=%d, fallback to 25\n", pst_attr->s32_fps);
        u32_frame_ms = 40;  /* 1000/25 */
    } else {
        u32_frame_ms = (ts_u32)(1000 / pst_attr->s32_fps);
    }
    /* 等待 3 帧，确保传感器输出稳定 */
    ts_cpl_fix_msleep(3 * u32_frame_ms);
}

ts_void tdk_snrcfg_pwr_on(ts_void)
{
    tdk_sensor_cfg_s_t *pst_cfg;

    pst_cfg = &gst_snr_ctx.st_io_cfg;
    if (-1 == pst_cfg->s32_pwr_gpio_num)
        return;
    
    if (TS_ELEC_LVL_HI == pst_cfg->en_pwr_off_lvl)
        snr_gpio_set_out_lvl(pst_cfg->s32_pwr_gpio_num, TS_ELEC_LVL_LO);
    else
        snr_gpio_set_out_lvl(pst_cfg->s32_pwr_gpio_num, TS_ELEC_LVL_HI);
}

ts_void tdk_snrcfg_pwr_off(ts_void)
{
    tdk_sensor_cfg_s_t *pst_cfg;

    pst_cfg = &gst_snr_ctx.st_io_cfg;
    if (-1 == pst_cfg->s32_pwr_gpio_num)
        return;

    snr_gpio_set_out_lvl(pst_cfg->s32_pwr_gpio_num, pst_cfg->en_pwr_off_lvl);
}

ts_void snr_insmod_all_tdk(ts_u32 u32_param)
{
    TS_UNUSED(u32_param);

    /* insmod tdk ko */
    ts_cpl_insmod(viu, ts_null);
    ts_cpl_insmod(isp, ts_null);
    ts_cpl_insmod(vpo, ts_null);
    ts_cpl_insmod(osd, ts_null);

    /* insmod other ko */
    ts_cpl_insmod(pwm, ts_null);
    ts_cpl_insmod(i2c, ts_null);

    ts_cpl_insmod(usb, ts_null);

    if (u32_param > 0) {
        ts_cpl_insmod(aisp, ts_null);
    }

    tdl_isp_upload_init();
}


static ts_void isp_pthread(ts_void) 
{
    ts_cpl_timeval_s_t st_start, st_end;
    ts_s32 s32_ret;
    ts_u32 u32_frm_cnt, u32_dif;

    u32_frm_cnt = 0;
    while (1) {
        /* 高优先级线程，保证可靠喂狗 */
        // ts_lib_wdt_feeding();

        if ((gst_snr_ctx.s32_run_isp > 0) && (!gst_snr_ctx.s32_run_pause)) {
            ts_cpl_gettimeofday(&st_start);
            s32_ret = ts_tdk_isp_process(TS_ISP_MDEV0);
            ts_cpl_gettimeofday(&st_end);
            if (s32_ret != TS_OK) {
                u32_dif = ts_cpl_get_timedif((ts_cpl_timeval_s_t *)&st_start, (ts_cpl_timeval_s_t *)&st_end);
                printf("IspProc return=%d, frm_cn =%d, cost time=%d us\n", s32_ret, u32_frm_cnt, u32_dif);
            } else {
                u32_frm_cnt++;
            }
        } else {
            ts_cpl_msleep(100);
        }
    }
}


ts_void  TS_WEAK usb_usr_init(ts_void *pv_h_tmm)
{

}


ts_void  TS_WEAK usbd_shutdown(ts_void)
{

}


snr_attr_s_t * snr_get_attr(ts_void)
{
    return (snr_attr_s_t *)&gst_snr_ctx.st_attr;
}


extern ts_u32 __npu_cfg_start;
extern ts_u32 __npu_cfg_end;
static ts_u32 snr_npu_load_model(snr_npu_attr_s_t *pst_npu_cfg)
{
    ts_void *pv_tmm;
    ts_u32 u32_model_addr;
    ts_s32 s32_hfile_r;
    ts_char *pc_model_fn;
    ts_u32 u32_cfg_addr, u32_cfg_size;
    ts_u32 *pu32_ver;

    pv_tmm = (ts_void *)ts_cpl_tmm_malloc(ts_null, (TS_NPU_MODEL_SIZE + 256), TS_HW_ALIGN_SIZE, "npu_model"); 
    if (ts_null == pv_tmm) {
        vision_log_err("malloc sr model err\n");

        return 0;
    }

    /* 对齐 */
    u32_model_addr = (ts_u32)pv_tmm;
    u32_model_addr = (u32_model_addr + 127) / 128 * 128;
    if (TS_NPU_OPS_SR == pst_npu_cfg->en_ops)
        pc_model_fn = FN_SR_CFG;
    else
        pc_model_fn = FN_AISP_CFG;

    s32_hfile_r = ts_lib_sfc_open(pc_model_fn);
    if (s32_hfile_r < 0) {
        vision_log_err("open %s fail!load def\n", pc_model_fn);
    } else {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)u32_model_addr, TS_NPU_MODEL_SIZE);
        pu32_ver = (ts_u32 *)u32_model_addr;
        if (0x20241121 == pu32_ver[0]) {
            vision_log_debug("load npu model from %s\n", pc_model_fn);

            return u32_model_addr;
        }
    }

    u32_cfg_addr = (ts_u32)&__npu_cfg_start;
    u32_cfg_size = (ts_u32)&__npu_cfg_end - (ts_u32)&__npu_cfg_start;
    if (u32_cfg_size > 0) {
        ts_cpl_memcpy((ts_void *)u32_model_addr, (ts_void *)u32_cfg_addr, u32_cfg_size);

        vision_log_debug("load npu model from uimg\n");

        return u32_model_addr;
    } else {
        ts_cpl_tmm_free(pv_tmm);

        return 0;
    }
}


static ts_void snr_start_npu(snr_attr_s_t *pst_attr)
{
    ts_u32 u32_model_addr, u32_dly_ms;
    ts_aisp_attr_s_t st_npu_attr;
    snr_npu_attr_s_t *pst_npu_cfg;

    pst_npu_cfg = (snr_npu_attr_s_t *)&pst_attr->st_npu;

    if ((TS_NPU_OPS_DISABLE == pst_npu_cfg->en_ops) || (pst_npu_cfg->en_ops >= TS_NPU_OPS_BUTT))
        return;

    ts_cpl_memset32((ts_void *)&st_npu_attr, 0, (sizeof(ts_aisp_attr_s_t) >> 2));

    /* 需要开启 npu 加载相应模型 */
    if (pst_npu_cfg->u32_clk > 0) {
        u32_model_addr = snr_npu_load_model(pst_npu_cfg);
    } else {
        u32_model_addr = 0;
    }

    st_npu_attr.u32_npu_clk = pst_npu_cfg->u32_clk;
    st_npu_attr.pv_model = (ts_void *)u32_model_addr;
    st_npu_attr.u32_model_size = TS_NPU_MODEL_SIZE;
    st_npu_attr.u16_task_num = 8;
    st_npu_attr.u16_task_node_num = 3;

    st_npu_attr.en_ops = pst_npu_cfg->en_ops;

    st_npu_attr.u08_rotate_dir = (ts_u08)pst_npu_cfg->u08_rotate_dir;

    st_npu_attr.u08_roi_en = pst_npu_cfg->u08_roi_en;
    st_npu_attr.u16_roi_left = pst_npu_cfg->u16_roi_left;
    st_npu_attr.u16_roi_top = pst_npu_cfg->u16_roi_top;
    st_npu_attr.u08_pad_lines = pst_npu_cfg->u08_pad_lines;

    /* 宽高一半 */
    st_npu_attr.u16_roi_width = pst_attr->s32_img_w >> 1;
    st_npu_attr.u16_roi_height = pst_attr->s32_img_h >> 1;

    /* 默认开启 */
    st_npu_attr.u08_run_en = 1;

    /* copy mem type */
    //st_npu_attr.u08_mem_type = pst_npu_cfg->u08_mem_type;

    /* 等待硬件稳定输出流, 如果开启 pod 可设置成 0 */
    u32_dly_ms = 500;

    /* 默认关闭坏斑 */
    st_npu_attr.u08_extbp_en = 0;

#ifdef CONFIG_EXTBP_EN
    #ifdef CONFIG_AISP_EN
        st_npu_attr.u08_extbp_en = 1;
    #endif
#endif
    global_sys_ctrl_info.m_npu_aisp_flag = TS_ERR;
    // 失败则关闭npu超分,A6开启640->960超分，会一直报错Permission denied:illegal TDK for AISP，导致系统无法启动，此时需要关闭超分，系统自动重启
    global_sys_ctrl_info.m_npu_aisp_flag = ts_tdk_aisp_create(2, 2048, (ts_aisp_attr_s_t *)&st_npu_attr, u32_dly_ms);
}


ts_s32 snr_exec_sh(ts_void)
{
    ts_char *pc_cmd;
    tdk_cfg_s_t *pst_mmap;
    ts_u32 u32_offset, u32_max;

    pst_mmap = gst_snr_ctx.pst_tdk_map;
    if ((ts_null == pst_mmap) || (pst_mmap->u32_magic != TS_TDK_FILE_MAGIC)) {
        vision_log_notice("%s\n", TS_ENONEXIST_STR);

        return TS_ERR;
    }

    if (0 == pst_mmap->u08_sh_en) {
        vision_log_notice("No script available!\n");

        return TS_OK;
    }

    pc_cmd = (ts_char *)&pst_mmap->ac_sh;
    u32_offset = 0;
    u32_max = 250;
    while (1) {
        if ('\0' == pc_cmd[0]) {
            break;
        }

        u32_offset = ts_cpl_strnlen(pc_cmd, u32_max);
        if (0 == u32_offset) {
            break;
        }

        vision_log_debug("run:%s\n", pc_cmd);

        cli_simple_run_command(pc_cmd);

        u32_max -= (u32_offset + 1);
        pc_cmd += (u32_offset + 1);
        
    }

    vision_log_debug("sh done!\n");

    return TS_OK;
}

#if 0
static ts_void snr_load_sys_cfg(ts_void)
{
    ts_s32 s32_scfg_hfile, s32_ret;
    ts_scfg_s_t *pst_scfg;

    vision_log_debug("scfg sz=%d\n", sizeof(ts_scfg_s_t));

    s32_scfg_hfile = ts_lib_sfc_open(FN_SUPER_CFG);
    if (s32_scfg_hfile < 0) {
        s32_ret = -1;
        pst_scfg = (ts_scfg_s_t *)ts_cpl_tmm_malloc(gpv_tmm_usr, sizeof(ts_scfg_s_t), 32, "scfg");
        if (ts_null == pst_scfg) {
            vision_log_err("%s\n", TS_ENOMEM_STR);
    
            gst_snr_ctx.pst_scfg = ts_null;

            return;
        }
    } else {
        pst_scfg = (ts_scfg_s_t *)ts_lib_sfc_mmap(s32_scfg_hfile, 0, sizeof(ts_scfg_s_t));
        if (TS_SCFG_MAGIC == pst_scfg->u32_magic) {
            /* 完整性由 super 保证 */
            s32_ret = 0;
        } else {
            s32_ret = -1;
        }
    }

    gst_snr_ctx.pst_scfg = pst_scfg;
    if (-1 == s32_ret) {
        ts_cpl_memset32((ts_void *)pst_scfg, 0, sizeof(ts_scfg_s_t) >> 2);

        /* smart */
        pst_scfg->u32_magic = TS_SCFG_MAGIC;
        pst_scfg->u32_smart_size = 0;

        /* vpo, cvbs+uvc */
        pst_scfg->u32_cfg_num = 0;
        pst_scfg->u32_usb_type = TS_SCFG_INIT_USB_LIBUSB;

        /* 挡片默认 */
        pst_scfg->s32_max_gap = 2 * 60 * 1000;
        pst_scfg->s32_cut_max_temp_diff = 1500;
        pst_scfg->s32_max_gap_en = 1;
        pst_scfg->s32_cut_max_temp_diff_en = 1;

        /* bt 配置默认 */
        pst_scfg->u08_bt_clk_cfg = 1;
        pst_scfg->u08_bt_data_cfg = 0;

        ts_lib_sfc_sync();
    }

    if ((0 == pst_scfg->s32_max_gap) || (pst_scfg->s32_max_gap > (35 * 60 * 1000)))
        pst_scfg->s32_max_gap = 2 * 60 * 1000;

    if ((0 == pst_scfg->s32_cut_max_temp_diff) || (pst_scfg->s32_cut_max_temp_diff > (20 * 1000)))
        pst_scfg->s32_cut_max_temp_diff = 1500;

    vision_log_debug("time gap=%d,temp gap=%d\n", pst_scfg->s32_max_gap, pst_scfg->s32_cut_max_temp_diff);
}
#endif


ts_s32 ts_tdk_change_ooc_idx(ts_u08 u08_idx)
{
    ts_buffer_s_t st_occ_buf;
    sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_hfile_r;
    ts_u32 u32_size;

    s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),sizeof(sfc_cfg_file_s_t));
        if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & (1< u08_idx))) {
            ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);
            u32_size = st_occ_buf.u32_width * st_occ_buf.u32_height;

            ts_u32 u32_offset = sizeof(sfc_cfg_file_s_t) + st_occ_buf.sz_size * u08_idx;
            if (u32_offset + u32_size == FN_OCC_FILE_SIZE) {
                ts_lib_sfc_read(s32_hfile_r, u32_offset, (ts_u08*)st_occ_buf.pu08_ptr, st_occ_buf.sz_size);
                vision_log_debug("use saved occ value for idx %d . \n", u08_idx);
            } else {
                vision_log_debug("idx %d out of file range\n", u08_idx);
            }
        }
        ts_lib_sfc_close(s32_hfile_r);
    }

    return TS_OK;
}

#ifdef CONFIG_SL_EN
static ts_s32 snr_change_ooc_idx(ts_u08 u08_idx)
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
#endif

extern ts_u32 __snr_start;
extern ts_u32 __snr_end;
static ts_u32 su32_start_cnt = 0;
extern ts_void snr_init_ext_bp(ts_void);
ts_s32 start_isp(ts_char *pc_name, ts_u32 u32_usb_en, ts_u32 u32_auto_cut, ts_u32 u32_isp_en)
{
    ts_u32 i, u32_found;
    ts_u32 u32_snr_addr, u32_snr_cnt;
    snr_attr_s_t *pst_snr_attr;
    snr_usr_s_t *pst_snr_usr;
    ts_cpl_task_s_t st_task;
    ts_s32 s32_scfg_hfile;

    gst_snr_ctx.s32_sffc_alg = 0;
    if (su32_start_cnt > 0) {
        vision_log_err("%s:snr restart\n", TS_EACCES_STR);

        return TS_ERR;
    }

    if (ts_null == pc_name) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return TS_ERR;
    }

    u32_snr_addr = (ts_u32)&__snr_start;
    u32_snr_cnt = (ts_u32)&__snr_end - (ts_u32)&__snr_start;
    u32_snr_cnt = u32_snr_cnt / sizeof(snr_usr_s_t);
    if (0 == u32_snr_cnt) {
        vision_log_err("%s:snr cnt=0\n", TS_EINVAL_STR);

        return TS_ERR;
    }

    pst_snr_usr = (snr_usr_s_t *)u32_snr_addr;
    u32_found = 0;
    for (i = 0; i < u32_snr_cnt; i++) {
        if (0 == strcmp(pc_name, pst_snr_usr->pc_name)) {
            u32_found = 1;

            break;
        }

        pst_snr_usr++;
    }

    if (0 == u32_found) {
        vision_log_err("%s:%s\n", TS_ENODEV_STR, pc_name);

        return -TS_ENODEV;
    }

    pst_snr_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    pst_snr_attr->pc_name = pst_snr_usr->pc_name;
#if 0
    pst_snr_attr->u32_ooc_rst_val = 0x80;
    pst_snr_attr->cut_process = snr_auto_cut_process;
    pst_snr_attr->pv_cut_pdata = ts_null;

    /* 闭环与应用解耦 */
    pst_snr_attr->st_cut_ctx.u32_target_mean = 8192;
    pst_snr_attr->st_cut_ctx.pfn_sht_close = tdk_snrcfg_shutter_close;
    pst_snr_attr->st_cut_ctx.pfn_sht_open = tdk_snrcfg_shutter_open;
    pst_snr_attr->st_cut_ctx.pfn_wait_pod = ts_tdk_snr_wait_pod_done;
    pst_snr_attr->st_cut_ctx.pfn_get_force = snr_get_cut_force;
    pst_snr_attr->st_cut_ctx.pfn_get_auto = ts_tdk_snr_get_cut_auto;
    pst_snr_attr->st_cut_ctx.pfn_get_temp_auto = ts_tdk_snr_get_cut_temp_auto;
    pst_snr_attr->st_cut_ctx.pfn_i2c_ra08d08_setreg = snr_i2c_setreg;
    pst_snr_attr->st_cut_ctx.pfn_i2c_ra08d08_getreg = snr_i2c_getreg;

    pst_snr_attr->st_cut_ctx.u32_fast_cnt = 6;
    pst_snr_attr->st_cut_ctx.u32_fast_time_delta = 30 * 1000;

    /* 默认使能温度打挡控制，温度变化设置成 1 度 */
    pst_snr_attr->st_cut_ctx.u32_temp_en = 1;
    pst_snr_attr->st_cut_ctx.s32_temp_delta = 1000;
    pst_snr_attr->st_cut_ctx.s32_temp_fix = 0;

    /* 默认使能时间打挡控制，时间变化设置成 3 分钟 */
    pst_snr_attr->st_cut_ctx.u32_time_en = 1;
    pst_snr_attr->st_cut_ctx.u32_time_delta = 3 * 60 * 1000;

    /* 场景判断 */
    pst_snr_attr->st_cut_ctx.u32_mean_en = 1;
    pst_snr_attr->st_cut_ctx.u32_mean_delta = 2000;

    snr_load_sys_cfg();
#endif
    vision_log_notice("start snr %s\n", pc_name);

    /* 使用 mmap 方式加载 sffs 中的 tdk 配置文件，后续可以通过内存访问方式进行读写，最终使用 ts_lib_sfc_sync 同步到 flash */
    s32_scfg_hfile = ts_lib_sfc_open(FN_SUPER_TDK);
    if (s32_scfg_hfile > 0) {
        gst_snr_ctx.pst_tdk_map = (tdk_cfg_s_t *)ts_lib_sfc_mmap(s32_scfg_hfile, 0, sizeof(tdk_cfg_s_t));
        if (gst_snr_ctx.pst_tdk_map->u32_magic != TS_TDK_FILE_MAGIC) {
            ts_cpl_memset32((ts_void *)gst_snr_ctx.pst_tdk_map, 0, (sizeof(tdk_cfg_s_t) >> 2));

            gst_snr_ctx.pst_tdk_map->u32_dbg_lvl =  TS_LOG_WARNING;
        }

        vision_log_debug("tdk map = 0x%p, sz=%d\n", gst_snr_ctx.pst_tdk_map, sizeof(tdk_cfg_s_t));
    } else {
        gst_snr_ctx.pst_tdk_map = ts_null;
    }

    gst_snr_ctx.u32_tdk_pr_en = 0;

    pst_snr_usr->start_snr((ts_u32)&gst_snr_ctx);

// #ifdef CONFIG_SL_EN

    if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON) {

        if(SystemParaMenu2.m_temp_drift_aisp_mode == TEMP_DRIFT_COMPENSATION_AI_POST_PROCESS_ON){
             pst_snr_attr->st_npu.en_ops =TS_NPU_OPS_AISP;
             pst_snr_attr->st_npu.u32_clk = 180000000;
             pst_snr_attr->s32_vo_img_w = pst_snr_attr->s32_img_w ;
             pst_snr_attr->s32_vo_img_h = pst_snr_attr->s32_img_h ;
        }

        ts_s32 s32_ret = ts_sl_init();
        if (s32_ret != TS_OK){
            vision_log_err("Fail to init sl\n");
        }
        vision_log_warn("gst_snr_ctx.h_sl %p \n",gst_snr_ctx.h_sl);
        gst_snr_ctx.s32_run_auto_b = 1;
        pst_snr_attr->st_cut_ctx.h_free_sht = gst_snr_ctx.h_sl;
        pst_snr_attr->st_cut_ctx.pfn_change_occ = ts_tdk_change_ooc_idx;
    }
// #endif

    // ts_cpl_kthread_tmm_create(ts_null, (ts_cpl_kthread_t)planck640c_auto_cut_process, ts_null, "auto_cut", 4, 4096, ts_null); 
    if (SystemParaMenu2.Usermenu_SW) {
        ts_cpl_kthread_tmm_create(ts_null, (ts_cpl_kthread_t)measurement_auto_cut_process, ts_null, "auto_cut", 4, 4096, ts_null);
    }
    else {
        ts_cpl_kthread_tmm_create(ts_null, (ts_cpl_kthread_t)observation_auto_cut_process, ts_null, "auto_cut", 4, 4096, ts_null); 
    }
    if(SystemParaMenu2.m_fix_snc_switch==VOT_SWITCH_ON){
        ts_cpl_kthread_tmm_create(ts_null, (ts_cpl_kthread_t)snr_fix_snc_proc, ts_null, "fix_snc", 20, 4096, ts_null);
    }

#ifdef VOT_SFFC_TEST
    ts_cpl_kthread_create(ts_null, (ts_cpl_kthread_t)sffc_process, ts_null, "sffc_process", 4, 4096);
#endif

    if (u32_isp_en > 0)
        gst_snr_ctx.s32_run_isp = 1;
    else
        gst_snr_ctx.s32_run_isp = 0;

    if (u32_auto_cut > 0)
        gst_snr_ctx.s32_run_auto_cut = 1;
    else
        gst_snr_ctx.s32_run_auto_cut = 0;

    gst_snr_ctx.s32_run_pause = 0;

    if(SystemParaMenu2.Bpc_ext_SW>0){
        ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150,0);
    }
    // else
    // {
    //     ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150,0);
    // }

// #ifdef CONFIG_EXTBP_EN
//     #ifdef CONFIG_AISP_EN
//         ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150, 1);
//     #else
//         ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150, 0);
//     #endif
// #endif

    ts_cpl_kthread_tmm_create(&st_task, (ts_cpl_kthread_t)isp_pthread, ts_null, "isp", 3, 16384, ts_null);


#ifdef  DEV_SIM_MODE
    // if (u32_usb_en > 0)
    //     usb_usr_init(gpv_tmm_usr);
    vot_usb_device_init();
#endif

    snr_start_npu(pst_snr_attr);

    su32_start_cnt++;

    /* 执行开机脚本，可根据需要放置执行位置 */
    snr_exec_sh();

    // if (pst_snr_attr->u32_pod_dly_ms > 0 && SystemParaMenu2.Bpc_ext_SW>0) 
    // {
    //     /* 用户根据实际探测器图像稳定实际配置休眠等待时间 */
    //     ts_cpl_msleep(pst_snr_attr->u32_pod_dly_ms);
    //     ts_cpl_msleep(200);
    //     snr_init_ext_bp();// 重启加载坏斑 将初始化坏斑加到start_isp函数的以下位置。
    //     pst_snr_attr->u32_pod_dly_ms = 0;
    //     snr_cfg_vpo(pst_snr_attr);
    //    // vision_log_err("snr_init_ext_bp\n ");
    // }

    return TS_OK;
}


ts_s32 snr_get_cut_force(ts_void)
{
    if (gst_snr_ctx.s32_force_cut > 0) {
        gst_snr_ctx.s32_force_cut  = 0;

        return 1;
    } else {
        return 0;
    }
}

ts_s32 ts_tdk_snr_get_cut_temp_auto(ts_void)
{
    return gst_snr_ctx.s32_run_auto_b;
}

ts_s32 ts_tdk_snr_get_cut_auto(ts_void)
{
    return gst_snr_ctx.s32_run_auto_cut;
}


ts_void ts_tdk_snr_wait_pod_done(ts_void)
{
    ts_u32 u32_tm_out, u32_dly;

    if (gst_snr_ctx.st_attr.u32_pod_dly_ms > 200)
        u32_dly = gst_snr_ctx.st_attr.u32_pod_dly_ms;
    else
        u32_dly = 200;

    u32_tm_out = 0;
    while (1) {
        if (gst_snr_ctx.st_attr.u32_pod_done > 0)
            break;

        ts_cpl_fix_msleep(20);

        u32_tm_out += 20;
        if (u32_tm_out >= u32_dly)
            break;
    }
}


ts_void isp_dump(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_pos) 
{
    ts_s32 s32_dump_frm_cnt;
   	#ifdef CONFIG_SNR_PLANCK1280C
    {
          s32_dump_frm_cnt = NUC_SW_FRM_CNT * (1280 * 1024) / (s32_img_w * s32_img_h); 
    }
    #else
    {
        s32_dump_frm_cnt = NUC_SW_FRM_CNT * (640 * 512) / (s32_img_w * s32_img_h);
    }
    #endif
    if (s32_dump_frm_cnt > NUC_SW_FRM_CNT)
        s32_dump_frm_cnt = NUC_SW_FRM_CNT;

    if (0 == sst_nuc_ctx.u32_raw_addr)
        sst_nuc_ctx.u32_raw_addr = (ts_u32)ts_cpl_tmm_malloc(ts_null, s32_img_w * s32_img_h * 2 * s32_dump_frm_cnt, TS_HW_ALIGN_SIZE, "dump_raw");

    if (0 == sst_nuc_ctx.u32_raw_addr) {
        vision_log_err("get raw error !\n");

        return;
    }

    ts_u32 u32_node_cnt = s32_dump_frm_cnt + 1;
    ts_u32 s32_is_dma = 0;

    ts_lib_up_start(s32_pos, (ts_u32*)sst_nuc_ctx.u32_raw_addr, s32_dump_frm_cnt, u32_node_cnt, s32_is_dma);
    ts_s32 s32_cnt = 0;
    for (ts_s32 i = 0; i < s32_dump_frm_cnt; i++) 
    {
        ts_nodebuf_s_t *pst_node = ts_lib_up_get_node(100);
        if (ts_null == pst_node)
            break;

        if (s32_pos < 11) {
            /* 计算当前节点 */
            ts_u16 *pu16_ptr = (ts_u16 *)pst_node->u32_virt_addr;
            for (ts_s32 j = 0; j < s32_img_w * s32_img_h; j++) {
                pu16_ptr[j] = pu16_ptr[j] & 0x3fff;
            }
        }

        ts_lib_up_release_node(pst_node);
        s32_cnt++;
    }

    ts_lib_up_stop();

    vision_log_debug("get frm ok, frmcnt = %d, addr = 0x%x\n", s32_cnt, sst_nuc_ctx.u32_raw_addr);
}


ts_s32 isp_dump_raw_sigle(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_pos, ts_void *pv_raw) 
{
    ts_u32 u32_node_cnt = 1;
    ts_u32 s32_is_dma = 0;

    tdl_isp_claim_s_chn();
    ts_lib_up_start(s32_pos, (ts_u32*)pv_raw, 1, u32_node_cnt, s32_is_dma);
    ts_nodebuf_s_t *pst_node = ts_lib_up_get_node(1000);
    if (ts_null == pst_node)
        return TS_ERR;
    ts_lib_up_release_node(pst_node);
    ts_lib_up_stop();
    tdl_isp_release_s_chn();

    return TS_OK;
}

ts_s32 isp_get_raw_raw_sigle(ts_s32 s32_pos, ts_isp_image_s_t *pst_image, ts_u32 u32_timeout_ms) 
{
    ts_u32 u32_node_cnt = pst_image->u16_frm_num + 1;
    ts_u32 s32_is_dma = 0;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_dump_ad_mutex);
    tdl_isp_claim_s_chn();
    ts_lib_up_start(s32_pos, (ts_u32*)pst_image->pv_image_virt, pst_image->u16_frm_num, u32_node_cnt, s32_is_dma);
    ts_nodebuf_s_t *pst_node = ts_lib_up_get_node(u32_timeout_ms);
    if (ts_null == pst_node)
        return TS_ERR;
    ts_lib_up_release_node(pst_node);
    ts_lib_up_stop();
    tdl_isp_release_s_chn();
    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_dump_ad_mutex);
    return TS_OK;
}

static ts_void snr_cut(ts_s32 *ps32_temp)
{
    ts_u32 u32_1f_dly;
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;

#ifdef CONFIG_SL_EN
    ts_s32 s32_ret;
    ts_sl_status_s_t st_status;
    ts_isp_nuc_attr_s_t st_nuc_attr;
#endif


    pst_ctx = (snr_ctx_s_t *)&gst_snr_ctx;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;

    u32_1f_dly = 1000 / pst_attr->s32_fps;

    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0xffffffff);
    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1);

    /* 等待一帧时间 */
    ts_cpl_fix_msleep(u32_1f_dly);

    //数据流已经关闭，进行相应操作
    tdk_snrcfg_shutter_close();

    ts_s32 s32_temp_1st = sns_get_temperature();

#ifdef CONFIG_SL_EN
    ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    st_nuc_attr.u16_coef = 0;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    ts_cpl_fix_msleep(u32_1f_dly* 2);

    if (gst_snr_ctx.s32_run_auto_b > 0 && gst_snr_ctx.u32_sl_en) {
        vision_log_debug("autob temp %d\n", s32_temp_1st);
        ts_tdk_sl_process(gst_snr_ctx.h_sl, s32_temp_1st, 1);
        s32_ret = ts_tdk_sl_get_status(gst_snr_ctx.h_sl, &st_status);
        if(s32_ret >= 0) {
            if (st_status.s16_tpl_grp_id >= 0 && st_status.s16_tpl_grp_id < 3)
                snr_change_ooc_idx((ts_u32)st_status.s16_tpl_grp_id);
            else
                snr_change_ooc_idx(0);
        }
    }
#endif

    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    ts_s32 s32_temp_2st = sns_get_temperature();

    tdk_snrcfg_shutter_open();

    /* 解除冻结 */
    ts_tdk_vpo_freeze(TS_VPO_MDEV0);
    ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);

    /* 延时1帧 */
    ts_cpl_fix_msleep(u32_1f_dly);
    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);

    /* 延时4帧 */
    ts_cpl_fix_msleep(u32_1f_dly * 4);

    ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);

    *ps32_temp = (s32_temp_1st + s32_temp_2st) / 2;
}

ts_s32 snr_auto_cut_process(ts_void *pv_data)
{
    ts_s32 s32_temp_cur;
    ts_s32 s32_temp_last = 0;
    ts_viu_corr_rst_s_t st_rst_attr;
    ts_s32 s32_auto_b_flag;
    ts_s32 s32_static_time = 3 * 60 * 1000;
    ts_s32 s32_temp_diff_thr = 1500;

#ifdef CONFIG_SL_EN
    ts_isp_nuc_attr_s_t st_nuc_attr;
    ts_s32 s32_frm;
#endif

    ts_s32 s32_temp_1st;
    ts_s32 s32_cnt_times;
    ts_s32 s32_cnt_times_all;
    ts_s32 s32_force_inter;
    static ts_u32 test_cnt;
    ts_s32 s32_flag;
    ts_s32 s32_cut_force;
    ts_s32 s32_diff, s32_diff_abs;

    s32_cnt_times = 0;
    s32_cnt_times_all = 0;
    s32_temp_last = 0;
    s32_auto_b_flag = 0;

    tdk_snrcfg_shutter_close();
    st_rst_attr.u08_reset_only = 0;
    st_rst_attr.u08_reset_val = 0x80;
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
    tdk_snrcfg_shutter_open();

    // 第一次打挡片
    snr_cut(&s32_temp_cur);

    vision_log_debug("1st proc cut!\n");

#ifdef CONFIG_SL_EN
    ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
    s32_frm = st_nuc_attr.u08_stat_frm_num;
    st_nuc_attr.u08_stat_frm_num = 32;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
#endif
    ///////////////////////////////////////////////////////

    s32_temp_1st = sns_get_temperature();

    /* 在自动标定使能位加一个标志位， 防止已经有配置文件的设备，此位为0导致自动挡片功能失效 */
    /* 在 start_isp 中已经完成加载 */

    gst_snr_ctx.s32_run_auto_cut = 1;
    gst_snr_ctx.s32_run_auto_b = 1;
    s32_force_inter = 6;
    test_cnt = 0;
    s32_flag = 1;

    if (gst_snr_ctx.u32_sl_en > 0) {
        /* 根据探测器性能做修改 */
        s32_temp_diff_thr = 10000;
        s32_static_time = 30 * 60 * 1000;
    } else {
        s32_static_time = 3 * 60 * 1000;
        s32_temp_diff_thr = 1500;
    }

    while(1) {
        if ((0 == gst_snr_ctx.s32_run_auto_b) && (s32_auto_b_flag != 1) && (gst_snr_ctx.u32_sl_en > 0)) {

            #ifdef CONFIG_SL_EN
                ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
                st_nuc_attr.u16_coef = 0;
                ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
            #endif

            s32_auto_b_flag = 1;
        }

        s32_cut_force = 0;

        s32_temp_1st = sns_get_temperature();
        s32_diff = s32_temp_1st - s32_temp_last;
        s32_diff_abs = s32_diff;
        if (s32_diff_abs < 0) {
            s32_diff_abs = 0 - s32_diff_abs;
        }

        // 温度差值大于5打一次挡片
        if (s32_diff_abs > s32_temp_diff_thr) {
            s32_cut_force = 1;
        }

        // 开机启动打一次挡片
        if(s32_flag == 1) {
            s32_cut_force = 1;
            s32_flag = 0;
        }

        // 稳定后切换为原来的打挡帧数
        if ((s32_force_inter > 0) && ((s32_cnt_times >= 15000))) {
            s32_cut_force = 1;
            s32_force_inter--;
            if (s32_force_inter < 0) {
                s32_force_inter = 1;
            }

            if (1 == s32_force_inter) {
                #ifdef CONFIG_SL_EN
                    st_nuc_attr.u08_stat_frm_num = s32_frm;
                    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
                #endif
            }
        }

        // 大于多少时间打一次挡片
        if (s32_cnt_times >= s32_static_time) {
            s32_cut_force = 1;
        }

        // 手动打挡片
        if (1 == gst_snr_ctx.s32_force_cut) {
            s32_cut_force = 1;
            gst_snr_ctx.s32_force_cut = 0;
        }

        // 打挡片
        if (s32_cut_force && gst_snr_ctx.s32_run_auto_cut) {
            snr_cut(&s32_temp_cur);

            vision_log_debug("temp=%d,%d\n", s32_temp_1st, s32_temp_cur);   

            s32_cnt_times = 0;
            s32_temp_last = s32_temp_cur;

        } else {
            if ((!gst_snr_ctx.s32_run_auto_b) || (!gst_snr_ctx.s32_run_auto_cut)) {
                goto proc_done;
            }

            #ifdef CONFIG_SL_EN
                if (gst_snr_ctx.s32_run_auto_b > 0 && gst_snr_ctx.u32_sl_en) {
                    s32_auto_b_flag = 0;
                    s32_temp_cur = sns_get_temperature();
                    ts_tdk_sl_process(gst_snr_ctx.h_sl,  s32_temp_cur, 0);
                }
            #endif
        }

proc_done:

        test_cnt++;

        ts_cpl_msleep(20);  
        s32_cnt_times += 20;
        s32_cnt_times_all += 20;
    }

    return TS_ERR;
}


ts_void snr_init_smart(ts_void *pv_snr_smart, ts_u32 u32_size)
{
    sfc_cfg_file_s_t *pst_mmap;
    ts_s32 s32_hfile_r;
    ts_s32 s32_cfg_flag, s32_file_sz;

    pst_mmap = gst_snr_ctx.pst_sns_map;
    s32_file_sz = 0;
    if (ts_null == pst_mmap) {
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_SNS);
        if (s32_hfile_r > 0) {
            s32_file_sz = ts_lib_sfc_get_size(s32_hfile_r);
            pst_mmap = (sfc_cfg_file_s_t *)ts_lib_sfc_mmap(s32_hfile_r, 0, (ts_u32)s32_file_sz);
            if (ts_null == pst_mmap)
                vision_log_err("sns file mmap fail!\n");
            else
                gst_snr_ctx.pst_sns_map = pst_mmap;
        } else {
            vision_log_err("sns file open fail!\n");
        }
    }

    s32_cfg_flag = 0;
    if ((pst_mmap != ts_null) && (s32_file_sz > 0)) {
        if (pst_mmap->u32_valid_code != SFC_FILE_VALID_CODE) {
            ts_cpl_memset32((ts_void *)pst_mmap, 0, ((ts_u32)s32_file_sz >> 2));
        } else {
            if (pst_mmap->u32_valid_flag > 0) {
                ts_cpl_memcpy(pv_snr_smart, (ts_void *)pst_mmap->au64_payload, u32_size);
                s32_cfg_flag = 1;
            }
        }
    }

    if (0 == s32_cfg_flag)
        vision_log_warn("sns cfg use def cfg!\n");
    else
        vision_log_warn("sns cfg load done!, mmap addr = %x\n", (ts_u32)pst_mmap);
}


extern ts_u32 __cfg_start;
extern ts_u32 __cfg_end;
static ts_void * snr_get_uimg_isp_cfg(ts_u32 u32_cfg_addr)
{
    ts_u32 u32_isp_cfg_addr, u32_isp_cfg_size;
    // ts_u32 *pu32_ptr;
    ts_void *pv_cfg;

    if (0 == u32_cfg_addr)
        return ts_null;

    u32_isp_cfg_addr = (ts_u32)&__cfg_start;
    u32_isp_cfg_size = (ts_u32)&__cfg_end - (ts_u32)&__cfg_start;

    if (u32_isp_cfg_size > 0) {
        gst_snr_ctx.pst_isp_cfg = (sfc_cfg_file_s_t *)u32_cfg_addr;
        gst_snr_ctx.pst_isp_cfg->u32_valid_code = SFC_FILE_VALID_CODE;

        pv_cfg = (ts_void *)gst_snr_ctx.pst_isp_cfg->au64_payload;
        u32_isp_cfg_addr += 0x24;
        u32_isp_cfg_size -= 0x24;
        ts_cpl_memcpy(pv_cfg, (ts_void *)u32_isp_cfg_addr, u32_isp_cfg_size);

        vision_log_debug("use uimg isp cfg\n");

        return pv_cfg;
    } else {
        vision_log_debug("use def isp cfg!\n");

        return ts_null;
    }
}


ts_void *snr_get_isp_cfg(ts_u32 u32_def_cfg_type)
{
    ts_s32 s32_hfile_r;
    ts_u32 u32_cfg_size;
    ts_u32 u32_file_size; 

    u32_cfg_size = ts_tdk_isp_get_cfg_size();
    u32_file_size = sizeof(sfc_cfg_file_s_t) + u32_cfg_size * SFC_FILE_MAX_ISP_CFG_NUM;

    /* 申请 isp 配置空间 */
    if (0 == gst_snr_ctx.u32_isp_cfg_addr) {
        //gst_snr_ctx.u32_isp_cfg_addr = (ts_u32)ts_cpl_kmalloc_align(u32_file_size, TS_CPL_GFP_KERNEL, 32);
        gst_snr_ctx.u32_isp_cfg_addr = (ts_u32)ts_cpl_tmm_malloc(null, u32_file_size, 32, "isp_cfg");

        vision_log_debug("cfg addr=%#x\n", gst_snr_ctx.u32_isp_cfg_addr);
    }

    if (0 == gst_snr_ctx.u32_isp_cfg_addr) {
        vision_log_err("malloc isp cfg err !\n");

        TS_CPL_ASSERT(0);
    }

    s32_hfile_r = ts_lib_sfc_open(FN_SUPER_ISP);
    if (s32_hfile_r > 0) {
        gst_snr_ctx.pst_isp_cfg = (sfc_cfg_file_s_t *)gst_snr_ctx.u32_isp_cfg_addr;
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)gst_snr_ctx.pst_isp_cfg, u32_file_size);
        ts_lib_sfc_close(s32_hfile_r);
        if (SFC_FILE_VALID_CODE == gst_snr_ctx.pst_isp_cfg->u32_valid_code) {
            vision_log_debug("use save isp cfg!\n");

            return (ts_void *)gst_snr_ctx.pst_isp_cfg->au64_payload;
        }
    }

    vision_log_debug("use def isp cfg!\n");

#if 1
    return snr_get_uimg_isp_cfg(gst_snr_ctx.u32_isp_cfg_addr);
#else
    return ts_null;
#endif
}


ts_void snr_init_ooc(ts_void)
{
    ts_buffer_s_t st_occ_buf;
    sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_hfile_r;
    ts_u32 u32_size, u32_cfg_flag;

    /* OCC 初始化 */
    u32_cfg_flag = 0;
    s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));

        /* 当前demo只是支持一个OCC */ 
        if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & 1)) {
            ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);
            vision_log_debug("occ info: name %s, flag %#x, num %d, w,h=%d,%d\n", \
                st_sfc_file.ac_cfg_name, st_sfc_file.u32_valid_flag, st_sfc_file.u32_cfg_num, st_occ_buf.u32_width, st_occ_buf.u32_height);

            u32_size = st_occ_buf.u32_width * st_occ_buf.u32_height;
            ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)st_occ_buf.pu08_ptr, u32_size); //656 * 522

            vision_log_debug("use saved occ value...\n");

            u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
    }

    if (0 == u32_cfg_flag)
        vision_log_debug("use cut occ value...\n");
}


ts_void snr_init_sffc(ts_isp_sffc_attr_s_t *pst_sffc_attr)
{
    ts_s32 s32_hfile_r;
    snr_attr_s_t *pst_attr;
    sfc_cfg_file_s_t st_sfc_file;
    ts_isp_sffc_buf_s_t st_sffc_buf;
    ts_u32 u32_cfg_flag, u32_size;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, pst_sffc_attr); 

    st_sffc_buf.b_working = 1;
    ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, (ts_isp_sffc_buf_s_t *)&st_sffc_buf);
    vision_log_debug("sffc get buf 0x%x \n", st_sffc_buf.st_buf.phys_addr);

    s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
    u32_cfg_flag = 0;
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {
            u32_size = pst_attr->s32_img_w * pst_attr->s32_img_h * 2;
            ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)st_sffc_buf.st_buf.pu08_ptr, u32_size);
            vision_log_debug("use saved sffc value...\n");
            u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
    }

    if (0 == u32_cfg_flag) {
        ts_u16 *pu16_ptr = (ts_u16*)st_sffc_buf.st_buf.pu08_ptr;
        for (ts_s32 i = 0; i < pst_attr->s32_img_w * pst_attr->s32_img_h ; i++)
            pu16_ptr[i] = 0; 

        vision_log_debug("use 0.0 sffc value...\n");
    }

    ts_tdk_isp_flush_sffc_buf(TS_ISP_MDEV0);
}


ts_void snr_init_nuc(ts_isp_nuc_attr_s_t *pst_nuc_attr)
{
    ts_s32 s32_hfile_r;
    ts_u32 i, j, u32_cfg_flag;
    sfc_cfg_file_s_t st_sfc_file;
    ts_buffer_s_t st_kbuf;
    ts_u16 *pu16_ptr;
    snr_attr_s_t *pst_attr;
    
    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, (const ts_isp_nuc_attr_s_t *)pst_nuc_attr); 
    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    s32_hfile_r = ts_lib_sfc_open(FN_K_CFG);
    ts_cpl_memset(&st_sfc_file, 0, sizeof(sfc_cfg_file_s_t));
    u32_cfg_flag = 0;
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));

        if (SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) {
            ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)st_kbuf.pu08_ptr, pst_attr->s32_img_w * pst_attr->s32_img_h * 2);
            vision_log_debug("use saved k value...\n");
            u32_cfg_flag = 1;
        }

        ts_lib_sfc_close(s32_hfile_r);
    }

    pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
    if (0 == u32_cfg_flag) {
        for (i = 0; i < pst_attr->s32_img_w * pst_attr->s32_img_h; i++)
            pu16_ptr[i] = 0x4000; 

        vision_log_debug("use 1.0 k value...\n");
    } else {
        vision_log_debug("load k value:\n");
        j = 0;
        for (i = 0; i < 32; i++) {
            printf("%05d ", pu16_ptr[i]);
            j++;
            if (j >= 8) {
                j = 0;

                printf("\n");
            }
        }
    }

    ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    /* 合并KB表 */
    vot_isp_bp_kb_table_init();
    vot_isp_bp_kb_table_merge_to_sdk_k_table(); 
    // vot_isp_dump_bp_k();

    ts_u32 u32_bp_buf_size =  MAX_BP_POS_NUM * 4 * 2;
    ts_u32 u32_malloc_addr_bp = (ts_u32)ts_cpl_tmm_malloc(ts_null, u32_bp_buf_size, TS_HW_ALIGN_SIZE, "bp_pos");
    if (0 == u32_malloc_addr_bp)
    {
       printf("malloc vp buf error \n"); 
       while(1);  //////snr_init_nuc 出错则设备无法运行，保留现场
    }
    ts_u32 *pu32_bp_buf = (ts_u32 *)u32_malloc_addr_bp;
    ts_cpl_memset(pu32_bp_buf, 0, u32_bp_buf_size);
    gst_bp_info.pv_bp_buf_addr = (ts_void *)u32_malloc_addr_bp;
    gst_bp_info.pv_bp_save_addr = (ts_void *)(u32_malloc_addr_bp + MAX_BP_POS_NUM * 4);
    gst_bp_info.u32_bp_cnt_new_add = 0;
    gst_bp_info.u32_bp_cnt = 0;

    s32_hfile_r = ts_lib_sfc_open(FN_SUPER_BP);
    if (s32_hfile_r > 0) {
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)&gst_bp_info, sizeof(bp_pos_info_s_t));
        if ((BP_FILE_VALID_FLAG_0 == gst_bp_info.u32_valid_code_0) && (BP_FILE_VALID_FLAG_1 == gst_bp_info.u32_valid_code_1)){
            gst_bp_info.pv_bp_buf_addr = (ts_void *)u32_malloc_addr_bp;
            gst_bp_info.pv_bp_save_addr = (ts_void *)(u32_malloc_addr_bp + MAX_BP_POS_NUM * 4);
            gst_bp_info.u32_bp_cnt_new_add = gst_bp_info.u32_bp_cnt;
            gst_bp_info.u32_bp_cnt = 0;
            ts_lib_sfc_read(s32_hfile_r, sizeof(bp_pos_info_s_t), (ts_u08*)gst_bp_info.pv_bp_buf_addr, MAX_BP_POS_NUM * 4);
        }else{
            gst_bp_info.pv_bp_buf_addr = (ts_void *)u32_malloc_addr_bp;
            gst_bp_info.pv_bp_save_addr = (ts_void *)(u32_malloc_addr_bp + MAX_BP_POS_NUM * 4);
            gst_bp_info.u32_bp_cnt_new_add = 0;
            gst_bp_info.u32_bp_cnt = 0;
            printf("save bp cfg is not valid \n");
        }

		printf("sync bp done...\n");
        ts_lib_sfc_close(s32_hfile_r);
    }

    ts_tdk_snr_lib_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, (bp_pos_info_s_t *)&gst_bp_info);
    ts_isp_set_bp_multi_data();

    printf("snr_init_nuc:num = %d, addr = 0x%08X, 0x%08X\n",
            gst_bp_info.u32_bp_cnt,                // 打印pu32_bp_cnt指向的数值（坏点个数）
            (ts_u32)gst_bp_info.pv_bp_buf_addr, (ts_u32)gst_bp_info.pv_bp_save_addr); // 打印缓冲区地址，方便调试

}

ts_void snr_fix_snc_proc(ts_void)
{
    ts_isp_ext_func_s_t st_ext_func;

    st_ext_func.pfn_fw_isp_get_raw = isp_get_raw_raw_sigle;
    ts_tdk_isp_set_ext_func(TS_ISP_MDEV0, &st_ext_func);

    // gst_snr_ctx.s32_fix_snc_en = 1;
    while(1) {
        
        // 矫正的时候不进行fix_snc
        if (ISP_get_det_calibrationing_flag() > 0)
        {
            ts_cpl_fix_msleep(1000);
            continue;
        }
        
        if (ISP_get_det_switching_flag() > 0)
        {
            ts_cpl_fix_msleep(100);
            continue;
        }

        if (ISP_get_bottom_statistic_force_flag() > 0)
        {
            ts_cpl_fix_msleep(100);
            continue;
        }
        /* 测温功能正在取流, 不允许其他地方再取 */
        if (global_sys_ctrl_info.m_temp_get_streaming_flag == 1) {
            ts_cpl_fix_msleep(100);
            continue;
        }
        if (gst_snr_ctx.s32_fix_snc_en)
            ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);

        ts_cpl_msleep(10);  
    }
}


ts_s32 snr_wait_ready(ts_u32 u32_tmout_ms)
{
    ts_viu_array_info_s_t st_viu_info;
    ts_cpl_timeval_s_t st_start, st_end;
    ts_u32 u32_dif;

    ts_cpl_gettimeofday(&st_start);
    while (1) {
        /* FIXME : 统一喂狗 */
        ts_lib_wdt_feeding();

        ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
        if (st_viu_info.u32_mean > 0)
            break;

        ts_cpl_gettimeofday(&st_end);
        u32_dif = ts_cpl_get_timedif((ts_cpl_timeval_s_t *)&st_start, (ts_cpl_timeval_s_t *)&st_end);
        u32_dif = u32_dif / 1000;
        if (u32_dif > u32_tmout_ms) {
            vision_log_err("%s\n", TS_ETIMEDOUT_STR);

            return -TS_ETIMEDOUT;
        }

        ts_cpl_msleep(10);
    }

    ts_cpl_gettimeofday(&st_end);
    u32_dif = ts_cpl_get_timedif((ts_cpl_timeval_s_t *)&st_start, (ts_cpl_timeval_s_t *)&st_end);

    vision_log_debug("Sensor ok, cost time=%d us\n", u32_dif);

    return TS_OK;
}


ts_s32 sns_get_temperature(ts_void)
{
    if (ts_null == gst_snr_ctx.st_attr.get_temperature)
        return 0;
    else
        return gst_snr_ctx.st_attr.get_temperature();
}

extern ts_void vot_hst_update_attr_safe(void);
extern ts_void vot_get_fire_statis(ts_isp_fire_statis_s_t* out_fire_statis);


/*isotherm*/
TS_DECLARE_CMD(iso)
{
    ts_u32 u32_iso_h, u32_iso_l;
    ts_vpo_pcolor_attr_s_t st_pcolor_attr;
    ts_viu_array_info_s_t st_viu_info;

    vot_hst_update_attr_safe();
    ts_isp_fire_statis_s_t fire_statis;
    vot_get_fire_statis(&fire_statis);

    ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);

    

    if (argc > 3) {
        // u32_iso_h = st_viu_info.u32_mean + atoi(argv[2]) + atoi(argv[3]);
        // u32_iso_l = st_viu_info.u32_mean + atoi(argv[2]) - atoi(argv[3]);

        u32_iso_h = atoi(argv[2]);
        u32_iso_l = atoi(argv[3]);

        printf("argv2 %d\n",atoi(argv[2]));
    } else {
        u32_iso_h = st_viu_info.u32_mean + 200;
        u32_iso_l = st_viu_info.u32_mean - 200;
    }

    printf(" u16_min_val %u u16_max_val %u,u16_mean %u\n",fire_statis.st_frm_info.u16_min_val,fire_statis.st_frm_info.u16_max_val,fire_statis.st_frm_info.u16_mean);
    printf("st_viu_info.u32_mean %u au32_iso_h %u,u32_iso_l %u\n",st_viu_info.u32_mean,u32_iso_h,u32_iso_l);
   
    /* 凑成 16Bit */
    u32_iso_h = u32_iso_h << 2;
    u32_iso_l = u32_iso_l << 2;

    if (argc > 1) {
        if ('i' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 0;
            st_iso.u16_isotherm_high = u32_iso_h; 
            st_iso.u16_isotherm_low = u32_iso_l; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);
            printf(" u32_iso_h %u u32_iso_l%u !\n", u32_iso_h, u32_iso_l);
            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_iron_tab;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            printf(" st_iso.u16_isotherm_high %u st_iso.u16_isotherm_low %u !\n", st_iso.u16_isotherm_high, st_iso.u16_isotherm_low);
            printf("iso init over !\n");
        }
        else if ('e' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 1;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 0;
            st_iso.u16_isotherm_high = u32_iso_h; 
            st_iso.u16_isotherm_low = u32_iso_l; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_green_vuya;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;
            printf(" st_iso.u16_isotherm_high %u st_iso.u16_isotherm_low %u !\n", st_iso.u16_isotherm_high, st_iso.u16_isotherm_low);
            printf(" u32_iso_h %u u32_iso_l%u !\n", u32_iso_h, u32_iso_l);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            
            printf("iso init over e !\n");
        } 
        else if ('u' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 255;
            st_iso.u16_isotherm_high = 65535; 
            st_iso.u16_isotherm_low = u32_iso_h; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor =  (ts_yuv_s_t *)&sast_iso_red_vuya; //(ts_yuv_s_t *)&LanHei_TAB; // //
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            printf(" u32_iso_h %u u32_iso_l %u !\n", u32_iso_h, u32_iso_l);
            printf(" st_iso.u16_isotherm_high %u st_iso.u16_isotherm_low %u !\n", st_iso.u16_isotherm_high, st_iso.u16_isotherm_low);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            
            printf("iso init u  1 !\n");
        } 
        else if ('d' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 0;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 255;
            st_iso.u16_isotherm_high = u32_iso_l; 
            st_iso.u16_isotherm_low = 0; 
            st_iso.u16_max_y16 = 65535;
            st_iso.u16_min_y16 = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);
            printf(" u32_iso_h %u u32_iso_l%u !\n", u32_iso_h, u32_iso_l);
            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_white_vuya; //(ts_yuv_s_t *)&RAINBOW_TAB; //
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            printf(" u32_iso_h %u u32_iso_l %u !\n", u32_iso_h, u32_iso_l);
            printf(" st_iso.u16_isotherm_high %u st_iso.u16_isotherm_low %u !\n", st_iso.u16_isotherm_high, st_iso.u16_isotherm_low);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            
            printf("iso init over d 2 !\n");
        } 
        else if ('o' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 1;
            st_iso.u08_mode = 1;
            st_iso.u08_y8_high = 255;
            st_iso.u08_y8_low = 0;
            st_iso.u16_isotherm_high = u32_iso_h;
            st_iso.u16_isotherm_low = u32_iso_l;
            st_iso.u16_max_y16 = 12000 * 4;
            st_iso.u16_min_y16 = 6000 * 4;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_black_vuya; //(ts_yuv_s_t *)&sast_iso_yuv_tab;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_ISO;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            printf(" u32_iso_h %u u32_iso_l %u !\n", u32_iso_h, u32_iso_l);
            printf(" st_iso.u16_isotherm_high %u st_iso.u16_isotherm_low %u !\n", st_iso.u16_isotherm_high, st_iso.u16_isotherm_low);
            printf("iso init over !\n");
        } else if ('m' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            #if 0
            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_iron_tab;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_NORMAL;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            #endif


            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_gray_tab;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_NORMAL;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
        } else if ('n' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 1;
            st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)&sast_iso_iron_tab;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_NORMAL;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
        } else if ('c' == argv[1][1]) {
            ts_isp_isotherm_attr_s_t st_iso;
            st_iso.u08_enable = 0;
            ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);

            st_pcolor_attr.u32_enable = 0;
            st_pcolor_attr.pst_pcolor = ts_null;
            st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_NORMAL;
            st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

            ts_tdk_vpo_set_pcolor_attr(TS_VPO_MEM_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
            ts_tdk_vpo_set_pcolor_attr(TS_VPO_CVBS_MDEV, (const ts_vpo_pcolor_attr_s_t *)&st_pcolor_attr);
        } else {
            printf("iso -i \n");
            printf("iso -e \n");
            printf("iso -o \n");
        }
    } else {
        printf("iso -i \n");
        printf("iso -e \n");
        printf("iso -o \n");
    }

    return 0;
}


ts_s32 snr_open_i2c_dev(ts_char *pc_devname, ts_u08 u08_dev_addr)
{
    ts_i2c_attr_s_t st_i2c_attr;

    if (0 == sst_i2c_ctx.u32_opened) {
        sst_i2c_ctx.s32_i2c_hdl = ts_lib_i2c_open(pc_devname);
        if (sst_i2c_ctx.s32_i2c_hdl < 0) {
            vision_log_err("open i2c device fail!\n");

            return TS_ERR;
        }

        sst_i2c_ctx.u32_opened = 1;
        sst_i2c_ctx.u32_dev_addr = (ts_u32)u08_dev_addr;

        ts_lib_i2c_get_attr(sst_i2c_ctx.s32_i2c_hdl, (ts_i2c_attr_s_t *)&st_i2c_attr);

        #ifdef CONFIG_SNR_GST412
            st_i2c_attr.u32_bus_hz = 80000;
        #else
            st_i2c_attr.u32_bus_hz = 200000;
        #endif

        ts_lib_i2c_set_attr(sst_i2c_ctx.s32_i2c_hdl, (ts_i2c_attr_s_t *)&st_i2c_attr);
    }

    return TS_OK;
}


ts_s32 snr_i2c_getreg(ts_u08 u08_regaddr, ts_u08 *pu08_data)
{
    ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);

        return -TS_ENOTCFG;
    } else {
        s32_ret = ts_lib_i2c_ra08d08_readb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u08_regaddr, pu08_data);
        if (s32_ret != TS_OK)
            vision_log_err("sensor_getreg err:ret=%d\n", s32_ret);

        return s32_ret;
    }
}


ts_void snr_i2c_setreg(ts_u08 u08_regaddr, ts_u08 u08_data)
{
    ts_s32 s32_ret;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);
    } else {
        s32_ret = ts_lib_i2c_ra08d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u08_regaddr, u08_data);
        if (s32_ret != TS_OK)
            printf("snr_i2c_setreg err:ret=%d\n", s32_ret);
    }
}


ts_s32 snr_i2c_get_r16d08_continue(ts_u32 u32_reg_addr, ts_u08 *pu08_data, ts_u32 u32_cnt)
{
    ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);

        return -TS_ENOTCFG;
    } else {
        s32_ret = ts_lib_i2c_ra16d08_read(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, pu08_data, u32_cnt);
        if (s32_ret != TS_OK)
            vision_log_err("ret=%d\n", s32_ret);

        return s32_ret;
    }
}


ts_void snr_i2c_set_r16d08_continue(ts_u32 u32_reg_addr, ts_u08 *pu08_data, ts_u32 u32_cnt)
{
	ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);
    } else {
        s32_ret = ts_lib_i2c_ra16d08_write(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, pu08_data, u32_cnt);
        if (s32_ret != TS_OK)
            vision_log_err("ret=%d\n", s32_ret);
    }
}


ts_s32 snr_i2c_get_r16d08(ts_u32 u32_reg_addr, ts_u08 *pu08_data)
{
    ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);

        return -TS_ENOTCFG;
    } else {
        s32_ret = ts_lib_i2c_ra16d08_readb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, pu08_data);
        if (s32_ret != TS_OK)
            vision_log_err("ret=%d\n", s32_ret);

        return s32_ret;
    }
}


ts_void snr_i2c_set_r16d08(ts_u32 u32_reg_addr, ts_u08 u08_data)
{
	ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        printf("%s\n", TS_ENOTCFG_STR);
    } else {
        s32_ret = ts_lib_i2c_ra16d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, u08_data);
        if (s32_ret != TS_OK)
            printf("snr_i2c_set_r16d08 err:ret=%d\n", s32_ret);
    }
}

ts_s32 snr_i2c_get_r08d16(ts_u32 u32_reg_addr, ts_u16 *pu16_data)
{
    ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        printf("%s\n", TS_ENOTCFG_STR);

        return -TS_ENOTCFG;
    } else {
        s32_ret = ts_lib_i2c_ra08d16_readw(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, pu16_data);
        if (s32_ret != TS_OK)
            printf("snr_i2c_get_r08d16 err:ret=%d\n", s32_ret);

        return s32_ret;
    }
}


ts_void snr_i2c_set_r08d16(ts_u32 u32_reg_addr, ts_u16 u16_data)
{
	ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        printf("%s\n", TS_ENOTCFG_STR);
    } else {
        s32_ret = ts_lib_i2c_ra08d16_writew(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u32_reg_addr, u16_data);
        if (s32_ret != TS_OK)
            printf("snr_i2c_set_r08d16 err:ret=%d\n", s32_ret);
    }
}


TS_DECLARE_CMD(pw)
{
    tdk_cfg_s_t *pst_mmap;

    pst_mmap = gst_snr_ctx.pst_tdk_map;
    if (ts_null == pst_mmap) {
        vision_log_err("%s\n", TS_ENONEXIST_STR);

        return TS_ERR;
    }

    ts_cpl_memset32((ts_void *)&pst_mmap->ac_usr_info, 0, 8);
    ts_cpl_memset32((ts_void *)&pst_mmap->ac_usr_date, 0, 8);
    if (3 == argc) {
        ts_cpl_strncpy((char *)&pst_mmap->ac_usr_info, argv[1], 30);
        ts_cpl_strncpy((char *)&pst_mmap->ac_usr_date, argv[2], 30);
    }

    pst_mmap->u32_magic = TS_TDK_FILE_MAGIC;

    /* 同步文件 */
    ts_lib_sfc_sync();

    return TS_OK;
}


TS_DECLARE_CMD(pr)
{
    tdk_cfg_s_t *pst_mmap;

    pst_mmap = gst_snr_ctx.pst_tdk_map;

    gst_snr_ctx.u32_tdk_pr_en = 1;

    vision_log_notice("uinfo:%s, udate:%s\n", (ts_char *)&pst_mmap->ac_usr_info, (ts_char *)&pst_mmap->ac_usr_date);

    return TS_OK;
}

TS_DECLARE_CMD(usbt)
{
    ts_u32 u32_type;

    if (argc > 1) {
        u32_type = ts_cpl_str2num(argv[1]);
    } else {
        u32_type = 0;
    }

    if (u32_type > 1)
        u32_type = 1;

    gst_snr_ctx.pst_tdk_map->u08_usb_type = (ts_u08)u32_type;
    gst_snr_ctx.pst_tdk_map->u32_magic = TS_TDK_FILE_MAGIC;

    /* 同步文件 */
    ts_lib_sfc_sync();

    return TS_OK;
}


TS_DECLARE_CMD(sh)
{
    tdk_cfg_s_t *pst_mmap;
    ts_char *pc_ptr;
    ts_u32 i, u32_max, u32_len;

    pst_mmap = gst_snr_ctx.pst_tdk_map;
    ts_cpl_memset32((ts_void *)&pst_mmap->ac_sh, 0, 64);
    if (argc > 1) {
        pc_ptr = (ts_char *)&pst_mmap->ac_sh;

        u32_max = 250;
        for (i = 1; i < argc; i++) {
            ts_cpl_strncpy(pc_ptr, argv[i], u32_max);
            u32_len = ts_cpl_strnlen(argv[i], u32_max);
            if (';' == pc_ptr[0]) {
                pc_ptr[0] = '\0';
            }

            pc_ptr += u32_len;
            pc_ptr[0] = ' ';
            u32_len++;
            pc_ptr++;

            if (u32_len > u32_max)
                u32_max = 0;
            else
                u32_max = u32_max - u32_len;

            if (0 == u32_max)
                break;
        }

        pc_ptr = (ts_char *)&pst_mmap->ac_sh;
        u32_max = 250;
        i = 0;
        while (1) {
            u32_len = ts_cpl_strnlen(pc_ptr, u32_max);
            vision_log_notice("sh[%d] = %s\n", i, pc_ptr);

            i++;
            pc_ptr += (u32_len + 1);
            u32_max -= (u32_len + 1);
            if ('\0' == pc_ptr[0])
                break;
        }

        pst_mmap->u08_sh_en = 1;
    } else {
        pst_mmap->u08_sh_en = 0;
    }

    pst_mmap->u32_magic = TS_TDK_FILE_MAGIC;

    /* 同步文件 */
    ts_lib_sfc_sync();

    return TS_OK;
}

TS_DECLARE_CMD(show)
{
    snr_exec_sh();

    return TS_OK;
}


static ts_s32 scfg_set_cut_param(int argc, char * argv[])
{
    ts_s32 s32_ret;
    ts_u32 i, u32_sync_en;

    for (i = 0; i < argc; i++) {
        vision_log_debug("%d:%s\n", i, argv[i]);
    }

    s32_ret = TS_OK;
    if (argc > 0) {
        if ('r' == argv[0][0]) {
            printf("temp gap=%d, time gap=%d\n", gst_snr_ctx.pst_scfg->s32_cut_max_temp_diff, gst_snr_ctx.pst_scfg->s32_max_gap);
        } else {
            if (2 == argc) {
                gst_snr_ctx.pst_scfg->s32_cut_max_temp_diff = ts_cpl_str2num(argv[1]);
                u32_sync_en = 1;
            } else if (3 == argc) {
                gst_snr_ctx.pst_scfg->s32_cut_max_temp_diff = ts_cpl_str2num(argv[1]);
                gst_snr_ctx.pst_scfg->s32_max_gap = ts_cpl_str2num(argv[2]);
                u32_sync_en = 1;
            } else {
                u32_sync_en = 0;
            }

            if (u32_sync_en > 0) {
                gst_snr_ctx.u32_sync_scfg = 1;

                ts_lib_sfc_sync();
            }
        }
    }

    return s32_ret;
}


TS_DECLARE_CMD(scfg)
{
    ts_s32 s32_ret;

    s32_ret = TS_OK;
    if (argc > 1) {
        if (!ts_cpl_strcmp(argv[1], "cut")) {
            argc -= 2;
            argv += 2;

            s32_ret = scfg_set_cut_param(argc, argv);
        }
    } else {
        printf("scfg cut w temp_gap time_gap\n");
        printf("scfg cut r\n");
    }

    return s32_ret;
}


TS_DECLARE_CMD(getcol)
{
    ts_u32 u32_col_addr, u32_col_val;
    ts_u32 i, u32_col_num;
    ts_s32 s32_ret;

    u32_col_addr = 200;
    u32_col_num = 1;

    if (argc >= 2)
        u32_col_addr = (ts_u32)ts_cpl_str2num(argv[1]);

    if (argc >= 3)
        u32_col_num = (ts_u32)ts_cpl_str2num(argv[2]);

    for (i = 0; i < u32_col_num; i++) {
        s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, u32_col_addr, 0, ts_null, (ts_u32 *)&u32_col_val);
        if (TS_OK == s32_ret) {
            vision_log_debug("col[%d]:%d\n", u32_col_addr, u32_col_val);
        }

        u32_col_addr++;
        ts_cpl_msleep(100);
    }

    return TS_OK;
}


/* 
    sht -o  //开启快门
    sht -c  //关闭快门
*/
TS_DECLARE_CMD(sht)
{
    if (argc > 1) {
        if ('o' == argv[1][1])
            tdk_snrcfg_shutter_open();
        else
            tdk_snrcfg_shutter_close();
    }

    return 0;
}

TS_DECLARE_CMD(SW)
{
	ts_u32 u32_cfg;
	snr_attr_s_t *pst_attr;
	snr_ctx_s_t *pst_ctx;
	if (argc > 1){
		u32_cfg = ts_cpl_str2num(argv[1]);
	}
	else {
		u32_cfg = 0x00;
	}

	pst_ctx =(snr_ctx_s_t *)&gst_snr_ctx;
	pst_attr = &pst_ctx->st_attr;
	ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
	ts_tdk_vpo_disable_chn(TS_VPO_MEM_MDEV);
	//ts_tdk_vpo_disable_grp(TS_VPo_MDEve);

	ts_cpl_msleep(100);
	if (0 == u32_cfg){
        ts_tdk_aisp_set_npu_status(0);
        ts_tdk_aisp_set_sr_status(0);
        pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
        pst_attr->st_npu.u32_clk = 0;

        pst_attr->s32_vo_img_w = pst_attr->s32_img_w;
        pst_attr->s32_vo_img_h = pst_attr->s32_img_h;
	}
	else{
        ts_tdk_aisp_set_sr_status(1);
        pst_attr->st_npu.en_ops = TS_NPU_OPS_SR;
        pst_attr->st_npu.u32_clk = 132000000;
        pst_attr->s32_vo_img_w = pst_attr->s32_img_w << 1;
        pst_attr->s32_vo_img_h = pst_attr->s32_img_h << 1;
	}
	snr_cfg_vpo(pst_attr);
	return 0;
}
