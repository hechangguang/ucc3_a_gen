/*
 * isp.c -- vision 图像校正等相关命令
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
#include <command.h>
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "sffs_files.h"
#include "snr_comm.h"
#include <uapi/isp_reg.h>
#include <uapi/fw_isp_status.h>
#include <uapi/fw_isp_cali.h>

extern snr_ctx_s_t gst_snr_ctx;
extern ts_void ts_tdk_snr_set_debug(ts_u32 u32_lvl);

ts_void sffc_off(ts_void);
ts_void sffc_on_m(ts_s32 s32_v);
ts_void ts_tdk_snr_sht_open(ts_void)
{
    tdk_snrcfg_shutter_open();
}


ts_void ts_tdk_snr_sht_close(ts_void)
{
    tdk_snrcfg_shutter_close();
}


ts_s32 ts_tdk_snr_get_raw_sigle(ts_s32 s32_pos, ts_isp_image_s_t *pst_image, ts_u32 u32_timeout_ms)
{
    return isp_get_raw_raw_sigle(s32_pos, pst_image, u32_timeout_ms);
}


ts_void isp_set_run(ts_u32 u32_run_en)
{
    gst_snr_ctx.s32_run_isp = u32_run_en;
}

typedef struct ts_isp_en_s         
{
	ts_u08 u08_bpc_en;
	ts_u08 u08_ltm_en;
	ts_u08 u08_ee_en;
	ts_u08 u08_dde_en;
	ts_u08 u08_nr2d_raw_en;
	ts_u08 u08_nr2d_yuv_c0_en;
	ts_u08 u08_nr2d_yuv_c1_en;
	ts_u08 u08_snr_en;
	ts_u08 u08_fpnc_en;
	ts_u08 u08_drc_en;
	ts_u08 u08_lce_en;
	ts_u08 u08_usr_adj_en;
	ts_u08 u08_rev[4];

    ts_u16 u16_thr_brit;
    ts_u16 u16_thr_dark;
}ts_isp_en_s_t; 

ts_isp_en_s_t st_isp_en_ctx = {0};


TS_WEAK ts_void snr_force_cut(ts_void)
{
    
}

TS_WEAK ts_s32 ts_isp_set_bp_multi_data(ts_void)
{
    /* do nothing */
    ts_cpl_log_warn("do nothing\n");

    return -TS_ENOTSUP;
}

TS_WEAK ts_void *ts_isp_get_bp_pos_addr(ts_u32 u32_bp_cnt)
{
    ts_cpl_log_warn("do nothing\n");

    return ts_null;
}


// TS_WEAK ts_s32 isp_auto_bp_det(ts_u32 u32_hist, ts_u32 u32_diff, ts_u32 u32_flt_size, ts_u32 u32_det_type, ts_u32 u32_pos)
// {
//     ts_cpl_log_warn("%s do nothing\n",__FUNCTION__);

//     return -TS_ENOTSUP;
// }

/* occ 相关函数封装 */
ts_void snr_exec_ooc(ts_u32 u32_rst_en, ts_u32 u32_rst_val, ts_u32 u32_sht_en, ts_u32 u32_occ_en)
{
    ts_viu_corr_rst_s_t st_rst_attr;

    if (u32_sht_en > 0)
        tdk_snrcfg_shutter_close();

    if (u32_rst_en > 0) {
        st_rst_attr.u08_reset_only = 1;
        st_rst_attr.u08_reset_val = (ts_u08)u32_rst_val;

        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
    } else {
        st_rst_attr.u08_reset_only = 0;
        st_rst_attr.u08_reset_val = 0;
    }

    if (u32_occ_en > 0)
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, ts_null);

    if (u32_sht_en > 0)
        tdk_snrcfg_shutter_open();
}

//需要新增flash空间(可根据⾃⼰需求⾃⾏选择保存位置)。
ts_extbpc_cfg_s_t gst_extbpc_cfg[16] = {0};
#define  EXT_BPC_MAX_NUM   16
ts_void snr_init_ext_bp(ts_void)
{
    ts_s32 s32_hfile_r;
    sfc_cfg_file_s_t st_sfc_file;
    s32_hfile_r = ts_lib_sfc_open(FN_SUPER_EXT_BP);
    ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08 *)&st_sfc_file, sizeof(sfc_cfg_file_s_t));
    if ((st_sfc_file.u32_valid_code !=SFC_FILE_VALID_CODE)|| (st_sfc_file.u32_valid_flag !=1)) 
    {
        printf("flash data invalid\n");
        ts_lib_sfc_close(s32_hfile_r);
        return;
    }
    ts_lib_sfc_read(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08 *)gst_extbpc_cfg, sizeof(gst_extbpc_cfg));
    ts_lib_sfc_close(s32_hfile_r);
    for (ts_s32 i = 0; i < EXT_BPC_MAX_NUM; i++) 
    {
        if (gst_extbpc_cfg[i].u32_en==1) {
            ts_s32 s32_add_ret;
            s32_add_ret = ts_tdk_add_extbpc(&gst_extbpc_cfg[i]);
            printf("s32_add_ret:%d\n",s32_add_ret);
        }
    }
    return;
}

//坏斑保存
ts_s32 save_ext_bp(ts_void)
{
    ts_s32 s32_hfile_r;
    ts_s32 s32_ret;
    sfc_cfg_file_s_t st_sfc_file;

    s32_hfile_r = ts_lib_sfc_open(FN_SUPER_EXT_BP);
    if (s32_hfile_r<=0) 
    {
        printf("open ext bp flash file fail!\n");
        return -1;
    }
    //读取原有头部信息 
    s32_ret = ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
    strcpy(st_sfc_file.ac_cfg_name, FN_SUPER_EXT_BP);
    st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
    st_sfc_file.u32_valid_flag = 1;

    s32_ret = ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
    s32_ret = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)gst_extbpc_cfg, sizeof(gst_extbpc_cfg));
    ts_lib_sfc_close(s32_hfile_r);
    if (s32_ret==sizeof(gst_extbpc_cfg)) {
        printf("save extbpc cfg success, total data len:%d\n", sizeof(gst_extbpc_cfg));
    } else {
        printf("save extbpc cfg fail ret:%d\n", s32_ret);
    }
    return 0;
}



