/*
 * ispfunction.c
 *
 *  Created on: 2025年6月9日
 *      Author: lyb
 */

#include "../myheader.h"
#include"ispfunction.h"
#include "snr_comm.h"
#include "votmenu.h"
#include "sffs_files_vot.h"
#include "votmenu.h"
#include "uapi/isp_reg.h"
#include <math.h>
#include <uapi/upl_v100.h>
#include <uapi/snr_v100.h>
#include "menu/votmain.h"
#include "menu/ispfunction.h"
#include <fontlib_vot.h>
#include <snr_comm_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
#include <uapi/alg_v100.h>


extern ts_s32 change_ooc_idx(ts_u08 u08_idx);

extern SysCtrlInfo_s_t global_sys_ctrl_info;

static ts_cpl_timeval_s_t st_tv_shutter_count_start, st_tv_shutter_count_cur;
// static ts_s32 s32_temp_shutter_count_cur;

extern snr_ctx_s_t gst_snr_ctx;

extern ts_u32 det_gos_switch_force_last;


ts_void ISP_set_TDC_node(ts_s32 s32_oft)
{
	ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	vision_log_warn("s32_oft [%d] \n" , s32_oft);
	if(SystemParaMenu2.m_temp_drift_comp_mode!=TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){
		vision_log_warn("mode [%d] it don't support! \n" , SystemParaMenu2.m_temp_drift_comp_mode);
		return;
	}

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

	tdk_snrcfg_shutter_close();
	ts_s32 s32_temp_1st = sns_get_temperature();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	ts_s32 s32_temp_2st = sns_get_temperature();
	tdk_snrcfg_shutter_open();

	ts_buffer_s_t st_mbuf;
	ts_tdk_isp_get_nuc_mbuf(TS_ISP_MDEV0, &st_mbuf);


	ts_tdk_snr_write_bbuf(FN_B_CFG, s32_oft, s32_temp_1st, s32_temp_2st, st_mbuf.pu08_ptr, (s32_img_w * s32_img_h * 2));

}

ts_void ISP_set_usr_mode_enalbe(ts_u32 enable)
{
	ts_u32 *pu32_addr, u32_len;

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	if(enable==1){
		pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
		pst_isp_reg->st_usr.u08_luma_coef = 64;  /*用户亮度调节系数， U2.6 标量，默认值是64,2的6次方*/
		pst_isp_reg->st_usr.u08_ctr_coef = 64;
		pst_isp_reg->st_usr.u08_nr_coef = 64;
		pst_isp_reg->st_usr.u08_enh_coef = 64;
		pst_isp_reg->st_usr.u08_agc_coef = 64;
	}
	else{
		pst_isp_reg->st_usr.u32_usr_adj_en = 0;
	}

	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

}


ts_void ISP_set_usr_cfg(ts_isp_cfg_reg_s_t *isp_cfg_input)
{

	ts_u32 *pu32_addr, u32_len;

	if(isp_cfg_input==NULL){
		return;
	}
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	memcpy(pst_isp_reg, isp_cfg_input,sizeof(ts_isp_cfg_reg_s_t));
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);
}

