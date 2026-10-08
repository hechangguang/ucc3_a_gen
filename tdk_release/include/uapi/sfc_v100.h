/* 
 * sfc_v100.h -- sfc v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_SFC_V100_H__
#define __API_SFC_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_log.h>
#include <base/ts_errno.h>
#include <cpl/cpl_ioctl.h>

/* SFC 多线制配置, 依次为单线、双线、四线 */
#define TS_SFC_MODE_STANDARD        (1)
#define TS_SFC_MODE_DUAL            (2)
#define TS_SFC_MODE_QUAD            (4)

/* SFC 写入 cover 和 merge 策略
cover : 在写不满扇区或者写起始地址不是扇区起始地址的时候，
        不会先将范围外的数据读回
merge : 在写不满扇区或者写起始地址不是扇区起始地址的时候，
        将范围外的数据读回，并与要写入的数据合并成一个扇区的数据再写入
*/
#define TS_SFC_WRITE_COVER      (0)
#define TS_SFC_WRITE_MERGE      (1)

/* 设备属性 */
typedef struct ts_sfc_attr_s {
    /* 采样延迟 */
    ts_u08 u08_rx_sample_dly;

    /* 片选信号 */
    ts_u08 u08_cs;

    /* 以 jiffies 为单位，最大 255 个系统时钟,用于擦除等待 */
    ts_u08 u08_tmout_jfs;

    ts_u08 u08_rev;

    /* 时钟配置, hz 为单位 */
    ts_u32 u32_bus_clk;
} ts_sfc_attr_s_t;


/* sffs 配置结构体 */
typedef struct sffs_cfg_s {
    /* sffs 使用或占用的块个数 */
    ts_u32 u32_blk_cnt;

    /* 额外用于擦除写入轮转的块个数,或者是预留块数 */
    ts_u32 u32_extra_blk_cnt;

    /* 起始偏移块地址 */
    ts_u32 u32_offset_blk_addr;

    /* SFC 属性配置 */
    ts_sfc_attr_s_t st_attr;

    /* 挂载的文件 */
    ts_void *pv_fs;

    /* 保护区 */
    ts_void *pv_protect;
} sffs_cfg_s_t;


/* flash 命令字 */

/* --- SPI_NOR --- */
#define SFC_COMM_CMD_READID             (0x9f)
#define SFC_NOR_CMD_DUAL_READ_PAGE      (0x3B)

#define SPIF_NOR_CMD_WRITE_SR1          (0x01)
#define SPIF_NOR_CMD_WRITE_SR2          (0x31)
#define SPIF_NOR_CMD_WRITE_SR3          (0x11)
#define SPIF_NOR_CMD_READ_SR1           (0x05)
#define SPIF_NOR_CMD_READ_SR2           (0x35)
#define SPIF_NOR_CMD_READ_SR3           (0x15)

#define SPIF_NOR_CMD_READ_CFG           (0x15)
#define SPIF_NOR_CMD_WRITE_CFG          (0x11)
#define SPIF_NOR_CMD_PROGRAM_PAGE       (0x02)
#define SPIF_NOR_CMD_BUFFER_WRITE       (0x9c)
#define SPIF_NOR_CMD_BUFFER_PROGRAM     (0x9d)
#define SPIF_NOR_CMD_WRITE_ENABLE       (0x06)
#define SPIF_NOR_CMD_ERASE_SECTOR       (0x20)
#define SPIF_NOR_CMD_ERASE_64KB         (0xD8)

/* --- SPI_NAND --- */
#define SFC_NAND_CMD_PAGE_PROG          (0x02)
#define SFC_NAND_CMD_PAGE_PROG_QUAD     (0x34)
#define SFC_NAND_CMD_PAGE_LOAD_PROG_QUAD     (0x32)


#define SFC_NAND_CMD_PROG_EXC           (0x10)
#define SFC_NAND_CMD_PRELOAD            (0x13)
#define SFC_NAND_CMD_READ_PAGE          (0x03)
#define SFC_NAND_CMD_DUAL_READ_PAGE     (0x3B)
#define SFC_NAND_CMD_QUAD_READ_PAGE     (0x6B)

#define SFC_NAND_CMD_BLK_ERASE          (0xD8)

#define SFC_NAND_CMD_READ_FEATURE       (0x0F)
#define SFC_NAND_CMD_SET_FEATURE        (0x1F)

