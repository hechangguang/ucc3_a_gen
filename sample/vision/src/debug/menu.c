/*
 * menu.c -- tdk 菜单生成例子
 */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_media.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/chip_def.h>
#include <uapi/osd_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/viu_v100.h>
#include <command.h>
#include <stdio.h>
#include "fontlib.h"


static ts_mdev_t shdl_osd_dev;
static ts_u32 su32_inited = 0;

/* 字符为 8x16 或者 16x16 ，再加上上下各两行留白 */
//#define MNU_LINE_MARGIN     (2)
//#define MNU_LINE_HEIGHT     (FONT_FULL_HEIGHT + MNU_LINE_MARGIN + MNU_LINE_MARGIN)

extern ts_nodebuf_s_t *ts_lib_fontl_gen_label(ts_mdev_t h_osd_dev, const ts_char *pc_str, \
    ts_fontl_lbl_atts_s_t *pst_attr);

enum {
    /* OSD 类型仅能使用前面 4 个 */
    OSD_COLOR_1 = 0,
    OSD_COLOR_2 = 1,
    OSD_COLOR_3 = 2,
    OSD_COLOR_4 = 3,

    /* 光标类型不会有大缓存，默认使用 4 bit 颜色深度，可以使用后续颜色 */
    OSD_COLOR_5 = 4,
    OSD_COLOR_6 = 5,
    OSD_COLOR_7 = 6,
    
    OSD_COLOR_BUTT,
};

static ts_rgb_s_t sast_mnu_rgbs[OSD_COLOR_SIZE] = {
    /* 白色， 透明度 0， 即完全不透明 */
    {255, 255, 255, 0},

    /* 黑色， 透明度 0, 即完全不透明 */
    {0, 0, 0, 0},

    /* 灰色 透明度 0, 即完全不透明 */
    {128, 128, 128, 0},

    /* 红色， 透明度0， 即完全不透明 */
    {255, 0, 0, 0},

    /* 白色， 透明度 0， 即完全不透明 */
    {255, 255, 255, 0},

    /* 蓝色， 透明度0， 即完全不透明 */
    {255, 0, 0, 0},
};


static ts_rgb_s_t sast_mnu_rgbs_cfg2[OSD_COLOR_SIZE] = {
    /* 黑色， 透明度 0， 即完全不透明 */
    {0, 0, 0, 0},

    /* 土黄色 透明度 0, 即完全不透明 */
    {223, 184, 93, 0},

    /* 淡黄色， 透明度0， 即完全不透明 */
    {252, 246, 180, 0},

    /* 蓝色， 透明度0， 即完全不透明 */
    {0, 0, 255, 0},

    /* 白色， 透明度 0， 即完全不透明 */
    {255, 255, 255, 0},

    /* 红色， 透明度0， 即完全不透明 */
    {255, 0, 0, 0},
};


#define MNU_FONT_COLOR          OSD_COLOR_1
#define MNU_BOX_COLOR           OSD_COLOR_1

#define MNU_MAIN_BG_COLOR       OSD_COLOR_2
#define MNU_SUB_BG_COLOR        OSD_COLOR_3
#define MNU_SEL_COLOR           OSD_COLOR_4

#define MNU_CURSOR_COLOR        OSD_COLOR_5

/* 演示菜单宽度 144 */
#define MNU_PIX_WIDTH       (144)

/* 边框宽度为 2 */
#define MNU_BOX_PIX_WIDTH   (2)

/* 字符与边框留空 6 个像素，凑齐 8 个像素，与 1bit 颜色深度兼容 */
#define MNU_MARGIN_WIDTH    (6)

/* 有效字符像素为 ： MNU_PIX_WIDTH - MNU_BOX_PIX_WIDTH - MNU_MARGIN_WIDTH */
#define MNU_FONT_PIX_WIDTH  (MNU_PIX_WIDTH - ((MNU_BOX_PIX_WIDTH + MNU_MARGIN_WIDTH) * 2))

/* 有效字符像素转成英文字符宽度(8x16)为除以 8 */
#define MNU_FONT_MAX_WIDTH  (MNU_FONT_PIX_WIDTH >> 3)

