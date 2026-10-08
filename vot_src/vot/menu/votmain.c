/*
 * display.c -- tdk 菜单生成例子
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
#include <uapi/wdt_v100.h>
#include <command.h>
#include <stdio.h>
#include "fontlib.h"
#include <api.h>
#include <uapi/upl_v100.h>

#include "usermenu.h"
#include "votmenu.h"
#include "votmain.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <menu/testmenufunction.h>
#include <fontlib_vot.h>
#include <snr_comm_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <common/common.h>
#include <unistd.h>
#include <snr_comm.h>
#include <menu/usermenufunction.h>
#include <fontlib_vot.h>
#include <common/TempCal.h>

vot_hbp_ctx_s_t vot_uart1_ctx;
vot_hbp_ctx_s_t vot_uart0_ctx;

extern DetPara detPara;

// extern COM_data com_data;

ts_mdev_t shdl_osd_dev;
static ts_u32 su32_inited = 0;
SysCtrlInfo_s_t global_sys_ctrl_info;

ts_void sffc_off(ts_void);
ts_void sffc_on_m(ts_s32 s32_v);
ts_void sffc_on_a(ts_void);

extern ts_s32 ts_lib_fontl_clear_label_info(ts_nodebuf_s_t *pst_lbl);

extern ts_nodebuf_s_t *ts_lib_fontl_gen_label(ts_mdev_t h_osd_dev, const ts_char *pc_str,
                                              ts_fontl_lbl_atts_s_t *pst_attr);

// static ts_u32 u32_framebbuffer_addr=0;
// ts_nodebuf_s_t *global_sys_ctrl_info.pst_framebuffer_osd=NULL;

ts_bool ring_buffer_update_write(ring_buffer_t *rb, ts_u08 *data_in, ts_u32 data_len);




#if 1
static ts_rgb_s_t sast_mnu_rgbs_vot[VOT_OSD_COLOR_MAX] = {
    /* 白色， 透明度 0， 即完全不透明 */
    {255, 255, 255, 0},

    /* 黑色， 透明度 0, 即完全透明 */
    {0, 0, 0, 255},

    /* 灰色 透明度 0, 即完全不透明 */
    {128, 128, 128, 0},

    /* 红色， 透明度0， 即完全不透明 */
    {255, 0, 0, 0},

    /* 明黄色， 透明度 0， 即完全不透明 */
    {255, 255, 0, 0},

    /* 绿色， 透明度0， 即完全不透明 */
    {0, 255, 0, 0},

    /* 蓝色， 透明度0， 即完全不透明 */
    {0, 0, 255, 0},

    /* 品红色， 透明度0， 即完全不透明 */
    {255, 0, 255, 0},
};

static ts_rgb_s_t sast_mnu_rgbs_cfg2_vot[OSD_COLOR_SIZE] = {
    /* 黑色， 透明度 0， 即完全不透明 */
    {255, 128, 0, 0},

    /* 土黄色 透明度 0, 即完全不透明 */
    {223, 184, 93, 255},

    /* 淡黄色， 透明度0， 即完全不透明 */
    {252, 246, 180, 0},

    /* 蓝色， 透明度0， 即完全不透明 */
    {0, 0, 255, 0},

    /* 白色， 透明度 0， 即完全不透明 */
    {255, 255, 255, 0},

    /* 红色， 透明度0， 即完全不透明 */
    {255, 0, 0, 0},
};

// /* 更新调色板后，需要重新配置一下，用于使能驱动刷新 */
// static ts_void mnu_refresh_chk(ts_void)
// {
//     ts_osd_attr_s_t st_act_attr;

//     //ts_tdk_osd_get_attr(shdl_osd_dev,sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);

//     st_act_attr.u08_visible = 1;
//     //ts_tdk_osd_set_attr(sst_ctx.pst_mnu_active, (ts_osd_attr_s_t *)&st_act_attr);
// }

/* u32_reset = 1: 需要关闭显示 */
ts_void menu_change_palette_vot(ts_u32 u32_cfg, ts_u32 u32_reset)
{
    ts_rgb_s_t *pst_rgb;

    if (0 == u32_cfg)
        pst_rgb = (ts_rgb_s_t *)&sast_mnu_rgbs_vot;
    else
        pst_rgb = (ts_rgb_s_t *)&sast_mnu_rgbs_cfg2_vot;

    if (u32_reset > 0)
    {
        /* 等待生效, 帧率越高可以休眠时间越少 */
        ts_cpl_msleep(100);

        /* 重新配置调色板 */
        ts_tdk_osd_set_palette(pst_rgb, OSD_COLOR_SIZE);


    }
}
#endif

/**
 * @description:
 * @param {ts_mdev_t} h_osd_dev osd通道，取值 TS_OSD_CVBS_MDEV0  或者  TS_OSD_DISP_MDEV0
 * @param {ts_osd_attr_s_t}     osd属性
 * @return {ts_nodebuf_s_t}     返回的osdbuf节点，后续的osd显示操作，都是通过这个结构来操作的
 */
ts_nodebuf_s_t *osd_node_init(ts_mdev_t h_osd_dev, ts_osd_attr_s_t* ptr_st_attr)
{
    // ts_u32 u32_framebbuffer_addr_t;

    ts_nodebuf_s_t *pst_lbl;

    if(ts_null == ptr_st_attr)
        return ts_null;

    vision_log_warn("width = %d %d %d\n", ptr_st_attr->u16_width, ptr_st_attr->u16_height, ptr_st_attr->st_osd_cfg.s08_alpha);

    pst_lbl = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)ptr_st_attr);
    if (ts_null == pst_lbl)
        return ts_null;


    return pst_lbl;
}

/**
 * @description: 创建一个正方形光标
 * @param {*}
 * @return {ts_nodebuf_s_t *}
 */
ts_nodebuf_s_t *vot_create_cursor_rect(ts_void)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* 中心点坐标以及宽高 */
    st_attr.u16_left = 320;
    st_attr.u16_top = 256;
    st_attr.u16_height = 51;
    st_attr.u16_width = 51;

    /* 状态可见 */
    st_attr.u08_visible = 1;

    /* 最高优先级 */
    st_attr.u08_priority = 90;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_RECT; // TS_OSD_TYPE_RECT;//// TS_OSD_TYPE_CURSOR_RECT;

    /* 中心矩形的水平线粗 */
    st_attr.st_misc_cfg.u08_margin_h = 1;

    /* 中心矩形的垂直线粗 */
    st_attr.st_misc_cfg.u08_margin_v = 1;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = 0;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = 0;

    st_attr.st_misc_cfg.u08_blink = 0;

    st_attr.st_misc_cfg.u08_center_hide = 0;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null)
    {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = 4;
        pu08_buf[1] = 4;
        pu08_buf[2] = 4;
        pu08_buf[3] = 4;
        pu08_buf[4] = 4;
        pu08_buf[5] = 4;
        pu08_buf[6] = 4;
        pu08_buf[7] = 4;
    }

    // vot_osd_node_clear_info(pst_cursor );
    return pst_cursor;
}

ts_nodebuf_s_t* votCreate_cursor_SpecifiedColor(ts_u08 mColor) {
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

	ts_u16 new_width, new_height;
	v_getDector_DisplayResolution(&new_width, &new_height);
    /* 中心点坐标以及宽高 */
    st_attr.u16_left = new_width / 2;
    st_attr.u16_top = new_height / 2;
    st_attr.u16_height = 1;
    st_attr.u16_width = 1;

    /* 状态可见 */
    st_attr.u08_visible = 1;

    /* 最高优先级 */
    st_attr.u08_priority = 90;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_BLINK;//TS_OSD_TYPE_CURSOR_RECT;

    /* 中心矩形的水平线粗 */
    st_attr.st_misc_cfg.u08_margin_h = 1;

    /* 中心矩形的垂直线粗 */
    st_attr.st_misc_cfg.u08_margin_v = 1;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = 7;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = 7;

    st_attr.st_misc_cfg.u08_blink = 0;

    st_attr.st_misc_cfg.u08_center_hide = 1;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null) {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = mColor;
        pu08_buf[1] = mColor;
        pu08_buf[2] = mColor;
        pu08_buf[3] = mColor;
        pu08_buf[4] = mColor;
        pu08_buf[5] = mColor;
        pu08_buf[6] = mColor;
        pu08_buf[7] = mColor;
    }
    return pst_cursor;
}

ts_nodebuf_s_t *vot_create_cursor_dot(ts_u08 u08_len_h,ts_u08 u08_len_v ,ts_u08 center_color, ts_u08 cursor_color, ts_u08 center_hide , ts_u08 u08_priority)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_cursor;
    ts_u08 *pu08_buf;

    ts_cpl_memset32((ts_void *)&st_attr, 0, (sizeof(ts_osd_attr_s_t) >> 2));

    /* 中心点坐标以及宽高 */
    st_attr.u16_left = 300;
    st_attr.u16_top = 256;
    st_attr.u16_height = 1;
    st_attr.u16_width = 1;

    /* 状态可见 */
    st_attr.u08_visible = 0;

    /* 最高优先级 */
    st_attr.u08_priority = u08_priority;

    /* 类型为带闪烁十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_CURSOR_BLINK; // TS_OSD_TYPE_CURSOR_RECT;

    /* 中心矩形的水平线粗 */
    st_attr.st_misc_cfg.u08_margin_h = 2;

    /* 中心矩形的垂直线粗 */
    st_attr.st_misc_cfg.u08_margin_v = 2;

    /* 水平方向十字的长度 */
    st_attr.st_misc_cfg.u08_len_h = u08_len_h;

    /* 垂直方向与十字的长度 */
    st_attr.st_misc_cfg.u08_len_v = u08_len_v;

    st_attr.st_misc_cfg.u08_blink = 0;

    st_attr.st_misc_cfg.u08_center_hide = center_hide;

    /* 创建节点，并且绑定 */
    pst_cursor = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(TS_OSD_ALL_MDEV0, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_cursor)
        return ts_null;

    if (pst_cursor != ts_null)
    {
        pu08_buf = (ts_u08 *)pst_cursor->u32_virt_addr;

        /* 颜色配置 */
        pu08_buf[0] = center_color;
        pu08_buf[1] = cursor_color;
        pu08_buf[2] = cursor_color;
        pu08_buf[3] = cursor_color;
        pu08_buf[4] = cursor_color;
        pu08_buf[5] = cursor_color;
        pu08_buf[6] = cursor_color;
        pu08_buf[7] = cursor_color;
    }
  
    return pst_cursor;
}


void init_hst_osd_handler(void)
{
    global_sys_ctrl_info.m_hst_min_cursor_osd_dot = vot_create_cursor_dot(6,6 , OSD_COLOR_WHITE,  OSD_COLOR_WHITE, 1 , 91);
    global_sys_ctrl_info.m_hst_max_cursor_osd_dot = vot_create_cursor_dot(6,6 , OSD_COLOR_YELLOW,  OSD_COLOR_YELLOW, 1 ,91);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_hst_min_cursor_osd_dot, 1);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_hst_max_cursor_osd_dot, 1);
    
    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}

/** 
 * @description: cvbs设备相关的 osd和cursor全局句柄初始化
 * @return {void}
 */
void init_cvbs_osd_handler(void)
{

    ts_u08 osd_type  =  vot_menu_osd_type();
	if(osd_type==VOT_MENU_OSD_RES_1280)
    {        
        {
            static ts_osd_attr_s_t st_osd_attr;
            /* OSD 坐标配置 */
            st_osd_attr.u16_left = 0;
            st_osd_attr.u16_top = 500;    // osd 是矩形框是0,500点开始，矩形框是1280x50
            st_osd_attr.u16_height = 200; // 三行信息显示，一行显示初始化提示信息y=0（提示信息或者设备信息），第二行显示版本信息y=50，第三行显示菜单二的参数y=100,第四行y=150（设备信息）
            st_osd_attr.u16_width = 1280;
            
            /* 固定 2bit 颜色深度 */
            st_osd_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
            /* 是否可显示 */
            st_osd_attr.u08_visible = 1;
            /* 出现覆盖后的覆盖优先级, 0 最低 */
            st_osd_attr.u08_priority = 50;
            /* OSD 或者 十字光标 */
            st_osd_attr.u08_type = TS_OSD_TYPE_OSD;
            /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
            st_osd_attr.st_osd_cfg.s08_alpha = -1;
            /* 是否开启反色 */
            st_osd_attr.st_osd_cfg.u08_auto_invert = 0;
            /* 前景色与背景色配置 */
            st_osd_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
            st_osd_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
            // vision_log_debug("width = %d %d %d\n", st_osd_attr.u16_width, st_osd_attr.u16_height, st_osd_attr.st_osd_cfg.s08_alpha);
            global_sys_ctrl_info.m_bottom_info_osd_node = osd_node_init(shdl_osd_dev, &st_osd_attr);
        }
        {
            static ts_osd_attr_s_t st_osd_attr1280;
            /* OSD 坐标配置 */
            st_osd_attr1280.u16_left = 0;
            st_osd_attr1280.u16_top = 0;
            st_osd_attr1280.u16_height = 1024;
            st_osd_attr1280.u16_width = 1280;
            /* 固定 2bit 颜色深度 */
            st_osd_attr1280.u08_pix_bw = TS_PIXEL_BW_2BIT;
            /* 是否可显示 */
            st_osd_attr1280.u08_visible = 1;
            /* 出现覆盖后的覆盖优先级, 0 最低 */
            st_osd_attr1280.u08_priority = 50;
            /* OSD 或者 十字光标 */
            st_osd_attr1280.u08_type = TS_OSD_TYPE_OSD;
            /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
            st_osd_attr1280.st_osd_cfg.s08_alpha = -1;
            /* 是否开启反色 */
            st_osd_attr1280.st_osd_cfg.u08_auto_invert = 0;
            /* 前景色与背景色配置 */
            st_osd_attr1280.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
            st_osd_attr1280.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;

            global_sys_ctrl_info.m_special_info_osd_node = osd_node_init(shdl_osd_dev, &st_osd_attr1280);
            
        }
    } 
    else
    {   
        {
            ts_osd_attr_s_t bottom_st_osd_attr;
            /* OSD 坐标配置 */
            bottom_st_osd_attr.u16_left = 0;
            bottom_st_osd_attr.u16_top = 348;    // osd 是矩形框是0,400点开始，矩形框是640x50
            bottom_st_osd_attr.u16_height = 104; // 三行信息显示，一行显示初始化提示信息y=0（提示信息或者设备信息），第二行显示版本信息y=25，第三行显示菜单二的参数y=50,第四行y=76（设备信息）
            bottom_st_osd_attr.u16_width = 640;
            if(osd_type==VOT_MENU_OSD_RES_384){
                bottom_st_osd_attr.u16_width = 384;
                bottom_st_osd_attr.u16_height = 128; //16x8  
            }

            /* 固定 2bit 颜色深度 */
            bottom_st_osd_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
            /* 是否可显示 */
            bottom_st_osd_attr.u08_visible = 1;
            /* 出现覆盖后的覆盖优先级, 0 最低 */
            bottom_st_osd_attr.u08_priority = 50;
            /* OSD 或者 十字光标 */
            bottom_st_osd_attr.u08_type = TS_OSD_TYPE_OSD;
            /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
            bottom_st_osd_attr.st_osd_cfg.s08_alpha = -1;
            /* 是否开启反色 */
            bottom_st_osd_attr.st_osd_cfg.u08_auto_invert = 0;
            /* 前景色与背景色配置 */
            bottom_st_osd_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
            bottom_st_osd_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
            // vision_log_debug("width = %d %d %d\n", bottom_st_osd_attr.u16_width, bottom_st_osd_attr.u16_height, bottom_st_osd_attr.st_osd_cfg.s08_alpha);

            global_sys_ctrl_info.m_bottom_info_osd_node = osd_node_init(shdl_osd_dev, &bottom_st_osd_attr);
        }
        
        //384 且不在超分时，只有384x288 菜单
        // if(res_type==DECTOR_RESOLUTION_384X288 && SystemParaMenu2.m_ts_npu_ops_sr_en!=3)
        if(osd_type==VOT_MENU_OSD_RES_384)
        {

            ts_osd_attr_s_t special_st_osd_attr;
            /* OSD 坐标配置 */
            special_st_osd_attr.u16_left = 0;
            special_st_osd_attr.u16_top = 0;
            special_st_osd_attr.u16_height = 288;
            special_st_osd_attr.u16_width = 384;
            /* 固定 2bit 颜色深度 */
            special_st_osd_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
            /* 是否可显示 */
            special_st_osd_attr.u08_visible = 1;
            /* 出现覆盖后的覆盖优先级, 0 最低 */
            special_st_osd_attr.u08_priority = 50;
            /* OSD 或者 十字光标 */
            special_st_osd_attr.u08_type = TS_OSD_TYPE_OSD;
            /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
            special_st_osd_attr.st_osd_cfg.s08_alpha = -1;
            /* 是否开启反色 */
            special_st_osd_attr.st_osd_cfg.u08_auto_invert = 0;
            /* 前景色与背景色配置 */
            special_st_osd_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
            special_st_osd_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
            // vision_log_debug("width = %d %d %d\n", special_st_osd_attr.u16_width, special_st_osd_attr.u16_height, special_st_osd_attr.st_osd_cfg.s08_alpha);

            global_sys_ctrl_info.m_special_info_osd_node = osd_node_init(shdl_osd_dev, &special_st_osd_attr);
        }
        else
        {
            ts_osd_attr_s_t special_st_osd_attr;
            /* OSD 坐标配置 */
            special_st_osd_attr.u16_left = 0;
            special_st_osd_attr.u16_top = 0;
            special_st_osd_attr.u16_height = 512;
            special_st_osd_attr.u16_width = 640;
            /* 固定 2bit 颜色深度 */
            special_st_osd_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
            /* 是否可显示 */
            special_st_osd_attr.u08_visible = 1;
            /* 出现覆盖后的覆盖优先级, 0 最低 */
            special_st_osd_attr.u08_priority = 50;
            /* OSD 或者 十字光标 */
            special_st_osd_attr.u08_type = TS_OSD_TYPE_OSD;
            /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
            special_st_osd_attr.st_osd_cfg.s08_alpha = -1;
            /* 是否开启反色 */
            special_st_osd_attr.st_osd_cfg.u08_auto_invert = 0;
            /* 前景色与背景色配置 */
            special_st_osd_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
            special_st_osd_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
            // vision_log_debug("width = %d %d %d\n", special_st_osd_attr.u16_width, special_st_osd_attr.u16_height, special_st_osd_attr.st_osd_cfg.s08_alpha);

            global_sys_ctrl_info.m_special_info_osd_node = osd_node_init(shdl_osd_dev, &special_st_osd_attr);
        }
    }
    
   
    vision_log_warn(" before vot_create_cursor_dot \n");
    global_sys_ctrl_info.m_cvbs_cursor_osd_dot = vot_create_cursor_dot(7,7 , OSD_COLOR_WHITE,  OSD_COLOR_WHITE, 1,70 );

    vision_log_warn(" leave \n");
    
}
/** 清除测温功能相关光标，OSD，捕获光标 */
ts_void Clear_Templut_Information(void)
{
    vision_log_debug("Auto_Semi=%d Spot_state = 0x%x, Area_state = 0x%x, Capture = 0x%x\n", SystemParaMenu2.Auto_Semi, Spot_state, Area_state, UsermenuParameter.CaptureFlag);
	Spot_state = 0;
	UsermenuParameter.CaptureFlag = 0;
	if (SystemParaMenu2.Auto_Semi) {
		Area_state &= 0xfff0;	/* 仅区域状态清零 */
	}
	else {
		Area_state = 0;
	}

	vot_osd_node_clear_info(global_sys_ctrl_info.m_templut_info_osd_node);
	vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st);
	vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st);
	vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st);
	vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_maxCursor_dot);
	vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_minCursor_dot);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, 0);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, 0);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, 0);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 0);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 0);
	templut_captions_refresh_display(0);
}
static ts_void init_templut_osd_handler(void) {
    ts_osd_attr_s_t st_osd_attr;

    global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st = NULL;
    global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st = NULL;
    global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st = NULL;
    global_sys_ctrl_info.m_lutCapture_maxCursor_dot = NULL;
    global_sys_ctrl_info.m_lutCapture_minCursor_dot = NULL;
    global_sys_ctrl_info.m_Crosshair_info_osd_dot = NULL;

    global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st = votCreate_cursor_SpecifiedColor(OSD_COLOR_WHITE);
    global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st = votCreate_cursor_SpecifiedColor(OSD_COLOR_WHITE);
    global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st = votCreate_cursor_SpecifiedColor(OSD_COLOR_WHITE);
    global_sys_ctrl_info.m_lutCapture_maxCursor_dot = votCreate_cursor_SpecifiedColor(OSD_COLOR_RED);
    global_sys_ctrl_info.m_lutCapture_minCursor_dot = votCreate_cursor_SpecifiedColor(OSD_COLOR_BLUE);
    global_sys_ctrl_info.m_Crosshair_info_osd_dot = votCreate_cursor_SpecifiedColor(OSD_COLOR_YELLOW);

	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

    /* 测温OSD节点 */
   	{
	   /* OSD 坐标配置 */
	   st_osd_attr.u16_left = 0;
	   st_osd_attr.u16_top =  0;
	   st_osd_attr.u16_height = max_heigth;
	   st_osd_attr.u16_width = max_width;
	   /* 固定 2bit 颜色深度 */
	   st_osd_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
	   /* 是否可显示 */
	   st_osd_attr.u08_visible = 1;
	   /* 出现覆盖后的覆盖优先级, 0 最低 */
	   st_osd_attr.u08_priority = 80;
	   /* OSD 或者 十字光标 */
	   st_osd_attr.u08_type = TS_OSD_TYPE_OSD;
	   /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
	   st_osd_attr.st_osd_cfg.s08_alpha = -1;
	   /* 是否开启反色 */
	   st_osd_attr.st_osd_cfg.u08_auto_invert = 0;
	   /* 前景色与背景色配置 */
	   st_osd_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
	   st_osd_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
	   global_sys_ctrl_info.m_templut_info_osd_node = osd_node_init(shdl_osd_dev, &st_osd_attr);
    }
    vot_osd_node_clear_info(global_sys_ctrl_info.m_templut_info_osd_node);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_maxCursor_dot);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_minCursor_dot);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_Crosshair_info_osd_dot);

    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_templut_info_osd_node, 0);
	ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, 0);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, 0);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, 0);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 0);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 0);
    ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_Crosshair_info_osd_dot, 0);

	ts_tdk_osd_refresh_display( TS_OSD_REFRESH_VISIBLE_ON);
}
/* 生成一个固定大小，且有外框的字符 */
ts_nodebuf_s_t *display_osd_init(ts_mdev_t h_osd_dev, ts_u32 u32_width, ts_u32 u32_height)
{
    ts_osd_attr_s_t st_attr;
    // ts_u32 u32_buf_addr, u32_tmp;
    ts_nodebuf_s_t *pst_lbl;

    /* OSD 坐标配置 */
    st_attr.u16_left = 0;
    st_attr.u16_top = 0;
    st_attr.u16_height = u32_height;
    st_attr.u16_width = u32_width;

    dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();
    if(res_type==DECTOR_RESOLUTION_384X288){
        st_attr.u16_height = 288;
        st_attr.u16_width = 384;

    }

    /* 固定 2bit 颜色深度 */
    st_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;

    /* 是否可显示 */
    st_attr.u08_visible = 1;

    /* 出现覆盖后的覆盖优先级, 0 最低 */
    st_attr.u08_priority = 80;

    /* OSD 或者 十字光标 */
    st_attr.u08_type = TS_OSD_TYPE_OSD;

    /* 整个 OSD 的透明度配置， -1 禁止 OSD 透明度配置，所有透明度配置依据调色板的配置 */
    st_attr.st_osd_cfg.s08_alpha = -1;

    /* 是否开启反色 */
    st_attr.st_osd_cfg.u08_auto_invert = 0;

    /* 前景色与背景色配置 */
    st_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
    st_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;

    vision_log_debug("width = %d %d %d\n", st_attr.u16_width, st_attr.u16_height, st_attr.st_osd_cfg.s08_alpha);

    pst_lbl = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(h_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_lbl)
        return ts_null;

    // {
    //     ts_u08 *pu08_ptr;
    //     // ts_u08 u08_val;
    //     ts_u32 i, j, u32_width;
    //     /* 填充成 2bit 色条 */
    //     // u08_val = 0;
    //     // k = 0;

    //     // 每个像素2bit 为 b 01,所以为1，sast_mnu_rgbs对应 /* 黑色， 透明度 0, 即完全不透明 */
    //     /* 每个像素占用 2bit */
    //     u32_width = st_attr.u16_width >> 2;
    //     for (i = 0; i < st_attr.u16_height; i++)
    //     {
    //         pu08_ptr = (ts_u08 *)pst_lbl->u32_virt_addr;
    //         pu08_ptr += i * u32_width;

    //         for (j = 0; j < u32_width; j++)
    //         {
    //             pu08_ptr[j] = 0x55;
    //         }
    //     }
    // }

    return pst_lbl;
}


ts_u16 uart_data_len = 0;
static ts_u08 uart_data_flag = 0;
ts_u16 vot_uart_protocol(ts_u08 *pu08_ptr)
{
    if(vot_get_force_debug_flag()==0){
        return 0;
    }

    // ts_u16 u16_len=0;
    ts_u16 u16_total_len = 0;
    ts_u16 i;
    ts_u08 check = 0;

    // vision_log_debug("======== vot_uart_protocol \n");
    if (pu08_ptr[0] != 0xA5 || pu08_ptr[1] != 0x52)
        return 0;

    u16_total_len = pu08_ptr[2] | (pu08_ptr[3] << 8);

    if (u16_total_len > 2048)
    {
        return 0;
    }

    for (i = 0; i < u16_total_len + 7; i++)
    {
        check += pu08_ptr[i];
    }
    check = ~check;

    if (check != pu08_ptr[u16_total_len + 7])
    {
        return 0;
    }

    // vision_log_debug("u16_total_len = %d\n", u16_total_len+8);
    // for(i=0; i<u16_total_len+8; i++)
    // {
    //         vision_log_debug("%x ", pu08_ptr[i]);
    // }
    // vision_log_debug("\n");

    // 检测是vot的消息才处理
    if (pu08_ptr[4] == 'V' && pu08_ptr[5] == 'O' && pu08_ptr[6] == 'T')
    {
        // uart_data_len = u16_total_len;
        // vision_log_debug("VOT len=%d\n", uart_data_len);
        // memcpy(uart_data, &pu08_ptr[4], uart_data_len);

        ring_buffer_update_write(&vot_uart0_ctx.m_ring_buf_instance, (ts_u08 *)&(pu08_ptr[4]), (ts_u32)u16_total_len);
        return u16_total_len;

        // Asic_Update_Comm_Data(&pu08_ptr[4], u16_total_len);
        // uart_data_flag = 1;

        // ts_cpl_up(&vot_uart0_ctx.st_sem);
    }
    else
    {
        return 0;
    }

    return u16_total_len + 8;
}

