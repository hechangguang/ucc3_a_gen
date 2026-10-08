/*
 * fontlib.c -- 字库实现例子
 */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_errno.h>
#include <uapi/osd_v100.h>
#include <cpl/ts_cpl.h>
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "fontlib.h"


/* 保存纯字 OSD 信息 */
#define STR_MAX_LINES       (5)
#define STR_MAX_NUM         (16)

/* OSD 上下文 */
typedef struct osd_ctx_s {
    /* 以下保存一些无关内存空间大小的重复配置 */
    ts_u08 u08_def_pix_bw;              /* 颜色位宽，仅支持 1bit 和 2bit 深度 */
    ts_u08 u08_def_font_color;          /* 字体颜色 */
    ts_u08 u08_def_bg_color;            /* 背景色 */
    ts_u08 u08_def_box_color;           /* 边框颜色 */

    ts_s08 s08_def_alpha;               /* 透明度 */
    ts_u08 u08_auto_invert;             /* 自动反转 */

    ts_u08 u08_def_lr_margin;           /* 两边留空像素 */
    ts_u08 u08_def_tb_margin;           /* 上下留空像素 */

    ts_u32 u32_rev32;
} osd_ctx_s_t;

static osd_ctx_s_t sst_ctx = {0};


#define FONTL_MAX_NUM       (32)

static ts_u08 *fontl_seek(ts_u32 u32_utf8, ts_u32 u32_code)
{
    ts_u32 i, u32_chk_code, u32_pre_size;
    ts_u08 *pu08_ascii_code;
    ts_u32 *pu32_utf8_code;
    ts_u08 *pu08_ptr;

    i = 0;
    // pu08_ascii_code = (ts_u08 *)&sau08_asiic;
    // pu32_utf8_code = (ts_u32 *)&sau32_utf8;
    #ifdef CONFIG_SNR_PLANCK1280C
    // if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
       pu08_ascii_code = (ts_u08 *)&sau08_asiic32;
       pu32_utf8_code = (ts_u32 *)&sau32_utf8_32;     
    }
    #else
    {
        pu08_ascii_code = (ts_u08 *)&sau08_asiic16;
       pu32_utf8_code = (ts_u32 *)&sau32_utf8_16;
    }
    #endif
    while (1) {
        if (0 == u32_utf8)
            u32_chk_code = (ts_u32)pu08_ascii_code[i];
        else
            u32_chk_code = pu32_utf8_code[i];

        if (0 == u32_chk_code)
            return ts_null;
        else if (u32_code == u32_chk_code)
            break;
        else
            i++;
    }

    // if (0 == u32_utf8) {
    //     pu08_ptr = (ts_u08 *)&au08_font_asiic;
    //     u32_pre_size = FONT_HALF_SIZE;
    // } else {
    //     pu08_ptr = (ts_u08 *)&au08_font_song;
    //     u32_pre_size = FONT_FULL_SIZE;
    // }

    #ifdef CONFIG_SNR_PLANCK1280C
    {
        if (0 == u32_utf8) 
        {
            pu08_ptr = (ts_u08 *)&au08_font_asiic32;
            u32_pre_size = 64;
        } 
        else {
            pu08_ptr = (ts_u08 *)&au08_font_song32;
            u32_pre_size = 128;
        }     
    }
    #else
    {
        if (0 == u32_utf8) 
        {
            pu08_ptr = (ts_u08 *)&au08_font_asiic16;
            u32_pre_size = 16;
        } 
        else {
            pu08_ptr = (ts_u08 *)&au08_font_song16;
            u32_pre_size = 32;
        }     
    }
    #endif
    pu08_ptr += u32_pre_size * i;

    return pu08_ptr;
}


static ts_void fontl_clr_buf(ts_nodebuf_s_t *pst_hosd, ts_u32 u32_bit_width, ts_u32 u32_bg_color, ts_u08 *pu08_fill)
{
    ts_u32 i;
    ts_u08 u08_fill, u08_tmp;

    u08_fill = 0;
    if (TS_PIXEL_BW_1BIT == u32_bit_width) {
        if (u32_bg_color > 0)
            u08_fill = 0xff;
    } else {
        if (u32_bg_color > 0) {
            u08_tmp = u32_bg_color & 0x03;
            for (i = 0; i < 4; i++) {
                u08_fill = u08_fill << 2;
                u08_fill |= u08_tmp;
            }
        }
    }

    if (pu08_fill != ts_null)
        *pu08_fill = u08_fill;

    ts_cpl_memset((ts_void *)pst_hosd->u32_virt_addr, u08_fill, pst_hosd->u32_size);
}


/* 生成矩形框 */
ts_nodebuf_s_t *ts_lib_fontl_gen_rectangle(ts_mdev_t h_osd_dev, ts_fontl_rect_attr_s_t *pst_rect_attr)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_rect;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    st_attr.u16_left = pst_rect_attr->u16_left;
    st_attr.u16_top = pst_rect_attr->u16_top;
    st_attr.u16_width = pst_rect_attr->u16_width;
    st_attr.u16_height = pst_rect_attr->u16_height;

    st_attr.u08_pix_bw = 0;
    if (pst_rect_attr->u08_visiable > 0)
        st_attr.u08_visible = 1;
    else
        st_attr.u08_visible = 0;

    if (pst_rect_attr->u08_priority > 100)
        st_attr.u08_priority = 100;
    else
        st_attr.u08_priority = pst_rect_attr->u08_priority;

    st_attr.u08_type = TS_OSD_TYPE_RECT;

    st_attr.st_misc_cfg.u08_margin_h = pst_rect_attr->u08_thick;
    st_attr.st_misc_cfg.u08_margin_v = pst_rect_attr->u08_thick;

    pst_rect = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_rect)
        return ts_null;

    if (pst_rect != ts_null) {
        pu08_buf = (ts_u08 *)pst_rect->u32_virt_addr;
        pu08_buf[0] = pst_rect_attr->u08_color;
        pu08_buf[1] = pst_rect_attr->u08_color;
        pu08_buf[2] = pst_rect_attr->u08_color;
        pu08_buf[3] = pst_rect_attr->u08_color;
    }

    return pst_rect;
}

