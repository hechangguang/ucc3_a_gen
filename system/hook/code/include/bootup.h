/*
 * bootup.h -- bootup 公共头文件,导出所有固件接口
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __BOOTUP_H__
#define __BOOTUP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_bitop.h>

/* for clock */
#define TS_BOOTUP_CLKT_AHB          (0)
#define TS_BOOTUP_CLKT_APB          (1)

/* 启动支持类型 */
#define BOOTUP_DEVTYPE_ILLEGAL      (0)
#define BOOTUP_DEVTYPE_RAM          (1)
#define BOOTUP_DEVTYPE_SPI_NOR      (2)
#define BOOTUP_DEVTYPE_SPI_NAND     (3)
#define BOOTUP_DEVTYPE_EMMC         (4)
#define BOOTUP_DEVTYPE_SDIO         (5)
#define BOOTUP_DEVTYPE_SDCARD       (6)
#define BOOTUP_DEVTYPE_PCI          (7)
#define BOOTUP_DEVTYPE_USB          (8)
#define BOOTUP_DEVTYPE_NET_TCP      (9)
#define BOOTUP_DEVTYPE_NET_UDP      (10)
#define BOOTUP_DEVTYPE_UART         (11)
#define BOOTUP_DEVTYPE_SPI          (12)
#define BOOTUP_DEVTYPE_SSBUS        (13)

#define BOOTUP_DEVTYPE_UNKONW       (-1)

/* 支持校验类型 */
#define BOOTUP_VERIFY_NULL          (0)
#define BOOTUP_VERIFY_SUM32         (1)
#define BOOTUP_VERIFY_SUM8          (2)
#define BOOTUP_VERIFY_CRC32         (3)
#define BOOTUP_VERIFY_CRC8          (4)
#define BOOTUP_VERIFY_SIGN          (5)

#define BOOTUP_VERIFY_UNKONW        (-1)

/* 支持镜像压缩类型 */
#define BOOTUP_COMPRESS_NONE        (0)
#define BOOTUP_COMPRESS_LZO         (1)
#define BOOTUP_COMPRESS_RLE         (2)
#define BOOTUP_COMPRESS_FSE         (3)

#define BOOTUP_COMPRESS_UNKNOW      (4)

/* DDR 单片容量 */
#define BOOTUP_DDR_VOLUME_4MB       (0)
#define BOOTUP_DDR_VOLUME_16MB      (1)
#define BOOTUP_DDR_VOLUME_32MB      (2)

/* image info */
#define IH_BOOTUP_MAGIC             (0x15735423)
#define IH_UBOOT_MAGIC              (0x27051956)
#define IH_BOOTUP_CRC_OFFSET        (8)
#define IH_UBOOT_NMLEN              (32)            /* U-Boot Image Name Length   */

/* 设备能力, 内部缓冲大小，代表一次通讯可以发送的最大数据, 不包括属性包
    计算方式 0 = 256
    其余 x << 9
    比如  512 = 1  << 9
 */
#define BOOTUP_CAPS_BUF_256         (0)
#define BOOTUP_CAPS_BUF_512         (1)
#define BOOTUP_CAPS_BUF_1024        (2)
#define BOOTUP_CAPS_BUF_2048        (4)
#define BOOTUP_CAPS_BUF_4096        (8)