typedef struct mnu_ctx_s {
    /* 光标 */
    ts_nodebuf_s_t *pst_cursor;

    ts_osd_s_t *pst_sys;
    ts_osd_s_t *pst_img;

    /* 选中框 */
    ts_nodebuf_s_t *pst_mnu_active;

    /* img 的次级菜单 */
    ts_osd_s_t *pst_img_sub1;

    /* 6 行 osd 的公用双缓存 */
    ts_nodebuf_s_t *pst_line6_buf;
} mnu_ctx_s_t;

static mnu_ctx_s_t sst_ctx = {0};


/* img 的次级菜单各行定义 */
static ts_osd_line_s_t sast_img_sub1_lines[] = {
    /* 第一行： 混合:ON/CLOSE */
    {
        .u08_left_align_type = OSD_ALIGN_LEFT,
        .u08_left_font_num = 8,
        .u08_right_align_type = OSD_ALIGN_LEFT,

        /* 配置成最大，实际上会被修正到 (MNU_FONT_MAX_WIDTH - u08_left_font_num) */
        .u08_right_font_num = MNU_FONT_MAX_WIDTH
    },

    /* 第二行： DDE:xx */
    {
        .u08_left_align_type = OSD_ALIGN_LEFT,
        .u08_left_font_num = 8,
        .u08_right_align_type = OSD_ALIGN_LEFT,
        .u08_right_font_num = MNU_FONT_MAX_WIDTH
    },

    /* 第三行: PCOLOR:xx */
    {
        .u08_left_align_type = OSD_ALIGN_LEFT,
        .u08_left_font_num = 8,
        .u08_right_align_type = OSD_ALIGN_LEFT,
        .u08_right_font_num = MNU_FONT_MAX_WIDTH
    },
};


/* 创建一个多条合并的 osd 菜单 */
static ts_osd_s_t *menu_create_img_sub(ts_void)
{
    ts_osd_s_t *pst_hosd;
    ts_u32 u32_line_num;
    ts_s32 s32_ret;
    ts_u32 MNU_LINE_HEIGHT;
     if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        MNU_LINE_HEIGHT = 32+8;  
    }
    else
    {
        MNU_LINE_HEIGHT = 16+4; 
    }
    /* 创建 osd 区域， 设置成最大显示 6 行，多个 6 行显示的 OSD 缓冲可以互用，形成缓冲组 */
    pst_hosd = ts_lib_osd_create(shdl_osd_dev, "sub_img", TS_PIXEL_BW_2BIT, MNU_PIX_WIDTH, MNU_LINE_HEIGHT, 6, MNU_BOX_PIX_WIDTH, 1, 9);
    if (ts_null == pst_hosd) {
        printf("Create osd fail.\n");

        return ts_null;
    }

    /* 创建一个 6 行的缓冲，用于同类型的显示切换 */
    sst_ctx.pst_line6_buf = ts_lib_osd_create_dbbuf(pst_hosd);

    /* 配置行信息 */
    u32_line_num = sizeof(sast_img_sub1_lines) / sizeof(ts_osd_line_s_t);
    s32_ret = ts_lib_osd_set_line_attr(pst_hosd, (ts_osd_line_s_t *)&sast_img_sub1_lines, u32_line_num);
    if (s32_ret != TS_OK)
        return ts_null;

    /* 仅配置显示参数,本 osd 最大显示 6 行，本次显示为 3 行 */
    ts_lib_osd_set(pst_hosd, ts_null, 20, 44, ts_null, 3);

    /* 显示第一行 混合:ON/CLOSE */
    ts_lib_osd_update(pst_hosd, ts_null, "混合", 0, MNU_UPD_LEFT);
    ts_lib_osd_update(pst_hosd, ts_null, ":CLOSE", 0, MNU_UPD_RIGHT);

    /* 显示第二行 DDE:0..100 */
    ts_lib_osd_update(pst_hosd, ts_null, "DDE", 1, MNU_UPD_LEFT);
    ts_lib_osd_update(pst_hosd, ts_null, ":10", 1, MNU_UPD_RIGHT);

    /* 显示第三行 PCOLOR:0..1 */
    ts_lib_osd_update(pst_hosd, ts_null, "PCOLOR", 2, MNU_UPD_LEFT);
    ts_lib_osd_update(pst_hosd, ts_null, ":0", 2, MNU_UPD_RIGHT);

    /* 直接设置成显示，并且优先级为 9 */
    //ts_lib_osd_set_visible(pst_hosd, 1, 9);

    return pst_hosd;
}

