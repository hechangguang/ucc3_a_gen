/* 
 * viu_v100.h -- viu v100 驱动接口头文件
 */
#ifndef __UAPI_VIU_V100_H__
#define __UAPI_VIU_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* ID 定义：
    编号    类型    自定义      厂家
    0..7    8..15   16..23   24..31
*/
#define TS_VIU_MK_SNRT(id, type, custom, vendor)    \
            (((vendor & 0xFF) << 24) | ((custom & 0xFF) << 16) | ((type & 0xFF) << 8) | (id & 0xFF))

/* vendor */
#define TS_VIU_SNRV_TS          'S'
#define TS_VIU_SNRV_HIK         'H'
#define TS_VIU_SNRV_IRAY        'I'
#define TS_VIU_SNRV_GD          'G'
#define TS_VIU_SNRV_TG          'T'
#define TS_VIU_SNRV_VOT         'V'
#define TS_VIU_SNRV_DL          'D'
#define TS_VIU_SNRV_AK          'A'
#define TS_VIU_SNRV_FJR         'F'
#define TS_VIU_SNRV_TEEMSUN     'K'
#define TS_VIU_SNRV_ZS          'Z'

/* 自定义探测器 */
#define TS_VIU_SNRV_CUS         'C'

/* type，可以用于分辨率、制冷、非制冷等类型定义 */
#define TS_VIU_SNRT_NORMAL      (0)
#define TS_VIU_SNRT_COOL        (1)
#define TS_VIU_SNRT_UNCOOL      (2)

/* 统计值类型 */
#define TS_VIU_STATS_COL        (0)
#define TS_VIU_STATS_ROW        (1)
#define TS_VIU_STATS_DOT        (2)

/* EX VSYNC MODE */
#define TS_VIU_EX_VSYNC_AUTO_GEN (1)
#define TS_VIU_EX_VSYNC_FROM_IO  (2)
#define TS_VIU_EX_VSYNC_CVBS     (3)

/* 对接探测器定义 */
typedef enum ts_viu_snrt_en {
    /* 灰条 */
    TS_VIU_SNRT_TESTBAR = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_NORMAL, 0, TS_VIU_SNRV_TS),
    TS_VIU_SNRT_MEM = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_NORMAL, 0, TS_VIU_SNRV_TS),

    /* HK */
    TS_VIU_SNRT_H6412C1SD = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_HIK),
    TS_VIU_SNRT_H3812 = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_HIK),
    TS_VIU_SNRT_H2512 = TS_VIU_MK_SNRT(2, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_HIK),
    TS_VIU_SNRT_H9612 = TS_VIU_MK_SNRT(3, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_HIK),

    /* IRAY */
    TS_VIU_SNRT_RTC3123 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTC3123_MODE1 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 1, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTD6122 = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTD121C = TS_VIU_MK_SNRT(2, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTD123C = TS_VIU_MK_SNRT(3, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTC6081 = TS_VIU_MK_SNRT(4, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_RTD3122 = TS_VIU_MK_SNRT(5, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),
    TS_VIU_SNRT_ELF256 = TS_VIU_MK_SNRT(6, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_IRAY),

    /* GD */
    TS_VIU_SNRT_GD256 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_GD),
    TS_VIU_SNRT_TIMO212 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 1, TS_VIU_SNRV_GD),
    TS_VIU_SNRT_GST612C2 = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_GD),
    TS_VIU_SNRT_GST1212 = TS_VIU_MK_SNRT(2, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_GD),
    TS_VIU_SNRT_GST612_PASSIVE = TS_VIU_MK_SNRT(3, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_GD),
    TS_VIU_SNRT_GST1212_PASSIVE = TS_VIU_MK_SNRT(4, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_GD),

    /* TGKJ */
    TS_VIU_SNRT_TG6515 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_COOL, 0, TS_VIU_SNRV_TG),

    /* VOT */
    TS_VIU_SNRT_PLANCK640C = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_VOT),
    TS_VIU_SNRT_PLANCK384 = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_VOT),
    TS_VIU_SNRT_RV1280 = TS_VIU_MK_SNRT(2, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_VOT),

    /* DL */
    TS_VIU_SNRT_VM1265 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_DL),

    /* AK */
    TS_VIU_SNRT_AK612 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_AK),
    TS_VIU_SNRT_AK312 = TS_VIU_MK_SNRT(1, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_AK),
    TS_VIU_SNRT_AK3212_MODE0 = TS_VIU_MK_SNRT(2, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_AK),
    TS_VIU_SNRT_VA640 = TS_VIU_MK_SNRT(3, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_AK),

    /* FJR */
    TS_VIU_SNRT_RF640 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_FJR),
    /* TEEMSUN */
    TS_VIU_SNRT_TS640 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_TEEMSUN),

    /* ZS */
    TS_VIU_SNRT_BFX6412 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_ZS),

    /* BRIDGE */
    TS_VIU_SNRT_BCUDAS12 = TS_VIU_MK_SNRT(0, TS_VIU_SNRT_UNCOOL, 0, TS_VIU_SNRV_CUS),

    TS_VIU_SNRT_BUTT = 0xffffffff,
} ts_viu_snrt_en_t;