ts_void ISP_get_usr_cfg(ts_isp_cfg_reg_s_t *isp_cfg_output)
{
	ts_u32 *pu32_addr, u32_len;
	if(isp_cfg_output==NULL){
		return;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	memcpy(isp_cfg_output, pst_isp_reg, sizeof(ts_isp_cfg_reg_s_t));

}


ts_isp_cfg_reg_s_t* ISP_get_usr_cfg_ptr(void)
{
	ts_u32 *pu32_addr, u32_len;

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return NULL;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	return pst_isp_reg;
}

/**
 * @description: 设置DDE level值
 * @param {ts_u08} level：取值范围
 * @return {*}
 */
ts_u08 ISP_dde_level(ts_u08 level)
{
	ts_u32 *pu32_addr, u32_len;

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	ts_u08 lc_str=level;
	if(lc_str>255){
		lc_str = 254;
	}

	pst_isp_reg->st_dde.u08_en = 1;
	pst_isp_reg->st_dde.u08_lc_str = lc_str;
	// pst_isp_reg->st_dde.u16_det_flt_str = level;
	// pst_isp_reg->st_dde.u08_det_enh_str = level;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


ts_u08 ISP_dde_set_mode(ts_u08 u08_mode)
{
	ts_u32 *pu32_addr, u32_len;
	static ts_u08 u08_lc_str_bak; 
	static ts_u16 u16_lcl_flt_str_bak; 

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	
	if(u08_mode==0){
		pst_isp_reg->st_dde.u08_en = 0;
	}
	else if(u08_mode==1){
		pst_isp_reg->st_dde.u08_en = 1;
		u08_lc_str_bak = pst_isp_reg->st_dde.u08_lc_str;
		u16_lcl_flt_str_bak = pst_isp_reg->st_dde.u16_lcl_flt_str;
		pst_isp_reg->st_dde.u08_lc_str = 0;
		pst_isp_reg->st_dde.u16_lcl_flt_str = 0;

	}
	else if(u08_mode==2){
		pst_isp_reg->st_dde.u08_en = 1;
		pst_isp_reg->st_dde.u08_lc_str = u08_lc_str_bak;
		pst_isp_reg->st_dde.u16_lcl_flt_str = u16_lcl_flt_str_bak;
	}

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

ts_u08 ISP_dde_set_en(ts_u08 u08_en)
{
	ts_u32 *pu32_addr, u32_len;

	if(u08_en>1){
		u08_en = 1;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	pst_isp_reg->st_dde.u08_en = u08_en;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

ts_u08 ISP_snc_set_en(ts_u08 u08_en)
{
	ts_u32 *pu32_addr, u32_len;

	if(u08_en>1){
		u08_en = 1;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_snc.u08_snc_en=0x%04x\n", pst_isp_reg->st_snc.u08_snc_en);
	pst_isp_reg->st_snc.u08_snc_en = u08_en;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


/**
 * @description: 消除竖纹开关
 * @param {ts_u08} u08_en取值[0,1]
 * @return {*}
 */
ts_u08 ISP_fpnc_set_en(ts_u08 u08_en)
{
	ts_u32 *pu32_addr, u32_len;

	if(u08_en>1){
		u08_en = 1;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_fpnc.u08_en=0x%04x\n", pst_isp_reg->st_fpnc.u08_en);
	pst_isp_reg->st_fpnc.u08_en = u08_en;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

ts_void ISP_enable_drc( ts_u08 enable )
{
	ts_u32 *pu32_addr, u32_len;
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。

	
	if(enable){
		pst_isp_reg->st_drc.u08_en = 1;
	}
	else{
		pst_isp_reg->st_drc.u08_en = 0;
	}

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);
}

ts_void ISP_luma_and_ctr_by_drc( ts_s32 luma_value, ts_s32 ctr_value )
{
	ts_u32 *pu32_addr, u32_len;
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。

	if(luma_value>0 && luma_value<1024){
		pst_isp_reg->st_drc.u08_en = 1;
		pst_isp_reg->st_drc.u16_luma_adj = luma_value;
	}
	else{
		pst_isp_reg->st_drc.u08_en = 0;
	}
	
	if(ctr_value>0 && ctr_value<16384){
		pst_isp_reg->st_drc.u08_en = 1;
		pst_isp_reg->st_drc.u16_coef_adj = ctr_value;
	}
	else{
		pst_isp_reg->st_drc.u08_en = 0;
	}

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

}

/**
 * @description: 亮度对比度自动还是手动
 * @param {ts_u08} manual_flag , 1:手动，0：自动
 * @return {*}
 */
ts_void ISP_luma_and_ctr_set_manual(ts_u08 manual_flag, ts_u16 reset_luma_value, ts_u16 reset_ctr_value )
{
	ts_u32 *pu32_addr, u32_len;
	

	vision_log_debug("ISP_luma_and_ctr_set_manual  manual_flag=0x%04x\n", manual_flag);

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	// vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_drc.u16_luma_adj=0x%04x\n", pst_isp_reg->st_drc.u16_luma_adj);
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	if(reset_luma_value>0 && reset_luma_value<1024 ){
		pst_isp_reg->st_usr.u08_luma_coef = 64;  	
		pst_isp_reg->st_drc.u16_luma_adj = reset_luma_value; 
	}

	if(reset_ctr_value>0 && reset_ctr_value < 16384){
		pst_isp_reg->st_usr.u08_ctr_coef = 64;
		pst_isp_reg->st_drc.u16_coef_adj = reset_ctr_value;
	}

	pst_isp_reg->st_drc.u08_en = manual_flag;

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

}

/**
 * @brief:亮度调整
 * @parameter:取值范围请查看isp中，user和drc模块
 *
 */
ts_u08 ISP_luma_set(ts_u16 birt)
{
	ts_u32 *pu32_addr, u32_len;
	
	vision_log_debug("ISP_luma_set  birt=%d\n", birt);

	if(birt > (ts_u16)255){
		birt = 255;
		vision_log_debug("ISP_luma_set  birt=%d\n", birt);
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。

	if(pst_isp_reg->st_drc.u08_en==0){
		pst_isp_reg->st_drc.u08_en = 1;
		pst_isp_reg->st_drc.u16_luma_adj = 512;
		pst_isp_reg->st_drc.u16_coef_adj = 1024;
	}

	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_luma_coef = birt;  
	// vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_drc.u16_luma_adj=0x%04x\n", pst_isp_reg->st_drc.u16_luma_adj);

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


ts_u08 ISP_luma_set_by_drc(ts_u16 birt)
{
	ts_u32 *pu32_addr, u32_len;

	vision_log_debug("ISP_luma_set  birt=%d\n", birt);

	if(birt > (ts_u16)1023){
		birt = 1023;
		vision_log_debug("ISP_luma_set  birt=%d\n", birt);
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。

	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_luma_coef = 64;
	pst_isp_reg->st_drc.u08_en = 1;
	pst_isp_reg->st_drc.u16_luma_adj =  birt;
	// vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_drc.u16_luma_adj=0x%04x\n", pst_isp_reg->st_drc.u16_luma_adj);

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

/**
 * @brief:按百分比亮度调整
 * @parameter:
 *
 */
ts_u08 ISP_luma_set_by_percent(ts_u08 luma_percent_mode)
{
	ts_u16 max_birt =128;
	ts_u16 birt = 64;

	if(luma_percent_mode>=LUMA_PERCENT_MODE_BUTT){
		luma_percent_mode=LUMA_PERCENT_MODE_AUTO;
	}

	if(luma_percent_mode!=LUMA_PERCENT_MODE_AUTO){
		birt = luma_percent_mode*max_birt/100;
	}

	//针对indoor模式，100%模式进行限制
	if(birt>100){
		birt = 100;
	}

	ISP_luma_set(birt);
	
	return 0;
}

ts_u16 ISP_luma_get(void)
{
	ts_u32 *pu32_addr, u32_len;
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	// ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	
	// vision_log_debug("  birt=0x%04x\n",  pu32_addr->st_drc.u16_luma_adj);

	return 0;
}


/**
 * @brief:对比度调整
 * @parameter:ctr取值范围请查看isp中，user和drc模块
 *
 */
ts_u08 ISP_coef_set(ts_u16 ctr)
{
	ts_u32 *pu32_addr, u32_len;
	
	vision_log_debug("ISP_coef_set  ctr=0x%04x\n", ctr);

	if(ctr>(ts_u16)255){
		ctr = 255;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	if(pst_isp_reg->st_drc.u08_en==0){
		pst_isp_reg->st_drc.u08_en = 1;
		pst_isp_reg->st_drc.u16_luma_adj = 512;
		pst_isp_reg->st_drc.u16_coef_adj = 1024;
	}

	//所有的参数均通过 pst_isp_reg指针访问和修改。
	// vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_drc.u16_coef_adj=0x%04x\n", pst_isp_reg->st_drc.u16_coef_adj);
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_ctr_coef = ctr;
	vision_log_debug("u08_ctr_coef  ctr=%d \n", ctr);

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


ts_u08 ISP_coef_set_by_drc(ts_u16 ctr)
{
	ts_u32 *pu32_addr, u32_len;
	
	if(ctr>(ts_u16)2048){
		ctr = 2048;
	}

	vision_log_debug("ISP_coef_set  ctr=%d\n", ctr);
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	// vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_drc.u16_coef_adj=0x%04x\n", pst_isp_reg->st_drc.u16_coef_adj);
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_ctr_coef = 64;
	pst_isp_reg->st_drc.u08_en = 1;
	pst_isp_reg->st_drc.u16_coef_adj = ctr;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

/**
 * @brief:按百分比调整对比度
 * @parameter:
 *
 */
ts_u08 ISP_ctr_set_by_percent(ts_u08 ctr_percent_mode)
{
	ts_u16 max_ctr = 256;
	ts_u16 ctr = 64;

	if(ctr_percent_mode>=CONTRAST_PERCENT_MODE_BUTT){
		ctr_percent_mode=CONTRAST_PERCENT_MODE_AUTO;
	}

	if(ctr_percent_mode!=CONTRAST_PERCENT_MODE_AUTO){
		ctr = ctr_percent_mode*max_ctr/100;
	}

	// vision_log_warn("ctr_percent_mode %d ctr %d \n",ctr_percent_mode, ctr);
	ISP_coef_set(ctr);
	
	return 0;
}

ts_u16 ISP_coef_get(void)
{
	ts_u32 *pu32_addr, u32_len;
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}

	
	// ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	// vision_log_debug(" ctr=0x%04x\n", pst_isp_reg->st_drc.u16_coef_adj);
	return 0;
}

ts_void Isp_set_agc_loc_min_max(ts_u16 u16_ad_min, ts_u16 u16_ad_max)
{
	ts_u32 *pu32_addr, u32_len;
	
	if(u16_ad_min>16383){
		u16_ad_min = 16383;
	}

	if(u16_ad_max>16383){
		u16_ad_max = 16383;
	}

	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return ;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	pst_isp_reg->st_agc_v02.u16_loc_min = u16_ad_min;
	pst_isp_reg->st_agc_v02.u16_loc_max = u16_ad_max;

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);
}

/**
 * @brief:伪彩
 * @parameter:uvya
 *
 */
ts_u08 ISP_palette_set(ts_yuv_s_t *yuv_tab)
{
    ts_vpo_pcolor_attr_s_t st_pcolor_attr;
	ts_isp_isotherm_attr_s_t st_iso;
	
	vision_log_debug("ISP_palette_set\n");
	st_iso.u08_enable = 0;
	ts_tdk_isp_set_isotherm_attr(TS_ISP_MDEV0, &st_iso);
	
	st_pcolor_attr.u32_enable = 1;
	st_pcolor_attr.pst_pcolor = (ts_yuv_s_t *)yuv_tab;
	st_pcolor_attr.en_mode = TS_VPO_PCOLOR_MODE_NORMAL;
	st_pcolor_attr.en_roi_mode = TS_VPO_PCOLOR_ROI_CLOSE;

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
	return 0;
}

/**
 * @brief:设置降噪
 * @parameter:luma: 0:为不降噪，大于0为降噪，值越大，降噪越强,[0 -255]
 *
 */
ts_u08 ISP_nr_set(ts_u16 nr)
{
	ts_u32 *pu32_addr, u32_len;
	
	vision_log_debug("ISP_nr_set  nr=0x%02x\n", nr);

	if(nr>255){
		nr = 255;
	}
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_nr_coef = nr;

	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


ts_u08 ISP_enhance_set(ts_u16 enhance)
{
	ts_u32 *pu32_addr, u32_len;

	if(enhance>255){
		enhance = 255;
	}
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_enh_coef = enhance;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}


/**
 * @description: 配置usr模块的调光
 * @param {*}
 * @return {*}
 */
ts_u08 ISP_agc_luma_set(ts_u08 agc_luma)
{
	ts_u32 *pu32_addr, u32_len;
	

	if(agc_luma>255){
		agc_luma = 255;
	}
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	pst_isp_reg->st_usr.u32_usr_adj_en = ISP_USR_ADJ_EN;
	pst_isp_reg->st_usr.u08_agc_coef = agc_luma;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

ts_void ISP_ee_enhance_mode_set(ts_u08 ee_enhance_mode, ts_u08 store_flag)
{
	static ts_u08 white_enhance=0;
	static ts_u08 black_enhance=0;

	ts_u32 *pu32_addr, u32_len;
	
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
	}
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;

	if(store_flag==1){
		white_enhance = pst_isp_reg->st_ee.u08_str_pos;
		black_enhance = pst_isp_reg->st_ee.u08_str_neg;
	}

	if(ee_enhance_mode == EE_ENHANCE_MODE_OFF){
		pst_isp_reg->st_ee.u08_str_pos = white_enhance;
		pst_isp_reg->st_ee.u08_str_neg = black_enhance;
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_WHITE_BLACK){
		pst_isp_reg->st_ee.u08_str_pos = 255;
		pst_isp_reg->st_ee.u08_str_neg = 255;
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_WHITE){
		pst_isp_reg->st_ee.u08_str_pos = 255;
		pst_isp_reg->st_ee.u08_str_neg = 0;
	}
	else if(ee_enhance_mode == EE_ENHANCE_MODE_BLACK){
		pst_isp_reg->st_ee.u08_str_pos = 0;
		pst_isp_reg->st_ee.u08_str_neg = 255;
	}

	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);
}


/**
 * @brief:设置增强
 * @parameter:ee:256为不增强，值越大增强越强
 *
 */
ts_u08 ISP_ee_set(ts_u16 ee)
{
	ts_u32 *pu32_addr, u32_len;
	
	vision_log_debug("ISP_ee_set  ee=0x%04x\n", ee);
	//通过如下函数获取到isp寄存器及地址。
	if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
	{
		vision_log_debug("get isp reg error !\n");
		return 0;
	}

	if(ee>255){
		ee = 255;
	}
	
	ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t*)pu32_addr;
	//所有的参数均通过 pst_isp_reg指针访问和修改。
	vision_log_debug("ts_tdk_isp_get_reg_addr  pst_isp_reg->st_nr2d_raw.u16_enh_str=0x%04x\n", pst_isp_reg->st_nr2d_raw.u16_enh_str);
	pst_isp_reg->st_ee.u08_str_pos = ee;
	
	//注意：修改完参数后，需要以下操作才能生效
	ts_tdk_isp_update_reg(TS_ISP_MDEV0);

	return 0;
}

ts_void ISP_fix_snc_switch_set(ts_u08 switch_en)
{
	if(switch_en == VOT_SWITCH_ON){
		gst_snr_ctx.s32_fix_snc_en = 1;
	}
	else{
		gst_snr_ctx.s32_fix_snc_en = 0;
	}

}

/**
 * @brief:设置电子放大
 * @parameter:zoom:
 *
 */
ts_u08 	ISP_continuous_zoom_set(ts_u16 zoom)
{
    ts_vpo_zoom_attr_s_t st_attr;
	vision_log_debug("ISP_continuous_zoom_set  zoom=%d\n", zoom);
	if(zoom<10 || zoom>100){
		return -1;
	}

    ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    st_attr.en_type = TS_VPO_ZOOM_CENTER;
	st_attr.u16_ratio = zoom;
	st_attr.u08_hmirr_en = UsermenuParameter.Mirror&0x1;
	st_attr.u08_vmirr_en = (UsermenuParameter.Mirror>>1)&0x1;

	ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);
    
#ifdef DEBUG_XKQ
    vision_log_debug("current zoom = %d\n", zoom);
#endif // DEBUG
	return 0;
}

/**
 * @brief:设置电子放大
 * @parameter:zoom:
 *
 */
ts_void ISP_zoom_set(ts_u16 zoom)
{
	ts_vpo_zoom_attr_s_t st_attr;
	vision_log_debug("ISP_zoom_set  zoom=0x%04x\n", zoom);
	if(zoom<0 || zoom>10 ){
		vision_log_debug("error ISP_zoom_set  zoom=0x%04x\n", zoom);
		return;
	}

	ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    st_attr.en_type = TS_VPO_ZOOM_CENTER;
	st_attr.u16_ratio = zoom*10;
	st_attr.u08_hmirr_en = UsermenuParameter.Mirror&0x1;
	st_attr.u08_vmirr_en = (UsermenuParameter.Mirror&0x2)>>1;

	ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);

	// ts_vpo_grp_attr_s_t st_attr;
	// ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
	
	// if(zoom == 1)
	// {
	// 	st_attr.u16_cut_width = 640;
	// 	st_attr.u16_cut_height = 512;
	// 	st_attr.u16_cut_start_h = 0;
	// 	st_attr.u16_cut_start_v = 0;
	// }
	// else if(zoom == 2)
	// {
	// 	st_attr.u16_cut_width = 320;
	// 	st_attr.u16_cut_height = 256;
	// 	st_attr.u16_cut_start_h = 160;
	// 	st_attr.u16_cut_start_v = 128;
	// }
	// else if(zoom == 4)
	// {
	// 	st_attr.u16_cut_width = 160;
	// 	st_attr.u16_cut_height = 128;
	// 	st_attr.u16_cut_start_h = 240;
	// 	st_attr.u16_cut_start_v = 192;
	// }
	// else if(zoom == 8)
	// {
	// 	st_attr.u16_cut_width = 80;
	// 	st_attr.u16_cut_height = 64;
	// 	st_attr.u16_cut_start_h = 280;
	// 	st_attr.u16_cut_start_v = 224;
	// }

	// ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, &st_attr);
	// ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
}


ts_void ISP_vot_zoom_set(ts_u08 Enlarge)
{

	if(UsermenuParameter.m_pip_mode != PIP_MODE_NONE){
		return;
	}

#ifdef SUPPORT_CONTINUOUS_ZOOM
	if (Enlarge >= 10 && Enlarge <= 100)
	{
		UsermenuParameter.Enlarge = Enlarge;
		ISP_continuous_zoom_set(UsermenuParameter.Enlarge);
		vision_log_debug("SUPPORT_CONTINUOUS_ZOOM\n");
	}
#else
	if (Enlarge >= 0x00 && Enlarge <= 0x03)
	{
		UsermenuParameter.Enlarge = Enlarge;
		ISP_zoom_set(1 << UsermenuParameter.Enlarge);
	}
#endif

}
/**
 * @brief:设置图像镜像
 * @parameter:mirr:
 *
 */
ts_u08 ISP_mirr_set(ts_u16 mirr)
{

	vision_log_debug("ISP_mirr_set  mirr=0x%04x\n", mirr);
	vot_vpo_freeze();
#ifndef SUPPORT_CONTINUOUS_ZOOM
	ts_vpo_grp_attr_s_t st_attr;
	ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
	st_attr.u08_hmirr_en = mirr&0x1;
	st_attr.u08_vmirr_en = (mirr&0x2)>>1;
	ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, &st_attr);
	ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
#else
	ts_u08 zoom = UsermenuParameter.Enlarge;
 	ts_vpo_zoom_attr_s_t st_attr;
	if(zoom<10 || zoom>100){
		return -1;
	}
    ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    st_attr.en_type = TS_VPO_ZOOM_CENTER;
	st_attr.u16_ratio = zoom;
	st_attr.u08_hmirr_en =  mirr&0x1;
	st_attr.u08_vmirr_en =  (mirr&0x2)>>1;
	ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);
#endif
	vot_vpo_unfreeze();

	return 0;
}

/**
 * @brief:
 * @parameter:shuttermode: 0:off 1:auto 2:temp
 *
 */
ts_u08 ISP_shuttermode(ts_u08 shuttermode)
{
	vision_log_debug("ISP_shuttermode  shuttermode=%d\n", shuttermode);

	return 0;
}



ts_u08 ISP_get_ShutterMode(void)
{
	ts_u08 ShutterMode = SHUTTER_MODE_OFF; 
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	ShutterMode =UsermenuParameter.ShutterMode;
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	return ShutterMode;
}


/**
 * @description: 获取用户的实际打档间隔
 * @param {void}
 * @return {ts_s32} 毫秒
 */
ts_s32 ISP_get_shutter_timer_gap(void)
{
	ts_u64  diff_delta = UsermenuParameter.AutoShutter*30*1000;
	return (ts_s32)diff_delta;
}

ts_u08 ISP_get_output_fps(void)
{
    ts_u08 m_device_fps = 50;

    if(SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_50FPS){
		m_device_fps = 50;
	}
	else if(SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_25FPS){
		m_device_fps = 25;
	}
	else if(SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_10FPS){
		m_device_fps = 10;
	}
	else if(SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_5FPS){
		m_device_fps = 5;
	}
	else{
		m_device_fps = 50;
	}

	// if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI 
	// 	||  SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656
	// 	||  SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601){
		
	// 	vision_log_warn("SystemParaMenu2.m_video_out_mode  %d  \n",SystemParaMenu2.m_video_out_mode );
	// 	if( (SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_20FPS 
	// 		|| SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_30FPS 
	// 		|| SystemParaMenu2.m_device_fps == DEVICE_FPS_MODE_40FPS ))
	// 	{
	
	// 		SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_50FPS;
	// 		m_device_fps = 50;
			
	// 	}
	// }
	#ifdef CONFIG_SNR_PLANCK1280C
	{
			SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_25FPS;
			m_device_fps = 25;
	}
	#endif
    return m_device_fps;
}


ts_u16 ISP_get_det_ad_value(void)
{
	return (ts_u16)global_sys_ctrl_info.m_det_temp_ad_value;
}

ts_void ISP_auto_shutter_process(ts_void)
{
	static  ts_u16 last_temp_ad_value=0;
	static ts_u64 run_delta = 1*60*1000; //上电三分钟前，15秒打档一次
	static ts_s32 us_delta = 15*1000;
	ts_s32 diff_us = 0;
	ts_u64 do_shutter_flag=0;
	ts_s32 sys_run_time = 0;
	static ts_cpl_timeval_s_t  st_tv_shutter_count_last;


	//打档开关优先级以第一
	if(UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF)
	{//关闭
		// vision_log_debug(" SHUTTER_MODE_OFF \n");
		ts_cpl_gettimeofday(&st_tv_shutter_count_last);
		return;
	}

	ts_cpl_gettimeofday(&st_tv_shutter_count_cur);

	sys_run_time = ((st_tv_shutter_count_cur.tv_sec - global_sys_ctrl_info.m_tv_start.tv_sec) * 1000000 + st_tv_shutter_count_cur.tv_usec - global_sys_ctrl_info.m_tv_start.tv_usec) / 1000;
          

	//开机三分钟内，15秒打档，3分钟后，按照用户逻辑打档
	if(sys_run_time < run_delta){
		diff_us = ((st_tv_shutter_count_cur.tv_sec - st_tv_shutter_count_start.tv_sec) * 1000000 + st_tv_shutter_count_cur.tv_usec - st_tv_shutter_count_start.tv_usec) / 1000;
 
		if(diff_us > us_delta){
			do_shutter_flag = 1;
			st_tv_shutter_count_start = st_tv_shutter_count_cur;
			// vision_log_debug(" start system shutter \n");
		}
	}
	else{

		// vision_log_debug("ISP_shuttermode  UsermenuParameter.ShutterMode=%d UsermenuParameter.AutoShutter %d \n", UsermenuParameter.ShutterMode,UsermenuParameter.AutoShutter);

		if(UsermenuParameter.ShutterMode == SHUTTER_MODE_TIMER && UsermenuParameter.AutoShutter>0)
		{//自动
			
			// vision_log_debug(" SHUTTER_MODE_TIMER  UsermenuParameter.AutoShutter %d \n",UsermenuParameter.AutoShutter);
			ts_cpl_gettimeofday(&st_tv_shutter_count_cur);
	
            ts_s32 s32_time_diff = ((st_tv_shutter_count_cur.tv_sec - st_tv_shutter_count_last.tv_sec) * 1000000 + st_tv_shutter_count_cur.tv_usec - st_tv_shutter_count_last.tv_usec) / 1000;
          
		  	if(UsermenuParameter.AutoShutter <=0){
				UsermenuParameter.AutoShutter = 1;
			}
			ts_s32 us_delta = UsermenuParameter.AutoShutter*30*1000;
			
			//达到打档周期
			if(us_delta<s32_time_diff){
				// vision_log_debug(" ShutterMode=%d  us_delta %d , s32_time_diff  %d\n", UsermenuParameter.ShutterMode, us_delta , s32_time_diff);
				// vision_log_debug("SHUTTER_MODE_TIMER UsermenuParameter.AutoShutter %ld, us_delta %ld ",UsermenuParameter.AutoShutter, us_delta );
				// ISP_cut();
				do_shutter_flag = 1;
			}
			
		}
	}


	//只要开启打档，就要需要考虑温度打档
	if(UsermenuParameter.ShutterMode != SHUTTER_MODE_OFF)
	{//温度
		vot_read_det_ad();
		// vision_log_debug("last_temp_ad_value %d, m_det_temp_ad_value %d \n ",last_temp_ad_value, global_sys_ctrl_info.m_det_temp_ad_value );
		ts_u16 temp_ad_value  = abs(global_sys_ctrl_info.m_det_temp_ad_value - last_temp_ad_value);
		if(temp_ad_value>7){
			do_shutter_flag = 1;
			// vision_log_debug("last_temp_ad_value %d, m_det_temp_ad_value %d  \n",last_temp_ad_value, global_sys_ctrl_info.m_det_temp_ad_value );
			last_temp_ad_value = global_sys_ctrl_info.m_det_temp_ad_value;
		}

	}

	if(do_shutter_flag){
		ISP_cut();
		ts_cpl_gettimeofday(&st_tv_shutter_count_last);  
	}
	
}


ts_void ISP_force_cut(ts_s32 s32_force_cut)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	gst_snr_ctx.s32_force_cut = s32_force_cut;
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}


ts_void ISP_set_auto_cut_flag(ts_s32 cut_flag)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	gst_snr_ctx.s32_run_auto_cut = cut_flag;

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

}


ts_s32 ISP_get_auto_cut_flag(ts_void)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	ts_s32 auto_cut_flag = gst_snr_ctx.s32_run_auto_cut;

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	return auto_cut_flag;
}




ts_void ISP_set_auto_cut_flag_auto(ts_void)
{
	// ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	// if(UsermenuParameter.ShutterMode == SHUTTER_MODE_TIMER){
	// 	gst_snr_ctx.s32_run_auto_cut = 1;
	// }
	// else{
	// 	gst_snr_ctx.s32_run_auto_cut = 0;
	// }
	// ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    // vision_log_debug("gst_snr_ctx.s32_run_auto_cut %d \n",gst_snr_ctx.s32_run_auto_cut);
}


ts_void ISP_cut_v2(void)
{
	// ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	// ts_u32 u32_cut_status=0x1;
	// ts_s32 s32_temp, u32_mean;
	// snr_planck640_cut(u32_cut_status,&s32_temp,&u32_mean);
	// ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}
/** @note 无解冻的打挡 */
ts_void vot_isp_cut_with_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 delay_flag, ts_u08 cut_type) 
{
    snr_attr_s_t *pst_attr = &gst_snr_ctx.st_attr;;
	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间

    ts_viu_corr_rst_s_t st_rst_attr;

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	vision_log_warn("enter: !\n");
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间    
	if(cut_type==0){  
		if (SystemParaMenu2.Usermenu_SW == 0) {
			tdk_snrcfg_shutter_close();
			ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
			tdk_snrcfg_shutter_open();
		}
		else {
			vision_log_warn("Temp Mode shift with defrost !\n");
			vot_templut_shutter_close();
			ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);
			vot_templut_shutter_open();
		}
	}
	else if(cut_type==2){
		tdk_snrcfg_shutter_close();
		st_rst_attr.u08_reset_only = 0;
		st_rst_attr.u08_reset_val = (ts_u08)0x80;
		
		ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
		snr_init_ooc();
		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
		tdk_snrcfg_shutter_open();
	}
	else if(cut_type==3){

		st_rst_attr.u08_reset_only = 0;
		st_rst_attr.u08_reset_val = (ts_u08)0x80;
		
		ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);
		snr_init_ooc();
		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	
	}
	else if(cut_type==4){

		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	
	}
	else{
		tdk_snrcfg_shutter_close();
		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
		tdk_snrcfg_shutter_open();
	}


	ts_tdk_vpo_freeze(TS_VPO_MDEV0);

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	
	ts_cpl_fix_msleep(u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	if(delay_flag==0){
		ts_cpl_fix_msleep(3*u32_msecs); ///延时三帧时间  
	}
	else{
		ts_cpl_fix_msleep(300); ///延时三帧时间  
	}
   
	if(u08_need_unfreeze_flag==1) {  
		ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	vision_log_warn("leave: !\n");
}
/**
 * @description: 支持插入彩条的打档
 * @param 
  1. pod_type取值，VOT_POD_TYPE_ENUM
  2. pod_flag=0, 是否插入彩条，0不插入，1插入
  3. u08_need_unfreeze_flag=1, 解冻vpo, u08_need_unfreeze_flag=0, 不解冻vpo
 * @return {*}
 */
ts_void vot_isp_cut_with_pod_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 pod_flag ,ts_u08 pod_type ) 
{
    snr_attr_s_t *pst_attr = &gst_snr_ctx.st_attr;;
	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间

	// vision_log_warn("enter: !\n");

	if(pod_flag==1){
		if(pod_type==0){
			SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_WHITE_GRAY;
		}
		else if(pod_type==1){
			SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_BLACK;
		}
		else if(pod_type==2){
			SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_GRAY;
		}
		// vision_log_warn("enter: pod_flag %d !\n",pod_flag);
		vot_snr_set_grp_pod_switch(1);
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间      
	if (SystemParaMenu2.Usermenu_SW == 0) {
		tdk_snrcfg_shutter_close();
		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
		tdk_snrcfg_shutter_open();
	}
	else {
		vision_log_warn("Temp Mode shift with defrost !\n");
		vot_templut_shutter_close();
		ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);
		vot_templut_shutter_open();
	}

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	
	ts_cpl_fix_msleep(u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(300); ///延时三帧时间     
	if(u08_need_unfreeze_flag) {  
		ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	}
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	if(pod_flag==1){
		vot_snr_set_grp_pod_switch(0);
		SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_WHITE_GRAY;
	}
	// vision_log_warn("leave: !\n");
}
/** @note 测温挡位切换专属打挡 */
ts_void vot_isp_temp_cut_with_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 delay_flag) {
    snr_attr_s_t *pst_attr = &gst_snr_ctx.st_attr;;
	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	ts_cpl_fix_msleep(3*u32_msecs);  ///延时一帧时间    

	vision_log_warn("Temp Mode shift with defrost !\n");
	vot_templut_shutter_close();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);
	ts_cpl_msleep(100);
	vot_templut_shutter_open();

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	if(delay_flag == 0){
		ts_cpl_fix_msleep(3*u32_msecs); ///延时三帧时间  
	}
	else{
		ts_cpl_fix_msleep(300); ///延时三帧时间  
	}
   
	if(u08_need_unfreeze_flag == 1) {  
		ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	}
}
ts_u08 ISP_cut_old(void)
{
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

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
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
		vision_log_debug("ts_tdk_isp_get_offline_frm err !\n");
		return 0;
	}
	ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
	if (s32_ret != TS_OK)
	{
		ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
		vision_log_debug("ts_tdk_isp_send_offline_frm err !\n");
		return 0;
	}
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_msleep(40);       
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst
	ts_cpl_msleep(40);       
	ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	return 0;
}

/**
 * @description: 外参考采样接口
 * @param {void}
 * @return {ts_void}
 */
ts_void ISP_cut_new_exteral(void)
{
    snr_attr_s_t *pst_attr;
	if(ISP_get_sun_pt_force_flag()>0 ||  global_sys_ctrl_info.m_shutter_force_close_flag==1){
		return;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    pst_attr = &gst_snr_ctx.st_attr;

	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);
	ts_u32 u32_msecs_3frame = 3*u32_msecs;

	global_sys_ctrl_info.isp_cut_cnt_u32++;	

	// vision_log_warn("new shutter test1 ,u32_msecs %d !\n",u32_msecs);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	//ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间      
	ts_cpl_fix_msleep(u32_msecs_3frame);  //延时三帧时间 
	// /* 暂停坏斑，将数据缓存恢复 */
    // ts_tdk_extbpc_pause();
    // ts_tdk_extbpc_sync2dst();

	// tdk_snrcfg_shutter_close();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	// ts_viu_array_info_s_t st_viu_info;
	// ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
    // global_sys_ctrl_info.m_cur_bak_ad_mean_u32  = st_viu_info.u32_mean;          //获取到挡片均值
	// tdk_snrcfg_shutter_open();
	
	// vision_log_warn("global_sys_ctrl_info.m_cur_bak_ad_mean_u32  %u !\n",global_sys_ctrl_info.m_cur_bak_ad_mean_u32  );

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);

    // /* 同步坏斑，并启动 */
    // ts_tdk_extbpc_sync(1, 1, 1, 0);
    // ts_tdk_extbpc_run();

	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(u32_msecs_3frame); ///延时三帧时间              
	ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);

	if(global_sys_ctrl_info.m_show_det_cut_info>0){
		vision_log_warn("isp_auto_cut_run_cnt_u64 %llu !\n",global_sys_ctrl_info.isp_auto_cut_run_cnt_u64);
		vision_log_warn("isp_cut_cnt_u32 %u !\n",global_sys_ctrl_info.isp_cut_cnt_u32 );
		vision_log_warn("isp_cut_time_sec_u32 %u !\n",global_sys_ctrl_info.isp_cut_time_sec_u32 );
		vision_log_warn("isp_temp_ad_value_u16 %u !\n",global_sys_ctrl_info.isp_temp_ad_value_u16 );
	}


	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}