/* 生成光标 */
ts_nodebuf_s_t *ts_lib_fontl_gen_cursor(ts_mdev_t h_osd_dev, ts_fontl_cursor_attr_s_t *pst_cursor_attr)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    st_attr.u16_left = pst_cursor_attr->u16_center_left;
    st_attr.u16_top = pst_cursor_attr->u16_center_top;
    st_attr.u16_height = pst_cursor_attr->u16_height;
    st_attr.u16_width = pst_cursor_attr->u16_width;

    st_attr.u08_pix_bw = 0;
    if (pst_cursor_attr->u08_visiable > 0)
        st_attr.u08_visible = 1;
    else
        st_attr.u08_visible = 0;

    if (pst_cursor_attr->u08_priority > 100)
        st_attr.u08_priority = 100;
    else
        st_attr.u08_priority = pst_cursor_attr->u08_priority;

    st_attr.u08_type = TS_OSD_TYPE_CURSOR_BLINK;

    if (pst_cursor_attr->u08_margin > 0x20) {
        st_attr.st_misc_cfg.u08_margin_v = 0x20;
        st_attr.st_misc_cfg.u08_margin_h = 0x20;
    } else {
        st_attr.st_misc_cfg.u08_margin_v = pst_cursor_attr->u08_margin;
        st_attr.st_misc_cfg.u08_margin_h = pst_cursor_attr->u08_margin;
    }
        
    st_attr.st_misc_cfg.u08_len_h = pst_cursor_attr->u08_thick;
    st_attr.st_misc_cfg.u08_len_v = pst_cursor_attr->u08_thick;

    if (pst_cursor_attr->u08_blink > 100)
        st_attr.st_misc_cfg.u08_blink = 100;
    else
        st_attr.st_misc_cfg.u08_blink = pst_cursor_attr->u08_blink;

    st_attr.st_misc_cfg.u08_center_hide = pst_cursor_attr->u08_center_hide;

    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null) {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;
        if (TS_OSD_TYPE_CURSOR_RECT == st_attr.u08_type) {
            pu08_buf[0] = pst_cursor_attr->u08_center_color;
            pu08_buf[1] = pst_cursor_attr->u08_center_color;
            pu08_buf[2] = pst_cursor_attr->u08_center_color;
            pu08_buf[3] = pst_cursor_attr->u08_center_color;
            pu08_buf[4] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[5] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[6] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[7] = pst_cursor_attr->u08_cursor_color;

            st_attr.st_misc_cfg.u08_blink = 0;
        } else {
            pu08_buf[0] = pst_cursor_attr->u08_center_color;
            pu08_buf[1] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[2] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[3] = pst_cursor_attr->u08_cursor_color;
            pu08_buf[4] = pst_cursor_attr->u08_cursor_color;
        }
    }

    return pst_cursor;
}


/* 生成 osd 的背景以及外框 */
static ts_void fontl_gen_bg(ts_osd_s_t *pst_osd)
{
    ts_u32 i, j, k, u32_width;
    ts_u08 u08_color, u08_fill, u08_l_fill, u08_r_fill;
    ts_u08 *pu08_buf;
    ts_nodebuf_s_t *pst_hosd;
    ts_osd_attr_s_t *pst_osd_attr;

    pst_hosd = pst_osd->pst_hnode;
    pst_osd_attr = (ts_osd_attr_s_t *)&pst_osd->st_osd_attr;

    fontl_clr_buf(pst_hosd, pst_osd_attr->u08_pix_bw, pst_osd_attr->st_osd_cfg.u08_bg_color, (ts_u08 *)&u08_fill);

    /* 生成外框 */
    if (pst_osd->u08_box_en > 0) {
        /* 像素宽度转换到数据宽度 */
        u32_width = (ts_u32)pst_osd_attr->u16_width;
        u08_color = pst_osd->u08_box_color;
        if (TS_PIXEL_BW_1BIT == pst_osd_attr->u08_pix_bw) {
            u32_width = u32_width >> 3;
            u08_color &= 0x01;
            
            u08_l_fill = u08_fill & 0xFC;
            u08_l_fill |= (u08_color << 0) | (u08_color << 1);

            u08_r_fill = u08_fill & 0x3F;
            u08_r_fill |= (u08_color << 7) | (u08_color << 6);

            /* 计算横向填充数据 */
            if (u08_color > 0)
                u08_fill = 0xff;
            else
                u08_fill = 0x00;
        } else {
            u32_width = u32_width >> 2;
            u08_color &= 0x03;

            u08_l_fill = u08_fill & 0xF0;
            u08_l_fill |= (u08_color << 0) | (u08_color << 2);

            u08_r_fill = u08_fill & 0x0F;
            u08_r_fill |= (u08_color << 6) | (u08_color << 4);

            /* 计算横向填充数据 */
            u08_fill = (u08_color << 0) | (u08_color << 2) | (u08_color << 4) | (u08_color << 6);
        }

        /* 先确定两个边的边框, 左上角和右下角由横向边框补齐 */
        for (i = 0; i < (pst_osd->u16_ro_height - 1); i++) {
            pu08_buf = (ts_u08 *)pst_hosd->u32_virt_addr;
            pu08_buf += (i + 1) * u32_width - 1;
            pu08_buf[0] = u08_r_fill;
            pu08_buf[1] = u08_l_fill;
        }

        /* 按照给定的行数画上横向边框 */
        pu08_buf = (ts_u08 *)pst_hosd->u32_virt_addr;
        for (i = 0; i < (pst_osd->u08_line_num + 1); i++) {
            for (j = 0; j < pst_osd->u08_box_width; j++) {
                for (k = 0; k < u32_width; k++)
                    pu08_buf[k] = u08_fill;
                
                pu08_buf += u32_width;
            }

            pu08_buf += u32_width * pst_osd->u08_ro_line_height;
        }
    }
}


