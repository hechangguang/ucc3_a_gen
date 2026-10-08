/*
 * bootup_api.h -- bootup 接口部分头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __BOOTUP_API_H__
#define __BOOTUP_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_compiler.h>

#ifndef CONFIG_HOST
extern ts_s32 printf(const ts_char *fmt, ...) ts_printf_like(1, 2);
#endif

/* 注册命令,命令支持最大输出长度为 64 个字节，包括命令本身 */
#define CMDL_REGISTER(name, func) \
    const volatile static CMDL_SECT cmd_info_s_t cmdl_reg_##name = { \
        .pc_name = #name,   \
        .pfn_cmd = func,    \
    };


/* --- function list --- */

/* 
 * 函数名 : cmdl_set_extra_list
 * 描  述 : 向芯片固件设置 hook 额外的命令列表
 * 输  入 : 参数           描述
 *          pst_cmd_list    hook 所注册的命令列表
 *          u32_cnt         命令个数
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
extern ts_void cmdl_set_extra_list(cmd_info_s_t *pst_cmd_list, ts_u32 u32_cnt);


/* 
 * 函数名 : bootup_hook
 * 描  述 : 芯片固件会调用此函数，从而进入 hook 镜像
 * 输  入 : 参数           描述
 *          无             无
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *           -              由用户决定
 */
extern ts_s32 bootup_hook(ts_void);


/*
 * 函数名 : loader_read
 * 描  述 : 用于 hook 等外部调用读 flash 等存储外设
 * 输  入 : 参数        描述
 *          u32_addr    目标地址
 *          u32_size    读大小
 * 输  出 : 参数        描述
 *          pu08_buf    读数据存储缓冲
 * 返  回 : 返回值      描述
 *          TS_ERR      读取失败
 *          TS_OK       读取成功
 *          TS_EIO      不存在加载器
 *          TS_ENULLPTR pu08_buf 为空
 */
extern ts_s32 loader_read(ts_u32 u32_addr, ts_u08 *pu08_buf, ts_u32 u32_size);


/* 
 * 函数名 : tdl_sfc_set_attr
 * 描  述 : 设置 sfc 驱动属性
 * 输  入 : 参数               描述
 *          u32_clk_div         sfc 分频，必须偶数
 *          u32_rx_dly          sfc 读采样延迟拍数
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void tdl_sfc_set_attr(ts_u32 u32_clk_div, ts_u32 u32_rx_dly);


/* 
 * 函数名 : tdl_sfc_config
 * 描  述 : sfc 根据读写类型不同进行配置, 写仅支持单线
 * 输  入 : 参数           描述
 *          u32_lines       采用线制
 *          u32_rw          读或者写
 *          u32_dummy       dummy 设定
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           无             无
 */
extern ts_void tdl_sfc_config(ts_u32 u32_lines, ts_u32 u32_rw, ts_u32 u32_dummy);


/* 
 * 函数名 : tdl_sfc_xfer_stand
 * 描  述 : sfc 单线读,需配合 tdl_sfc_config
 * 输  入 : 参数           描述
 *          pu08_cmd        写命令缓存
 *          u32_cmd_size    写命令个数
 *          u32_rd_size     读数据个数
 * 输  出 : 参数           描述
 *           pu08_rd_buf     读缓存
 * 返  回 : 返回值         描述
 *           TS_OK          读成功
 *           TS_ETIMEDOUT   读超时
 */
extern ts_s32 tdl_sfc_xfer_stand(ts_u08 *pu08_cmd, ts_u32 u32_cmd_size, ts_u08 *pu08_rd_buf, ts_u32 u32_rd_size);


/* 
 * 函数名 : tdl_sfc_xfer_multi
 * 描  述 : sfc dual 或者 quad 读,需配合 tdl_sfc_config
 * 输  入 : 参数           描述
 *          pu32_cmd        写命令缓存
 *          u32_cmd_size    写命令个数
 *          u32_rd_size     读数据个数
 * 输  出 : 参数           描述
 *           pu32_rd_buf     读缓存
 * 返  回 : 返回值         描述
 *           TS_OK          读成功
 *           TS_ETIMEDOUT   读超时
 */
extern ts_s32 tdl_sfc_xfer_multi(ts_u32 *pu32_cmd,  ts_u32 u32_cmd_size, ts_u32 *pu32_rd_buf, ts_u32 u32_rd_size);


/* 
 * 函数名 : tdl_sfc_write_standard
 * 描  述 : sfc 只写单线实现,仅用于 flash 寄存器写，擦除, 数据写等命令,需配合 tdl_sfc_config
 * 输  入 : 参数           描述
 *          pu08_cmd        命令序列缓存
 *          u32_cmd_size    待写命令或数据大小
 *          pu08_wr_buf     待写数据缓存
 *          u32_wr_size     待写数据大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           TS_ERR         写失败
 *           TS_OK          写成功
 */