/* 校正相关配置 */
typedef enum ts_viu_corr_alg_en {
    /* successive approximation */
    TS_VIU_CORR_ALG_SA = 0,

    /* dichotomy */
    TS_VIU_CORR_ALG_DICHOTOMY = 1,

    TS_VIU_CORR_ALG_BUTT
} ts_viu_corr_alg_en_t;

#define TS_VIU_ADJ_MODE_BACKWARD    (0)
#define TS_VIU_ADJ_MODE_FORWARD     (1)
typedef struct ts_viu_adj_s {
    ts_u16 u16_thr_hi;
    ts_u16 u16_thr_lo;

    /* 调整模式 */
    ts_u08 u08_mode;

    /* 调整步进 */
    ts_u08 u08_step;

    /* 调整掩码 */
    ts_u08 u08_mask;

    /* 调整帧数 */
    ts_u08 u08_frame_cnt;
} ts_viu_adj_s_t;


typedef struct ts_viu_corr_attr_s {
    /* 硬件校正使用的算法 */
    ts_u08 u08_alg;

    /* 对校正值做限幅 */
    ts_u08 u08_clip_en;

    /* 校正执行频率，取值范围 1..9 取值越小执行频率越小 */
    ts_u08 u08_freq;

    /* 校正初始值 */
    ts_u08 u08_reset_val;

    /* 配置校正的最大最小值 */
    ts_u08 u08_max;
    ts_u08 u08_min;

    ts_u08 u08_blanking_val;
    ts_u08 u08_rev;

    /* 快速调整以及微调配置 */
    ts_viu_adj_s_t st_fast;
    ts_viu_adj_s_t st_trim;
} ts_viu_corr_attr_s_t;


/* sensor 相关配置 */
typedef struct ts_viu_snr_cfg_s {
    ts_u32 u32_addr;
    ts_u32 u32_val;
} ts_viu_snr_cfg_s_t;

typedef struct ts_viu_snr_timing_attr_s {
    ts_u08 u08_redefine_timing_en;

    /* image inverse enable */
    ts_u08 u08_img_inv_en;

    ts_u16 u16_rev1;

    /* total height */
    ts_u16 u16_raw_h;

    /* total width */
    ts_u16 u16_raw_w;

    /* extra timing */
    ts_u16 u16_extra_timing;

    /* frame sync code start line */
    ts_u16 u16_vsync_start;

    /* line sync code start dot */
    ts_u16 u16_hsync_start;

    /* sensor valid height */
    ts_u16 u16_vld_h;

    /* sensor valid width */
    ts_u16 u16_vld_w;

    /* sensor cut start vertical position,cuted height = ts_viu_comm_attr_s.u16_height */
    ts_u16 u16_cut_start_v;

    /* sensor cut start horizontal position,cuted width = ts_viu_comm_attr_s.u16_width */
    ts_u16 u16_cut_start_h;

    ts_u16 u16_rev2;

    /* sensor output vsync to data valid time */
    ts_u32 u32_data_dly;
} ts_viu_snr_timing_attr_s_t;


typedef struct ts_viu_snr_attr_s {
    /* 通过 viu 发送给 sensor 的配置结构体,
        发送的单位长度  
    */
    ts_u16 u16_uint_len;

    /* 配置个数 */
    ts_u16 u16_cfg_num;

    /* 具体配置 */
    ts_viu_snr_cfg_s_t *pst_cfg;
} ts_viu_snr_attr_s_t;


