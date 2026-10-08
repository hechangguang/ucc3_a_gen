// /*
//  * osd.c -- tdk osd 使用例子
//  */
// #include <base/ts_type.h>
// #include <base/ts_comm.h>
// #include <base/ts_media.h>
// #include <cpl/ts_cpl.h>
// #include <uapi/base_v100.h>
// #include <uapi/chip_def.h>
// #include <uapi/osd_v100.h>
// #include <uapi/vpo_v100.h>
// #include <uapi/viu_v100.h>
// #include <command.h>
// #include <stdio.h>
// #include "fontlib.h"


// #define OSD_COLOR_USE_BLUE

// enum {
//     OSD_COLOR_WHITE = 0,
//     OSD_COLOR_BLACK = 1,
//     OSD_COLOR_RED = 2,

// #ifdef OSD_COLOR_USE_BLUE
//     OSD_COLOR_BLUE = 3,
// #else
//     OSD_COLOR_YELLOW = 3,
// #endif

//     OSD_COLOR_BUTT,
// };

// static ts_rgb_s_t sast_rgbs[OSD_COLOR_SIZE] = {
//     /* 白色， 透明度100， 即完全透明 */
//     {255, 255, 255, 0},

//     /* 黑色， 透明度0， 即完全不透明 */
//     {0, 0, 0, 100},

//     /* 红色， 透明度0， 即完全不透明 */
//     {255, 0, 0, 0},

// #ifdef OSD_COLOR_USE_BLUE
//     /* 蓝色， 透明度0， 即完全不透明*/
//     {0, 0, 255, 0},
// #else
//     /* 黄色， 透明度75 */
//     {255, 255, 0, 0},
// #endif
// };

// static ts_mdev_t shdl_osd_dev;

// static ts_u32 su32_inited = 0;

// ts_u32 osd_get_rnd(ts_u32 u32_max, ts_u32 u32_min)
// {
//     ts_u32 u32_tmp;

//     u32_tmp = ts_cpl_random();

//     u32_tmp = (u32_tmp % (u32_max - u32_min + 1)) + u32_min;

//     return u32_tmp;
// }


// static ts_void osd_set_rnd_corr(ts_nodebuf_s_t *pst_osd_node, ts_u32 u32_width, ts_u32 u32_height)
// {
//     ts_u32 u32_left, u32_top;
//     ts_osd_attr_s_t st_attr;

//     ts_tdk_osd_get_attr(pst_osd_node, (ts_osd_attr_s_t *)&st_attr);

// #if 1
//     u32_left = osd_get_rnd(u32_width, 0);
//     u32_top = osd_get_rnd(u32_height, 0);
//     st_attr.u16_left = (ts_u16)u32_left;
//     st_attr.u16_top = (ts_u16)u32_top;
// #else
//     st_attr.u16_left = st_attr.u16_left--;
// #endif

//     if (st_attr.u08_visible > 0)
//         st_attr.u08_visible = 1;
//     else
//         st_attr.u08_visible = 1;

//     ts_tdk_osd_set_attr(pst_osd_node, (ts_osd_attr_s_t *)&st_attr);
// }


// static ts_void osd_demo_rnd_temp(ts_u32 *pu32_buf)
// {
//     ts_u32 i, j, u32_cnt, u32_tmp;
//     ts_u32 au32_tmp[3];

//     u32_cnt = 3;
//     for (i = 0; i < u32_cnt; i++)
//         au32_tmp[i] = osd_get_rnd(400, 0);

//     for (i = 0; i < u32_cnt; i++) {
//         for (j = 0; j < (u32_cnt -i - 1); j++) {
//             if (au32_tmp[j] > au32_tmp[j + 1]) {
//                 u32_tmp = au32_tmp[j];

//                 au32_tmp[j] = au32_tmp[j + 1];
//                 au32_tmp[j + 1] = u32_tmp;
//             }
//         }
//     }

//     for (i = 0; i < u32_cnt; i++)
//         pu32_buf[i] = au32_tmp[i];
// }