ts_u16 vot_uart_handle(ts_u08 *pu08_ptr)
{
    if (uart_data_flag == 1)
    {
        vision_log_debug("vot_uart_handle uart_data_len = %d, uart_data_flag = %d\n", uart_data_len, uart_data_flag);

        if (pu08_ptr[7] == 0xFF)
        {
            if (pu08_ptr[8] == 0xFE)
            {
                KEY_FE_PRO();
            }
            else if (pu08_ptr[8] == 0xFD)
            {
                KEY_FD_PRO();
            }
            else if (pu08_ptr[8] == 0xFB)
            {
                KEY_FB_PRO();
            }
            else if (pu08_ptr[8] == 0xF7)
            {
                KEY_F7_PRO();
            }
            else if (pu08_ptr[8] == 0xEF)
            {
                KEY_EF_PRO();
            }
            else if (pu08_ptr[8] == 0x7F)
            {
                Key_ExitUsermenu();
            }
            else if (pu08_ptr[8] == 0xD9)
            {
                enter_menu2();
            }
            else if (pu08_ptr[8] == 0xDF)
            {
                ISP_cut();
            }
            else if (pu08_ptr[8] == 0xFA)
            {
                // 图像冻结
                SetImgAutoFreeze();
            }
            else if (pu08_ptr[8] == 0xF4)
            {
                // 设备信息
                DisplayDeviceInfoAuto();
            }
            else if (pu08_ptr[8] == 0x78)
            {
                // 版本信息
                DisPlayDeviceInfoAndSoftVersionInfoAuto(1);
            }
            else if (pu08_ptr[8] == 0xE1)
            {
                // 进入坏点标记模式
                EnterOrExitBPCMaualMode(0);
            }
            else
            {
            }
        }
        else if (pu08_ptr[7] == 0x65)
        {

            if (pu08_ptr[8] == 0xF1) // 一键DET
            {
                // vision_log_debug("AutomaticDetParameterConfiguration \n ");
                AutomaticDetParameterConfiguration();
            }
            else if (pu08_ptr[8] == 0xA1) // 初始化菜单一
            {
                init_menu1_parameters();
            }
            else if (pu08_ptr[8] == 0xB2) // 初始化菜单二
            {
                init_menu2_parameters();
            }
            else if (pu08_ptr[8] == 0xA2) // 保存菜单一
            {
                save_menu1_parameters();
            }
            else if (pu08_ptr[8] == 0xC2) // 保存菜单二
            {
                save_menu2_parameters();
            }
        }
        else if (pu08_ptr[7] == 0x64)
        {
            // 一键OCC
            if (pu08_ptr[8] == 0x06)
            {
                vision_log_debug("AutomaticOccParameterConfiguration \n ");
                AutomaticOccParameterConfiguration();
            }
        }
        else if (pu08_ptr[7] == 0x0E)
        {
            if (pu08_ptr[6] == 0xAC)
            {
                // 设置设备信息
                GetDeviceInfo(pu08_ptr, uart_data_len);
            }
        }

        uart_data_len = 0;
        uart_data_flag = 0;
    }

    return 0;
}


ts_u32 vot_uart1_write_str(ts_char *buff)
{

    if (vot_uart1_ctx.s32_dev > 0 && strlen(buff) > 0)
    {
        return write(vot_uart1_ctx.s32_dev, buff, strlen(buff));
    }

    return 0;
}

ts_u16 vot_read_det_ad(void)
{

    ts_u16 temp_ad_value_u16 = 0;
    ts_f32 temp_value_f32 = 0;

    ISP_get_det_ad_info(&temp_value_f32, &temp_ad_value_u16);

    global_sys_ctrl_info.m_det_temp_ad_value = temp_ad_value_u16;
    global_sys_ctrl_info.m_det_temp_value = temp_value_f32;

    return temp_ad_value_u16;
}

ts_void check_menu1_parameters(void)
{
    dector_resolution_type_t res_type = get_dector_resolution_type();
    // vision_log_warn("res_type %d \n",res_type);
    if (res_type == DECTOR_RESOLUTION_384X288)
    {
        if (!(UsermenuParameter.m_dev_env_mode >= DEVICE_MODE_384_LOW_ATD && UsermenuParameter.m_dev_env_mode < DEVICE_MODE_384_END))
        {
            UsermenuParameter.m_dev_env_mode = DEVICE_MODE_384_LOW_ATD;
        }
    }
    else
    {
        if (UsermenuParameter.m_dev_env_mode >= (DEVICE_MODE_OUTDOOR_BPC))
        {
            UsermenuParameter.m_dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
        }
    }

    //非DSW定制,不适用CITY配置
    if (get_customer_type() != CUSTOMER_TYPE_DSW){
        if(UsermenuParameter.m_dev_env_mode >= DEVICE_MODE_OUTDOOR_CITY_HIGH_LIGHT && UsermenuParameter.m_dev_env_mode <= DEVICE_MODE_OUTDOOR_CITY_RED_HOT ){
             UsermenuParameter.m_dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
        }
        
    }

    if (UsermenuParameter.m_pip_mode >= PIP_MODE_BUTT)
    {
        UsermenuParameter.m_pip_mode = PIP_MODE_NONE;
    }

    if (UsermenuParameter.m_hst_mode >= HOT_SPOT_TRACKING_MODE_BUTT)
    {
        UsermenuParameter.m_hst_mode = HOT_SPOT_TRACKING_MODE_NONE;
    }
  
    //定制客户DSW深圳鼎深微，亮度对比度按照百分比方式
    if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
        if(UsermenuParameter.m_luma_percent_mode>=LUMA_PERCENT_MODE_BUTT){
            UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
        }
        
        if(UsermenuParameter.m_contrast_percent_mode>=CONTRAST_PERCENT_MODE_BUTT){
            UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
        }

        //亮度对比的自动模式下，是关联绑定的
        // if(UsermenuParameter.m_contrast_percent_mode==CONTRAST_PERCENT_MODE_AUTO || UsermenuParameter.m_luma_percent_mode == LUMA_PERCENT_MODE_AUTO){
        //     UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
        //     UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
        // }

        if(UsermenuParameter.m_rz_ratio>=RZ_RATIO_MODE_BUTT){
            UsermenuParameter.m_rz_ratio = RZ_RATIO_MODE_1X;
        }

        //sdk dv220, pip功能和电子变倍只能二选一
        if (UsermenuParameter.m_pip_mode != PIP_MODE_NONE && UsermenuParameter.Enlarge != 10)
        {
            UsermenuParameter.Enlarge = 10; //1.0变倍
        }
        
	}

    if (UsermenuParameter.m_SunPT_mode >= SUN_PT_MODE_BUTT)
    {
        UsermenuParameter.m_SunPT_mode = SUN_PT_MODE_AUTO;
    }

    if (UsermenuParameter.m_SunPT_gap <= 0 || UsermenuParameter.m_SunPT_gap > MAX_SUN_PT_TIME_SECS)
    {
        ISP_set_sun_pt_gap(DEFAULT_SUN_PT_TIME_SECS);
    }
    
    if( UsermenuParameter.m_menu1_info_tip_visible >= VOT_SWITCH_BUTT){ 
        UsermenuParameter.m_menu1_info_tip_visible = VOT_SWITCH_ON_LIKE0;
    }
    
}