extern ts_s32 tdl_sfc_write_standard(ts_u08 *pu08_cmd, ts_u32 u32_cmd_size, ts_u08 *pu08_wr_buf, ts_u32 u32_wr_size);



/*
 * 函数名 : sfc_nand_chk_blk
 * 描  述 : 检测指定地址所在的块是不是坏块
 * 输  入 : 参数            描述
 *          u32_address     指定 NAD-FLASH 地址
 * 输  出 : 参数            描述
 *          无              无
 * 返  回 : 返回值          描述
 *          TS_OK           好块
 *          TS_ERR          读取错误
 *          -TS_EBADF       坏块
 */
extern ts_s32 sfc_nand_chk_blk(ts_u32 u32_address);


/*
 * 函数名 : sfc_read_reg
 * 描  述 : 发送指定命令读取 FLASH 内部寄存器
 * 输  入 : 参数            描述
 *          pu08_cmd        Flash 命令缓冲
 *          u32_cmd_size    命令个数
 * 输  出 : 参数            描述
 *          pu08_rd_buf     读缓冲
 *          u32_rd_size     读个数
 * 返  回 : 返回值          描述
 *          TS_OK           读取成功
 *          TS_ERR          读取错误
 */
extern ts_s32 sfc_read_reg(ts_u08 *pu08_cmd, ts_u32 u32_cmd_size, ts_u08 *pu08_rd_buf, ts_u32 u32_rd_size);


/*
 * 函数名 : sfc_write_reg
 * 描  述 : 发送指定命令写 FLASH 内部寄存器
 * 输  入 : 参数            描述
 *          pu08_cmd        Flash 命令与写数据缓冲
 *          u32_cmd_size    命令与数据个数
 * 输  出 : 参数            描述
*          无               无
 * 返  回 : 返回值          描述
 *          TS_OK           写成功
 *          TS_ERR          写错误
 */
extern ts_s32 sfc_write_reg(ts_u08 *pu08_cmd, ts_u32 u32_cmd_size);


/* 
 * 函数名 : lib_sfc_write_sr
 * 描  述 : 写 flash 状态寄存器, 对 sfc_write_reg 再封装
 * 输  入 : 参数           描述
 *          u32_sfc_type    flash 类型，支持 nor 和 nand, 查看 spi-flash.h
 *          u32_cmd_sr      snor 为寄存器命令， snand 为寄存器地址
 *          u32_val         待写入配置
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           无             无
 */
extern ts_void lib_sfc_write_sr(ts_u32 u32_sfc_type, ts_u32 u32_cmd_sr, ts_u32 u32_val);


/* 
 * 函数名 : lib_sfc_read_sr
 * 描  述 : 读取 flash 状态寄存器
 * 输  入 : 参数           描述
 *          u32_sfc_type    flash 类型，支持 nor 和 nand,查看 spi-flash.h
 *          u32_cmd_sr      snor 为寄存器命令， snand 为寄存器地址
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           TS_ERR         读取失败
 *           0..FF          读取成功
 */
extern ts_s32 lib_sfc_read_sr(ts_u32 u32_sfc_type, ts_u32 u32_cmd_sr);


/* 
 * 函数名 : lib_sfc_read_id
 * 描  述 : 读取 flash id
 * 输  入 : 参数           描述
 *          u32_sfc_type    flash 类型，支持 nor 和 nand
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           0              读取失败
 *           1..FFFFFF      读取成功, 为状态寄存器值
 */
extern ts_u32 lib_sfc_read_id(ts_u32 u32_sfc_type);


/* 
 * 函数名 : lib_sfc_chk_flash_busy
 * 描  述 : 检测对接 flash 是否空闲
 * 输  入 : 参数           描述
 *          u32_sfc_type    flash 类型，支持 nor 和 nand
 *          u32_tmout_us    判断等待超时时间
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           TS_OK          空闲
 *           TS_EBUSY       忙碌,注意，非负值
 */
extern ts_s32 lib_sfc_chk_flash_busy(ts_u32 u32_sfc_type, ts_u32 u32_tmout_us);


/* 
 * 函数名 :  lib_sfc_write_en
 * 描  述 :  使能写功能
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
extern ts_void lib_sfc_write_en(ts_void);


/* 
 * 函数名 : lib_sfc_nand_read
 * 描  述 : 读取数据到缓冲，会自动按照 page(默认2048) 大小拆分,内带坏块检测,忙检测
 * 输  入 : 参数           描述
 *          u32_address     sfc-nand flash 线性地址,内部拆分地址
 *          u32_size        读取的大小, 内部会自动按照 pagesize 拆分分批读取
 * 输  出 : 参数           描述
 *           pu08_buf        读取的数据输出缓冲
 * 返  回 : 返回值         描述
 *           0              读取失败
 *           u32_size       读取成功
 */