// static ts_void osd_update_temp(ts_nodebuf_s_t *pst_old_node, ts_nodebuf_s_t *pst_new_node, ts_u32 u32_temp)
// {
//     ts_char au08_buf[8];
//     ts_osd_attr_s_t st_attr;
//     ts_u32 u32_int, u32_remain;

//     u32_int = u32_temp / 10;
//     u32_remain = u32_temp % 10;

//     ts_cpl_snprintf((ts_char *)&au08_buf, 6, "%2d.%1d", u32_int, u32_remain);

//     ts_tdk_osd_get_attr(pst_old_node, (ts_osd_attr_s_t *)&st_attr);
//     ts_tdk_osd_set_attr(pst_new_node, (ts_osd_attr_s_t *)&st_attr);

//     ts_lib_fontl_update_str(pst_new_node, (ts_char *)&au08_buf, st_attr.st_osd_cfg.u08_font_color, st_attr.st_osd_cfg.u08_bg_color);

//     ts_tdk_osd_update(pst_old_node, pst_new_node);
// }


// ts_void osd_demo_init(ts_void)
// {
//     ts_osd_ch_attr_s_t st_osd_disp_ch_attr;
   
//     st_osd_disp_ch_attr.pst_rgb = (const ts_rgb_s_t *)&sast_rgbs;
//     st_osd_disp_ch_attr.u16_rgb_num = OSD_COLOR_SIZE;
//     st_osd_disp_ch_attr.u08_pixel_bw = TS_PIXEL_BW_2BIT;
//     st_osd_disp_ch_attr.u08_fill_pixel = 0;

//     st_osd_disp_ch_attr.u32_merge_size = 32 * 1024;

// #if 0
//     shdl_osd_dev = TS_OSD_CVBS_MDEV0;
// #else
//     shdl_osd_dev = TS_OSD_DISP_MDEV0;
// #endif

//     ts_tdk_osd_set_ch_attr(shdl_osd_dev, (const ts_osd_ch_attr_s_t *)&st_osd_disp_ch_attr);
// }


// static ts_nodebuf_s_t *spst_dot;
// TS_DECLARE_CMD(ofix)
// {
//     ts_u32 u32_left, u32_top;
//     ts_osd_attr_s_t st_attr;

//     ts_tdk_osd_get_attr(spst_dot, (ts_osd_attr_s_t *)&st_attr);

//     if (argc > 1) {
//         u32_left = ts_cpl_str2num(argv[1]);
//         u32_top = ts_cpl_str2num(argv[2]);
//     } else {
//         u32_left = 0;
//         u32_top = 0;
//     }
    
//     st_attr.u16_left = (ts_u16)u32_left;
//     st_attr.u16_top = (ts_u16)u32_top;

//     ts_tdk_osd_set_attr(spst_dot, (ts_osd_attr_s_t *)&st_attr);
//     ts_tdk_osd_refresh_display(shdl_osd_dev, TS_OSD_TYPE_CURSOR, TS_OSD_REFRESH_VISIBLE_ON);
//     ts_tdk_osd_refresh_display(shdl_osd_dev, TS_OSD_TYPE_CURSOR, TS_OSD_REFRESH_VISIBLE_ON);
	
// 	return 0;
// }



// ts_s32 osd_demo1(ts_void *pv_param)
// {
//     ts_vpo_chn_attr_s_t st_vpo_attr;
//     ts_u32 u32_cursor_width;
//     ts_nodebuf_s_t *pst_osd_center, *pst_osd_hi, *pst_osd_lo, *pst_osd_buf0, *pst_osd_buf1, *pst_osd_buf2;
//     ts_nodebuf_s_t *pst_cursor_center, *pst_cursor_hi, *pst_cursor_lo;
//     ts_u32 u32_center, u32_hi, u32_lo, u32_sw_center, u32_sw_hi, u32_sw_lo;
//     ts_u32 au32_tmp[3];
//     ts_fontl_cursor_attr_s_t st_cursor_attr;
//     ts_fontl_osd_attr_s_t st_osd_attr;

