/*
 * templut.c -- temperature lookup table test
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <command.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cpl/ts_cpl.h>
#include <uapi/vpo_v100.h>
#include <uapi/chip_def.h>
#include <math.h>
#include <api.h>

static ts_void tempLut_help(ts_void)
{
    vision_log_debug("tl testCaseId.\n");
}

extern ts_s32 ts_tdk_vpo_set_templut_attr(ts_mdev_t h_vpo, const ts_vpo_templut_attr_s_t *pst_attr);

static ts_u32 u32_lutAddr = 0;

TS_DECLARE_CMD(tl)
{
    ts_vpo_templut_attr_s_t st_templut_attr;

    ts_u32 lutMaxSize = 4096*2;
    ts_u16* pu16_lutAddr;
    ts_u08 u08_source_sel = 0;
    ts_u16 u16_b1 = 0;
    if(0 == u32_lutAddr){
        //u32_lutAddr = (ts_u32)malloc(lutMaxSize);
        u32_lutAddr = (ts_u32)ts_cpl_tmm_malloc(ts_null, lutMaxSize, 256, "tempLutAddr");
        if(0 == u32_lutAddr)
            vision_log_debug("templut addr is null..\n");
    }
    pu16_lutAddr = (ts_u16*)u32_lutAddr;

    if (argc < 2){
        tempLut_help();
    }else{
        switch (atoi(argv[1]))
        {
        case 0:
            vision_log_debug("templut close test.\n");
            if (argc ==3){
                u08_source_sel = atoi(argv[2]);
            }

            if (argc ==4){
                u16_b1 = atoi(argv[3]);
            }
            st_templut_attr.u08_lut_update_en = 0;
            st_templut_attr.u16_mk = 8192;
            st_templut_attr.u16_mb = 0;
            st_templut_attr.u16_b1 = u16_b1;
            st_templut_attr.u32_enable = 0;
            st_templut_attr.u32_lut_base_addr = u32_lutAddr;
            st_templut_attr.u08_source_sel = u08_source_sel;
            vision_log_debug("st_templut_attr.u08_source_sel %d .\n",st_templut_attr.u08_source_sel);
            ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0,&st_templut_attr);
            break;

        case 1:
            vision_log_debug("templut 4096 seq test.\n");
            if (argc ==3){
                u08_source_sel = atoi(argv[2]);
            }

            if (argc ==4){
                u16_b1 = atoi(argv[3]);
            }
            st_templut_attr.u08_lut_update_en = 1;
            st_templut_attr.u16_mk = 8192;
            st_templut_attr.u16_mb = 0;
            st_templut_attr.u16_b1 = u16_b1;
            st_templut_attr.u32_enable = 1;
            st_templut_attr.u32_lut_base_addr = u32_lutAddr;
            st_templut_attr.u08_source_sel = u08_source_sel;
            for(ts_u32 i =0;i<4096;i++){
                pu16_lutAddr[i] = i * 4;
            }
            ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0,&st_templut_attr);
            vision_log_debug("st_templut_attr.u08_source_sel %d .\n",st_templut_attr.u08_source_sel);
            break;

        case 2:
            vision_log_debug("templut 4096 rev test.\n");
            st_templut_attr.u08_lut_update_en = 1;
            st_templut_attr.u16_mk = 8192;
            st_templut_attr.u16_mb = 0;
            st_templut_attr.u16_b1 = 0;
            st_templut_attr.u32_enable = 1;
            st_templut_attr.u32_lut_base_addr = u32_lutAddr;
            st_templut_attr.u08_source_sel = 0;
            for(ts_u32 i =0;i<4096;i++){
                pu16_lutAddr[i] = 16383 - (i * 4);
            }
            ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0,&st_templut_attr);
            break;

        case 3:
            vision_log_debug("templut 4096 high contrast to seq test.\n");
            st_templut_attr.u08_lut_update_en = 1;
            st_templut_attr.u16_mk = 8192;
            st_templut_attr.u16_mb = 0;
            st_templut_attr.u16_b1 = 0;
            st_templut_attr.u32_enable = 1;
            st_templut_attr.u32_lut_base_addr = u32_lutAddr;
            st_templut_attr.u08_source_sel = 1;
            ts_f32 coef = atof(argv[2]);
            for(ts_u32 i =0;i<4096;i++){
                ts_f32 f32_x = (ts_f32)i / 4096.0;
                ts_f32 f32_y = pow(f32_x,coef);
                f32_y = (f32_y /( f32_y + pow(1-f32_x,coef))) * 16384;
                pu16_lutAddr[i] = (int)f32_y;
            }
            ts_tdk_vpo_set_templut_attr(TS_VPO_MDEV0,&st_templut_attr);
            break;

        default:
            vision_log_debug("Unsupported templut test mode.\n");
            break;
        }
    }

    return 0;
}