static ts_u32 fontl_gen_str_line_num(const ts_char *pc_str)
{
    ts_u32 i, k, n;

    i = 0;
    k = 0;
    n = 0;
    while (1) {
        if ('\n' == pc_str[i]) {
            n = 0;
            k++;
        } else if ('\0' == pc_str[i]) {
            if (n > 0)
                k++;

            break;
        } else {
            n++;
        }
        
        i++;
    }

    return k;
}


/* 找到 '\n 或者 '\0'; 返回长度 */
static ts_u32 fontl_string_cpy_line(ts_char *pc_src_str, ts_char *pc_dst_str)
{
    ts_u32 i, n;

    i = 0;
    n = 0;
    while (1) {
        if (('\n' == pc_src_str[i]) || ('\0' == pc_src_str[i])) {
            pc_dst_str[i] = '\0';

            break;
        } else {
            n++;
            pc_dst_str[i] = pc_src_str[i];
        }

        i++;
    }

    return n;
}


/* 返回最大字符宽度 */
static ts_u32 fontl_gen_str_info(ts_char *pc_str, ts_u32 *pu32_fonts, ts_u32 u32_max_num, ts_u32 *pu32_next_offset)
{
    ts_u32 i, k, u32_code, width;
    ts_u08 u08_code;

    /* 清零 */
    for (i = 0; i < u32_max_num; i++)
        pu32_fonts[i] = 0x00;

    i = 0;
    k = 0;
    width = 0;
    while (1) {
        /* 碰到结束符后退出 */
        if (('\0' == pc_str[i]) || ('\r' == pc_str[i]) || ('\n' == pc_str[i]))
            break;

        u08_code = pc_str[i];
        if (u08_code >= 0x80) {
            /* is utf8 */
            u32_code = u08_code << 16;
            u32_code |= pc_str[i + 1] << 8;
            u32_code |= pc_str[i + 2];
            pu32_fonts[k] = u32_code;
            i += 3;
            //width += FONT_FULL_WIDTH;
             #ifdef CONFIG_SNR_PLANCK1280C   
            {
               width += 4;     
            }
            #else
            {
                width += 2;
            }
            #endif
        } else {
            /* is ascii */
            pu32_fonts[k] = (ts_u32)u08_code;
            i++;
            //width += FONT_HALF_WIDTH;
            #ifdef CONFIG_SNR_PLANCK1280C   
            {
               width += 2;     
            }
            #else
            {
                width += 1;
            }
            #endif
        }

        k++;
        if (k >= u32_max_num) {
            vision_log_err("The number of char exceeds the max:%s\n", pc_str);

            return 0;
        }
    }

    if (pu32_next_offset != ts_null)
        *pu32_next_offset = i;

    return width;
}