ts_void check_menu2_parameters(void)
{

    if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC)
    {
        SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_UVC;
    }
    else
    {
        SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;
    }

    dector_resolution_type_t res_type = get_dector_resolution_type();
    // vision_log_warn("res_type %d \n",res_type);
    if (res_type == DECTOR_RESOLUTION_640X512)
    {
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    if (res_type == DECTOR_RESOLUTION_640X512)
    {
        SystemParaMenu2.m_bt_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    if (res_type == DECTOR_RESOLUTION_640X512)
    {
        SystemParaMenu2.m_mipi_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }


    if (SystemParaMenu2.m_video_out_mode >= VIDEO_OUTPUT_MODE_ENUM_BUTT)
    {
        SystemParaMenu2.m_video_out_mode = VIDEO_OUTPUT_MODE_ENUM_CVBS;
    }

    if (SystemParaMenu2.m_mipi_lane_mode >= MIPI_LANE_MODE_BUTT)
    {
        SystemParaMenu2.m_mipi_lane_mode = MIPI_LANE_MODE_2_LANES; // 默认为2Lanes
    }

    if (is_dector_type_manual() != 1)
    {
        SystemParaMenu2.m_force_detector_type = FORCE_DETECTOR_TYPE_NMOD;
    }
    else
    {
        if (SystemParaMenu2.m_force_detector_type >= FORCE_DETECTOR_TYPE_NMOD)
        {
            SystemParaMenu2.m_force_detector_type = FORCE_DETECTOR_TYPE_VOT_640X512; // 默认为光智640x512探测器
        }
    }


    if (is_hardware_port_type_manual() != 1)
    {
        SystemParaMenu2.m_force_hardware_port_type = FORCE_HARDWARE_PORT_TYPE_NMOD;
    }
    else
    {
        if (SystemParaMenu2.m_force_hardware_port_type >= FORCE_HARDWARE_PORT_TYPE_NMOD)
        {
            SystemParaMenu2.m_force_hardware_port_type = FORCE_HARDWARE_PORT_TYPE_UAD3_LITE; // 默认为UAD3_Lite接口板
        }
    }


    if ( is_customer_type_manual() != 1)
    {
        SystemParaMenu2.m_force_customer_id = CUSTOMER_TYPE_NMOD;
    }
    else
    {
        if (SystemParaMenu2.m_force_customer_id >= CUSTOMER_TYPE_NMOD)
        {
            SystemParaMenu2.m_force_customer_id = CUSTOMER_TYPE_STANDARD; 
        }
    }

    if ( is_focus_type_manual() != 1)
    {
        SystemParaMenu2.m_force_focus_type = FOCUS_TYPE_NMOD;
    }
    else
    {
        if (SystemParaMenu2.m_force_focus_type >= FOCUS_TYPE_NMOD)
        {
            SystemParaMenu2.m_force_focus_type = FOCUS_TYPE_PRIME_LENS; 
        }
    }

    
    if ( is_function_type_manual() != 1)
    {
        SystemParaMenu2.m_force_function_type = FUNCTION_TYPE_FIXED;
    }
    else
    {
        if (SystemParaMenu2.m_force_function_type >= FUNCTION_TYPE_FIXED)
        {
            SystemParaMenu2.m_force_function_type = FUNCTION_TYPE_INFRARED_IMAGING;  //默认红外观测
        }
    }

    //T8模式，uvc输出大小要和vpo一致
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
        SystemParaMenu2.m_bt_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
        SystemParaMenu2.m_mipi_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    //检查标记为是否在正常范围内
    if( SystemParaMenu2.m_user_metadata_flag >= USER_METADATA_BUTT){ 
        SystemParaMenu2.m_user_metadata_flag = USER_METADATA_DISABLE;
    }

    if( SystemParaMenu2.m_fix_snc_switch >= VOT_SWITCH_BUTT){ 
        SystemParaMenu2.m_fix_snc_switch = VOT_SWITCH_OFF;
    }

    if( SystemParaMenu2.m_iso_switch >= VOT_SWITCH_BUTT){ 
        SystemParaMenu2.m_iso_switch = VOT_SWITCH_OFF;
    }

    if( SystemParaMenu2.m_filter_ad_mode >= VOT_SWITCH_BUTT){ 
        SystemParaMenu2.m_filter_ad_mode = VOT_SWITCH_OFF;
    }

    if( SystemParaMenu2.m_menu2_info_tip_visible >= VOT_SWITCH_BUTT){ 
        SystemParaMenu2.m_menu2_info_tip_visible = VOT_SWITCH_ON_LIKE0;
    }

    if (SystemParaMenu2.m_ts_npu_ops_sr_en >= NPU_SR_BUTT) {
        SystemParaMenu2.m_ts_npu_ops_sr_en = NPU_SR_OFF;
    }

    //开机后，默认不能是温补标定模式
    if( SystemParaMenu2.m_temp_drift_comp_mode >= TEMP_DRIFT_COMPENSATION_MODE_CALIBRATION){ 
        SystemParaMenu2.m_temp_drift_comp_mode = TEMP_DRIFT_COMPENSATION_MODE_OFF;
    }

    if( SystemParaMenu2.m_temp_drift_aisp_mode >= TEMP_DRIFT_COMPENSATION_AI_APP_BUTT){ 
        SystemParaMenu2.m_temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
    }

    if( SystemParaMenu2.m_temp_drift_aisp_mode > TEMP_DRIFT_COMPENSATION_AI_APP_OFF){ 
        //非A6和A12，不支持AI后处理，且该后处理和超分功能不能同时开启，只能二选一，此处默认超分功能优先
        if(vot_get_temp_drift_aisp_enable() == 0 || SystemParaMenu2.m_ts_npu_ops_sr_en > NPU_SR_OFF){
            SystemParaMenu2.m_temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;
        }
    }

    if( SystemParaMenu2.m_t8_mode_force_bpc_enable >= VOT_SWITCH_BUTT){ 
        SystemParaMenu2.m_t8_mode_force_bpc_enable = VOT_SWITCH_OFF;
    }
    
    if( SystemParaMenu2.m_vot_pod_type >= VOT_POD_TYPE_BUTT){ 
        SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_WHITE_GRAY;
    }

    if( SystemParaMenu2.m_update_firmware_policy_mode >=  VOT_UPDATE_FIRMWARE_POLICY_BUTT){ 
        SystemParaMenu2.m_update_firmware_policy_mode = VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B;
    }

    if( SystemParaMenu2.m_temp_regularity >= TEMP_REGULARITY_BUTT){ 
        SystemParaMenu2.m_temp_regularity = TEMP_REGULARITY_GENERAL;
    }
    
}
/** @note 校验m_templut_target_gear合法性并应用待生效挡位 */
static void menu2_templut_target_gear_boot_check(void)
{
    ts_u08 val = SystemParaMenu2.m_templut_target_gear;

    if (SystemParaMenu2.m_templutMode_Limit < 1 || SystemParaMenu2.m_templutMode_Limit > 3) {
        SystemParaMenu2.m_templutMode_Limit = 1;
    }

    /* 只接受三个合法值：0xFF（无待生效）、0（OFF）、1（一档） */
    if (val != 0xFF && val != 0 && val != 1) {
        vision_log_debug("m_templut_target_gear invalid (0x%02X), reset to 0xFF\n", val);
        SystemParaMenu2.m_templut_target_gear = 0xFF;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
        return;
    }
    // 无待生效
    if (val == 0xFF) {
        if (SystemParaMenu2.Usermenu_SW > SystemParaMenu2.m_templutMode_Limit) {
            vision_log_debug("Boot fix: Usermenu_SW=%d > Limit=%d, clamping\n", SystemParaMenu2.Usermenu_SW, SystemParaMenu2.m_templutMode_Limit);
            SystemParaMenu2.Usermenu_SW = SystemParaMenu2.m_templutMode_Limit;
            ISP_write_sys_menu2_cfg(SystemParaMenu2);
        }
        return;
    }
    // 防御性判断
    if (val == SystemParaMenu2.Usermenu_SW) {
        SystemParaMenu2.m_templut_target_gear = 0xFF;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
        return;
    }
    vision_log_debug("Boot: applying target gear %d -> run gear %d\n", val, SystemParaMenu2.Usermenu_SW);
    SystemParaMenu2.Usermenu_SW = val;
    SystemParaMenu2.m_templut_target_gear = 0xFF;

    ISP_write_sys_menu2_cfg(SystemParaMenu2);
    if (SystemParaMenu2.Usermenu_SW > SystemParaMenu2.m_templutMode_Limit) {
        SystemParaMenu2.Usermenu_SW = SystemParaMenu2.m_templutMode_Limit;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
    }
}

ts_void set_default_sence_isp_cfg(void)
{
    dector_resolution_type_t res_type = get_dector_resolution_type();
    vision_log_warn("res_type %d \n", res_type);
    if (res_type == DECTOR_RESOLUTION_384X288)
    {
        UsermenuParameter.m_dev_env_mode = DEVICE_MODE_384_LOW_ATD;
    }
    else
    {
        UsermenuParameter.m_dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
    }
}

/**
 * @description: 菜单二默认参数配置
 * @return {*}
 */
ts_void menu1_default_parameters_init(void)
{
    vision_log_debug("menu1_default_parameters_init !\n");
    UsermenuParameter.Bright = 32;
    UsermenuParameter.Contrast = 8;
    UsermenuParameter.manual_ImagePara.Bright = 32;
    UsermenuParameter.manual_ImagePara.Contrast = 8;

    UsermenuParameter.SFilter = 8;
    UsermenuParameter.enhance_level = 4;

    UsermenuParameter.Palette = 0;
    UsermenuParameter.Mirror = 0;
    UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
#ifdef SUPPORT_CONTINUOUS_ZOOM
    UsermenuParameter.Enlarge = 10; // 默认1倍
#else
    UsermenuParameter.Enlarge = 0; // 默认1倍
#endif

    UsermenuParameter.EE_Level = 0;

    // 0.5 minus
    UsermenuParameter.AutoShutter = 1;

    if (UsermenuParameter.AutoShutter > 0)
    {
        UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
    }
    else if (UsermenuParameter.AutoShutter == 0)
    {
        UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
    }

    if (UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF)
    {
        global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
    }
    else
    {
        global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
    }

    UsermenuParameter.IsEnableEnligh = 0;

    set_default_sence_isp_cfg();
    ISP_set_auto_cut_flag_auto();

    UsermenuParameter.m_pip_mode  = PIP_MODE_NONE;
    UsermenuParameter.m_hst_mode  = HOT_SPOT_TRACKING_MODE_NONE;
    UsermenuParameter.m_Crosshair_OnlyDisplay = OFF;
    getDisplay_Crosshair_Switch();
    //增补初始化，由于特殊定制需求，阳光保护部分变量在菜单一和菜单二同时存在
    UsermenuParameter.m_SunPT_gap = DEFAULT_SUN_PT_TIME_SECS;
    UsermenuParameter.m_SunPT_mode = SUN_PT_MODE_AUTO;
    UsermenuParameter.m_menu1_info_tip_visible = VOT_SWITCH_ON_LIKE0;
    
	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
        UsermenuParameter.m_luma_percent_mode = LUMA_PERCENT_MODE_AUTO;
        UsermenuParameter.m_contrast_percent_mode = CONTRAST_PERCENT_MODE_AUTO;
        UsermenuParameter.m_rz_ratio = RZ_RATIO_MODE_1X;
	}

    //注意：JSON导入的用户菜单参数配置，只有在菜单一初始化时才会生效。勿与全局变量UsermenuParameter的值混淆，使用过程中UsermenuParameter的值会随着菜单操作而改变，而JSON导入的用户菜单参数配置是固定不变的。
    struct _Usermenu_Parameter UsermenuDefParam;
    struct _Usermenu_Parameter Valid;
    if (ISP_read_UserMenu_def_para_cfg(&UsermenuDefParam) == 0) // 读取用户菜单默认参数配置成功
    {
        if (UsermenuDefParam.UseUserMenuDefParam == 1) // 如果标记使用导入JSON的用户菜单默认参数，则加载到全局变量UsermenuParameter用户菜单默认参数
        {
            if(read_JsonUserMenuDefPara_valid_flag(&Valid)!=0)// 读取用户菜单默认参数配置的有效性标记
            {
                vision_log_err("read_JsonUserMenuDefPara_valid_flag failed \n");// 读取用户菜单默认参数配置的有效性标记失败
                return;
            }
            UpdatePromptInfoWithTimeMs("use UsermenuDefParamJson config!", 1000,VOT_LOG_TYPE_MENU1);
            vision_log_warn("use UsermenuDefParam json config \n");

            if(Valid.Bright == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的亮度值
            {
                UsermenuParameter.Bright=UsermenuDefParam.Bright;
            }
            if(Valid.Contrast == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的对比度值
            {
                UsermenuParameter.Contrast=UsermenuDefParam.Contrast;
            }
            if(Valid.Palette == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的伪彩值
            {
                UsermenuParameter.Palette=UsermenuDefParam.Palette;
            }
            if(Valid.Enlarge == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的放大值
            {
                UsermenuParameter.Enlarge=UsermenuDefParam.Enlarge;
            }
            if(Valid.IsEnableEnligh == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的增强值
            {
                UsermenuParameter.IsEnableEnligh=UsermenuDefParam.IsEnableEnligh;
            }
            if(Valid.Mirror == 1)// 如果有效，则使用Json导入的用户菜单默认参数配置的镜像值
            {
                UsermenuParameter.Mirror=UsermenuDefParam.Mirror;
            }
            if(Valid.AutoShutter == 1)
            {
                UsermenuParameter.AutoShutter=UsermenuDefParam.AutoShutter;
            }
            if(Valid.enhance_level == 1)
            {
                UsermenuParameter.enhance_level=UsermenuDefParam.enhance_level;
            }
            if(Valid.SFilter == 1)
            {
                UsermenuParameter.SFilter=UsermenuDefParam.SFilter;
            }
            if(Valid.m_dev_env_mode == 1)
            {
                UsermenuParameter.m_dev_env_mode=UsermenuDefParam.m_dev_env_mode;
            }
            // UsermenuParameter.EE_Level=UsermenuDefParam.EE_Level;
            // UsermenuParameter.DDE_Level=UsermenuDefParam.DDE_Level;
            // UsermenuParameter.ShutterMode=UsermenuDefParam.ShutterMode;
            if(Valid.manual_ImagePara.Bright == 1)UsermenuParameter.manual_ImagePara.Bright = UsermenuDefParam.manual_ImagePara.Bright;
            if(Valid.manual_ImagePara.Contrast == 1)UsermenuParameter.manual_ImagePara.Contrast = UsermenuDefParam.manual_ImagePara.Contrast;
            if(Valid.m_luma_percent_mode == 1)UsermenuParameter.m_luma_percent_mode = UsermenuDefParam.m_luma_percent_mode;
            if(Valid.m_contrast_percent_mode == 1)UsermenuParameter.m_contrast_percent_mode = UsermenuDefParam.m_contrast_percent_mode;
            if(Valid.m_pip_mode == 1)UsermenuParameter.m_pip_mode = UsermenuDefParam.m_pip_mode;
            if(Valid.m_hst_mode == 1)UsermenuParameter.m_hst_mode = UsermenuDefParam.m_hst_mode;
            if(Valid.m_rz_ratio == 1)UsermenuParameter.m_rz_ratio = UsermenuDefParam.m_rz_ratio;

            ISP_set_sun_pt_gap(UsermenuDefParam.m_SunPT_gap);
            if(Valid.m_SunPT_mode == 1)UsermenuParameter.m_SunPT_mode = UsermenuDefParam.m_SunPT_mode;

            if(Valid.m_menu1_info_tip_visible == 1)UsermenuParameter.m_menu1_info_tip_visible = UsermenuDefParam.m_menu1_info_tip_visible;
            if (UsermenuParameter.ShutterMode == SHUTTER_MODE_OFF)
            {
                global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
            }
            else
            {
                global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
            }
            
            if (UsermenuParameter.m_dev_env_mode>=DEVICE_MODE_BUTT)
            {
                UsermenuParameter.m_dev_env_mode=DEVICE_MODE_INDOOR;
            }
        
            ISP_set_auto_cut_flag_auto();
             if (SystemParaMenu2.DetMode >= DET_TEMP_MODE_BUTT)
            {
                SystemParaMenu2.DetMode = DET_TEMP_MODE_AUTO;
            }
        }
    }
    
}

/**
 * @description: 菜单二默认参数配置
 * @return {*}
 */
ts_void menu2_default_parameters_init(void)
{
    vision_log_debug("menu2_default_parameters_init !\n");

    // SystemParaMenu2.Usermenu_SW = 0; // 测温状态不允许进入

    SystemParaMenu2.m_sffc_mode = SFFC_MODE_MANUAl; // sffc 模式 SFFC_MODE_ENUM
    SystemParaMenu2.m_sffc_level = 8000;            // sffc 手动的模式的强度

    SystemParaMenu2.m_osd_show_on_dev = OSD_POSITION_ON_CVBS;

    SystemParaMenu2.m_ee_enhance_mode = EE_ENHANCE_MODE_OFF;
    if(SystemParaMenu2.Usermenu_SW){
        SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;    
    }else{
        SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ENLARGE;
    }

    SystemParaMenu2.BAD_SW = 1;
    SystemParaMenu2.NUC_SW = 1;
    SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_ON;
    SystemParaMenu2.m_bpc_view = 0;

    unsave_local_isp_cfg();
    SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_CVBS;	

    SystemParaMenu2.m_st0_total_duration_s = ST_0_TOTAL_DURATION_S_DEFAULT;  // 0-60s
    SystemParaMenu2.m_st0_interval_s = ST_0_INTERVAL_S_DEFAULT; // 1-20s

    SystemParaMenu2.m_st1_m = 1;  // 0-10 min
    SystemParaMenu2.m_st1_s = 15; // 5-30s
    SystemParaMenu2.m_det_st = 2; // 0-50 ==> 0 - 5°

    SystemParaMenu2.SunPT_SW = 1;
    SystemParaMenu2.S_Val = DEFAULT_SUN_PT_THRESHOLD;    // 满足条件的像素个数
    SystemParaMenu2.S_Range = DEFAULT_SUN_PT_GRAY_LEVEL; // 对应的灰度值 1000 , ad = 16*1000=16000
    SystemParaMenu2.P_Time_U16 = DEFAULT_SUN_PT_TIME_SECS;


    // 退出当前阳光保护，打开挡片，清空提示
    ISP_set_sun_pt_force_flag(0);
    vot_sun_pt_shutter_open();
    vot_set_sun_pt_notice_info(0);

    if (SystemParaMenu2.DetMode >= DET_TEMP_MODE_BUTT)
    {
        SystemParaMenu2.DetMode = DET_TEMP_MODE_AUTO;
    }

    SystemParaMenu2.TemTest_SW = 3;//默认全部开启
    // default close
    SystemParaMenu2.m_bpc_cluster_on_flag = 0;

    SystemParaMenu2.Bpc_ext_SW = 0; // 坏斑总开关默认关闭

    // SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_NONE;
    // SystemParaMenu2.m_templutMode_Limit = 1;
    SystemParaMenu2.m_temperCalibra_Flag = 0;
    SystemParaMenu2.AD_T = 2;
    SystemParaMenu2.m_user_tempFrame_Mode = TEMP_NORMAL_DISP_LUT_AD;
    SystemParaMenu2.m_raw_node_pos = TS_UPL_POS_BEFORE_AGC;
    //测温版本默认UYVY格式
    if(SystemParaMenu2.Usermenu_SW) { 
        SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_UYVY;
    }
    else {
        SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_YUYV;
    }

    SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_ON;
    dector_resolution_type_t res_type = get_dector_resolution_type();
    if (res_type == DECTOR_RESOLUTION_384X288) //384模式默认一比一输出
    {
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }
    if (SystemParaMenu2.Usermenu_SW) {
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    SystemParaMenu2.m_bt_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    SystemParaMenu2.m_mipi_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    SystemParaMenu2.SunPT_Info_SW = 1; 

    SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_NONE; 

    SystemParaMenu2.m_user_metadata_flag = USER_METADATA_DISABLE;

    SystemParaMenu2.m_fix_snc_switch = VOT_SWITCH_OFF;

    SystemParaMenu2.m_iso_switch = VOT_SWITCH_OFF;
    SystemParaMenu2.m_iso_position_mode = VOT_ISO_POSITION_MODE_INSIDE;	
	SystemParaMenu2.m_iso_color_mode = VOT_ISO_COLOR_MODE_GREEN;	
	SystemParaMenu2.m_iso_ad_high = 65535;
	SystemParaMenu2.m_iso_ad_low = 0;

	//ad 温度过滤相关配置
	SystemParaMenu2.m_filter_ad_mode = VOT_MODE_AUTO;
	SystemParaMenu2.m_manual_ad_high = 8500;
	SystemParaMenu2.m_manual_ad_low = 7500;

   	Temp_Parameter.Second_CalibFlag = 0; // 二次标定开关默认关闭
    SystemParaMenu2.sensor = 1; //默认GC
    
    SystemParaMenu2.m_menu2_info_tip_visible = VOT_SWITCH_ON_LIKE0;

    SystemParaMenu2.m_temp_drift_comp_mode = TEMP_DRIFT_COMPENSATION_MODE_OFF;

    SystemParaMenu2.m_temp_drift_aisp_mode = TEMP_DRIFT_COMPENSATION_AI_APP_OFF;

    SystemParaMenu2.m_t8_mode_force_bpc_enable = VOT_SWITCH_OFF;

    SystemParaMenu2.m_vot_pod_type = VOT_POD_TYPE_WHITE_GRAY;

    SystemParaMenu2.m_update_firmware_policy_mode = VOT_UPDATE_FIRMWARE_POLICY_BOTH_A_B;


    //注意：JSON导入的系统菜单默认参数配置，只有在菜单二初始化时才会生效。勿与全局变量SystemParaMenu2的值混淆，使用过程中SystemParaMenu2的值会随着菜单操作而改变，而JSON导入的系统菜单默认参数配置是固定不变的。
    struct Menu2_Para SysMenuDefParam;
    struct Menu2_Para Valid;
    if (ISP_read_SysMenu_def_para_cfg(&SysMenuDefParam) == 0) // 读取系统菜单默认参数配置成功
    {
        if (SysMenuDefParam.UseSysMenuDefParam == 1) // 如果标记使用导入JSON的系统菜单默认参数，则加载到全局变量SystemParaMenu2系统菜单默认参数
        {
            if(read_JsonSystemMenuDefPara_valid_flag(&Valid)!=0)// 读取用户菜单默认参数配置的有效性标记
            {
                vision_log_err("read_JsonSystemMenuDefPara_valid_flag failed \n");// 读取系统菜单默认参数配置的有效性标记失败
                return;
            }
            vision_log_warn("use SysMenuDefParam json config \n");
            UpdatePromptInfoWithTimeMs("use SysMenuDefParamJson config!", 1000,VOT_LOG_TYPE_MENU2);

            #if 0
            SystemParaMenu2.Usermenu_SW = SysMenuDefParam.Usermenu_SW;
            SystemParaMenu2.m_sffc_mode = SysMenuDefParam.m_sffc_mode; // sffc 模式 SFFC_MODE_ENUM
            SystemParaMenu2.m_sffc_level = SysMenuDefParam.m_sffc_level;            // sffc 手动的模式的强度
            //SystemParaMenu2.m_osd_show_on_dev = SysMenuDefParam.m_osd_show_on_dev;
            SystemParaMenu2.m_ee_enhance_mode = SysMenuDefParam.m_ee_enhance_mode;
           
            SystemParaMenu2.BAD_SW =SysMenuDefParam.BAD_SW;
            SystemParaMenu2.NUC_SW = SysMenuDefParam.NUC_SW;
            SystemParaMenu2.m_bpc_cfg = SysMenuDefParam.m_bpc_cfg;
            SystemParaMenu2.m_bpc_view = SysMenuDefParam.m_bpc_view;
            SystemParaMenu2.m_local_isp_magic_value = SysMenuDefParam.m_local_isp_magic_value;
            SystemParaMenu2.m_bpc_ref_mode = SysMenuDefParam.m_bpc_ref_mode;
            //菜单二DET选项 ,注意：每个探测器的Det默认初始化参数不一样，故不需要给Det赋初始值，否则一键Det后，Det参数在此被更改。
            // SystemParaMenu2.DetMode=SysMenuDefParam.DetMode;
            // SystemParaMenu2.NormalTemp.INT_C =  SysMenuDefParam.NormalTemp.INT_C;
            // SystemParaMenu2.NormalTemp.INT_T =  SysMenuDefParam.NormalTemp.INT_T;
            // SystemParaMenu2.NormalTemp.OOC_Par =  SysMenuDefParam.NormalTemp.OOC_Par;

            // SystemParaMenu2.LowTemp.INT_C =  SysMenuDefParam.LowTemp.INT_C;
            // SystemParaMenu2.LowTemp.INT_T =  SysMenuDefParam.LowTemp.INT_T;
            // SystemParaMenu2.LowTemp.OOC_Par =  SysMenuDefParam.LowTemp.OOC_Par;

            // SystemParaMenu2.HighTemp.INT_C =  SysMenuDefParam.HighTemp.INT_C;
            // SystemParaMenu2.HighTemp.INT_T =  SysMenuDefParam.HighTemp.INT_T;
            // SystemParaMenu2.HighTemp.OOC_Par =  SysMenuDefParam.HighTemp.OOC_Par;

            // SystemParaMenu2.offChange.INT_C =  SysMenuDefParam.offChange.INT_C;
            // SystemParaMenu2.offChange.INT_T =  SysMenuDefParam.offChange.INT_T;
            // SystemParaMenu2.offChange.OOC_Par =  SysMenuDefParam.offChange.OOC_Par;
            //菜单二BPC选项
            SystemParaMenu2.m_bpc_cluster_on_flag = SysMenuDefParam.m_bpc_cluster_on_flag;
            SystemParaMenu2.Bpc_ext_SW = SysMenuDefParam.Bpc_ext_SW; // 坏斑总开关默认关闭
            #endif
            //菜单二System选项
            if(Valid.m_video_out_mode == 1)SystemParaMenu2.m_video_out_mode = SysMenuDefParam.m_video_out_mode;// 如果有效，则使用Json导入的系统菜单默认参数配置的视频输出模式
            //Misc选项
            if(Valid.m_cvbs_display_mode == 1)SystemParaMenu2.m_cvbs_display_mode = SysMenuDefParam.m_cvbs_display_mode;// 如果有效，则使用Json导入的系统菜单默认参数配置的CVBS显示模式
            if(Valid.m_uvc_enlarger_mode == 1)SystemParaMenu2.m_uvc_enlarger_mode = SysMenuDefParam.m_uvc_enlarger_mode;// 如果有效，则使用Json导入的系统菜单默认参数配置的UVC放大模式
            if(Valid.m_mipi_lane_mode == 1)SystemParaMenu2.m_mipi_lane_mode = SysMenuDefParam.m_mipi_lane_mode;// 如果有效，则使用Json导入的系统菜单默认参数配置的MIPI lane模式
            if(Valid.m_force_detector_type == 1)SystemParaMenu2.m_force_detector_type = SysMenuDefParam.m_force_detector_type;
            if(Valid.m_force_hardware_port_type == 1)SystemParaMenu2.m_force_hardware_port_type = SysMenuDefParam.m_force_hardware_port_type;
            if(Valid.m_force_customer_id == 1)SystemParaMenu2.m_force_customer_id = SysMenuDefParam.m_force_customer_id;
            if(Valid.m_force_focus_type == 1)SystemParaMenu2.m_force_focus_type = SysMenuDefParam.m_force_focus_type;
            if(Valid.m_force_function_type == 1)SystemParaMenu2.m_force_function_type = SysMenuDefParam.m_force_function_type;

            //Sun_Prot选项
            if(Valid.m_st1_m == 1)SystemParaMenu2.m_st1_m = SysMenuDefParam.m_st1_m;  // 0-10 min
            if(Valid.m_st1_s == 1)SystemParaMenu2.m_st1_s = SysMenuDefParam.m_st1_s; // 5-30s
            if(Valid.m_det_st == 1)SystemParaMenu2.m_det_st = SysMenuDefParam.m_det_st; // 0-50 ==> 0 - 5°
            if(Valid.SunPT_SW == 1)SystemParaMenu2.SunPT_SW = SysMenuDefParam.SunPT_SW;
            if(Valid.S_Val == 1)SystemParaMenu2.S_Val = SysMenuDefParam.S_Val;    // 满足条件的像素个数
            if(Valid.S_Range == 1)SystemParaMenu2.S_Range = SysMenuDefParam.S_Range; // 对应的灰度值 1000 , ad = 16*1000=16000
            if(Valid.P_Time_U16 == 1)SystemParaMenu2.P_Time_U16 = SysMenuDefParam.P_Time_U16;
            //SystemParaMenu2.SunPT_Info_SW = SysMenuDefParam.SunPT_Info_SW;

            if(Valid.m_st0_total_duration_s == 1)SystemParaMenu2.m_st0_total_duration_s = SysMenuDefParam.m_st0_total_duration_s; // 5-30s
            if(Valid.m_st0_interval_s == 1)SystemParaMenu2.m_st0_interval_s = SysMenuDefParam.m_st0_interval_s; // 0-50 ==> 0 - 5°

           //T8模式选项
            if(Valid.m_raw_yuv_mode == 1)SystemParaMenu2.m_raw_yuv_mode = SysMenuDefParam.m_raw_yuv_mode;
            if(Valid.m_raw_node_pos == 1)SystemParaMenu2.m_raw_node_pos = SysMenuDefParam.m_raw_node_pos;
            if(Valid.m_bt_time_sequece_mode == 1)SystemParaMenu2.m_bt_time_sequece_mode = SysMenuDefParam.m_bt_time_sequece_mode;
            if(Valid.m_user_metadata_flag == 1)SystemParaMenu2.m_user_metadata_flag =SysMenuDefParam.m_user_metadata_flag;

            if(Valid.m_fix_snc_switch == 1)SystemParaMenu2.m_fix_snc_switch= SysMenuDefParam.m_fix_snc_switch;
          
            //json导入导出时，记得同步更新配置参数的默认值，并且如果增加了新的参数，也要记得在这里添加默认值的加载，否则可能会出现新增加的参数无法加载默认值，导致异常的情况
            //其他扩展选项
            #if 0
            for(int i=0;i<51;i++){
                if(Valid.SmartData408bit[i] == 1)
                SystemParaMenu2.SmartData408bit[i] = SysMenuDefParam.SmartData408bit[i];
            }
            if(Valid.SmartData408bitValidFlag == 1)SystemParaMenu2.SmartData408bitValidFlag = SysMenuDefParam.SmartData408bitValidFlag;
            #endif
            if(Valid.set_current_ua == 1)SystemParaMenu2.set_current_ua=SysMenuDefParam.set_current_ua;

            
            if(Valid.m_bt_enlarger_mode == 1)SystemParaMenu2.m_bt_enlarger_mode = SysMenuDefParam.m_bt_enlarger_mode;
            if(Valid.m_mipi_enlarger_mode == 1)SystemParaMenu2.m_mipi_enlarger_mode = SysMenuDefParam.m_mipi_enlarger_mode;

            if(Valid.m_menu2_info_tip_visible == 1)SystemParaMenu2.m_menu2_info_tip_visible = SysMenuDefParam.m_menu2_info_tip_visible;
            if(Valid.m_temp_drift_comp_mode == 1)SystemParaMenu2.m_temp_drift_comp_mode = SysMenuDefParam.m_temp_drift_comp_mode;
            if(Valid.m_temp_drift_aisp_mode == 1)SystemParaMenu2.m_temp_drift_aisp_mode = SysMenuDefParam.m_temp_drift_aisp_mode;

            if(Valid.m_t8_mode_force_bpc_enable == 1)SystemParaMenu2.m_t8_mode_force_bpc_enable = SysMenuDefParam.m_t8_mode_force_bpc_enable;

            if(Valid.m_vot_pod_type == 1)SystemParaMenu2.m_vot_pod_type = SysMenuDefParam.m_vot_pod_type;
            if(Valid.sensor == 1)SystemParaMenu2.sensor = SysMenuDefParam.sensor;
            if(Valid.Usermenu_SW == 1)SystemParaMenu2.Usermenu_SW = SysMenuDefParam.Usermenu_SW;
            if(Valid.TemTest_SW == 1)SystemParaMenu2.TemTest_SW = SysMenuDefParam.TemTest_SW;
            if(Valid.m_templutMode_Limit == 1)SystemParaMenu2.m_templutMode_Limit = SysMenuDefParam.m_templutMode_Limit;
            if(Valid.m_uvc_output_format == 1)SystemParaMenu2.m_uvc_output_format = SysMenuDefParam.m_uvc_output_format;
            if(Valid.AD_T == 1)SystemParaMenu2.AD_T = SysMenuDefParam.AD_T;
            if(Valid.m_temperCalibra_Flag == 1)SystemParaMenu2.m_temperCalibra_Flag = SysMenuDefParam.m_temperCalibra_Flag;
            if(Valid.m_user_tempFrame_Mode == 1)SystemParaMenu2.m_user_tempFrame_Mode = SysMenuDefParam.m_user_tempFrame_Mode;
            
            if(Valid.m_update_firmware_policy_mode == 1)SystemParaMenu2.m_update_firmware_policy_mode = SysMenuDefParam.m_update_firmware_policy_mode;
            if(Valid.m_temp_regularity == 1)SystemParaMenu2.m_temp_regularity = SysMenuDefParam.m_temp_regularity;


        }
    }
}

ts_void menu2_parameters_init(void)
{
    struct Menu2_Para sys_menu2_cfg = {0};

    if (ISP_read_sys_menu2_cfg(&sys_menu2_cfg) == 0)
    {
        memcpy(&SystemParaMenu2, &sys_menu2_cfg, sizeof(struct Menu2_Para));
        vision_log_debug("GAIN = %d, GOF = %d, OOC_Par = %d, INT_C = %d, INT_T = %d\n", SystemParaMenu2.GAIN, SystemParaMenu2.GOF, SystemParaMenu2.OOC_Par, SystemParaMenu2.INT_C, SystemParaMenu2.INT_T);
    }
    else
    {
        // 如果用户没有保存配置，加载用户菜单一的默认排至
        menu2_default_parameters_init();
        // 保存配置
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
    }

    check_menu2_parameters();

    // SystemParaMenu2.Usermenu_SW = 0;

    if (SystemParaMenu2.m_sffc_level < 2000)
    {
        SystemParaMenu2.m_sffc_level = 8000;
        SystemParaMenu2.m_sffc_mode = SFFC_MODE_MANUAl;
    }

    shdl_osd_dev = TS_OSD_ALL_MDEV0;


    if (SystemParaMenu2.m_cvbs_display_mode >= DEVICE_CVBS_DISPLAY_MODE_BUTT)
    {
        SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
    }

    // 正常模式都是开启的状态
    if (SystemParaMenu2.NUC_SW == 0)
    {
        SystemParaMenu2.NUC_SW = 1;
    }

    if (SystemParaMenu2.BAD_SW == 0)
    {
        SystemParaMenu2.BAD_SW = 1;
    }

    SystemParaMenu2.m_bpc_cfg = BPC_CFG_MODE_ON;
    SystemParaMenu2.m_bpc_view = 0;

    SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_CVBS;
    SystemParaMenu2.Auto_Semi = 0; // 半自动校正固定为关闭

    if (SystemParaMenu2.SunPT_SW > 1)
    {
        SystemParaMenu2.SunPT_SW = 1;
    }
    // 1-1025 gray level
    if (SystemParaMenu2.S_Range <= 0 || SystemParaMenu2.S_Range > 1025)
    {
        SystemParaMenu2.S_Range = DEFAULT_SUN_PT_GRAY_LEVEL;
    }

    // 1-MAX_SUN_PT_THRESHOLD
    if (SystemParaMenu2.S_Val <= 0 || SystemParaMenu2.S_Val > MAX_SUN_PT_THRESHOLD)
    {
        SystemParaMenu2.S_Val = DEFAULT_SUN_PT_THRESHOLD;
    }

    // 1-MAX_SUN_PT_TIME_SECS  senconds
    if (SystemParaMenu2.P_Time_U16 <= 0 || SystemParaMenu2.P_Time_U16 > MAX_SUN_PT_TIME_SECS)
    {
        SystemParaMenu2.P_Time_U16 = DEFAULT_SUN_PT_TIME_SECS;
    }

    if (SystemParaMenu2.SunPT_Info_SW > 1)
    {
        SystemParaMenu2.SunPT_Info_SW = 1;
    }

    // 0-10 min
    if (SystemParaMenu2.m_st1_m > 10)
    {
        SystemParaMenu2.m_st1_m = 1;
    }

    // 5-30s
    if (SystemParaMenu2.m_st1_s < 5 || SystemParaMenu2.m_st1_s > 30)
    {
        SystemParaMenu2.m_st1_s = 15;
    }

    // 0-50 ==> 0 - 5°
    if (SystemParaMenu2.m_det_st > 50)
    {
        SystemParaMenu2.m_det_st = 2;
    }


    // 0-60sencond
    if (SystemParaMenu2.m_st0_total_duration_s > ST_0_TOTAL_DURATION_S_MAX)
    {
        SystemParaMenu2.m_st0_total_duration_s = ST_0_TOTAL_DURATION_S_DEFAULT;
    }

    //  1-20s
    if (SystemParaMenu2.m_st0_interval_s < 1 || SystemParaMenu2.m_st0_interval_s > SystemParaMenu2.m_st0_total_duration_s || SystemParaMenu2.m_st0_interval_s > ST_0_INTERVAL_S_MAX)
    {
        SystemParaMenu2.m_st0_interval_s = ST_0_INTERVAL_S_DEFAULT;
    }



#ifdef VOT_BT656_EN
    SystemParaMenu2.m_video_out_mode = VIDEO_OUTPUT_MODE_ENUM_BT601;
#endif

    if (SystemParaMenu2.m_raw_yuv_mode >= RAW_YUV_MODE_BUTT)
    {
        SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_NONE;
    }

    SystemParaMenu2.m_raw_node_pos = vot_check_lut_raw_pos(SystemParaMenu2.m_raw_node_pos);

    if (SystemParaMenu2.m_uvc_enlarger_mode >= OUTPUT_MODE_ENLARGE_BUTT)
    {
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    if (SystemParaMenu2.m_bt_enlarger_mode >= OUTPUT_MODE_ENLARGE_BUTT)
    {
        SystemParaMenu2.m_bt_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    if (SystemParaMenu2.m_mipi_enlarger_mode >= OUTPUT_MODE_ENLARGE_BUTT)
    {
        SystemParaMenu2.m_mipi_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }

    // #ifdef UVC_FORCE384
    //     SystemParaMenu2.m_uvc_enlarger_mode =  OUTPUT_MODE_ENLARGE_OFF;
    // #endif

    vision_log_warn("SystemParaMenu2.m_uvc_enlarger_mode %d \n",SystemParaMenu2.m_uvc_enlarger_mode);
    menu2_templut_target_gear_boot_check();
    // vot_uvc_output_restore_or_backup(0);
    if (SystemParaMenu2.Usermenu_SW) {
/* 物联部门--杨能筒机专用 */
#if defined(VOT_IOT_DEFINE)
        SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_25FPS;   // 25fps
        SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_RAW_YUV; // T8=raw+yuv
        SystemParaMenu2.m_templutMode_Limit = 3;
#endif
        /* 开机对参数行分情况执行 */
        if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW) {
            SystemParaMenu2.m_user_metadata_flag = USER_METADATA_ENABLE;
            UsermenuParameter.Mirror = 0;   /* RAW+YUV 模式下不允许进行翻转 */
        }
        else {
            SystemParaMenu2.m_user_metadata_flag = USER_METADATA_DISABLE;
        }
    }
    else {
        SystemParaMenu2.m_user_metadata_flag = USER_METADATA_DISABLE;
    }
    //只有在BT模式下，自定义bt时序才有效
    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){

        if(SystemParaMenu2.m_bt_time_sequece_mode >=BT_TIMING_SEQ_MODE_BUTT){
            SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_NONE;
        }

        if(SystemParaMenu2.m_bt_time_sequece_mode == BT_TIMING_SEQ_MODE_WIN2_25HZ ){
            SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_25FPS;
        }

        if(SystemParaMenu2.m_bt_time_sequece_mode == BT_TIMING_SEQ_MODE_WIN2_30HZ ){
            SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_25FPS;
        }

        if(SystemParaMenu2.m_bt_time_sequece_mode == BT_TIMING_SEQ_MODE_WIN2_50HZ ){
            SystemParaMenu2.m_device_fps = DEVICE_FPS_MODE_50FPS;
        }

        //自定义时序和raw+yuv只能二选一
        if(SystemParaMenu2.m_bt_time_sequece_mode !=BT_TIMING_SEQ_MODE_NONE ){
            
                SystemParaMenu2.m_raw_yuv_mode = RAW_YUV_MODE_NONE;
        }
    }
    else{
        SystemParaMenu2.m_bt_time_sequece_mode = BT_TIMING_SEQ_MODE_NONE;
    }

    if(SystemParaMenu2.set_current_ua>=200)SystemParaMenu2.set_current_ua=200;//gm1215线性稳压电源，设置目标电流0~200μA

    VotParameters.Test_Flag = 0;
    // 测温下CVBS/UVC强制原始尺寸显示
    if (SystemParaMenu2.Usermenu_SW) {
        SystemParaMenu2.m_cvbs_display_mode = DEVICE_CVBS_DISPLAY_MODE_ORG;
        SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
        SystemParaMenu2.m_bt_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;   // BT/MIPI均为预留
        SystemParaMenu2.m_mipi_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
    }
    // 测温工具边界限制
    if (SystemParaMenu2.m_temperCalibra_Flag > 1) {
        SystemParaMenu2.m_temperCalibra_Flag = 0;
    }
    if (SystemParaMenu2.AD_T > 3) {
        SystemParaMenu2.AD_T = 2;
    }
    // UVC输出数据格式边界限制
    if (SystemParaMenu2.m_uvc_output_format >= UVC_YUV_FORMAT_BUTT) {
        SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_YUYV;
    }

    set_powerup_temp_drift_comp_mode(SystemParaMenu2.m_temp_drift_comp_mode);
}

ts_void menu1_parameters_init(void)
{
    if (ISP_read_sys_cfg() != 0)
    {
        // flash中没有菜单一的配置，配置默认值
        menu1_default_parameters_init();
        ISP_write_sys_cfg();
    }

    if (UsermenuParameter.AutoShutter > 0)
    {
        UsermenuParameter.ShutterMode = SHUTTER_MODE_TIMER;
    }
    else if (UsermenuParameter.AutoShutter == 0)
    {
        UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
    }

    if (UsermenuParameter.IsEnableEnligh > 1)
    {
        UsermenuParameter.IsEnableEnligh = 0;
    }

    if (UsermenuParameter.m_dev_env_mode >= DEVICE_MODE_BUTT)
    {
        UsermenuParameter.m_dev_env_mode = DEVICE_MODE_OUTDOOR_K1;
    }

    // 检查放大倍数是否合法
#ifdef SUPPORT_CONTINUOUS_ZOOM
    if (UsermenuParameter.Enlarge < 10 || UsermenuParameter.Enlarge > 100)
    {
        UsermenuParameter.Enlarge = 10;
    }

#endif
    global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;

    // 检查定制客户Manual场景下亮度和对比度是否异常0值，如果异常则恢复默认值
    if(UsermenuParameter.manual_ImagePara.Bright==0 && UsermenuParameter.manual_ImagePara.Contrast==0)
    {
        UsermenuParameter.manual_ImagePara.Bright=32;
        UsermenuParameter.manual_ImagePara.Contrast=8;
    }

    check_menu1_parameters();
}


ts_void vot_misc_parameters_init(void)
{
    struct Vot_Misc_Para misc_cfg = {0};

    if (ISP_read_vot_misc_cfg(&misc_cfg) == 0)
    {
        memcpy(&global_vot_misc_cfg, &misc_cfg, sizeof(struct Vot_Misc_Para));
        vision_log_warn("m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
    }
    else
    {
        global_vot_misc_cfg.m_force_debug_confirm = 0;  //只是取值为VOT_DEBUG_OUTPUT_CONFIRM_MAGIC，才开启日志输出
        // 保存配置
        ISP_write_vot_misc_cfg(global_vot_misc_cfg);
        vision_log_warn("m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
    }
  
}
/**
 * @description: 配置特殊参数，如黑边移除
 * @return {*}
 */
ts_void special_para_init(ts_u08 Palette)
{
    // 去除边框彩条
#if 1
    ts_cpl_writel(0x05000019, 0x0035c000);
    ts_cpl_writel(0x00002000, 0x0035c004);
    ts_cpl_writel(0x0000035f, 0x0035c008);
    ts_cpl_writel(0x2a098acb, 0x0035c00c);
    ts_cpl_writel(0x00000003, 0x0035c01c);
    ts_cpl_writel(0x0d62a000, 0x0035c020);
    ts_cpl_writel(0x00000000, 0x0035c01c);
    ts_cpl_writel(0x17c1340e, 0x0035c028);
    ts_cpl_writel(0x00000001, 0x0035c01c);
    ts_cpl_writel(0x39f2642d, 0x0035c028);
    ts_cpl_writel(0x00000002, 0x0035c01c);
    ts_cpl_writel(0x07714058, 0x0035c028);
    ts_cpl_writel(0x00000003, 0x0035c01c);
    ts_cpl_writel(0x9c270c4e, 0x0035c028);
    ts_cpl_writel(0x00000004, 0x0035c01c);
    ts_cpl_writel(0x8e3f3244, 0x0035c028);


    /*
    mem w 0x0035c01c 0x00000000
    mem w 0x0035c028 0x17b1200e
    0x0035c01c （从寄存器偏移地址）
    0x0035c028[19:10]调整cvbs同步电平的结束时间
    */
    ts_cpl_writel(0x00000000, 0x0035c01c);
    if (Palette != 0)
    {
        //mem w 0x0035c028 0x17b1200f  
        ts_cpl_writel(0x17b1200f, 0x0035c028);
    }
    else
    {
        if (SystemParaMenu2.m_cvbs_display_mode == DEVICE_CVBS_DISPLAY_MODE_ENLARGE)
        {
            ts_cpl_writel(0x17c13409, 0x0035c028); // 全屏模式
        }
        else
        {
            ts_cpl_writel(0x17c1340c, 0x0035c028); // 一比一模式
        }
    }

    // vision_log_debug("SystemParaMenu2.m_cvbs_display_mode=%d\n", SystemParaMenu2.m_cvbs_display_mode);
#endif
}

void display_det_info(void)
{
    
    //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
	#ifdef CONFIG_SNR_PLANCK1280C
	{
 // vision_log_debug("SystemParaMenu2.GAIN1280=%d\n",SystemParaMenu2.GAIN1280);
    // vision_log_debug("GOF1280=%d\n", SystemParaMenu2.GOF1280);
    // vision_log_debug("Cint1280=%d\n",SystemParaMenu2.Cint1280);
    // vision_log_debug("Nint=%d\n", SystemParaMenu2.Nint);
    // vision_log_debug("Sel_PJ=%d\n", SystemParaMenu2.Sel_PJ);
    // vision_log_debug("NTCC=%d\n", SystemParaMenu2.NTCC);
    // vision_log_debug("Voccm=%d\n",SystemParaMenu2.Voccm);
    // vision_log_debug("Config=%d\n", SystemParaMenu2.detConfig);
    // vision_log_debug("Vfid=%d\n",SystemParaMenu2.Vfid);
    // planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
    // planck1280c_write_gof(SystemParaMenu2.GOF1280);
    // planck1280c_write_cint(SystemParaMenu2.Cint1280);
    // planck1280c_write_Nint(SystemParaMenu2.Nint);
    // planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);
    // planck1280c_write_NTCC(SystemParaMenu2.NTCC);
    // planck1280c_write_Voccm(SystemParaMenu2.Voccm);
    // planck1280c_write_detConfig(SystemParaMenu2.detConfig);
    // planck1280c_write_Vfid(SystemParaMenu2.Vfid);
    vision_log_debug("GAIN1280=%d\n", planck1280c_read_gain_adc());
    vision_log_debug("GOF1280=%d\n", planck1280c_read_gof());
    vision_log_debug("Cint1280=%d\n", planck1280c_read_Cint());
    vision_log_debug("Nint=%d\n", planck1280c_read_Nint());
    vision_log_debug("Sel_PJ=%d\n", planck1280c_read_Sel_PJ());
    vision_log_debug("NTCC=%d\n", planck1280c_read_NTCC());
    vision_log_debug("Voccm=%d\n", planck1280c_read_Voccm());
    vision_log_debug("Config=%d\n", planck1280c_read_detConfig());
    vision_log_debug("Vfid=%d\n", planck1280c_read_Vfid());
    }
    #else
    {
    // ts_u32 GAIN = planck640c_read_gain_adc();
    // ts_u32 GOF = planck640c_read_gof();
    // ts_u32 INT_C = planck640c_read_cint();
    // ts_u32 INT_T = planck640c_read_tint();
    // ts_u32 OOC_Par = planck640c_read_gos();
    ts_u32 NDAC_L = planck640c_read_ndac() & 0x1F;
    ts_u32 NDAC_H = (planck640c_read_ndac() >> 5) & 0xFF;

    vision_log_debug("GAIN=%d\n", planck640c_read_gain_adc());
    vision_log_debug("GOF=%d\n", planck640c_read_gof());
    vision_log_debug("NDAC_L=%d\n", planck640c_read_ndac() & 0x1F);
    vision_log_debug("NDAC_H=%d\n", (planck640c_read_ndac() >> 5) & 0xFF);
    vision_log_debug("GOS=%d \n", planck640c_read_gos());
    vision_log_debug("INT_C=%d\n", planck640c_read_cint());
    vision_log_debug("INT_T=%d\n", planck640c_read_tint());

    SystemParaMenu2.NDAC_H = NDAC_H;
    SystemParaMenu2.NDAC_L = NDAC_L;
    }
    #endif
}

void TestmenuParameterInit(void)
{
    menu2_parameters_init();
}

void UserParameterInit(void)
{
    menu1_parameters_init();
}

ts_void userpara_reset(void)
{
    static ts_u08 first_init_flag = 1;
    if (first_init_flag == 0)
    {
        load_palette(UsermenuParameter.Palette);
    }
    else
    {
        first_init_flag = 0;
    }

    // ISP_mirr_set(UsermenuParameter.Mirror);
    ISP_shuttermode(UsermenuParameter.ShutterMode);

    // ISP_ee_set(UsermenuParameter.EE_Level*16);
    ISP_nr_set(UsermenuParameter.SFilter * 8); // 8  256  8

    // ISP_dde_level(UsermenuParameter.DDE_Level*16); //16 256  4
    ISP_enhance_set(UsermenuParameter.enhance_level * 16);

    //     ISP_mirr_set(UsermenuParameter.Mirror);
    // #ifdef SUPPORT_CONTINUOUS_ZOOM
    //     ISP_continuous_zoom_set(UsermenuParameter.Enlarge);
    // #else
    //     ISP_zoom_set(1<<UsermenuParameter.Enlarge);
    // #endif


    vot_isp_luma_set();
    vot_isp_coef_set();

    ISP_vot_zoom_set(UsermenuParameter.Enlarge);

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
        vot_pip_mode_set(UsermenuParameter.m_pip_mode, RZ_RATIO_MODE_2X, 1);
        vot_hst_mode_set(UsermenuParameter.m_hst_mode);
        vision_log_warn("CUSTOMER_TYPE_DSW\n");
    }

    vision_log_debug("UsermenuParameter.Bright=%d\n", UsermenuParameter.Bright);
    vision_log_debug("UsermenuParameter.Contrast=%d\n", UsermenuParameter.Contrast);
    vision_log_debug("UsermenuParameter.manual_ImagePara.Bright=%d\n", UsermenuParameter.manual_ImagePara.Bright);
    vision_log_debug("UsermenuParameter.manual_ImagePara.Contrast=%d\n", UsermenuParameter.manual_ImagePara.Contrast);
    vision_log_debug("userpara_reset:UsermenuParameter.m_dev_env_mode=%d\n", UsermenuParameter.m_dev_env_mode);

}

ts_void testpara_reset_det_parameter(void)
{

    ISP_set_det_calibrationing_flag(1);
    ISP_pause();

    if (SystemParaMenu2.INT_T >= 77)
    {
        SystemParaMenu2.INT_T = 76;
    }
	 //如果是vot_planck1280c，则写入vot_planck1280c的det寄存器配置，否则写入vot_planck640c的det寄存器配置
	#ifdef CONFIG_SNR_PLANCK1280C
	{
        vision_log_debug("SystemParaMenu2.Sel_PJ=%d\n", SystemParaMenu2.Sel_PJ);
        vision_log_debug("SystemParaMenu2.GOF1280=%d\n", SystemParaMenu2.GOF1280);
        vision_log_debug("SystemParaMenu2.GAIN1280=%d\n", SystemParaMenu2.GAIN1280);
        vision_log_debug("SystemParaMenu2.Cint1280=%d\n", SystemParaMenu2.Cint1280);
        vision_log_debug("SystemParaMenu2.Nint=%d\n", SystemParaMenu2.Nint);
        vision_log_debug("SystemParaMenu2.NTCC=%d\n", SystemParaMenu2.NTCC);
        vision_log_debug("SystemParaMenu2.Voccm=%d\n", SystemParaMenu2.Voccm);
        vision_log_debug("SystemParaMenu2.detConfig=%d\n", SystemParaMenu2.detConfig);
        vision_log_debug("SystemParaMenu2.Vfid=%d\n", SystemParaMenu2.Vfid);
        planck1280c_write_Sel_PJ(SystemParaMenu2.Sel_PJ);//SystemParaMenu2.Sel_PJ=0;
        planck1280c_write_gof(SystemParaMenu2.GOF1280);
        planck1280c_write_gain_adc(SystemParaMenu2.GAIN1280);
        planck1280c_write_cint(SystemParaMenu2.Cint1280);
        planck1280c_write_Nint(SystemParaMenu2.Nint);
        planck1280c_write_NTCC(SystemParaMenu2.NTCC);
        planck1280c_write_Voccm(SystemParaMenu2.Voccm);
        planck1280c_write_detConfig(SystemParaMenu2.detConfig);
        planck1280c_write_Vfid(SystemParaMenu2.Vfid);
    }
    #else
    {
        vision_log_debug("SystemParaMenu2.GAIN=%d\n", SystemParaMenu2.GAIN);
        vision_log_debug("SystemParaMenu2.GOF=%d\n", SystemParaMenu2.GOF);
        vision_log_debug("SystemParaMenu2.INT_T=%d\n", SystemParaMenu2.INT_T);
        vision_log_debug("SystemParaMenu2.INT_C=%d\n", SystemParaMenu2.INT_C);
        vision_log_debug("SystemParaMenu2.OOC_Par=%d\n", SystemParaMenu2.OOC_Par);   
        planck640c_write_gain_adc(SystemParaMenu2.GAIN);
        planck640c_write_gof(SystemParaMenu2.GOF);
        planck640c_write_cint(SystemParaMenu2.INT_C);
        planck640c_write_tint(SystemParaMenu2.INT_T);
        planck640c_write_rst_data(77-SystemParaMenu2.INT_T);
        planck640c_write_gos(SystemParaMenu2.OOC_Par);
	}
	#endif
    ISP_run();
    ts_cpl_fix_msleep(300);
    ISP_cut();
    ISP_set_det_calibrationing_flag(0);
}

ts_void testpara_reset(void)
{

    ISP_ee_enhance_mode_set(SystemParaMenu2.m_ee_enhance_mode, 1);
    display_det_info();
}

ts_void show_packet(ts_u08 *payload, ts_u32 payloadlen)
{
    int i = 0;
    for (i = 0; i < payloadlen; i++)
    {
        vision_log_debug(" 0x%x \n", payload[i]);
    }
}

ts_void unsave_local_isp_cfg(void)
{
    SystemParaMenu2.m_local_isp_magic_value = 0x0;
}

ts_void save_local_isp_cfg(void)
{
    // ts_isp_cfg_reg_s_t* isp_cfg_ptr = ISP_get_usr_cfg_ptr();
    // ISP_write_vot_isp_cfg(*isp_cfg_ptr);
    ISP_get_usr_cfg(&global_sys_ctrl_info.m_isp_cfg);
    ISP_write_vot_isp_cfg(global_sys_ctrl_info.m_isp_cfg);
    SystemParaMenu2.m_local_isp_magic_value = LOCAL_ISP_MAGIC_NUM;
}

ts_void reset_local_isp_cfg(void)
{
    vision_log_debug(" reset_local_isp_cfg m_local_isp_magic_value=0x%x\n", SystemParaMenu2.m_local_isp_magic_value);
    if (SystemParaMenu2.m_local_isp_magic_value == LOCAL_ISP_MAGIC_NUM)
    {
        ts_u08 ret = ISP_read_vot_isp_cfg(&global_sys_ctrl_info.m_isp_cfg);
        if (ret == 0)
        {
            ISP_set_usr_cfg(&global_sys_ctrl_info.m_isp_cfg);
            vision_log_debug("  reset_local_isp_cfg done \n");
        }
        else
        {
            vision_log_debug(" error  reset_local_isp_cfg\n");
        }
    }

    ISP_set_usr_mode_enalbe(1);
}

ts_void reset_usr_menu1_isp_cfg(void)
{

    vot_isp_luma_set();
    vot_isp_coef_set();

    ISP_enhance_set(UsermenuParameter.enhance_level * 16);
    ISP_nr_set(UsermenuParameter.SFilter * 8);
    ISP_ee_enhance_mode_set(SystemParaMenu2.m_ee_enhance_mode, 0);
    ISP_agc_luma_set(64);


}

/**
 * @description: 重置isp配置文件，根据室内还是室外
 * @return {*}
 */
ts_void reset_usr_isp_cfg(void)
{
    vot_vpo_freeze();

    load_isp_config();

    ISP_set_usr_mode_enalbe(1);

    // 恢复图像参数
    reset_usr_menu1_isp_cfg();

    ts_cpl_fix_msleep(200);
    vot_vpo_unfreeze();
}

ts_void user_isp_cfg_save(void)
{
    return;
}

/**
 * @description: 加载用户出厂isp相关配置文件
 * @return {ts_void}
 */
// ts_void usr_isp_cfg_reset(void)
// {
//     // name               offset                  size
//     // isp_cfg       0x80000 + 0x80000(512kb)      (0x1000)4096B

//     // isp_cfg
//     ts_u32 u32_addr = 0x100000;
//     ts_u32 want_rd_size = 4096;
//     ts_u32 rd_cnt = 0;
//     user_cfg_header_s_t usr_cfg_header;
//     // user_cfg_node_header_s_t usr_cfg_node;
//     ts_u32 rd_offset = 0;

//     ts_u08 *pu08_rd_buf = (ts_u08 *)vot_tlsf_safe_malloc(&g_VotMemPoolInfo,(4096 * 3 )) ;

//     want_rd_size = sizeof(user_cfg_header_s_t);

//     rd_cnt = ts_lib_sfc_raw_read(u32_addr, pu08_rd_buf, want_rd_size);
//     if (rd_cnt > 0)
//     {
//         vision_log_debug(" ts_lib_sfc_raw_read rd_cnt=%d\n", rd_cnt);
//         // show_packet(pu08_rd_buf, 16);
//         memcpy(&usr_cfg_header, pu08_rd_buf, want_rd_size);
//         vision_log_debug(" usr_cfg_header.base_addr=0x%x \n", usr_cfg_header.base_addr);
//         vision_log_debug(" usr_cfg_header.cfg_node_nums=%u \n", usr_cfg_header.cfg_node_nums);
//         vision_log_debug(" usr_cfg_header.cfg_total_bytes=%u \n", usr_cfg_header.cfg_total_bytes);

//         // if(usr_cfg_header.base_addr>0 && usr_cfg_header.cfg_total_bytes>0 && usr_cfg_header.cfg_node_nums>0){

//         // }

//         if (usr_cfg_header.cfg_total_bytes > 0x80000 || usr_cfg_header.cfg_node_nums > 64)
//         {
//             vision_log_debug("error: usr_cfg_header.cfg_total_bytes=%u > 0x80000 \n", usr_cfg_header.cfg_total_bytes);
//             vision_log_debug("error: usr_cfg_header.cfg_node_nums=%u > 64 \n", usr_cfg_header.cfg_node_nums);
//         }
//         else
//         {

//             int cnt = 0;

//             for (cnt = 0; cnt < usr_cfg_header.cfg_node_nums; cnt++)
//             {
//                 vision_log_debug("cnt %d usr_cfg_header.cfg_node_array[cnt].name  %s \n", cnt, usr_cfg_header.cfg_node_array[cnt].name);
//                 vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].offset  0x%x \n", cnt, usr_cfg_header.cfg_node_array[cnt].offset);
//                 vision_log_debug("cnt %d  usr_cfg_header.cfg_node_array[cnt].size  0x%x \n", cnt, usr_cfg_header.cfg_node_array[cnt].size);
//                 rd_offset = sizeof(usr_cfg_header);
//                 u32_addr = 0x100000;

//                 if (cnt == 0)
//                 {

//                     rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
//                     u32_addr += rd_offset;
//                     want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
//                     if (want_rd_size == 0)
//                     {
//                         continue;
//                     }

//                     vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n", u32_addr, want_rd_size);
//                     rd_cnt = ts_lib_sfc_raw_read(u32_addr, pu08_rd_buf, want_rd_size);
//                     if (rd_cnt > 0)
//                     {
//                         vision_log_debug(" ts_lib_sfc_raw_read rd_cnt=%d\n", rd_cnt);
//                         show_packet(pu08_rd_buf, 16);
//                         if (UsermenuParameter.m_dev_env_mode == DEVICE_MODE_OUTDOOR)
//                         {
//                             ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t *)pu08_rd_buf;
//                             ISP_set_usr_cfg(pst_isp_reg);
//                             vision_log_debug(" DEVICE_MODE_OUTDOOR \n");
//                         }
//                     }
//                     else
//                     {
//                         vision_log_debug(" ts_lib_sfc_raw_read rd_failed=%d\n", rd_cnt);
//                     }
//                 }
//                 else if (cnt == 1)
//                 {
//                     rd_offset += usr_cfg_header.cfg_node_array[cnt].offset;
//                     u32_addr += rd_offset;
//                     want_rd_size = usr_cfg_header.cfg_node_array[cnt].size;
//                     if (want_rd_size == 0)
//                     {
//                         continue;
//                     }

//                     vision_log_debug(" u32_addr=0x%x want_rd_size 0x%x \n", u32_addr, want_rd_size);
//                     rd_cnt = ts_lib_sfc_raw_read(u32_addr, pu08_rd_buf, want_rd_size);
//                     if (rd_cnt > 0)
//                     {
//                         vision_log_debug(" ts_lib_sfc_raw_read rd_cnt=%d\n", rd_cnt);
//                         show_packet(pu08_rd_buf, 16);
//                         if (UsermenuParameter.m_dev_env_mode == DEVICE_MODE_INDOOR)
//                         {
//                             vision_log_debug(" DEVICE_MODE_INDOOR \n");
//                             ts_isp_cfg_reg_s_t *pst_isp_reg = (ts_isp_cfg_reg_s_t *)pu08_rd_buf;
//                             ISP_set_usr_cfg(pst_isp_reg);
//                         }
//                     }
//                     else
//                     {
//                         vision_log_debug(" ts_lib_sfc_raw_read rd_failed=%d\n", rd_cnt);
//                     }
//                 }
//             }
//         }
//     }
//     else
//     {
//         vision_log_debug(" ts_lib_sfc_raw_read rd_failed=%d\n", rd_cnt);
//     }

//     if(pu08_rd_buf){
//         vot_tlsf_safe_free(&g_VotMemPoolInfo,(ts_void**)&pu08_rd_buf); 
//     }

 
//     display_det_info();
// }

/**
 * @description: I2C读取测试用例
 * @return {*}
 */
void i2c_write_read(void)
{
}

/**
 * @description: isp 部分功能测试用例
 * @return {void}
 */
void isp_sensor_test(void)
{
}

/**
 * @description: sffc锅盖开关
 * @param {ts_u08} sffc_mode ： SFFC_MODE_ENUM
 * @param {ts_u08 restore_or_backup} 备份或者恢复上次状态
 * @return {void}
 */
void sffc_on_off(ts_u08 sffc_mode, ts_u08 restore_or_backup)
{
    // if(restore_or_backup==OPERATION_MODE_BACKUP){
    //     global_sys_ctrl_info.m_sffc_mode_backup = SystemParaMenu2.m_sffc_mode;
    // }
    // else if(restore_or_backup==OPERATION_MODE_RESTORE){
    //     sffc_mode = global_sys_ctrl_info.m_sffc_mode_backup;
    // }

    if (sffc_mode >= SFFC_MODE_BUTT)
    {
        sffc_mode = SFFC_MODE_AUTO;
    }

    vision_log_debug("sffc_mode [%d] \n", sffc_mode);

    SystemParaMenu2.m_sffc_mode = sffc_mode;

#ifndef VOT_NUC_SFFC_TEST

    if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_OFF)
    {
        sffc_off();
    }
    else if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_AUTO)
    {
        sffc_on_a();
    }
    else if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_MANUAl)
    {
        SystemParaMenu2.m_sffc_level = 8000;
        sffc_on_m(SystemParaMenu2.m_sffc_level);
    }
#else
    ISP_set_auto_cut_flag_auto();
#endif

}

void shutter_set_mode(ts_u08 shutter_mode)
{
    if (shutter_mode == SHUTTER_MODE_OFF)
    {
        UsermenuParameter.ShutterMode = SHUTTER_MODE_OFF;
        // 不改变用户菜单参数
        //  UsermenuParameter.AutoShutter = 0;
    }
    else
    {
        UsermenuParameter.ShutterMode = shutter_mode;
        if (UsermenuParameter.AutoShutter == 0)
        {
            UsermenuParameter.AutoShutter = 1;
        }
    }
    ISP_set_auto_cut_flag_auto();
}

/**
 * @description: 打档开关切换
 * @param {ts_u08} shutter_mode ： enum SHUTTER_MODE_ENUM
 * @param {ts_u08 restore_or_backup} 备份或者恢复上次状态
 * @return {void}
 */
void shutter_cut_on_off(ts_u08 shutter_mode, ts_u08 restore_or_backup)
{
    if (restore_or_backup == OPERATION_MODE_BACKUP)
    {
        global_sys_ctrl_info.m_shutter_mode_backup = UsermenuParameter.ShutterMode;
    }
    else if (restore_or_backup == OPERATION_MODE_RESTORE)
    {
        shutter_mode = global_sys_ctrl_info.m_shutter_mode_backup;
    }

    if (shutter_mode >= SHUTTER_MODE_BUTT)
    {
        shutter_mode = SHUTTER_MODE_OFF;
    }

    if (shutter_mode == SHUTTER_MODE_OFF)
    {
        global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_OFF;
    }
    else
    {
        global_sys_ctrl_info.m_shutter_mode = SHUTTER_MODE_TIMER;
    }

    vision_log_debug("shutter_mode [%d] \n", shutter_mode);

    shutter_set_mode(shutter_mode);
}


ts_void vot_device_init(void)
{

    ts_u32 u32_width, u32_height;

    ISP_set_auto_cut_flag(0);

    ISP_init_user_cfg();

    // 加载固件携带的默认isp场景配置。
    load_isp_config();

    ISP_set_usr_mode_enalbe(1);

    // 生效菜单一和菜单二的参数
    testpara_reset();
    userpara_reset();

    // ts_cpl_fix_msleep(1000);
    vision_log_warn("m_sys_boot_mode %d \n", SystemParaMenu2.m_sys_boot_mode);
  
    vot_occ_update_with_calibration(0);


    ts_u08 SFilterPre = UsermenuParameter.SFilter;
    UsermenuParameter.SFilter = 20;

    if(SystemParaMenu2.m_temp_drift_comp_mode!=TEMP_DRIFT_COMPENSATION_MODE_ON){
        ISP_dde_set_en(0);
        // ISP_dde_set_mode(1);
    }
    ISP_set_auto_cut_flag(1);
    
    if(SystemParaMenu2.m_temp_drift_comp_mode==TEMP_DRIFT_COMPENSATION_MODE_ON){
        vot_set_temp_drift_calibration_mode(get_powerup_temp_drift_comp_mode(),1);
        ISP_force_cut(1);
    }

    vot_snr_set_grp_pod_switch(0);

    // 加载用户自己isp配置，用于动态修改isp配置，方便调试，不用重新打包固件
    reset_local_isp_cfg();

    vot_check_bpc_ext_para(); // 坏斑功能

    update_cvbs_vpo_attr();

    ISP_fix_snc_switch_set(SystemParaMenu2.m_fix_snc_switch);

    // dector_resolution_type_t res_type =  get_powerup_dector_resolution_type();

    u32_width = 640;  //(ts_u32)st_vpo_attr.u16_disp_width;
    u32_height = 400; // 320;//480;//(ts_u32)st_vpo_attr.u16_disp_height;
    #ifdef CONFIG_SNR_PLANCK1280C
    {
        u32_width = 1280;  //(ts_u32)st_vpo_attr.u16_disp_width;//
        u32_height =1024;//900 ;
    }
  	#endif
    // osd 是矩形框是0,0点开始，矩形框是640x320
    global_sys_ctrl_info.pst_framebuffer_osd = display_osd_init(shdl_osd_dev, u32_width, u32_height);
    init_cvbs_osd_handler();
    vot_osd_node_clear_info(global_sys_ctrl_info.m_bottom_info_osd_node);
    // vot_osd_node_clear_info(global_sys_ctrl_info.m_cvbs_cursor_osd_dot);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
    vot_osd_node_clear_info(global_sys_ctrl_info.pst_framebuffer_osd);
    vision_log_debug("before   vot_refresh_osd_bind \n");

    vot_refresh_osd_bind();

    init_templut_osd_handler();
    vision_log_debug("after   init_templut_osd_handler \n");

    if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_MANUAl)
    {
        sffc_on_off(SFFC_MODE_MANUAl, OPERATION_MODE_NONE);
    }
    else if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_OFF)
    {
        sffc_on_off(SFFC_MODE_OFF, OPERATION_MODE_NONE);
    }
    else if (SystemParaMenu2.m_sffc_mode == SFFC_MODE_AUTO)
    {
        sffc_on_off(SFFC_MODE_AUTO, OPERATION_MODE_NONE);
    }


 
    vision_log_warn("vot_snr_set_grp_pod_switch \n");

    cvbs_cursor_refresh_display(0);
    cvbs_osd_refresh_display(1);
    
    vot_usb_device_init();
    load_palette(UsermenuParameter.Palette);

    ISP_enhance_set(UsermenuParameter.enhance_level * 16);
    ISP_nr_set(UsermenuParameter.SFilter * 8);
    vot_menu_init();

    vot_isp_luma_set();
    vot_isp_coef_set();

    ts_tdk_osd_refresh_display( TS_OSD_REFRESH_VISIBLE_ON);
    
    // if(SystemParaMenu2.m_temp_drift_comp_mode!=TEMP_DRIFT_COMPENSATION_MODE_ON){
    //     if(res_type!=DECTOR_RESOLUTION_384X288){
    //         ISP_dde_set_en(1);
    //     }
    // }

    ISP_vot_zoom_set(UsermenuParameter.Enlarge);

	if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{
        vot_pip_mode_set(UsermenuParameter.m_pip_mode, RZ_RATIO_MODE_2X, 1);
        init_hst_osd_handler();
        vot_hst_mode_set(UsermenuParameter.m_hst_mode);
    }
   
    special_para_init(UsermenuParameter.Palette);    
    
    UsermenuParameter.SFilter = SFilterPre;
    vision_log_debug("leave: vot_device_init \n");
}

ts_void display_init(ts_void)
{
    // ts_cpl_task_s_t st_osd_task;
    ts_osd_ch_attr_s_t st_osd_disp_ch_attr;
    su32_inited = 0;

    if (0 == su32_inited)
    {
        st_osd_disp_ch_attr.pst_rgb = (const ts_rgb_s_t *)&sast_mnu_rgbs_vot;
        st_osd_disp_ch_attr.u16_rgb_num = VOT_OSD_COLOR_MAX;
        st_osd_disp_ch_attr.u08_pixel_bw = TS_PIXEL_BW_2BIT;
        st_osd_disp_ch_attr.u08_fill_pixel = 0;
        st_osd_disp_ch_attr.u32_merge_size = 32 * 1024;

        shdl_osd_dev = TS_OSD_ALL_MDEV0;

        vision_log_debug("SystemParaMenu2.m_osd_show_on_dev %d\n", SystemParaMenu2.m_osd_show_on_dev);

        ts_tdk_osd_set_ch_attr((const ts_osd_ch_attr_s_t *)&st_osd_disp_ch_attr);

        su32_inited = 1;

        vot_device_init();
 
    }
}

static ts_void vot_clean_parse_info(vot_hbp_ctx_s_t *p_vot_uart0_ctx)
{
    if (p_vot_uart0_ctx)
    {
        p_vot_uart0_ctx->u08_chk_sum = 0;
        p_vot_uart0_ctx->u32_wr_idx = 0;

        /* 最小接收个数为一个帧头 */
        // p_vot_uart0_ctx->u32_total_len = sizeof(vot_hbp_head_s_t);
        p_vot_uart0_ctx->u08_packet_complete_flag = 0;
        vot_uart0_ctx.u32_total_len = 0;
    }
}

ts_void vot_show_packet(ts_u08 *buff, ts_u32 bytes, ts_char *from)
{
    ts_u32 index = 0;
    if (from == NULL || buff == NULL)
    {
        return;
    }
    printf("\n from %s start: ", from);
    bytes = bytes > 32 ? 32 : bytes;
    for (index = 0; index < bytes; index++)
    {
        printf(" 0x%x ", buff[index]);
    }
    printf(" end \n");
}

ts_u32 vot_uart_write(ts_u08 *buff, ts_u32 bytes)
{
    // vot_show_packet( buff,  bytes, "A->PC");
    if(vot_get_force_debug_flag()==0){
        if (vot_uart0_ctx.s32_dev > 0 && bytes > 0)
        {
            return write(vot_uart0_ctx.s32_dev, buff, bytes);
        }
    }
    return 0;
}

ts_u32 vot_uart1_write(ts_u08 *buff, ts_u32 bytes)
{

    if (vot_uart1_ctx.s32_dev > 0 && bytes > 0)
    {
        return write(vot_uart1_ctx.s32_dev, buff, bytes);
    }

    return 0;
}

void ring_buffer_init(ring_buffer_t *rb, ts_char *buf_name)
{
    // ring_buffer_t *rb = &(vot_uart0_ctx.m_ring_buf_instance);
    rb->write_index = 0;
    rb->read_index = 0;
    rb->left_bytes = 0;
    rb->uart_buff_len = 512 * 1024;
    #ifdef CONFIG_SNR_PLANCK1280C
    //改为存放的内存地址：22000000
    rb->buffer = (ts_u32)ts_cpl_tmm_malloc(gpv_tmm_usr, rb->uart_buff_len, 32, buf_name);
    #else
    rb->buffer = (ts_u32)ts_cpl_tmm_malloc(ts_null, rb->uart_buff_len, 32, buf_name);
    #endif
    rb->uart_buff = (ts_u08 *)rb->buffer;
    ts_cpl_mutex_init(&rb->ring_buff_mutex);
    // memset(rb->uart_buff, 0, rb->uart_buff_len); // 非必须
}

void ring_buffer_deinit(ring_buffer_t *rb)
{

    rb->write_index = 0;
    rb->read_index = 0;
    rb->left_bytes = 0;
    rb->uart_buff_len = 512 * 1024;
    if (rb->buffer)
    {
        ts_cpl_tmm_free((ts_void *)rb->buffer);
        rb->buffer = 0;
    }
    rb->uart_buff = 0;
    ts_cpl_mutex_destory(&rb->ring_buff_mutex);
}

void get_ring_buffer_info(ring_buffer_t *dst_rb, ring_buffer_t *src_rb)
{
    ts_cpl_mutex_lock(&src_rb->ring_buff_mutex);

    ts_cpl_memcpy(dst_rb, src_rb, sizeof(ring_buffer_t));
    ts_cpl_mutex_unlock(&src_rb->ring_buff_mutex);
}

void ring_buffer_update_read(ring_buffer_t *rb_in, ring_buffer_t *src_rb)
{
    ts_cpl_mutex_lock(&src_rb->ring_buff_mutex);

    // ring_buffer_t *rb = &(vot_uart0_ctx.m_ring_buf_instance);
    src_rb->read_index = rb_in->read_index;
    src_rb->left_bytes -= rb_in->left_bytes;
    ts_cpl_mutex_unlock(&src_rb->ring_buff_mutex);
}

ts_bool ring_buffer_update_write(ring_buffer_t *rb, ts_u08 *data_in, ts_u32 data_len)
{

    ts_cpl_mutex_lock(&rb->ring_buff_mutex);

    ts_u32 will_left_bytes = rb->left_bytes + data_len;
    if (will_left_bytes > rb->uart_buff_len) // 缓存数据大于最大ring长度了,先不写入
    {
        ts_cpl_mutex_unlock(&rb->ring_buff_mutex);
        return ts_false;
    }

    ts_u32 byte_index = rb->write_index + data_len;

    // vision_log_debug("write_index=%u  byte_index=%u  rb->uart_buff_len %u \n", rb->write_index ,byte_index ,rb->uart_buff_len);

    if (byte_index > rb->uart_buff_len)
    {
        ts_u32 bytes_size = rb->uart_buff_len - rb->write_index;
        ts_u08 *dst_ptr = rb->uart_buff + rb->write_index;
        ts_u08 *src_ptr = data_in;
        ts_cpl_memcpy(dst_ptr, src_ptr, bytes_size);
        rb->write_index = 0;

        dst_ptr = rb->uart_buff;
        src_ptr = data_in + bytes_size;
        ts_u32 bytes_left = data_len - bytes_size;

        ts_cpl_memcpy(dst_ptr, src_ptr, bytes_left);
        rb->write_index = (byte_index % rb->uart_buff_len);
    }
    else
    {
        ts_u08 *dst_ptr = rb->uart_buff + rb->write_index;
        ts_u08 *src_ptr = data_in;
        memcpy(dst_ptr, src_ptr, data_len);
        rb->write_index = (byte_index % rb->uart_buff_len);
        ;
    }
    rb->left_bytes += data_len;

    ts_cpl_mutex_unlock(&rb->ring_buff_mutex);

    return ts_true;
}

ts_u16 vot_asic_msg_check_sum(ts_u08 *pu08_ptr)
{
    ts_u16 u16_total_len = 0;
    ts_u16 i;
    ts_u08 check = 0;

    if (pu08_ptr[0] != 0xA5 || pu08_ptr[1] != 0x52)
        return 0;

    u16_total_len = pu08_ptr[2] | (pu08_ptr[3] << 8);

    if (u16_total_len > 2048)
    {
        return 0;
    }

    for (i = 0; i < u16_total_len + 7; i++)
    {
        check += pu08_ptr[i];
    }
    check = ~check;

    if (check != pu08_ptr[u16_total_len + 7])
    {
        return 0;
    }

    return 1;
}

ts_void vot_hbp_parse_cb_v2(state_machine_t *state_machine, ts_u08 *u08_val, ts_u16 read_bytes, FuncVotTransfer *transfer_callback)
{
    ts_u08 *pu08_ptr;
    ts_u16 index = 0;

    ts_u08 *data = u08_val;
    ts_u16 data_len = 0;
    ts_u16 cp_len = 0;
    ts_u16 will_index = 0;
    // ts_u08 test_data[30]={0x6,0x6,0x06,0x6,0x6};

    for (index = 0; index < read_bytes;)
    {

        switch (state_machine->m_state)
        {
        default:
        {
            state_machine->m_state = STATE_WAIT_HEADER;
        }
        case STATE_WAIT_HEADER:
        {

            if (data[index] == 0x56 && data[index + 1] == 0x4f && data[index + 2] == 0x54)
            {

                state_machine->m_frame_type = UART_MSG_TYPE_VOT_NORMAL;

                data_len = ((data[index + 4] & 0xff) | (data[index + 3] << 8)) + 5;
                state_machine->m_complete_frame_len = data_len;
                state_machine->m_frame_bytes_index = 0;
                if (data_len > 2048)
                {
                    state_machine->m_state = STATE_WAIT_HEADER;
                    state_machine->m_complete_frame_len = 0;
                    break;
                }
                state_machine->m_state = STATE_WAIT_COMPLETE_FRAME;

                will_index = index + data_len;
                pu08_ptr = (ts_u08 *)state_machine->s08_buff;

                // 当前的接收到的数据不完整，分两次拷贝
                if (will_index > read_bytes)
                {
                    cp_len = read_bytes - index;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                else
                {
                    cp_len = data_len;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                state_machine->m_frame_bytes_index += cp_len;
                index += cp_len;
                // vision_log_debug("index %d cp_len %d  will_index %d state_machine->m_frame_bytes_index %d \n", index,cp_len, will_index, state_machine->m_frame_bytes_index);
            }
            else if (data[index] == 0xA5 && data[index + 1] == 0x52)
            {

                state_machine->m_frame_type = UART_MSG_TYPE_VOT_ASIC;
                data_len = ((data[index + 2] & 0xff) | (data[index + 3] << 8)) + 8;

                state_machine->m_complete_frame_len = data_len;
                state_machine->m_frame_bytes_index = 0;
                if (data_len > 2048)
                {
                    state_machine->m_state = STATE_WAIT_HEADER;
                    state_machine->m_complete_frame_len = 0;
                    break;
                }
                state_machine->m_state = STATE_WAIT_COMPLETE_FRAME;

                will_index = index + data_len;
                pu08_ptr = (ts_u08 *)&state_machine->s08_buff;

                if (will_index > read_bytes)
                {
                    cp_len = read_bytes - index;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                else
                {
                    cp_len = data_len;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                state_machine->m_frame_bytes_index += cp_len;
                index += cp_len;

                // state_machine->m_frame_type = UART_MSG_TYPE_VOT_ASIC ;
                // vision_log_debug("index %d cp_len %d  will_index %d \n", index,cp_len, will_index);
            }
            else
            {
                index++;
                // vision_log_debug("index %d cp_len %d  will_index %d \n", index,cp_len, will_index);
            }
        }
        case STATE_WAIT_COMPLETE_FRAME:
        {

            if (state_machine->m_frame_bytes_index != state_machine->m_complete_frame_len)
            {
                pu08_ptr = (ts_u08 *)&state_machine->s08_buff;
                pu08_ptr += state_machine->m_frame_bytes_index;

                ts_u32 left_bytes = (state_machine->m_complete_frame_len - state_machine->m_frame_bytes_index);
                will_index = index + left_bytes;

                if (will_index > read_bytes)
                {
                    cp_len = read_bytes - index;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                else
                {
                    cp_len = left_bytes;
                    if (cp_len)
                    {
                        ts_cpl_memcpy(pu08_ptr, &data[index], cp_len);
                    }
                }
                state_machine->m_frame_bytes_index += cp_len;
                index += cp_len;
            }
            // else
            {

                if (state_machine->m_complete_frame_len > 0 && state_machine->m_frame_bytes_index == state_machine->m_complete_frame_len)
                {

                    // ts_cpl_mutex_lock(&global_sys_ctrl_info.m_uart1_msg_mutex);

                    if (state_machine->m_frame_type == UART_MSG_TYPE_VOT_ASIC)
                    {
                        vision_log_debug("UART_MSG_TYPE_VOT_ASIC \n");
                        if (vot_asic_msg_check_sum((ts_u08 *)&state_machine->s08_buff) > 0)
                        {

                            ts_u16 u16_msg_len = state_machine->m_complete_frame_len - 8;
                            ts_u08 *pu08_ptr = (ts_u08 *)&state_machine->s08_buff;
                            pu08_ptr += 4;
                            Asic_Update_Comm_Data(pu08_ptr, u16_msg_len);
                        }
                    }
                    else if (state_machine->m_frame_type == UART_MSG_TYPE_VOT_NORMAL)
                    {

                        // ts_u08* pu08_rbuf =  state_machine->s08_buff;

                        vision_log_debug("UART_MSG_TYPE_VOT_NORMAL \n");
                        Asic_Update_Comm_Data((ts_u08 *)&state_machine->s08_buff, state_machine->m_complete_frame_len);
                        uart_data_flag = 1;
                        // ts_cpl_up(&state_machine->m_st_sem);
                        // ts_cpl_up(&vot_uart0_ctx.st_sem);
                    }

                    state_machine->m_state = STATE_WAIT_HEADER;
                    state_machine->m_complete_frame_len = 0;
                    state_machine->m_frame_bytes_index = 0;

                    global_sys_ctrl_info.m_transfer_callback = transfer_callback;

                    ASIC_Data_Packets_Receive_And_Process();
                    // ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_uart1_msg_mutex);
                }
            }

            // index++;
        }

            // vision_log_debug("index %d cp_len %d  will_index %d  state_machine->m_frame_bytes_index %d \n", index,cp_len, will_index, state_machine->m_frame_bytes_index);
            // vision_log_debug("m_frame_bytes_index %d state_machine->m_complete_frame_len %d \n",state_machine->m_frame_bytes_index,state_machine->m_complete_frame_len);
        }
    }

    // return ;
}


static ts_s32 uart0_recv_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ts_u08 au08_rd_buf[2048];
    ts_u32 u32_rnd_cnt;

    /* 初始化hbp解析参数*/
    vot_clean_parse_info(&vot_uart0_ctx);

    vot_uart0_ctx.s32_dev = open("/dev/ttyS0", O_RDWR);

    // ts_u08* pu08_ptr;
    // ts_u32 u32_total_len;
    // ts_u08 key_str[] = {0xff,0xff};
    // ts_u08* pu08_vot_ptr;

    while (1)
    {

        if (vot_uart0_ctx.s32_dev > 0)
        {
            u32_rnd_cnt = read(vot_uart0_ctx.s32_dev, (ts_void *)&au08_rd_buf, 2048);
            // vision_log_debug("rd cnt = %d\n", u32_rnd_cnt);

            if (u32_rnd_cnt > 0 && global_sys_ctrl_info.m_sys_init_finished_flag == 1)
            {

                ring_buffer_update_write(&vot_uart0_ctx.m_ring_buf_instance, au08_rd_buf, u32_rnd_cnt);
            }
        }
        else
        {
            ts_cpl_msleep(3000);
            if (vot_uart0_ctx.s32_dev)
            {
                close(vot_uart0_ctx.s32_dev);
            }
            ts_cpl_msleep(1000);
            vot_uart0_ctx.s32_dev = open("/dev/ttyS0", O_RDWR);
        }
    }

    if (vot_uart0_ctx.s32_dev)
    {
        close(vot_uart0_ctx.s32_dev);
        vot_uart0_ctx.s32_dev = -1;
    }

    return 0;
}

/**
 * @description: 串口消息处理线程
 * @param {ts_void} *pv_param
 * @return {*}
 */
static ts_s32 uart0_msg_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ring_buffer_t uart_rb;
    ts_u08 *rd_buf;
    ts_u32 rnd_cnt;
    ts_u32 bytes_index;
    ts_u32 left_bytes;
    // ts_u32  m_left_bytes;

    state_machine_t m_state_machine;
    m_state_machine.m_state = STATE_WAIT_HEADER;
    m_state_machine.m_frame_type = 0;
    m_state_machine.m_complete_frame_len = 0;
    m_state_machine.m_frame_bytes_index = 0;
// m_state_machine.m_s32_dev = -1;
#ifdef Receive_JSON_File_EN
    ts_u08 *rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff; // add by changguanghe 2026-03-18, 避免每次解析都在栈上申请大内存，改为全局内存
    ts_u08 *json_str = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
    static ts_u16 rx_json_buf_count = 0;
    memset(rd_json_buf, 0, 1024 * 4);
#endif

    while (1)
    {

        get_ring_buffer_info(&uart_rb, &(vot_uart0_ctx.m_ring_buf_instance));
        if (uart_rb.write_index != uart_rb.read_index)
        {

            get_ring_buffer_info(&uart_rb, &(vot_uart0_ctx.m_ring_buf_instance));
            if (uart_rb.left_bytes == 0)
            {
                ts_cpl_msleep(20);
                continue;
            }

            bytes_index = uart_rb.left_bytes + uart_rb.read_index;

            // vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
            if (bytes_index > uart_rb.uart_buff_len)
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.uart_buff_len - uart_rb.read_index;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_uart_write);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif

                rd_buf = uart_rb.uart_buff;
                left_bytes = uart_rb.left_bytes - rnd_cnt;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, left_bytes, &vot_uart_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, left_bytes); // add by changguanghe 2026-03-18
                rd_json_buf += left_bytes;
#endif
            }
            else
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.left_bytes;

                //  vot_uart_write(rd_buf,rnd_cnt);

                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_uart_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif
            }
#ifdef Receive_JSON_File_EN
            rx_json_buf_count++;
#endif
            ring_buffer_update_read(&uart_rb, &(vot_uart0_ctx.m_ring_buf_instance));
        }
        else
        {
            ts_cpl_msleep(50);
#ifdef Receive_JSON_File_EN
            if (rx_json_buf_count > 0)
            {
                rx_json_buf_count++;
                if ((rx_json_buf_count > 30)) // add by changguanghe 2026-03-18, 避免长时间未解析导致的json数据过大占满内存，增加解析次数限制，超过限制则丢弃数据并重置计数器
                {
                    // vision_log_debug("json_str len: %d \n", strlen(json_str));
                    if (strlen((char *)json_str) > 250) // add by changguanghe 2026-03-18, 避免解析异常数据导致的系统问题，增加json长度限制，超过限制则丢弃数据并重置计数器
                    {
                        ParseJsonParameters((char *)json_str);
                    }
                    rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
                    memset(json_str, 0, 1024 * 4); // add by changguanghe 2026-03-18, 解析完成后重置json缓冲区和计数器
                    rx_json_buf_count = 0;
                }
            }
#endif
        }
    }

    return 0;
}

static ts_s32 uart1_recv_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ts_u08 au08_rd_buf[2048];
    ts_u32 u32_rnd_cnt;

    /* 初始化hbp解析参数*/
    vot_clean_parse_info(&vot_uart1_ctx);

    vot_uart1_ctx.s32_dev = open("/dev/ttyS1", O_RDWR);

    // ts_u08* pu08_ptr;
    // ts_u32 u32_total_len;
    // ts_u08 key_str[] = {0xff,0xff};
    // ts_u08* pu08_vot_ptr;

    while (1)
    {

        if (vot_uart1_ctx.s32_dev > 0)
        {
            u32_rnd_cnt = read(vot_uart1_ctx.s32_dev, (ts_void *)&au08_rd_buf, 2048);
            // vision_log_debug("rd cnt = %d\n", u32_rnd_cnt);

            // if (u32_rn  d_cnt > 0) {
            if (u32_rnd_cnt > 0 && global_sys_ctrl_info.m_sys_init_finished_flag == 1)
            {
                ring_buffer_update_write(&vot_uart1_ctx.m_ring_buf_instance, au08_rd_buf, u32_rnd_cnt);
            }
        }
        else
        {
            ts_cpl_msleep(3000);
            if (vot_uart1_ctx.s32_dev)
            {
                close(vot_uart1_ctx.s32_dev);
            }
            ts_cpl_msleep(1000);
            vot_uart1_ctx.s32_dev = open("/dev/ttyS1", O_RDWR);
        }
    }

    if (vot_uart1_ctx.s32_dev)
    {
        close(vot_uart1_ctx.s32_dev);
        vot_uart1_ctx.s32_dev = -1;
    }

    return 0;
}