#define SFC_NAND_STATUS_BP              (0xA0)
#define SFC_NAND_STATUS_FEATURE0        (0xB0)
#define SFC_NAND_STATUS_STATUS0         (0xC0)
#define SFC_NAND_STATUS_FEATURE1        (0xD0)
#define SFC_NAND_STATUS_STATUS1         (0xF0)

#define SFC_NAND_RESET                  (0xFF)

/* C0H: bit 定义 */
#define SFC_NAND_OIP                    (0x01)
#define SFC_NAND_WEL                    (0x02)
#define SFC_NAND_E_FAIL                 (0x04)
#define SFC_NAND_P_FAIL                 (0x08)
#define SFC_NAND_FAIL                   (SFC_NAND_E_FAIL | SFC_NAND_P_FAIL)

#define SFC_NAND_ECCS0                  (0x10)
#define SFC_NAND_ECCS1                  (0x20)
#define SFC_NAND_ECCS                   (SFC_NAND_ECCS0 | SFC_NAND_ECCS1)

/* F0H: bit 定义 */
#define SFC_NAND_ECCE0                  (0x10)
#define SFC_NAND_ECCE1                  (0x20)
#define SFC_NAND_ECCE                   (SFC_NAND_ECCE0 | SFC_NAND_ECCE1)

/* 最小擦除扇区 */
#define SFC_ERASE_SIZE                  (4 * 1024)
#define SFC_PAGE_SIZE                   (256)


/* 错误码定义 */

/* 坏块 */
#define SFC_EBAD_BLK                    TS_DEFINE_ERRNO(TS_LOG_ERR, TS_IOC_MAGIC_SFC, 0)

/* 发生 ECC 错误，但是可以纠正 */
#define SFC_EECC_CORR                   TS_DEFINE_ERRNO(TS_LOG_ERR, TS_IOC_MAGIC_SFC, 1)

/* 发送 ECC 错误，且无法纠正 */
#define SFC_EECC_FAIL                   TS_DEFINE_ERRNO(TS_LOG_ERR, TS_IOC_MAGIC_SFC, 2)


/* 文件名最大长度 */
#define SFFS_MAX_FN_LEN         (32)

/* 最大文件个数 */
#define SFFS_MAX_FILE_CNT       (256)


typedef struct sfc_fs_file_s sfc_fs_file_s_t;

/* 文件 flag 定义 */

/* 文件本身为实时写入到 flash, 此参数确定 super 块是否跟随本次写入而同步保存到 flash, 0: 不同步； 1: 同步 */
#define SFFS_SUPER_SYNC         (1 << 0)

/* 文件是否强制锁定映射，即除非发生坏块或者 ECC 等问题，此文件将一直保持初始映射关系
 如果配置了此标志，SFFS_FF_SUPER_SYNC 将被忽略 */
#define SFFS_FIXED_MAPPING      (1 << 4)

/* 强制此文件独占所有块空间(按照此文件大小计算) */
#define SFFS_EXCLUSIVE          (1 << 8)

/* 强制此文件地址偏移计算为块对齐 */
#define SFFS_ALIGN_BLOCK        (1 << 12)

/* 强制此文件保存到 super 区域, 可以进行 ts_sfc_mmap 进行文件映射 */
#define SFFS_IN_SUPER           (1 << 16)

/* 只读 */
#define SFFS_READ_ONLY          (1 << 20)

#define SFFS_ALIGN_BLK_MASK     (SFFS_EXCLUSIVE | SFFS_ALIGN_BLOCK)

typedef struct sfc_fs_file_s {
    /* 文件名 */
    const ts_char *pc_fname;

    /* 偏移地址，自动计算获得 */
    ts_u32 u32_offset;

    /* 用户指定大小，必须 4-byte 对齐 */
    ts_u32 u32_size;

    /* 定义参见 : SFFS_SUPER_SYNC, SFFS_FIXED_MAPPING, SFFS_EXCLUSIVE, SFFS_ALIGN_BLOCK */
    ts_u32 u32_flag;

    /* 读操作的回调函数，用户可以在此函数实现默认值、合法性校验等功能 */
    ts_s32 (*read_cb)(ts_void *pv_rbuf, ts_u32 u32_size);
} sfc_fs_file_s_t;

/* 用于擦除保护，软件性质，非 FLASH 内部保护 */
typedef struct sfc_protect_s {
    /* 此配置是否有效，需配置成 1， 只有最后一个配置成 0， 代表配置结束 */
    ts_u16 u16_vld;

    /* 此保护是否使能 */
    ts_u16 u16_en;

    /* 开始块地址,包括此块 */
    ts_u16 u16_start_blk;

    /* 结束块地址，包括此块 */
    ts_u16 u16_end_blk;
} sfc_protect_s_t;


