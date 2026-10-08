/*
 * @Author: shixiaobo 328494516@qq.com
 * @Date: 2025-08-01 17:20:03
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-06-26 09:05:04
 * @FilePath: /ucc3_a_gen/vot_src/vot/menu/votmain.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef __VOT_MAIN_H__
#define __VOT_MAIN_H__

#include <cpl/cpl_semaphore.h>
#include <cpl/cpl_mutex.h>
// #include <myheader.h>
#include <menu/votmenu.h>
#include <menu/usermenu.h>
#include "base/ts_comm.h"

#define TS_HBP_BUF_MAX_SIZE  (2048+64)

#define  VOT_OSD_COLOR_MAX 8
enum {
    /* OSD 类型仅能使用前面 4 个 */
    OSD_COLOR_WHITE = 0,
    OSD_COLOR_BLACK = 1,
    OSD_COLOR_GRAY = 2,
    OSD_COLOR_RED = 3,

    /* 光标类型不会有大缓存，默认使用 4 bit 颜色深度，可以使用后续颜色 */
    OSD_COLOR_YELLOW = 4,
    OSD_COLOR_GREEN = 5,
    OSD_COLOR_BLUE = 6,
    OSD_COLOR_MAGENTA = 7,

    OSD_COLOR_BUTT,
};



typedef enum vot_transfer_type_enum_t_s{
    VOT_TRANSFER_TYPE_NORMAL_UART=0,
    VOT_TRANSFER_TYPE_CDC_ACM,
    VOT_TRANSFER_TYPE_LIBUSB,
} vot_transfer_type_enum_t ;


// 定义协议状态
typedef enum parser_state_enum_t_s{
    STATE_WAIT_HEADER=0,
    STATE_WAIT_COMPLETE_FRAME,
} parser_state_enum_t ;

typedef struct ring_buffer_t_s{
	ts_u32  buffer;
	ts_u32  uart_buff_len;
    ts_u08* uart_buff;
    ts_u32 left_bytes;  // 剩余的有效数据字节数
    ts_u32 write_index; // 写指针，中断修改，必须加volatile
    ts_u32 read_index;  // 读指针，主循环修改，也加volatile

    ts_cpl_mutex_s_t ring_buff_mutex;

	// volatile ts_u32 left_bytes;  // 剩余的有效数据字节数
    // volatile ts_u32 write_index; // 写指针，中断修改，必须加volatile
    // volatile ts_u32 read_index;  // 读指针，主循环修改，也加volatile

} ring_buffer_t;



typedef struct state_machine_t_s{
	// ts_s32 m_s32_dev;
	ts_u08 m_state;
	ts_u08 m_frame_type;
	ts_u32 m_complete_frame_len;
	ts_u32 m_frame_bytes_index;
	
	// ts_u32 m_packet_buf;   
	
    // ts_cpl_semphr_s_t m_st_sem;
    ts_u08 s08_buff[2048];

}state_machine_t;

typedef struct vot_hbp_ctx_s {
    /* 对接设备 */
    ts_s32 s32_dev;

    /* 当前解析的总大小 */
    ts_u32 u32_total_len;

    /* 当前写指针 */
    ts_u32 u32_wr_idx;

    /* 保存的当前校验和 */
    ts_u08 u08_chk_sum;
    // ts_u08 u08_rcv_dev_ready;
    ts_u08 u08_packet_complete_flag;
    ts_u08 msg_type;

    // ts_u08 au08_rev[3];

    ts_cpl_semphr_s_t st_sem;
    // ts_cpl_spinlock_s_t st_spinlock;

    ring_buffer_t m_ring_buf_instance;
    /* 统计信息 */
    // ts_u32 u32_err_cnt;
    // ts_u32 u32_nrml_cnt;
    // ts_u32 u32_tot_frm;

 
} vot_hbp_ctx_s_t;

typedef struct vot_head_s {
    ts_u08 u08_flag1;
    ts_u08 u08_flag2;
    ts_u08 u08_flag3;
    ts_u08 u08_length_high;
    ts_u08 u08_length_low;

} vot_head_s_t;

typedef struct vot_hbp_head_s {
    ts_u08 u08_flag;
    ts_u08 u08_cmd;

    /* 1-byte 为单位 ， 最大 TS_HBP_DATA_MAX_SIZE, 用户不可见 */
    ts_u16 u16_size;
} vot_hbp_head_s_t;

typedef struct vot_hbp_tail_s {

    /* 开放，不用命令字有各自的属性或者配置, 参看各自的结构体 */
    ts_u08 au08_rev[3];

    /* 校验和取反 */
    ts_u08 u08_sum_n;
} vot_hbp_tail_s_t;