/* 创建主菜单 */
static ts_osd_s_t *menu_create_main(ts_u32 u32_type)
{
    ts_char *pc_name;
    ts_osd_s_t *pst_hosd;
    ts_osd_line_s_t st_line;
    ts_s32 s32_ret;

    if (0 == u32_type)
        pc_name = "main_img";
    else if (1 == u32_type)
        pc_name = "main_sys";
    else
        return ts_null;

    ts_u32 MNU_LINE_HEIGHT;
     if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        MNU_LINE_HEIGHT = 32+8;  
    }
    else
    {
        MNU_LINE_HEIGHT = 16+4; 
    }    
    /* 创建 osd 区域, 仅一行即可 */
    pst_hosd = ts_lib_osd_create(shdl_osd_dev, (const ts_char *)pc_name, TS_PIXEL_BW_2BIT, 
                                    MNU_PIX_WIDTH, MNU_LINE_HEIGHT, 1, MNU_BOX_PIX_WIDTH, 0, 10);
    if (ts_null == pst_hosd) {
        printf("Create osd fail.\n");

        return ts_null;
    }

    /* 配置行信息 */
    st_line.u08_left_align_type = OSD_ALIGN_CENTER;
    st_line.u08_left_font_num = MNU_FONT_MAX_WIDTH;
    st_line.u08_right_align_type = 0;
    st_line.u08_right_font_num = 0;
    s32_ret = ts_lib_osd_set_line_attr(pst_hosd, (ts_osd_line_s_t *)&st_line, 1);
    if (s32_ret != TS_OK)
        return ts_null;

    /* 配置显示坐标，并写入字符 */
    if (0 == u32_type)
        ts_lib_osd_set(pst_hosd, ts_null, 20, 20, "图像", 1);
    else if (1 == u32_type)
        ts_lib_osd_set(pst_hosd, ts_null, 172, 20, "系统", 1);

    /* 设置成显示 */
    ts_lib_osd_set_visible(pst_hosd, 1);

    return pst_hosd;
}

/* 创建一个光标 */
static ts_void menu_create_cursor(ts_void)
{
    ts_fontl_cursor_attr_s_t st_cursor_attr;

    ts_cpl_memset32((ts_void *)&st_cursor_attr, 0, (sizeof(ts_fontl_cursor_attr_s_t) >> 2));

    /* 坐标根据需要修改 */
    st_cursor_attr.u16_center_left = 200;
    st_cursor_attr.u16_center_top = 200;

#if 1
    /* 中心点为 1x1 */
    st_cursor_attr.u16_height = 1;
    st_cursor_attr.u16_width = 1;

    st_cursor_attr.u08_thick = 16;

    st_cursor_attr.u08_margin = 4;
    st_cursor_attr.u08_blink = 60;
    st_cursor_attr.u08_center_hide = 0;
#else
    /* 中心点为 1x1 */
    st_cursor_attr.u16_height = 1;
    st_cursor_attr.u16_width = 1;

    st_cursor_attr.u08_thick = 16;

    st_cursor_attr.u08_margin = 0;
    st_cursor_attr.u08_blink = 0;
    st_cursor_attr.u08_center_hide = 0;
#endif

    st_cursor_attr.u08_visiable = 1;
    st_cursor_attr.u08_priority = 6;

    /* 光标固定颜色5 */
    st_cursor_attr.u08_center_color = MNU_CURSOR_COLOR;
    st_cursor_attr.u08_cursor_color = MNU_CURSOR_COLOR;
    sst_ctx.pst_cursor = ts_lib_fontl_gen_cursor(shdl_osd_dev, &st_cursor_attr);
}


/* 创建选中框 */
static ts_void menu_create_sel(ts_u32 u32_left, ts_u32 u32_top)
{
    ts_fontl_rect_attr_s_t st_rect_attr;

    /* 生成选择框 */
    st_rect_attr.u16_left = u32_left;
    st_rect_attr.u16_top = u32_top;
    st_rect_attr.u16_width = 144;
    st_rect_attr.u16_height = 24;
    st_rect_attr.u08_thick = 2;
    st_rect_attr.u08_visiable = 0;
    st_rect_attr.u08_priority = 4;

    st_rect_attr.u08_color = MNU_SEL_COLOR;

    sst_ctx.pst_mnu_active = ts_lib_fontl_gen_rectangle(shdl_osd_dev, (ts_fontl_rect_attr_s_t *)&st_rect_attr);
}

