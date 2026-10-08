/* 
 * gpio_v100.h -- gpio v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_GPIO_V100_H__
#define __API_GPIO_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>

#include <cpl/cpl_ioctl.h>


/* gpio 中断回调函数定义 */
typedef ts_void (*ts_gpio_irqcb_t) (ts_void);

/* gpio 方向枚举 */
typedef enum ts_gpio_dir_en {
    TS_GPIO_DIR_INPUT = 0,
    TS_GPIO_DIR_OUTPUT = 1,

    TS_GPIO_DIR_BUTT
} ts_gpio_dir_en_t;

/* gpio 摆率 */
typedef enum ts_gpio_sr_en {
    TS_GPIO_SR_SLOW = 0,
    TS_GPIO_SR_FAST = 1,

    TS_GPIO_SR_BUTT
} ts_gpio_sr_en_t;


/* gpio 中断配置属性 */
typedef struct ts_gpio_irq_attr_s {
    /* 管脚序号 */
    ts_u32 u32_pin_no;

    /* 中断使能 */
    ts_option_en_t en_irqen;

    /* 电平中断类型 */
    ts_elec_level_en_t en_irq_type;

    /* 中断回调函数 */
    ts_gpio_irqcb_t fp_irqcb;
} ts_gpio_irq_attr_s_t;

/* gpio 基础属性 */
typedef struct ts_gpio_attr_s {
    /* 管脚序号 */
    ts_u32 u32_pin_no;

    /* 管脚方向 */
    ts_gpio_dir_en_t en_dir;

    /* 消抖使能 */
    ts_option_en_t en_debounce_en;

    /* 初始值，输出方向有效 */
    ts_elec_level_en_t en_lvl;
} ts_gpio_attr_s_t;


#define TS_EGPIODIRECTION             TS_DEFINE_ERRNO(TS_LOG_ERR, TS_IOC_MAGIC_GPIO, 0x01)
#define TS_EGPIODIRECTION_STR         "Wrong direction"

/* API */

/* 
 * 函数名 :  ts_lib_gpio_init
 * 描  述 :  初始化 GPIO 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
extern ts_s32 ts_lib_gpio_init(ts_void);


/* 
 * 函数名 :  ts_lib_gpio_deinit
 * 描  述 :  GPIO 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
extern ts_s32 ts_lib_gpio_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_gpio_set_mux
 * 描  述 :  设置 GPIO 管脚复用关系， u32_ioname 和 u32_func 请查阅 iomux.h 定义
 * 输  入 :  参数           描述
 *          u32_ioname      管脚名称定义
 *          u32_func        管脚功能定义
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_gpio_set_mux(ts_u32 u32_ioname, ts_u32 u32_func);


/* 
 * 函数名 :  ts_lib_gpio_set_pull
 * 描  述 :  设置 GPIO 上下拉， u32_ioname 请查阅 iomux.h 定义；
 *             支持上拉(TS_ELEC_LVL_HI), 下拉(TS_ELEC_LVL_LO), 高阻，既不上拉也不下拉(TS_ELEC_LVL_Z);
 * 输  入 :  参数           描述
 *          u32_ioname      管脚名称定义
 *          en_lvl          电平定义，请查阅 ts_comm.h 中定义
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_gpio_set_pull(ts_u32 u32_ioname, ts_elec_level_en_t en_lvl);


/* 
 * 函数名 :  ts_lib_gpio_set_o_strong
 * 描  述 :  设置 GPIO 输出强度， 强度范围 [0..3]
 * 输  入 :  参数           描述
 *          u32_ioname      管脚名称定义
 *          u32_str         输出强度，范围 0..3,对应 4,8,12,16ma
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_gpio_set_o_strong(ts_u32 u32_ioname, ts_u32 u32_str);


/* 
 * 函数名 :  ts_lib_gpio_set_sr
 * 描  述 :  GPIO 摆率控制， TS_GPIO_SR_SLOW 和 TS_GPIO_SR_FAST 可配
 * 输  入 :  参数           描述
 *          u32_ioname      管脚名称定义
 *          en_sr           摆率快慢配置
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_gpio_set_sr(ts_u32 u32_ioname, ts_gpio_sr_en_t en_sr);


/* 
 * 函数名 :  ts_lib_gpio_set_schmitt
 * 描  述 :  GPIO 施密特触发器使能
 * 输  入 :  参数           描述
 *          u32_ioname      管脚名称定义
 *          en_opt          enable or disable, 参见 ts_comm.h
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_gpio_set_schmitt(ts_u32 u32_ioname, ts_option_en_t en_opt);


/* 
 * 函数名 :  ts_lib_gpio_set_attr
 * 描  述 :  设置 GPIO 属性， 管脚按照编号配置，如 GPIO58 即配置成 58
 * 输  入 :  参数           描述
 *           pst_attr       管脚属性
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置属性成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_ENULLPTR     参数为 ts_null
 */