/**
 * @description: 串口消息处理线程
 * @param {ts_void} *pv_param
 * @return {*}
 */
static ts_s32 uart1_msg_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ring_buffer_t uart_rb;
    ts_u08 *rd_buf;
    ts_u32 rnd_cnt;
    ts_u32 bytes_index;
    ts_u32 left_bytes;
    // ts_u32  m_left_bytes;

    state_machine_t m_state_machine;
    m_state_machine.m_state = STATE_WAIT_HEADER;
    m_state_machine.m_frame_type = 0;
    m_state_machine.m_complete_frame_len = 0;
    m_state_machine.m_frame_bytes_index = 0;
// m_state_machine.m_s32_dev = -1;
#ifdef Receive_JSON_File_EN
    ts_u08 *rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff; // add by changguanghe 2026-03-18, 避免每次解析都在栈上申请大内存，改为全局内存
    ts_u08 *json_str = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
    static ts_u16 rx_json_buf_count = 0;
    memset(rd_json_buf, 0, 1024 * 4);
#endif

    while (1)
    {

        get_ring_buffer_info(&uart_rb, &(vot_uart1_ctx.m_ring_buf_instance));
        if (uart_rb.write_index != uart_rb.read_index)
        {

            get_ring_buffer_info(&uart_rb, &(vot_uart1_ctx.m_ring_buf_instance));
            if (uart_rb.left_bytes == 0)
            {
                ts_cpl_msleep(20);
                continue;
            }

            bytes_index = uart_rb.left_bytes + uart_rb.read_index;

            // vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
            if (bytes_index > uart_rb.uart_buff_len)
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.uart_buff_len - uart_rb.read_index;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_uart1_write);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif
                rd_buf = uart_rb.uart_buff;
                left_bytes = uart_rb.left_bytes - rnd_cnt;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, left_bytes, &vot_uart1_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, left_bytes); // add by changguanghe 2026-03-18
                rd_json_buf += left_bytes;
