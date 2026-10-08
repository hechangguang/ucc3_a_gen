/*
 * afs.c -- 聚焦算子获取使用例子
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <command.h>
#include <stdio.h>
#include <api.h>

ts_void afs_init(ts_void)
{
    ts_isp_afs_attr_s_t afs_attr;
    ts_isp_pipe_attr_s_t pipe_attr;
    ts_s32 s32_ret;

    s32_ret = ts_tdk_isp_get_afs_attr(TS_ISP_MDEV0, (ts_isp_afs_attr_s_t *)&afs_attr); 

    afs_attr.u08_af_en = 1;
    s32_ret = ts_tdk_isp_set_afs_attr(TS_ISP_MDEV0, (ts_isp_afs_attr_s_t *)&afs_attr); 

    pipe_attr.en_pipe_module_sel = TS_ISP_PIPE_AFS_SEL;
    pipe_attr.u08_mod_pos = 1;
    ts_tdk_isp_set_pipe_pos(TS_ISP_MDEV0, &pipe_attr);

    afs_Adjust();

    vision_log_debug("afs init done, ret = %d!\n", s32_ret);
}

ts_void afs_Adjust(ts_void)
{
    ts_s32 s32_thr1 = 0;  
    ts_s32 s32_delta = 5;
    ts_s32 s32_coef1 = 0;  ///高频太少认为是噪声， 进行抑制
    ts_s32 s32_coef2 = 64;

    ts_isp_afs_attr_s_t st_af_attr;
    ts_tdk_isp_get_afs_attr(TS_ISP_MDEV0, &st_af_attr);
    st_af_attr.st_hf_adj_h.u08_hf_coef1 = s32_coef1;
    st_af_attr.st_hf_adj_h.u08_hf_coef2 = s32_coef2;
    st_af_attr.st_hf_adj_h.u32_hf_thr1 = s32_thr1;
    st_af_attr.st_hf_adj_h.u08_hf_delta1 = s32_delta;
    ts_tdk_isp_set_afs_attr(TS_ISP_MDEV0, &st_af_attr);    
    printf("afs flt_param set to %d, %d, %d, %d \n", s32_thr1, s32_delta, s32_coef1, s32_coef2);  

}

ts_void afs_calc_mean(ts_void)
{
    ts_cpl_timeval_s_t st_time0, st_time1;
    ts_isp_af_statis_s_t st_af_statis;
    ts_s32 s32_ret; 

    s32_ret = ts_tdk_isp_get_af_statis(TS_ISP_MDEV0, (ts_isp_af_statis_s_t *)&st_af_statis, 2000);
    if (s32_ret != TS_OK) {
        vision_log_debug("get af res err! \n");
    } else {
        ts_cpl_gettimeofday(&st_time0);   
        vision_log_debug("get af res ok !\n");
        ts_s64 s64_sum_h1 = 0;
        ts_s64 s64_sum_h2 = 0;

        ts_s64 s64_sum_v1 = 0;
        ts_s64 s64_sum_v2 = 0;

        ts_s64 s64_abs_h = 0;
        ts_s64 s64_abs_v = 0;

        ts_s64 s64_sum_aver = 0;
        for (ts_s32 i = 0; i < st_af_statis.u16_blk_num_h * st_af_statis.u16_blk_num_w; i++) {
            s64_sum_h1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1);
            s64_sum_h2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2);

            s64_sum_v1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1);
            s64_sum_v2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2);

            s64_abs_v += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2));
            s64_abs_h += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2));
            s64_sum_aver += (ts_s64)st_af_statis.pst_af_statis_info[i].u16_aver;
        }
        ts_cpl_gettimeofday(&st_time1);
        vision_log_debug("afs  %d us : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec));
        vision_log_debug("af info num = [%d, %d], addr = 0x%x, af mean = h:[%d, %d, %d], v:[%d, %d: %d], aver:[%d]\n", st_af_statis.u16_blk_num_h, st_af_statis.u16_blk_num_w,
                                                (ts_u32)st_af_statis.pst_af_statis_info, 
                                                (ts_s32)(s64_sum_h1 / 1024), 
                                                (ts_s32)(s64_sum_h2 / 1024), 
                                                (ts_s32)(s64_abs_h / 1024), 
                                                (ts_s32)(s64_sum_v1 / 1024), 
                                                (ts_s32)(s64_sum_v2 / 1024), 
                                                (ts_s32)(s64_abs_v / 1024), 
                                                (ts_s32)(s64_sum_aver /1024));

        // vision_log_debug("Focus_Value/1024 = [%d]\n]", (ts_s32)((s64_abs_v+s64_abs_h)/1024));
        // vision_log_debug("Focus_Value/512 = [%d]\n]", (ts_s32)((s64_abs_v+s64_abs_h)/512));
        // vision_log_debug("Focus_Value_L = [%lld]\n]", (s64_abs_v+s64_abs_h));
    }

    // ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, st_af_statis.pv_private);
    ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, &st_af_statis);
}

// 100帧统计加计算
ts_void afs_calc_mean_100(ts_void)
{
    ts_cpl_timeval_s_t st_time0, st_time1;
    ts_isp_af_statis_s_t st_af_statis;
    ts_s32 s32_ret; 
    ts_cpl_gettimeofday(&st_time0);
    for(ts_s32 j = 0; j < 100; j++){
        s32_ret = ts_tdk_isp_get_af_statis(TS_ISP_MDEV0, (ts_isp_af_statis_s_t *)&st_af_statis, 2000);
        if (s32_ret != TS_OK) {
            printf("get af res err! \n");
        } else {
            // printf("get af res ok !\n");
            ts_s64 s64_sum_h1 = 0;
            ts_s64 s64_sum_h2 = 0;

            ts_s64 s64_sum_v1 = 0;
            ts_s64 s64_sum_v2 = 0;

            ts_s64 s64_abs_h = 0;
            ts_s64 s64_abs_v = 0;

            ts_s64 s64_sum_aver = 0;
            

            for (ts_s32 i = 0; i < st_af_statis.u16_blk_num_h * st_af_statis.u16_blk_num_w; i++) {
                s64_sum_h1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1);
                s64_sum_h2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2);

                s64_sum_v1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1);
                s64_sum_v2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2);

                s64_abs_v += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2));
                s64_abs_h += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2));
                s64_sum_aver += (ts_s64)st_af_statis.pst_af_statis_info[i].u16_aver;
            }
        }
        ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, &st_af_statis);
    }
    ts_cpl_gettimeofday(&st_time1);
    vision_log_debug("afs_100  %d ms : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec) / 1000);

}

ts_void afs_help(ts_void)
{
    vision_log_debug("afs -i\n");
    vision_log_debug("afs -m\n");

}

TS_DECLARE_CMD(afs)
{
    if (argc > 1)  {
        if ('i' == argv[1][1]) {
            afs_init();
        }else if ('m' == argv[1][1]) {
            afs_calc_mean();
        }else if ('n' == argv[1][1]) {
            afs_calc_mean_100();       //100帧统计加计算
        }
        else 
            afs_help();
    } else {
        afs_help();
    }

    return 0;
}
