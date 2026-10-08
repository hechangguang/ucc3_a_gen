/* 
 * i2c_v100.h -- i2c v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_I2C_V100_H__
#define __API_I2C_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/cpl_ioctl.h>

/* 
 * 定义支持 I2C 总线模式, 设备地址 8 bit 固定
 */
#define I2C_BUS_RA08_D08        (0)
#define I2C_BUS_RA08_D16        (1)
#define I2C_BUS_RA16_D08        (2)
#define I2C_BUS_RA16_D16        (3)

typedef struct ts_i2c_attr_s {
    /* 主从模式 */
    ts_u08 u08_master;

    /* 时钟占空比设置, 配置范围 10..90 */
    ts_u08 u08_duty;

    /* 调整时钟占空比，默认时钟为 50% , 比如根据 u32_bus_hz 计算出来，高时钟计数 50， 
     * 低时钟计数 50, 如 u16_duty_adj 不为 0 ， 则 高时钟 += u16_duty_adj， 低时钟 -= u16_duty_adj
     */
    ts_u08 u08_duty_adj;

    /* 指定开启中断模式的数据长度阈值， 如配置成 16， 只有读写超过 16 个数据才会启动中断模式，反之只会轮询 */
    ts_u08 u08_roll_thr;

    /* 配置 SDA HOLD，单位微妙， 数据将在 SCL 下降延后延迟配置微妙时间后输出, 配置成 0 则使用 u08_sda_hold_clk */
    ts_u08 u08_sda_hold_us;

    /* 配置 SDA HOLD，单位 hz， 数据将在 SCL 下降延后延迟配置 hz 数后输出(非 SCL 时钟，一般为 i2c 模块的 APB 时钟)
        配置成 0 成使用 u08_sda_hold_us
        u08_sda_hold_clk 优先于 u08_sda_hold_us
     */
    ts_u08 u08_sda_hold_clk;

    /* sda setup */
    ts_u08 u08_sda_setup_us;

    /* 超时设置, 最小一个系统滴答 */
    ts_u08 u08_time_out_jfs;

    /* i2c 时钟设置 */
    ts_u32 u32_bus_hz;
} ts_i2c_attr_s_t;


/* 从设备属性 */
typedef struct ts_i2c_slv_attr_s {
    /* 从设备地址 */
    ts_u08 u08_dev_addr;

    /* 寄存器、数据位宽配置:  I2C_BUS_RA08_D08, I2C_BUS_RA08_D16, I2C_BUS_RA16_D08, I2C_BUS_RA16_D16 */
    ts_u08 u08_mode;

    /* 从设备对外提供的寄存器表大小 */
    ts_u16 u16_reg_size;

    /* 对外寄存器表, 在 i2c slave 接收到读命令时，会根据地址，返回此表内的数据 */
    ts_void *pv_reg_map;

    /* 接收 fifo 阈值，建议 16， 如果测试发现丢数据，可以适当减小 */
    ts_u08 u08_rfl;

    /* 保留 */
    ts_u08 u08_rev;
    ts_u16 u16_rev;
    ts_u32 u32_rev;
} ts_i2c_slv_attr_s_t;


typedef struct ts_i2c_msg_s {
    /* i2c 总线 模式，支持寄存器地址 8 和 16 bit */
    ts_u08 u08_mode;

    /* i2c 读写时，忽略寄存器地址配置 */
    ts_u08 u08_ignore_reg_addr;

    ts_u08 au08_rev[2];

    /* i2c 目标设备地址 */
    ts_u16 u16_dev_addr;

    /* i2c 目标寄存器地址 */
    ts_u16 u16_reg_addr;

    /* 数据缓冲配置 */
    ts_u32 u32_buf_size;
    ts_void *pv_buf;

    /* 从设备读的等待超时 */
    ts_u32 u32_tmout_ms;

    /* i2c 属性, 如果不为空，驱动会按照此属性配置设备 */
    ts_i2c_attr_s_t *pst_attr;
} ts_i2c_msg_s_t;



/* API */

