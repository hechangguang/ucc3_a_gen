/* 
 * osd_v100.h -- osd v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_OSD_V100_H__
#define __API_OSD_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_media.h>

/* 定义 OSD 类型 */

/* 普通 OSD */
#define TS_OSD_TYPE_OSD             (0x00)

/* 定制化的中心点可闪烁的十字光标 */
#define TS_OSD_TYPE_CURSOR_BLINK    (0x01)

/* 定制化的中心为矩形的光标  */
#define TS_OSD_TYPE_CURSOR_RECT     (0x02)

/* 定制化的矩形框 */
#define TS_OSD_TYPE_RECT            (0x03)

/* 刷新规则 */
typedef enum ts_osd_refresh_en {
    /* 只将不可见的 osd 节点显示出来, 废弃，等同于 TS_OSD_REFRESH_ALL_OFF */
    TS_OSD_REFRESH_HIDE_ON = 0,

    /* 只将可见的 osd 节点显示出来 */
    TS_OSD_REFRESH_VISIBLE_ON = 1,

    /* 将全部本 osd 设备的节点显示出来，不会更改节点本身的可见属性，废弃，等同于 TS_OSD_REFRESH_ALL_OFF */
    TS_OSD_REFRESH_ALL_ON = 2,

    /* 关闭所有 osd 节点的显示，不会更改节点本身的可见属性 */
    TS_OSD_REFRESH_ALL_OFF = 3,

    TS_OSD_REFRESH_BUTT
} ts_osd_refresh_en_t;


/* 存 osd 显示属性 */
typedef struct ts_osd_cfg_s {
    /* 透明度, 0..100 ， 100为完全透明， 0 则完全不透明, -1 则使用系统颜色透明度 */
    ts_s08 s08_alpha;

    /* 自动反色， 0 ： 禁用， 1 ： 使能 */
    ts_u08 u08_auto_invert;

    /* 字体颜色 */
    ts_u08 u08_font_color;

    /* 背景颜色 */
    ts_u08 u08_bg_color;

    /* 用于偏移显示，仅适用于高度方向。基于第 0 行 */
    ts_u16 u16_start_h;
    ts_u16 u16_end_h;
} ts_osd_cfg_s_t;


typedef struct ts_misc_cfg_s {
    /* osd 组合成带闪十字光标时有效， 0 关闭闪烁， > 0 为闪烁间隔(单位为帧数) */
    ts_u08 u08_blink;

    /* 中心点是否隐藏，仅限于 blink 类型 */
    ts_u08 u08_center_hide;

    /* 横向、纵向方向的留空大小 */
    ts_u08 u08_margin_h;
    ts_u08 u08_margin_v;

    /* 十字光标的横向、纵向光标长度 */
    ts_u08 u08_len_h;
    ts_u08 u08_len_v;

    /* 仅对矩形框有效， > 0 的情况下，对矩形框做四周挖空 */
    ts_u08 u08_brk_len;

    /* 是否在画中画中支持十字光标, 仅支持灰度，计算显示范围按照 viu 输入范围计算 */
    ts_u08 u08_pip_en;
} ts_misc_cfg_s_t;


/* osd attr */
#define OSD_ATTR_CFG_NUM            (2)
typedef struct ts_osd_attr_s {
    /* osd 起始位置和宽高 */
    ts_u16 u16_left;
    ts_u16 u16_top;
    ts_u16 u16_width;
    ts_u16 u16_height;

    /* 像素位宽设置, 取值范围 2,4,16,256 */
    ts_u08 u08_pix_bw;

    /* 是否可见 0: 隐藏 */
    ts_u08 u08_visible;

    /* 覆盖优先级，作为覆盖或者被覆盖的判断依据， 0 优先级最低 */
    ts_u08 u08_priority;

    /* 配置类型 */
    ts_u08 u08_type;

    union {
        ts_osd_cfg_s_t st_osd_cfg;
        ts_misc_cfg_s_t st_misc_cfg;
        ts_u32 au32_cfg[OSD_ATTR_CFG_NUM];
    };
} ts_osd_attr_s_t;


/* 通道属性 */
typedef struct ts_osd_ch_attr_s {
    /* 调色板， RGB + 透明度 */
    const ts_rgb_s_t *pst_rgb;

    /* 调色板个数 */
    ts_u16 u16_rgb_num;

    /* 调色板像素位宽, 不能超过调色板个数, 如调色板个数为 2 ，像素位宽只能是 1BIT */
    ts_u08 u08_pixel_bw;

    /* osd 重叠，额外扩充的部分填充的像素数值 */
    ts_u08 u08_fill_pixel;

    /* osd 重叠进行软件合并的内存空间大小，如果设置成 0 , 则禁止合并,
     * > 0 : 驱动将会申请两片此大小的空间作为合并双缓冲.
     */
    ts_u32 u32_merge_size;
} ts_osd_ch_attr_s_t;