/* 更新点阵 */
ts_void fontl_gen_dot_matrix(ts_u08 *pu08_buf, ts_u32 *pu32_font_code, ts_u32 u32_font_num, ts_u32 u32_buf_width, \
            ts_u32 u32_pix_bw, ts_u32 u32_left, ts_u08 u08_font_color, ts_u08 u08_bg_color)
{
    ts_u32 i, j, k, n, u32_code;
    ts_u32 u32_font_width, u32_font_pix_width, u32_offset;
    ts_u08 *pu08_font, u08_tmp;
    ts_u08 *pu08_ptr;
    ts_u32 u32_new_pix, u32_color_val, u32_bg_set;
    ts_u32 FONT_FULL_HEIGHT;

    if (TS_PIXEL_BW_1BIT == u32_pix_bw) {
        u32_color_val = (ts_u32)(u08_font_color & 0x01);
        u32_bg_set = (ts_u32)(u08_bg_color & 0x01);
        u32_offset = u32_left >> 3;
    } else {
        u32_color_val = (ts_u32)(u08_font_color & 0x03);
        u32_bg_set = (ts_u32)(u08_bg_color & 0x03);

        /* 2bit 深度需要乘以 2 */
        u32_offset = u32_left >> 2;
    }

    for (i = 0; i < u32_font_num; i++) {
        u32_code = pu32_font_code[i];
        if (0 == u32_code)
            return;
    
        /* 获取字符点阵, 字符宽度举例： 16x16 宽度 = 2； 8x16 宽度 = 1 */
        if (u32_code >= 0x80) {
            pu08_font = fontl_seek(1, u32_code);
           // u32_font_width = FONT_FULL_WIDTH;
             #ifdef CONFIG_SNR_PLANCK1280C   
            {
               u32_font_width = 4;  
               FONT_FULL_HEIGHT = 32;    
            }
            #else
            {
                u32_font_width = 2;
                FONT_FULL_HEIGHT = 16; 
            }
            #endif
             
        } else {
            pu08_font = fontl_seek(0, (ts_u32)u32_code);
            //u32_font_width = FONT_HALF_WIDTH;
            #ifdef CONFIG_SNR_PLANCK1280C
            {
               u32_font_width = 2;
               FONT_FULL_HEIGHT = 32;      
            }
            #else
            {
                u32_font_width = 1;
                FONT_FULL_HEIGHT = 16; 
            }
           #endif  
        }

        if (ts_null == pu08_font)
            continue;

        /* 2bit 颜色深度时， 宽度需要乘以 2, */
        if (TS_PIXEL_BW_2BIT == u32_pix_bw)
            u32_font_pix_width = u32_font_width << 1;
        else
            u32_font_pix_width = u32_font_width;

        /* 开始填充当前字符点阵到缓冲 */
        for (j = 0; j < FONT_FULL_HEIGHT; j++) {
            pu08_ptr = pu08_buf;
            pu08_ptr += u32_offset;
            pu08_ptr += j * u32_buf_width;
            for (k = 0; k < u32_font_width; k++) {
                u08_tmp = *pu08_font++;
                if (TS_PIXEL_BW_1BIT == u32_pix_bw) {
                    if (u32_color_val > 0)
                        *pu08_ptr++ = u08_tmp;
                    else
                        *pu08_ptr++ = ~u08_tmp;
                } else {
                    u32_new_pix = 0;
                    for (n = 0; n < 8; n++) {
                        if (u08_tmp & 0x80)
                            u32_new_pix |= u32_color_val;
                        else
                            u32_new_pix |= u32_bg_set;

                        u08_tmp = u08_tmp << 1;
                        u32_new_pix = u32_new_pix << 2;
                    }

                    /* 前一个循环多移了 2bit */
                    u32_new_pix = u32_new_pix >> 2;
                    *pu08_ptr++ = u32_new_pix & 0xFF;
                    *pu08_ptr++ = (u32_new_pix >> 8) & 0xFF;
                }
            }
        }

        u32_offset += u32_font_pix_width;
    }
}


ts_nodebuf_s_t *ts_lib_osd_update(ts_osd_s_t *pst_osd, ts_nodebuf_s_t *pst_next_node, 
                    ts_char *pc_str, ts_u32 u32_line_idx, ts_u32 u32_upd_type)
{
    ts_u32 u32_font_width, u32_buf_width, u32_offset;
    ts_u32 u32_tmp, u32_max_num, u32_align_type, u32_line_width;
    ts_osd_attr_s_t *pst_osd_attr;
    ts_nodebuf_s_t *pst_cur_node;
    ts_osd_line_s_t *pst_osd_line;
    ts_u32 au32_font_codes[STR_MAX_NUM];
    ts_u32 i, j, au32_fix_code[STR_MAX_NUM];
    ts_u08 *pu08_ptr;

    if ((ts_null == pst_osd) || (ts_null == pc_str) || (u32_upd_type > MNU_UPD_RIGHT)) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return ts_null;
    }

    if (u32_line_idx >= pst_osd->u08_line_num) {
        vision_log_err("line(%d) >= cfg(%d)\n", u32_line_idx, pst_osd->u08_line_num);

        return ts_null;
    }

    /* 获取字符串的宽度 */
    u32_font_width = fontl_gen_str_info(pc_str, (ts_u32 *)&au32_font_codes, STR_MAX_NUM, ts_null);
    if (0 == u32_font_width) {
        vision_log_err("Unable to parse this string:%s\n", pc_str);

        return ts_null;
    }

    pst_osd_attr = (ts_osd_attr_s_t *)&pst_osd->st_osd_attr;
    u32_buf_width = pst_osd_attr->u16_width;

    /* 获取行信息 */
    pst_osd_line = (ts_osd_line_s_t *)&pst_osd->pst_lines;
    pst_osd_line += u32_line_idx;

    /* 确定要写的起始位置和宽度是否合法, 整行写需要按照行属性配置拆分写入 */
    u32_line_width = u32_buf_width - (pst_osd->u08_lr_margin << 1);
    u32_line_width = u32_line_width >> 3;
    u32_offset = (ts_u32)pst_osd->u08_lr_margin;
    if (MNU_UPD_RIGHT == u32_upd_type) {
        u32_align_type = (ts_u32)pst_osd_line->u08_right_align_type;
        u32_max_num = (ts_u32)pst_osd_line->u08_right_font_num;
        u32_offset += pst_osd_line->u08_left_font_num << 3;
    } else {
        u32_align_type = (ts_u32)pst_osd_line->u08_left_align_type;
        if (MNU_UPD_LINE == u32_upd_type)
            u32_max_num = u32_line_width;
        else
            u32_max_num = (ts_u32)pst_osd_line->u08_left_font_num;
    }

    //vision_log_debug("align,line_w,max_n,fn=%d,%d,%d,%d\n", u32_align_type, u32_line_width, u32_max_num, u32_font_width);

    /* FIXME: 未做 offset + font_pix_width 是否越界； 此处全靠用户使用保证 */
    if (u32_font_width > u32_max_num) {
        vision_log_err("Ready to update font width is %d, but max is %d\n", u32_font_width, u32_max_num);

        return ts_null;
    }

    /* 前后字符数可能不一致，这里先全部刷新成 ' ', 在依据对齐方式补齐数据 */