//     TS_UNUSED(pv_param);

//     /* 获取输出图像的宽高 */
//     //ts_tdk_vpo_get_chn_attr(shdl_osd_dev, &st_vpo_attr);
//     u32_cursor_width = 1;

//     st_vpo_attr.u16_disp_width = 640;
//     st_vpo_attr.u16_disp_height = 512;

//     /* 创建三种温度文字和光标 */
//     st_cursor_attr.u16_center_left = (st_vpo_attr.u16_disp_width - u32_cursor_width) >> 1;
//     st_cursor_attr.u16_center_top = (st_vpo_attr.u16_disp_height - u32_cursor_width) >> 1;
//     st_cursor_attr.u16_height = 16;
//     st_cursor_attr.u16_width = u32_cursor_width;

//     st_cursor_attr.u08_type = TS_CURSOR_TYPE_DOT;

//     if (TS_CURSOR_TYPE_RECT == st_cursor_attr.u08_type) {
//         st_cursor_attr.u08_thick = 2;
//         st_cursor_attr.u16_rect_width = 20;
//         st_cursor_attr.u16_rect_height = 20;
//     } else {
//         st_cursor_attr.u08_margin = 4;
//         st_cursor_attr.u08_blink = 25;
//     }

//     st_cursor_attr.u08_visiable = 1;
//     st_cursor_attr.u08_priority = 0;

//     st_cursor_attr.u08_center_color = OSD_COLOR_WHITE;
//     st_cursor_attr.u08_cursor_color = OSD_COLOR_WHITE;
//     pst_cursor_center = ts_lib_fontl_gen_cursor(shdl_osd_dev, &st_cursor_attr);
//     if (ts_null == pst_cursor_center) {
//         vision_log_debug("create cursor_center fail.\n");

//         return -1;
//     }

//     st_cursor_attr.u16_center_left = 100;
//     st_cursor_attr.u16_center_top = 200;
//     st_cursor_attr.u08_center_color = OSD_COLOR_WHITE;
//     st_cursor_attr.u08_cursor_color = OSD_COLOR_WHITE;
//     pst_cursor_hi = ts_lib_fontl_gen_cursor(shdl_osd_dev, &st_cursor_attr);
//     if (ts_null == pst_cursor_hi) {
//         vision_log_debug("create cursor_center fail.\n");

//         return -1;
//     }

//     spst_dot = pst_cursor_hi;


//     st_cursor_attr.u16_center_left = 101;
//     st_cursor_attr.u16_center_top = 300;
//     st_cursor_attr.u08_visiable = 1;
//     st_cursor_attr.u08_center_color = OSD_COLOR_WHITE;
//     st_cursor_attr.u08_cursor_color = OSD_COLOR_WHITE;
//     pst_cursor_lo = ts_lib_fontl_gen_cursor(shdl_osd_dev, &st_cursor_attr);
//     if (ts_null == pst_cursor_lo) {
//         vision_log_debug("create cursor_lo fail.\n");

//         return -1;
//     }

//     /* 创建温度显示 */
//     st_osd_attr.u16_left = 520;
//     st_osd_attr.u16_top = 0;

//     st_osd_attr.u08_pxi_bw = TS_PIXEL_BW_1BIT;
//     st_osd_attr.u08_visiable = 1;
//     st_osd_attr.u08_priority = 0;

//     st_osd_attr.s08_alpha = 50;
//     st_osd_attr.u08_auto_invert = 0;

//     st_osd_attr.u08_font_color = OSD_COLOR_WHITE;
//     st_osd_attr.u08_bg_color = OSD_COLOR_BLACK;

//     ts_lib_fontl_gen_str(shdl_osd_dev, 
//             "最高温度:     ℃\n中心温度:     ℃\n最低温度:     ℃", &st_osd_attr);
    