/* 
 * 函数名 :  ts_lib_i2c_open
 * 描  述 :  打开 i2c 设备
 * 输  入 :  参数           描述
 *          pc_dev_name    设备文件名
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           >= 0           设备句柄
 *           < 0            打开设备失败
 */
extern ts_s32 ts_lib_i2c_open(ts_char *pc_dev_name);


/* 
 * 函数名 :  ts_lib_i2c_close
 * 描  述 :  关闭 i2c 设备句柄
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
extern ts_void ts_lib_i2c_close(ts_s32 s32_handle);


/* 
 * 函数名 :  ts_lib_i2c_get_attr
 * 描  述 :  获取 i2c 属性
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *           pst_attr      i2c 设备属性，内存空间由用户申请
 * 返  回 :  返回值         描述
 *          TS_OK           获取成功
 *          TS_ERR          无法获取设备句柄
 *          -TS_ENULLPTR    pst_attr 为空指针
 */
extern ts_s32 ts_lib_i2c_get_attr(ts_s32 s32_handle, ts_i2c_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_lib_i2c_set_attr
 * 描  述 :  设置 i2c 属性
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          pst_attr        i2c 设备属性句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          TS_OK           获取成功
 *          TS_ERR          无法获取设备句柄
 *          -TS_ENULLPTR    pst_attr 为空指针
 */
extern ts_s32 ts_lib_i2c_set_attr(ts_s32 s32_handle, const ts_i2c_attr_s_t *pst_attr);



/* 
 * 函数名 :  ts_lib_i2c_ra08d08_read
 * 描  述 :  按照寄存器、数据都是 8 bit 位宽读指定长度的数据到缓冲
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u32_cnt         读取个数
 * 输  出 :  参数           描述
 *           pu08_rd_buf    读取缓冲
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d08_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_rd_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra08d08_readb
 * 描  述 :  按照寄存器、数据都是 8 bit 位宽读取一个字节数据
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 * 输  出 :  参数           描述
 *           pu08_val        单字节结果指针
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d08_readb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val);


/* 
 * 函数名 :  ts_lib_i2c_ra08d16_read
 * 描  述 :  按照寄存器 8bit、数据是 16 bit 位宽读指定长度的数据到缓冲
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u32_cnt         读取个数
 * 输  出 :  参数           描述
 *           pu16_rd_buf    读取缓冲
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d16_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
            ts_u16 *pu16_rd_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra08d16_readw
 * 描  述 :  按照寄存器 8bit、数据是 16 bit 位宽读取一个字节数据
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 * 输  出 :  参数           描述
 *           pu16_val        单字节结果指针
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d16_readw(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 *pu16_val);


/* 
 * 函数名 :  ts_lib_i2c_ra16d08_read
 * 描  述 :  按照寄存器 16bit、数据是 8bit 位宽读指定长度的数据到缓冲
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u32_cnt         读取个数
 * 输  出 :  参数           描述
 *           pu08_rd_buf    读取缓冲
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d08_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_rd_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra16d08_readb
 * 描  述 :  按照寄存器 16bit、数据是 8 bit 位宽读取一个字节数据
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 * 输  出 :  参数           描述
 *           pu08_val        单字节结果指针
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d08_readb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val);


/* 
 * 函数名 :  ts_lib_i2c_ra16d16_read
 * 描  述 :  按照寄存器 16bit、数据是 16bit 位宽读指定长度的数据到缓冲
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u32_cnt         读取个数
 * 输  出 :  参数           描述
 *           pu16_rd_buf    读取缓冲
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d16_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u16 *pu16_rd_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra16d16_readw
 * 描  述 :  按照寄存器、数据都是 16 bit 位宽读取一个 short 数据
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 * 输  出 :  参数           描述
 *           pu16_val        16 字节结果指针
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d16_readw(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 *pu16_val);


/* 
 * 函数名 :  ts_lib_i2c_ra08d08_write
 * 描  述 :  按照寄存器、数据都是 8 bit 位宽把指定长度的数据缓冲写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          pu08_wr_buf     写数据缓冲
 *          u32_cnt         写个数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d08_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_wr_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra08d08_writeb
 * 描  述 :  按照寄存器、数据都是 8 bit 位宽把指定 1 个数据写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u08_val        单字节数据
 * 输  出 :  参数           描述
 *             无           无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  读取超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d08_writeb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 u08_val);


/* 
 * 函数名 :  ts_lib_i2c_ra08d16_write
 * 描  述 :  按照寄存器 8bit 、数据是 16 bit 位宽把指定长度的数据缓冲写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          pu16_wr_buf     写数据缓冲
 *          u32_cnt         写个数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d16_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u16 *pu16_wr_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra08d16_writew
 * 描  述 :  按照寄存器 8bit 、数据是 16 bit 位宽把 1个 长度的数据缓冲写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u16_val         待写数据
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra08d16_writew(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 u16_val);


/* 
 * 函数名 :  ts_lib_i2c_ra16d08_write
 * 描  述 :  按照寄存器 16bit 、数据是 8 bit 位宽把指定长度的数据缓冲写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          pu08_val        写数据缓冲
 *          u32_cnt         写个数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d08_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val, \
                            ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra16d08_writeb
 * 描  述 :  按照寄存器 16bit 、数据是 8 bit 位宽把 1个 长度的数据缓冲写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u08_val         待写数据
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_EINVAL     参数错误
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d08_writeb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 u08_val);


/* 
 * 函数名 :  ts_lib_i2c_ra16d16_write
 * 描  述 :  按照寄存器、数据都是 16 bit 位宽把指定 1 个数据写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          pu16_wr_buf     待写缓冲
 *          u32_cnt         待写个数
 * 输  出 :  参数           描述
 *             无           无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d16_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
    ts_u16 *pu16_wr_buf, ts_u32 u32_cnt);


/* 
 * 函数名 :  ts_lib_i2c_ra16d16_writew
 * 描  述 :  按照寄存器、数据都是 16 bit 位宽把指定 1 个数据写到 i2c 从设备
 * 输  入 :  参数           描述
 *          s32_handle      设备句柄
 *          u32_dev_addr    i2c 目标设备地址
 *          u32_reg_addr    i2c 目标设备寄存器地址
 *          u16_val        单字节数据
 * 输  出 :  参数           描述
 *             无           无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           -TS_EBUSY      i2c 设备忙
 *           -TS_ETIMEDOUT  写超时
 *           -TS_ERR        其他错误
 */
extern ts_s32 ts_lib_i2c_ra16d16_writew(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 u16_val);


/* 从模式接口 */

/* 
 * 函数名 :  ts_lib_i2c_start_slave
 * 描  述 :  将 i2c 设备设置成从模式,并且启动监听
 * 输  入 :  参数               描述
 *          s32_handle          i2c 设备句柄，由调用 ts_lib_i2c_open 获得
 *          pst_slv_attr        i2c 从设备的属性句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *           -TS_EBUSY          i2c 设备忙
 *           -TS_EINVAL         参数错误
 *           -TS_ERR            设置失败其他错误
 */
extern ts_s32 ts_lib_i2c_start_slave(ts_s32 s32_handle, ts_i2c_slv_attr_s_t *pst_slv_attr);


/* 
 * 函数名 :  ts_lib_i2c_slv_read_msg
 * 描  述 :  读取主机传输过来的数据
 * 输  入 :  参数               描述
 *          s32_handle          i2c 设备句柄，由调用 ts_lib_i2c_open 获得
 *          u32_cnt             指定读大小
 *          u32_tmout_ms        超时时间， ms 为单位
 * 输  出 :  参数               描述
 *           pu08_buf           读缓存, 需用户申请内存
 * 返  回 :  返回值             描述
 *           >= 0               实际读取个数
 *           -TS_EBUSY          i2c 设备忙
 *           -TS_EINVAL         参数错误
 *           -TS_ERR            读失败其他错误
 */
extern ts_s32 ts_lib_i2c_slv_read_msg(ts_s32 s32_handle, ts_u08 *pu08_buf, ts_u32 u32_cnt, ts_u32 u32_tmout_ms);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_I2C_V100_H__ */