#if 0
    /* 对齐调试用 */
    if (MNU_UPD_RIGHT == u32_upd_type) {
        for (i = 0; i < u32_max_num; i++)
            au32_fix_code[i] = '.';
    } else if (MNU_UPD_LEFT == u32_upd_type) {
        for (i = 0; i < u32_max_num; i++)
            au32_fix_code[i] = ',';
    } else {
        for (i = 0; i < u32_max_num; i++)
            au32_fix_code[i] = '-';
    }
#else
    for (i = 0; i < u32_max_num; i++)
        au32_fix_code[i] = ' ';
#endif

    u32_max_num = u32_max_num - u32_font_width;
    if (OSD_ALIGN_CENTER == u32_align_type)
        j = u32_max_num >> 1;
    else if (OSD_ALIGN_RIGHT == u32_align_type)
        j = u32_max_num;
    else
        j = 0;

    for (i = 0; i < u32_font_width; i++) {
        u32_tmp = au32_font_codes[i];
        if (0 == u32_tmp)
            break;

        au32_fix_code[j] = u32_tmp;
        j++;
        u32_max_num++;
    }

    //vision_log_debug("u32_offset=%d,%d\n", u32_offset, u32_max_num);

    /* 检测 left 是否对齐无效, 1bit 颜色深度需要 8 像素对齐， 2bit 颜色深度需要 4 像素对齐 */
    if (TS_PIXEL_BW_1BIT == pst_osd_attr->u08_pix_bw) {
        u32_buf_width = u32_buf_width >> 3;
    } else {
        u32_buf_width = u32_buf_width >> 2;
    }

    pst_cur_node = pst_osd->pst_hnode;

    /* 切换缓冲需要 copy 旧节点的配置 */
    if (pst_next_node != ts_null) {
        /* 属性在 update 时会从旧节点复制到新节点 */
        ts_tdk_osd_update(pst_cur_node, pst_next_node);
        ts_tdk_osd_get_attr(pst_osd->h_osd_dev, pst_next_node, pst_osd_attr);

        /* 进行全缓存 copy */
        ts_cpl_memcpy((ts_void *)pst_next_node->u32_virt_addr, (ts_void *)pst_cur_node->u32_virt_addr, pst_cur_node->u32_size);

        pst_osd->pst_hnode = pst_next_node;
    }

    /* 计算对应行号的缓冲位置 */
    pu08_ptr = (ts_u08 *)pst_osd->pst_hnode->u32_virt_addr;

    if (pst_osd->u08_box_en > 0)
        pu08_ptr += u32_buf_width * pst_osd->u08_box_width * (u32_line_idx + 1);

    pu08_ptr += u32_buf_width * pst_osd->u08_ro_line_height * u32_line_idx;

    /* 加速行间距 */
    pu08_ptr += pst_osd->u08_tb_margin * u32_buf_width;

    /* 填充点阵 */
    fontl_gen_dot_matrix(pu08_ptr, (ts_u32 *)&au32_fix_code, u32_max_num, u32_buf_width,
                            pst_osd_attr->u08_pix_bw, u32_offset, pst_osd_attr->st_osd_cfg.u08_font_color, 
                            pst_osd_attr->st_osd_cfg.u08_bg_color);

    return pst_cur_node;
}


ts_nodebuf_s_t *ts_lib_osd_set(ts_osd_s_t *pst_osd, ts_nodebuf_s_t *pst_next_node, ts_u32 u32_left, 
                                    ts_u32 u32_top, const ts_char *pc_string, ts_u32 u32_max_line)
{
    ts_osd_attr_s_t st_attr;
    ts_u32 i, u32_fix_height, u32_line_num, u32_str_len;
    ts_nodebuf_s_t *pst_cur_node;
    ts_nodebuf_s_t *pst_set_node;
    ts_char *pc_ptr;
    ts_char ac_str[STR_MAX_NUM * 2];

    if ((ts_null == pst_osd) || (ts_null == pst_osd->pst_hnode)) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return ts_null;
    }

    /* 获取行数，不能超过创建时配置的最大行数 */
    if (ts_null == pc_string) {
        u32_line_num = u32_max_line;
    } else {
        u32_line_num = fontl_gen_str_line_num(pc_string);
        if ((0 == u32_line_num) || (u32_line_num > (ts_u32)pst_osd->u08_line_num)) {
            vision_log_err("String is null or line number(%d) exceeds the max(%d)!\n", u32_line_num, (ts_u32)pst_osd->u08_line_num);
    
            return ts_null;
        }

        if (u32_line_num > u32_max_line)
            u32_line_num = u32_max_line;
    }

    pst_cur_node = pst_osd->pst_hnode;
    ts_tdk_osd_get_attr(pst_osd->h_osd_dev, pst_cur_node, (ts_osd_attr_s_t *)&st_attr);

    st_attr.u16_left = (ts_u16)u32_left;
    st_attr.u16_top = (ts_u16)u32_top;

    /* 计算当前所需要的高度 */
    u32_fix_height = pst_osd->u08_ro_line_height * u32_line_num;

    /* 如果有外框，需要加 n+1 个外框厚度 */
    if (pst_osd->u08_box_en > 0)
        u32_fix_height += pst_osd->u08_box_width * (u32_line_num + 1);

    st_attr.u16_height = u32_fix_height;
    pst_osd->u16_ro_height = u32_fix_height;

    vision_log_debug("fix=%d, lnum=%d,%d\n", u32_fix_height, u32_line_num, pst_osd->u08_line_num);

    if (ts_null == pst_next_node)
        pst_set_node = pst_cur_node;
    else
        pst_set_node = pst_next_node;

    ts_tdk_osd_set_attr(pst_osd->h_osd_dev, pst_set_node, (ts_osd_attr_s_t *)&st_attr);
    pst_osd->pst_hnode = pst_set_node;

    /* 对整个 osd 填充背景色，如果有外框，则画上外框 */
    pst_osd->u08_line_num = u32_line_num;
    fontl_gen_bg(pst_osd);

    if (pc_string != ts_null) {
        pc_ptr = (ts_char *)pc_string;
        for (i = 0; i < u32_line_num; i++) {
            /* 获取当前行字符编码，以及当前行的长度 */
            u32_str_len = fontl_string_cpy_line(pc_ptr, (ts_char *)&ac_str);
            if (0 == u32_str_len)
                break;
    
            ts_lib_osd_update(pst_osd, ts_null, (ts_char *)&ac_str, i, MNU_UPD_LINE);
            pc_ptr += u32_str_len + 1;
        }
    }

    return pst_cur_node;
}