TS_DECLARE_CMD(isp)
{
    ts_u32 u32_rcfg_size;
    ts_void *pv_def_cfg;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    u32_rcfg_size = ts_tdk_isp_get_cfg_size();

    if (!strcmp(argv[1], "start")){
        if (argc < 3) {
            printf("cmd fmt: isp start sensor \n");

            return 0;
        }

        start_isp(argv[2], 1, 1, 1);
        return 0;
    }

    if (!strcmp(argv[1], "set_sns")) {
        if (argc < 3) {
            printf("cmd fmt: isp set_sns sensor \n");
            return 0;               
        }

        ts_s32 s32_hfile_r;
        sfc_cfg_file_s_t st_sfc_file;
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_USR);
        if (s32_hfile_r > 0){
            ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
            strcpy(st_sfc_file.ac_cfg_name, argv[2]);
            ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            ts_lib_sfc_close(s32_hfile_r);
            printf("set sns done. \n");
        }else{
            printf("open file fail\n");
            return -1;
        }
        return 0;  
    }

    if (!strcmp(argv[1], "b_saved")) {
        //ts_tdk_snr_set_debug(TS_LOG_DEBUG);
        return ts_tdk_snr_load_bcfg_hdr(FN_B_CFG);
    }

    if (!strcmp(argv[1], "save_b_flag")){

        ts_s32 s32_oft1 = 0;
        ts_s32 s32_flag = 0;
        ts_s32 s32_temp = 0;
        if (argc > 3) {
            s32_oft1 = ts_cpl_str2num(argv[2]);
            s32_flag = ts_cpl_str2num(argv[3]);
            s32_temp = ts_cpl_str2num(argv[4]);
        }

        return ts_tdk_snr_write_bcfg_hdr(FN_B_CFG, s32_oft1, s32_flag, s32_temp);
    }

    if (!strcmp(argv[1], "nuc_db")) {
   
        ts_s32 s32_oft = 0;
        if (argc > 2)
        {
            s32_oft = ts_cpl_str2num(argv[2]);
        }

        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        st_nuc_attr.u16_coef = s32_oft & 0xffff;
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        return 0;
    }

    if (!strcmp(argv[1], "b_save")){
        ts_s32 s32_oft = 0;
        if (argc > 2)
            s32_oft = ts_cpl_str2num(argv[2]);

        tdk_snrcfg_shutter_close();
        ts_s32 s32_temp_1st = sns_get_temperature();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        ts_s32 s32_temp_2st = sns_get_temperature();
        tdk_snrcfg_shutter_open();

        ts_buffer_s_t st_mbuf;
        ts_tdk_isp_get_nuc_mbuf(TS_ISP_MDEV0, &st_mbuf);

        return ts_tdk_snr_write_bbuf(FN_B_CFG, s32_oft, s32_temp_1st, s32_temp_2st, st_mbuf.pu08_ptr, (s32_img_w * s32_img_h * 2));
    }

    /* cut相关 */
    if (!strcmp(argv[1], "set_sns")) {
        if (argc < 3) {
            printf("cmd fmt: isp set_sns sensor \n");

            return 0;               
        }

        ts_s32 s32_hfile_r;
        sfc_cfg_file_s_t st_sfc_file;
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_USR);
        if (s32_hfile_r > 0){
            ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
            strcpy(st_sfc_file.ac_cfg_name, argv[2]);
            ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            ts_lib_sfc_close(s32_hfile_r);
            printf("set sns done. \n");
        }  
    }


    if (!strcmp(argv[1], "rst_on")) {
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0xffffffff);

        return 0;
    }

    if (!strcmp(argv[1], "rst_off")) {
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);

        return 0;
    }

    if (!strcmp(argv[1], "freeze")) {
        ts_tdk_vpo_freeze(TS_VPO_MDEV0);

        return 0;
    }

    if (!strcmp(argv[1], "unfreeze")) {
        ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);

        return 0;
    }

    if ((!strcmp(argv[1], "cut_v")) || (!strcmp(argv[1], "cut"))) {
        
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(40);        

        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();

        ts_tdk_vpo_freeze(TS_VPO_MDEV0);

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
        ts_cpl_msleep(40);       
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst
        ts_cpl_msleep(40);       
        ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "cut_force")) {
        gst_snr_ctx.s32_force_cut = 1;
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ")) {
        
        printf("cut_occ begin  \n");
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xffffffff);////pause

        /*由于挡片后，图像整体亮度发生变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        ts_cpl_msleep(40);       
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////rst

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(40);   
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_cpl_msleep(40);   
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////rst

        ts_cpl_msleep(80);   
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0x8, 0);////pause

        ts_cpl_msleep(80);        
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);


        printf("cut_occ done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ_b")) {
   
        ts_s32 s32_frm_wait = 10;
        if (argc > 2)
        {
            s32_frm_wait = ts_cpl_str2num(argv[2]);
        }
        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff0);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息

        /*由于挡片后，图像整体亮度发生较大变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现 可以节省内存*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前

        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 解除冻结为agc前
        ts_cpl_msleep(1010 * s32_frm_wait/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);

        printf("cut_occ b done \n");
        return 0;
    }

    
    if (!strcmp(argv[1], "cut_occ_b_t")) {
   
        ts_s32 s32_frm_wait = 10;
        ts_s32 s32_oft = 0;

        if (argc > 3)
        {
            s32_frm_wait = ts_cpl_str2num(argv[2]);
            s32_oft = ts_cpl_str2num(argv[3]);

        }
        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff0);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息

        /*由于挡片后，图像整体亮度发生较大变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现 可以节省内存*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  

        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        st_nuc_attr.u16_frm_oft = (ts_u16)(s32_oft & 0xffff);
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        ts_cpl_msleep(1000/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。


        tdk_snrcfg_shutter_open();
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前

        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 解除冻结为agc前
        ts_cpl_msleep(1010 * s32_frm_wait/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);

        printf("cut_occ b done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ_bm")) {
   
        ts_s32 s32_frm_wait1 = 10;
        ts_s32 s32_frm_wait2 = 10;

        if (argc > 3)
        {
            s32_frm_wait1 = ts_cpl_str2num(argv[2]);
            s32_frm_wait2 = ts_cpl_str2num(argv[3]);
        }
        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff4);////复位fw，清除内部迭代信息
        //ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息

        /*由于挡片后，图像整体亮度发生较大变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现 可以节省内存*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_cpl_msleep(1000/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前
        ts_cpl_msleep(s32_frm_wait1 * 1000/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。

        //ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff4);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
        ts_cpl_msleep(s32_frm_wait2 * 1000/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。

        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);


        printf("cut_occ b done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_b")) {
   
        ts_s32 s32_frm_wait = 10;
        if (argc > 2)
        {
            s32_frm_wait = ts_cpl_str2num(argv[2]);
        }
        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff8);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息

        /*由于挡片后，图像整体亮度发生较大变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现 可以节省内存*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前
        tdk_snrcfg_shutter_open();

        //ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 解除冻结为agc前
        ts_cpl_msleep(1010 * s32_frm_wait/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);


        printf("cut_occ b done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ_o")) {
   
        static ts_s32 s32_sum_oft = 0;

        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
       // ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xffffffff); //算法暂停
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认冻结完成//后续接口需要修改成中断等待避免手动延迟

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认采集到挡上挡片的图像
        ts_viu_array_info_s_t st_viu_info;
        ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
        ts_s32 s32_mean_0 = st_viu_info.u32_mean;          //获取到挡片均值

        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        
        ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
        ts_s32 s32_mean_1 = st_viu_info.u32_mean;          //获取到挡片均值

        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 

        ts_s32 s32_diff = s32_mean_0 + s32_sum_oft - s32_mean_1;
        s32_sum_oft = s32_diff;
        ts_s32 s32_exp = s32_mean_1 + s32_diff;
        st_nuc_attr.u16_frm_oft = (ts_u16)(s32_diff & 0xffff);
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认生效

        tdk_snrcfg_shutter_open();
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认生效

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前
        //ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////开启算法

        printf("cut_occ_o done, last = %d, cut = %d, oft = %d, exp = %d\n", s32_mean_0, s32_mean_1, s32_sum_oft, s32_exp);
        return 0;
    }



    if (!strcmp(argv[1], "cut_occ_o2")) {
        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);
        //ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xffffffff); //算法暂停
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认冻结完成//后续接口需要修改成中断等待避免手动延迟

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认采集到挡上挡片的图像

        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        
        tdk_snrcfg_shutter_open();
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认生效
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前


        //ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////开启算法
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ_o1")) {
   
        ts_s32 s32_frm_wait = 10;
        if (argc > 2)
        {
            s32_frm_wait = ts_cpl_str2num(argv[2]);

        }
        /*由于挡片后，图像整体亮度发生变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }

        printf("cut_occ begin b, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);


        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps);   //确认冻结完成

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 冻结为agc前

        ts_cpl_msleep(s32_frm_wait / gst_snr_ctx.st_attr.s32_fps);   //确认冻结完成
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0); //算法暂停
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0); //算法暂停


        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);
        printf("cut_occ_o done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_occ_f")) {
   
        ts_s32 s32_frm_wait = 10;
        if (argc > 2)
        {
            s32_frm_wait = ts_cpl_str2num(argv[2]);

        }
        printf("cut_occ begin f, fps = %d  \n", gst_snr_ctx.st_attr.s32_fps);

        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xffffffff);////复位fw，清除内部迭代信息
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0xffffffff);////复位fw，清除内部迭代信息

        /*由于挡片后，图像整体亮度发生较大变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现 可以节省内存*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("1ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        s32_ret = ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("2ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(1000 * s32_frm_wait/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为7， 如果打挡片前后图像差异不大，可以适当减少。

        //数据流已经关闭，进行相应操作
        tdk_snrcfg_shutter_close();
        ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null); 
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为7， 如果打挡片前后图像差异不大，可以适当减少。

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 解除冻结为agc前

        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
        ts_cpl_msleep(1000 * s32_frm_wait/ gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);


        printf("cut_occ b done \n");
        return 0;
    }

    if (!strcmp(argv[1], "cut_o")) {
        
        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        return 0;
    }

    if (!strcmp(argv[1], "nuc_b")) {
        
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0); 
        return 0;    
    }

    if (!strcmp(argv[1], "cut_t")) {
        ts_s32 s32_wait_frm_cnt = 3;
        if (argc > 2)
            s32_wait_frm_cnt = ts_cpl_str2num(argv[2]);

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffffc);////暂停计算
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息
        ts_tdk_vpo_freeze(TS_VPO_MDEV0);

        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
        ts_cpl_msleep(1000 / gst_snr_ctx.st_attr.s32_fps); 

        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffff0);////暂停计算
        ts_cpl_msleep(1010 * s32_wait_frm_cnt / gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
        ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息

        ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);

        printf("cut_t %d over !\n", s32_wait_frm_cnt);
        return 0;
    }

    if (!strcmp(argv[1], "cut_n")) 
    {
        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_s32 s32_frm = 0;
        if (argc > 2)
        {
            ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
            s32_frm = st_nuc_attr.u08_stat_frm_num;
            st_nuc_attr.u08_stat_frm_num = ts_cpl_str2num(argv[2]);
            ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        }

        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
        ts_cpl_msleep(40);        

        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
        tdk_snrcfg_shutter_open();

        /*由于挡片后，图像整体亮度发生变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现*/
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        ts_isp_offline_frm_s_t st_frm;
        ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_offline_frm err !\n");
            return -1;
        }
        ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_send_offline_frm err !\n");
            return -1;
        }
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
        ts_cpl_msleep(40);       
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst
        ts_cpl_msleep(40);       
        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);

        if (argc > 2)
        {
            st_nuc_attr.u08_stat_frm_num = s32_frm;
            ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        }
         printf("cut_n %d over !\n", s32_frm);
        return 0;
    }

    if (!strcmp(argv[1], "cut_sffc")) {
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1);
        ts_cpl_msleep(20);        

        tdk_snrcfg_shutter_close();
        ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  

        isp_sw_calc_nuc_sffc(s32_img_w, s32_img_h, 0);
        tdk_snrcfg_shutter_open();
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);
        return 0;
    }


    /* nuc相关 */
    if (!strcmp(argv[1], "nuc_mode"))
    {
        if (argc < 4)
        {
            printf("isp nuc_mode mode state\n");
            return 0;  
        }
        ts_u32 s32_mode = ts_cpl_str2num(argv[2]);
        ts_u32 s32_stat = ts_cpl_str2num(argv[3]);

        /*设置成均值获取模式*/
        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_s32 s32_ret = ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, (ts_isp_nuc_attr_s_t *)&st_nuc_attr); 
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_attr error !\n");
            return -1;       
        }
        st_nuc_attr.en_mode = s32_mode;
        st_nuc_attr.en_stat_mode = s32_stat;

        s32_ret = ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, (ts_isp_nuc_attr_s_t *)&st_nuc_attr); 
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_set_nuc_attr err !\n");
            return -1;       
        }

        printf("isp set nuc_mode %d, %d \n", s32_mode, s32_stat);
        return 0;
    }

    if (!strcmp(argv[1], "nuc_buf"))
    {
        ts_buffer_s_t st_buf;

        ts_s32 s32_ret = ts_tdk_isp_get_nuc_mbuf(TS_ISP_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_mbuf err !\n");
            return -1;       
        }
        printf("m addr = 0x%x,size = [%d, %d]\n", st_buf.phys_addr, s32_img_w, s32_img_h);

        s32_ret = ts_tdk_isp_get_nuc_dbuf(TS_ISP_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_dbuf err !\n");
            return -1;       
        }
        printf("d addr = 0x%x,size = [%d, %d]\n", st_buf.phys_addr, s32_img_w, s32_img_h);
        return 0;
    }

    if (!strcmp(argv[1], "nuc_oft")) {
   
        ts_s32 s32_frm_oft;
        if (argc > 2)
            s32_frm_oft = ts_cpl_str2num(argv[2]);
        else
            s32_frm_oft = 0;

        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 

        st_nuc_attr.u16_frm_oft = (ts_u16)(s32_frm_oft & 0xffff);
        ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        return 0;
    }

    if (!strcmp(argv[1], "cali_buf"))
    {
        ts_buffer_s_t st_buf;

        ts_s32 s32_ret = ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_viu_get_correct_buf err !\n");
            return -1;       
        }
        printf("occ_buf = 0x%x,size = [%d]\n", st_buf.phys_addr, st_buf.sz_size);

        s32_ret = ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_kbuf err !\n");
            return -1;       
        }
        printf("k_buf = 0x%x,size = [%d]\n", st_buf.phys_addr, st_buf.sz_size);

        ts_isp_sffc_buf_s_t st_sffc_buf;
        st_sffc_buf.b_working = 1;
        ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, (ts_isp_sffc_buf_s_t *)&st_sffc_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_kbuf err !\n");
            return -1;       
        }
        printf("sffc addr = 0x%x,size = [%d]\n", st_sffc_buf.st_buf.phys_addr, st_sffc_buf.st_buf.sz_size);
        return 0;
    }


    if (!strcmp(argv[1], "nuc_deltab_test"))
    {
        if (argc < 3)
        {
            printf("cmd fmt: isp nuc_deltab_test val \n");       
            return 0;               
        }
        ts_s32 s32_b = ts_cpl_str2num(argv[2]);

        ts_buffer_s_t st_buf;
        ts_s32 s32_ret = ts_tdk_isp_get_nuc_mbuf(TS_ISP_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_mbuf err !\n");
            return -1;       
        }
        printf("m addr = 0x%x,size = [%d, %d]\n", st_buf.phys_addr, s32_img_w, s32_img_h);

        s32_ret = ts_tdk_isp_get_nuc_dbuf(TS_ISP_MDEV0, &st_buf);
        if (s32_ret != TS_OK)
        {
            printf("ts_tdk_isp_get_nuc_dbuf err !\n");
            return -1;       
        }
        printf("d addr = 0x%x,size = [%d, %d]\n", st_buf.phys_addr, s32_img_w, s32_img_h);
        
        ts_s16 *ps16_deltab = (ts_s16*)st_buf.phys_addr;
        for (ts_s32 i = 0; i < s32_img_w * s32_img_h; i++)
        {
            ps16_deltab[i] = s32_b;
        }
        ts_tdk_isp_flush_nuc_dbuf(TS_ISP_MDEV0, &st_buf);
        return 0;
    }

    /* 使能相关 */
    if (!strcmp(argv[1], "offline_on")) {

        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
        return 0;
    }

    if (!strcmp(argv[1], "offline_send")) {

        ts_isp_offline_frm_s_t st_frm;
        ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
        ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
        return 0;
    }

    if (!strcmp(argv[1], "offline_off")) {

        ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);
        return 0;
    }

    if (!strcmp(argv[1], "rst_on")) {
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0xffffffff);
        return 0;
    }

    if (!strcmp(argv[1], "rst_off")) {
        ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);
        return 0;
    }

    if (!strcmp(argv[1], "frz_on")) {
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1);
        return 0;
    }

    if (!strcmp(argv[1], "frz_off")) {
        ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);
        return 0;
    }

    if (!strcmp(argv[1], "d0")) {
        tdk_snrcfg_shutter_close();
        return 0;
    }

    if (!strcmp(argv[1], "d1")) {
        tdk_snrcfg_shutter_open();
        return 0;
    }

    if (!strcmp(argv[1], "run")) {
        gst_snr_ctx.s32_run_isp = 1;
        return 0;
    }

    if (!strcmp(argv[1], "pause")) {
        gst_snr_ctx.s32_run_isp = 0;
        return 0;
    }

    if (!strcmp(argv[1], "auto_cut_on")) {
        gst_snr_ctx.s32_run_auto_cut = 1;
        return 0;
    }

    if (!strcmp(argv[1], "auto_cut_off")) {
        gst_snr_ctx.s32_run_auto_cut = 0;
        return 0;
    }

    if (!strcmp(argv[1], "auto_fix_snc_off")) {
        gst_snr_ctx.s32_fix_snc_en = 0;
        return 0;
    }

    if (!strcmp(argv[1], "auto_fix_snc_on")) {
        gst_snr_ctx.s32_fix_snc_en = 1;
        return 0;
    }

    if (!strcmp(argv[1], "auto_b_on")) {
        gst_snr_ctx.s32_run_auto_b = 1;
        return 0;
    }

    if (!strcmp(argv[1], "auto_b_off")) {
        gst_snr_ctx.s32_run_auto_b = 0;
        return 0;
    }

    if (!strcmp(argv[1], "auto_b_status_l")) {
        gst_snr_ctx.s32_force_status= -1;
        return 0;
    }

    if (!strcmp(argv[1], "auto_b_status_h")) {
        gst_snr_ctx.s32_force_status= 1;
        return 0;
    }

    if (!strcmp(argv[1], "auto_b_status_n")) {
        gst_snr_ctx.s32_force_status= 0;
        return 0;
    }  

    if (!strcmp(argv[1], "auto_b_status_o")) {
        gst_snr_ctx.s32_force_status= 0x12345678;
        return 0;
    }  

    // if (!strcmp(argv[1], "calc_nuc")) {
    //     if (argc < 4) {
    //         printf("isp reg_w addr val \n");
    //         return 0;
    //     }
    //     ts_char *pc_fname_k = FN_K_CFG;
    //     ts_char *pc_fname_sffc = FN_SFFC_CFG;
    //     ts_u32 u32_file_k_size = FN_K_FILE_SIZE;
    //     ts_u32 u32_file_sffc_size = FN_SFFC_FILE_SIZE;
    //     ts_u32 s32_tmp = ts_cpl_str2num(argv[2]);
    //     ts_u32 s32_step = ts_cpl_str2num(argv[3]);
    //     isp_sw_calc_k_sffc(pc_fname_k, pc_fname_sffc, u32_file_k_size,
    //     u32_file_sffc_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
    // }

    // if (!strcmp(argv[1], "sffc_nr")) {
    //     ts_u32 s32_index = 0;
    //     ts_u32 s32_step = 0;
    //     ts_s32 s32_win_size = 2; // 滤波窗尺寸
    //     if (argc == 4) {
    //         s32_index = ts_cpl_str2num(argv[2]);
    //         s32_step = ts_cpl_str2num(argv[3]);
    //     }
    //     ts_char *pc_fname = FN_SFFC_CFG;
    //     ts_u32 u32_file_size = FN_SFFC_FILE_SIZE;
    //     isp_sw_calc_sffc_nr(pc_fname, u32_file_size, s32_win_size, s32_img_w,
    //     s32_img_h, s32_index, s32_step);
    // }

    /* 标定相关 */
    if (!strcmp(argv[1], "calc_k")) {
        if (argc < 4) {
            printf("isp reg_w addr val \n");
            return -1;  
        }
        ts_u32 s32_tmp = ts_cpl_str2num(argv[2]);
        ts_u32 s32_step = ts_cpl_str2num(argv[3]);
        ts_char *pc_fname = FN_K_CFG;
        ts_u32 u32_file_size = FN_K_FILE_SIZE;
        isp_sw_calc_k_nuc(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
        return 0;
    }

    if (!strcmp(argv[1], "calc_step_k_bp")) {
        if (argc < 4) {
            printf("isp calc_step_k grp step \n");
            return -1;  
        }
        ts_u32 s32_tmp = ts_cpl_str2num(argv[2]);
        ts_u32 s32_step = ts_cpl_str2num(argv[3]);

        ts_char *pc_fname = FN_K_CFG;
        ts_u32 u32_file_size = FN_K_FILE_SIZE;

        isp_sw_calc_k_nuc_step_bp(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
        return 0;
    }

    if (!strcmp(argv[1], "calc_step_k")) {
        if (argc < 4) {
            printf("isp calc_step_k grp step \n");
            return -1;  
        }
        ts_u32 s32_tmp = ts_cpl_str2num(argv[2]);
        ts_u32 s32_step = ts_cpl_str2num(argv[3]);

        ts_char *pc_fname = FN_K_CFG;
        ts_u32 u32_file_size = FN_K_FILE_SIZE;

        isp_sw_calc_k_nuc_step(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
        return 0;
    }

    if (!strcmp(argv[1], "calc_lens")) {
        if (argc < 4) {
            printf("isp reg_w addr val \n");
            return -1;  
        }
        ts_u32 s32_tmp = ts_cpl_str2num(argv[2]);
        ts_u32 s32_step = ts_cpl_str2num(argv[3]);

        ts_char *pc_fname = FN_K_CFG;
        ts_u32 u32_file_size = FN_K_FILE_SIZE;

        isp_sw_calc_k_lens(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
        return 0;
    }

    if (!strcmp(argv[1], "init_k")){
        printf("to init \n");
        ts_buffer_s_t st_kbuf;
        ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
        ts_u16 *pu16_ptr = (ts_u16 *)st_kbuf.pu08_ptr;
        ts_u32 u32_cnt = (ts_u32)(st_kbuf.sz_size >> 1);
        for (ts_s32 i = 0; i < u32_cnt; i++) { 
            pu16_ptr[i] = 0x4000;  
        }
        ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
        return 0;
    }


    if (!strcmp(argv[1], "occ_rst")) {
        ts_u32 u32_rst_val;

        if (argc > 2)
            u32_rst_val = ts_cpl_str2num(argv[2]);
        else
            u32_rst_val = 0x80;

        snr_exec_ooc(1, u32_rst_val, 0, 0);

        return 0;
    }


    if (!strcmp(argv[1], "occ")) {
        ts_u32 u32_ooc_val;
        if (3 == argc) {
            u32_ooc_val = ts_cpl_str2num(argv[2]);
        } else {
            u32_ooc_val = 0x80;
        }

        snr_exec_ooc(1, u32_ooc_val, 1, 1);
        return 0;
    }

    if (!strcmp(argv[1], "occ_d1")) {
        snr_exec_ooc(1, pst_attr->u32_ooc_rst_val, 0, 1);
        return 0;
    }

    if (!strcmp(argv[1], "occ_save")) {
        if (argc < 3) {
            printf("isp occ_save index \n");
            return -1;  
        }
        ts_u32 u32_index = ts_cpl_str2num(argv[2]);
        if ((u32_index + 1) * s32_img_w * s32_img_h + sizeof(sfc_cfg_file_s_t) >= FN_OCC_FILE_SIZE) {
            printf("wrong occ index \n");
            return -1;  
        }
        ts_buffer_s_t st_ooc_buf;
        ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, (ts_buffer_s_t *)&st_ooc_buf);
        sfc_cfg_file_s_t st_sfc_file;
        ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_OCC_CFG);
        if (s32_hfile_r > 0) {
            ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            strcpy(st_sfc_file.ac_cfg_name, FN_OCC_CFG);
            if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
                st_sfc_file.u32_cfg_num = 0;    
                st_sfc_file.u32_valid_flag = 0;            
            }
            st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
            st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
            for (ts_s32 i = 0; i < 32; i++) {
                st_sfc_file.u32_cfg_num = 0;
                if ((st_sfc_file.u32_valid_flag >> i) != 0) {
                    st_sfc_file.u32_cfg_num++;
                }
            }
            ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
            ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + st_ooc_buf.sz_size * u32_index, (ts_u08*)st_ooc_buf.pu08_ptr, st_ooc_buf.sz_size);

        }
        printf("save oc %d done\n", u32_index);
        return 0;
    }

    if (!strcmp(argv[1], "sffc_cali")) {
        // ts_u32 s32_index = 0;
        // if (argc >= 3) {
        //     s32_index = ts_cpl_str2num(argv[2]);
        // }

        ts_char *pc_fname = FN_SFFC_CFG;
        ts_u32 u32_file_size = FN_SFFC_FILE_SIZE;

        isp_sw_calc_sffc_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);

        return 0;
    }

    if (!strcmp(argv[1], "sffc_alg")) {
        ts_u32 s32_index = 0;
        if (argc >= 3) {
            s32_index = ts_cpl_str2num(argv[2]);
        }
        gst_snr_ctx.s32_sffc_alg = s32_index;
        return 0;
    }

    if (!strcmp(argv[1], "sffc_refresh")) {
        // ts_u32 s32_index = 0;
        // if (argc >= 3) {
        //     s32_index = ts_cpl_str2num(argv[2]);
        // }

        ts_char *pc_fname = FN_SFFC_CFG;
        ts_u32 u32_file_size = FN_SFFC_FILE_SIZE;

        isp_sw_calc_sffc_refresh_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);
        return 0;
    }

    /* 坏点相关 */
    if (!strcmp(argv[1], "set_bp")) {
        if (argc < 4){
            printf("cmd fmt: isp set_bp s32_x  s32_y \n");       
            return -1;               
        }
        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]);   
        isp_set_bp_pos(s32_x, s32_y);
        return 0;
    }
    
    if (!strcmp(argv[1], "del_bp")){
        if (argc < 4){
            printf("cmd fmt: isp del_bp s32_x  s32_y \n");       
            return -1;               
        }
        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]);  
        isp_delete_bp_pos(s32_x, s32_y);
        return 0;
    }
    if (!strcmp(argv[1], "set_ext_bp")) {
        ts_s32 s32_idx;
        ts_s32 s32_bad_x;
        ts_s32 s32_bad_y;
        ts_s32 s32_bad_w;
        ts_s32 s32_bad_h;
        ts_s32 s32_bad_center_x;
        ts_s32 s32_bad_center_y;
        ts_s32 s32_win_x_start;
        ts_s32 s32_win_y_start;
        ts_s32 s32_ret;
 
        const ts_s32 window_size = 9;
        const ts_s32 window_half = window_size / 2;
 
        if (argc < 9) {
            printf("cmd fmt: isp set_ext_bp2 idx en win_sz bad_x bad_y bad_w bad_h\n");
            return -1;
        }
 
        s32_idx = ts_cpl_str2num(argv[2]);
        if (s32_idx < 0 || s32_idx >= EXT_BPC_MAX_NUM) {
            printf("invalid idx %d, valid range: 0-%d\n",s32_idx, EXT_BPC_MAX_NUM - 1);
            return -1;
        }
 
        if ((ts_s32)s32_img_w < window_size ||(ts_s32)s32_img_h < window_size) {
            printf("image size is smaller than correction window\n");
            return -1;
        }
 
        s32_bad_x = ts_cpl_str2num(argv[5]);
        s32_bad_y = ts_cpl_str2num(argv[6]);
        s32_bad_w = ts_cpl_str2num(argv[7]);
        s32_bad_h = ts_cpl_str2num(argv[8]);
 
        if (s32_bad_x < 0 || s32_bad_y < 0 ||s32_bad_w<=0 || s32_bad_h<=0) {
            printf("invalid bad block: x=%d y=%d w=%d h=%d\n",s32_bad_x, s32_bad_y, s32_bad_w, s32_bad_h);
            return -1;
        }
 
        s32_bad_center_x = s32_bad_x + s32_bad_w / 2;
        s32_bad_center_y = s32_bad_y + s32_bad_h / 2;
 
        s32_win_x_start = s32_bad_center_x - window_half;
        s32_win_y_start = s32_bad_center_y - window_half;
 
        if (s32_win_x_start < 0) 
        {
            s32_win_x_start = 0;
        } 
        else if (s32_win_x_start >(ts_s32)s32_img_w - window_size) 
        {
            s32_win_x_start = (ts_s32)s32_img_w - window_size;
        }
        if (s32_win_y_start < 0) 
        {
            s32_win_y_start = 0;
        } 
        else if (s32_win_y_start >(ts_s32)s32_img_h - window_size) 
        {
            s32_win_y_start = (ts_s32)s32_img_h - window_size;
        }
 
        gst_extbpc_cfg[s32_idx].s32_idx = s32_idx;
        gst_extbpc_cfg[s32_idx].u32_en = (ts_u32)ts_cpl_str2num(argv[3]);
        gst_extbpc_cfg[s32_idx].u32_cor_win_sz = (ts_u32)ts_cpl_str2num(argv[4]);
 
        gst_extbpc_cfg[s32_idx].x = (ts_u16)s32_win_x_start;
        gst_extbpc_cfg[s32_idx].y = (ts_u16)s32_win_y_start;
 
        gst_extbpc_cfg[s32_idx].u16_x_m = (ts_u16)(s32_bad_x - s32_win_x_start);
        gst_extbpc_cfg[s32_idx].u16_y_m = (ts_u16)(s32_bad_y - s32_win_y_start);
 
        gst_extbpc_cfg[s32_idx].u16_size_x = (ts_u16)s32_bad_w;
        gst_extbpc_cfg[s32_idx].u16_size_y = (ts_u16)s32_bad_h;
 
        s32_ret = ts_tdk_add_extbpc(&gst_extbpc_cfg[s32_idx]);
 
        printf("extbpc[%d]: x=%u y=%u xm=%u ym=%u size=%ux%u ret=%d\n",
            s32_idx,
            gst_extbpc_cfg[s32_idx].x,
            gst_extbpc_cfg[s32_idx].y,
            gst_extbpc_cfg[s32_idx].u16_x_m,
            gst_extbpc_cfg[s32_idx].u16_y_m,
            gst_extbpc_cfg[s32_idx].u16_size_x,
            gst_extbpc_cfg[s32_idx].u16_size_y,
            s32_ret);
        return 0;
    }
    if (!strcmp(argv[1], "save_ext_bp")){
        ts_s32 s32_hfile_r;
        ts_s32 s32_ret;
        sfc_cfg_file_s_t st_sfc_file;
 
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_EXT_BP);
        if (s32_hfile_r<=0) 
        {
            printf("open ext bp flash file fail!\n");
            return -1;
        }
         //读取原有头部信息 
        s32_ret = ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        strcpy(st_sfc_file.ac_cfg_name, FN_SUPER_EXT_BP);
        st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
        st_sfc_file.u32_valid_flag = 1;
 
        s32_ret = ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        s32_ret = ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t), (ts_u08*)gst_extbpc_cfg, sizeof(gst_extbpc_cfg));
        ts_lib_sfc_close(s32_hfile_r);
        if (s32_ret==sizeof(gst_extbpc_cfg)) {
            printf("save extbpc cfg success, total data len:%d\n", sizeof(gst_extbpc_cfg));
        } else {
            printf("save extbpc cfg fail ret:%d\n", s32_ret);
        }
        return 0;
    }


    if (!strcmp(argv[1], "set_ext_bp1")) {
        if (argc < 10){
            printf("cmd fmt: isp set_bp s32_index, s32_en, s32_x, s32_y, s32_x_m, s32_y_m, s32_size_x, s32_size_y\n");       
            return -1;               
        }

        ts_extbpc_cfg_s_t st_cfg;
        ts_s32 s32_ret;

        st_cfg.s32_idx = ts_cpl_str2num(argv[2]);
        st_cfg.u32_en = (ts_u32)ts_cpl_str2num(argv[3]);

        st_cfg.x = (ts_u16)ts_cpl_str2num(argv[4]);
        st_cfg.y = (ts_u16)ts_cpl_str2num(argv[5]);   
        st_cfg.u16_x_m = (ts_u16)ts_cpl_str2num(argv[6]);
        st_cfg.u16_y_m = (ts_u16)ts_cpl_str2num(argv[7]);   
 
        st_cfg.u16_size_x = (ts_u16)ts_cpl_str2num(argv[8]);
        st_cfg.u16_size_y = (ts_u16)ts_cpl_str2num(argv[9]);   

        s32_ret = ts_tdk_add_extbpc(&st_cfg);

        return s32_ret;
    }

    if (!strcmp(argv[1], "set_ext_bp2")) {
        if (argc < 9) {
            printf("cmd fmt: isp set_ext_bp s32_index s32_en s32_win_sz, s32_bad_x, s32_bad_y, s32_bad_w, s32_bad_h\n");
            return -1;               
        }

        ts_extbpc_cfg_s_t st_cfg;
        ts_s32 s32_ret;

        const ts_s32 window_size = 9;
        const ts_s32 window_half = 4;

        ts_s32 s32_bad_x = ts_cpl_str2num(argv[5]);
        ts_s32 s32_bad_y = ts_cpl_str2num(argv[6]);
        ts_s32 s32_bad_w = ts_cpl_str2num(argv[7]);
        ts_s32 s32_bad_h = ts_cpl_str2num(argv[8]);

        ts_s32 s32_bad_center_x = s32_bad_x + s32_bad_w / 2;
        ts_s32 s32_bad_center_y = s32_bad_y + s32_bad_h / 2;

        ts_s32 s32_win_x_start = s32_bad_center_x - window_half;
        ts_s32 s32_win_y_start = s32_bad_center_y - window_half;

        s32_win_x_start = (s32_win_x_start < 0) ? 0 : s32_win_x_start;
        s32_win_x_start = (s32_win_x_start > (ts_s32)s32_img_w - window_size) ? (ts_s32)s32_img_w - window_size : s32_win_x_start;

        s32_win_y_start = (s32_win_y_start < 0) ? 0 : s32_win_y_start;
        s32_win_y_start = (s32_win_y_start > (ts_s32)s32_img_h - window_size) ? (ts_s32)s32_img_h - window_size : s32_win_y_start;


        st_cfg.s32_idx = ts_cpl_str2num(argv[2]);
        st_cfg.u32_en = (ts_u32)ts_cpl_str2num(argv[3]);
        st_cfg.u32_cor_win_sz = (ts_u32)ts_cpl_str2num(argv[4]);

        st_cfg.x = (ts_u16)s32_win_x_start;
        st_cfg.y = (ts_u16)s32_win_y_start;

        st_cfg.u16_x_m = (ts_u16)(s32_bad_x - s32_win_x_start);
        st_cfg.u16_y_m = (ts_u16)(s32_bad_y - s32_win_y_start);
  
        st_cfg.u16_size_x = (ts_u16)s32_bad_w;
        st_cfg.u16_size_y = (ts_u16)s32_bad_h;   

        s32_ret = ts_tdk_add_extbpc(&st_cfg);
        return s32_ret;
    }

    if (!strcmp(argv[1], "set_ext_bp3")) {
        if (argc < 7) {
            printf("cmd fmt: isp set_ext_bp3 s32_index s32_en s32_bad_cx s32_bad_cy s32_diameter\n");
            return TS_ERR;
        }

        ts_extbpc_cfg_s_t st_cfg;
        ts_s32 s32_ret;
        const ts_s32 window_size = 9;
        const ts_s32 window_half = 4;

        st_cfg.s32_idx = ts_cpl_str2num(argv[2]);
        st_cfg.u32_en = (ts_u32)ts_cpl_str2num(argv[3]);
        ts_s32 s32_bad_cx = ts_cpl_str2num(argv[4]); 
        ts_s32 s32_bad_cy = ts_cpl_str2num(argv[5]); 
        ts_s32 s32_diameter = ts_cpl_str2num(argv[6]);
        

        if ((s32_diameter < 1) || (s32_diameter > 5)) {
            printf("diameter must be 1~5 \n");
            return TS_ERR;
        }

        ts_s32 s32_bad_size = s32_diameter;  

        ts_s32 s32_bad_x = s32_bad_cx - (s32_diameter - 1) / 2;
        ts_s32 s32_bad_y = s32_bad_cy - (s32_diameter - 1) / 2;

        ts_s32 s32_bad_center_x = s32_bad_cx;
        ts_s32 s32_bad_center_y = s32_bad_cy;

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
        st_cfg.u16_size_x = (ts_u16)s32_bad_size;
        st_cfg.u16_size_y = (ts_u16)s32_bad_size;

        s32_ret = ts_tdk_add_extbpc(&st_cfg);

        return s32_ret;
    }

    if (!strcmp(argv[1], "del_ext_bp")){
        if (argc < 3){
            printf("cmd fmt: isp del_ext_bp  s32_index \n");       
            return -1;               
        }
        ts_s32 s32_index = ts_cpl_str2num(argv[2]);

        return ts_tdk_del_extbpc(s32_index);
    }

    if (!strcmp(argv[1], "ext_bp_proc")){
        ts_tdk_extbpc_status();

        return TS_OK;
    }

    if (!strcmp(argv[1], "ext_bp_pause")){
        ts_tdk_extbpc_pause();

        return TS_OK;
    }

    if (!strcmp(argv[1], "ext_bp_run")){
        ts_tdk_extbpc_run();

        return TS_OK;
    }

    if (!strcmp(argv[1], "ext_bp_dbg_on")) {
        ts_u32 u32_dbg_val;

        if (argc >= 3) {
            u32_dbg_val = ts_cpl_str2num(argv[2]);
        } else {
            u32_dbg_val = 0xffffffff;
        }

        ts_tdk_extbpc_set_bdg(1, u32_dbg_val);

        return TS_OK;
    }

    if (!strcmp(argv[1], "ext_bp_dbg_off")) {
        ts_tdk_extbpc_set_bdg(0, 0xffffffff);

        return TS_OK;
    }

    if (!strcmp(argv[1], "set_bp_line")){
        if (argc < 3){
            printf("cmd fmt: isp set_bp line \n");       
            return -1;               
        }
        ts_s32 s32_line = ts_cpl_str2num(argv[2]);
        isp_set_bp_line(s32_line);
        return 0;
    }

    if (!strcmp(argv[1], "del_bp_line")){
        if (argc < 3){
            printf("cmd fmt: isp del_bp line \n");       
            return -1;               
        }
        ts_s32 s32_line = ts_cpl_str2num(argv[2]);
        isp_delete_bp_line(s32_line);
        return 0;
    }

    if (!strcmp(argv[1], "set_bp_col")){
        if (argc < 3){
            printf("cmd fmt: isp set_bp col \n");       
            return -1;               
        }
        ts_s32 s32_col = ts_cpl_str2num(argv[2]);
        isp_set_bp_col(s32_col);
        return 0;
    }

    if (!strcmp(argv[1], "del_bp_col")){
        if (argc < 3){
            printf("cmd fmt: isp del_bp col \n");       
            return -1;               
        }
        ts_s32 s32_col = ts_cpl_str2num(argv[2]);
        isp_delete_bp_col(s32_col);
        return 0;
    }

    if (!strcmp(argv[1], "set_bp_block")) {
        if (argc < 6){
            printf("cmd fmt: isp set_bp s32_x s32_y s32_x_len s32_y_len\n");       
            return -1;               
        }
        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]); 
        ts_s32 s32_x_len = ts_cpl_str2num(argv[4]);   
        ts_s32 s32_y_len = ts_cpl_str2num(argv[5]);   
        isp_set_bp_block(s32_x, s32_y, s32_x_len, s32_y_len);
        return 0;
    }

    if (!strcmp(argv[1], "del_bp_block")) {
        if (argc < 6){
            printf("cmd fmt: isp del_bp s32_x s32_y s32_x_len s32_y_len\n");       
            return -1;               
        }
        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]); 
        ts_s32 s32_x_len = ts_cpl_str2num(argv[4]);   
        ts_s32 s32_y_len = ts_cpl_str2num(argv[5]);   
        isp_delete_bp_block(s32_x, s32_y, s32_x_len, s32_y_len);
        return 0;
    }

    if (!strcmp(argv[1], "read_bp_pos")) {

        ts_s32 s32_x = 0;
        ts_s32 s32_y = 0; 
        ts_s32 s32_x_len = s32_img_w;   
        ts_s32 s32_y_len = s32_img_h;

        if (argc >= 6){
            s32_x = ts_cpl_str2num(argv[2]);
            s32_y = ts_cpl_str2num(argv[3]); 
            s32_x_len = ts_cpl_str2num(argv[4]);   
            s32_y_len = ts_cpl_str2num(argv[5]);               
        }   
        isp_read_bp_pos(s32_x, s32_y, s32_x_len, s32_y_len);
        return 0;
    }

    if (!strcmp(argv[1], "bp2cfg")){
        ts_char *pc_fname = FN_K_CFG;//FN_SUPER_BP;
        isp_set_sync_bp(pc_fname);
        return 0;
    }
    
    if (!strcmp(argv[1], "bp_clr")){
        isp_clr_bp();
        return 0;
    }

    if (!strcmp(argv[1], "calc_bp")){

        if (argc > 4) {
            ts_s32 u32_hist_thr = ts_cpl_str2num(argv[2]);
            ts_s32 u32_diff_thr = ts_cpl_str2num(argv[3]);
            ts_s32 u32_flt_size = ts_cpl_str2num(argv[4]);
            ts_s32 u32_det_type = 0;
            ts_s32 u32_pos = 0;
            if (argc > 5)
                u32_det_type = ts_cpl_str2num(argv[5]);

            if (argc > 6)
                u32_pos = ts_cpl_str2num(argv[6]);

            isp_auto_bp_det(1,u32_hist_thr, u32_diff_thr, u32_flt_size, u32_det_type, u32_pos);
        } else {
            printf("isp calc_bp hist diff flt_size\n");
            return -1;
        }

        return 0;
    }

    if (!strcmp(argv[1], "save_bp")){
        ts_u32 u32_bp_cnt;
        ts_u08 *pu08_buf = (ts_u08 *)ts_isp_get_bp_pos_buf(0, &u32_bp_cnt);
        ts_u08 *pu08_dst = (ts_u08 *)ts_isp_get_bp_pos_addr(u32_bp_cnt);

        printf("bp cnt = %d \n", u32_bp_cnt);

        if (0 == u32_bp_cnt || ts_null == pu08_buf || ts_null == pu08_dst) {
            return 0;
        }

        ts_cpl_memcpy((ts_void *)pu08_dst, (ts_void *)pu08_buf, u32_bp_cnt << 2);

        return ts_isp_set_bp_multi_data();
    }

    if (!strcmp(argv[1], "save_bp_flash")){
        ts_char *pc_fname = FN_SUPER_BP;
        ts_s32 s32_ret = isp_save_bp_flash(pc_fname);
        if(s32_ret != 0)
        {
            printf("save_bp_flash err\n");
            return -1;
        }
        return 0;
    }

    if (!strcmp(argv[1], "bp2nuc")){
        ts_char *pc_fname = FN_SUPER_BP;
        ts_s32 s32_ret = isp_bp_flash_to_nuc_k(pc_fname);
        if(s32_ret != 0)
        {
            printf("bp2nuc err\n");
            return -1;
        }
        return 0;
    }

    if (!strcmp(argv[1], "clr_bp_flash")){
        ts_char *pc_fname = FN_SUPER_BP;
        ts_s32 s32_ret = isp_clr_bp_flash(pc_fname);
        if(s32_ret != 0)
        {
            printf("clr_bp_flash err\n");
            return -1;
        }
        return 0;
    }

    if (!strcmp(argv[1], "bp_off")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
        pst_reg->st_bpc.u08_en = 0;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        printf("bp off !\n");
        return 0;
    }

    if (!strcmp(argv[1], "bp_on")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
        pst_reg->st_bpc.u08_en = 1;
        pst_reg->st_bpc.u16_thr_brit = 16383;
        pst_reg->st_bpc.u16_thr_dark = 16383;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        printf("bp on !\n");
        return 0;
    }


    /* 寄存器相关 */
    if (!strcmp(argv[1], "reg_r")){
        if (argc < 4) {
            printf("isp reg_r addr cnt \n");
            return -1;  
        }
        ts_u32 u32_addr = ts_cpl_str2num(argv[2]);
        ts_u32 u32_cnt = ts_cpl_str2num(argv[3]);
        ts_u32 u32_reg;
        if (u32_cnt > 16){
            u32_cnt = 16;
        }

        printf("Address      Val(hex)\n");
        for (ts_u32 kk = 0; kk < u32_cnt; kk++) {
            ts_tdk_isp_read_reg(TS_ISP_MDEV0, u32_addr, &u32_reg);
            printf("[0x%08x]:0x%08x\n", u32_addr, u32_reg);
            u32_addr+=4;
        }
        return 0;
    }

    if (!strcmp(argv[1], "reg_w")) {
        if (argc < 4) {
            printf("isp reg_w addr val \n");
            return -1;  
        }
        ts_u32 u32_addr = ts_cpl_str2num(argv[2]);
        ts_u32 u32_val = ts_cpl_str2num(argv[3]);
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, u32_addr, u32_val);
        return 0;
    }

    if (!strcmp(argv[1], "get_status")){
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_status_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get status err");
            return -1;
        }
        fw_isp_status_s_t *pst_status = (fw_isp_status_s_t*)pu32_addr;
        printf("isp status : \n");
        printf("agc : mean : %d, noise: %d, range: [%d, %d] ->[%d, %d] \n", \
                                                pst_status->st_agc_status.s32_mean,\
                                                pst_status->st_agc_status.s32_noise, \
                                                pst_status->st_agc_status.u16_src_l, \
                                                pst_status->st_agc_status.u16_src_r, \
                                                pst_status->st_agc_status.u16_dst_l, \
                                                pst_status->st_agc_status.u16_dst_r);

        printf("sence : unisf : %d, mean_noise: %d, mean_val: %d, range: [%d, %d]\n", \
            pst_status->st_sence_status.st_sen_gbl.s32_unisf,\
            pst_status->st_sence_status.st_sen_gbl.s32_mean_noise,\
            pst_status->st_sence_status.st_sen_gbl.s32_mean_val,\
            pst_status->st_sence_status.st_sen_gbl.s32_min_val,\
            pst_status->st_sence_status.st_sen_gbl.s32_max_val);

        return 0;
    }

    if (!strcmp(argv[1], "reg2cfg")) {
        if (argc < 3) {
            printf("isp cfg save index \n");
            return -1;  
        }

        ts_u32 u32_index = ts_cpl_str2num(argv[2]);
        if (u32_index >= SFC_FILE_MAX_ISP_CFG_NUM) {
            u32_index = SFC_FILE_MAX_ISP_CFG_NUM - 1;
        }

        ts_u32 *pu32_addr, u32_len;

        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return 0;
        }

        ts_s32 s32_hfile_r;
        sfc_cfg_file_s_t st_sfc_file;
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_ISP);
        if (s32_hfile_r > 0) {
            ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
            st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);

            strcpy(st_sfc_file.ac_cfg_name, "isp cfg");
            if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
                st_sfc_file.u32_cfg_num = 0;    
                st_sfc_file.u32_valid_flag = 0;            
            }

            for (ts_s32 i = 0; i < 32; i++) {
               if ((st_sfc_file.u32_valid_flag >> i) != 0) {
                    st_sfc_file.u32_cfg_num++;
                }
            }
            ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + u32_rcfg_size * u32_index, (ts_u08*)pu32_addr, u32_rcfg_size);

            ts_lib_sfc_close(s32_hfile_r);
            printf("isp save %d isp cfg done. \n", u32_index);
        } 
        return 0; 
    }

    if (!strcmp(argv[1], "cfg2reg")) {
        if (argc < 3) {
            printf("isp cfg save index \n");
            return -1;  
        }

        ts_u32 u32_index = ts_cpl_str2num(argv[2]);
        ts_u32 *pu32_addr, u32_len;
        if (u32_index >= SFC_FILE_MAX_ISP_CFG_NUM) {
            u32_index = SFC_FILE_MAX_ISP_CFG_NUM - 1;
        }

        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }

        ts_s32 s32_hfile_r;
        sfc_cfg_file_s_t st_sfc_file;
        s32_hfile_r = ts_lib_sfc_open(FN_SUPER_ISP);
        if (s32_hfile_r > 0) {
            ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
            if ((SFC_FILE_VALID_CODE == st_sfc_file.u32_valid_code) && (st_sfc_file.u32_valid_flag & (1 << u32_index))) {
                ts_lib_sfc_read(s32_hfile_r,  sizeof(sfc_cfg_file_s_t) + u32_index * u32_rcfg_size, (ts_u08*)pu32_addr, u32_rcfg_size);
                printf("isp read %d isp cfg done. \n", u32_index);
            }
            
            ts_lib_sfc_close(s32_hfile_r);
            ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        }else{
            printf("open file fail\n");
            return -1;
        }
        return 0; 
    }

    if (!strcmp(argv[1], "def2reg")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }

        pv_def_cfg = ts_tdk_isp_get_def_cfg(TDK_ISP_DEFCFG_512P);
        memcpy(pu32_addr, pv_def_cfg, u32_rcfg_size);
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        printf("idef2re  done. \n");
        return 0;
    }

    if (!strcmp(argv[1], "reg_test")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
        printf("st_pipe reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_pipe, (ts_u32)&pst_reg->st_pipe - (ts_u32)pu32_addr);
        printf("st_statis reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_statis, (ts_u32)&pst_reg->st_statis - (ts_u32)pu32_addr);
        printf("st_agc_stat reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_agc_stat, (ts_u32)&pst_reg->st_agc_stat - (ts_u32)pu32_addr);
        printf("st_sence reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_sence, (ts_u32)&pst_reg->st_sence - (ts_u32)pu32_addr);
        printf("st_agc reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_agc, (ts_u32)&pst_reg->st_agc - (ts_u32)pu32_addr);
        printf("st_ltm reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_ltm, (ts_u32)&pst_reg->st_ltm - (ts_u32)pu32_addr);
        printf("st_nr2d_raw reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_nr2d_raw, (ts_u32)&pst_reg->st_nr2d_raw - (ts_u32)pu32_addr);
        printf("st_nr2d_yuv reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_nr2d_yuv, (ts_u32)&pst_reg->st_nr2d_yuv - (ts_u32)pu32_addr);
        printf("st_nr3d reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_nr3d, (ts_u32)&pst_reg->st_nr3d - (ts_u32)pu32_addr);
        printf("st_snc reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_snc, (ts_u32)&pst_reg->st_snc - (ts_u32)pu32_addr);
        printf("st_lce reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_lce, (ts_u32)&pst_reg->st_lce - (ts_u32)pu32_addr);
        printf("st_dde reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_dde, (ts_u32)&pst_reg->st_dde - (ts_u32)pu32_addr);
        printf("st_ee reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_ee, (ts_u32)&pst_reg->st_ee - (ts_u32)pu32_addr);
        printf("st_fpnc reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_fpnc, (ts_u32)&pst_reg->st_fpnc - (ts_u32)pu32_addr);
        printf("st_bpc reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_bpc, (ts_u32)&pst_reg->st_bpc - (ts_u32)pu32_addr);
        printf("st_drc reg = 0x%x, 0x%x \n", (ts_u32)&pst_reg->st_drc, (ts_u32)&pst_reg->st_drc - (ts_u32)pu32_addr);
        return 0;
    }

    if (!strcmp(argv[1], "corr_start")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }

        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        st_isp_en_ctx.u08_bpc_en = pst_reg->st_bpc.u08_en;
	    st_isp_en_ctx.u08_ltm_en = pst_reg->st_ltm.u08_en;
	    st_isp_en_ctx.u08_ee_en = pst_reg->st_ee.u08_en;
	    st_isp_en_ctx.u08_dde_en = pst_reg->st_dde.u08_en;
	    st_isp_en_ctx.u08_nr2d_raw_en = pst_reg->st_nr2d_raw.u08_enable;
	    st_isp_en_ctx.u08_nr2d_yuv_c0_en = pst_reg->st_nr2d_yuv.u08_c0_en;
	    st_isp_en_ctx.u08_nr2d_yuv_c1_en = pst_reg->st_nr2d_yuv.u08_c1_en;
	    st_isp_en_ctx.u08_snr_en = pst_reg->st_snc.u08_snc_en;
	    st_isp_en_ctx.u08_fpnc_en = pst_reg->st_fpnc.u08_en;
	    st_isp_en_ctx.u08_drc_en = pst_reg->st_drc.u08_en;
	    st_isp_en_ctx.u08_lce_en = pst_reg->st_lce.u08_en;
	    st_isp_en_ctx.u08_usr_adj_en = pst_reg->st_usr.u32_usr_adj_en;
	    st_isp_en_ctx.u16_thr_brit = pst_reg->st_bpc.u16_thr_brit;
	    st_isp_en_ctx.u16_thr_dark = pst_reg->st_bpc.u16_thr_dark;

        pst_reg->st_bpc.u08_en = 1;
        pst_reg->st_bpc.u16_thr_brit = 16383;
        pst_reg->st_bpc.u16_thr_dark = 16383;

        pst_reg->st_ltm.u08_en = 0;
        pst_reg->st_ee.u08_en = 0;
        pst_reg->st_dde.u08_en = 0;
        pst_reg->st_nr2d_raw.u08_enable = 0;
        pst_reg->st_nr2d_yuv.u08_c0_en = 0;
        pst_reg->st_nr2d_yuv.u08_c1_en = 0;
        pst_reg->st_snc.u08_snc_en = 0;
        pst_reg->st_fpnc.u08_en = 0;
        pst_reg->st_drc.u08_en = 0;
        pst_reg->st_lce.u08_en = 0;
        pst_reg->st_usr.u32_usr_adj_en = 0;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        gst_snr_ctx.s32_run_isp = 0;
        gst_snr_ctx.s32_run_auto_cut = 0;
        sffc_off();

        ts_cpl_msleep(1000);

        printf("you can start correction now!\n");
        return 0;
    }

    if (!strcmp(argv[1], "corr_end")) {
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }

        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_bpc.u08_en          =     st_isp_en_ctx.u08_bpc_en            ;
	    pst_reg->st_bpc.u16_thr_brit    =     st_isp_en_ctx.u16_thr_brit          ;
        pst_reg->st_bpc.u16_thr_dark    =     st_isp_en_ctx.u16_thr_dark          ;
        pst_reg->st_ltm.u08_en          =     st_isp_en_ctx.u08_ltm_en            ;
        pst_reg->st_ee.u08_en           =     st_isp_en_ctx.u08_ee_en             ;
        pst_reg->st_dde.u08_en          =     st_isp_en_ctx.u08_dde_en            ;
        pst_reg->st_nr2d_raw.u08_enable =     st_isp_en_ctx.u08_nr2d_raw_en       ;
        pst_reg->st_nr2d_yuv.u08_c0_en  =     st_isp_en_ctx.u08_nr2d_yuv_c0_en    ;
        pst_reg->st_nr2d_yuv.u08_c1_en  =     st_isp_en_ctx.u08_nr2d_yuv_c1_en    ;
        pst_reg->st_snc.u08_snc_en      =     st_isp_en_ctx.u08_snr_en            ;
        pst_reg->st_fpnc.u08_en         =     st_isp_en_ctx.u08_fpnc_en           ;
        pst_reg->st_drc.u08_en          =     st_isp_en_ctx.u08_drc_en            ;
        pst_reg->st_lce.u08_en          =     st_isp_en_ctx.u08_lce_en            ;
        pst_reg->st_usr.u32_usr_adj_en  =     st_isp_en_ctx.u08_usr_adj_en        ;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        gst_snr_ctx.s32_run_isp = 1;
        gst_snr_ctx.s32_run_auto_cut = 1;
        sffc_on_m(8000);

        ts_cpl_msleep(1000);

        printf("end correction!\n");
        return 0;
    }

    if (!strcmp(argv[1], "temp")) {
        ts_s32 s32_temp = sns_get_temperature();
        printf("sns temp = %d\n", s32_temp);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_trig")){
        ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_trig_s")){
        ts_s32 s32_flt_size = 20;
        if (argc >= 3) {
            s32_flt_size = ts_cpl_str2num(argv[2]);
        }

        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
        pst_reg->st_fix_snc.u08_mode = 0;
        pst_reg->st_fix_snc.u08_snc_h_en = 1;
        pst_reg->st_fix_snc.u08_snc_v_en = 1;
        pst_reg->st_fix_snc.u08_sta_frm_num = 64;
        pst_reg->st_fix_snc.u08_smooth = 0;
        pst_reg->st_fix_snc.u08_slice_num_h = 128;
        pst_reg->st_fix_snc.u08_slice_num_v = 128;

        pst_reg->st_fix_snc.u08_flt_h_size = s32_flt_size;
        pst_reg->st_fix_snc.u08_flt_v_size = s32_flt_size;
        pst_reg->st_fix_snc.u16_flt_h_thr = 111;
        pst_reg->st_fix_snc.u16_flt_v_thr = 111;
        pst_reg->st_fix_snc.u08_flt_step_h = 0;
        pst_reg->st_fix_snc.u08_flt_step_v = 0;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        ts_isp_ext_func_s_t st_ext_func;
        st_ext_func.pfn_fw_isp_get_raw = isp_get_raw_raw_sigle;
        ts_tdk_isp_set_ext_func(TS_ISP_MDEV0, &st_ext_func);
        ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_trig_t")){
        ts_s32 s32_flt_step = 1;
        if (argc >= 3) {
            s32_flt_step = ts_cpl_str2num(argv[2]);
        }

        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
        pst_reg->st_fix_snc.u08_mode = 0;
        pst_reg->st_fix_snc.u08_snc_h_en = 1;
        pst_reg->st_fix_snc.u08_snc_v_en = 1;
        pst_reg->st_fix_snc.u08_sta_frm_num = 64;
        pst_reg->st_fix_snc.u08_smooth = 0;
        pst_reg->st_fix_snc.u08_slice_num_h = 64;
        pst_reg->st_fix_snc.u08_slice_num_v = 64;

        pst_reg->st_fix_snc.u08_flt_h_size = 20;
        pst_reg->st_fix_snc.u08_flt_v_size = 10;
        pst_reg->st_fix_snc.u16_flt_h_thr = 111;
        pst_reg->st_fix_snc.u16_flt_v_thr = 111;
        pst_reg->st_fix_snc.u08_flt_step_h = s32_flt_step;
        pst_reg->st_fix_snc.u08_flt_step_v = s32_flt_step;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        ts_isp_ext_func_s_t st_ext_func;
        st_ext_func.pfn_fw_isp_get_raw = isp_get_raw_raw_sigle;
        ts_tdk_isp_set_ext_func(TS_ISP_MDEV0, &st_ext_func);
        ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_trig_f")){
        ts_s32 s32_flt_frm = 64;
        if (argc >= 3) {
            s32_flt_frm = ts_cpl_str2num(argv[2]);
        }

        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_mode = 0;
        pst_reg->st_fix_snc.u08_snc_h_en = 1;
        pst_reg->st_fix_snc.u08_snc_v_en = 1;
        pst_reg->st_fix_snc.u08_sta_frm_num = s32_flt_frm;
        pst_reg->st_fix_snc.u08_smooth = 0;
        pst_reg->st_fix_snc.u08_slice_num_h = 64;
        pst_reg->st_fix_snc.u08_slice_num_v = 64;

        pst_reg->st_fix_snc.u08_flt_h_size = 20;
        pst_reg->st_fix_snc.u08_flt_v_size = 10;
        pst_reg->st_fix_snc.u16_flt_h_thr = 111;
        pst_reg->st_fix_snc.u16_flt_v_thr = 111;
        pst_reg->st_fix_snc.u08_flt_step_h = 4;
        pst_reg->st_fix_snc.u08_flt_step_v = 4;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        ts_isp_ext_func_s_t st_ext_func;
        st_ext_func.pfn_fw_isp_get_raw = isp_get_raw_raw_sigle;
        ts_tdk_isp_set_ext_func(TS_ISP_MDEV0, &st_ext_func);
        ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);
        return 0;
    }


    if (!strcmp(argv[1], "fix_snc_trig_c")){
        ts_s32 s32_flt_frm = 64;
        if (argc >= 3) {
            s32_flt_frm = ts_cpl_str2num(argv[2]);
        }

        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_mode = 1;
        pst_reg->st_fix_snc.u08_snc_h_en = 1;
        pst_reg->st_fix_snc.u08_snc_v_en = 1;
        pst_reg->st_fix_snc.u08_sta_frm_num = s32_flt_frm;
        pst_reg->st_fix_snc.u08_smooth = 0;
        pst_reg->st_fix_snc.u08_slice_num_h = 64;
        pst_reg->st_fix_snc.u08_slice_num_v = 64;

        pst_reg->st_fix_snc.u08_flt_h_size = 20;
        pst_reg->st_fix_snc.u08_flt_v_size = 10;
        pst_reg->st_fix_snc.u16_flt_h_thr = 111;
        pst_reg->st_fix_snc.u16_flt_v_thr = 111;
        pst_reg->st_fix_snc.u08_flt_step_h = 4;
        pst_reg->st_fix_snc.u08_flt_step_v = 4;

        ts_tdk_isp_update_reg(TS_ISP_MDEV0);

        ts_isp_ext_func_s_t st_ext_func;
        st_ext_func.pfn_fw_isp_get_raw = isp_get_raw_raw_sigle;
        ts_tdk_isp_set_ext_func(TS_ISP_MDEV0, &st_ext_func);
        ts_tdk_isp_fix_snc_proc(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "reg_usr")) {
        
        if (argc < 4) {
            printf("isp reg_usr en value\n");
            printf("isp reg_usr luma value\n");
            printf("isp reg_usr ctr value\n");
            printf("isp reg_usr nr value\n");
            printf("isp reg_usr enh value\n");
            printf("isp reg_usr agc value\n");
            return 0; 
        }
        
            /*图像调节通过寄存器调节， 先获取isp寄存器*/
        ts_u32 *pu32_addr, u32_len;    
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t *)pu32_addr;
    
        char* param = argv[2];
        if(0 == strcmp(param, "en"))                             		//开启和关闭用户风格调节功能
        {
            ts_s32 s32_usr_en;
            s32_usr_en = ts_cpl_str2num(argv[3]);
        if(0 == s32_usr_en)
        {
            pst_isp_reg->st_usr.u32_usr_adj_en = 0;				/*该操作关闭功能*/
        }
        else
        {
            pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN; /*该操作开启功能*/
        }
        }
        else if(0 == strcmp(param, "luma"))
        {
            pst_isp_reg->st_usr.u08_luma_coef = ts_cpl_str2num(argv[3]);  /*用户亮度调节系数， U2.6，[0-255], 64为默认亮度， 值越大图像越亮， 否则越暗*/
        }
        else if(0 == strcmp(param, "ctr"))
        {
            pst_isp_reg->st_usr.u08_ctr_coef = ts_cpl_str2num(argv[3]);  /*用户对比度调节系数， U2.6，[0-255], 64为默认对比度， 值越大图像对比度越强， 否则越弱*/
        }
        else if(0 == strcmp(param, "nr"))
        {
            pst_isp_reg->st_usr.u08_nr_coef = ts_cpl_str2num(argv[3]);  /*用户降噪调节系数， U2.6，[0-255], 64为默认降噪强度， 值越大图像降噪越强， 否则越弱*/
        }
        else if(0 == strcmp(param, "enh"))
        {
            pst_isp_reg->st_usr.u08_enh_coef = ts_cpl_str2num(argv[3]); /*用户增强调节系数， U2.6，[0-255], 64为默认增强强度， 值越大图像增强越强， 否则越弱*/
        }
        else 
        {
            printf("wrong usr cfg !\n");
            return -1;
        }

        //注意：修改完参数后，需要以下操作才能生效
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        return 0;
    }
  
    if (!strcmp(argv[1], "fix_snc_on")){
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_snc_h_en = 1;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_off")){
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_snc_h_en = 0;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_fpnc_on")){
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_snc_v_en = 1;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_fpnc_off")){
        ts_u32 *pu32_addr, u32_len;
        if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
            printf("get isp reg error !\n");
            return -1;
        }
        ts_isp_cfg_reg_s_t *pst_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

        pst_reg->st_fix_snc.u08_snc_v_en = 0;
        ts_tdk_isp_update_reg(TS_ISP_MDEV0);
        return 0;
    }

    if (!strcmp(argv[1], "fix_snc_rst")){
        ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 1 << 15);
        printf("fix_snc_rst done !\n");
        return 0;
    }

    return 0;
}

/* sffc */
ts_void sffc_on_m(ts_s32 s32_v)
{
    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = s32_v;

    ts_s32 s32_ret = ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
    printf("sffc  init over, ret = %d !\n", s32_ret);
}

ts_void sffc_on_a(ts_void)
{
    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 3000;

    ts_s32 s32_ret = ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
    printf("sffc  init over, ret = %d !\n", s32_ret);
}

/* sffc */
ts_void sffc_off(ts_void)
{
    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 0;
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 0;

    ts_s32 s32_ret = ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
    printf("sffc  init over, ret = %d !\n", s32_ret);
}

ts_void sffc_cali(ts_void)
{
    ts_isp_sffc_cali_s_t        st_cali;
    ts_isp_sffc_cali_res_s_t    st_cali_res;

    st_cali.u08_sample_cnt = 4;
    ts_s32 s32_ret = ts_tdk_isp_run_sffc_cali(TS_ISP_MDEV0, &st_cali, &st_cali_res);
    if (s32_ret != TS_OK) {
        printf("ts_tdk_isp_run_sffc_cali err! \n");
        return;
    }
    printf("ts_tdk_isp_run_sffc_cali done!\n");
    printf("size = %d, addr = 0x%x \n", st_cali_res.u32_size, (ts_u32)st_cali_res.pv_virt_addr);

    ts_isp_sffc_buf_s_t st_sffc_buf;
    st_sffc_buf.b_working = 0;
    s32_ret = ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, &st_sffc_buf);
    printf("test sffc get buf s32_ret = %d, info size = %d, addr = 0x%x \n", s32_ret, (ts_u32)st_sffc_buf.st_buf.sz_size, (ts_u32)st_sffc_buf.st_buf.pu08_ptr);

    memcpy(st_sffc_buf.st_buf.pu08_ptr, (ts_void*)st_cali_res.pv_virt_addr, st_cali_res.u32_size);
    ts_tdk_isp_flush_sffc_buf(TS_ISP_MDEV0);
    printf("ts_tdk_isp_flush_sffc_buf ok !\n");
}


ts_void sffc_cali_mean(ts_s32 s32_frames)
{
    ts_isp_sffc_cali_s_t        st_cali;
    ts_isp_sffc_cali_res_s_t    st_cali_res;

    snr_attr_s_t *pst_attr;
    ts_s32 s32_img_w, s32_img_h;
    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
    ts_s32 s32_size = s32_img_w * s32_img_h;

    st_cali.u08_sample_cnt = 4;
    ts_u32 u32_sffc = (ts_u32)ts_cpl_tmm_malloc(ts_null, s32_size * 4, 32, "sffc");
    if (0 == u32_sffc)
    {
        printf("malloc cali sffc buf err \n");
        return;
    }

    ts_f32* pf32_sffc = (ts_f32* )u32_sffc;
    ts_cpl_memset(pf32_sffc, 0, s32_size * 4);

    tdk_snrcfg_shutter_close();
    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    tdk_snrcfg_shutter_open();

    for (ts_s32 i = 0; i < s32_frames; i++) {
        ts_s32 s32_ret = ts_tdk_isp_run_sffc_cali(TS_ISP_MDEV0, &st_cali, &st_cali_res);
        if (s32_ret != TS_OK) {
            printf("ts_tdk_isp_run_sffc_cali err! \n");
            return;
        }
        printf("ts_tdk_isp_run_sffc_cali done!\n");
        printf("size = %d, addr = 0x%x \n", st_cali_res.u32_size, (ts_u32)st_cali_res.pv_virt_addr);
        ts_s16* ps16_curr_data = (ts_s16*)st_cali_res.pv_virt_addr;
        for (ts_s32 j = 0; j < s32_size; j++)
        {
            pf32_sffc[j] = pf32_sffc[j] + ps16_curr_data[j];
        }
    }
    for (ts_s32 j = 0; j < s32_size; j++) {
        pf32_sffc[j] /= s32_frames; 
    }

    ts_isp_sffc_buf_s_t st_sffc_buf;
    st_sffc_buf.b_working = 0;
    ts_s32 s32_ret = ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, &st_sffc_buf);
    printf("test sffc get buf s32_ret = %d, info size = %d, addr = 0x%x \n", s32_ret, (ts_u32)st_sffc_buf.st_buf.sz_size, (ts_u32)st_sffc_buf.st_buf.pu08_ptr);

    ts_s16 *ps16_sffc = (ts_s16*)st_sffc_buf.st_buf.pu08_ptr;
    for (ts_s32 i = 0; i < s32_size; i++) {
        ps16_sffc[i] = pf32_sffc[i]; 
    }

    // ts_cpl_memcpy(st_sffc_buf.st_buf.pu08_ptr, (ts_void*)pf32_sffc, st_cali_res.u32_size);
    ts_tdk_isp_flush_sffc_buf(TS_ISP_MDEV0);
    printf("ts_tdk_isp_flush_sffc_buf ok !\n");

    ts_cpl_tmm_free((ts_void*)u32_sffc);
}

ts_void sffc_save(ts_void)
{
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
    ts_u32 s32_index = 0;
    // isp_sw_calc_sffc(s32_img_w, s32_img_h, s32_index);
    ts_isp_sffc_buf_s_t st_sffc_buf;
    st_sffc_buf.b_working = 1;
    ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, (ts_isp_sffc_buf_s_t *)&st_sffc_buf);

    ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
    if (s32_hfile_r > 0) {
        ts_u32 u32_index = s32_index;
        if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) > FN_SFFC_FILE_SIZE) {
            printf("wrong k index \n");
            return;  
        }

        sfc_cfg_file_s_t st_sfc_file;
        ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
        strcpy(st_sfc_file.ac_cfg_name, FN_SFFC_CFG);
        if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
            st_sfc_file.u32_cfg_num = 0;    
            st_sfc_file.u32_valid_flag = 0;            
        }

        st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
        st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
        for (ts_s32 i = 0; i < 32; i++) {
            st_sfc_file.u32_cfg_num = 0;
            if ((st_sfc_file.u32_valid_flag >> i) != 0) {
                st_sfc_file.u32_cfg_num++;
            }
        }
        ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
        ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + s32_img_w * s32_img_h  * 2 * u32_index, (ts_u08*)st_sffc_buf.st_buf.pu08_ptr, s32_img_w * s32_img_h  * 2);

    }
}

ts_void sffc_help(ts_void)
{
    printf("sffc -a \n");
    printf("sffc -m \n");
    printf("sffc -o \n");
    printf("sffc -c \n");
    printf("sffc -x 0|1\n");
    printf("sffc -b \n");
    printf("sffc -r \n");
    printf("sffc -n \n");
    printf("sffc -s \n");
    printf("sffc -f \n");
}


ts_s32 s32_sffc_run_flag1 = 1;

ts_void sffc_process(ts_void)
{
    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 1024;
    sffc_attr.u16_man_cor_v = 8192;
    ts_s32 s32_ret = ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
    printf("sffc  init over, ret = %d !\n", s32_ret);


    ts_isp_sffc_blk_info_s_t st_blk = {0};

    st_blk.u08_blk_size = 64;
    printf("u08_blk_size = %d\n", st_blk.u08_blk_size);

    s32_ret = ts_isp_set_sffc_usr_blk_info(TS_ISP_MDEV0, &st_blk);
    if (s32_ret != 0) {
        printf("set sffc blk failed! ret=%d\n", s32_ret);
    }

    ts_s32 s32_sffc_str = 8192;
    while (1)
    {
        if (0 == s32_sffc_run_flag1)
        {
            ts_cpl_msleep(100);  
            continue;   
        }
        
        ts_isp_sffc_statis_s_t st_stat = {0};

        ts_s32 s32_ret = ts_isp_get_sffc_usr_statis(TS_ISP_MDEV0, &st_stat);
        if (s32_ret != 0) {
            printf("sffc get_status_err\n ");
        }
        else
        {
            ts_s32 s32_status_cur = st_stat.s16_ratio;
            if (s32_status_cur < 4000)
            {
                s32_status_cur = 4000;
            }
            s32_sffc_str = s32_sffc_str * 200 + s32_status_cur * 56;
            s32_sffc_str = s32_sffc_str / 256;

            ts_isp_sffc_attr_s_t sffc_attr;
            sffc_attr.u08_enable = 1;
            sffc_attr.u08_en_man = 1;
            sffc_attr.u08_upd_speed = 32;
            sffc_attr.u16_flt_thr = 1024;
            sffc_attr.u16_man_cor_v = s32_sffc_str & 0xffff;
            ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
            // printf("sffc str = %d, ori = %d \n", s32_sffc_str, st_stat.s16_ratio);
        }

        ts_cpl_msleep(100);  
    }
}


extern ts_s32 s32_sffc_run_flag;
extern ts_void sffc_cali_hw_flt(ts_s32 s32_img_w, ts_s32 s32_img_h);
TS_DECLARE_CMD(sffc)
{
    ts_s32 s32_img_w, s32_img_h, s32_str = 0;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    if (argc > 1)  {
        if ('a' == argv[1][1]) {
            sffc_on_a();
        } else if ('m' == argv[1][1]) {

            if (argc > 2)
            {
                s32_str = ts_cpl_str2num(argv[2]);
            }
            sffc_on_m(s32_str);
        } else if ('o' == argv[1][1]) {
            sffc_off();
        } else if ('c' == argv[1][1]) {
            sffc_cali();
        } else if ('x' == argv[1][1]) {
            ts_s32 s32_status = 0;

            if (argc > 2)
            {
                s32_status = ts_cpl_str2num(argv[2]);
            }
            // s32_sffc_run_flag = s32_status;
            s32_sffc_run_flag1 = s32_status;

        } else if ('b' == argv[1][1]) {
            ts_s32 s32_blk = 0;

            if (argc > 2)
            {
                s32_blk = ts_cpl_str2num(argv[2]);
            }

            ts_isp_sffc_blk_info_s_t st_blk = {0};

            st_blk.u08_blk_size = s32_blk;
            printf("u08_blk_size = %d\n", st_blk.u08_blk_size);

            ts_s32 s32_ret = ts_isp_set_sffc_usr_blk_info(TS_ISP_MDEV0, &st_blk);

            if (s32_ret != 0) {
                printf("set sffc blk failed! ret=%d\n", s32_ret);
            }

        } else if ('w' == argv[1][1]) {
            // ts_s32 s32_ret;

            ts_isp_sffc_attr_s_t sffc_attr;
            sffc_attr.u08_enable = ts_cpl_str2num(argv[2]);
            sffc_attr.u08_en_man = ts_cpl_str2num(argv[3]);
            sffc_attr.u08_upd_speed = ts_cpl_str2num(argv[4]);
            sffc_attr.u16_flt_thr = ts_cpl_str2num(argv[5]);
            sffc_attr.u16_man_cor_v = ts_cpl_str2num(argv[6]);

            printf("sffc_attr.u08_enable   = %d\n", sffc_attr.u08_enable);
            printf("sffc_attr.u08_en_man   = %d\n", sffc_attr.u08_en_man);
            printf("sffc_attr.u08_upd_speed = %d\n", sffc_attr.u08_upd_speed);
            printf("sffc_attr.u16_flt_thr  = %d\n", sffc_attr.u16_flt_thr);
            printf("sffc_attr.u16_man_cor_v = %d\n", sffc_attr.u16_man_cor_v);

            ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 

        } else if ('r' == argv[1][1]) {
            ts_isp_sffc_statis_s_t st_stat = {0};

            ts_s32 s32_ret = ts_isp_get_sffc_usr_statis(TS_ISP_MDEV0, &st_stat);

            if (s32_ret == 0) {
                printf("sffc ratio = %d\n", st_stat.s16_ratio);
            }

        } else if ('n' == argv[1][1]) {
            ts_s32 s32_frames = 16;
            if (argc > 2)
            {
                s32_frames = ts_cpl_str2num(argv[2]);
            }
            sffc_cali_mean(s32_frames);
        } else if ('s' == argv[1][1]) {
            sffc_save();
        } else if ('f' == argv[1][1]) {
            sffc_cali_hw_flt(s32_img_w, s32_img_h);

            ts_u32 s32_index = 0;
           // isp_sw_calc_sffc(s32_img_w, s32_img_h, s32_index);
            ts_isp_sffc_buf_s_t st_sffc_buf;
            st_sffc_buf.b_working = 1;
            ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, (ts_isp_sffc_buf_s_t *)&st_sffc_buf);

            ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_SFFC_CFG);
            if (s32_hfile_r > 0) {
                ts_u32 u32_index = s32_index;
                if ((u32_index + 1) * s32_img_w * s32_img_h * 2 + sizeof(sfc_cfg_file_s_t) > FN_SFFC_FILE_SIZE) {
                    printf("wrong k index \n");
                    return 0;  
                }

                sfc_cfg_file_s_t st_sfc_file;
                ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file), sizeof(sfc_cfg_file_s_t));
                strcpy(st_sfc_file.ac_cfg_name, FN_SFFC_CFG);
                if (st_sfc_file.u32_valid_code != SFC_FILE_VALID_CODE) {
                    st_sfc_file.u32_cfg_num = 0;    
                    st_sfc_file.u32_valid_flag = 0;            
                }
    
                st_sfc_file.u32_valid_code = SFC_FILE_VALID_CODE;
                st_sfc_file.u32_valid_flag = st_sfc_file.u32_valid_flag | (1 << u32_index);
                for (ts_s32 i = 0; i < 32; i++) {
                    st_sfc_file.u32_cfg_num = 0;
                    if ((st_sfc_file.u32_valid_flag >> i) != 0) {
                        st_sfc_file.u32_cfg_num++;
                    }
                }
                ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&st_sfc_file),  sizeof(sfc_cfg_file_s_t));
                ts_lib_sfc_write(s32_hfile_r, sizeof(sfc_cfg_file_s_t) + s32_img_w * s32_img_h  * 2 * u32_index, (ts_u08*)st_sffc_buf.st_buf.pu08_ptr, s32_img_w * s32_img_h  * 2);
            }
        } else {
            sffc_help();
        }
    } else {
        sffc_help();
    }

    return 0;
}