/* --- function list --- */

/* 
 * 函数名 :  ts_lib_sfc_init
 * 描  述 :  初始化 sfc, 同时可支持内置和外置 Flash 各一个
 * 输  入 :  参数               描述
 *          pc_root             挂载的文件路径
 *          pst_inner           内置 flash 配置，为必须参数
 *          pst_outer           外置 flash 配置，为可选参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              初始化成功
 *           TS_ERR             初始化失败
 */
ts_s32 ts_lib_sfc_init(const ts_char *pc_root, sffs_cfg_s_t *pst_inner, sffs_cfg_s_t *pst_outer);


/* 
 * 函数名 :  ts_lib_sfc_deinit
 * 描  述 :  sfc 去初始化
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              去初始化成功
 */
ts_s32 ts_lib_sfc_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_sfc_open
 * 描  述 :  打开一个文件
 * 输  入 :  参数               描述
 *           pc_fname           文件名
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_ERR             打开文件失败，或者无此文件
 *          > 0                 文件句柄
 */
ts_s32 ts_lib_sfc_open(ts_char *pc_fname);


/* 
 * 函数名 :  ts_lib_sfc_close
 * 描  述 :  关闭一个文件
 * 输  入 :  参数               描述
 *           s32_fhdl           文件句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_sfc_close(ts_s32 s32_fhdl);


/* 
 * 函数名 :  ts_lib_sfc_sync
 * 描  述 :  将 SFFS super 块同步到 Flash
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              同步成功
 *          TS_ERR              同步失败
 */
ts_s32 ts_lib_sfc_sync(ts_void);


/* 
 * 函数名 :  ts_lib_sfc_mmap
 * 描  述 :  将 Flash 中的文件映射到内存中，可以对此映射后的地址进行读写操作
 * 输  入 :  参数               描述
 *          s32_fhdl            文件句柄
 *          u32_offset         文件内部偏移量
 *          u32_size            映射大小
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            映射失败
 *           非 ts_null         映射成功
 */
ts_void *ts_lib_sfc_mmap(ts_s32 s32_fhdl, ts_u32 u32_offset, ts_u32 u32_size);


/* 
 * 函数名 :  ts_lib_sfc_unmmap
 * 描  述 :  解除映射关系
 * 输  入 :  参数               描述
 *          pv_mmap_buf         映射缓冲首地址
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_sfc_unmmap(ts_void *pv_mmap_buf);


/* 
 * 函数名 :  ts_lib_sfc_write
 * 描  述 :  通过文件方式向 Flash 写数据
 * 输  入 :  参数               描述
 *          s32_fhdl            文件句柄
 *          u32_addr            文件内部偏移地址，默认为 0，且不能大于文件大小, 需 4 字节对齐
 *          pu08_wr_buf         写入缓冲，需 4 字节对齐
 *          u32_wr_size         写入大小，需 4 字节对齐
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  写入失败
 *          > 0                 写入个数
 */
ts_s32 ts_lib_sfc_write(ts_s32 s32_fhdl, ts_u32 u32_addr, ts_u08 *pu08_wr_buf, ts_u32 u32_wr_size);


/* 
 * 函数名 :  ts_lib_sfc_read
 * 描  述 :  通过文件方式从 Flash 读数据
 * 输  入 :  参数               描述
 *          s32_fhdl            文件句柄
 *          u32_addr            文件内部偏移地址，默认为 0，且不能大于文件大小, 需 4 字节对齐
 *          u32_wr_size         读大小，需 4 字节对齐
 * 输  出 :  参数               描述
 *          pu08_rd_buf         读缓冲，需 4 字节对齐
 * 返  回 :  返回值             描述
 *           0                  读失败
 *          > 0                 读个数
 */
ts_s32 ts_lib_sfc_read(ts_s32 s32_fhdl, ts_u32 u32_addr, ts_u08 *pu08_rd_buf, ts_u32 u32_rd_size);


/* 
 * 函数名 :  ts_lib_sfc_set_protect
 * 描  述 :  设置保护使能，不能增加区域，只能对已有区域生效
 * 输  入 :  参数               描述
 *          u32_cs              片选，0: 内部 flash; 1: 外部 flash
 *          u32_blk_addr        块地址
 *          u32_en              0 : 关闭， > 0 生效
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              完成设置
 *           TS_ERR             设置失败
 */
ts_s32 ts_lib_sfc_set_protect(ts_u32 u32_cs, ts_u32 u32_blk_addr, ts_u32 u32_en);