/* 选中状态设置 */
static ts_void mnu_set_check(ts_osd_s_t *pst_hosd, ts_u32 u32_line_idx)
{
    ts_osd_attr_s_t st_mnu_attr, st_act_attr;
    ts_u16 u16_top, u16_height;

    if (u32_line_idx >= (ts_u32)pst_hosd->u08_line_num) {
        printf("line inx is invalid\n");

        return;
    }
    ts_u32 MNU_LINE_HEIGHT;
    if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        MNU_LINE_HEIGHT = 32+8;  
    }
    else
    {
        MNU_LINE_HEIGHT = 16+4; 
    }
    ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, pst_hosd->pst_hnode, (ts_osd_attr_s_t *)&st_mnu_attr);
    ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);

    st_act_attr.u16_left = st_mnu_attr.u16_left;

    /* 根据行号计算偏移位置 */
    u16_top = st_mnu_attr.u16_top;
    u16_height = MNU_LINE_HEIGHT;
    if (pst_hosd->u08_box_en > 0) {
        u16_top += pst_hosd->u08_box_width * u32_line_idx;
        u16_height += pst_hosd->u08_box_width * 2;
    }

    u16_top += MNU_LINE_HEIGHT * u32_line_idx;
    
    st_act_attr.u16_top = u16_top;

    st_act_attr.u08_visible = 1;
    ts_tdk_osd_set_attr(pst_hosd->h_osd_dev, sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);
}


static TS_FUNC_UNUSED ts_void mnu_set_uncheck(ts_void)
{
    ts_osd_attr_s_t st_act_attr;
    ts_osd_s_t *pst_hosd;

    pst_hosd = sst_ctx.pst_img_sub1;

    ts_tdk_osd_get_attr(pst_hosd->h_osd_dev, sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);

    st_act_attr.u08_visible = 0;
    ts_tdk_osd_set_attr(pst_hosd->h_osd_dev, sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);
}


/* u32_reset = 1: 需要关闭显示 */
static ts_void menu_change_palette(ts_u32 u32_cfg, ts_u32 u32_reset)
{
    ts_rgb_s_t *pst_rgb;

    if (0 == u32_cfg)
        pst_rgb = (ts_rgb_s_t *)&sast_mnu_rgbs;
    else
        pst_rgb = (ts_rgb_s_t *)&sast_mnu_rgbs_cfg2;

    if (u32_reset > 0) {
        /* 关闭显示 */
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_ALL_OFF);

        /* 等待生效, 帧率越高可以休眠时间越少 */
        ts_cpl_msleep(100);

        /* 重新配置调色板 */
        ts_tdk_osd_set_palette(pst_rgb, OSD_COLOR_SIZE);

        /* 重新显示 */
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
    }
}


static ts_nodebuf_s_t * menu_create_cursor_blink(ts_void)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* 中心点坐标以及宽高 */
    st_attr.u16_left = 220;
    st_attr.u16_top = 110;
    st_attr.u16_height = 1;
    st_attr.u16_width = 1;

    /* 状态可见 */
    st_attr.u08_visible = 1;

    /* 最高优先级 */
    st_attr.u08_priority = 99;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_BLINK;

    /* 中心点水平方向与十字的间隔大小 */
    st_attr.st_misc_cfg.u08_margin_h = 4;

    /* 中心点垂直方向与十字的间隔大小 */
    st_attr.st_misc_cfg.u08_margin_v = 4;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = 12;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = 16;

    /* 闪烁间隔帧率, 0 关闭 */
    st_attr.st_misc_cfg.u08_blink = 10;

    /* 中心点是否隐藏, > 0 隐藏 */
    st_attr.st_misc_cfg.u08_center_hide = 0;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null) {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = OSD_COLOR_5;
        pu08_buf[1] = OSD_COLOR_5;
        pu08_buf[2] = OSD_COLOR_5;
        pu08_buf[3] = OSD_COLOR_5;
        pu08_buf[4] = OSD_COLOR_5;
    }

    return pst_cursor;
}