extern ts_s32 ts_lib_gpio_set_attr(ts_gpio_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_lib_gpio_set_out
 * 描  述 :  设置 GPIO 为输出方向, ts_lib_gpio_set_attr 函数的再封装
 * 输  入 :  参数           描述
 *          s32_gpio_num    管脚按照编号配置，如 GPIO58 即配置成 58
 *          en_o_def        初始电平
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置输出成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_ENULLPTR     参数为 ts_null
 */
extern ts_s32 ts_lib_gpio_set_out(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_def);


/* 
 * 函数名 :  ts_lib_gpio_set_in
 * 描  述 :  设置 GPIO 为输入方向, ts_lib_gpio_set_attr 函数的再封装
 * 输  入 :  参数           描述
 *          s32_gpio_num    管脚按照编号配置，如 GPIO58 即配置成 58
 *          en_debounce_en  是否开启消抖
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置输入成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_ENULLPTR     参数为 ts_null
 */
extern ts_s32 ts_lib_gpio_set_in(ts_s32 s32_gpio_num, ts_option_en_t en_debounce_en);


/* 
 * 函数名 :  ts_lib_gpio_set_irq_attr
 * 描  述 :  设置 GPIO 中断属性， 管脚按照编号配置，如 GPIO58 即配置成 58
 * 输  入 :  参数           描述
 *           pst_irq_attr   管脚中断属性
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置属性成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_ENULLPTR     参数为 ts_null
 */
extern ts_s32 ts_lib_gpio_set_irq_attr(ts_gpio_irq_attr_s_t *pst_irq_attr);


/* 
 * 函数名 :  ts_lib_gpio_read_pin
 * 描  述 :  读取指定编号管脚电平， 管脚按照序号配置，如 GPIO58 即配置成 58
 * 输  入 :  参数           描述
 *           u32_pin_no     管脚编号
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           0              管脚上为低电平
 *           1              管脚上为高电平
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  管脚编号错误，无对应 GPIO 组
 */
extern ts_s32 ts_lib_gpio_read_pin(ts_u32 u32_pin_no);


/* 
 * 函数名 :  ts_lib_gpio_write_pin
 * 描  述 :  写指定编号管脚， 管脚按照序号配置，如 GPIO58 即配置成 58
 * 输  入 :  参数           描述
 *          u32_pin_no      管脚编号
 *          en_lvl          电平，支持 TS_ELEC_LVL_LO 和 TS_ELEC_LVL_HI, 参见 ts_comm.h
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          写成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  管脚编号错误，无对应 GPIO 组
 */
extern ts_s32 ts_lib_gpio_write_pin(ts_u32 u32_pin_no, ts_elec_level_en_t en_lvl);


/* 
 * 函数名 :  ts_lib_gpio_flip_pin
 * 描  述 :  翻转指定编号管脚， 管脚按照序号配置，如 GPIO58 即配置成 58
 * 输  入 :  参数           描述
 *          u32_pin_no      管脚编号
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          写成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  管脚编号错误，无对应 GPIO 组
 */
extern ts_s32 ts_lib_gpio_flip_pin(ts_u32 u32_pin_no);


/* 
 * 函数名 :  ts_lib_gpio_read_grp
 * 描  述 :  读取指定 GPIO 组，可设定感兴趣 PIN 脚; 如 u32_pin_idx = TS_GPIO_BANK0_0 | TS_GPIO_BANK0_14;
 *              如果全部读取，则 u32_pin_idx 配置成 0xFFFFFFFF
 * 输  入 :  参数           描述
 *          u32_grp_idx     GPIO 组序号
 *          u32_pin_idx     感兴趣 PIN 序号，定义参见 chip_def.h
 * 输  出 :  参数           描述
 *           pu32_val        (GPIO 组数据 & u32_pin_idx) 后的返回值
 * 返  回 :  返回值         描述
 *           TS_OK          读成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  无对应 GPIO 组
 *          -TS_ENULLPTR     pu32_val 为 ts_null
 */
extern ts_s32 ts_lib_gpio_read_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx, ts_u32 *pu32_val);


/* 
 * 函数名 :  ts_lib_gpio_write_grp
 * 描  述 :  写指定 GPIO 组，可设定感兴趣 PIN 脚; 如 u32_pin_idx = TS_GPIO_BANK0_0 | TS_GPIO_BANK0_14;
 *              如果全部写入，则 u32_pin_idx 配置成 0xFFFFFFFF
 * 输  入 :  参数           描述
 *          u32_grp_idx     GPIO 组序号
 *          u32_pin_idx     感兴趣 PIN 序号，定义参见 chip_def.h
 *          u32_val         写入值， 可以配置成 (TS_GPIO_BANK0_0 | TS_GPIO_BANK0_14)
 * 输  出 :  参数           描述
 *           pu32_val        (GPIO 组数据 & u32_pin_idx) 后的返回值
 * 返  回 :  返回值         描述
 *           TS_OK          读成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  无对应 GPIO 组
 *          -TS_ENULLPTR     pu32_val 为 ts_null
 */
extern ts_s32 ts_lib_gpio_write_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx, ts_u32 u32_val);


/* 
 * 函数名 :  ts_lib_gpio_flip_pin
 * 描  述 :  翻转指定 GPIO 组， 可设定感兴趣 PIN 脚; 如 u32_pin_idx = TS_GPIO_BANK0_0 | TS_GPIO_BANK0_14;
 * 输  入 :  参数           描述
 *          u32_pin_no      管脚编号
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          写成功
 *          -TS_ENOINIT      GPIO 库未初始化
 *          -TS_EINVALGRPID  管脚编号错误，无对应 GPIO 组
 */
ts_s32 ts_lib_gpio_flip_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_GPIO_V100_H__ */

