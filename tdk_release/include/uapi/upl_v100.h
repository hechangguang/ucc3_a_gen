/* 
 * upl_v100.h -- upload 库头文件，用于导 raw 
 */
#ifndef __UPL_V100_H__
#define __UPL_V100_H__

#include <base/ts_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* 主通道，一般用于显示，比如 USB 传输 */
#define TS_ISP_UPL_PRIMARY_CH       (0)

/* 副通道，一般用于导出 raw 计算 */
#define TS_ISP_UPL_SECONDARY_CH     (1)

/* 导出位置定义 */
typedef enum ts_upl_pos_en {
    /* ISP 最前级输入, y14 */
    TS_UPL_POS_RAW = 0,

    /* NUC 输出, y14 */
    TS_UPL_POS_NUC = 1,

    /* SFFC 校正后输出, y14 */
    TS_UPL_POS_SFFC = 2,

    /* AGC 之前和之后 y14 */
    TS_UPL_POS_BEFORE_AGC = 3,
    TS_UPL_POS_AFTER_AGC = 4,

    /* ISP 最终输出, y8 */
    TS_UPL_POS_ISP = 5,

    /* VPO 输出, yuv16 */
    TS_UPL_POS_VPO_YUV = 100,

    /* VPO 输出, y8 */
    TS_UPL_POS_VPO_Y8 = 101,

    TS_UPL_POS_BUTT
} ts_upl_pos_en_t;


typedef struct ts_isp_upload_attr_s {
    /* raw 从指定位置模块导出 */
    ts_u32 u32_module;

    /* 指定通道, 可配置成 TS_ISP_UPL_PRIMARY_CH 和 TS_ISP_UPL_SECONDARY_CH */
    ts_u32 u32_chn;

    /* 导出帧数, 内存分配时 w * h * (2 or 1) * (s32_frame_num + 1) */
    ts_s32 s32_frame_num;

    /* 超时时间 */
    ts_u32 u32_tmout_ms;
} ts_isp_upload_attr_s_t;


/* 函数列表 */

/* 中断完成回调 */
typedef ts_void (*pfn_upload_cb_t)(ts_void *pv_node);

/* 短延时完成回调 */
typedef ts_void (*pfn_sd_cb_t)(ts_void *, ts_u32, ts_u32);

/* 
 * 函数名 :  tdl_isp_upload_init
 * 描  述 :  初始化 upload 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *          TS_ERR          初始化失败
 */
ts_s32 tdl_isp_upload_init(ts_void);


/* 
 * 函数名 :  tdl_isp_upload_put_free_node
 * 描  述 :  准备足够的节点缓冲
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 *          pv_node         单个节点句柄
 *          u32_chn         通道
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void tdl_isp_upload_put_free_node(ts_mdev_t h_dev, ts_void *pv_node, ts_u32 u32_chn);


/* 
 * 函数名 :  tdl_isp_run_upload
 * 描  述 :  开始 upload
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 *          pst_attr        配置属性
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          启动成功
 *          -TS_EINVAL      无效参数
 *          -TS_EBUSY       设备正忙
 *          TS_ERR          启动失败
 */
ts_s32 tdl_isp_run_upload(ts_mdev_t h_dev, const ts_isp_upload_attr_s_t *pst_attr);


/* 
 * 函数名 :  tdl_isp_upload_set_cb
 * 描  述 :  设置每帧完成后的回调函数句柄
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 *          u32_chn         通道号, 取 TS_ISP_UPL_PRIMARY_CH or TS_ISP_UPL_SECONDARY_CH
 *          pfn_cb          回调函数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void tdl_isp_upload_set_cb(ts_mdev_t h_dev, ts_u32 u32_chn, pfn_upload_cb_t pfn_cb);


/* 
 * 函数名 :  tdl_isp_upload_set_p_resume
 * 描  述 :  回复主通道
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          恢复成功
 *          -TS_EINVAL      无效参数
 */
ts_s32 tdl_isp_upload_set_p_resume(ts_mdev_t h_dev);


/* 
 * 函数名 :  tdl_isp_upload_set_p_freeze
 * 描  述 :  冻结主通道
 * 输  入 :  参数           描述
 *          h_dev      废弃，可以直接配置成 ts_null
 *          u32_owner       所有者
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          冻结成功
 *          -TS_ETIMEDOUT   冻结超时失败
 */
ts_s32 tdl_isp_upload_set_p_freeze(ts_mdev_t h_dev, ts_u32 u32_owner);


/* 
 * 函数名 :  tdl_isp_get_pixel_info
 * 描  述 :  获取当前的像素信息
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 * 输  出 :  参数           描述
 *          pu32_width      像素宽
 *          pu32_height     像素高
 * 返  回 :  返回值         描述
 *           > 0             像素个数
 */
ts_u32 tdl_isp_get_pixel_info(ts_mdev_t h_dev, ts_u32 *pu32_width, ts_u32 *pu32_height);


/* 
 * 函数名 :  tdl_isp_stop_upload
 * 描  述 :  停止指定通道
 * 输  入 :  参数           描述
 *          h_dev           废弃，可以直接配置成 ts_null
 *          u32_chn         通道号, 取 TS_ISP_UPL_PRIMARY_CH or TS_ISP_UPL_SECONDARY_CH
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          停止成功
 *          TS_ERR          停止失败
 */
ts_s32 tdl_isp_stop_upload(ts_mdev_t h_dev, ts_u32 u32_chn);


/* 
 * 函数名 :  tdl_mupl_get_frm_size
 * 描  述 :  获取 vpo 通道的帧大小，需保证大于等于 isp 输出帧大小
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           pu32_frm_size  帧大小
 * 返  回 :  返回值         描述
 *           TS_OK          停止成功
 *          TS_ERR          停止失败
 */
ts_void tdl_mupl_get_frm_size(ts_u32 *pu32_frm_size);


/* 
 * 函数名 :  tdl_mupl_get_frm_info
 * 描  述 :  获取 vpo 通道的帧宽高
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           pu32_width     宽度
 *          pu32_height     高度
 *          pu32_fps        帧率
 * 返  回 :  返回值         描述
 *           TS_OK          停止成功
 *          TS_ERR          停止失败
 */
ts_void tdl_mupl_get_frm_info(ts_u32 *pu32_width, ts_u32 *pu32_height, ts_u32 *pu32_fps);


/* 
 * 函数名 :  tdl_isp_claim_s_chn
 * 描  述 :  获取副通道的使用权
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void tdl_isp_claim_s_chn(ts_void);


/* 
 * 函数名 :  tdl_isp_release_s_chn
 * 描  述 :  释放副通道的使用权
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void tdl_isp_release_s_chn(ts_void);


/* 以下接口仅用于 uvc 短延时，不开放 */
ts_void tdl_mupl_set_sd_cb(pfn_sd_cb_t pfn_cb);
ts_void tdl_mupl_put_free_ref(ts_void);
ts_s32 tdl_vpo_set_sd_mode(ts_u32 u32_sd_en, ts_u32 u32_pos, ts_void *pv_mem);
ts_s32 tdl_mupl_close(ts_void);
ts_s32 tdl_mupl_open(ts_void);
ts_void tdl_mpul_set_pos(ts_u32 u32_pos);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UPL_V100_H__ */