static ts_nodebuf_s_t * menu_create_cursor_rect(ts_void)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* 中心点坐标以及宽高 */
    st_attr.u16_left = 200;
    st_attr.u16_top = 300;
    st_attr.u16_height = 30;
    st_attr.u16_width = 20;

    /* 状态可见 */
    st_attr.u08_visible = 1;

    /* 最高优先级 */
    st_attr.u08_priority = 90;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_RECT;

    /* 中心矩形的水平线粗 */
    st_attr.st_misc_cfg.u08_margin_h = 6;

    /* 中心矩形的垂直线粗 */
    st_attr.st_misc_cfg.u08_margin_v = 6;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = 18;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = 20;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null) {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = OSD_COLOR_5;
        pu08_buf[1] = OSD_COLOR_5;
        pu08_buf[2] = OSD_COLOR_5;
        pu08_buf[3] = OSD_COLOR_5;
        pu08_buf[4] = OSD_COLOR_5;
        pu08_buf[5] = OSD_COLOR_5;
        pu08_buf[6] = OSD_COLOR_5;
        pu08_buf[7] = OSD_COLOR_5;
    }

    return pst_cursor;
}


static ts_nodebuf_s_t *menu_create_rectangle(ts_void)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_rect;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    st_attr.u16_left = 100;
    st_attr.u16_top = 400;
    st_attr.u16_width = 64;
    st_attr.u16_height = 24;

    st_attr.u08_visible = 1;
    st_attr.u08_priority = 12;

    st_attr.u08_type = TS_OSD_TYPE_RECT;

    /* 配置水平方向的线条粗细大小 */
    st_attr.st_misc_cfg.u08_margin_h = 2;

    /* 配置垂直方向的线条粗细大小 */
    st_attr.st_misc_cfg.u08_margin_v = 4;

    pst_rect = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_rect)
        return ts_null;

    if (pst_rect != ts_null) {
        pu08_buf = (ts_u08 *)pst_rect->u32_virt_addr;
        pu08_buf[0] = OSD_COLOR_5;
        pu08_buf[1] = OSD_COLOR_5;
        pu08_buf[2] = OSD_COLOR_5;
        pu08_buf[3] = OSD_COLOR_5;
    }

    return pst_rect;
}


static TS_FUNC_UNUSED ts_nodebuf_s_t *menu_create_osd(ts_mdev_t h_osd_dev, ts_u32 u32_left, ts_u32 u32_top)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_osd;
    ts_u32 i, j, k, u32_width;
    ts_u08 *pu08_ptr;
    ts_u08 u08_val;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* OSD 坐标配置 */
    st_attr.u16_left = u32_left;
    st_attr.u16_top = u32_top;
    st_attr.u16_height = 24;
    st_attr.u16_width = 120;

    /* 像素位宽, 推荐 2bit */
    st_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;

    /* 设置成可见状态 */
    st_attr.u08_visible = 1;

    /* 出现覆盖后的覆盖优先级, 0 最低 */
    st_attr.u08_priority = 0;

    /* OSD */
    st_attr.u08_type = TS_OSD_TYPE_OSD;

    /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
    st_attr.st_osd_cfg.s08_alpha = -1;

    /* 是否开启反色 */
    st_attr.st_osd_cfg.u08_auto_invert = 0;

    /* 一般用于字符型 OSD */
    st_attr.st_osd_cfg.u08_font_color = 0;
    st_attr.st_osd_cfg.u08_bg_color = 0;

    pst_osd = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_osd)
        return ts_null;

    /* 填充成 2bit 色条 */
    u08_val = 0;
    k = 0;

    /* 每个像素占用 2bit */
    u32_width = st_attr.u16_width >> 2;
    for (i = 0; i < st_attr.u16_height; i++) {
        pu08_ptr = (ts_u08 *)pst_osd->u32_virt_addr;
        pu08_ptr += i * u32_width;

        for (j = 0; j < u32_width; j++)
            pu08_ptr[j] = u08_val;
    
        k++;
        if (k >= 6) {
            k = 0;
            u08_val += 0x55;
        }
    }

    return pst_osd;
}