#endif
            }
            else
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.left_bytes;

                //  vot_uart1_write(rd_buf,rnd_cnt);

                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_uart1_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif
            }
#ifdef Receive_JSON_File_EN
            rx_json_buf_count++;
#endif
            ring_buffer_update_read(&uart_rb, &(vot_uart1_ctx.m_ring_buf_instance));
        }
        else
        {
            ts_cpl_msleep(50);
#ifdef Receive_JSON_File_EN
            if (rx_json_buf_count > 0)
            {
                rx_json_buf_count++;
                if ((rx_json_buf_count > 30)) // add by changguanghe 2026-03-18, 避免长时间未解析导致的json数据过大占满内存，增加解析次数限制，超过限制则丢弃数据并重置计数器
                {
                    // vision_log_debug("json_str len: %d \n", strlen(json_str));
                    if (strlen((char *)json_str) > 250) // add by changguanghe 2026-03-18, 避免解析异常数据导致的系统问题，增加json长度限制，超过限制则丢弃数据并重置计数器
                    {
                        ParseJsonParameters((char *)json_str);
                    }
                    rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
                    memset(json_str, 0, 1024 * 4); // add by changguanghe 2026-03-18, 解析完成后重置json缓冲区和计数器
                    rx_json_buf_count = 0;
                }
            }
#endif
        }
    }

    return 0;
}

/**
 * @description: 刷新osd状态栏信息线程
 * @param {ts_void} *pv_param
 * @return {ts_s32}
 */
// static ts_s32 update_statis_info_process(ts_void *pv_param)
// {
//     TS_UNUSED(pv_param);

//     // ts_u16 update_cnt=0;
//     // ts_cpl_msleep(3000);

// 	// while(1){

//     //     // ts_cpl_msleep(500);

//     //     // update_cnt++;
//     //     // if(update_cnt>2){
//     //     //     update_cnt = 0;
//     //     //     ts_cpl_mutex_lock(&global_sys_ctrl_info.m_uart1_msg_mutex);
//     //     //     {
//     //     //         GetAndDisplayIspStatisInfo(2);
//     //     //     }
//     //     //     ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_uart1_msg_mutex);
//     //     // }

// 	// }

// 	return 0;
// }

static ts_s32 vot_templut_update_process(ts_void *pv_param)
{
    const int RT_INTV = 1;  // 实时任务
    const int DL_INTV = 1;  // 延时任务
    ts_u32 dl_cnt = 0;
    ts_s32 s32_nPos = TS_UPL_POS_BEFORE_AGC;
    ts_u08 last_cycleClear_osd = 0;
    ts_s08 cRet = 0;

    for(;;) {
        if (global_sys_ctrl_info.m_sys_init_finished_flag) {
            ts_cpl_fix_msleep(1000);
            break;
        }
        ts_cpl_fix_msleep(500);
    }
    global_sys_ctrl_info.m_sync_latest_frame_id = 0;
    global_sys_ctrl_info.IsUpdate_TempLutCalc_Flag = 1;
    # if defined (VOT_TEMP_YUV_FIRE) || defined(VOT_TEMP_RAW_FIRE)
        for (cRet = 0; cRet < 3; cRet++) {
            votSafe_execute_FireModule();
            ts_cpl_fix_msleep(50);
        }
    #else 
        s32_nPos = TS_UPL_POS_BEFORE_AGC;
        if (votMalloc_FrmAddr(s32_nPos) != TS_OK) {
            return 0;
        }
        for (cRet = 0; cRet < 3; cRet++) {
            votDumpTemplut_SingleFrmData(s32_nPos);
            ts_cpl_fix_msleep(200);
        }
    #endif
    /* 内部测试使用：采集开机快门温度 */
    global_sys_ctrl_info.votShutter_temp_at_powerup = READ_TMP112_Temp();
    for (;;) 
    {
        if ((dl_cnt % RT_INTV) == 0) {
            vot_Set_DistCorr_Fact();    /* 距离修正系数赋值 */
            vot_Set_imgParam_row_data(SystemParaMenu2.Usermenu_SW, Temp_Parameter.Lens_Set);
            READ_Sensor_Temp();         // 测温数据补偿
        }

        if ((dl_cnt % DL_INTV) == 0) 
        {
            /* 任务一: 更新温度矩阵表 */
            if (SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW) 
            {
                vot_update_templut_table(SystemParaMenu2.m_user_tempFrame_Mode, global_sys_ctrl_info.m_cur_bak_ad_mean_u32);
            }
            /* 任务二: 获取UPL数据流及计算 */
            #if defined(VOT_TEMP_YUV_FIRE) || defined(VOT_TEMP_RAW_FIRE)
                votSafe_execute_FireModule();
            #else
                votDumpTemplut_SingleFrmData(s32_nPos);
                votCalcul_memory_TempOrAd(s32_nPos);
            #endif
            /* 任务三：测温OSD工具显示 */
            if (global_sys_ctrl_info.Usermenu_SW_Switching == 0) {
                ReDispSpot1Message();
                ReDispCaptureMessage();
                if (dl_cnt%4 == 0) 
                {
                    ReDispArea1Message();
                }
            }
            /* 任务四：*/
            if (Spot_state == 0 && (Area_state&0x0f) == 0 && UsermenuParameter.CaptureFlag == 0) {
                if (!last_cycleClear_osd) {
                    Clear_Templut_Information();
                    last_cycleClear_osd = 1;
                }
            }
            else if (Spot_state || (Area_state&0x0f) || UsermenuParameter.CaptureFlag){
                last_cycleClear_osd = 0;
            }
        }
        dl_cnt ++;
        if (dl_cnt > 60000) {
            dl_cnt = 0;
        }
        #if defined(VOT_TEMP_YUV_FIRE) || defined(VOT_TEMP_RAW_FIRE)
            ts_cpl_fix_msleep(500);
        #else
            ts_cpl_fix_msleep(100);
        #endif
    }
}

static ts_s32 vot_Temp_SecondCalib_process(ts_void *pv_param)
{
    while(1)
	{
		if(Temp_Parameter.Second_CalibFlag == 1)
		{
			SecondTemp_Autocalibration();
			SecondCalib_Offset_Function();
			
		}
		ts_cpl_msleep(200);
	}
    return 0;
}

ts_s32 vot_wdt_feed_process(ts_void *pv_param)
{

    while (1)
    {
        if(ISP_get_feeding_flag()){
            ts_lib_wdt_feeding();
        }

        ts_cpl_msleep(2000);
    }

    return 0;
}

/**
 * @description: 创建喂狗线程
 * @return {ts_void}
 */
ts_void create_vot_wdt_feed_pthread(void)
{
    ts_cpl_task_s_t st_vot_wdt_feed_task;
    // ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_vot_wdt_feed_task, (ts_cpl_kthread_t)vot_wdt_feed_process, ts_null, "vot_wdt_feed_process", 4, 1024);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_vot_wdt_feed_task, (ts_cpl_kthread_t)vot_wdt_feed_process, ts_null, "vot_wdt_feed_process", 4, 2 * 2048,ts_null);
}

static ts_s32 vot_det_ad_update_process(ts_void *pv_param)
{
    ts_u16 loop_cnt_u16 = 0;
    // ts_u08 force_flag=0;

    while (1)
    {

        ts_cpl_msleep(200);

        loop_cnt_u16++;
        if (loop_cnt_u16 % 15 == 0)
        {
            loop_cnt_u16 = 0;
            vot_snr_update_det_ad();
        }
    }

    return 0;
}

static ts_s32 vot_det_switch_process(ts_void *pv_param)
{
    ts_u16 loop_cnt_u16 = 1;
    ts_u08 force_flag = 0;
    while (1)
    {

        ts_cpl_msleep(200);

        if(global_sys_ctrl_info.m_powerup_inited_flag == 0){
            continue;
        }

        // 矫正的时候不进行 det切换
        if (ISP_get_det_calibrationing_flag() > 0)
        {

            if ((force_flag = ISP_get_det_switch_force_flag()) > 0)
            {
                // 矫正的时候，只有立即切换指令才会生效
                if (force_flag == 2)
                {
                    VotDETParSelect();
                    ISP_set_det_switch_force_flag(0);
                    // 避免频繁检查切换
                    loop_cnt_u16 = 0;
                }
            }
            // vision_log_warn(" det calibrationing ! stop update statics \n");
            continue;
        }

        if (ISP_get_det_switch_force_flag() > 0)
        {
            VotDETParSelect();
            ISP_set_det_switch_force_flag(0);
            // 避免频繁检查切换
            loop_cnt_u16 = 0;
        }
        else
        {
            loop_cnt_u16++;
            if (loop_cnt_u16 % 20 == 0)
            {
                // DetParaAutoSelect();
                // VotDETParSelect();
                ISP_set_det_switch_force_flag(1);
            }
        }
        vot_uaw_get_gos_from_det_temp();//UAW&UAW_Lite根据det温度自动调整gos参数
    }
    return 0;
}

