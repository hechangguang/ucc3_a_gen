/*
 * vpo_cut.c -- 剪切放大功能示例
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
#include <uapi/usb/usb_dc.h>
#include <uapi/base_v100.h>
#include "api.h"
#include "snr_comm.h"

static ts_void cut_help(ts_void)
{
    printf("cut cutedWidth cutedHeight cutStartH cutStartV hMirror vMirror\n");
}

TS_DECLARE_CMD(cut)
{
    ts_vpo_grp_attr_s_t st_attr;

    if (argc < 6){
        cut_help();
    }else{
        ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
        st_attr.u16_cut_width = atoi(argv[1]);//cuted width
        st_attr.u16_cut_height = atoi(argv[2]);//cuted height
        st_attr.u16_cut_start_h = atoi(argv[3]);//horizontal cut start position
        st_attr.u16_cut_start_v = atoi(argv[4]);//vertical cut start position
        st_attr.u08_hmirr_en = atoi(argv[5]);//horizontal mirror
        st_attr.u08_vmirr_en = atoi(argv[6]);//vertical mirror
        ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0, &st_attr);
        ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
    }
    return 0;
}

static ts_void yuvraw_en_help(ts_void)
{
    printf("yuvraw_en yuv_en raw_en\n");
}

TS_DECLARE_CMD(yuvraw_en)
{
    ts_vpo_grp_attr_s_t st_attr;

    if (argc < 3){
        yuvraw_en_help();
    }else{
        ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);
        st_attr.u32_yuv_out_ch1_sel = (atoi(argv[1])>0) ? TS_VPO_MEM_MDEV : TS_VPO_NULL_MDEV;
        st_attr.u32_raw_out_ch_sel = (atoi(argv[2])>0) ? TS_VPO_MEM_MDEV : TS_VPO_NULL_MDEV;
        ts_tdk_vpo_set_grp_attr(TS_VPO_GRP0,&st_attr);
        ts_tdk_vpo_enable_grp(TS_VPO_GRP0);
    }
    return 0;

}

static ts_void cut_help_ratio(ts_void)
{
    printf("zoom type ratio hmirr_en vmirr_en; cut 0..4 10..70 0..1 0..1\n");
}

static ts_void cut_help_roi(ts_void)
{
    printf("zoom 5 x y width height hmirr_en vmirr_en\n");
}


TS_DECLARE_CMD(zoom)
{
    ts_vpo_zoom_attr_s_t st_attr;

    ts_cpl_memset32((ts_u32 *)&st_attr, 0, (sizeof(ts_vpo_zoom_attr_s_t) >> 2));
    if (5 == argc) {
        st_attr.en_type = (ts_vpo_zoom_en_t)atoi(argv[1]);
        if (st_attr.en_type < TS_VPO_ZOOM_ROI) {
            st_attr.u16_ratio = atoi(argv[2]);
            st_attr.u08_hmirr_en = atoi(argv[3]);
            st_attr.u08_vmirr_en = atoi(argv[4]);
            ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);
        } else {
            cut_help_ratio();
        }
    } else if (8 == argc) {
        st_attr.en_type = (ts_vpo_zoom_en_t)atoi(argv[1]);
        if (TS_VPO_ZOOM_ROI == st_attr.en_type) {
            st_attr.u16_roi_x = atoi(argv[2]);
            st_attr.u16_roi_y = atoi(argv[3]);
            st_attr.u16_roi_width = atoi(argv[4]);
            st_attr.u16_roi_height = atoi(argv[5]);

            st_attr.u08_hmirr_en = atoi(argv[6]);
            st_attr.u08_vmirr_en = atoi(argv[7]);
            ts_tdk_vpo_cut_zoom_in(TS_VPO_GRP0, &st_attr);
        } else {
            cut_help_roi();
        }
    } else {
        cut_help_ratio();
        
    }


    return 0;
}

ts_void vpo_disable_all(ts_void)
{
    ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);
    ts_tdk_vpo_disable_chn(TS_VPO_BTENC_MDEV);
    ts_tdk_vpo_disable_chn(TS_VPO_MEM_MDEV);
    ts_tdk_vpo_disable_chn(TS_VPO_LCD_MDEV);
    ts_tdk_vpo_disable_chn(TS_VPO_MIPI_CSI_MDEV);
    ts_tdk_vpo_disable_chn(TS_VPO_MIPI_DSI_MDEV);
    ts_tdk_vpo_disable_grp(TS_VPO_MDEV0);
    //usbd_deinitialize();
}

#ifdef CONFIG_UVC
extern ts_void usb_capture_init(ts_void);
#endif

#ifdef CONFIG_LIBUSB
extern ts_void usb_libusb_init(ts_void);
#endif


TS_DECLARE_CMD(vpo)
{
    ts_vpo_grp_attr_s_t st_attr;
    snr_attr_s_t *pst_attr;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    ts_tdk_vpo_get_grp_attr(TS_VPO_GRP0, &st_attr);

    vpo_disable_all();

    if(!ts_cpl_strcmp(argv[1], "cvbs")) {
        usbd_set_plug(1);

        ts_cpl_msleep(200);

        pst_attr->st_cvbs.u08_en = 1;
        pst_attr->st_cvbs.u08_sd_en = 1;
        pst_attr->st_cvbs.u08_orgi_en = 0;
        pst_attr->st_cvbs.u08_pal_en = 1;

        pst_attr->st_bte.u08_en = 0;
        pst_attr->st_mipi.u08_en = 0;
        pst_attr->u32_mem_en = 0;

        snr_cfg_vpo(pst_attr);

        /* 关闭 usb， 此处需要配合实际应用实现, 关闭后枚举也不能进行 */
        usb_dc_deinit();
    } else if(!ts_cpl_strcmp(argv[1], "usb")) {
        pst_attr->st_cvbs.u08_en = 0;
        pst_attr->st_bte.u08_en = 0;
        pst_attr->st_mipi.u08_en = 0;
        pst_attr->u32_mem_en = 1;

        snr_cfg_vpo(pst_attr);

        /* 重新配置 usb , 需要根据不同类型进行调用 */
        #ifdef CONFIG_UVC
            usb_capture_init();
        #endif

        #ifdef CONFIG_LIBUSB
            usb_libusb_init();
        #endif

    } else if(!ts_cpl_strcmp(argv[1], "bte")) {
        pst_attr->st_cvbs.u08_en = 0;
        
        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 0;
        pst_attr->st_bte.u08_external_sync_en = 1;

        pst_attr->st_mipi.u08_en = 0;
        pst_attr->u32_mem_en = 0;

        snr_cfg_vpo(pst_attr);
    } else if(!ts_cpl_strcmp(argv[1], "mipicsi")) {

        pst_attr->st_cvbs.u08_en = 0;
        pst_attr->st_bte.u08_en = 0;

        pst_attr->st_mipi.u08_en = 1;
        pst_attr->st_mipi.u08_dsi_en = 0;
        pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
        pst_attr->st_mipi.u08_dphy_lane_num = 4;
        pst_attr->st_mipi.u16_data_rate = 500;

        pst_attr->u32_mem_en = 0;

        snr_cfg_vpo(pst_attr);
    } else if(!ts_cpl_strcmp(argv[1], "cvbs_usb")) {
        pst_attr->st_cvbs.u08_en = 1;
        pst_attr->st_cvbs.u08_sd_en = 1;
        pst_attr->st_cvbs.u08_orgi_en = 0;
        pst_attr->st_cvbs.u08_pal_en = 1;

        pst_attr->st_bte.u08_en = 0;
        pst_attr->st_mipi.u08_en = 0;
        pst_attr->u32_mem_en = 1;

        snr_cfg_vpo(pst_attr);
    } else if(!ts_cpl_strcmp(argv[1], "cvbs_bte")) {
        pst_attr->st_cvbs.u08_en = 1;
        pst_attr->st_cvbs.u08_sd_en = 1;
        pst_attr->st_cvbs.u08_orgi_en = 0;
        pst_attr->st_cvbs.u08_pal_en = 1;

        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 0;
        pst_attr->st_bte.u08_external_sync_en = 1;

        pst_attr->st_mipi.u08_en = 0;
        pst_attr->u32_mem_en = 0;

        snr_cfg_vpo(pst_attr);

        /* 重新配置 usb , 需要根据不同类型进行调用 */
            #ifdef CONFIG_UVC
            usb_capture_init();
        #endif

        #ifdef CONFIG_LIBUSB
            usb_libusb_init();
        #endif

    } else if(!ts_cpl_strcmp(argv[1], "cvbs_mipicsi")) {
        pst_attr->st_cvbs.u08_en = 1;
        pst_attr->st_cvbs.u08_sd_en = 1;
        pst_attr->st_cvbs.u08_orgi_en = 0;
        pst_attr->st_cvbs.u08_pal_en = 1;

        pst_attr->st_mipi.u08_en = 1;
        pst_attr->st_mipi.u08_dsi_en = 0;
        pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
        pst_attr->st_mipi.u08_dphy_lane_num = 4;
        pst_attr->st_mipi.u16_data_rate = 500;

        pst_attr->st_bte.u08_en = 0;
        pst_attr->u32_mem_en = 0;

        snr_cfg_vpo(pst_attr);
    } else {
        printf("Unsupport CMD\n");
    }

    

    return TS_OK;
}