/* hook 镜像包头 */
typedef struct bootup_ih_s {
    /* Image Header Magic Number */
    ts_u32  u32_ih_magic;

    /* Image Header CRC Checksum */
    ts_u32  u32_ih_hcrc;

    /* Image Creation Timestamp */             
    ts_u32  u32_ih_time;

    /* Image Hook Size */               
    ts_u32  u32_ih_size;

    /* Image Hook Load  Address */
    ts_u32  u32_ih_load;

    /* Image Hook Entry Point Address */
    ts_u32  u32_ih_ep;

    /* Image Data CRC Checksum */
    ts_u32  u32_ih_dcrc;

    /* 校验方式 crc 或者 rsa2048 */
    ts_u08 u08_ih_verify_type;

    /* 忽略校验，在安全启动开始时，此设置无效 */
    ts_u08 u08_ignore_verify;

    /* 使用默认的公钥进行加密，在开启硬件安全启动时， 此设置无效*/
    ts_u08 u08_ih_sign_en;

    /* 压缩方式 */
    ts_u08 u08_ih_comp;

    /* 日志等级 */
    ts_u08 u08_ih_debug_level;

    /* Hook 备份个数 */
    ts_u08 u08_ih_bk_cnt;

    /* Image 备份个数 */
    ts_u08 u08_ih_img_bk_cnt;

    /* Image 自带包头，如 ts-bootup 和 u-boot 包头 */
    ts_u08 u08_rev;

    /* bootup 备份基于 u32_ih_load 的偏移大小 */
    ts_u32 u32_ih_bk_offset;

    /* 镜像备份基于 u32_ih_img_addr 的偏移大小 */
    ts_u32 u32_ih_img_bk_offset;

    /* 待加载的镜像信息, 如果 u32_ih_img_load_addr = u32_ih_load, 
     * 表明 镜像紧随 hook 部分之后，使用以下配置加载
     * 如果 u32_ih_img_load_addr != u32_ih_load , 表明 镜像单独存放，且
     * 有自己的头部信息，此时仅使用 u32_ih_img_load_addr 配置，其余配置不使用.
     */
    ts_u32 u32_ih_img_load_addr;
    ts_u32 u32_ih_img_ep_addr;
    ts_u32 u32_ih_img_size;
    ts_u32 u32_ih_img_crc;

    ts_u32  u32_rev;
} bootup_ih_s_t;

/* image(u-boot format) */
typedef struct uboot_ih_s {
    ts_u32  u32_ih_magic;                   /* Image Header Magic Number    */
    ts_u32  u32_ih_hcrc;                    /* Image Header CRC Checksum    */
    ts_u32  u32_ih_time;                    /* Image Creation Timestamp     */
    ts_u32  u32_ih_size;                    /* Image Data Size              */
    ts_u32  u32_ih_load;                    /* Data     Load  Address       */
    ts_u32  u32_ih_ep;                      /* Entry Point Address          */
    ts_u32  u32_ih_dcrc;                    /* Image Data CRC Checksum      */
    ts_u08  u08_ih_os;                      /* Operating System             */
    ts_u08  u08_ih_arch;                    /* CPU architecture             */
    ts_u08  u08_ih_type;                    /* Image Type                   */
    ts_u08  u08_ih_comp;                    /* Compression Type             */
    ts_u08  u08_ih_name[IH_UBOOT_NMLEN];    /* Image Name    */
} uboot_ih_s_t;

/* 命令链接段 */
#define HOOK_START_SECT __attribute__((unused)) __attribute__((section(".hook_start")))
#define CMDL_SECT __attribute__((unused)) __attribute__((section(".cmdl_sect")))

/* 命令结构体 */
typedef struct cmd_info_s {
    /* 命令名称 */
    char *pc_name;

    /* 命令实现 */
    ts_void (*pfn_cmd)(int argc, char * argv[]);
} cmd_info_s_t;


/* 超时上下文 */
typedef struct dly_context_s {
    ts_u32 u32_start;
    ts_u32 u32_count;
} dly_context_s_t;


#ifdef CONFIG_SUPPORT_SIGN
#include <bootup_sign.h>
#endif

#include <bootup_api.h>


/* hook 函数实现 */


/* 
 * 函数名 : soc_init
 * 描  述 : 芯片级初始化
 * 输  入 : 参数           描述
 *          无             无
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
ts_void soc_init(ts_void);


/* 
 * 函数名 : soc_get_ddr_bank_regbase
 * 描  述 : 获取指定 DDR BANK 的寄存器基地址
 * 输  入 : 参数           描述
 *          u32_bank_idx    Bank 序号
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          0               无效设备
 *          > 0             bank寄存器
 */
ts_u32 soc_get_ddr_bank_regbase(ts_u32 u32_bank_idx);


/* 
 * 函数名 : soc_get_uart_regbase
 * 描  述 : 获取指定串口设备的寄存器基地址
 * 输  入 : 参数           描述
 *          u32_dev_idx    串口设备序号
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          0               无效设备
 *          > 0             指定串口寄存器
 */
ts_u32 soc_get_uart_regbase(ts_u32 u32_dev_idx);