ts_nodebuf_s_t *ts_lib_osd_create_dbbuf(ts_osd_s_t *pst_osd)
{
    ts_osd_attr_s_t st_attr;

    if ((ts_null == pst_osd) || (ts_null == pst_osd->pst_hnode)) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return ts_null;
    }

    ts_tdk_osd_get_attr(pst_osd->h_osd_dev, pst_osd->pst_hnode, (ts_osd_attr_s_t *)&st_attr);

    return ts_tdk_osd_create_node(ts_null, (const ts_osd_attr_s_t *)&st_attr);
}


ts_s32 ts_lib_osd_set_line_attr(ts_osd_s_t *pst_osd, ts_osd_line_s_t *pst_lines, ts_u32 u32_line_num)
{
    ts_u32 i, u32_r_num, u32_font_max_num, u32_font_remain;
    ts_osd_line_s_t *pst_osd_line;
    ts_osd_line_s_t *pst_dst_line;

    if ((ts_null == pst_osd) || (ts_null == pst_lines) || (0 == u32_line_num)) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    if (u32_line_num > pst_osd->u08_line_num) {
        vision_log_err("Line numer(%d) out of range[1..%d]\n", u32_line_num, pst_osd->u08_line_num);

        return -TS_EINVAL;
    }

    /* 循环检测布局属性是否合法 */
    //u32_pix_bw = (ts_u32)pst_osd->st_osd_attr.u08_pix_bw;
    u32_font_max_num = (pst_osd->st_osd_attr.u16_width - (pst_osd->u08_lr_margin * 2)) >> 3;
    pst_osd_line = pst_lines;
    pst_dst_line = (ts_osd_line_s_t *)&pst_osd->pst_lines;

    //vision_log_debug("o.p=%p,%p\n", pst_osd, pst_dst_line);
    for (i = 0; i < u32_line_num; i++) {
        if (pst_osd_line->u08_left_align_type > OSD_ALIGN_RIGHT) {
            vision_log_err("line[%d] left_align_type(%d) is invalid[0..2]\n", i, pst_osd_line->u08_left_align_type);

            return -TS_EINVAL;
        }

        if (pst_osd_line->u08_left_font_num > u32_font_max_num) {
            vision_log_err("line[%d] left_font_num(%d) > max(%d)\n", i, pst_osd_line->u08_left_font_num, u32_font_max_num);

            return -TS_EINVAL;
        }

        /* 没有分割成左右两部分显示 */
        u32_r_num = 0;
        if (pst_osd_line->u08_right_font_num > 0) {
            if (pst_osd_line->u08_right_align_type > OSD_ALIGN_RIGHT) {
                vision_log_err("line[%d] right_align_type(%d) is invalid[0..2]\n", i, pst_osd_line->u08_left_align_type);

                return -TS_EINVAL;
            }

            u32_font_remain = u32_font_max_num - pst_osd_line->u08_left_font_num;
            if (pst_osd_line->u08_right_font_num > u32_font_remain) {
                /* 超过剩余最大范围，修正到剩余个数 */
                u32_r_num = u32_font_remain;
                vision_log_warn("line[%d] right_font_num(%d)>remain(%d), fix to %d\n", i, pst_osd_line->u08_right_font_num, u32_font_remain, u32_font_remain);
            } else {
                u32_r_num = pst_osd_line->u08_right_font_num;
            }
        }

        pst_dst_line->u08_left_align_type = pst_osd_line->u08_left_align_type;
        pst_dst_line->u08_left_font_num = pst_osd_line->u08_left_font_num;
        pst_dst_line->u08_right_align_type = pst_osd_line->u08_right_align_type;
        pst_dst_line->u08_right_font_num = u32_r_num;

        pst_osd_line++;
        pst_dst_line++;
    }

    return TS_OK;
}