ts_void ISP_cut_manual(void)
{
	vision_log_warn("enter: !\n");
    snr_attr_s_t *pst_attr;
	if(ISP_get_sun_pt_force_flag()>0 ||  global_sys_ctrl_info.m_shutter_force_close_flag==1){
		return;
	}

	vision_log_warn("enter: !\n");
	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    pst_attr = &gst_snr_ctx.st_attr;

	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);
	ts_u32 u32_msecs_3frame = 3*u32_msecs;

	global_sys_ctrl_info.isp_cut_cnt_u32++;	

	vision_log_warn("new shutter test1 ,u32_msecs %d !\n",u32_msecs);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	//ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间      
	ts_cpl_fix_msleep(u32_msecs_3frame);  //延时三帧时间       

	tdk_snrcfg_shutter_close();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  

	ts_viu_array_info_s_t st_viu_info;
	ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
    global_sys_ctrl_info.m_cur_bak_ad_mean_u32  = st_viu_info.u32_mean;          //获取到挡片均值
	tdk_snrcfg_shutter_open();
	
	// vision_log_warn("global_sys_ctrl_info.m_cur_bak_ad_mean_u32  %u !\n",global_sys_ctrl_info.m_cur_bak_ad_mean_u32  );

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);


	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(u32_msecs_3frame); ///延时三帧时间              
	ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);

	if(global_sys_ctrl_info.m_show_det_cut_info>0){
		vision_log_warn("isp_auto_cut_run_cnt_u64 %llu !\n",global_sys_ctrl_info.isp_auto_cut_run_cnt_u64);
		vision_log_warn("isp_cut_cnt_u32 %u !\n",global_sys_ctrl_info.isp_cut_cnt_u32 );
		vision_log_warn("isp_cut_time_sec_u32 %u !\n",global_sys_ctrl_info.isp_cut_time_sec_u32 );
		vision_log_warn("isp_temp_ad_value_u16 %u !\n",global_sys_ctrl_info.isp_temp_ad_value_u16 );
	}

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
	vision_log_warn("leave: !\n");
}
/** @note 硬件层测温版打挡（指定挡位）*/
static ts_void vot_hw_enter_measurement(void) {
    snr_attr_s_t *pst_attr;
	if(ISP_get_sun_pt_force_flag()>0 ||  global_sys_ctrl_info.m_shutter_force_close_flag==1 ||MotorFlag.Flag){
		return;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    pst_attr = &gst_snr_ctx.st_attr;

	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间
	//ts_u32 u32_msecs = 3000 / (pst_attr->s32_fps);//3000/50=60ms
	ts_u32 u32_msecs_3frame = 3*u32_msecs;
	global_sys_ctrl_info.isp_cut_cnt_u32++;	

	// vision_log_warn("new shutter test1 ,u32_msecs %d !\n",u32_msecs);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	//ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间      
	ts_cpl_fix_msleep(u32_msecs_3frame);  //延时三帧时间 ,延时时间太短,打挡会闪屏  

	if(SystemParaMenu2.m_force_detector_type == FORCE_DETECTOR_TYPE_VOT_384X288){
		Auto_GOFSet_HAND();
	}
	if((SystemParaMenu2.Usermenu_SW) && (SystemParaMenu2.TemTest_SW == 1 || SystemParaMenu2.TemTest_SW == 3)) {
		Auto_INT();
	}

	vot_templut_shutter_close();
	
	if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){
		ts_isp_nuc_attr_s_t st_nuc_attr;
		ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
		st_nuc_attr.u16_coef = 0;
		ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
	}


	ts_s32 s32_temp_1st = sns_get_temperature();
	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	ts_s32 s32_temp_2st = sns_get_temperature();

	global_sys_ctrl_info.m_cali_temp_value = (s32_temp_1st+s32_temp_2st)/2;

#ifdef VOT_NUC_SFFC_TEST
	if (SystemParaMenu2.m_sffc_mode != SFFC_MODE_OFF && gst_snr_ctx.s32_sffc_alg > 0) {
		// vision_log_warn("gst_snr_ctx.s32_sffc_alg: %d !\n",gst_snr_ctx.s32_sffc_alg);
        ts_isp_sffc_attr_s_t st_sffc_attr;
        ts_tdk_isp_get_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);
        st_sffc_attr.u08_enable = 0;
        ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);
        ts_s32 s32_img_w, s32_img_h;
        snr_attr_s_t *pst_attr;
        pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
        s32_img_w = pst_attr > s32_img_w;
        s32_img_h = pst_attr > s32_img_h;
        isp_sw_calc_nuc_sffc(s32_img_w, s32_img_h, 0);
    }