static ts_s32 vot_update_bottom_statistic_process(ts_void *pv_param)
{
    ts_u16 loop_cnt_u16 = 1;
    ts_u08 force_flag = 0;

    while (1)
    {

        ts_cpl_msleep(100);

        // 矫正的时候不进行更新
        if (ISP_get_det_calibrationing_flag() > 0)
        {

            // 矫正的时候，只有隐藏指令生效
            if ((force_flag = ISP_get_bottom_statistic_force_flag()) > 0)
            {
                // vision_log_warn(" force_flag %d \n",force_flag);
                if (force_flag == 1)
                {
                    GetAndDisplayIspStatisInfo(0);
                    ISP_set_bottom_statistic_force_flag(0);
                    // 避免频繁检查切换
                    loop_cnt_u16 = 0;
                }
            }

            continue;
        }

        if (ISP_get_det_switch_force_flag() > 0)
        {
            // vision_log_warn(" ISP_get_det_switch_force_flag ! stop update statics \n");
            ts_cpl_fix_msleep(1000);
            continue;
        }

        // 用于串口指令需要刷新的需求
        if ((force_flag = ISP_get_bottom_statistic_force_flag()) > 0)
        {
            // vision_log_warn("force_flag %d \n",force_flag);
            if (force_flag == 1)
            {
                GetAndDisplayIspStatisInfo(0);
            }
            else if (force_flag == 2)
            {
                GetAndDisplayIspStatisInfo(1);
            }
            else if (force_flag == 3)
            {
                GetAndDisplayIspStatisInfo(2);
            }
            ISP_set_bottom_statistic_force_flag(0);
            // 避免频繁检查切换
            loop_cnt_u16 = 0;
        }
        else
        {
            loop_cnt_u16++;
            if (loop_cnt_u16 % 2 == 0)
            {
                // GetAndDisplayIspStatisInfo(2);
                ISP_set_bottom_statistic_force_flag(3);
                // vision_log_warn("loop_cnt_u16 %d \n",loop_cnt_u16);
            }
        }
    }

    return 0;
}

static ts_s32 vot_update_fire_statis_process(ts_void *pv_param)
{
    
    ts_u16 loop_cnt_u16 = 1;
    ts_u08 force_flag = 0;

    while (1)
    {
        ts_cpl_fix_msleep(100);

        if ((force_flag = ISP_get_update_fire_statis_force_flag()) > 0){
            vot_hst_update_attr_safe();
            
            ISP_set_update_fire_statis_force_flag(0);
            // 避免频繁更新
            loop_cnt_u16 = 0;
        }
        else
        {
            loop_cnt_u16++;
            if (loop_cnt_u16 % 20 == 0)
            {
                ISP_set_update_fire_statis_force_flag(1);
                loop_cnt_u16 = 0;
                // vision_log_warn("loop_cnt_u16 %d \n",loop_cnt_u16);
            }
            
        }
    }

    return 0;
}

static ts_s32 vot_hot_spot_tracking_process(ts_void *pv_param)
{

    ts_isp_fire_statis_s_t fire_statis;
    ts_u16 min_pos_x=0,min_pos_y=0;
    ts_u16 max_pos_x=0,max_pos_y=0;

    while (1)
    {
        ts_cpl_fix_msleep(500);
        
        if(UsermenuParameter.m_hst_mode != HOT_SPOT_TRACKING_MODE_NONE){
            
            vot_get_fire_statis(&fire_statis);
            min_pos_x = fire_statis.st_frm_info.u16_min_pos_x;
            min_pos_y = fire_statis.st_frm_info.u16_min_pos_y;

            max_pos_x = fire_statis.st_frm_info.u16_max_pos_x;
            max_pos_y = fire_statis.st_frm_info.u16_max_pos_y;
            if(UsermenuParameter.m_hst_mode==HOT_SPOT_TRACKING_MODE_MAX){
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,  max_pos_x,  max_pos_y);
            }
            else if(UsermenuParameter.m_hst_mode==HOT_SPOT_TRACKING_MODE_MIN){
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,  min_pos_x,  min_pos_y);
            }
            else if(UsermenuParameter.m_hst_mode==HOT_SPOT_TRACKING_MODE_MAX_MIN){
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_max_cursor_osd_dot,  max_pos_x,  max_pos_y);
                vot_set_hst_cursor_position(global_sys_ctrl_info.m_hst_min_cursor_osd_dot,  min_pos_x,  min_pos_y);
            }
            ISP_set_update_fire_statis_force_flag(2);

        }
        else{
            ts_cpl_fix_msleep(1000);
        }
    
    }
    return 0;
}

static ts_s32 vot_sun_pt_process(ts_void *pv_param)
{

    ts_cpl_timeval_s_t st_time_start, st_time_cur;
    ts_cpl_gettimeofday(&st_time_start);
    ts_u32 target_pixels = 0;
    ts_u32 gray_level_value;

    ISP_set_sun_pt_force_flag(0);

    // 开机时间小于15秒，不运行阳光保护
    ts_cpl_fix_msleep(15000);

    //重新上电后，以用户配置间隔为准
    ISP_set_sun_pt_gap(UsermenuParameter.m_SunPT_gap);

    while (1)
    {

        // 开机打档小于8次，不运行阳光保护
        if (global_sys_ctrl_info.isp_cut_cnt_u32 < 5)
        {
            ts_cpl_fix_msleep(1000);
            continue;
        }

        // 强制关闭挡片时，不运行阳光保护
        if (global_sys_ctrl_info.m_shutter_force_close_flag > 0)
        {
            ts_cpl_fix_msleep(1000);
            continue;
        }

        // 矫正的时候不进行阳光保护
        if (ISP_get_det_calibrationing_flag() > 0)
        {
            // vision_log_warn(" det calibrationing ! stop update statics \n");
            ts_cpl_fix_msleep(1000);
            continue;
        }

        //升级过程中，不进行阳光保护检查
        if ( global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_UPDATE_FIRMWARE)
        {
            ts_cpl_fix_msleep(1000);
            continue;
        }
       
        if (ISP_get_det_switching_flag() > 0)
        {
            // vision_log_warn(" ISP_get_det_switching_flag ! stop update statics \n");
            ts_cpl_fix_msleep(100);
            continue;
        }

        // 坏点矫正不支持阳光保护
        if (global_sys_ctrl_info.m_system_mode == SYSTEM_MODE_BPC)
        {
            ts_cpl_fix_msleep(1000);
            continue;
        }


        if (SystemParaMenu2.SunPT_SW == 1)
        {

            if (ISP_get_sun_pt_force_flag() > 0)
            {
                ts_s32 sun_pt_time_ms = ISP_get_sun_pt_gap() * 1000; 
                ts_cpl_gettimeofday(&st_time_cur);
                ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000;
                if (s32_time_diff > sun_pt_time_ms)
                {
                    vot_sun_pt_notify_peer(0);
                    ISP_set_sun_pt_force_flag(0);
                    if ( UsermenuParameter.m_SunPT_mode == SUN_PT_MODE_AUTO ){
                        vot_sun_pt_shutter_open();
                        ISP_cut_new();
                    }
                    vot_set_sun_pt_notice_info(0);
                    global_sys_ctrl_info.prevent_burn_flag = 0;
                    vision_log_warn("end: sun pt. m_sun_pt_force_flag %d \n ", global_sys_ctrl_info.m_sun_pt_force_flag);
                }
                //避免强制弹开后，立即重新进入下一次阳光保护
                ts_cpl_fix_msleep(1500);
            }
            else
            {

                gray_level_value = SystemParaMenu2.S_Range;
                target_pixels = vot_get_sun_pt_targer_pixels(gray_level_value);

                if (target_pixels > SystemParaMenu2.S_Val)
                {
                    vision_log_warn("target_pixels %d  SystemParaMenu2.S_Val %d gray_level_value %d \n ", target_pixels, SystemParaMenu2.S_Val, gray_level_value);
                    vot_sun_pt_notify_peer(1);
                    ISP_set_sun_pt_force_flag(1);

                    ts_cpl_gettimeofday(&st_time_start);

                    if ( UsermenuParameter.m_SunPT_mode == SUN_PT_MODE_AUTO ){
                        // 关闭挡片
                        vot_sun_pt_shutter_close();
                    }
                    global_sys_ctrl_info.prevent_burn_flag = 1;
                    vision_log_warn("SystemParaMenu2.SunPT_Info_SW  %d \n",SystemParaMenu2.SunPT_Info_SW );
                    if (SystemParaMenu2.SunPT_Info_SW == 1)
                    {
                        vot_set_sun_pt_notice_info(1);
                    }


                    vision_log_warn("start:  m_sun_pt_force_flag %d \n ", global_sys_ctrl_info.m_sun_pt_force_flag);
                }
            }

            ts_cpl_fix_msleep(100);
        }
        else
        {
            ts_cpl_fix_msleep(1500);
        }
    }

    return 0;
}

ts_s32 s32_sffc_run_flag = 1;
// 创建一个开关函数，控制锅盖去圆斑功能的开启和关闭
ts_void create_sffc_run_state(ts_void)
{
    static ts_u08 u08_sffc_run_state = 0;

    if(u08_sffc_run_state == 0) 
    {
       s32_sffc_run_flag = 1;//开启锅盖去圆斑运行标志
    }
    else  if(u08_sffc_run_state == 1) 
    {
       s32_sffc_run_flag = 0;//关闭锅盖去圆斑运行标志
    }
    else if(u08_sffc_run_state == 2) 
    {
        u08_sffc_run_state=0;
        s32_sffc_run_flag = 1;//开启锅盖去圆斑运行标志
    }

    u08_sffc_run_state++;
}



static ts_s32 planck640c_sffc_process(ts_void *pv_param)
{
    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 8192;
    ts_s32 s32_ret = ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
    printf("sffc  init over, ret = %d !\n", s32_ret);


    ts_isp_sffc_blk_info_s_t st_blk = {0};

    st_blk.u08_blk_size = 32;
    printf("u08_blk_size = %d\n", st_blk.u08_blk_size);

    s32_ret = ts_isp_set_sffc_usr_blk_info(TS_ISP_MDEV0, &st_blk);
    if (s32_ret != 0) {
        printf("set sffc blk failed! ret=%d\n", s32_ret);
    }

    ts_s32 s32_sffc_str = 8192;
    while (1)
    {
        if (0 == s32_sffc_run_flag)
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
            s32_sffc_str = s32_sffc_str * 240 + s32_status_cur * 16;
            s32_sffc_str = s32_sffc_str / 256;

            ts_isp_sffc_attr_s_t sffc_attr;
            sffc_attr.u08_enable = 1;
            sffc_attr.u08_en_man = 1;
            sffc_attr.u08_upd_speed = 32;
            sffc_attr.u16_flt_thr = 2560;
            sffc_attr.u16_man_cor_v = s32_sffc_str & 0xffff;
            ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &sffc_attr); 
            //printf("sffc str = %d, ori = %d \n", s32_sffc_str, st_stat.s16_ratio);
        }

        ts_cpl_msleep(100);  
    }
    return 0;
}

/**
 * @description: 创建5.2mm镜头去圆圈圆斑（调节锅盖强度）线程
 * @return {ts_void}
 */
ts_void create_planck640c_sffc_process_pthread(void)
{
    //针对5.2mm镜头圆圈圆斑的修改效果，增加一个持续更新sffc状态的线程，确保在圆斑开启的情况下，能够持续根据状态调整sffc参数，提升圆斑效果的稳定性和适应性
    ts_cpl_task_s_t planck640c_sffc_process_task;
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&planck640c_sffc_process_task, (ts_cpl_kthread_t)planck640c_sffc_process, ts_null, "planck640c_sffc_process", 6, 4 * 2048, ts_null);
}

/**
 * @description: vot业务串口线程初始化
 * @return {*}
 */
ts_void vot_uart_pthread_init(void)
{
    ts_cpl_task_s_t st_uart0_proc_task;
    ts_cpl_task_s_t st_uart1_rcv_task;
    ts_cpl_task_s_t st_uart1_proc_task;
    ring_buffer_init(&(vot_uart0_ctx.m_ring_buf_instance), "vot_ring_buff");
    ring_buffer_init(&(vot_uart1_ctx.m_ring_buf_instance), "vot_ring1_buff");
    vot_uart0_ctx.s32_dev = -1;
    vot_uart1_ctx.s32_dev = -1;
    if(vot_get_force_debug_flag()==0){
        ts_cpl_task_s_t st_uart0_rcv_task;
        ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart0_rcv_task, (ts_cpl_kthread_t)uart0_recv_process, ts_null, "uart0_recv_process", 10, 6 * 2048, ts_null);
    }
   // ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_uart0_proc_task, (ts_cpl_kthread_t)uart0_msg_process, ts_null, "uart0_msg_process", 18, 6 * 2048);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart0_proc_task, (ts_cpl_kthread_t)uart0_msg_process, ts_null, "uart0_msg_process", 18, 10 * 2048, ts_null);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart1_rcv_task, (ts_cpl_kthread_t)uart1_recv_process, ts_null, "uart1_recv_process", 10, 6 * 2048, ts_null);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart1_proc_task, (ts_cpl_kthread_t)uart1_msg_process, ts_null, "uart1_msg_process", 18, 10 * 2048, ts_null);
}


/**
 * @description: vot业务相关线程初始化
 * @return {*}
 */
ts_void vot_routine_pthread_init(void)
{

    ts_cpl_task_s_t st_uart0_proc_task;
    ts_cpl_task_s_t st_uart1_rcv_task;
    ts_cpl_task_s_t st_uart1_proc_task;

    ts_cpl_task_s_t st_det_switch_task;
    ts_cpl_task_s_t st_update_statis_task;
    ts_cpl_task_s_t st_update_det_info_task;
    ts_cpl_task_s_t st_update_ad_temp_task;
    ts_cpl_task_s_t st_update_fire_statis_task;
	ts_cpl_task_s_t st_update_SecondTemp_task;
	
    ring_buffer_init(&(vot_uart0_ctx.m_ring_buf_instance), "vot_ring_buff");

    ring_buffer_init(&(vot_uart1_ctx.m_ring_buf_instance), "vot_ring1_buff");

    vot_uart0_ctx.s32_dev = -1;

    vot_uart1_ctx.s32_dev = -1;

    // 比消息线程的优先级大一点,收到指令后，下一次马上就可以执行
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_det_switch_task, (ts_cpl_kthread_t)vot_update_bottom_statistic_process, ts_null, "vot_update_bottom_statistic_process", 17, 6 * 2048, ts_null);

    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_update_statis_task, (ts_cpl_kthread_t)vot_det_switch_process, ts_null, "vot_det_switch_process", 17, 4 * 2048, ts_null);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_update_det_info_task, (ts_cpl_kthread_t)vot_det_ad_update_process, ts_null, "vot_det_ad_update_process", 17, 4 * 2048, ts_null);

//如果使用1280探测器，暂时不创建测温线程，否则内存过大，无法操作NUC/SFFC;只有使用640探测器，才创建测温线程
//#ifdef CONFIG_SNR_PLANCK640C
    if (SystemParaMenu2.Usermenu_SW) {
        ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_update_ad_temp_task, (ts_cpl_kthread_t)vot_templut_update_process, ts_null, "vot_templut_update_process", 13, 8*2048,ts_null);
        ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_update_SecondTemp_task, (ts_cpl_kthread_t)vot_Temp_SecondCalib_process, ts_null, "vot_Temp_SecondCalib_process", 15, 4*2048,ts_null);
    }  
//#endif

    if(vot_get_force_debug_flag()==0){
        ts_cpl_task_s_t st_uart0_rcv_task;
        ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart0_rcv_task, (ts_cpl_kthread_t)uart0_recv_process, ts_null, "uart0_recv_process", 10, 6 * 2048, ts_null);
    }

   // ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_uart0_proc_task, (ts_cpl_kthread_t)uart0_msg_process, ts_null, "uart0_msg_process", 18, 6 * 2048);
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart0_proc_task, (ts_cpl_kthread_t)uart0_msg_process, ts_null, "uart0_msg_process", 18, 10 * 2048, ts_null);

    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart1_rcv_task, (ts_cpl_kthread_t)uart1_recv_process, ts_null, "uart1_recv_process", 10, 6 * 2048, ts_null);

    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_uart1_proc_task, (ts_cpl_kthread_t)uart1_msg_process, ts_null, "uart1_msg_process", 18, 10 * 2048, ts_null);

    create_sun_pt_pthread();
    //create_gm1215_task_pthread();//创建gm1215线性稳压电源线程
    //create_planck640c_sffc_process_pthread();//创建5.2mm镜头去圆圈圆斑（调节锅盖强度）线程


    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_update_fire_statis_task, (ts_cpl_kthread_t)vot_update_fire_statis_process, ts_null, "update_fire_statis", 18, 6 * 2048, ts_null);

    if (get_customer_type() == CUSTOMER_TYPE_DSW)
	{

        ts_cpl_task_s_t st_hot_spot_tracking_task;

        ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_hot_spot_tracking_task, (ts_cpl_kthread_t)vot_hot_spot_tracking_process, ts_null, "hot_spot_tracking", 18, 6 * 2048, ts_null);

    }
    global_sys_ctrl_info.m_sys_init_finished_flag = 1;

    vision_log_debug(" leave \n");
}

/**
 * @description: 创建阳光保护新线程
 * @return {ts_void}
 */
ts_void create_sun_pt_pthread(void)
{
    ts_cpl_task_s_t st_sun_pt_proc_task;
    // 阳光保护的优先级比消息线程的优先级低一点
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_sun_pt_proc_task, (ts_cpl_kthread_t)vot_sun_pt_process, ts_null, "vot_sun_pt_process", 5, 6 * 2048, ts_null);
}

/**
 * @description: 销毁阳光保护新线程
 * @return {ts_void}
 */
ts_void destory_sun_pt_pthread(void)
{
}

/**
 * @description: 关闭整个cvbs的osd显示，只保留光标层
 * @param {int} is_active，0：关闭显示； 1：开启显示
 * @return {ts_void}
 */
ts_void cvbs_osd_refresh_display(int is_active)
{
    // vision_log_debug("is_active %d m_bottom_info_osd_node %p global_sys_ctrl_info.pst_framebuffer_osd %p \n  ", is_active, global_sys_ctrl_info.m_bottom_info_osd_node, global_sys_ctrl_info.pst_framebuffer_osd);
    if (is_active == 0)
    {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.pst_framebuffer_osd, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_bottom_info_osd_node, 0);
    }
    else
    {

        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.pst_framebuffer_osd, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_bottom_info_osd_node, 1);
    }

    vision_log_debug(" before ts_tdk_osd_refresh_display \n  ");

    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
    vot_refresh_osd_bind();

}

/** @note 测温区域的OSD是否全隐藏 */
ts_void IsHide_templut_osd_display(ts_u08 is_active) {
    vot_osd_node_clear_info(global_sys_ctrl_info.m_templut_info_osd_node);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st);
    vot_osd_node_clear_info(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_maxCursor_dot);
    vot_osd_node_clear_info(global_sys_ctrl_info.m_lutCapture_minCursor_dot);
    if (is_active == 0) {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_templut_info_osd_node, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 0);
    }
    else {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_templut_info_osd_node, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_1st, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_2st, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.mTemplut_OsdCursor_dot_3st, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_maxCursor_dot, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_lutCapture_minCursor_dot, 1);
    }
    ts_tdk_osd_refresh_display( TS_OSD_REFRESH_VISIBLE_ON);
}
/** @note 测温区域的OSD显示控制 
 * {int} is_active，0：关闭；1：开启
*/
ts_void templut_captions_refresh_display(int is_active) {
    if (is_active == 0) {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_templut_info_osd_node, 0);
    }
    else {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_templut_info_osd_node, 1);
    }
    ts_tdk_osd_refresh_display( TS_OSD_REFRESH_VISIBLE_ON);
}
/**
 * @description: cvbs设备的光标层显示控制
 * @param {int} is_active，0：关闭；1：开启
 * @return {ts_void}
 */
ts_void cvbs_cursor_refresh_display(int is_active)
{
    vision_log_debug("is_active %d m_cvbs_cursor_osd_dot %p m_special_info_osd_node %p \n ", is_active, global_sys_ctrl_info.m_cvbs_cursor_osd_dot, global_sys_ctrl_info.m_special_info_osd_node);
    if (is_active == 0)
    {
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, 0);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
    }
    else
    {

        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, 1);
        ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 1);
    }

    ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
}

ts_void cvbs_cursor_refresh_display_auto(void)
{
    static ts_u08 is_active = 1;

    ts_u08 local_is_active = 1 - is_active;
    cvbs_cursor_refresh_display(local_is_active);
    is_active = local_is_active;
}

/**
 * @description: 初始化系统参数
 * @return {ts_void}
 */
ts_void init_sys_ext_config(void)
{
    global_sys_ctrl_info.m_bottom_info_osd_node = NULL;
    global_sys_ctrl_info.m_cvbs_cursor_osd_dot = NULL;
    // global_dev_info_osd_node = NULL;

    memset(&vot_uart0_ctx, 0, sizeof(vot_hbp_ctx_s_t));
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&vot_uart0_ctx.st_sem, 0);

    InitSysCtrlInfo();
    /*半自动校正坏点区域坐标初始值*/
    BadMes.Area_Lx = 303;
    BadMes.Area_Ly = 239;
    BadMes.Area_Rx = 363; // 335-AreaOffset;
    BadMes.Area_Ry = 299; // 271-AreaOffset;
}

/**
 * @description: 反注销系统参数
 * @return {ts_void}
 */
ts_void deinit_sys_ext_config(void)
{
    DeinitSysCtrlInfo();
    ts_cpl_semphr_destory(&vot_uart0_ctx.st_sem);
}