ts_void ts_lib_osd_set_color(ts_osd_s_t *pst_hosd, ts_u32 u32_pix_bw, ts_u32 u32_font_color, 
                    ts_u32 u32_bg_color, ts_u32 u32_box_color, ts_s08 s08_alpha, ts_u08 u08_auto_invert)
{
    ts_osd_attr_s_t *pst_attr;
    ts_nodebuf_s_t *pst_cur_node;

    if (ts_null == pst_hosd) {
        sst_ctx.u08_def_pix_bw = (ts_u08)u32_pix_bw;
        sst_ctx.u08_def_font_color = (ts_u08)u32_font_color;
        sst_ctx.u08_def_bg_color = (ts_u08)u32_bg_color;
        sst_ctx.u08_def_box_color = (ts_u08)u32_box_color;

        sst_ctx.s08_def_alpha = s08_alpha;
        sst_ctx.u08_auto_invert = u08_auto_invert;
    } else {
        pst_cur_node = pst_hosd->pst_hnode;
        pst_attr = (ts_osd_attr_s_t *)&pst_hosd->st_osd_attr;

        ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);

        pst_attr->u08_pix_bw = (ts_u08)u32_pix_bw;
        pst_attr->st_osd_cfg.u08_font_color = (ts_u08)u32_font_color;
        pst_attr->st_osd_cfg.u08_bg_color = (ts_u08)u32_bg_color;

        pst_attr->st_osd_cfg.s08_alpha = s08_alpha;
        pst_attr->st_osd_cfg.u08_auto_invert = u08_auto_invert;

        ts_tdk_osd_set_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);
    }
}


ts_void ts_lib_osd_set_margin(ts_osd_s_t *pst_hosd, ts_u32 u32_pix_bw, ts_u32 u32_lr_margin, ts_u32 u32_tb_margin)
{
    ts_u08 u08_lr_margin, u08_fix_margin, u08_pix_bw;

    u08_lr_margin = (ts_u08)u32_lr_margin;

    if (ts_null == pst_hosd)
        u08_pix_bw = (ts_u08)u32_pix_bw;
    else
        u08_pix_bw = pst_hosd->st_osd_attr.u08_pix_bw;

    /* 左右对齐必须是 1bit 颜色深度按照 8 像素对齐， 2bit 颜色深度按照 4 像素对齐 */
    u08_fix_margin = u08_lr_margin;
    if (TS_PIXEL_BW_1BIT == u08_pix_bw) {
        if ((u08_lr_margin & 0x07) > 0) {
            u08_fix_margin = (u08_lr_margin & 0xF8) + 8;
            vision_log_warn("lr_margin(%d) must be aligned to 8 pixels!fix to %d\n", u08_lr_margin, u08_fix_margin);
        }
    } else {
        if ((u08_lr_margin & 0x03) > 0) {
            u08_fix_margin = (u08_lr_margin & 0xFC) + 4;
            vision_log_warn("lr_margin(%d) must be aligned to 4 pixels!fix to %d\n", u08_lr_margin, u08_fix_margin);
        }
    }

    if (ts_null == pst_hosd) {
        sst_ctx.u08_def_lr_margin = u08_fix_margin;
        sst_ctx.u08_def_tb_margin = (ts_u08)u32_tb_margin;
    } else {
        pst_hosd->u08_lr_margin = u08_fix_margin;
        pst_hosd->u08_tb_margin = (ts_u08)u32_tb_margin;
    }
}


ts_s32 ts_lib_node_set_xy(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hnode, ts_u32 u32_left, ts_u32 u32_top)
{
    ts_osd_attr_s_t st_attr;
    ts_s32 s32_ret;

    if (ts_null == pst_hnode) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    s32_ret = ts_tdk_osd_get_attr(h_osd_dev, pst_hnode, (ts_osd_attr_s_t *)&st_attr);
    if (TS_OK == s32_ret) {
        st_attr.u16_left = (ts_u16)u32_left;
        st_attr.u16_top = (ts_u16)u32_top;

        s32_ret = ts_tdk_osd_set_attr(h_osd_dev, pst_hnode, (ts_osd_attr_s_t *)&st_attr);
    }
    
    return s32_ret;
}


ts_s32 ts_lib_osd_set_xy(ts_osd_s_t *pst_hosd, ts_u32 u32_left, ts_u32 u32_top)
{
    ts_osd_attr_s_t *pst_attr;
    ts_nodebuf_s_t *pst_cur_node;
    
    if (ts_null == pst_hosd) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    pst_cur_node = pst_hosd->pst_hnode;
    pst_attr = (ts_osd_attr_s_t *)&pst_hosd->st_osd_attr;

    ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);

    pst_attr->u16_left = (ts_u16)u32_left;
    pst_attr->u16_top = (ts_u16)u32_top;

    ts_tdk_osd_set_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);

    return TS_OK;
}


ts_s32 ts_lib_osd_set_visible(ts_osd_s_t *pst_hosd, ts_u32 u32_visible)
{
    ts_osd_attr_s_t *pst_attr;
    ts_nodebuf_s_t *pst_cur_node;
    
    if (ts_null == pst_hosd) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    pst_cur_node = pst_hosd->pst_hnode;
    pst_attr = (ts_osd_attr_s_t *)&pst_hosd->st_osd_attr;

    ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);

    if (u32_visible > 0)
        pst_attr->u08_visible = 1;
    else
        pst_attr->u08_visible = 0;

    ts_tdk_osd_set_attr(pst_hosd->h_osd_dev, pst_cur_node, pst_attr);

    return TS_OK;
}


