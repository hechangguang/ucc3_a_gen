/* 
 * spi_v100.h -- spi_v100 上层用户调用头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_SPI_V100_H__
#define __API_SPI_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>

/* 
 *  mode        CPOL    CPHA 
 *   0          0       0
 *   1          0       1
 *   2          1       0
 *   3          1       1
 */
#define SPI_MODE_0      (0)
#define SPI_MODE_1      (1)
#define SPI_MODE_2      (2)
#define SPI_MODE_3      (3)

/* little endian */
#define SPI_ENDIAN_LE   (0)

/* big endian */
#define SPI_ENDIAN_BE   (1)

typedef struct ts_spi_attr_s {
    /* spi 模式，支持 0,1,2,3 */
    ts_u16 u16_mode;

    /* 数据大小端 */
    ts_u16 u16_endian;

    /* spi 分频系数 */
    ts_u16 u16_clk_div;

    /* spi 读采样时钟延时 */
    ts_u16 u16_rx_dly;
} ts_spi_attr_s_t;


typedef struct ts_spi_msg_s {
    /* spi 片选 ，根据设备具体片选信号设置 */
    ts_u08 u08_cs;

    /* spi 读写数据 bit 位宽， 取值范围 8,16,32 , 可根据每次读写选择位宽大小 */
    ts_u08 u08_dfs;

    ts_u08 au08_rev[2];

    /* send 包括命令缓存和发送缓存，避免可能出现的数据拷贝, 大小单位为 byte */
    ts_u32 u32_cmd_size;
    ts_u08 *pu08_cmd_buf;

    ts_u32 u32_tx_size;
    ts_u08 *pu08_tx_buf;

    /* recv */
    ts_u32 u32_rx_size;
    ts_u08 *pu08_rx_buf;
} ts_spi_msg_s_t;

/* ioctl */
enum {
	/* SPI设置属性 */
	IOC_NR_SPI_SET_ATTR = 0,

	/* SPI获取属性 */
	IOC_NR_SPI_GET_ATTR,

	IOC_NR_SPI_XFER,

	IOC_NR_SPI_BUTT,
};

#define TS_SPI_IOCTL_SET_ATTR       TS_CPL_IOW(TS_IOC_MAGIC_SPI, IOC_NR_SPI_SET_ATTR, ts_spi_attr_s_t)
#define TS_SPI_IOCTL_GET_ATTR       TS_CPL_IOW(TS_IOC_MAGIC_SPI, IOC_NR_SPI_GET_ATTR, ts_spi_attr_s_t)
#define TS_SPI_IOCTL_XFER           TS_CPL_IOW(TS_IOC_MAGIC_SPI, IOC_NR_SPI_XFER, ts_spi_msg_s_t)

/*
 * 函数名 :  ts_lib_spimst_set_attr
 * 描  述 :  设置spimst 传输参数
 * 输  入 :  参数               描述
 *           s32_hdl            spimst 句柄
 *           pst_attr           spimst 属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK               设置成功
 *           TS_ERR              设置失败
 */
ts_s32 ts_lib_spimst_set_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr);


/*
* 函数名 :  ts_lib_spimst_get_attr
* 描  述 :  获取spimst 传输参数
* 输  入 :  参数               描述
*           s32_hdl            spimst 句柄
* 输  出 :  参数               描述
*           pst_attr           spimst 属性
* 返  回 :  返回值             描述
*           TS_OK               获取成功
*           TS_ERR              获取失败
*/
ts_s32 ts_lib_spimst_get_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr);


/*
* 函数名 :  ts_lib_spimst_xfer
* 描  述 :  spimst 传输
* 输  入 :  参数               描述
*           s32_hdl            spimst 句柄
*           pst_msg            传输message
* 输  出 :  参数               描述
*           无                  无
* 返  回 :  返回值             描述
*           TS_OK               获取成功
*           TS_ERR              获取失败
*           -TS_EINVAL          无效参数
*/
ts_s32 ts_lib_spimst_xfer(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg);

ts_s32 ts_lib_spislv_set_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr);
ts_s32 ts_lib_spislv_get_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr);
ts_s32 ts_lib_spislv_write(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg);
ts_s32 ts_lib_spislv_read(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_SPI_V100_H__ */