/* 公共属性结构体定义 */
typedef struct ts_viu_comm_attr_s {
    /* 指定探测器 */
    ts_viu_snrt_en_t en_snr;

    /* viu 以及后续 isp vpo 等工作有效宽高 */
    ts_u16 u16_height;
    ts_u16 u16_width;

    /* 帧率 */
    ts_f32 f32_fps;

    /* 单位微秒， 如果为 0 ，则不进行内部复位输出 */
    ts_u16 u16_rst_dly_us;

    /* 像素与时钟关系，一个时钟一个像素 = 1; 两个时钟一个像素 = 2 */
    ts_u08 u08_pixel_ratio;
    ts_u08 u08_rev;

    /* 线序配置 */
    ts_u32 u32_i_order_hi;
    ts_u32 u32_i_order_lo;
    ts_u32 u32_o_order_hi;
    ts_u32 u32_o_order_lo;

    /* ex vsync */
    ts_u08 u08_ex_vsync_en;
    ts_u08 u08_ex_vsync_mode;
    ts_u16 u16_rev;

	ts_f32 f32_ex_vsync_auto_gen_fps;

    /* 校正属性 */
    ts_viu_corr_attr_s_t st_corr_attr;

    /* comtimize config */
    ts_viu_snr_timing_attr_s_t st_timing_attr;
} ts_viu_comm_attr_s_t;


/* correct 复位配置 */
typedef struct ts_viu_corr_rst_s {
    ts_u08 u08_reset_val;
    ts_u08 u08_reset_only;
    ts_u16 u16_rev;
} ts_viu_corr_rst_s_t;


/* 面阵均值 */
typedef struct ts_viu_array_info_s {
    ts_u64 u64_sum;
    ts_u32 u32_mean;
    ts_u16 u16_width;
    ts_u16 u16_height;
} ts_viu_array_info_s_t;

/* 帧中断回调，用于精确帧计数等 */
typedef struct ts_viu_frame_cb_s {
    /* 回调函数 */
    ts_void (*frame_cb)(ts_void *);

    /* 回调函数的入参 */
    ts_void *pv_param;
} ts_viu_frame_cb_s_t;


/* 导出一帧数据 */
typedef struct ts_viu_fb_attr_s {
    

} ts_viu_fb_attr_s_t;

/* --- function list --- */

/* 
 * 函数名 :  ts_tdk_viu_set_comm_attr
 * 描  述 :  设置 viu 的公共属性
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_comm_attr       viu 属性指针，内存由调用者申请
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_ENULLPTR        参数为空
 *          -TS_ENOTSUP         不支持此探测器
 *          -TS_ENOMEM          申请内存失败
 */
ts_s32 ts_tdk_viu_set_comm_attr(ts_mdev_t h_viu, const ts_viu_comm_attr_s_t *pst_comm_attr);


/* 
 * 函数名 :  ts_tdk_viu_get_comm_attr
 * 描  述 :  获取 viu 的公共属性
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          pst_comm_attr       viu 属性指针，内存由调用者申请
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_ENULLPTR        参数为空
 */
ts_s32 ts_tdk_viu_get_comm_attr(ts_mdev_t h_viu, ts_viu_comm_attr_s_t *pst_comm_attr);


/* 
 * 函数名 :  ts_tdk_viu_set_snr_attr
 * 描  述 :  设置指定 viu 通道的探测器属性
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_snr_attr        探测器特定属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_ENULLPTR        参数为空
 *          -TS_ENOTREADY       通道未准备，未配置
 *          -TS_ENOTSUP         不支持的探测器型号
 */
ts_s32 ts_tdk_viu_set_snr_attr(ts_mdev_t h_viu, const ts_viu_snr_attr_s_t *pst_snr_attr);


/* 
 * 函数名 :  ts_tdk_viu_get_snr_attr
 * 描  述 :  获取指定 viu 通道的探测器属性
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          pst_snr_attr        探测器特定属性
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_ENULLPTR        参数为空
 */
ts_s32 ts_tdk_viu_get_snr_attr(ts_mdev_t h_viu, ts_viu_snr_attr_s_t *pst_snr_attr);


/* 
 * 函数名 :  ts_tdk_viu_get_snr_type
 * 描  述 :  获取指定 viu 通道的探测器属性
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          pen_snrt            探测器特定属性
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_ENULLPTR        参数为空
 */
ts_s32 ts_tdk_viu_get_snr_type(ts_mdev_t h_viu, ts_viu_snrt_en_t *pen_snrt);


/* 
 * 函数名 :  ts_tdk_viu_set_fix_attr
 * 描  述 :  设置修订参数，用于发布包补丁，暂未实现
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pv_fix_attr         修订配置
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          -TS_ENOTSUP         不支持
 */
ts_s32 ts_tdk_viu_set_fix_attr(ts_mdev_t h_viu, const ts_void *pv_fix_attr);