ts_nodebuf_s_t *ts_lib_osd_8bit_create(ts_mdev_t h_osd_dev, ts_u32 u32_left, ts_u32 u32_top, ts_u32 u32_width, ts_u32 u32_height)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_osd;
    ts_u32 i, j, k;
    ts_u08 *pu08_ptr;
    ts_u08 u08_val;

    ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* OSD 坐标配置 */
    st_attr.u16_left = u32_left;
    st_attr.u16_top = u32_top;
    st_attr.u16_height = u32_height;
    st_attr.u16_width = u32_width;

    /* 像素位宽 */
    st_attr.u08_pix_bw = TS_PIXEL_BW_8BIT;

    /* 是否可显示 */
    st_attr.u08_visible = 1;

    /* 出现覆盖后的覆盖优先级, 0 最低 */
    st_attr.u08_priority = 0;

    /* OSD 或者 十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_OSD;

    /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
    st_attr.st_osd_cfg.s08_alpha = -1;

    /* 是否开启反色 */
    st_attr.st_osd_cfg.u08_auto_invert = 0;

    /* 前景色与背景色配置, 256 色不关注 */
    st_attr.st_osd_cfg.u08_font_color = 0;
    st_attr.st_osd_cfg.u08_bg_color = 0;

    pst_osd = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_osd)
        return ts_null;

    /* 读入文件库 */
    vision_log_debug("va=%x,sz=%d\n", pst_osd->u32_virt_addr, pst_osd->u32_size);
    pu08_ptr = (ts_u08 *)pst_osd->u32_virt_addr;
    u08_val = 0;
    k = 0;
    for (i = 0; i < u32_height; i++) {
        for (j = 0; j < u32_width; j++)
            *pu08_ptr ++ = u08_val;

        k++;
        if (k >= 8) {
            k = 0;
            u08_val++;
        }
    }

    return pst_osd;
}


ts_osd_s_t *ts_lib_osd_create(ts_mdev_t h_osd_dev, const ts_char *pc_name, ts_u32 u32_pix_bw, ts_u32 u32_width, 
                                    ts_u32 u32_line_height, ts_u32 u32_line_num, ts_u32 u32_box_width, ts_u32 u32_visible, ts_u32 u32_priority)
{
    ts_osd_attr_s_t *pst_attr;
    ts_u32 u32_fix_height, u32_lines, u32_size;
    ts_nodebuf_s_t *pst_osd_node;
    ts_osd_s_t *pst_hosd;

    if (u32_pix_bw > TS_PIXEL_BW_2BIT) {
        vision_log_err("%s\n", TS_EINVAL_STR);

        return ts_null;
    }

    /* 最少一行显示 */
    if (u32_line_num > 1)
        u32_lines = u32_line_num;
    else
        u32_lines = 1;

    /* 创建 osd 上下文 */
    u32_size = sizeof(ts_osd_s_t);

    /* 增加每行信息的内存空间, 需要去除 ts_osd_s_t 自带的一个行信息 */
    u32_size += sizeof(ts_osd_line_s_t) * (u32_lines - 1);

    pst_hosd = (ts_osd_s_t *)malloc(u32_size);
    if (ts_null == pst_hosd) {
        vision_log_err("%s:size = %d\n", TS_ENOMEM_STR, u32_size);

        return ts_null;
    }

    ts_cpl_memset32((ts_void *)pst_hosd, 0, (sizeof(ts_osd_s_t) >> 2));

    /* 此处仅供申请内存用，无需精确配置 */
    pst_attr = (ts_osd_attr_s_t *)&pst_hosd->st_osd_attr;

    pst_attr->u08_type = TS_OSD_TYPE_OSD;
    pst_attr->u16_width = (ts_u16)u32_width;

    if (u32_visible > 0)
        pst_attr->u08_visible = 1;
    else
        pst_attr->u08_visible = 0;

    if (u32_priority > 100)
        pst_attr->u08_priority = 100;
    else
        pst_attr->u08_priority = (ts_u08)u32_priority;

    /* 如果是多条 osd 合并显示，需要重新计算 osd 的真实高度 */
    u32_fix_height = u32_line_height * u32_lines;

    /* 如果有外框，需要加 n+1 个外框厚度 */
    if (u32_box_width > 0)
        u32_fix_height += u32_box_width * (u32_lines + 1);

    /* 修正参数 */
    pst_attr->u16_height = u32_fix_height;
    pst_attr->u08_pix_bw = u32_pix_bw;

    /* 使用默认参数 */
    pst_attr->st_osd_cfg.s08_alpha = sst_ctx.s08_def_alpha;
    pst_attr->st_osd_cfg.u08_auto_invert = sst_ctx.u08_auto_invert;
    pst_attr->st_osd_cfg.u08_font_color = sst_ctx.u08_def_font_color;
    pst_attr->st_osd_cfg.u08_bg_color = sst_ctx.u08_def_bg_color;

    vision_log_debug("pst_hosd = %p, %d,%d,%d\n", pst_hosd, sizeof(ts_osd_s_t), sizeof(ts_osd_line_s_t), u32_size);

    /* 创建节点 */
    pst_osd_node = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)pst_attr);
    if (ts_null == pst_osd_node) {
        vision_log_err("Create osd nodebuf fail!\n");

        free(pst_hosd);

        return ts_null;
    }

    vision_log_debug("pst_osd_node = %x, %d\n", pst_osd_node->u32_virt_addr, pst_osd_node->u32_size);

    pst_hosd->pc_name = pc_name;
    pst_hosd->pst_hnode = pst_osd_node;
    
    if (u32_box_width > 0)
        pst_hosd->u08_box_en = 1;

    pst_hosd->u08_box_width = u32_box_width;
    pst_hosd->u08_line_num = u32_lines;
    pst_hosd->u16_ro_height = u32_fix_height;
    pst_hosd->u08_ro_line_height = u32_line_height;

    pst_hosd->u08_box_color = sst_ctx.u08_def_box_color;
    pst_hosd->u08_lr_margin = sst_ctx.u08_def_lr_margin;
    pst_hosd->u08_tb_margin = sst_ctx.u08_def_tb_margin;

    pst_hosd->h_osd_dev = h_osd_dev;

    return pst_hosd;
}

