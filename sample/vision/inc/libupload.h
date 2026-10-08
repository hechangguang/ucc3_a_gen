/*
 * libupload.h -- tdk 使用 upload 模块导出 y8/y16 数据进行校正等计算例子, 类驱动态，避开 ioctl 以及直接使用驱动接口头文件
 */
#ifndef __LIB_UPLOAD_H__
#define __LIB_UPLOAD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif

#endif /* __cplusplus */
#include <base/ts_type.h>
#include <uapi/upl_v100.h>


typedef enum ts_upload_type_en {
    /* 获取一帧 */
    TS_UPT_SINGLE_FRAME = 0,

    /* 获取全部 start 时指定的帧数 */
    TS_UPT_ALL_FRAME = 1,

    TS_UPT_BUTT
} ts_upload_type_en_t;


/*
 * 函数名 :  ts_lib_up_start
 * 描  述 :  启动指定帧数的 y8/y16 数据导出, 如果后续一次性获取全部缓冲，需要配置 (u32_frm_cnt + 1) 大小
 *             的总缓冲，如果单帧获取，需确保计算速度(还回帧存)与填充速度可以避免填充阻塞。
 * 输  入 :  参数           描述
 *          u32_up_pos       0,1,2
 *          u32_frm_cnt     导出的帧数
 *          u32_node_cnt    需要配置的节点个数，需要与 (pu32_buf / u32_frm_size) 匹配
 *          s32_is_dma      是否为 DMA 内存空间，如果是，后续不会进行 DMA MAP 和 UNMAP 操作
 * 输  出 :  参数           描述
 *           pu32_buf       视频数据
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_s32 ts_lib_up_start(ts_u32 u32_up_pos, ts_u32 *pu32_buf, ts_u32 u32_frm_cnt, \
            ts_u32 u32_node_cnt, ts_s32 s32_is_dma);

/*
 * 函数名 :  ts_lib_up_get_buf
 * 描  述 :  获取一帧数据
 * 输  入 :  参数           描述
 *          u32_tmout_ms   超时时间
 * 输  出 :  参数           描述
*           无             无
 * 返  回 :  返回值         描述
 *           ts_null       获取失败
 *          !ts_null       获取的首帧节点句柄
 */
extern ts_nodebuf_s_t *ts_lib_up_get_node(ts_u32 u32_tmout_ms);

/*
 * 函数名 :  ts_lib_up_get_buf
 * 描  述 :  获取指定帧数的准备好的缓冲节点
 * 输  入 :  参数           描述
 *          en_up_type     本次获取帧的类型，单帧或者全部获取
 *          u32_tmout_ms   超时时间
 * 输  出 :  参数           描述
*           无             无
 * 返  回 :  返回值         描述
 *           ts_null       获取失败
 *          !ts_null       获取的首帧节点句柄
 */
extern ts_void ts_lib_up_release_node(ts_nodebuf_s_t *pst_node);

/*
 * 函数名 :  ts_lib_up_stop
 * 描  述 :  结束本次 upload
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_s32 ts_lib_up_stop(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LIB_UPLOAD_H__ */