/* 
 * 函数名 :  ts_tdk_viu_get_fix_attr
 * 描  述 :  获取修订参数
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          pv_fix_attr         修订配置
 * 返  回 :  返回值             描述
 *          -TS_ENOTSUP         不支持
 */
ts_s32 ts_tdk_viu_get_fix_attr(ts_mdev_t h_viu, ts_void *pv_fix_attr);


/* 
 * 函数名 :  ts_tdk_viu_enable
 * 描  述 :  使能指定 viu 通道
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              使能成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 *          -TS_EBUSY           设备正忙
 *          -TS_ENOTREADY       探测器未被正确配置过
 */
ts_s32 ts_tdk_viu_enable(ts_mdev_t h_viu);


/* 
 * 函数名 :  ts_tdk_viu_disable
 * 描  述 :  关闭指定 viu 通道, 当 ref 为 0 时会关闭设备文件, 但不实际关闭探测器输入
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              使能成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 */
ts_s32 ts_tdk_viu_disable(ts_mdev_t h_viu);


/* 
 * 函数名 :  ts_tdk_viu_exp_raw
 * 描  述 :  从 VIU 获取一帧 raw 数据，暂未实现
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          u32_ms              超时时间
 * 输  出 :  参数               描述
 *          pst_src             raw 数据缓冲句柄
 * 返  回 :  返回值             描述
 *          -TS_ENOTSUP         不支持
 */
ts_s32 ts_tdk_viu_exp_raw(ts_mdev_t h_viu, ts_buffer_s_t *pst_dst, ts_u32 u32_ms);


/* 
 * 函数名 :  ts_tdk_viu_release
 * 描  述 :  释放 VIU 获取一帧的 raw 帧存
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_src             raw 数据缓冲句柄
 * 输  出 :  参数               描述
 *            无                无
 * 返  回 :  返回值             描述
 *          -TS_ENOTSUP         不支持
 */
ts_s32 ts_tdk_viu_release(ts_mdev_t h_viu, ts_buffer_s_t *pst_dst);


/* 
 * 函数名 :  ts_tdk_viu_get_correct_buf
 * 描  述 :  获取当前 ooc 使用中的缓冲，包括缓冲地址和大小
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *           pst_buf            ooc 缓存信息
 * 返  回 :  返回值             描述
 *           TS_OK              获取成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENOTREADY       设备未准备好
 *          -TS_ENULLPTR        pst_buf 为空指针
 *           TS_ERR             获取失败
 */