/* --- function list --- */

/* 
 * 函数名 :  ts_tdk_osd_set_ch_attr
 * 描  述 :  设置 osd 设备属性，只能设置一次，对全部通道生效
 * 输  入 :  参数               描述
 *          pst_ch_attr         设备属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *          -TS_ENULLPTR        入参无效
 *          -TS_ENODEV          打开设备失败或者设备不存在
 */
ts_s32 ts_tdk_osd_set_ch_attr(const ts_osd_ch_attr_s_t *pst_ch_attr);


/* 
 * 函数名 :  ts_tdk_osd_get_ch_attr
 * 描  述 :  获取 osd 设备属性
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *          pst_ch_attr         设备属性
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *          -TS_ENULLPTR        入参无效
 */
ts_s32 ts_tdk_osd_get_ch_attr(ts_osd_ch_attr_s_t *pst_ch_attr);


/* 
 * 函数名 :  ts_tdk_osd_create_node
 * 描  述 :  创建一个 osd 节点，用于填充用户 osd 数据
 * 输  入 :  参数               描述
 *          h_osd_dev           osd 设备句柄，如果为 0, 则不绑定设备, 反之则自动绑定到指定层设备
 *          pst_osd_attr        单个 osd 节点的属性配置
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            创建失败
 *           !ts_null           创建成功
 */
ts_nodebuf_s_t *ts_tdk_osd_create_node(ts_mdev_t h_osd_dev, const ts_osd_attr_s_t *pst_osd_attr);


/* 
 * 函数名 :  ts_tdk_osd_destroy_node
 * 描  述 :  销毁一个节点
 * 输  入 :  参数               描述
 *          pst_hosd           单个 osd 节点的句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              销毁成功
 *          -TS_ENULLPTR        入参无效
 *          -TS_ENODEV          打开设备失败或者设备不存在
 *          -TS_ENOTSUP         不支持此功能
 */
ts_s32 ts_tdk_osd_destroy_node(ts_nodebuf_s_t *pst_hosd);


/* 
 * 函数名 :  ts_tdk_osd_set_attr
 * 描  述 :  设置一个节点的属性配置，宽高、像素位宽配置不到大于创建时的宽高、像素位宽等计算后申请的缓冲大小。
 * 输  入 :  参数               描述
 *          h_osd_dev           指定显示设备
 *          pst_hosd            单个 osd 节点的句柄
 *          pst_osd_attr        单个 osd 节点的属性配置
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              读取成功
 *          -TS_ENULLPTR        入参无效
 */
ts_s32 ts_tdk_osd_set_attr(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd, ts_osd_attr_s_t *pst_osd_attr);


/* 
 * 函数名 :  ts_tdk_osd_get_attr
 * 描  述 :  获取一个节点的属性配置
 * 输  入 :  参数               描述
 *          h_osd_dev           指定显示设备
 *          pst_hosd            单个 osd 节点的句柄
 * 输  出 :  参数               描述
 *          pst_osd_attr        单个 osd 节点的属性配置
 * 返  回 :  返回值             描述
 *           TS_OK              读取成功
 *          -TS_ENULLPTR        入参无效
 */
ts_s32 ts_tdk_osd_get_attr(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd, ts_osd_attr_s_t *pst_osd_attr);


/* 
 * 函数名 :  ts_tdk_osd_bind
 * 描  述 :  将指定的 osd 节点绑定到一个 osd 设备上去
 * 输  入 :  参数               描述
 *          h_osd_dev           osd 设备句柄
 *          pst_hosd            osd 节点句柄
 *          u32_visible         是否可见，如果设置成 0 ， 则会此节点处于隐藏状态(hide)
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              绑定成功
 *          -TS_EINVALDEVID     设备句柄错误
 *          -TS_ENULLPTR        入参无效
 *          -TS_ENODEV          打开设备失败或者设备不存在
 */
ts_s32 ts_tdk_osd_bind(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd, ts_u32 u32_visible);


/* 
 * 函数名 :  ts_tdk_osd_unbind
 * 描  述 :  将指定的 osd 从 osd 设备解绑
 * 输  入 :  参数               描述
 *          h_osd_dev           从指定设备从解绑
 *          pst_hosd            指定 osd 节点的句柄
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              绑定成功
 *          -TS_EINVALDEVID     设备句柄错误
 *          -TS_ENULLPTR        入参无效
 *          -TS_ENODEV          打开设备失败或者设备不存在
 */