extern ts_u32 lib_sfc_nand_read(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size);


/* 
 * 函数名 : lib_sfc_nand_erase_block
 * 描  述 : 单个 block 擦除
 * 输  入 : 参数           描述
 *          u32_address     sfc-nand flash 线性地址，必须块对齐，由调用者保证，不带坏块检测，带忙检测
 *          u32_size        擦除的大小, 只支持 128KB 和 256KB
 * 输  出 : 参数           描述
 *          无              无
 * 返  回 : 返回值         描述
 *           0              读取失败
 *           u32_size       读取成功
 */
extern ts_u32 lib_sfc_nand_erase_block(ts_u32 u32_address, ts_u32 u32_size);


/* 
 * 函数名 : lib_sfc_nand_write
 * 描  述 : 写缓冲到 flash,只能写一页，内带 flash 一页为 2048 字节，内带坏块检测,忙检测
 * 输  入 : 参数           描述
 *          u32_address     sfc-nand flash 线性地址
 *          pu08_buf        写缓冲
 *          u32_size        写的大小, 不能超过 pagesize
 * 输  出 : 参数           描述
 *          无              无
 * 返  回 : 返回值         描述
 *           0              写入失败
 *           u32_size       写入成功
 */
extern ts_u32 lib_sfc_nand_write(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size);


/*
 * 函数名 : clk_drv_set
 * 描  述 : 设置指定类型的总线时钟
 * 输  入 : 参数        描述
 *          u32_type    时钟类型
 *          u32_clk_hz  时钟频率，单位赫兹
 * 输  出 : 参数        描述
 *          无          无
 * 返  回 : 返回值      描述
 *          无          无
 */
extern ts_void clk_drv_set(ts_u32 u32_type, ts_u32 u32_clk_hz);


/*
 * 函数名 : clk_dev_get(为 clk_drv_get 笔误，固件无法修改)
 * 描  述 : 获取指定类型的总线时钟 
 * 输  入 : 参数        描述
 *          u32_type    时钟类型
 * 输  出 : 参数        描述
 *          无          无
 * 返  回 : 返回值      描述
 *          0           获取失败
 *          > 0         时钟赫兹
 */
extern ts_u32 clk_dev_get(ts_u32 u32_type);


/*
 * 函数名 : verify_lib_calc_sum32
 * 描  述 : 按照 4 byte 计算校验和
 * 输  入 : 参数           描述
 *          u32_init_val   初始值
 *          pu8_inbuf      待计算缓冲
 *          u32_size       待计算大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *          0..0xffffffff  最终计算值
 */
extern ts_u32 verify_lib_calc_sum32(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);


/*
 * 函数名 : verify_lib_calc_crc32
 * 描  述 : 对输入数据做 CRC32 计算
 * 输  入 : 参数           描述
 *          u32_init_val    用户自定义初值，也可以是分步计算的前一个计算结果
 *          pu8_inbuf      待计算缓冲
 *          u32_size       待计算大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *          0..0xffffffff  最终计算值
 */
extern ts_u32 verify_lib_calc_crc32(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);


/* 
 * 函数名 : dly_lib_init_ctx
 * 描  述 : 初始化一个延时上下文
 * 输  入 : 参数               描述
 *           pst_dly           延时上下文
 *           u32_us            延时的微秒数
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void dly_lib_init_ctx(dly_context_s_t *pst_dly, ts_u32 u32_us);


/* 
 * 函数名 :  dly_lib_timeout
 * 描  述 :  判断一个延时上下文是否已经超时
 * 输  入 :  参数               描述
 *           pst_dly           延时上下文
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *          ts_true            超时
 *          ts_false           没有超时
 */
extern ts_bool dly_lib_timeout(dly_context_s_t *pst_dly);


/* 
 * 函数名 : dly_lib_delay_us
 * 描  述 : 延时指定的时间，单位 us
 * 输  入 : 参数               描述
 *           u32_us            延时的毫秒数
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void dly_lib_delay_us(ts_u32 u32_us);


/* 
 * 函数名 : cancel_lib_by_user
 * 描  述 : 进入读秒倒计时，如果在没有归零前通过串口输入任意值，则取消这次
 *          启动并进入命令行
 * 输  入 : 参数               描述
 *           u32_dly           读秒时间，单位为秒
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void cancel_lib_by_user(ts_u32 u32_dly);


/* 
 * 函数名 : cancel_lib_by_key
 * 描  述 : 在 50 毫秒内串口接收到指定个数的 '*'，则取消这次
 *          启动并进入命令行
 * 输  入 : 参数               描述
 *           u32_cnt           个数
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void cancel_lib_by_key(ts_u32 u32_cnt);


/* 
 * 函数名 : wdt_start
 * 描  述 : 看门狗启动
 * 输  入 : 参数               描述
 *          u32_period_ms  超时周期，ms为单位
 * 输  出 : 参数               描述
 *          无                 无
 * 返  回 : 返回值             描述
 *           无                 无
 */