/* 
 * 函数名 : soc_uart_set_mux
 * 描  述 : 串口设置复用关系
 * 输  入 : 参数           描述
 *          u32_dev_idx    串口设备序号
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          0               无效设备
 *          > 0             指定串口寄存器
 */
ts_void soc_uart_set_mux(ts_u32 u32_dev_idx);


/* 
 * 函数名 : soc_get_osc_clk
 * 描  述 : 获取当前晶振时钟
 * 输  入 : 参数           描述
 *          无             无
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          0               无效设备
 *          > 0             晶振时钟
 */
ts_u32 soc_get_osc_clk(ts_void);


/* 
 * 函数名 : bootup_ddr_init
 * 描  述 : 初始化 DDR
 * 输  入 : 参数           描述
 *          u32_bank0_en   bank0 是否使能，0：禁止，>0 使能
 *          u32_bank1_en   bank1 是否使能，0：禁止，>0 使能
 *          u32_volume     容量，仅支持 32Mb,128Mb,256Mb
 *          u32_ddr_clk    总线频率，MHZ为单位
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
ts_s32 hook_ddr_init(ts_u32 u32_bank0_en, ts_u32 u32_bank1_en, ts_u32 u32_volume, ts_u32 u32_ddr_clk);


/* 
 * 函数名 : hook_clkm_init
 * 描  述 : 初始化时钟
 * 输  入 : 参数           描述
 *          u32_cpu_clk     CPU 时钟，MHZ为单位
 *          u32_bus_clk     BUS 总线，MHZ为单位
 *          u32_ddr_clk     总线频率，MHZ为单位
 *          u32_vco_safe_en vco 将保证在 600MHZ 以上，u32_vco_safe_en = 0 情况下，不保证 PLL 稳定，风险自负
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
ts_void hook_clkm_init(ts_u32 u32_cpu_clk, ts_u32 u32_bus_clk, ts_u32 u32_ddr_clk, ts_u32 u32_vco_safe_en);


/* 
 * 函数名 : hook_uart_init
 * 描  述 : 初始化指定串口
 * 输  入 : 参数           描述
 *          u32_dev_idx     串口设备序号
 *          u32_baudrate    串口波特率
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          TS_OK           设置成功
 *          -TS_ENOTSUP     不支持此设备
 */
ts_s32 hook_uart_init(ts_u32 u32_dev_idx, ts_u32 u32_baudrate);


/* 
 * 函数名 : hook_uart_write
 * 描  述 : 通过指定串口设备写数据
 * 输  入 : 参数           描述
 *          u32_dev_idx     串口设备序号
 *          pu08_buf        写入缓冲
 *          u32_size        写入大小
 *          u32_timeout_us  串口读写超时设置，单位微妙
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          TS_OK           写成功
 *          -TS_ETIMEDOUT    写超时
 *          -TS_ENOTSUP     不支持此设备
 */
ts_s32 hook_uart_write(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us);


/* 
 * 函数名 : hook_uart_read
 * 描  述 : 通过指定串口设备读数据
 * 输  入 : 参数           描述
 *          u32_dev_idx     串口设备序号
 *          u32_size        读个数
 *  *       u32_timeout_us  串口读写超时设置，单位微妙
 * 输  出 : 参数           描述
 *          pu08_buf         读缓冲
 * 返  回 : 返回值         描述
 *          TS_OK           读成功
 *          TS_ERR          读失败
 *          -TS_ENOTSUP     不支持此设备
 */
ts_s32 hook_uart_read(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us);


/* 
 * 函数名 : hook_uart_putc
 * 描  述 : 通过指定串口设备写一个字符
 * 输  入 : 参数           描述
 *          u32_dev_idx     串口设备序号
 *          u08_val         待写字符
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
ts_void hook_uart_putc(ts_u32 u32_dev_idx, ts_u08 u08_val);


/* 
 * 函数名 : hook_uart_putc
 * 描  述 : 通过指定串口设备写字符串
 * 输  入 : 参数           描述
 *          u32_dev_idx     串口设备序号
 *          pc_str          待写字符串
 * 输  出 : 参数           描述
 *          无             无
 * 返  回 : 返回值         描述
 *          无             无
 */
ts_void hook_uart_puts(ts_u32 u32_dev_idx, ts_char *pc_str);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __BOOTUP_H__ */