/* 
 * 函数名 :  ts_lib_sfc_raw_write
 * 描  述 :  直接向 Flash 写数据, 写入前擦除块
 * 输  入 :  参数               描述
 *          u32_addr            flash 线性地址，最大支持内外各一片 flash, 需 4 字节对齐
 *          pu08_wr_buf         写入缓冲，需 4 字节对齐
 *          u32_wr_size         写入大小，需 4 字节对齐
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  写入失败
 *          > 0                 写入个数
 */
ts_u32 ts_lib_sfc_raw_write(ts_u32 u32_addr, ts_u08 *pu08_wr_buf, ts_u32 u32_wr_size);


/* 
 * 函数名 :  ts_lib_sfc_raw_write_no_erase
 * 描  述 :  直接向 Flash 写数据, 只写入不擦除
 * 输  入 :  参数               描述
 *          u32_addr            flash 线性地址，最大支持内外各一片 flash, 需 4 字节对齐
 *          pu08_wr_buf         写入缓冲，需 4 字节对齐
 *          u32_wr_size         写入大小，需 4 字节对齐
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  写入失败
 *          > 0                 写入个数
 */
ts_u32 ts_lib_sfc_raw_write_no_erase(ts_u32 u32_addr, ts_u08 *pu08_wr_buf, ts_u32 u32_wr_size);


/* 
 * 函数名 :  ts_lib_sfc_raw_read
 * 描  述 :  通过 sfc 读取 flash 的原始数据
 * 输  入 :  参数               描述
 *          u32_addr            flash 线性地址，最大支持内外各一片 flash, 需 4字节对齐
 *          u32_rd_size         要读的个数，需 4 字节对齐
 * 输  出 :  参数               描述
 *           pu08_rd_buf        数据缓冲，需 4 字节对齐
 * 返  回 :  返回值             描述
 *           0                  读失败
 *           > 0                读个数
 */
ts_u32 ts_lib_sfc_raw_read(ts_u32 u32_addr, ts_u08 *pu08_rd_buf, ts_u32 u32_rd_size);


/* 
 * 函数名 :  ts_lib_sfc_raw_erase
 * 描  述 : 指定地址和的大小直接擦除 Flash 
 * 输  入 :  参数               描述
 *          u32_addr            flash 线性地址，最大支持内外各一片 flash, 需 4字节对齐
 *          u32_size            擦除大小
 *          u32_force           是否强制擦除，此参数会强制擦除被标注成坏块的空间，而不是跳过
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  擦除失败
 *           > 0                擦除大小，块对齐
 */
ts_u32 ts_lib_sfc_raw_erase(ts_u32 u32_addr, ts_u32 u32_size, ts_u32 u32_force);


/* 
 * 函数名 :  ts_lib_sfc_raw_chk_blk
 * 描  述 : 查看指定地址所在的块是否是坏块，仅对 SPI-NAND 生效
 * 输  入 :  参数               描述
 *          u32_addr            flash 线性地址，最大支持内外各一片 flash, 需 4字节对齐
 *          u32_size            擦除大小
 *          u32_force           是否强制擦除，此参数会强制擦除被标注成坏块的空间，而不是跳过
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           -SFC_EBAD_BLK      坏块
 *          TS_ERR              读错误
 *           TS_OK              正常块
 */
ts_s32 ts_lib_sfc_raw_chk_blk(ts_u32 u32_addr);


/* 
 * 函数名 :  ts_lib_sfc_get_size
 * 描  述 :  获取文件的大小
 * 输  入 :  参数               描述
 *          s32_fhdl            文件句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  获取失败
 *           > 0                文件大小
 */
ts_s32 ts_lib_sfc_get_size(ts_s32 s32_fhdl);


/* 
 * 函数名 :  ts_lib_sfc_clear
 * 描  述 :  此操作将会把 Flash 中所有 super 块删除，且不会写入新的 super 块，操作需慎重
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_sfc_clear(ts_void);


/* 
 * 函数名 :  ts_lib_sfc_get_type
 * 描  述 :  获取内部 Flash 类型， 0:未知; 1:NOR; 2:NAND
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_u32 ts_lib_sfc_get_type(ts_void);


/* test */
ts_void ts_lib_sfc_raw_mark_bad(ts_u32 u32_addr);
ts_void snand_set_ecc_hmap(ts_void *pv_map);
ts_void snand_add_ecc_blk(ts_u32 u32_outer, ts_u32 u32_blk_addr);
ts_void snand_show_ecc_map(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_SFC_V100_H__ */