#define MENU_DBG
static ts_s32 menu_process(ts_void *pv_param)
{
    ts_u32 u32_disp_w, u32_disp_h;
    ts_u32 u32_idx, u32_dde_val, u32_chgs, u32_cfg;
    ts_char ac_dde_cfg[8];

    TS_UNUSED(pv_param);

#if 0
    ts_tdk_osd_get_disp_size(TS_OSD_DISP_MDEV0, (ts_u32 *)&u32_disp_w, (ts_u32 *)&u32_disp_h);
    ts_tdk_osd_get_disp_size(TS_OSD_CVBS_MDEV0, (ts_u32 *)&u32_cvbs_w, (ts_u32 *)&u32_cvbs_h);

    printf("disp w,h=%d,%d\n", u32_disp_w, u32_disp_h);
    printf("cvbs w,h=%d,%d\n", u32_cvbs_w, u32_cvbs_h);
#else
    ts_tdk_osd_get_disp_size(shdl_osd_dev, (ts_u32 *)&u32_disp_w, (ts_u32 *)&u32_disp_h);

    printf("disp w,h=%d,%d\n", u32_disp_w, u32_disp_h);
#endif

    /* 设置全部 OSD 的统一属性 */
    menu_change_palette(0, 0);
    ts_lib_osd_set_color(ts_null, TS_PIXEL_BW_2BIT, MNU_FONT_COLOR, MNU_MAIN_BG_COLOR, MNU_BOX_COLOR, 50, 0);
    //ts_lib_osd_set_margin(ts_null, TS_PIXEL_BW_2BIT, 8, MNU_LINE_MARGIN);
  if(get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        ts_lib_osd_set_margin(ts_null, TS_PIXEL_BW_2BIT, 8, 4);  
    }
    else
    {
        ts_lib_osd_set_margin(ts_null, TS_PIXEL_BW_2BIT, 8, 2);
    }
    sst_ctx.pst_img = menu_create_main(0);
    sst_ctx.pst_sys = menu_create_main(1);

    menu_create_cursor();
    menu_create_cursor_blink();
    menu_create_cursor_rect();
    menu_create_sel(0, 0);
    menu_create_rectangle();

    ts_lib_osd_set_color(ts_null, TS_PIXEL_BW_2BIT, MNU_FONT_COLOR, MNU_SUB_BG_COLOR, MNU_BOX_COLOR, 50, 0);
    sst_ctx.pst_img_sub1 = menu_create_img_sub();
    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

    u32_idx = 1;
    u32_dde_val = 0;
    u32_chgs = 0;
    u32_cfg = 1;
    while (1) {
        ts_cpl_msleep(5000);
        mnu_set_check(sst_ctx.pst_img_sub1, 0);

        /* 演示子菜单参数来回切换, 因为只有一个双缓存，同时只能修改一条 OSD */
        if (0 == u32_idx) {
            u32_idx = 1;

            /* 混合:CLOSE */
            sst_ctx.pst_line6_buf = ts_lib_osd_update(sst_ctx.pst_img_sub1, sst_ctx.pst_line6_buf, ":CLOSE", 0, MNU_UPD_RIGHT);
        } else {
            u32_idx = 0;

            /* 混合:ON */
            sst_ctx.pst_line6_buf = ts_lib_osd_update(sst_ctx.pst_img_sub1, sst_ctx.pst_line6_buf, ":ON", 0, MNU_UPD_RIGHT);
        }

        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        ts_cpl_msleep(5000);

        mnu_set_check(sst_ctx.pst_img_sub1, 1);
        u32_dde_val++;
        if (u32_dde_val > 100)
            u32_dde_val = 0;
        ts_cpl_sprintf((ts_char *)&ac_dde_cfg, ":%d", u32_dde_val);
        sst_ctx.pst_line6_buf = ts_lib_osd_update(sst_ctx.pst_img_sub1, sst_ctx.pst_line6_buf, (ts_char *)&ac_dde_cfg, 1, MNU_UPD_RIGHT);
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        ts_cpl_msleep(5000);

        ts_lib_osd_set_visible(sst_ctx.pst_img, 0);
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        ts_lib_osd_set_xy(sst_ctx.pst_img, 200, 200);

        ts_cpl_msleep(5000);

        ts_lib_osd_set_visible(sst_ctx.pst_img, 1);
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        ts_cpl_msleep(5000);

        ts_lib_osd_set_visible(sst_ctx.pst_img, 0);

        ts_lib_osd_set_visible(sst_ctx.pst_sys, 0);


        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        ts_lib_osd_set_xy(sst_ctx.pst_img, 20, 20);

        ts_cpl_msleep(5000);

        ts_lib_osd_set_visible(sst_ctx.pst_img, 1);
        ts_lib_osd_set_visible(sst_ctx.pst_sys, 1);
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);

        u32_chgs++;
        if (u32_chgs >= 5) {
            u32_chgs = 0;
            if (0 == u32_cfg)
                u32_cfg = 1;
            else
                u32_cfg = 0;

            menu_change_palette(u32_cfg, 1);

            ts_cpl_msleep(2000);
        }
    }

    return 0;
}