ts_s32 ts_tdk_osd_unbind(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd);


/* 
 * 函数名 :  ts_tdk_osd_update
 * 描  述 :  类似双缓冲，将旧的 osd 显示内容更换到新的 osd 显示内容
 * 输  入 :  参数               描述
 *          pst_old_hosd        被替换的 osd 节点
 *          pst_new_hosd        新准备的 osd 节点
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              绑定成功
 *          -TS_EINVALDEVID     设备句柄错误
 *          -TS_ENULLPTR        入参无效
 *          -TS_ENODEV          打开设备失败或者设备不存在
 *          -TS_EUNBIND         未绑定
 */
ts_s32 ts_tdk_osd_update(ts_nodebuf_s_t *pst_old_hosd, ts_nodebuf_s_t *pst_new_hosd);


/* 
 * 函数名 :  ts_tdk_osd_refresh_display
 * 描  述 :  刷新当前输出到显示图层的 osd 节点
 * 输  入 :  参数               描述
 *          en_disp_mode        参见 ts_osd_refresh_en_t 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              刷新成功
 *          -TS_EINVALDEVID     设备句柄错误
 *          -TS_ENODEV          打开设备失败或者设备不存在
 *          -TS_ERR             其他错误
 */
ts_s32 ts_tdk_osd_refresh_display(ts_osd_refresh_en_t en_disp_mode);


/* 
 * 函数名 :  ts_tdk_osd_set_palette
 * 描  述 :  设置调色板，需要关闭显示后再调用, 仅用于 rgb 格式
 * 输  入 :  参数               描述
 *          pst_rgb             调色板句柄
 *          u32_num             调色板颜色个数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *           -TS_ENOINIT        osd 设备未初始化
 *          -TS_EINVAL          u32_num 为 0
 *          -TS_ENULLPTR        pst_rgb 为空
 */
ts_s32 ts_tdk_osd_set_palette(const ts_rgb_s_t *pst_rgb, ts_u32 u32_num);


/* 
 * 函数名 :  ts_tdk_osd_set_palette_yuv
 * 描  述 :  设置调色板，需要关闭显示后再调用, 仅用于 yuv 格式
 * 输  入 :  参数               描述
 *          pst_yuv             调色板句柄
 *          u32_num             调色板颜色个数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *           -TS_ENOINIT        osd 设备未初始化
 *          -TS_EINVAL          u32_num 为 0
 *          -TS_ENULLPTR        pst_yuv 为空
 */
ts_s32 ts_tdk_osd_set_palette_yuv(const ts_yuv_s_t *pst_yuv, ts_u32 u32_num);


/* 
 * 函数名 :  ts_tdk_osd_get_disp_size
 * 描  述 :  获取指定显示设备的宽高
 * 输  入 :  参数               描述
 *          h_osd_dev           显示句柄
 * 输  出 :  参数               描述
 *          pu32_disp_w         宽
 *          pu32_disp_h         高
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *           -TS_ENOINIT        osd 设备未初始化
 *           -TS_EINVALDEVID    设备不合法
 *           -TS_ENULLPTR        pu32_disp_w 或者 pu32_disp_h 为空
 */
ts_s32 ts_tdk_osd_get_disp_size(ts_mdev_t h_osd_dev, ts_u32 *pu32_disp_w, ts_u32 *pu32_disp_h);


/* 
 * 函数名 :  ts_tdk_osd_set_tmm
 * 描  述 :  此接口可以不使用.
 *          向 osd 设置 tdk 内存管理分块，驱动节点等内存申请将使用指定的 tmm 分块申请;
 *              如果设置成 ts_null, 则按照硬件要求对齐向 tmm 申请
 * 输  入 :  参数               描述
 *          pv_tmm              指定 tmm 分块句柄
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *          -TS_ERR             设置失败
 */
ts_s32 ts_tdk_osd_set_tmm(ts_void *pv_tmm);


/* 
 * 函数名 :  ts_tdk_osd_get_tmm
 * 描  述 :  获取当前 osd 驱动使用的 tmm 分块句柄
 * 输  入 :  参数               描述
 *          无                  无
 * 输  出 :  参数               描述
 *          *pv_tmm             获取到的句柄
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *          -TS_ERR             设置失败
 */
ts_s32 ts_tdk_osd_get_tmm(ts_void **pv_tmm);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_OSD_V100_H__ */