TS_DECLARE_CMD(vot)
{


    if (!strcmp(argv[1], "cut_pod")) {
        ts_u08 mode =  ts_cpl_str2num(argv[2]);
        ts_u08 pod_type =  ts_cpl_str2num(argv[3]);
        vision_log_debug("mode %d pod_type %d \n", mode,pod_type);
        vot_isp_cut_with_pod_flag(1, mode,pod_type);
    }
    
    if (!strcmp(argv[1], "gain_pt")) {
        
         ts_u08 mode =  ts_cpl_str2num(argv[2]);

        if(mode == 0){
            ts_u32 value = planck640c_read_gain_protect_flag();
            vision_log_warn("value 0x%X \n",value);
        }
        else  if(mode == 1){
            ts_u32 u32_flag = 0x13;
            planck640c_write_gain_protect_flag(u32_flag);
        }
        else  if(mode == 2){
            ts_u32 u32_flag = 0x04;
            planck640c_write_gain_protect_flag(u32_flag);
        }
        else  if(mode == 3){
            ts_u32 u32_msb = ts_cpl_str2num(argv[3]);
            ts_u32 u32_lsb = ts_cpl_str2num(argv[4]);
            ts_u32 u32_data = ts_cpl_str2num(argv[5]);
            planck640c_write_extdata( u32_msb, u32_lsb, u32_data);
        }
        else  if(mode == 4){
            ts_u32 u32_msb = ts_cpl_str2num(argv[3]);
            ts_u32 u32_lsb = ts_cpl_str2num(argv[4]);
            ts_u32 value = planck640c_read_extdata(u32_msb, u32_lsb);
            vision_log_warn("value 0x%X \n",value);
        }
    }

    if (!strcmp(argv[1], "hot")) {
        
        vot_get_hot_statistics();
    }

    if (!strcmp(argv[1], "st_size")) {
        
        vision_log_debug(" sizeof(UsermenuParameter) %d \n ",sizeof(UsermenuParameter));
        vision_log_debug(" sizeof(struct Menu2_Para) %d \n ",sizeof(struct Menu2_Para));
        vision_log_debug(" sizeof(ts_isp_cfg_reg_s_t) %d \n ",sizeof(ts_isp_cfg_reg_s_t));
        vision_log_debug(" sizeof(MotorStorageParameters) %d \n ",sizeof(MotorStorageParameters));
        vision_log_debug(" sizeof(struct _Temp_Parameter) %d \n ",sizeof(struct _Temp_Parameter));
   
    }


    if (!strcmp(argv[1], "tlsf")) {
        
        ts_u08 mode =  ts_cpl_str2num(argv[2]);

        if(mode == 3){ //debug
            vot_tlsf_debug(&g_VotMemPoolInfo);
        }

    }

    if (!strcmp(argv[1], "tempad_test")) {
        ts_u16 rAD = 0;
        votLocal_TempAD_TO_RawAD(0xf96, &rAD);
        vision_log_notice("rAD = 0x%x\n", rAD);
    }

    if (!strcmp(argv[1], "osd_bind"))
    {
        if(argc>0){
            vot_refresh_osd_bind();
        }
    }

    if (!strcmp(argv[1], "sunpt_set"))
    {
        if(argc>3){
            SystemParaMenu2.P_Time_U16 = ts_cpl_str2num(argv[2]);

            SystemParaMenu2.S_Range = ts_cpl_str2num(argv[3]);
            SystemParaMenu2.S_Val = ts_cpl_str2num(argv[4]);
        }
    }

    if (!strcmp(argv[1], "iso")){
        if(argc>5){
            ts_u08 switch_flag =  ts_cpl_str2num(argv[2]);
            ts_u08 iso_position_mode  =  ts_cpl_str2num(argv[3]);
            ts_u08 iso_color_mode  =  ts_cpl_str2num(argv[4]);
            ts_u16 u16_iso_h = ts_cpl_str2num(argv[5]);
            ts_u16 u16_iso_l = ts_cpl_str2num(argv[6]);
            vot_set_iso_parameters( switch_flag, iso_position_mode,  iso_color_mode,  u16_iso_h, u16_iso_l );
        }

    }

    if (!strcmp(argv[1], "ad_filter")){
        if(argc>3){
            ts_u08 filter_ad_mode =  ts_cpl_str2num(argv[2]);
            ts_u16 manual_ad_high =  ts_cpl_str2num(argv[3]);
            ts_u16 manual_ad_low =  ts_cpl_str2num(argv[4]);
            vot_set_ad_filter_mode( filter_ad_mode,  manual_ad_high, manual_ad_low );
        }
    }


    if (!strcmp(argv[1], "fire"))
    {
        vot_hst_update_attr_safe();
        ts_isp_fire_statis_s_t fire_statis;
        vot_get_fire_statis(&fire_statis);
        printf(" u16_min_val %u u16_max_val %u,u16_mean %u\n",fire_statis.st_frm_info.u16_min_val,fire_statis.st_frm_info.u16_max_val,fire_statis.st_frm_info.u16_mean);
    }

    if (!strcmp(argv[1], "meta"))
    {
        ts_u08 mode = ts_cpl_str2num(argv[2]);
        ts_u08 user_metadata[648];
        ts_u32 metadata_size = 640;
        memset(user_metadata,mode,640);
        vot_add_user_metadata_info(user_metadata,  metadata_size);
        SystemParaMenu2.m_user_metadata_flag = mode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
    }

    if (!strcmp(argv[1], "bt_timing"))
    {
        ts_u08 mode = ts_cpl_str2num(argv[2]);
        SystemParaMenu2.m_bt_time_sequece_mode = mode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
    }

    if (!strcmp(argv[1], "output"))
    {
        ts_u08 mode = ts_cpl_str2num(argv[2]);
        SystemParaMenu2.m_video_out_mode = mode;
        ISP_write_sys_menu2_cfg(SystemParaMenu2);
    }

     if (!strcmp(argv[1], "sunpt"))
    {

   
        return 0;
    }

    if (!strcmp(argv[1], "uvc"))
    {
        ts_u08 u08_mode = ts_cpl_str2num(argv[2]);
        if (u08_mode)
        {
            // SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_UVC;
            SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_ON;
        }
        else
        {
            // SystemParaMenu2.m_bpc_ref_mode = BPC_MODE_ON_CVBS;
            SystemParaMenu2.m_uvc_enlarger_mode = OUTPUT_MODE_ENLARGE_OFF;
        }

        // vot_usb_uvc_deinit();
        // ts_cpl_msleep(1000);
        // cvbs_pod_set(0);
        // ts_cpl_msleep(1000);
        // vision_log_warn("cvbs_pod_set\n");

        // usb_usr_init(gpv_tmm_usr);
        vot_uvc_usr_reset();
        vision_log_warn("return \n");
    }

    // raw+yuv
    if (!strcmp(argv[1], "raw_yuv"))
    {
        ts_u08 u08_mode = ts_cpl_str2num(argv[2]);
        if (u08_mode >= RAW_YUV_MODE_NONE && u08_mode < RAW_YUV_MODE_BUTT)
        {
            SystemParaMenu2.m_raw_yuv_mode = u08_mode;
            ISP_write_sys_menu2_cfg(SystemParaMenu2);
            vision_log_debug(" SystemParaMenu2.m_raw_yuv_mode  %d  \n", SystemParaMenu2.m_raw_yuv_mode);
        }
    }

    if (!strcmp(argv[1], "raw_pos"))
    {
        ts_u08 u08_pos = ts_cpl_str2num(argv[2]);
        SystemParaMenu2.m_raw_node_pos = vot_check_lut_raw_pos(u08_pos);
        ISP_write_sys_menu2_cfg(SystemParaMenu2);

        vision_log_debug("m_raw_node_pos %d  \n", SystemParaMenu2.m_raw_node_pos);
    }

    // 外参考
    if (!strcmp(argv[1], "cut_ext"))
    {
        ISP_cut_new_exteral();
    }


    if (!strcmp(argv[1], "zoom_out"))
    {
        ts_u16 zoom = ts_cpl_str2num(argv[2]);
        ts_u08 zoom_type = ts_cpl_str2num(argv[3]);

        vot_zoom_out_set( zoom,  zoom_type);

    }

    if (!strcmp(argv[1], "zoom_roi"))
    {
        ts_u16 zoom = ts_cpl_str2num(argv[2]);
        ts_u16 u16_roi_x = ts_cpl_str2num(argv[3]);
        ts_u16 u16_roi_y = ts_cpl_str2num(argv[4]);

  
        ts_u16 u16_roi_width = ts_cpl_str2num(argv[5]);
        ts_u16 u16_roi_height = ts_cpl_str2num(argv[6]);
        vot_roi_zoom_set( zoom,  u16_roi_x,  u16_roi_y,u16_roi_width,u16_roi_height);
    }


    // vot  rz  1  384 288 60 60  213 0 1
    if (!strcmp(argv[1], "rz"))
    {
        if (argc >= 9)
        {

            ts_vpo_grp_attr_s_t st_attr;
            ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
            st_attr.u08_rz_en = ts_cpl_str2num(argv[2]);          //
            st_attr.u16_rz_src_start_h = ts_cpl_str2num(argv[3]); // horizontal src start position
            st_attr.u16_rz_src_start_v = ts_cpl_str2num(argv[4]); // vertical src start position
            st_attr.u16_rz_src_width = ts_cpl_str2num(argv[5]);   // horizontal mirror
            st_attr.u16_rz_src_height = ts_cpl_str2num(argv[6]);  // vertical mirror
            st_attr.u16_rz_dst_start_h = ts_cpl_str2num(argv[7]); // horizontal dst start position
            st_attr.u16_rz_dst_start_v = ts_cpl_str2num(argv[8]); // vertical dst start position
            st_attr.u08_rz_ratio = ts_cpl_str2num(argv[9]);

            // st_attr.u08_rz_en = 1;
            // st_attr.u16_rz_src_start_h = 0;
            // st_attr.u16_rz_src_start_v = 0;
            // st_attr.u16_rz_src_width =60;//horizontal mirror
            // st_attr.u16_rz_src_height = 60;//vertical mirror
            // st_attr.u16_rz_dst_start_h = 320;//horizontal dst start position
            // st_attr.u16_rz_dst_start_v = 256;//vertical dst start position
            // st_attr.u08_rz_ratio = 1;

            vision_log_debug("st_attr.u08_rz_en %d \n", st_attr.u08_rz_en);
            vision_log_debug("st_attr.u16_rz_src_start_h %d \n", st_attr.u16_rz_src_start_h);
            vision_log_debug("st_attr.u16_rz_src_start_v %d \n", st_attr.u16_rz_src_start_v);

            vision_log_debug("st_attr.u16_rz_src_width %d \n", st_attr.u16_rz_src_width);
            vision_log_debug("st_attr.u16_rz_src_height %d \n", st_attr.u16_rz_src_height);
            vision_log_debug("st_attr.u16_rz_dst_start_h %d \n", st_attr.u16_rz_dst_start_h);
            vision_log_debug("st_attr.u16_rz_dst_start_v %d \n", st_attr.u16_rz_dst_start_v);

            vision_log_debug("st_attr.u08_rz_ratio %d \n", st_attr.u08_rz_ratio);

            ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, &st_attr);
            ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
        }
    }

    if (!strcmp(argv[1], "cut_info"))
    {
        int is_visible = ts_cpl_str2num(argv[2]);
        global_sys_ctrl_info.m_show_det_cut_info = is_visible;
    }

    // vot font 1
    if (!strcmp(argv[1], "font"))
    {
        int is_visible = ts_cpl_str2num(argv[2]);
        vot_set_sun_pt_notice_info(is_visible);
    }
    if (!strcmp(argv[1], "pod"))
    {
        // int is_visible =  ts_cpl_str2num(argv[2]);
        // vot_snr_set_grp_pod_switch(is_visible);
    }

    if (!strcmp(argv[1], "cvbs"))
    {
        int is_enlarge = ts_cpl_str2num(argv[2]);
        // vot_snr_set_cvbs_enlarge_switch(is_enlarge);
        cvbs_display_mode_set(is_enlarge);
    }

    if (!strcmp(argv[1], "uvc"))
    {
        ts_u16 u16_disp_width = ts_cpl_str2num(argv[2]);
        ts_u16 u16_disp_height = ts_cpl_str2num(argv[3]);

        vot_snr_set_uvc_output_size(u16_disp_width, u16_disp_height);
    }

    if (!strcmp(argv[1], "temp"))
    {

        ts_u16 ad_value;
        ts_f32 temp_value = 0x0;
        ts_u16 u16_coor_y = ts_cpl_str2num(argv[2]);
        planck384c_get_temperature_test(&ad_value, &temp_value, u16_coor_y);
        vision_log_debug(" u16_coor_y %u ad_value %u temp_value %f \n", u16_coor_y, ad_value, temp_value);
    }

    if (!strcmp(argv[1], "hist_w"))
    {
        ts_s32 level_value = ts_cpl_str2num(argv[2]);
        ts_s32 pixel_num = ts_cpl_str2num(argv[3]);
        vot_whole_hist_test(level_value, pixel_num);
    }

    // if (!strcmp(argv[1], "hist_b"))
    // {
    //     vot_block_hist_test();
    // }

    if (!strcmp(argv[1], "semi_bpc"))
    {

        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]);
        global_sys_ctrl_info.m_BPC_cursor_pos_x = s32_x;
        global_sys_ctrl_info.m_BPC_cursor_pos_y = s32_y;
        BPC_Maual_mode_set_or_unset_bpc_with_auto();
    }

    if (!strcmp(argv[1], "semi_bpc_gaus"))
    {
    }

    if (!strcmp(argv[1], "color_bpc"))
    {

        ts_s32 s32_x = ts_cpl_str2num(argv[2]);
        ts_s32 s32_y = ts_cpl_str2num(argv[3]);
        ts_s32 is_visible = ts_cpl_str2num(argv[4]);
        vot_osd_node_clear_info(global_sys_ctrl_info.m_special_info_osd_node);
        vot_osd_set_bpc_points(global_sys_ctrl_info.m_special_info_osd_node, s32_x, s32_y, 0x3, is_visible);
    }

    if (!strcmp(argv[1], "color_bpc_k"))
    {

        ts_s32 is_visible = ts_cpl_str2num(argv[2]);
        ts_s32 is_clear = ts_cpl_str2num(argv[3]);

        ISP_color_dump_bp_k(global_sys_ctrl_info.m_special_info_osd_node, 0x3, is_visible, is_clear);
    }

    if (!strcmp(argv[1], "bpc_clr"))
    {

        ISP_bpc_off();
    }

    if (!strcmp(argv[1], "osd"))
    {

        ts_s32 is_visible = ts_cpl_str2num(argv[2]);
        if (is_visible)
        {
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, 1);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 1);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.pst_framebuffer_osd, 1);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_bottom_info_osd_node, 1);
        }
        else
        {
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_cvbs_cursor_osd_dot, 0);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.pst_framebuffer_osd, 0);
            ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_bottom_info_osd_node, 0);
        }
        ts_tdk_osd_refresh_display(TS_OSD_REFRESH_VISIBLE_ON);
    }

    if (!strcmp(argv[1], "user_addr"))
    {

        ts_u32 *pu32_addr, u32_len;
        if (ts_tdk_isp_get_reg_addr(TS_ISP_MDEV0, &pu32_addr, &u32_len) != TS_OK)
        {
            vision_log_debug("get isp reg error !\n");
            return 0;
        }

        #ifdef CONFIG_SNR_PLANCK1280C
        {
            vision_log_debug("isp addr: 0x%x  u32_len 0x%x  0x%x \n", (ts_u32)pu32_addr, u32_len, sizeof(ts_isp_cfg_reg_s_t));
            vision_log_debug("m_high_temp_ad_buff addr: 0x%x  u32_len   0x%x  \n", (ts_u32)global_sys_ctrl_info.m_high_temp_ad_buff, 1280 * 1024 * 2);
 
        }
        #else
        {
            vision_log_debug("isp addr: 0x%x  u32_len 0x%x  0x%x \n", (ts_u32)pu32_addr, u32_len, sizeof(ts_isp_cfg_reg_s_t));
            vision_log_debug("m_high_temp_ad_buff addr: 0x%x  u32_len   0x%x  \n", (ts_u32)global_sys_ctrl_info.m_high_temp_ad_buff, 640 * 512 * 2);
        }
        #endif
        vision_log_warn("isp addr: 0x%x  u32_len 0x%x  0x%x \n", (ts_u32)pu32_addr, u32_len, sizeof(ts_isp_cfg_reg_s_t));
        ts_isp_sffc_buf_s_t st_sffc_buf;
        st_sffc_buf.b_working = 1;
        ts_tdk_isp_get_sffc_buf(TS_ISP_MDEV0, (ts_isp_sffc_buf_s_t *)&st_sffc_buf);
        vision_log_warn("test sffc get buf  info size = %d, addr = 0x%x \n", (ts_u32)st_sffc_buf.st_buf.sz_size, (ts_u32)st_sffc_buf.st_buf.pu08_ptr);
        ts_buffer_s_t st_kbuf;
        ts_tdk_isp_get_nuc_kbuf(TS_ISP_MDEV0, &st_kbuf);
        vision_log_warn("test nuc get kbuf  info size = %d, addr = 0x%x \n", (ts_u32)st_kbuf.sz_size, (ts_u32)st_kbuf.pu08_ptr);
        ts_buffer_s_t st_bbuf;
        ts_tdk_isp_get_nuc_bbuf(TS_ISP_MDEV0, &st_bbuf);
        vision_log_warn("test nuc get bbuf  info size = %d, addr = 0x%x \n", (ts_u32)st_bbuf.sz_size, (ts_u32)st_bbuf.pu08_ptr);
        vision_log_warn("gst_snr_ctx.s32_sffc_alg %d \n", gst_snr_ctx.s32_sffc_alg);

        // vision_log_debug("global_sys_ctrl_info addr: 0x%x  u32_len   0x%x \n", &global_sys_ctrl_info,sizeof(SysCtrlInfo_s_t));

        // vision_log_debug("UsermenuParameter addr: %p  u32_len   0x%x \n", &UsermenuParameter,sizeof(struct _Usermenu_Parameter));

        // vision_log_debug("SystemParaMenu2 addr: %p  u32_len   0x%x \n", &SystemParaMenu2,sizeof(struct Menu2_Para));
        // vision_log_debug("g_test_array addr: 0x%x  u32_len   0x%x \n", &g_test_array,sizeof(g_test_array));
    }

    if (!strcmp(argv[1], "afscal"))
    {
        ts_cpl_timeval_s_t st_time0, st_time1;
        ts_isp_af_statis_s_t st_af_statis;
        ts_s32 s32_ret; 

        s32_ret = ts_tdk_isp_get_af_statis(TS_ISP_MDEV0, (ts_isp_af_statis_s_t *)&st_af_statis, 2000);
        if (s32_ret != TS_OK) {
            vision_log_debug("get af res err! \n");
        } else {
            ts_cpl_gettimeofday(&st_time0);   
            vision_log_debug("get af res ok !\n");
            ts_s64 s64_sum_h1 = 0;
            ts_s64 s64_sum_h2 = 0;

            ts_s64 s64_sum_v1 = 0;
            ts_s64 s64_sum_v2 = 0;

            ts_s64 s64_abs_h = 0;
            ts_s64 s64_abs_v = 0;

            ts_s64 s64_sum_aver = 0;
		ts_s32 F_AreaLx = SystemParaMenu2.Focus_Lx/20;
		ts_s32 F_AreaLy = SystemParaMenu2.Focus_Ly/16;
		ts_s32 F_AreaRx = SystemParaMenu2.Focus_Rx/20;
		ts_s32 F_AreaRy = SystemParaMenu2.Focus_Ry/16;	
		for (ts_s32 k = F_AreaLx; k <= F_AreaRx; k++)
		 {
			for(ts_s32 j = F_AreaLy; j <= F_AreaRy; j++){
				ts_s32 i = j*st_af_statis.u16_blk_num_h + k;
				s64_sum_h1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1);
				// s64_sum_h1 += (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1) * (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1);
				s64_sum_h2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2);

				s64_sum_v1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1);
				s64_sum_v2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2);

				s64_abs_v += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2));
				s64_abs_h += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2));
				s64_sum_aver += (ts_s32)st_af_statis.pst_af_statis_info[i].u16_aver;
			}
        }
            ts_cpl_gettimeofday(&st_time1);
            vision_log_debug("afs  %d us : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec));
            // vision_log_debug("af info num = [%d, %d], addr = 0x%x, af mean = h:[%d, %d, %d], v:[%d, %d: %d], aver:[%d]\n", st_af_statis.u16_blk_num_h, st_af_statis.u16_blk_num_w,
            //                                         (ts_u32)st_af_statis.pst_af_statis_info, 
            //                                         (ts_s32)(s64_sum_h1 / 1024), 
            //                                         (ts_s32)(s64_sum_h2 / 1024), 
            //                                         (ts_s32)(s64_abs_h / 1024), 
            //                                         (ts_s32)(s64_sum_v1 / 1024), 
            //                                         (ts_s32)(s64_sum_v2 / 1024), 
            //                                         (ts_s32)(s64_abs_v / 1024), 
            //                                         (ts_s32)(s64_sum_aver /1024));

            vision_log_debug("blk_num_h =[%d],F_AreaLxy =[%d][%d],F_AreaRxy =[%d][%d]\n", st_af_statis.u16_blk_num_h, F_AreaLx,F_AreaLy,F_AreaRx,F_AreaRy);
            // vision_log_debug("Focus_Value/512 = [%d]\n]", (ts_s32)((s64_abs_v+s64_abs_h)/512));
            // vision_log_debug("Focus_Value_L = [%lld]\n]", (s64_abs_v+s64_abs_h));
        }
        ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, &st_af_statis);
    }

   if(!strcmp(argv[1], "debug"))
    {
        ts_s32 debug =0;
        if(argc>2)
        {
            debug =ts_cpl_str2num(argv[2]);
        }
         if(debug)
         {
            global_vot_misc_cfg.m_force_debug_confirm = VOT_DEBUG_OUTPUT_CONFIRM_MAGIC;
         }
         else
         {
            global_vot_misc_cfg.m_force_debug_confirm = 0;  
         }
        // 保存配置
        ISP_write_vot_misc_cfg(global_vot_misc_cfg);
        vision_log_warn("m_force_debug_confirm [%x] \n",global_vot_misc_cfg.m_force_debug_confirm);
    }

    if(!strcmp(argv[1], "npu"))
    {
        ts_s32 npu_sr_en =1;//开启NPU SR超分功能
        if(argc>2)
        {
            npu_sr_en =ts_cpl_str2num(argv[2]);
        }
        if(npu_sr_en)
        {
            SystemParaMenu2.m_ts_npu_ops_sr_en = npu_sr_en;// 开启NPU SR超分功能
        }
        else
        {
            SystemParaMenu2.m_ts_npu_ops_sr_en = NPU_SR_OFF;// 默认关闭NPU SR超分功能
        }
        // 保存配置
        ISP_write_sys_menu2_cfg(SystemParaMenu2);//保存当前已同步更新的系统菜单参数，以便机芯重启，更改的参数生效
        vision_log_warn("m_ts_npu_ops_sr_en [%x] \n",SystemParaMenu2.m_ts_npu_ops_sr_en);
    }

    return 0;
}

// 初始化osd
// temp_osd_init(shdl_osd_dev, 640, 512);
ts_nodebuf_s_t *temp_osd_init(ts_u32 u32_width, ts_u32 u32_height)
{
    ts_osd_attr_s_t st_attr;
    ts_nodebuf_s_t *pst_lbl;

    st_attr.u16_left = 0;
    st_attr.u16_top = 0;
    st_attr.u16_height = u32_height;
    st_attr.u16_width = u32_width;
    st_attr.u08_pix_bw = TS_PIXEL_BW_2BIT;
    st_attr.u08_visible = 1;
    st_attr.u08_priority = 80;
    st_attr.u08_type = TS_OSD_TYPE_OSD;
    st_attr.st_osd_cfg.s08_alpha = -1;
    st_attr.st_osd_cfg.u08_auto_invert = 0;

    /* 前景色与背景色配置 */
    st_attr.st_osd_cfg.u08_font_color = OSD_COLOR_WHITE;
    st_attr.st_osd_cfg.u08_bg_color = OSD_COLOR_BLACK;
    pst_lbl = (ts_nodebuf_s_t *)ts_tdk_osd_create_node(shdl_osd_dev, (const ts_osd_attr_s_t *)&st_attr);
    if (ts_null == pst_lbl)
        return ts_null;

    {
        ts_u08 *pu08_ptr;
        ts_u32 i, j, u32_width;
        /* 填充成 2bit 色条 */
        // k = 0;

        // 每个像素2bit 为 b 01,所以为1，sast_mnu_rgbs对应 /* 黑色， 透明度 0, 即完全不透明 */
        /* 每个像素占用 2bit */
        u32_width = st_attr.u16_width >> 2;
        for (i = 0; i < st_attr.u16_height; i++)
        {
            pu08_ptr = (ts_u08 *)pst_lbl->u32_virt_addr;
            pu08_ptr += i * u32_width;

            for (j = 0; j < u32_width; j++)
            {
                pu08_ptr[j] = 0x55;
            }
        }
    }

    return pst_lbl;
}

static ts_s32 ts_lib_draw_crosshair(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 center_x,
    ts_u16 center_y,
    ts_u08 line_thickness,
    ts_u08 color,
    ts_u08 length)
{
   if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        if (pst_osd == NULL || center_x >= 1280 || center_y >= 1024)
        {
            return -TS_EINVAL;
        }
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        int length_add = 0;
        int length_sub = 0;
        if ((line_thickness % 2) == 0)
        {
            length_add = length + 1;
            length_sub = length;
        }
        else
        {
            length_sub = length;
            length_add = length;
        }
        ts_u16 h_x1 = center_x - length_sub;
        ts_u16 h_x2 = center_x + length_add;
        if (h_x1 < 0)
            h_x1 = 0;
        if (h_x2 >= 1280)
            h_x2 = 1279;
        ts_u16 v_y1 = center_y - length_sub;
        ts_u16 v_y2 = center_y + length_add;
        if (v_y1 < 0)
            v_y1 = 0;
        if (v_y2 >= 1024)
            v_y2 = 1023;

        ts_u08 half_thickness = (line_thickness - 1) / 2;
        ts_u16 h_start_y = (center_y >= half_thickness) ? (center_y - half_thickness) : 0;
        ts_u16 h_end_y = center_y + (line_thickness - 1 - half_thickness);
        if (h_end_y >= 1024)
            h_end_y = 1023;

        for (ts_u16 y = h_start_y; y <= h_end_y; y++)
        {
            for (ts_u16 x = h_x1; x <= h_x2; x++)
            {
                ts_lib_set_pixel(pst_osd, x, y, color);
            }
        }

        ts_u16 v_start_x = (center_x >= half_thickness) ? (center_x - half_thickness) : 0;
        ts_u16 v_end_x = center_x + (line_thickness - 1 - half_thickness);
        if (v_end_x >= 1280)
            v_end_x = 1279;
        for (ts_u16 x = v_start_x; x <= v_end_x; x++)
        {
            for (ts_u16 y = v_y1; y <= v_y2; y++)
            {
                ts_lib_set_pixel(pst_osd, x, y, color);
            }
        }
    }
    else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
    {
        if (pst_osd == NULL || center_x >= 640 || center_y >= 512)
        {
            return -TS_EINVAL;
        }
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        int length_add = 0;
        int length_sub = 0;
        if ((line_thickness % 2) == 0)
        {
            length_add = length + 1;
            length_sub = length;
        }
        else
        {
            length_sub = length;
            length_add = length;
        }
        ts_u16 h_x1 = center_x - length_sub;
        ts_u16 h_x2 = center_x + length_add;
        if (h_x1 < 0)
            h_x1 = 0;
        if (h_x2 >= 640)
            h_x2 = 639;
        ts_u16 v_y1 = center_y - length_sub;
        ts_u16 v_y2 = center_y + length_add;
        if (v_y1 < 0)
            v_y1 = 0;
        if (v_y2 >= 512)
            v_y2 = 511;

        ts_u08 half_thickness = (line_thickness - 1) / 2;
        ts_u16 h_start_y = (center_y >= half_thickness) ? (center_y - half_thickness) : 0;
        ts_u16 h_end_y = center_y + (line_thickness - 1 - half_thickness);
        if (h_end_y >= 512)
            h_end_y = 511;

        for (ts_u16 y = h_start_y; y <= h_end_y; y++)
        {
            for (ts_u16 x = h_x1; x <= h_x2; x++)
            {
                ts_lib_set_pixel(pst_osd, x, y, color);
            }
        }

        ts_u16 v_start_x = (center_x >= half_thickness) ? (center_x - half_thickness) : 0;
        ts_u16 v_end_x = center_x + (line_thickness - 1 - half_thickness);
        if (v_end_x >= 640)
            v_end_x = 639;
        for (ts_u16 x = v_start_x; x <= v_end_x; x++)
        {
            for (ts_u16 y = v_y1; y <= v_y2; y++)
            {
                ts_lib_set_pixel(pst_osd, x, y, color);
            }
        }
     }  
    return 0;
}

static ts_s32 ts_lib_draw_line(ts_nodebuf_s_t *pst_osd, ts_u16 x1, ts_u16 y1, ts_u16 x2, ts_u16 y2, ts_u08 line_thickness, ts_u08 color)
{
    if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        // 参数合法性检查
        if (pst_osd == NULL || x1 >= 1280 || y1 >= 1024 || x2 >= 1280 || y2 >= 1024)
        {
            return -TS_EINVAL;
        }
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }

        // Bresenham线段算法核心逻辑
        int dx = x2 - x1;
        int dy = y2 - y1;
        int adx = (dx > 0) ? dx : -dx; // 绝对值计算
        int ady = (dy > 0) ? dy : -dy;
        int step_x = (dx > 0) ? 1 : (dx < 0) ? -1
                                            : 0; // 步进方向
        int step_y = (dy > 0) ? 1 : (dy < 0) ? -1
                                            : 0;
        int err = adx - ady;
        int x = x1, y = y1;

        // 线宽扩展偏移量（确保线宽居中）
        int half_thickness = (line_thickness - 1) / 2;

        // 遍历线段上的每个主像素点
        for (int i = 0; i <= (adx > ady ? adx : ady); i++)
        {
            // 绘制线宽范围内的像素（垂直方向扩展）
            for (int k = 0; k < line_thickness; k++)
            {
                int current_y = y - half_thickness + k;
                if (current_y >= 0 && current_y < 1024)
                { // 防止Y坐标越界
                    ts_lib_set_pixel(pst_osd, x, current_y, color);
                }
            }

            // 更新误差项和坐标（Bresenham核心步骤）
            int e2 = 2 * err;
            if (e2 > -ady)
            {
                err -= ady;
                x += step_x;
            }
            if (e2 < adx)
            {
                err += adx;
                y += step_y;
            }
        }
    }
    else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
    {
        // 参数合法性检查
        if (pst_osd == NULL || x1 >= 640 || y1 >= 512 || x2 >= 640 || y2 >= 512)
        {
            return -TS_EINVAL;
        }
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }

        // Bresenham线段算法核心逻辑
        int dx = x2 - x1;
        int dy = y2 - y1;
        int adx = (dx > 0) ? dx : -dx; // 绝对值计算
        int ady = (dy > 0) ? dy : -dy;
        int step_x = (dx > 0) ? 1 : (dx < 0) ? -1
                                            : 0; // 步进方向
        int step_y = (dy > 0) ? 1 : (dy < 0) ? -1
                                            : 0;
        int err = adx - ady;
        int x = x1, y = y1;

        // 线宽扩展偏移量（确保线宽居中）
        int half_thickness = (line_thickness - 1) / 2;

        // 遍历线段上的每个主像素点
        for (int i = 0; i <= (adx > ady ? adx : ady); i++)
        {
            // 绘制线宽范围内的像素（垂直方向扩展）
            for (int k = 0; k < line_thickness; k++)
            {
                int current_y = y - half_thickness + k;
                if (current_y >= 0 && current_y < 512)
                { // 防止Y坐标越界
                    ts_lib_set_pixel(pst_osd, x, current_y, color);
                }
            }

            // 更新误差项和坐标（Bresenham核心步骤）
            int e2 = 2 * err;
            if (e2 > -ady)
            {
                err -= ady;
                x += step_x;
            }
            if (e2 < adx)
            {
                err += adx;
                y += step_y;
            }
        }
    }
    return 0;
}