#endif
	ts_viu_array_info_s_t st_viu_info;
	ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
    global_sys_ctrl_info.m_cur_bak_ad_mean_u32  = st_viu_info.u32_mean;          //获取到挡片均值
	vot_templut_shutter_open();
	
	// vision_log_warn("global_sys_ctrl_info.m_cur_bak_ad_mean_u32  %u !\n",global_sys_ctrl_info.m_cur_bak_ad_mean_u32  );

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);


	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(u32_msecs_3frame); ///延时三帧时间              
	ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	if(global_sys_ctrl_info.m_show_det_cut_info>0){
		vision_log_warn("isp_auto_cut_run_cnt_u64 %llu !\n",global_sys_ctrl_info.isp_auto_cut_run_cnt_u64);
		vision_log_warn("isp_cut_cnt_u32 %u !\n",global_sys_ctrl_info.isp_cut_cnt_u32 );
		vision_log_warn("isp_cut_time_sec_u32 %u !\n",global_sys_ctrl_info.isp_cut_time_sec_u32 );
		vision_log_warn("isp_temp_ad_value_u16 %u !\n",global_sys_ctrl_info.isp_temp_ad_value_u16 );
	}
}
/** @note 硬件层观测版打挡（关闭测温）*/
static ts_void vot_hw_enter_observation(void) {
    snr_attr_s_t *pst_attr;
	if(ISP_get_sun_pt_force_flag()>0 ||  global_sys_ctrl_info.m_shutter_force_close_flag==1 ||MotorFlag.Flag){
		return;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);

    pst_attr = &gst_snr_ctx.st_attr;
	ts_u32 u32_msecs = 1000 / (pst_attr->s32_fps);//1000/50=20ms打挡延时时间
	//ts_u32 u32_msecs = 3000 / (pst_attr->s32_fps);//3000/50=60ms
	ts_u32 u32_msecs_3frame = 3*u32_msecs;
	global_sys_ctrl_info.isp_cut_cnt_u32++;	

	// vision_log_warn("new shutter test1 ,u32_msecs %d !\n",u32_msecs);
	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
	//ts_cpl_fix_msleep(u32_msecs);  ///延时一帧时间      
	ts_cpl_fix_msleep(u32_msecs_3frame);  //延时三帧时间 ,延时时间太短,打挡会闪屏  
	//非矫正状态下，配置goc参数
	if(ISP_get_det_calibrationing_flag()==0 && det_gos_switch_force_last != SystemParaMenu2.OOC_Par){
		planck640c_write_gos(SystemParaMenu2.OOC_Par);
		vision_log_warn("vot_uaw_get_gos_from_det_temp SystemParaMenu2.OOC_Par =%d\n", SystemParaMenu2.OOC_Par);
		det_gos_switch_force_last = SystemParaMenu2.OOC_Par;
	}

	tdk_snrcfg_shutter_close();
	if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){
		ts_isp_nuc_attr_s_t st_nuc_attr;
		ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
		st_nuc_attr.u16_coef = 0;
		ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
	}


	ts_s32 s32_temp_1st = sns_get_temperature();
	if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){
		ts_sl_status_s_t st_status;
		ts_s32 s32_ret;
		if (gst_snr_ctx.s32_run_auto_b > 0 && gst_snr_ctx.u32_sl_en) {
			ts_cpl_log_debug("cut shutter temp %d\n", s32_temp_1st);
			ts_tdk_sl_process(gst_snr_ctx.h_sl, s32_temp_1st, 1);
			s32_ret = ts_tdk_sl_get_status(gst_snr_ctx.h_sl, &st_status);
			if(s32_ret >= 0) {
				if (st_status.s16_tpl_grp_id >= 0 && st_status.s16_tpl_grp_id < 3)
					change_ooc_idx((ts_u32)st_status.s16_tpl_grp_id);
				else
					change_ooc_idx(0);
			}
		}
	}


	ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
	ts_s32 s32_temp_2st = sns_get_temperature();

	global_sys_ctrl_info.m_cali_temp_value = (s32_temp_1st+s32_temp_2st)/2;

#ifdef VOT_NUC_SFFC_TEST
	if (SystemParaMenu2.m_sffc_mode != SFFC_MODE_OFF && gst_snr_ctx.s32_sffc_alg > 0) {
		// vision_log_warn("gst_snr_ctx.s32_sffc_alg: %d !\n",gst_snr_ctx.s32_sffc_alg);
        ts_isp_sffc_attr_s_t st_sffc_attr;
        ts_tdk_isp_get_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);
        st_sffc_attr.u08_enable = 0;
        ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);
        ts_s32 s32_img_w, s32_img_h;
        snr_attr_s_t *pst_attr;
        pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
        s32_img_w = pst_attr > s32_img_w;
        s32_img_h = pst_attr > s32_img_h;
        isp_sw_calc_nuc_sffc(s32_img_w, s32_img_h, 0);
    }
#endif
	ts_viu_array_info_s_t st_viu_info;
	ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_info);
    global_sys_ctrl_info.m_cur_bak_ad_mean_u32  = st_viu_info.u32_mean;          //获取到挡片均值
	tdk_snrcfg_shutter_open();
	
	// vision_log_warn("global_sys_ctrl_info.m_cur_bak_ad_mean_u32  %u !\n",global_sys_ctrl_info.m_cur_bak_ad_mean_u32  );

	ts_tdk_vpo_freeze(TS_VPO_MDEV0);


	ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0);//解除冻结
	ts_cpl_fix_msleep(2*u32_msecs); ///延时一帧时间        
	ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff);////fw rst

	// vision_log_warn("new shutter test1 ,u32_msecs_3frame %d !\n",u32_msecs_3frame);
	ts_cpl_fix_msleep(u32_msecs_3frame); ///延时三帧时间              
	ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);

	if(global_sys_ctrl_info.m_show_det_cut_info>0){
		vision_log_warn("isp_auto_cut_run_cnt_u64 %llu !\n",global_sys_ctrl_info.isp_auto_cut_run_cnt_u64);
		vision_log_warn("isp_cut_cnt_u32 %u !\n",global_sys_ctrl_info.isp_cut_cnt_u32 );
		vision_log_warn("isp_cut_time_sec_u32 %u !\n",global_sys_ctrl_info.isp_cut_time_sec_u32 );
		vision_log_warn("isp_temp_ad_value_u16 %u !\n",global_sys_ctrl_info.isp_temp_ad_value_u16 );
	}
}
/** @note 统一打挡执行函数 */
ts_void ISP_cut_new(void) {
	if (SystemParaMenu2.Usermenu_SW) {
		vot_hw_enter_measurement();
	}
	else {
		vot_hw_enter_observation();
	}
}
/** @note ISP打档 */
ts_void ISP_cut(void)
{
	if (SystemParaMenu2.Usermenu_SW) {
		/* 进入测温标定时，允许手动打档 */
		if (SystemParaMenu2.m_temperCalibra_Flag || SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
			ISP_cut_new();
			return;
		}
	}
	if( ISP_get_auto_cut_flag()>0 && ISP_get_sun_pt_force_flag()==0 ) {
		ISP_force_cut(1);
	}
	else {
		ISP_cut_new();
		vision_log_debug("ISP_get_auto_cut_flag() %d ISP_get_sun_pt_force_flag() %d !\n",ISP_get_auto_cut_flag(), ISP_get_sun_pt_force_flag());
	}
}