extern ts_void wdt_start(ts_u32 u32_period_ms);


/* 
 * 函数名 : wdt_stop
 * 描  述 : 看门狗停止,不建议生产环境使用
 * 输  入 : 参数        描述
 *          无         无
 * 输  出 : 参数        描述
 *          无         无
 * 返  回 : 返回值      描述
 *          无         无
 */
extern ts_void wdt_stop(ts_void);


/* 
 * 函数名 : wdt_knock
 * 描  述 : 看门狗敲门，默认 6s 超时
 * 输  入 : 参数        描述
 *           无         无
 * 输  出 : 参数        描述
 *           无         无
 * 返  回 : 返回值      描述
 *           无         无
 */
extern ts_void wdt_knock(ts_void);


/* 
 * 函数名 : timer_get_now
 * 描  述 : 读取当前定时器计数
 * 输  入 : 参数               描述
 *           无                 无
 * 输  出 : 参数               描述
 *           无                 无
 * 返  回 : 返回值             描述
 *          0..FFFFFFFF        定时器当前计数值
 */
extern ts_u32 timer_get_now(ts_void);


/*  
 * 函数名 :  serial_set_baudrate
 * 描  述 :  按照输入的波特率计算分频值，并配置到串口0寄存器
 * 输  入 :  参数           描述
 *          u32_clk         串口0时钟
 *          u32_baudrate    波特率设置(2400,9600...)
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           1..ffff        最终计算得到的时钟分频
 *           0              设置错误
 */
extern ts_u32 serial_set_baudrate(ts_u32 u32_clk, ts_u32 u32_baudrate);


/*  
 * 函数名 :  serial_set_baudrate_div
 * 描  述 :  直接写入串口0波特率分频
 * 输  入 :  参数           描述
 *          u32_div         波特率分频
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void serial_set_baudrate_div(ts_u32 u32_div);


/* 
 * 函数名 :  serial_drv_write
 * 描  述 :  串口 0 发送指定大小的数据
 * 输  入 :  参数           描述
 *          pu08_buf        待发送数据缓冲
 *          u32_size        待发送数据大小
 *          u32_timeout_us   超时, us 为单位
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          发送成功
 *           非 TS_OK       发送失败
 */
extern ts_s32 serial_drv_write(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us);


/* 
 * 函数名 :  serial_drv_read
 * 描  述 :  串口 0 读取指定大小的数据
 * 输  入 :  参数           描述
 *          u32_size         读数据大小
 *          u32_timeout_us   超时, us 为单位
 * 输  出 :  参数           描述
 *           pu08_buf        读缓冲
 * 返  回 :  返回值         描述
 *           TS_OK          发送成功
 *           非 TS_OK       发送失败
 */
extern ts_s32 serial_drv_read(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us);


/* 
 * 函数名 :  cache_drv_invalid_all
 * 描  述 :  无效所有 cache ，包括 DCache 和 ICache
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void cache_drv_invalid_all(ts_void);


/* 
 * 函数名 :  cache_drv_flush
 * 描  述 :  将 DCache 里的数据全部写出
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void cache_drv_flush(ts_void);



/*
 * 函数名 :  lib_sfc_scan
 * 描  述 :  通过读取 flash id ，判断对接设备类型
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           SFC_TYPE_NOR       对接 sfc-nor
 *           SFC_TYPE_NAND      对接 sfc-nand
 *           TS_ERR             非 nor 或 nand 设备
 */
extern ts_s32 lib_sfc_scan(ts_void);


/* 
 * 函数名 :  lib_sfc_nor_read
 * 描  述 :  读取数据到缓冲，会自动按照 page 大小拆分
 * 输  入 :  参数           描述
 *          u32_address     sfc-nor flash 线性地址
 *          u32_size        读取的大小, 内部会自动按照 pagesize 拆分分批读取
 * 输  出 :  参数           描述
 *           pu08_buf        读取的数据输出缓冲
 * 返  回 :  返回值         描述
 *           0              读取失败
 *           u32_size       读取成功
 */
ts_u32 lib_sfc_nor_read(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __BOOTUP_API_H__ */