static CharIndexResult get_char_index_ex(const char *str)
{
    CharIndexResult result = {0, INDEX_TYPE_NONE, -1}; // 默认未找到
    if (str == NULL || *str == '\0')
    {
        return result; // 输入无效，直接返回未找到
    }

    unsigned char first_byte = (unsigned char)*str;

    // -------------------------- 处理ASCII字符（单字节） -------------------------
    if (first_byte < 0x80)
    {
        for (int i = 0;; i++)
        {
           // ts_u08 current_char = sau08_asiic[i];
            ts_u08 current_char;
           if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
            {
                current_char = sau08_asiic32[i];
            }
            else
            {
                current_char = sau08_asiic16[i];
            }
            if (current_char == 0)
                break; // 数组结束，未找到
            if (current_char == first_byte)
            { // 找到ASCII字符
                result.found = 1;
                result.type = INDEX_TYPE_ASCII;
                result.index = i;
                return result;
            }
        }
    }

    // -------------------------- 处理UTF-8字符（多字节） -------------------------
    else
    {
        ts_u32 utf8_code = 0;
        int byte_count = 0;

        // 解析UTF-8字节序列为32位拼接值（与文档sau32_utf8格式匹配）
        if ((first_byte & 0xE0) == 0xC0)
        { // 2字节UTF-8
            byte_count = 2;
            utf8_code = ((ts_u32)first_byte << 8) | (unsigned char)*(str + 1);
        }
        else if ((first_byte & 0xF0) == 0xE0)
        { // 3字节UTF-8（如中文）
            byte_count = 3;
            utf8_code = ((ts_u32)first_byte << 16) |
                        ((ts_u32)(unsigned char)*(str + 1) << 8) |
                        (unsigned char)*(str + 2);
        }
        else
        {
            return result; // 不支持的UTF-8格式
        }

        // 检查输入字符串长度是否足够
        if (strlen(str) < (size_t)byte_count)
        {
            return result; // 输入不完整，未找到
        }

        // 在sau32_utf8数组中查找字节拼接值
        for (int i = 0;; i++)
        {
            //ts_u32 current_code = sau32_utf8[i];
             ts_u32 current_code;
            if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
            {
                current_code = sau32_utf8_32[i];
            }
            else
            {
                current_code = sau32_utf8_16[i];
            }
            if (current_code == 0)
                break; // 数组结束，未找到
            if (current_code == utf8_code)
            { // 找到UTF-8字符
                result.found = 1;
                result.type = INDEX_TYPE_UTF8;
                result.index = i;
                return result;
            }
        }
    }

    return result; // 未找到字符
}

static ts_s32 ts_lib_draw_string(ts_nodebuf_s_t *pst_osd, ts_u16 x, ts_u16 y, ts_u08 color, const char *str, ts_u08 spacing)
{
    int char_count = 0;
    if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        if (pst_osd == NULL || str == NULL || x >= 1280 || y >= 1024 || color > 0x03 || spacing < 1 || spacing > 5)
        {
            return -TS_EINVAL;
        }
        ts_u16 current_x = x;
        ts_u16 current_y = y;
       
        for (int i = 0; str[i] != '\0';)
        {
            CharIndexResult res = get_char_index_ex(&str[i]);
            if (!res.found)
            {
                i++;
                continue;
            }
            const ts_u08 *font_data = NULL;
            ts_u08 width = (res.type == INDEX_TYPE_ASCII) ? 16 : 32;
            ts_u08 height = 32;
            ts_u16 data_len = (res.type == INDEX_TYPE_ASCII) ? 64 : 128;
            if (res.type == INDEX_TYPE_ASCII)
            {
                font_data = au08_font_asiic32 + res.index * data_len;
            }
            else
            {
                font_data = au08_font_song32 + res.index * data_len;
            }
            if (current_x + width >= 1280 || current_y + height >= 1024)
            {
                return char_count;
            }
            for (ts_u08 row = 0; row < height; row++)
            {
                for (ts_u08 col = 0; col < width; col++)
                {
                    ts_u08 pixel = 0;
                    if (res.type == INDEX_TYPE_ASCII)
                    {
                        ts_u08 byte = font_data[row];
                        pixel = (byte >> col) & 0x01;
                    }
                    else
                    {
                        ts_u16 row_data = (font_data[row * 2 + 1] << 8) | font_data[row * 2];
                        pixel = (row_data >> col) & 0x01;
                    }
                    if (pixel)
                    {
                        ts_s32 ret = ts_lib_set_pixel(pst_osd, current_x + col, current_y + row, color);
                        if (ret != 0)
                            return char_count;
                    }
                }
            }
            current_x += width + spacing;
            char_count++;
            if (res.type == INDEX_TYPE_ASCII)
            {
                i++;
            }
            else
            {
                unsigned char first_byte = (unsigned char)str[i];
                if ((first_byte & 0xE0) == 0xC0)
                    i += 2;
                else if ((first_byte & 0xF0) == 0xE0)
                    i += 3;
                else
                    i++;
            }
        }
    }
    else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
    {
       if (pst_osd == NULL || str == NULL || x >= 640 || y >= 512 || color > 0x03 || spacing < 1 || spacing > 5)
        {
            return -TS_EINVAL;
        }
        ts_u16 current_x = x;
        ts_u16 current_y = y;
       
        for (int i = 0; str[i] != '\0';)
        {
            CharIndexResult res = get_char_index_ex(&str[i]);
            if (!res.found)
            {
                i++;
                continue;
            }
            const ts_u08 *font_data = NULL;
            ts_u08 width = (res.type == INDEX_TYPE_ASCII) ? 8 : 16;
            ts_u08 height = 16;
            ts_u16 data_len = (res.type == INDEX_TYPE_ASCII) ? 16 : 32;
            if (res.type == INDEX_TYPE_ASCII)
            {
                font_data = au08_font_asiic16 + res.index * data_len;
            }
            else
            {
                font_data = au08_font_song16 + res.index * data_len;
            }
            if (current_x + width >= 640 || current_y + height >= 512)
            {
                return char_count;
            }
            for (ts_u08 row = 0; row < height; row++)
            {
                for (ts_u08 col = 0; col < width; col++)
                {
                    ts_u08 pixel = 0;
                    if (res.type == INDEX_TYPE_ASCII)
                    {
                        ts_u08 byte = font_data[row];
                        pixel = (byte >> col) & 0x01;
                    }
                    else
                    {
                        ts_u16 row_data = (font_data[row * 2 + 1] << 8) | font_data[row * 2];
                        pixel = (row_data >> col) & 0x01;
                    }
                    if (pixel)
                    {
                        ts_s32 ret = ts_lib_set_pixel(pst_osd, current_x + col, current_y + row, color);
                        if (ret != 0)
                            return char_count;
                    }
                }
            }
            current_x += width + spacing;
            char_count++;
            if (res.type == INDEX_TYPE_ASCII)
            {
                i++;
            }
            else
            {
                unsigned char first_byte = (unsigned char)str[i];
                if ((first_byte & 0xE0) == 0xC0)
                    i += 2;
                else if ((first_byte & 0xF0) == 0xE0)
                    i += 3;
                else
                    i++;
            }
        }
     }
    return char_count;
}

// 绘制点测温光标+文字
ts_s32 ts_lib_draw_crosshair_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 center_x,
    ts_u16 center_y,
    ts_u08 line_thickness,
    ts_u08 color,
    ts_u08 length,
    int point_temp,
    ts_u08 char_spacing)
{
   if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        if (pst_osd == NULL)
            return -TS_EINVAL;
        if (center_x >= 1280 || center_y >= 1024)
            return -TS_EINVAL;
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
            return -TS_EINVAL;
        if (char_spacing < 1 || char_spacing > 5)
            return -TS_EINVAL;

        ts_s32 ret = ts_lib_draw_crosshair(pst_osd, center_x, center_y, line_thickness, color, length);
        if (ret != 0)
            return ret;
        ts_u16 total_width = 0;
        int char_count = 0;
        char str_label[16] = "\0";
        ts_cpl_snprintf(str_label, 16, "%.1f℃", point_temp / 10.f);
        for (int i = 0; str_label[i] != '\0';)
        {
            CharIndexResult res = get_char_index_ex(&str_label[i]);
            if (!res.found)
            {
                i++;
                continue;
            }
            ts_u08 char_w = (res.type == INDEX_TYPE_ASCII) ? 16 : 32;
            if (char_count > 0)
                total_width += char_spacing;
            total_width += char_w;
            char_count++;
            if (res.type == INDEX_TYPE_ASCII)
                i++;
            else
            {
                unsigned char first_byte = (unsigned char)str_label[i];
                if ((first_byte & 0xE0) == 0xC0)
                    i += 2;
                else if ((first_byte & 0xF0) == 0xE0)
                    i += 3;
                else
                    i++;
            }
        }
        if (char_count == 0)
            return 0;
        ts_u08 len_add = length, len_sub = length;
        if (line_thickness % 2 == 0)
        {
            len_add = length + 1;
            len_sub = length;
        }
        ts_u16 h_x2 = center_x + len_add;
        if (h_x2 >= 1280)
            h_x2 = 1279;
        ts_u16 v_y2 = center_y + len_add;
        if (v_y2 >= 1024)
            v_y2 = 1023;
        ts_u16 h_x1 = center_x - len_sub;
        if (h_x1 < 0)
            h_x1 = 0;
        ts_u16 v_y1 = center_y - len_sub;
        if (v_y1 < 0)
            v_y1 = 0;

        const ts_u08 label_h = 32;
        ts_u16 start_x = 0, start_y = 0;
        bool pos_ok = false;

        start_x = h_x2 + 1;
        start_y = center_y - label_h / 2;
        if (start_y >= 0 && (start_y + label_h) <= 1024 && (start_x + total_width) <= 1280)
        {
            pos_ok = true;
        }

        else
        {
            start_y = v_y2 + 1;
            start_x = center_x - total_width / 2;
            if (start_x >= 0 && (start_x + total_width) <= 1280 && (start_y + label_h) <= 1024)
            {
                pos_ok = true;
            }
            else
            {
                start_x = h_x1 - total_width;
                start_y = center_y - label_h / 2;
                if (start_x >= 0 && start_y >= 0 && (start_y + label_h) <= 1024)
                {
                    pos_ok = true;
                }
                else
                {
                    start_y = v_y1 - label_h;
                    start_x = center_x - total_width / 2;
                    if (start_y >= 0 && start_x >= 0 && (start_x + total_width) <= 1280)
                    {
                        pos_ok = true;
                    }
                }
            }
        }
        if (pos_ok)
        {
            ts_lib_draw_string(pst_osd, start_x, start_y, color, str_label, char_spacing);
        }
    }
    else //if (get_dector_resolution_type()==DECTOR_RESOLUTION_640X512)
    {
        if (pst_osd == NULL)
        return -TS_EINVAL;
        if (center_x >= 640 || center_y >= 512)
            return -TS_EINVAL;
        if (line_thickness < 1 || line_thickness > 10 || color > 0x03)
            return -TS_EINVAL;
        if (char_spacing < 1 || char_spacing > 5)
            return -TS_EINVAL;

        ts_s32 ret = ts_lib_draw_crosshair(pst_osd, center_x, center_y, line_thickness, color, length);
        if (ret != 0)
            return ret;
        ts_u16 total_width = 0;
        int char_count = 0;
        char str_label[16] = "\0";
        ts_cpl_snprintf(str_label, 16, "%.1f℃", point_temp / 10.f);
        for (int i = 0; str_label[i] != '\0';)
        {
            CharIndexResult res = get_char_index_ex(&str_label[i]);
            if (!res.found)
            {
                i++;
                continue;
            }
            ts_u08 char_w = (res.type == INDEX_TYPE_ASCII) ? 8 : 16;
            if (char_count > 0)
                total_width += char_spacing;
            total_width += char_w;
            char_count++;
            if (res.type == INDEX_TYPE_ASCII)
                i++;
            else
            {
                unsigned char first_byte = (unsigned char)str_label[i];
                if ((first_byte & 0xE0) == 0xC0)
                    i += 2;
                else if ((first_byte & 0xF0) == 0xE0)
                    i += 3;
                else
                    i++;
            }
        }
        if (char_count == 0)
            return 0;
        ts_u08 len_add = length, len_sub = length;
        if (line_thickness % 2 == 0)
        {
            len_add = length + 1;
            len_sub = length;
        }
        ts_u16 h_x2 = center_x + len_add;
        if (h_x2 >= 640)
            h_x2 = 639;
        ts_u16 v_y2 = center_y + len_add;
        if (v_y2 >= 512)
            v_y2 = 511;
        ts_u16 h_x1 = center_x - len_sub;
        if (h_x1 < 0)
            h_x1 = 0;
        ts_u16 v_y1 = center_y - len_sub;
        if (v_y1 < 0)
            v_y1 = 0;

        const ts_u08 label_h = 16;
        ts_u16 start_x = 0, start_y = 0;
        bool pos_ok = false;

        start_x = h_x2 + 1;
        start_y = center_y - label_h / 2;
        if (start_y >= 0 && (start_y + label_h) <= 512 && (start_x + total_width) <= 640)
        {
            pos_ok = true;
        }

        else
        {
            start_y = v_y2 + 1;
            start_x = center_x - total_width / 2;
            if (start_x >= 0 && (start_x + total_width) <= 640 && (start_y + label_h) <= 512)
            {
                pos_ok = true;
            }
            else
            {
                start_x = h_x1 - total_width;
                start_y = center_y - label_h / 2;
                if (start_x >= 0 && start_y >= 0 && (start_y + label_h) <= 512)
                {
                    pos_ok = true;
                }
                else
                {
                    start_y = v_y1 - label_h;
                    start_x = center_x - total_width / 2;
                    if (start_y >= 0 && start_x >= 0 && (start_x + total_width) <= 640)
                    {
                        pos_ok = true;
                    }
                }
            }
        }
        if (pos_ok)
        {
            ts_lib_draw_string(pst_osd, start_x, start_y, color, str_label, char_spacing);
        }

    }
    return 0;
}
ts_s32 ts_lib_draw_line_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 x1, ts_u16 y1,
    ts_u16 x2, ts_u16 y2,
    int max_temp,
    int avg_temp,
    int min_temp,
    ts_u08 line_thickness,
    ts_u08 color)
{
    if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        if (pst_osd == NULL ||
            x1 >= 1280 || y1 >= 1024 || x2 >= 1280 || y2 >= 1024 ||
            line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        ts_s32 ret = ts_lib_draw_line(pst_osd, x1, y1, x2, y2, line_thickness, color);
        if (ret != 0)
            return ret;
        ts_u16 line_min_x = (x1 < x2) ? x1 : x2;
        ts_u16 line_max_x = (x1 > x2) ? x1 : x2;
        ts_u16 line_min_y = (y1 < y2) ? y1 : y2;
        ts_u16 line_max_y = (y1 > y2) ? y1 : y2;

        int half_thickness = (line_thickness - 1) / 2;
        line_min_x -= half_thickness;
        line_max_x += half_thickness;
        line_min_y -= half_thickness;
        line_max_y += half_thickness;
        line_min_x = (line_min_x < 0) ? 0 : line_min_x;
        line_max_x = (line_max_x >= 1280) ? 1279 : line_max_x;
        line_min_y = (line_min_y < 0) ? 0 : line_min_y;
        line_max_y = (line_max_y >= 1024) ? 1023 : line_max_y;

    // #define TEXT_SPACING 2
    // #define LINE_HEIGHT 32
    // #define TOTAL_LINES 3                                 // 三行文字
    // #define TOTAL_TEXT_HEIGHT (LINE_HEIGHT * TOTAL_LINES) // 总高度：96px
    // #define MAX_TEXT_WIDTH 180

        ts_u16 text_x = 0, text_y = 0;
        bool found_valid_pos = false;
        text_y = line_max_y + 1;
        text_x = (line_min_x + line_max_x) / 2 - (180 / 2);
        if (text_y + 96 <= 1024 && text_x >= 0 && text_x + 180 <= 1280)
        {
            found_valid_pos = true;
        }
        else
        {
            text_y = line_min_y - 96;
            if (text_y >= 0 && text_x >= 0 && text_x + 180 <= 1280)
            {
                found_valid_pos = true;
            }
            else
            {
                text_x = line_max_x + 1;
                text_y = (line_min_y + line_max_y) / 2 - (96 / 2);
                if (text_x + 180 <= 1280 && text_y >= 0 && text_y + 96 <= 1024)
                {
                    found_valid_pos = true;
                }
                else
                {
                    text_x = line_min_x - 180;
                    if (text_x >= 0 && text_y >= 0 && text_y + 96 <= 1024)
                    {
                        found_valid_pos = true;
                    }
                }
            }
        }
        if (found_valid_pos)
        {
            ts_char max_str[20] = "\0", avg_str[20] = "\0", low_str[20] = "\0";
            ts_f32 f_max = max_temp / 10.0f;
            ts_f32 f_avg = avg_temp / 10.0f;
            ts_f32 f_min = min_temp / 10.0f;
            ts_cpl_snprintf(max_str, 20, "max:%.1f℃", f_max);
            ts_cpl_snprintf(avg_str, 20, "avg:%.1f℃", f_avg);
            ts_cpl_snprintf(low_str, 20, "min:%.1f℃", f_min);
            // ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, TEXT_SPACING);
            // ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT, color, avg_str, TEXT_SPACING);
            // ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT * 2, color, low_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, 2);
            ts_lib_draw_string(pst_osd, text_x, text_y + 32, color, avg_str, 2);
            ts_lib_draw_string(pst_osd, text_x, text_y + 32 * 2, color, low_str, 2);
        }
        else
        {
            return -TS_EINVAL;
        }
    }
    else //if (get_dector_resolution_type() == DECTOR_RESOLUTION_640X512)
    {
             if (pst_osd == NULL ||
            x1 >= 640 || y1 >= 512 || x2 >= 640 || y2 >= 512 ||
            line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        ts_s32 ret = ts_lib_draw_line(pst_osd, x1, y1, x2, y2, line_thickness, color);
        if (ret != 0)
            return ret;
        ts_u16 line_min_x = (x1 < x2) ? x1 : x2;
        ts_u16 line_max_x = (x1 > x2) ? x1 : x2;
        ts_u16 line_min_y = (y1 < y2) ? y1 : y2;
        ts_u16 line_max_y = (y1 > y2) ? y1 : y2;

        int half_thickness = (line_thickness - 1) / 2;
        line_min_x -= half_thickness;
        line_max_x += half_thickness;
        line_min_y -= half_thickness;
        line_max_y += half_thickness;
        line_min_x = (line_min_x < 0) ? 0 : line_min_x;
        line_max_x = (line_max_x >= 640) ? 639 : line_max_x;
        line_min_y = (line_min_y < 0) ? 0 : line_min_y;
        line_max_y = (line_max_y >= 512) ? 511 : line_max_y;

    #define TEXT_SPACING 2
    #define LINE_HEIGHT 16
    #define TOTAL_LINES 3                                 // 三行文字
    #define TOTAL_TEXT_HEIGHT (LINE_HEIGHT * TOTAL_LINES) // 总高度：48px
    #define MAX_TEXT_WIDTH 90

        ts_u16 text_x = 0, text_y = 0;
        bool found_valid_pos = false;
        text_y = line_max_y + 1;
        text_x = (line_min_x + line_max_x) / 2 - (MAX_TEXT_WIDTH / 2);
        if (text_y + TOTAL_TEXT_HEIGHT <= 512 && text_x >= 0 && text_x + MAX_TEXT_WIDTH <= 640)
        {
            found_valid_pos = true;
        }
        else
        {
            text_y = line_min_y - TOTAL_TEXT_HEIGHT;
            if (text_y >= 0 && text_x >= 0 && text_x + MAX_TEXT_WIDTH <= 640)
            {
                found_valid_pos = true;
            }
            else
            {
                text_x = line_max_x + 1;
                text_y = (line_min_y + line_max_y) / 2 - (TOTAL_TEXT_HEIGHT / 2);
                if (text_x + MAX_TEXT_WIDTH <= 640 && text_y >= 0 && text_y + TOTAL_TEXT_HEIGHT <= 512)
                {
                    found_valid_pos = true;
                }
                else
                {
                    text_x = line_min_x - MAX_TEXT_WIDTH;
                    if (text_x >= 0 && text_y >= 0 && text_y + TOTAL_TEXT_HEIGHT <= 512)
                    {
                        found_valid_pos = true;
                    }
                }
            }
        }
        if (found_valid_pos)
        {
            ts_char max_str[20] = "\0", avg_str[20] = "\0", low_str[20] = "\0";
            ts_f32 f_max = max_temp / 10.0f;
            ts_f32 f_avg = avg_temp / 10.0f;
            ts_f32 f_min = min_temp / 10.0f;
            ts_cpl_snprintf(max_str, 20, "max:%.1f℃", f_max);
            ts_cpl_snprintf(avg_str, 20, "avg:%.1f℃", f_avg);
            ts_cpl_snprintf(low_str, 20, "min:%.1f℃", f_min);
            ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT, color, avg_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT * 2, color, low_str, TEXT_SPACING);
        }
        else
        {
            return -TS_EINVAL;
        }
    }
    return 0;
}

ts_s32 ts_lib_draw_rect_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 x1, ts_u16 y1,
    ts_u16 x2, ts_u16 y2,
    int max_temp,
    int avg_temp,
    int min_temp,
    ts_u08 line_thickness,
    ts_u08 color)
{

   if (get_powerup_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024)
    {
        if (pst_osd == NULL ||x1 >= 1280 || y1 >= 1024 || x2 >= 1280 || y2 >= 1024 ||line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        ts_s32 ret = ts_lib_draw_corner_rect(pst_osd, x1, y1, x2, y2, line_thickness, 8, color);
        if (ret != 0)
            return ret;
        ts_u16 line_min_x = (x1 < x2) ? x1 : x2;
        ts_u16 line_max_x = (x1 > x2) ? x1 : x2;
        ts_u16 line_min_y = (y1 < y2) ? y1 : y2;
        ts_u16 line_max_y = (y1 > y2) ? y1 : y2;

        int half_thickness = (line_thickness - 1) / 2;
        line_min_x -= half_thickness;
        line_max_x += half_thickness;
        line_min_y -= half_thickness;
        line_max_y += half_thickness;
        line_min_x = (line_min_x < 0) ? 0 : line_min_x;
        line_max_x = (line_max_x >= 1280) ? 1279 : line_max_x;
        line_min_y = (line_min_y < 0) ? 0 : line_min_y;
        line_max_y = (line_max_y >= 1024) ? 1023 : line_max_y;

    // #define TEXT_SPACING 2
    // #define LINE_HEIGHT 32
    // #define TOTAL_LINES 3                                 // 三行文字
    // #define TOTAL_TEXT_HEIGHT (LINE_HEIGHT * TOTAL_LINES) // 总高度：96px
    // #define MAX_TEXT_WIDTH 180

        ts_u16 text_x = 0, text_y = 0;
        bool found_valid_pos = false;
        text_y = line_max_y + 1;
        text_x = (line_min_x + line_max_x) / 2 - (180 / 2);
        if (text_y + 96 <= 1024 && text_x >= 0 && text_x + 180 <= 1280)
        {
            found_valid_pos = true;
        }
        else
        {
            text_y = line_min_y - 96;
            if (text_y >= 0 && text_x >= 0 && text_x + 180 <= 1280)
            {
                found_valid_pos = true;
            }
            else
            {
                text_x = line_max_x + 1;
                text_y = (line_min_y + line_max_y) / 2 - (96 / 2);
                if (text_x + 180 <= 1280 && text_y >= 0 && text_y + 96 <= 1024)
                {
                    found_valid_pos = true;
                }
                else
                {
                    text_x = line_min_x - 180;
                    if (text_x >= 0 && text_y >= 0 && text_y + 96 <= 1024)
                    {
                        found_valid_pos = true;
                    }
                }
            }
        }

        if (found_valid_pos)
        {
            ts_char max_str[20] = "\0", avg_str[20] = "\0", low_str[20] = "\0";
            ts_f32 f_max = max_temp / 10.0f;
            ts_f32 f_avg = avg_temp / 10.0f;
            ts_f32 f_min = min_temp / 10.0f;
            ts_cpl_snprintf(max_str, 20, "max:%.1f℃", f_max);
            ts_cpl_snprintf(avg_str, 20, "avg:%.1f℃", f_avg);
            ts_cpl_snprintf(low_str, 20, "min:%.1f℃", f_min);
            // ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, TEXT_SPACING);
            // ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT, color, avg_str, TEXT_SPACING);
            // ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT * 2, color, low_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, 2);
            ts_lib_draw_string(pst_osd, text_x, text_y + 32, color, avg_str, 2);
            ts_lib_draw_string(pst_osd, text_x, text_y + 32 * 2, color, low_str, 2);
        }
        else
        {
            return -TS_EINVAL;
        }
    }
    else //if (get_dector_resolution_type() == DECTOR_RESOLUTION_640X512)
    {
        if (pst_osd == NULL ||x1 >= 640 || y1 >= 512 || x2 >= 640 || y2 >= 512 ||line_thickness < 1 || line_thickness > 10 || color > 0x03)
        {
            return -TS_EINVAL;
        }
        ts_s32 ret = ts_lib_draw_corner_rect(pst_osd, x1, y1, x2, y2, line_thickness, 8, color);
        if (ret != 0)
            return ret;
        ts_u16 line_min_x = (x1 < x2) ? x1 : x2;
        ts_u16 line_max_x = (x1 > x2) ? x1 : x2;
        ts_u16 line_min_y = (y1 < y2) ? y1 : y2;
        ts_u16 line_max_y = (y1 > y2) ? y1 : y2;

        int half_thickness = (line_thickness - 1) / 2;
        line_min_x -= half_thickness;
        line_max_x += half_thickness;
        line_min_y -= half_thickness;
        line_max_y += half_thickness;
        line_min_x = (line_min_x < 0) ? 0 : line_min_x;
        line_max_x = (line_max_x >= 640) ? 639 : line_max_x;
        line_min_y = (line_min_y < 0) ? 0 : line_min_y;
        line_max_y = (line_max_y >= 512) ? 511 : line_max_y;

    #define TEXT_SPACING 2
    #define LINE_HEIGHT 16
    #define TOTAL_LINES 3                                 // 三行文字
    #define TOTAL_TEXT_HEIGHT (LINE_HEIGHT * TOTAL_LINES) // 总高度：48px
    #define MAX_TEXT_WIDTH 90

        ts_u16 text_x = 0, text_y = 0;
        bool found_valid_pos = false;
        text_y = line_max_y + 1;
        text_x = (line_min_x + line_max_x) / 2 - (MAX_TEXT_WIDTH / 2);
        if (text_y + TOTAL_TEXT_HEIGHT <= 512 && text_x >= 0 && text_x + MAX_TEXT_WIDTH <= 640)
        {
            found_valid_pos = true;
        }
        else
        {
            text_y = line_min_y - TOTAL_TEXT_HEIGHT;
            if (text_y >= 0 && text_x >= 0 && text_x + MAX_TEXT_WIDTH <= 640)
            {
                found_valid_pos = true;
            }
            else
            {
                text_x = line_max_x + 1;
                text_y = (line_min_y + line_max_y) / 2 - (TOTAL_TEXT_HEIGHT / 2);
                if (text_x + MAX_TEXT_WIDTH <= 640 && text_y >= 0 && text_y + TOTAL_TEXT_HEIGHT <= 512)
                {
                    found_valid_pos = true;
                }
                else
                {
                    text_x = line_min_x - MAX_TEXT_WIDTH;
                    if (text_x >= 0 && text_y >= 0 && text_y + TOTAL_TEXT_HEIGHT <= 512)
                    {
                        found_valid_pos = true;
                    }
                }
            }
        }

        if (found_valid_pos)
        {
            ts_char max_str[20] = "\0", avg_str[20] = "\0", low_str[20] = "\0";
            ts_f32 f_max = max_temp / 10.0f;
            ts_f32 f_avg = avg_temp / 10.0f;
            ts_f32 f_min = min_temp / 10.0f;
            ts_cpl_snprintf(max_str, 20, "max:%.1f℃", f_max);
            ts_cpl_snprintf(avg_str, 20, "avg:%.1f℃", f_avg);
            ts_cpl_snprintf(low_str, 20, "min:%.1f℃", f_min);
            ts_lib_draw_string(pst_osd, text_x, text_y, color, max_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT, color, avg_str, TEXT_SPACING);
            ts_lib_draw_string(pst_osd, text_x, text_y + LINE_HEIGHT * 2, color, low_str, TEXT_SPACING);
        }
        else
        {
            return -TS_EINVAL;
        }
    }
    return 0;
}