ts_u08 ISP_pause(void)
{
	gst_snr_ctx.s32_run_isp = 0;
	return 0;
}

ts_u08 ISP_auto_cut_off(void)
{
	UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
	return 0;
}

ts_u08 ISP_run(void)
{
	gst_snr_ctx.s32_run_isp = 1;
	return 0;
}

ts_u08 ISP_auto_cut_on(void)
{
	UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
	return 0;
}



ts_u08 ISP_sffc_cali(ts_s32 s32_index)
{
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

	ts_char *pc_fname = FN_SFFC_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	//1280探测器 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。
	#endif

	isp_sw_calc_sffc_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);

	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
	return 0;
}


ts_u08 ISP_sffc_init(void)
{
	return 0;
	
}



ts_u08 ISP_sffc_calc(void)
{
	vision_log_debug("ISP_sffc_calc\n");
	sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
	shutter_cut_on_off(SHUTTER_MODE_OFF, OPERATION_MODE_NONE);
	// ISP_auto_cut_off();
	ts_cpl_msleep(2000);
	ISP_pause();
	ts_cpl_msleep(3000);
	ISP_cut();
	ts_cpl_msleep(3000);
	ts_s32 s32_index = 0;
	ISP_sffc_cali(s32_index);
	ts_cpl_msleep(3000);
	ISP_run();
	ISP_auto_cut_on();
	ts_cpl_msleep(3000);
	shutter_cut_on_off(SHUTTER_MODE_TIMER, OPERATION_MODE_NONE);
	sffc_on_off(SFFC_MODE_MANUAl, OPERATION_MODE_NONE);
	ISP_cut();

	return 0;
}



ts_void ISP_sffc_refresh(void)
{

#ifndef VOT_NUC_SFFC_TEST
	ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
	// ts_s32 s32_index = 0;

	//刷新前，先恢复k和坏点

	ISP_reset_nuc_k();

	ts_char *pc_fname = FN_SFFC_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);

	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	#endif
	isp_sw_calc_sffc_refresh_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
#endif

}



ts_void ISP_sffc_nr_calc(void)
{
	ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;


	//刷新前，先恢复k和坏点
	ISP_reset_nuc_k();
	// ISP_bpc_on();

 	// ts_u32 s32_index = 0;
	// ts_u32 s32_step = 0;
	// ts_s32 s32_win_size = 2; // 滤波窗尺寸

	// s32_index = 0;
	// s32_step = 0;
	
	ts_char *pc_fname = FN_SFFC_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	// isp_sw_calc_sffc_nr(pc_fname, u32_file_size, s32_win_size, s32_img_w,
	// 	s32_img_h, s32_index, s32_step);
	//isp_sw_calc_sffc_red(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_index);	
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	//1280探测器 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。
	#endif
	isp_sw_calc_sffc_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);
	//isp_sw_calc_sffc(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_index);	
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
}


ts_void ISP_sffc_nr_save(void)
{
	ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;


 	// ts_u32 s32_index = 0;
	// ts_u32 s32_step = 0;
	// ts_s32 s32_win_size = 2; // 滤波窗尺寸

	// s32_index = 0;
	//s32_step = 1;
	
	ts_char *pc_fname = FN_SFFC_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	// isp_sw_calc_sffc_nr(pc_fname, u32_file_size, s32_win_size, s32_img_w,
	// 	s32_img_h, s32_index, s32_step);

	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	//1280探测器 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。
	#endif
	isp_sw_calc_sffc_red(pc_fname, u32_file_size, s32_img_w, s32_img_h);	
	// isp_sw_calc_sffc(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_index);
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif		
}

ts_void ISP_sffc_nr_calc_and_save(void)
{
 	ISP_sffc_nr_save();
	ISP_sffc_nr_calc();
}

ts_u08 ISP_occ_init(void)
{
	vision_log_debug("ISP_occ_init\n");
	ISP_pause();
	// ts_cpl_msleep(2000);
	// ISP_auto_cut_off();
	//reset occ value only ， not calc occ
	ts_viu_corr_rst_s_t st_rst_attr;
	st_rst_attr.u08_reset_val = 0x80;
	st_rst_attr.u08_reset_only = 1;
	ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);  

	return 0;
}

/**
 * @description: 对着25度均匀面做OCC
 * @param {ts_void}
 * @return {ts_void}
 */
ts_void ISP_occ_calc(void)
{
	vision_log_debug("ISP_occ_calc\n");

	ts_cpl_msleep(2000);
	//calc occ base on 0x80 ,只用高6bit，100000
	ts_viu_corr_rst_s_t st_rst_attr;
	st_rst_attr.u08_reset_val = 0x80;
	st_rst_attr.u08_reset_only = 0;
	ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);  

	ts_cpl_msleep(1000);

}


/**
 * @description: 对着挡片做OCC
 * @param {ts_void}
 * @return {ts_void}
 */
ts_void ISP_occ_calc_with_shutter(void)
{
	vision_log_debug("ISP_occ_calc\n");
	tdk_snrcfg_shutter_close();
	ts_cpl_msleep(2000);
	ts_tdk_viu_exec_correct(TS_VIU_MDEV0, null);  
	tdk_snrcfg_shutter_open();
	ts_cpl_msleep(1000);

}

ts_void ISP_update_b(void)
{
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

}



ts_u08 ISP_occ_save(void)
{
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	vision_log_debug("ISP_occ_save\n");
    pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
	ts_u32 u32_index = 0;//ts_cpl_str2num(argv[2]);
	if ((u32_index + 1) * s32_img_w * s32_img_h + sizeof(sfc_cfg_file_s_t) >= vot_get_fn_occ_file_size()) {
		vision_log_debug("wrong occ index \n");
		return 0;  
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
		vision_log_debug("st_ooc_buf.sz_size %d \n", st_ooc_buf.sz_size);
		
	}

	ISP_cut();
	// ISP_update_b_table();
	return 0;
}

ts_u08 ISP_nuc_init(void)
{
	ts_buffer_s_t st_kbuf;
	ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
	ts_u16 *pu16_ptr = (ts_u16 *)st_kbuf.pu08_ptr;
	ts_u32 u32_cnt = (ts_u32)(st_kbuf.sz_size >> 1);
	
	for (ts_s32 i = 0; i < u32_cnt; i++) { 
		pu16_ptr[i] = 0x4000;  
	}
	ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

	ISP_cut();
	return 0;
}


ts_u08 ISP_nuc_calc_k_lowtemp(void)
{
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	vision_log_debug("ISP_nuc_calc_k_lowtemp\n");
    pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

#ifndef VOT_NUC_SFFC_TEST
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 1;

	vision_log_debug(" s32_img_w=%d,s32_img_h=%d, s32_step=%d\n", s32_img_w, s32_img_h, s32_step);
	// isp_sw_calc_k_nuc_step(s32_img_w, s32_img_h, s32_tmp, s32_step);

	ts_char *pc_fname = FN_K_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);	
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	#endif
    isp_sw_calc_k_nuc_step(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	vision_log_debug("ISP_nuc_calc_k_lowtemp done\n");
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
#else
	ts_char *pc_fname_k = FN_K_CFG;
	ts_char *pc_fname_sffc = FN_SFFC_CFG;
	ts_s32 s32_k_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_k_size = ts_lib_sfc_get_size(s32_k_f_hdl);
	ts_s32 s32_sffc_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);

    ts_u32 u32_file_sffc_size = ts_lib_sfc_get_size(s32_sffc_f_hdl);
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 1;
	isp_sw_calc_k_sffc(pc_fname_k, pc_fname_sffc, u32_file_k_size,
	u32_file_sffc_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	ts_lib_sfc_close(s32_k_f_hdl);
	ts_lib_sfc_close(s32_sffc_f_hdl);
#endif

	return 0;
}