//     /* 温度分成 3 个 OSD 显示，按照数据有变化才更新，减少开销 */
//     st_osd_attr.s08_alpha = -1;
//     st_osd_attr.u16_left = st_vpo_attr.u16_disp_width - 96;
//     st_osd_attr.u16_top = 0;
//     st_osd_attr.u08_priority = 1;
//     pst_osd_hi = ts_lib_fontl_gen_str(shdl_osd_dev, "32.1", &st_osd_attr);

//     st_osd_attr.u16_top = 32;
//     pst_osd_center = ts_lib_fontl_gen_str(shdl_osd_dev, "32.1", &st_osd_attr);

//     st_osd_attr.u16_top = 64;
//     pst_osd_lo = ts_lib_fontl_gen_str(shdl_osd_dev, "32.1", &st_osd_attr);


//     pst_osd_buf0 = ts_lib_fontl_gen_str(ts_null, "    ", &st_osd_attr);
//     pst_osd_buf1 = ts_lib_fontl_gen_str(ts_null, "    ", &st_osd_attr);
//     pst_osd_buf2 = ts_lib_fontl_gen_str(ts_null, "    ", &st_osd_attr);

//     u32_hi = 321;
//     u32_lo = 321;
//     u32_center = 321;
//     u32_sw_center = 0;
//     u32_sw_hi = 0;
//     u32_sw_lo = 0;
//     while (1) 
//     {
//         /* 随机位置 */
//         osd_set_rnd_corr(pst_cursor_hi, st_vpo_attr.u16_disp_width, st_vpo_attr.u16_disp_height);
//         osd_set_rnd_corr(pst_cursor_lo, st_vpo_attr.u16_disp_width, st_vpo_attr.u16_disp_height);

//         /* 随机三个温度 */
//         osd_demo_rnd_temp((ts_u32 *)&au32_tmp);

//         if (u32_lo != au32_tmp[0]) {
//             u32_lo = au32_tmp[0];
//             if (0 == u32_sw_lo) {
//                 osd_update_temp(pst_osd_lo, pst_osd_buf0, u32_lo);
//                 u32_sw_lo = 1;
//             } else {
//                 osd_update_temp(pst_osd_buf0, pst_osd_lo, u32_lo);
//                 u32_sw_lo = 0;
//             }
//         }

//         if (u32_center != au32_tmp[1]) {
//             u32_center = au32_tmp[1];
//             if (0 == u32_sw_center) {
//                 osd_update_temp(pst_osd_center, pst_osd_buf1, u32_center);
//                 u32_sw_center = 1;
//             } else {
//                 osd_update_temp(pst_osd_buf1, pst_osd_center, u32_center);
//                 u32_sw_center = 0;
//             }
//         }

//         if (u32_hi != au32_tmp[2]) {
//             u32_hi = au32_tmp[2];
//             if (0 == u32_sw_hi) {
//                 osd_update_temp(pst_osd_hi, pst_osd_buf2, u32_hi);
//                 u32_sw_hi = 1;
//             } else {
//                 osd_update_temp(pst_osd_buf2, pst_osd_hi, u32_hi);
//                 u32_sw_hi = 0;
//             }
//         }

//         ts_tdk_osd_refresh_display(shdl_osd_dev, TS_OSD_TYPE_CURSOR, TS_OSD_REFRESH_VISIBLE_ON);
//         ts_tdk_osd_refresh_display(shdl_osd_dev, TS_OSD_TYPE_OSD, TS_OSD_REFRESH_VISIBLE_ON);

//         ts_cpl_msleep(10000);
//     }

//     return 0;
// }


// /* 
//     osd on
//     osd off
// */
// TS_DECLARE_CMD(osd)
// {
//     // ts_cpl_task_s_t st_osd_task;

//     // if (0 == su32_inited) {
//     //     osd_demo_init();
//     //     su32_inited = 1;

//     //     #if 0
//     //         osd_demo1(ts_null);
//     //     #else
//     //         ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_osd_task, (ts_cpl_kthread_t)osd_demo1, ts_null, "osd", 20, 2048);
//     //     #endif
//     // }


//     return 0;
// }

