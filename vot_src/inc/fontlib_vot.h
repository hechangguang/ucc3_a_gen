/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 13:52:27
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-06-29 09:26:58
 * @FilePath: /ucc3_a_gen/vot_src/inc/fontlib_vot.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef  __FONTLIB_VOT_H__
#define  __FONTLIB_VOT_H__

#include <base/ts_type.h>
#include <stdio.h>
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_time.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
#include <uapi/upl_v100.h>
#include <math.h>
#include <fontlib.h>

typedef enum  {
    VOT_SHAPE_ENUM_DOUBLE_RECTARNGLE=0,
    VOT_SHAPE_ENUM_BUTT,
}  VOT_SHAPE_ENUM;

typedef struct vot_rectangles_attr_s{
    ts_u16 u16_left;
    ts_u16 u16_top;
    ts_u16 u16_width;
    ts_u16 u16_height;

    ts_u08 u08_bg_color;
    ts_u08 u08_font_color;


} vot_rectangles_attr_s_t;

typedef struct vot_double_rectangles_attr_s{
    vot_rectangles_attr_s_t outside;
    vot_rectangles_attr_s_t inside;

} vot_double_rectangles_attr_s_t;



typedef struct vot_shape_attr_s {
    ts_u08 u08_type;

    union {
        vot_double_rectangles_attr_s_t vs_drect_attr;
    } ;

} vot_shape_attr_s_t;


ts_s32 vot_check_font_info_vaild(ts_nodebuf_s_t *pst_lbl, ts_fontl_lbl_atts_s_t *fontl_attr_ptr);

ts_u32 fontl_get_fit_pixel_width(const ts_char *pc_str);
ts_s32 ts_lib_fontl_gen_label_info(ts_nodebuf_s_t *pst_lbl, const ts_char *pc_str, \
	ts_fontl_lbl_atts_s_t *pst_attr,ts_u08 line_clear_flag);

ts_s32 ts_lib_fontl_gen_label_info_font16x16(ts_nodebuf_s_t *pst_lbl, const ts_char *pc_str, \
    ts_fontl_lbl_atts_s_t *pst_attr,ts_u08 line_clear);
    
ts_u32 ts_lib_fontl_gen_label_1(ts_nodebuf_s_t *pst_framebuffer_osd, const ts_char *pc_str, \
    ts_fontl_lbl_atts_s_t *pst_attr);


ts_u08 ts_lib_osd_get_visible_vot(ts_nodebuf_s_t *pst_node);
ts_s32 ts_lib_osd_set_visible_vot(ts_nodebuf_s_t *pst_node, ts_u32 u32_visible);


ts_void vot_osd_node_clear_info(ts_nodebuf_s_t * pst_lbl );
ts_void vot_osd_set_bpc_points(ts_nodebuf_s_t * pst_lbl, ts_u16 pos_x, ts_u16 pos_y, ts_u08 u32_font_color , ts_u08 visible);

ts_u32 fontl_get_fit_pixel_width_font64x64(const ts_char *pc_str);
ts_s32 ts_lib_fontl_gen_label_info_font64x64(ts_nodebuf_s_t *pst_lbl, const ts_char *pc_str, ts_fontl_lbl_atts_s_t *pst_attr);

ts_u32 ts_lib_fontl_gen_label_font8x8(ts_nodebuf_s_t *pst_framebuffer_osd, const ts_char *pc_str,   ts_fontl_lbl_atts_s_t *pst_attr);

#endif