ts_u08 ISP_nuc_calc_k_hightemp(void)
{
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;


	vision_log_debug("ISP_nuc_calc_k_hightemp\n");
	pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
#ifndef VOT_NUC_SFFC_TEST
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 0;

	vision_log_debug(" s32_img_w=%d,s32_img_h=%d, s32_step=%d\n", s32_img_w, s32_img_h, s32_step);
	// isp_sw_calc_k_nuc_step(s32_img_w, s32_img_h, s32_tmp, s32_step);	

	ts_char *pc_fname = FN_K_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	#endif
    isp_sw_calc_k_nuc_step(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	vision_log_debug("ISP_nuc_calc_k_hightemp done\n");
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
#else
	ts_char *pc_fname_k = FN_K_CFG;
	ts_char *pc_fname_sffc = FN_SFFC_CFG;
	ts_s32 s32_k_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_k_size = ts_lib_sfc_get_size(s32_k_f_hdl);
	ts_s32 s32_sffc_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_sffc_size = ts_lib_sfc_get_size(s32_sffc_f_hdl);
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 0;
	isp_sw_calc_k_sffc(pc_fname_k, pc_fname_sffc, u32_file_k_size,
	u32_file_sffc_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	ts_lib_sfc_close(s32_k_f_hdl);
	ts_lib_sfc_close(s32_sffc_f_hdl);
#endif
	return 0;
}

ts_u08 ISP_nuc_calc_k_calc(void)
{

    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	vision_log_debug("ISP_nuc_calc_k_calc\n");


    pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
#ifndef VOT_NUC_SFFC_TEST
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 2;

	vision_log_debug(" s32_img_w=%d,s32_img_h=%d, s32_step=%d\n", s32_img_w, s32_img_h, s32_step);
	// isp_sw_calc_k_nuc_step(s32_img_w, s32_img_h, s32_tmp, s32_step);


	ts_char *pc_fname = FN_K_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	#endif
    isp_sw_calc_k_nuc_step(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
#else
	ts_char *pc_fname_k = FN_K_CFG;
	ts_char *pc_fname_sffc = FN_SFFC_CFG;
	ts_s32 s32_k_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_k_size = ts_lib_sfc_get_size(s32_k_f_hdl);
	ts_s32 s32_sffc_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_sffc_size = ts_lib_sfc_get_size(s32_sffc_f_hdl);
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 3;
	isp_sw_calc_k_sffc(pc_fname_k, pc_fname_sffc, u32_file_k_size,
	u32_file_sffc_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	ts_lib_sfc_close(s32_k_f_hdl);
	ts_lib_sfc_close(s32_sffc_f_hdl);
#endif


	vision_log_debug("ISP_nuc_calc_k_calc done\n");
	return 0;
}

ts_u08 ISP_nuc_calc_k_save(void)
{

    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	vision_log_debug("ISP_nuc_calc_k_save\n");

	pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

#ifndef VOT_NUC_SFFC_TEST
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 3;

	vision_log_debug(" s32_img_w=%d,s32_img_h=%d, s32_step=%d\n", s32_img_w, s32_img_h, s32_step);

	ts_char *pc_fname = FN_K_CFG;
	ts_s32 s32_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_size = ts_lib_sfc_get_size(s32_f_hdl);
	ts_lib_sfc_close(s32_f_hdl);
	vision_log_debug("before isp_sw_calc_k_nuc_step \n");
	#ifdef CONFIG_SNR_PLANCK1280C
	vot_tlsf_mem_deinit(&g_VotMemPoolInfo); //释放内存池
	//1280探测器 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。
	#endif
    isp_sw_calc_k_nuc_step(pc_fname, u32_file_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	vision_log_debug("after isp_sw_calc_k_nuc_step \n");
	// isp_sw_calc_k_nuc_step(s32_img_w, s32_img_h, s32_tmp, s32_step);
	#ifdef CONFIG_SNR_PLANCK1280C
	isp_cali_free_mem();//计算完后释放内存
	ts_u32 u32_pool_size = 16*1024*1024;
	vot_tlsf_mem_init(&g_VotMemPoolInfo,u32_pool_size,"vot_mempool");//重新申请内存池
	#endif
#else
	ts_char *pc_fname_k = FN_K_CFG;
	ts_char *pc_fname_sffc = FN_SFFC_CFG;
	ts_s32 s32_k_f_hdl = ts_lib_sfc_open(FN_K_CFG);
    ts_u32 u32_file_k_size = ts_lib_sfc_get_size(s32_k_f_hdl);
	ts_s32 s32_sffc_f_hdl = ts_lib_sfc_open(FN_SFFC_CFG);
    ts_u32 u32_file_sffc_size = ts_lib_sfc_get_size(s32_sffc_f_hdl);
	ts_u32 s32_tmp = 0;
	ts_u32 s32_step = 3;
	isp_sw_calc_k_sffc(pc_fname_k, pc_fname_sffc, u32_file_k_size,
	u32_file_sffc_size, s32_img_w, s32_img_h, s32_tmp, s32_step);
	ts_lib_sfc_close(s32_k_f_hdl);
	ts_lib_sfc_close(s32_sffc_f_hdl);
#endif
	vision_log_debug("ISP_nuc_calc_k_save done\n");

	ts_cpl_msleep(100);
	ISP_cut();
	
	ISP_run();
	return 0;
}

ts_u08 ISP_nuc_vot_kb_save(void)
{
	vot_isp_set_sync_bp();
	return 0;
}


ts_void ISP_clr_nuc_k(ts_void)
{
    ts_buffer_s_t st_kbuf;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
	
    for (ts_s32 i = 0; i < s32_img_w * s32_img_h; i++)
    {
		ts_u16 k_pre = pu16_ptr[i];
		pu16_ptr[i] = 0x4000;
		if(k_pre & 0x8000){
			//需要将坏点提示到kbuf最高位
			if(SystemParaMenu2.BAD_SW==1){
				pu16_ptr[i] = (pu16_ptr[i] | 0x8000);
			}
		}
    }
	vision_log_debug("s32_img_w %d s32_img_h %d...\n", s32_img_w, s32_img_h);
  	ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
}

ts_void ISP_reset_nuc_k(ts_void)
{
    ts_buffer_s_t st_kbuf;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;
    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;
    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;
	
	uint32_t pixels = s32_img_w * s32_img_h;
	uint32_t bpc_buf_size = pixels << 1;

	ts_u16* rd_kbuf = (ts_u16*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size);
	vision_log_debug("s32_img_w %d s32_img_h %d...\n", s32_img_w, s32_img_h);
	ts_s32 ret = vot_isp_sdk_k_table_read(rd_kbuf);
	if(ret==0){
		for (ts_s32 i = 0; i < pixels; i++)
		{
			ts_u16 k_pre = pu16_ptr[i];
			//考虑k值
			pu16_ptr[i] = (rd_kbuf[i] & 0x7fff);
			if(k_pre & 0x8000){
				pu16_ptr[i] = (pu16_ptr[i] | 0x8000);
			}
			
		}

	}
	else{
		vision_log_debug("vot_isp_sdk_k_table_reads read failed ! \n");
	}

	if(rd_kbuf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&rd_kbuf); 
    }
	ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

	ISP_bpc_on();
	vision_log_debug("s32_img_w %d s32_img_h %d...\n", s32_img_w, s32_img_h);
}


ts_u08 ISP_bpc_on(void)
{
   	ts_buffer_s_t st_kbuf;
    // sfc_cfg_file_s_t st_sfc_file;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    ts_u16 *pu16_ptr = (ts_u16*)st_kbuf.pu08_ptr;

	uint32_t pixels = s32_img_w * s32_img_h;
	uint32_t bpc_buf_size = pixels << 1;

	ts_u16* rd_kbuf = (ts_u16*)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,bpc_buf_size);

	ts_s32 ret = vot_isp_bp_kb_table_read(rd_kbuf);
	if(ret==0){
		for (ts_s32 i = 0; i < pixels; i++)
		{
			//只管坏点为bit15，k不变
			if(rd_kbuf[i] & 0x8000 ){
				pu16_ptr[i] |= 0x8000;
			}	
		}		
	}
	else{
		vision_log_debug("ISP_bpc_on read failed ! \n");
	}

	if(rd_kbuf){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&rd_kbuf); 
    }

	ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

	vision_log_debug("ISP_bpc_on  ! \n");
	return 0;
}

ts_u08 ISP_bpc_off(void)
{
	ts_buffer_s_t st_kbuf;
	ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
	ts_u16 *pu16_ptr = (ts_u16 *)st_kbuf.pu08_ptr;
	ts_u32 u32_cnt = (ts_u32)(st_kbuf.sz_size >> 1);
	
	//k不变
	for (ts_s32 i = 0; i < u32_cnt; i++) { 
		pu16_ptr[i] &= 0x7fff;  
	}
	ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

	vision_log_debug("ISP_bpc_off  ! \n");
	return 0;
}

ts_void ISP_update_b_table(void)
{

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

}


ts_u16 ISP_get_sun_pt_gap(ts_void)
{
	return UsermenuParameter.m_SunPT_gap;
}


ts_void ISP_set_sun_pt_gap(ts_u16 time_gap_sec)
{
	if(time_gap_sec>=MAX_SUN_PT_TIME_SECS){
		time_gap_sec = DEFAULT_SUN_PT_TIME_SECS;
	}
	
	//由于菜单一和菜单二同时用改变了，那么就进行同步修改
	UsermenuParameter.m_SunPT_gap = time_gap_sec; 
	SystemParaMenu2.P_Time_U16 = time_gap_sec;

}

ts_void ISP_set_sun_pt_mode(ts_u08 sun_pt_mode)
{
	if(sun_pt_mode>=SUN_PT_MODE_BUTT){
		sun_pt_mode = SUN_PT_MODE_AUTO;
	}
	UsermenuParameter.m_SunPT_mode = sun_pt_mode; 

	if(UsermenuParameter.m_SunPT_mode==SUN_PT_MODE_MANUAL && ISP_get_sun_pt_force_flag()){
		ISP_set_sun_pt_force_flag(0);
		vot_set_sun_pt_notice_info(0);
		vot_sun_pt_shutter_open();
		ISP_cut_new();
	}
}

ts_void ISP_set_sun_pt_sw(ts_u16 pt_sw)
{
	if(pt_sw>1){
		pt_sw = 1;
	}
	SystemParaMenu2.SunPT_SW = pt_sw; 
	if(pt_sw==0){
		vot_set_sun_pt_notice_info(0);
	}
	//如果在阳光保护期间，会主动打开挡片
	if(SystemParaMenu2.SunPT_SW==0 && ISP_get_sun_pt_force_flag()){
		ISP_set_sun_pt_force_flag(0);
		vot_sun_pt_shutter_open();
		ISP_cut_new();
	}
}

ts_void ISP_set_sun_pt_info_sw(ts_u08 pt_info_sw)
{
	if(pt_info_sw>1){
		pt_info_sw = 1;
	}
	SystemParaMenu2.SunPT_Info_SW = pt_info_sw; 
}



ts_void ISP_set_sun_pt_val(ts_u16 target_pixels)
{
	if(target_pixels==0 || target_pixels> MAX_SUN_PT_THRESHOLD){
		target_pixels = DEFAULT_SUN_PT_THRESHOLD;
	}
	SystemParaMenu2.S_Val = target_pixels;

    vision_log_debug("SystemParaMenu2.S_Val %u target_pixels %u \n",SystemParaMenu2.S_Val,target_pixels );
}

ts_void ISP_set_det_calibrationing_flag(ts_u08 det_calibrationing_flag)
{
	global_sys_ctrl_info.m_det_calibrationing_flag = det_calibrationing_flag;
	if(det_calibrationing_flag==1){
		ts_cpl_fix_msleep(500);
	}
    vision_log_debug("m_det_calibrationing_flag %d \n",global_sys_ctrl_info.m_det_calibrationing_flag);
}

ts_u08 ISP_get_det_calibrationing_flag(ts_void)
{
	return global_sys_ctrl_info.m_det_calibrationing_flag;
}


ts_void ISP_set_sun_pt_force_flag(ts_u08 sun_pt_force_flag)
{

	global_sys_ctrl_info.m_sun_pt_force_flag = sun_pt_force_flag;

    // vision_log_debug("m_sun_pt_force_flag %d \n",global_sys_ctrl_info.m_sun_pt_force_flag);
}

ts_u08 ISP_get_sun_pt_force_flag(ts_void)
{
	return global_sys_ctrl_info.m_sun_pt_force_flag;
}


ts_void ISP_get_uvc_size_in_bpc_mode(ts_u16* u16_disp_width_ptr, ts_u16* u16_disp_height_ptr)
{

	ts_u16 u16_disp_width = 640;
	ts_u16 u16_disp_height = 512;
	#ifdef CONFIG_SNR_PLANCK1280C
	{
		u16_disp_width = 1280;
		u16_disp_height = 1024;
	}
    #endif
	if(SystemParaMenu2.m_bpc_ref_mode == BPC_MODE_ON_UVC){
		u16_disp_width = global_sys_ctrl_info.m_sensor_s32_img_w;
		u16_disp_height = global_sys_ctrl_info.m_sensor_s32_img_h;
	}

	if(u16_disp_width_ptr){
		*u16_disp_width_ptr = u16_disp_width;
	}

	if(u16_disp_height_ptr){
		*u16_disp_height_ptr = u16_disp_height;
	}

}

/**
 * @brief:从Flash读取用户菜单默认参数配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_read_UserMenu_def_para_cfg(struct _Usermenu_Parameter *UserMenuDefPara)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	s32_hfile_r = ts_lib_sfc_open(FN_MENU1_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		memset(UserMenuDefPara, 0, sizeof(struct _Usermenu_Parameter));	
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(UserMenuDefPara), sizeof(struct _Usermenu_Parameter));
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief:FLash写入用户菜单默认参数配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_write_UserMenu_def_para_cfg(struct _Usermenu_Parameter UserMenuDefPara)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;

	s32_hfile_r = ts_lib_sfc_open(FN_MENU1_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&UserMenuDefPara), sizeof(struct _Usermenu_Parameter));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}

/**
 * @brief:从Flash读取系统菜单默认参数配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_read_SysMenu_def_para_cfg(struct Menu2_Para *SysMenuDefPara)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;
	s32_hfile_r = ts_lib_sfc_open(FN_MENU2_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		memset(SysMenuDefPara, 0, sizeof(struct Menu2_Para));	
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(SysMenuDefPara), sizeof(struct Menu2_Para));
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/**
 * @brief:FLash写入系统菜单默认参数配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_write_SysMenu_def_para_cfg(struct Menu2_Para SysMenuDefPara)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;
	s32_hfile_r = ts_lib_sfc_open(FN_MENU2_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&SysMenuDefPara), sizeof(struct Menu2_Para));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}


/**
 * @brief:从Flash读取JSON配置的用户菜单默认参数是否有效标志位
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 read_JsonUserMenuDefPara_valid_flag(struct _Usermenu_Parameter *UserParamValid)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;//数据有效标志位
	ts_u32 offset = 1024*512;	//前512K空间存储JSON配置的用户菜单默认参数，偏移512K,后512K空间存储JSON配置的用户菜单默认参数是否有效标志位
	s32_hfile_r = ts_lib_sfc_open(FN_MENU1_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		memset(UserParamValid, 0, sizeof(struct _Usermenu_Parameter));
		ts_lib_sfc_read(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, offset+4, (ts_u08*)(UserParamValid), sizeof(struct _Usermenu_Parameter));
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief:FLash写入JSON配置的用户菜单默认参数是否有效标志位
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 write_JsonUserMenuDefPara_valid_flag(struct _Usermenu_Parameter UserParamValid)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;
	ts_u32 offset = 1024*512;	//前512K空间存储JSON配置的用户菜单默认参数，偏移512K,后512K空间存储JSON配置的用户菜单默认参数是否有效标志位
	s32_hfile_r = ts_lib_sfc_open(FN_MENU1_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, offset+4, (ts_u08*)(&UserParamValid), sizeof(struct _Usermenu_Parameter));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}

/**
 * @brief:从Flash读取JSON配置的系统菜单默认参数是否有效标志位
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 read_JsonSystemMenuDefPara_valid_flag(struct Menu2_Para *SysParamValid)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;//数据有效标志位
	ts_u32 offset = 1024*512;	//前512K空间存储JSON配置的系统菜单默认参数，偏移512K,后512K空间存储JSON配置的系统菜单默认参数是否有效标志位
	s32_hfile_r = ts_lib_sfc_open(FN_MENU2_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		memset(SysParamValid, 0, sizeof(struct Menu2_Para));
		ts_lib_sfc_read(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, offset+4, (ts_u08*)(SysParamValid), sizeof(struct Menu2_Para));
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief:FLash写入JSON配置的系统菜单默认参数是否有效标志位
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 write_JsonSystemMenuDefPara_valid_flag(struct Menu2_Para Para_valid)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;
	ts_u32 offset = 1024*512;	//前512K空间存储JSON配置的系统菜单默认参数，偏移512K,后512K空间存储JSON配置的系统菜单默认参数是否有效标志位
	s32_hfile_r = ts_lib_sfc_open(FN_MENU2_DEF_PARA_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, offset+4, (ts_u08*)(&Para_valid), sizeof(struct Menu2_Para));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}

/**
 * @brief:从Flash读取配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_read_sys_cfg(void)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	vision_log_debug("s32_hfile_r=%d\n", s32_hfile_r);
	if (s32_hfile_r > 0) {
		memset(&UsermenuParameter, 0, sizeof(UsermenuParameter));
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(&UsermenuParameter), sizeof(UsermenuParameter));
		}
		ts_lib_sfc_close(s32_hfile_r);
	}

	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
 * @brief:FLash写入配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_write_sys_cfg(void)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;
	s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	// vision_log_debug("s32_hfile_r=%d\n", s32_hfile_r);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&UsermenuParameter), sizeof(UsermenuParameter));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}

/**
 * @brief:从Flash读取系统菜单二配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_read_sys_menu2_cfg( struct Menu2_Para *sys_menu2_cfg )
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	s32_hfile_r = ts_lib_sfc_open(FN_APP_MENU2_CFG);
	if (s32_hfile_r > 0) {
		memset(sys_menu2_cfg, 0, sizeof(struct Menu2_Para));
		
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(sys_menu2_cfg), sizeof(struct Menu2_Para));
			vision_log_debug("ISP_read_sys_menu2_cfg done. \n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}

	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/**
 * @brief:FLash写入系统菜单二配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_write_sys_menu2_cfg(struct Menu2_Para sys_menu2_cfg)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;

	// vision_log_debug("enter ISP_write_sys_menu2_cfg \n");
	s32_hfile_r = ts_lib_sfc_open(FN_APP_MENU2_CFG);
	// vision_log_debug("s32_hfile_r=%d\n", s32_hfile_r);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&sys_menu2_cfg), sizeof(struct Menu2_Para));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		vision_log_debug("ISP_write_sys_menu2_cfg done. \n");
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}

ts_u08 ISP_read_vot_misc_cfg( struct Vot_Misc_Para *misc_cfg )
{
	ts_s32 s32_hfile_r;
	ts_u32 data_valid=0;
	ts_u32 magic_number = 0x123456;

	s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	if (s32_hfile_r > 0) {
		memset(misc_cfg, 0, sizeof(struct Vot_Misc_Para));
		ts_u32 offset = 1024*512;
		if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
 			offset = 3*512;
		}
		ts_lib_sfc_read(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		vision_log_warn("data_valid [%x] \n",data_valid);
		if(data_valid==magic_number)
		{
			ts_lib_sfc_read(s32_hfile_r, offset+4, (ts_u08*)(misc_cfg), sizeof(struct Vot_Misc_Para));
			vision_log_debug("ISP_read_vot_misc_cfg done. \n");
		}
		ts_lib_sfc_close(s32_hfile_r);

	}

	if(data_valid==magic_number)
	{
		vision_log_debug("ISP_read_vot_misc_cfg done. \n");
		return 0;
	}
	else
	{
		return 1;
	}
}


ts_u08 ISP_write_vot_misc_cfg( struct Vot_Misc_Para misc_cfg)
{
	ts_s32 s32_hfile_r;
	ts_u32 data_valid=0;
	ts_u32 magic_number = 0x123456;

	s32_hfile_r = ts_lib_sfc_open(FN_APP_CFG);
	// vision_log_debug("s32_hfile_r=%d\n", s32_hfile_r);
	if (s32_hfile_r > 0) {
		ts_u32 offset = 1024*512;
		if(vot_get_flash_type()==VOT_ASIC_NOR_FLASH_8MB){
 			offset = 3*512;
		}
		data_valid=magic_number;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, offset+4, (ts_u08*)(&misc_cfg), sizeof(struct Vot_Misc_Para));
		data_valid=magic_number;
		ts_lib_sfc_write(s32_hfile_r, offset, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		vision_log_debug("ISP_write_vot_misc_cfg done. \n");
		ts_lib_sfc_sync();
		return 0;
	}
	return 1;
}


ts_u08 ISP_read_vot_isp_cfg( ts_isp_cfg_reg_s_t *vot_isp_cfg )
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	vision_log_debug("enter ISP_read_vot_isp_cfg \n");
	s32_hfile_r = ts_lib_sfc_open(FN_VOT_ISP_CFG);
	if (s32_hfile_r > 0) {
		memset(vot_isp_cfg, 0, sizeof(ts_isp_cfg_reg_s_t));
		
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(vot_isp_cfg), sizeof(ts_isp_cfg_reg_s_t));
			vision_log_debug("ISP_read_vot_isp_cfg done. \n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}

	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

ts_u08 ISP_write_vot_isp_cfg(ts_isp_cfg_reg_s_t vot_isp_cfg)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;

	vision_log_debug("enter ISP_write_vot_isp_cfg \n");

	s32_hfile_r = ts_lib_sfc_open(FN_VOT_ISP_CFG);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&vot_isp_cfg), sizeof(ts_isp_cfg_reg_s_t));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);

		ts_lib_sfc_close(s32_hfile_r);
		vision_log_debug("ISP_write_vot_isp_cfg done. \n");
		
		ts_lib_sfc_sync();
		return 0;
	}

	return 1;
}


ts_u16 ISP_get_nuc_kvalue(ts_s32 s32_x, ts_s32 s32_y)
{
    ts_buffer_s_t st_kbuf;
    ts_u16 *pu16_k_buf = null;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

	pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    pu16_k_buf = (ts_u16 *)st_kbuf.pu08_ptr;
    if ((s32_x < 0) || (s32_y < 0) || (s32_x >= s32_img_w) || (s32_y >= s32_img_h)) {
        vision_log_debug("wrong bp pos [%d, %d] \n", s32_x, s32_y);

        return -1;
    }

	ts_u16 kvalue=0;
	kvalue = pu16_k_buf[s32_y * s32_img_w + s32_x];

	return kvalue;
    
}

ts_void vot_mirror_bp_position(ts_s32 in_s32_x, ts_s32 in_s32_y, ts_s32* out_s32_x, ts_s32* out_s32_y)
{
    ts_s32 mirr_mode_enum =  ISP_mirr_get_mirr_enum(UsermenuParameter.Mirror);

    ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
    ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h ;
    
	vision_log_debug("UsermenuParameter.Mirror %d mirr_mode_enum %d \n", UsermenuParameter.Mirror,mirr_mode_enum);
    if(mirr_mode_enum == MIRROR_MODE_X){
        if(out_s32_x){
            *out_s32_x = s32_img_w - in_s32_x - 1;
        }

        if(out_s32_y){
            *out_s32_y = in_s32_y;
        }

    }
    else if(mirr_mode_enum == MIRROR_MODE_Y){

        if(out_s32_x){
            *out_s32_x = in_s32_x;
        }

        if(out_s32_y){
            *out_s32_y = s32_img_h - in_s32_y - 1;
        }

    }
    else if(mirr_mode_enum == MIRROR_MODE_X_Y){
        if(out_s32_x){
            *out_s32_x = s32_img_w - in_s32_x - 1;
        }

        if(out_s32_y){
            *out_s32_y = s32_img_h - in_s32_y - 1;
        }
    }
    else {
       if(out_s32_x){
            *out_s32_x = in_s32_x;
        }

        if(out_s32_y){
            *out_s32_y = in_s32_y;
        }
    }

}

ts_s32 ISP_mirr_get_mirr_enum(ts_u16 mirr)
{
	ts_u08 u08_hmirr_en = mirr&0x1;
	ts_u08 u08_vmirr_en = (mirr&0x2)>>1;
	ts_s32 mirr_mode_enum = MIRROR_MODE_OFF;


	if(u08_hmirr_en==1){
		if(u08_vmirr_en==1){
			mirr_mode_enum = MIRROR_MODE_X_Y;
		}
		else{
			mirr_mode_enum = MIRROR_MODE_X;
		}
	}
	else{
		if(u08_vmirr_en==1){
			mirr_mode_enum = MIRROR_MODE_Y;
		}
		else{
			mirr_mode_enum = MIRROR_MODE_OFF;
		}
	}
	
	return mirr_mode_enum;
}

ts_void ISP_unset_bp_pos(ts_s32 s32_x, ts_s32 s32_y)
{
    ts_buffer_s_t st_kbuf;
    ts_u16 *pu16_k_buf = null;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;


	pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

	ts_s32 out_s32_x,  out_s32_y;
	vot_mirror_bp_position(s32_x, s32_y, &out_s32_x, &out_s32_y);
	vision_log_warn("bp pos mirror: in[%d, %d] out[%d, %d]\n", s32_x, s32_y,out_s32_x,out_s32_y);
	s32_x = out_s32_x;
	s32_y = out_s32_y;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    pu16_k_buf = (ts_u16 *)st_kbuf.pu08_ptr;
    if ((s32_x < 0) || (s32_y < 0) || (s32_x >= s32_img_w) || (s32_y >= s32_img_h)) {
        vision_log_debug("wrong bp pos [%d, %d] \n", s32_x, s32_y);

        return;
    }

    pu16_k_buf[s32_y * s32_img_w + s32_x] &= 0x7fff;

    ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    vision_log_debug("unset bp pos [%d, %d] over \n", s32_x, s32_y);
}

ts_void ISP_set_bp_pos(ts_s32 s32_x, ts_s32 s32_y)
{
    ts_buffer_s_t st_kbuf;
    ts_u16 *pu16_k_buf = null;
    ts_s32 s32_img_w, s32_img_h;
    snr_attr_s_t *pst_attr;

    pst_attr = &(gst_snr_ctx.st_attr);
    s32_img_w = pst_attr->s32_img_w;
    s32_img_h = pst_attr->s32_img_h;

	//取余数计算坐标，兼容手持机和测温机芯
	s32_x = s32_x %pst_attr->s32_img_w;
	s32_y = s32_y % pst_attr->s32_img_h;

	ts_s32 out_s32_x,  out_s32_y;
	vot_mirror_bp_position(s32_x, s32_y, &out_s32_x, &out_s32_y);
	vision_log_warn("bp pos mirror: in[%d, %d] out[%d, %d]\n", s32_x, s32_y,out_s32_x,out_s32_y);
	s32_x = out_s32_x;
	s32_y = out_s32_y;

    ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);
    pu16_k_buf = (ts_u16 *)st_kbuf.pu08_ptr;
    if ((s32_x < 0) || (s32_y < 0) || (s32_x >= s32_img_w) || (s32_y >= s32_img_h)) {
        vision_log_debug("wrong bp pos [%d, %d] \n", s32_x, s32_y);

        return;
    }

    pu16_k_buf[s32_y * s32_img_w + s32_x] |= 0x8000;

    ts_tdk_isp_flush_nuc_kbuf(TS_ISP_MDEV0, (ts_buffer_s_t *)&st_kbuf);

    vision_log_debug("set bp pos [%d, %d] over \n", s32_x, s32_y);
}



ts_void ISP_init_user_cfg(ts_void)
{
	ts_cpl_gettimeofday(&st_tv_shutter_count_start);
	ts_cpl_gettimeofday(&st_tv_shutter_count_cur);
}



void  enter_exit_isp_bpc_adjust(ts_u08 calc_active)
{
    ts_u32 *pu32_addr, u32_len;
    //通过如下函数获取到isp寄存器及地址。
    if(ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK) {
        vision_log_debug("get isp reg error !\n");
        return ;
    }
	//所有的参数均可通过 pst_isp_reg 指针访问和修改。
    ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t *)pu32_addr;

	if(calc_active==1){

		//关闭bpc模块中，自动去坏点的功能，避免坏点矫正的时候，看不到自动去除的坏点
		pst_isp_reg->st_bpc.u16_thr_brit = 16383;	
		pst_isp_reg->st_bpc.u16_thr_dark = 16383;
		pst_isp_reg->st_bpc.u08_thr_brit_delta = 14;
		pst_isp_reg->st_bpc.u08_thr_dark_delta = 14;

		pst_isp_reg->st_bpc.u16_thr_k_max = 32767;
		pst_isp_reg->st_bpc.u16_thr_k_min = 0;
		

		//关闭ee模块中自动去突兀点，避免影响坏点的时候，看不到这类点
		pst_isp_reg->st_ee.u16_nr_thr1 = 511;
		pst_isp_reg->st_ee.u16_nr_thr2 = 1023;
		pst_isp_reg->st_ee.u08_nr_delta1 = 9;
		pst_isp_reg->st_ee.u08_nr_delta2 = 9;


	}
	// else{
	// 	//恢复参数
	// 	memcpy(pst_isp_reg, &global_sys_ctrl_info.m_isp_cfg,sizeof(ts_isp_cfg_reg_s_t) );
	// }
    
    
    //注意：修改完参数后，需要以下操作才能生效
    ts_tdk_isp_update_reg(TS_ISP_MDEV0);
}


void DeadPointCorrection(void)
{

	ts_u64 AD10Sum, AD60Sum;
	ts_s32 i,j;
	ts_u16 AD10adv, AD60adv;
	ts_u16 *pAD10tmp, *pAD60tmp;
	ts_u16 bad_point_cnt=0;

	ts_u32 m_bp_table_buff = 0;
	ts_u08 error_flag = 0;
	// ts_u16 IRwidth = 640, IRheight=512;
	ts_u16 IRwidth = global_sys_ctrl_info.m_sensor_s32_img_w;
	ts_u16 IRheight = global_sys_ctrl_info.m_sensor_s32_img_h;
	vision_log_warn(" IRwidth %d IRheight %d \n",IRwidth,IRheight);

	// if(SystemParaMenu2.m_bpc_ref_mode== BPC_MODE_ON_CVBS){
	// 	IRwidth = global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_width;
    // 	IRheight = global_sys_ctrl_info.st_cvbs_vpo_attr.u16_disp_height;
	// }

	if(global_sys_ctrl_info.m_high_temp_ad_buff==0){
		vision_log_warn(" global_sys_ctrl_info.m_high_temp_ad_buff==0 \n");
		return;
	}

	if(global_sys_ctrl_info.m_low_temp_ad_buff==0){
		vision_log_warn(" global_sys_ctrl_info.m_low_temp_ad_buff==0 \n");
		return;
	}

	if(m_bp_table_buff==0){
		ts_s32 s32_img_w = global_sys_ctrl_info.m_sensor_s32_img_w;
		ts_s32 s32_img_h = global_sys_ctrl_info.m_sensor_s32_img_h;
		m_bp_table_buff = (ts_u32)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,(s32_img_w * s32_img_h * 2 )) ;
		vision_log_warn(" s32_img_w %d s32_img_h %d \n",s32_img_w,s32_img_h);
	}


	ts_u16 *pAD10= (ts_u16*)global_sys_ctrl_info.m_low_temp_ad_buff ;
	ts_u16 *pAD60= (ts_u16*)global_sys_ctrl_info.m_high_temp_ad_buff ;
	ts_u08 *pDeadPoint= (ts_u08*)m_bp_table_buff ;

	AD10Sum = 0;
	AD60Sum = 0;
	pAD10tmp = pAD10;
	pAD60tmp = pAD60;

	//vision_log_debug("AD25 = %x, AD70 = %x\n", *pAD10, *pAD60);
	for(i=0; i<IRheight; i++)
	{
		for(j=0; j<IRwidth; j++)
		{
			AD10Sum += *pAD10tmp++;
			AD60Sum += *pAD60tmp++;
		}
	}

	vision_log_debug("AD10Sum = %lu, AD60Sum = %lu\n", (unsigned long)AD10Sum, (unsigned long)AD60Sum);
	AD10adv = (ts_u16)(AD10Sum/IRwidth/IRheight);
	AD60adv = (ts_u16)(AD60Sum/IRwidth/IRheight);
	vision_log_debug("AD10adv %x, AD60adv = %x\n", AD10adv, AD60adv);

	global_sys_ctrl_info.m_high_ad_mean_value  = AD60adv;			
	global_sys_ctrl_info.m_low_ad_mean_value = AD10adv;		

	pAD10tmp = pAD10;
	pAD60tmp = pAD60;

	float ADScoreValue=0.0f;
	ts_s32 max_bp_points = 100;

	float threshold_value=1.50f;
	for(i=0; i<IRheight; i++)
	{
		for(j=0; j<IRwidth; j++)
		{
			
			if(*pAD60tmp==0 || *pAD10tmp==0 || *pAD10tmp==*pAD60tmp)
			{
				*pDeadPoint++ = 1;
				// ISP_set_bp_pos(j, i);
				bad_point_cnt++;
				vision_log_debug("111 i:%d, j:%d--AD25 %04x, AD70 %04x\n", i, j, *pAD10tmp, *pAD60tmp);
			}
			else {

				ADScoreValue = (float)(AD60adv-AD10adv)/(*pAD60tmp-*pAD10tmp);

				if(*pAD60tmp>=AD60adv*2 || *pAD60tmp<=AD60adv/2 || *pAD10tmp>=AD10adv*2 || *pAD10tmp<=AD10adv/2)
				{
					*pDeadPoint++ = 1;
					// ISP_set_bp_pos(j, i);
					bad_point_cnt++;
					vision_log_debug("222 i:%d, j:%d--AD10 %04x, AD60 %04x, pK %f\n", i, j, *pAD10tmp, *pAD60tmp,ADScoreValue );
				}
				//if( abs(*pAD10tmp-AD10adv)>0x200 || abs(*pAD60tmp-AD60adv)>0x200 || abs(*pAD10tmp-*pAD60tmp)<0x10 )
				else if( ADScoreValue>threshold_value || ADScoreValue<0.8 ) //1.30, 1.32,1.35
				{
					*pDeadPoint++ = 1;
					
					bad_point_cnt++;
					vision_log_debug("333 i:%d, j:%d--AD10 %04x, AD60 %04x, pK %f\n", i, j, *pAD10tmp, *pAD60tmp, ADScoreValue);
				}
				else
				{
					// if((float)(AD60adv-AD10adv)/(*pAD60tmp-*pAD10tmp)>1.15 || (float)(AD60adv-AD10adv)/(*pAD60tmp-*pAD10tmp)<0.85)
					// 	vision_log_debug("----i:%d, j:%d--AD10 %04x, AD60 %04x, pK %f\n", i, j, *pAD10tmp, *pAD60tmp, (float)(AD60adv-AD10adv)/(*pAD60tmp-*pAD10tmp));

					*pDeadPoint++ = 0;
				}
			}

			if(bad_point_cnt>=max_bp_points){
				vision_log_debug("bad_point_cnt [%d] > max_bp_points %d \n", bad_point_cnt, max_bp_points);
				error_flag = 1;
				break ;
			}
			pAD60tmp++;
			pAD10tmp++;
		}
		if(error_flag ==1){
			break;
		}
	}

	//NUC之后，如果还存在大于max_bp_points个坏点，那么这个探测就是不良品，需要更新器件
	if(error_flag ==0 && bad_point_cnt<max_bp_points){
		pDeadPoint= (ts_u08*)m_bp_table_buff ;
		for(i=0; i<IRheight; i++)
		{
			for(j=0; j<IRwidth; j++)
			{
				if( (*pDeadPoint) == 0x1){
					ISP_set_bp_pos(j, i);
					vision_log_debug(" bad point at [%d,%d] \n", j, i);
				}
				pDeadPoint++;
			}
		}
	}

	// vot_isp_set_sync_bp();

    if(m_bp_table_buff){
        vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&m_bp_table_buff); 
    }

	vision_log_debug(" thr %f find  bad point sums [%d] \n", threshold_value, bad_point_cnt);

}


ts_void ISP_color_dump_bp_k(ts_nodebuf_s_t * pst_lbl, ts_u08 u32_font_color ,  ts_u08 visible, ts_u08 is_clear)
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

	if(is_clear){
		vot_osd_node_clear_info(pst_lbl );
	}

    vision_log_debug("visible [%d] is_clear [%d]  \n", visible, is_clear);
	
	if(visible==0){
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
	}
	else{
		ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 1);
	}
	ts_tdk_osd_refresh_display( TS_OSD_REFRESH_VISIBLE_ON);

	if(visible){
		for (ts_s32 i = 0; i < s32_img_w; i++) {
			for (ts_s32 y = 0; y < s32_img_h; y++) {
				k_value =  pu16_k_buf[y * s32_img_w + i];
				if(k_value & 0x8000){
					vot_osd_set_bpc_points(pst_lbl,  i,  y,  u32_font_color , visible);
				}
			}
		}
	}

}


/**
 * @brief:从Flash读取调焦功能相关配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_read_Motorparameter_cfg(void)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;

	s32_hfile_r = ts_lib_sfc_open(FN_MOTORSTORAGE_PARA_CFG);
	if (s32_hfile_r > 0) {
		memset(&MotorStorageParameters, 0, sizeof(MotorStorageParameters));
		
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(&MotorStorageParameters), sizeof(MotorStorageParameters));
			vision_log_debug("ISP_read_MotorStorage_parameter_cfg done. \n");
		}
		ts_lib_sfc_close(s32_hfile_r);
	}

	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/**
 * @brief:FLash写入调焦功能相关配置文件
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 ISP_write_Motorparameter_cfg(void)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;

	s32_hfile_r = ts_lib_sfc_open(FN_MOTORSTORAGE_PARA_CFG);

	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(&MotorStorageParameters), sizeof(MotorStorageParameters));
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);

		ts_lib_sfc_close(s32_hfile_r);
		vision_log_debug("ISP_write_MotorStorage_parameter_cfg done. \n");
		
		ts_lib_sfc_sync();
		return 0;
	}

	return 1;
}

////tmm -i 查看内存 FREE空间至少要预留16MB,否则执行NUC计算，sffc锅盖计算校正，锅盖刷新等操作会出现Out of memory超过存储提示。由于1280探测器用A12芯片DDR只有64MB,因此屏蔽占用固定内存空间的全局变量，改用局部变量
//#ifdef CONFIG_SNR_PLANCK1280C
#if 0 
/**
 * @brief:从Flash读取bp_table_buff1280
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 sfc_read_bp_table_buff1280(ts_u08 *bp_table_buff)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid=0;
	s32_hfile_r = ts_lib_sfc_open(FN_BP_TABLE_BUFF1280);
	if (s32_hfile_r > 0) {
		memset(bp_table_buff, 0, FN_BP_TABLE_BUFF1280_SIZE);	
		ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		if(data_valid==1)
		{
			ts_lib_sfc_read(s32_hfile_r, 4, (ts_u08*)(bp_table_buff), FN_BP_TABLE_BUFF1280_SIZE);
		}
		ts_lib_sfc_close(s32_hfile_r);
	}
	if(data_valid==1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/**
 * @brief:FLash写入bp_table_buff1280
 * @parameter:none
 * @return:0-success 1-failure
 */
ts_u08 sfc_write_bp_table_buff1280(ts_u08 *bp_table_buff)
{
	ts_s32 s32_hfile_r;
	ts_s32 data_valid;
	s32_hfile_r = ts_lib_sfc_open(FN_BP_TABLE_BUFF1280);
	if (s32_hfile_r > 0) {
		data_valid=0;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_write(s32_hfile_r, 4, (ts_u08*)(bp_table_buff), FN_BP_TABLE_BUFF1280_SIZE);
		data_valid=1;
		ts_lib_sfc_write(s32_hfile_r, 0, (ts_u08*)(&data_valid), 4);
		ts_lib_sfc_close(s32_hfile_r);
		ts_lib_sfc_sync();
		vision_log_debug("sfc_write_bp_table_buff1280 done. \n");
		return 0;
	}
	return 1;
}
#endif