ts_s32 ts_tdk_viu_get_correct_buf(ts_mdev_t h_viu, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_viu_get_correct_free_buf
 * 描  述 :  获取当前空闲的 ooc 缓冲，包括缓冲地址和大小
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *           pst_buf            ooc 缓存信息
 * 返  回 :  返回值             描述
 *           TS_OK              获取成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENOTREADY       设备未准备好
 *          -TS_ENULLPTR        pst_buf 为空指针
 *           TS_ERR             获取失败
 */
ts_s32 ts_tdk_viu_get_correct_free_buf(ts_mdev_t h_viu, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_viu_set_correct_buf
 * 描  述 :  将用户准备好的 ooc 缓冲配置给 VIU
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_buf            ooc 缓存信息
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENOTREADY       设备未准备好
 *          -TS_ENULLPTR        pst_buf 为空指针
 */
ts_s32 ts_tdk_viu_set_correct_buf(ts_mdev_t h_viu, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_viu_exec_correct
 * 描  述 :  开启指定 viu 通道的 ooc 功能
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_rst_attr        ooc 重新初始化配置, 如果为空，则使用之前的配置
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              使能成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENODEV          设备打开错误
 */
ts_s32 ts_tdk_viu_exec_correct(ts_mdev_t h_viu, ts_viu_corr_rst_s_t *pst_rst_attr);


/* 
 * 函数名 :  ts_tdk_viu_get_mean
 * 描  述 :  获取面阵均值以及计算信息
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *           pst_info            面阵均值数据
 * 返  回 :  返回值             描述
 *           TS_OK              获取成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENOTREADY       设备未准备好
 *          -TS_ENULLPTR        pst_info 为空指针
 */
ts_s32 ts_tdk_viu_get_mean(ts_mdev_t h_viu, ts_viu_array_info_s_t *pst_info);


/* 
 * 函数名 :  ts_tdk_viu_get_stats
 * 描  述 :  获取指定坐标的行、列、点统计信息
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          u32_type            获取信息的类型，取值范围: TS_VIU_STATS_COL, TS_VIU_STATS_ROW, TS_VIU_STATS_DOT
 *          u16_coor_x          x 坐标，取值范围 0 到探测器输入实际宽度
 *          u16_coor_y          y 坐标，取值范围 0 到探测器输入实际高度
 * 输  出 :  参数               描述
 *          pu32_sum            累计和，如果设置为 ts_null, 则不返回此项
 *          pu32_mean           均值，如果设置为 ts_null, 则不返回此项
 * 返  回 :  返回值             描述
 *           TS_OK              获取成功
 *          -TS_EINVALDEVID     通道句柄错误
 *          -TS_ENOTREADY       设备未准备好
 *          TS_ERR              其他错误
 */
ts_s32 ts_tdk_viu_get_stats(ts_mdev_t h_viu, ts_u32 u32_type, ts_u16 u16_coor_x, ts_u16 u16_coor_y, \
                ts_u32 *pu32_sum, ts_u32 *pu32_mean);


/* 
 * 函数名 :  ts_tdk_viu_set_frm_cb
 * 描  述 :  设置 viu 每帧中断回调
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_cb_attr         回调参数         
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置成功
 *          TS_ERR              设置失败
 */            
ts_s32 ts_tdk_viu_set_frm_cb(ts_mdev_t h_viu, ts_viu_frame_cb_s_t *pst_cb_attr);


/* 
 * 函数名 :  ts_tdk_viu_reset_fb
 * 描  述 :  复位当前的 viu 帧存
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               复位成功
 */
ts_s32 ts_tdk_viu_reset_fb(ts_mdev_t h_viu);


/* 
 * 函数名 :  ts_tdk_viu_get_fb
 * 描  述 :  从 VIU 获取一个空闲的帧存
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          u32_ms              超时时间
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               获取成功
 *          -TS_EINVAL          无效入参
 *          -TS_ENOINIT         viu 未正确初始化
 *          -TS_ENOMEM          无法获取到空闲帧存
 */
ts_s32 ts_tdk_viu_get_fb(ts_mdev_t h_viu, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_viu_set_fb
 * 描  述 :  将初始化好的帧存设置到 viu
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          pst_buf             帧存句柄
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               获取成功
 *          -TS_EINVAL          无效入参
 *          -TS_EBUSY           帧存处于忙状态
 *          -TS_ENOINIT         viu 未正确初始化
 *          -TS_ENOMEM          无法获取到空闲帧存
 */
ts_s32 ts_tdk_viu_set_fb(ts_mdev_t h_viu, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_viu_exvsync
 * 描  述 :  VIU 输出同步信号，向后级模块输出当前帧存内的图像数据
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          u32_force_en        无视帧存情况，强制发送当前已有地址上的数据
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               获取成功
 *          -TS_ENOINIT         viu 未正确初始化
 *          TS_ERR              同步信号发送失败
 */
ts_s32 ts_tdk_viu_exvsync(ts_mdev_t h_viu, ts_u32 u32_force_en);


/* 
 * 函数名 :  ts_tdk_viu_set_fix_occ
 * 描  述 :  设置一个 occ 强制修正系数，不推荐使用, > 0 生效
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 *          u32_fix_thr         修正系数，各个探测器值各不相同
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置成功
 *          TS_ERR              设置失败
 */
ts_s32 ts_tdk_viu_set_fix_occ(ts_mdev_t h_viu, ts_u32 u32_fix_thr);


/* 
 * 函数名 :  ts_tdk_viu_get_fix_occ
 * 描  述 :  获取 occ 强制修正系数
 * 输  入 :  参数               描述
 *          h_viu               viu 设备号，定义请参看 chip_def.h
 * 输  出 :  参数               描述
 *          pu32_fix_thr         修正系数，各个探测器值各不相同无
 * 返  回 :  返回值             描述
 *          TS_OK               获取成功
 *          TS_ERR              获取失败
 */
ts_s32 ts_tdk_viu_get_fix_occ(ts_mdev_t h_viu, ts_u32 *pu32_fix_thr);


/* 
 * 函数名 :  ts_tdk_viu_get_snr_sn
 */
ts_s32 ts_tdk_viu_get_snr_sn(ts_mdev_t h_viu, ts_u32 *pu32_snr_sn);


/* 
 * 函数名 :  ts_tdk_viu_rst_snr
 */
ts_s32 ts_tdk_viu_rst_snr(ts_mdev_t h_viu, ts_u32 u32_rst_en);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_VIU_V100_H__ */

