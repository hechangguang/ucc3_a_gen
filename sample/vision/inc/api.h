/*
 * @Author: shixiaobo 328494516@qq.com
 * @Date: 2025-08-08 17:37:23
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-29 15:06:08
 * @FilePath: /ucc3_a_gen/sample/vision/inc/api.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * api.h -- vision 接口等声明头文件
 */
 #ifndef __API_H__
 #define __API_H__
 
 #ifdef __cplusplus
 #if __cplusplus
 extern "C"
 {
 #endif
 
 #endif /* __cplusplus */
 #include <base/ts_type.h>
 #include <base/ts_comm.h>
 #include <base/ts_bitop.h>
 #include <cpl/ts_cpl.h>
 #include <vot_tlsf.h>
 //#define LOG_TAG    "main"
 
 #include <elog.h>
 #include <stdio.h>
 
 
//需要串口0输出日志，开启VOT_DEBUG_EN宏定义，
// 默认出厂固件编译时，不开启VOT_DEBUG_EN宏，此时，日志的输出由 vot_get_force_debug_flag()返回值决定是否开启日志输出
// 为了减少出厂固件大小，出厂时不开启VOT_DEBUG_EN宏定义，此时，vision_log_debug相关的日志不输出
//平时调试，可以开启VOT_DEBUG_EN，开启vision_log_debug相关的日志输出
// 必要日志才用vision_log_warn输出，普通调试日志使用vision_log_debug输出

//  #define VOT_DEBUG_EN 1
 
 //#define Receive_JSON_File_EN 1
 
 // #define VOT_IOT_DEFINE       /* 物联部门--杨能筒机专用 */
 
 /** @note 测温功能-->切换FIRE模块 */
 // #define VOT_TEMP_YUV_FIRE
 
 extern ts_s32 gs32_vision_log_lvl;
 
 

 /* 格式化日志输出 */
 #define vision_log_err(fmt, args...)                                                  \
     do                                                                                \
     {                                                                                 \
         log_e(fmt, ##args); \
     } while (0)
 
 #define vision_log_warn(fmt, args...)                                                     \
     do                                                                                    \
     {                                                                                     \
         log_w(fmt, ##args); \
     } while (0)
 
 #define vision_log_notice(fmt, args...)                                                  \
     do                                                                                   \
     {                                                                                    \
         log_i(fmt, ##args); \
     } while (0)
 
#ifdef VOT_DEBUG_EN
 #define vision_log_debug(fmt, args...)                                                  \
     do                                                                                  \
     {                                                                                   \
         log_d(fmt, ##args); \
     } while (0)
#else

#define vision_log_debug(fmt, args...)
#endif

 
 /** @note 测温功能启用FIRE模块 */
 #define COUNT_DEFINED_MACROS    (defined(VOT_TEMP_YUV_FIRE) + defined(VOT_TEMP_RAW_FIRE))
 #if COUNT_DEFINED_MACROS > 1
     #error "只能定义 VOT_TEMP_YUV_FIRE, VOT_TEMP_RAW_FIRE 中的一个，不能同时定义多个"
 #endif
 // #if COUNT_DEFINED_MACROS == 0
 //     #define VOT_TEMP_YUV_FIRE
 // #endif
 
     /*
     u32_version :
 
     高2位：
     MA/MF(0x4D41/0x4D46) ：为 mpw 的 asic 和 fpga 版本
     FA/FF(0x4641/0x4646) ：为 full mask 的 asic 和 fpga 版本
     */
 
 #define APP_VERSION_MPW_ASIC (0x4D410000)
 #define APP_VERSION_MPW_FPGA (0x4D460000)
 #define APP_VERSION_FMASK_ASIC (0x46410000)
 #define APP_VERSION_FMASK_FPGA (0x46460000)
 
 #define APP_SET_VERSION(a, b) (a | (b & 0xFFFF))
 
 extern ts_void insmod_all_sh(ts_void);
 
 int f_hci_init(const char *name, ts_cpl_semphr_s_t *init_done_sem, ts_void *pv_tmm);
 ts_void hci_init(ts_void *pv_tmm);
 
 ts_void vpo_cfg_bte(ts_void *pv_attr);
 ts_void vpo_cfg_cvbs(ts_void *pv_attr);
 ts_void vpo_cfg_mem(ts_u32 u32_width, ts_u32 u32_height, ts_f32 f32_fps);
 ts_void vpo_cfg_mipiCsi2(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH, ts_u32 yOutEn, ts_u32 rawOutEn, ts_u08 u08_data_type, ts_u08 u08_dphy_lane_num, ts_u16 u16_data_rate);
 ts_void vpo_cfg_cvbs_bte(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH, ts_u08 u08_bt_8bit_en, ts_u08 u08_external_sync_en, ts_u32 yOutEn, ts_u32 rawOutEn);
 
 ts_void vpo_cfg_cvbs_mem(ts_void *pv_attr);
 
 ts_void vpo_cfg_cvbs_memYUVRAW(ts_u32 u32_short_en, ts_u32 imgW, ts_u32 imgH, ts_u32 imgFps, ts_u32 yOutEn, ts_u32 rawOutEn);
 ts_void vpo_cfg_lcd_d0601280pfd40(ts_u32 fps);
 ts_void vpo_cfg_cvbs_mipiCsi2(ts_f32 f32_fps, ts_u32 imgW, ts_u32 imgH, ts_u32 yOutEn, ts_u32 rawOutEn);
 ts_void vpo_cfg_dsi_core(ts_u32 u32_img_w, ts_u32 u32_img_h, ts_f32 f32_img_fps);
 ts_void isp_demo(ts_void);
 ts_s32 start_isp(ts_char *pc_name, ts_u32 u32_usb_en, ts_u32 u32_auto_cut, ts_u32 u32_isp_en);
 
 /* for simulation */
 ts_void sim_set_cfg(ts_u32 u32_width, ts_u32 u32_height, ts_u32 u32_fps, ts_u32 u32_clk);
 
 /* for demo */
 ts_void start_testbar(ts_u32 u32_param);
 ts_void start_viu_mem(ts_u32 u32_param);
 
 #ifdef __cplusplus
 #if __cplusplus
 }
 #endif
 #endif /* __cplusplus */
 
 #endif /* __API_H__ */
  