typedef enum {
    UART_MSG_TYPE_VOT_NORMAL=0,
    UART_MSG_TYPE_VOT_ASIC,
    UART_MSG_TYPE_BUTT,
    
}UART_MSG_TYPE;

typedef ts_u32 (FuncVotTransfer)(ts_u08* payload, ts_u32 payloadlen);

extern int cli_simple_run_command(const char *cmd);

ts_void init_sys_ext_config(void);
ts_void display_init(ts_void);
ts_void update_statis_info_init(void);
ts_void deinit_sys_ext_config(void);
ts_void vot_misc_parameters_init(void);
ts_void Clear_Templut_Information(void);

void ring_buffer_init(ring_buffer_t *rb, ts_char* buf_name);
void ring_buffer_deinit(ring_buffer_t *rb);
void ring_buffer_update_read(ring_buffer_t *rb_in , ring_buffer_t *src_rb);
void get_ring_buffer_info(ring_buffer_t *dst_rb, ring_buffer_t *src_rb);
ts_bool ring_buffer_update_write(ring_buffer_t *rb, ts_u08* data_in, ts_u32 data_len );


ts_void menu1_parameters_init(void);
ts_void menu2_parameters_init(void);

ts_void userpara_reset(void);
ts_void testpara_reset(void);
ts_void menu1_default_parameters_init(void);
ts_void menu2_default_parameters_init(void);
ts_void user_isp_cfg_save(void);
ts_void testpara_reset_det_parameter(void);
ts_u16 vot_read_det_ad(void);
ts_void cvbs_osd_refresh_display(int is_active);
ts_void cvbs_cursor_refresh_display(int is_active);

ts_void reset_usr_isp_cfg(void);
// ts_void cvbs_cursor_refresh_display_auto(void);
ts_void cvbs_cursor_refresh_display_auto(void);

ts_void templut_captions_refresh_display(int is_active);
ts_void IsHide_templut_osd_display(ts_u08 is_active);

void GetDeviceInfo(ts_u08* uart_msg, ts_u16 msg_len);

void sffc_on_off(ts_u08 sffc_mode, ts_u08 restore_or_backup );
void shutter_cut_on_off(ts_u08 shutter_mode, ts_u08 restore_or_backup );
void shutter_set_mode(ts_u08  shutter_mode);

ts_void save_local_isp_cfg(void);
ts_void unsave_local_isp_cfg(void);

ts_void menu_change_palette_vot(ts_u32 u32_cfg, ts_u32 u32_reset);
ts_void special_para_init(ts_u08 Palette);
ts_void vot_hbp_parse_cb_v2(state_machine_t * state_machine, ts_u08* u08_val ,  ts_u16 read_bytes, FuncVotTransfer* transfer_callback);

ts_void create_sun_pt_pthread(void);
ts_void create_vot_wdt_feed_pthread(void);

ts_void vot_routine_pthread_init(void);
ts_void vot_uart_pthread_init(void);
ts_void create_sffc_run_state(ts_void);// 创建一个开关函数，控制锅盖去圆斑功能的开启和关闭
//测试根据字符返回数组点阵索引
typedef enum {
    INDEX_TYPE_NONE = -1,  // 未找到
    INDEX_TYPE_ASCII = 0,  // ASCII字符索引
    INDEX_TYPE_UTF8 = 1    // UTF-8字符索引
} IndexType;
 
// 返回结果结构体：整合是否找到、类型、索引
typedef struct {
    int found;          // 0：未找到，1：找到
    IndexType type;     // 索引类型（仅found=1时有效）
    int index;          // 索引值（仅found=1时有效）
} CharIndexResult;


ts_s32 ts_lib_draw_crosshair_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 center_x,
    ts_u16 center_y,
    ts_u08 line_thickness,
    ts_u08 color,
    ts_u08 length,
    int point_temp,
    ts_u08 char_spacing
);

ts_s32 ts_lib_draw_line_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 x1, ts_u16 y1,
    ts_u16 x2, ts_u16 y2,
    int max_temp,
    int avg_temp,
    int min_temp,
    ts_u08 line_thickness,
    ts_u08 color
);

ts_s32 ts_lib_draw_rect_with_temp(
    ts_nodebuf_s_t *pst_osd,
    ts_u16 x1, ts_u16 y1,
    ts_u16 x2, ts_u16 y2,
    int max_temp,
    int avg_temp,
    int min_temp,
    ts_u08 line_thickness,
    ts_u08 color
);

#endif