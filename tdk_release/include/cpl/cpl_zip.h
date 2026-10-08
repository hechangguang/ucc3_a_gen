/*
 * cpl_zip.h -- 压缩和解压头文件
 */
#ifndef __CPL_ZIP_H__
#define __CPL_ZIP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 压缩等级 */
#define TS_CPL_ZIP_MIN_LVL          (0)
#define TS_CPL_ZIP_MAX_LVL          (5)

/* 压缩类型 */
#define TS_CPL_ZIP_TYPE_AUTO        (0)
#define TS_CPL_ZIP_TYPE_MFR_S16     (1)

/* mfr 帧 */
typedef struct ts_zip_mfr_zone_s {
    ts_u32 u32_gm;
    ts_u32 u32_size;
} ts_zip_mfr_zone_s_t;

/* 分区,各个压缩类型共用 */
typedef union ts_zip_zone_un {
    ts_zip_mfr_zone_s_t st_mfr_zone[12];
    ts_u32 au32_zones[24];
} ts_zip_zone_un_t;

#define TS_CPL_ZIP_MAGIC        (0xA57A7601)
typedef struct ts_zip_file_s {
    /* 魔术字 */
    ts_u32 u32_magic;

    /* 压缩类型 */
    ts_u08 u08_type;

    /* 压缩等级 */
    ts_u08 u08_lvl;

    /* 分辨率 */
    ts_u16 u16_width;
    ts_u16 u16_height;

    /* 帧数 */
    ts_u08 u08_frm_cnt;
    ts_u08 u08_rev;

    ts_u32 au32_rev[5];

    ts_zip_zone_un_t un_zone;
} ts_zip_file_s_t;


/* 
 * 函数名 :  ts_cpl_zip_mfr_s16
 * 描  述 :  mfr 压缩方式，支持 s16 数据格式
 * 输  入 :  参数               描述
 *          ps16_ref            参考帧输入
 *          ps16_ibuf           待压缩数据输入
 *          u32_width           图像宽
 *          u32_height          图像高
 * 输  出 :  参数               描述
 *          ps16_obuf           压缩后数据
 *          pst_mfr_z           压缩后参数，包括 gm 和数据大小
 * 返  回 :  返回值             描述
 *          无                  无
 */
ts_void ts_cpl_zip_mfr_s16(ts_s16 *ps16_ref, ts_s16 *ps16_ibuf, ts_s16 *ps16_obuf, \
                            ts_u32 u32_width, ts_u32 u32_height, ts_zip_mfr_zone_s_t *pst_mfr_z);


/* 
 * 函数名 :  ts_cpl_unzip_mfr_s16
 * 描  述 :  mfr 解压方式，支持 s16 数据格式
 * 输  入 :  参数               描述
 *          ps16_ref            参考帧输入，注意：并最终解压数据输出到此缓冲
 *          ps16_ibuf           待解压数据输入
 *          u32_width           图像宽
 *          u32_height          图像高
 *          u32_gm              压缩系数
 * 输  出 :  参数               描述
 *          ps16_obuf           解压临时数据
 * 返  回 :  返回值             描述
 *          无                  无
 */
ts_void ts_cpl_unzip_mfr_s16(ts_s16 *ps16_ref, ts_s16 *ps16_ibuf, ts_s16 *ps16_obuf, \
                            ts_u32 u32_width, ts_u32 u32_height, ts_u32 u32_gm);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CPL_ZIP_H__ */