ts_void menu_init(ts_void)
{
    ts_cpl_task_s_t st_osd_task;
    ts_osd_ch_attr_s_t st_osd_disp_ch_attr;

    if (0 == su32_inited) {
        #if 1
            st_osd_disp_ch_attr.pst_rgb = (const ts_rgb_s_t *)&sast_mnu_rgbs_cfg2;
            st_osd_disp_ch_attr.u16_rgb_num = OSD_COLOR_SIZE;
            st_osd_disp_ch_attr.u08_pixel_bw = TS_PIXEL_BW_2BIT;
        #else
            st_osd_disp_ch_attr.pst_rgb = (const ts_rgb_s_t *)&sast_rgb256;
            st_osd_disp_ch_attr.u16_rgb_num = 256;
            st_osd_disp_ch_attr.u08_pixel_bw = TS_PIXEL_BW_8BIT;
        #endif

        st_osd_disp_ch_attr.u08_fill_pixel = 0;
        st_osd_disp_ch_attr.u32_merge_size = 32 * 1024;

        #if 0
            shdl_osd_dev = TS_OSD_DISP_MDEV0;
        #else
            shdl_osd_dev = TS_OSD_ALL_MDEV0;
        #endif

        ts_tdk_osd_set_ch_attr((const ts_osd_ch_attr_s_t *)&st_osd_disp_ch_attr);

        su32_inited = 1;

        #if 1
            ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_osd_task, (ts_cpl_kthread_t)menu_process, ts_null, "osd", 20, 4096);
        #else
            menu_process(0);
        #endif
    }
}


/* 
    osd on
    osd off
*/
TS_DECLARE_CMD(mnu)
{
    menu_init();

    return 0;
}

/// 创建一个闪烁的十字光标
static ts_nodebuf_s_t * create_cursor_blink(ts_void)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;
    ts_u32 u32_disp_w, u32_disp_h;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    ts_tdk_osd_get_disp_size(TS_OSD_DISP_MDEV0, (ts_u32 *)&u32_disp_w, (ts_u32 *)&u32_disp_h);
    //vision_log_debug("disp w=%d , h=%d\n", u32_disp_w, u32_disp_h);
   // vision_log_debug("u16_left=%d , u16_top=%d \n", u32_disp_w/2, u32_disp_h/2);
    /* 中心点坐标以及宽高 */
    st_attr.u16_left = u32_disp_w/2;
    st_attr.u16_top = u32_disp_h/2;
    st_attr.u16_height = 1;
    st_attr.u16_width = 1;

    /* 状态可见 */
    st_attr.u08_visible = 1;

    /* 最高优先级 */
    st_attr.u08_priority = 99;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_BLINK;

    /* 中心点水平方向与十字的间隔大小 */
    st_attr.st_misc_cfg.u08_margin_h = 4;

    /* 中心点垂直方向与十字的间隔大小 */
    st_attr.st_misc_cfg.u08_margin_v = 4;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = 12;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = 16;

    /* 闪烁间隔帧率, 0 关闭 */
    st_attr.st_misc_cfg.u08_blink = 10;

    /* 中心点是否隐藏, > 0 隐藏 */
    st_attr.st_misc_cfg.u08_center_hide = 0;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null) {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = OSD_COLOR_5;
        pu08_buf[1] = OSD_COLOR_5;
        pu08_buf[2] = OSD_COLOR_5;
        pu08_buf[3] = OSD_COLOR_5;
        pu08_buf[4] = OSD_COLOR_5;
    }

    return pst_cursor;
}

// 创建一个光标显示控制函数，演示闪烁光标的显示和隐藏
ts_void create_cursor_display(ts_void)
{
    static ts_u08 u08_cursor_display_state = 0;
 // menu_init();
    if(u08_cursor_display_state == 0) 
    {
        create_cursor_blink();//居中闪烁十字光标的创建
        ts_lib_osd_set_color(ts_null, TS_PIXEL_BW_2BIT, MNU_FONT_COLOR, MNU_SUB_BG_COLOR, MNU_BOX_COLOR, 50, 0);
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
    }
    else  if(u08_cursor_display_state == 1) 
    {
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_ALL_OFF);//关闭显示
    }
    else if(u08_cursor_display_state == 2) 
    {
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);//重新显示
        u08_cursor_display_state=0;
    }

    u08_cursor_display_state++;
}
