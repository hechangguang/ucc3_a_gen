/*
 * libupload.c -- tdk 使用 upload 模块导出 y8/y16 数据进行校正等计算例子, 类驱动态，避开 ioctl 以及直接使用驱动接口
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/vpo_v100.h>
#include "libupload.h"
#include <command.h>

typedef struct libup_ctx_s {
    /* 节点动态申请，此处保存节点首地址 */
    ts_nodebuf_s_t *past_node;

    /* 保存当前的 upload 节点 */
    ts_u32 u32_pos;

    /* 当前图像宽高信息 */
    ts_u16 u16_width;
    ts_u16 u16_height;

    /* 节点数 */
    ts_u32 u32_node_cnt;

    /* 指定获取的帧数 */
    ts_u32 u32_frm_cnt;

    /* 获取计数，不能大于 u32_frm_cnt */
    ts_u32 u32_get_cnt;

    /* 节点链表 */
    ts_cpl_list_head_s_t st_ready_head;

    /* isp 设备句柄 */
    ts_void *pv_isp;

    /* 同步信号 */
    ts_cpl_semphr_s_t st_sema;

    /* 是否为 DMA 内存 */
    ts_s32 s32_is_dma;
} libup_ctx_s_t;

/* libupload 上下文 */
static libup_ctx_s_t sst_ctx = {0};

extern ts_void tdl_mupl_get_frm_info(ts_u32 *pu32_width, ts_u32 *pu32_height, ts_u32 *pu32_fps);

static ts_void libup_release_node(ts_nodebuf_s_t *pst_node)
{
    ts_u32 i, u32_cnt;
    ts_nodebuf_s_t *pst_cur_node;
    ts_cpl_list_head_s_t *pst_cur_list;

    if (ts_null == pst_node) {
        /* 释放所有节点 */
        if ((ts_null == sst_ctx.past_node) || (0 == sst_ctx.u32_node_cnt))
            return;

        pst_cur_node = sst_ctx.past_node;
        u32_cnt = sst_ctx.u32_node_cnt;
    } else {
        pst_cur_node = pst_node;
        u32_cnt = 1;
    }

    for (i = 0; i < u32_cnt; i++) {
        pst_cur_list = (ts_cpl_list_head_s_t *)&pst_cur_node->st_list;

        if (0 == sst_ctx.s32_is_dma) {
            ts_cpl_dma_unmap_single(ts_null, pst_cur_node->u32_phy_addr, \
                        pst_cur_node->u32_size, TS_CPL_DMA_FROM_DEVICE);
        }

        ts_cpl_list_del_init(pst_cur_list);

        pst_cur_node++;
    }
}


/* upload 完成一帧数据后，会调用此回调函数，中断中执行，避免休眠、申请内存等操作 */
static ts_void libup_ready_cb(ts_void *pv_node)
{
    ts_cpl_list_head_s_t *pst_cur_list;

    if (ts_null == pv_node) {
        /* 出现严重错误 */
        ts_cpl_log_err("Get an empty framebuf.\n");
    } else {
        /* 中断内回调，将此节点挂载到准备完成队列 */
        pst_cur_list = (ts_cpl_list_head_s_t *)pv_node;
        ts_cpl_list_add_tail(pst_cur_list, (ts_cpl_list_head_s_t *)&sst_ctx.st_ready_head);
        ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_sema);
    }
}


/*
 * 函数名 :  ts_lib_up_get_buf
 * 描  述 :  获取一帧数据
 * 输  入 :  参数           描述
 *          u32_tmout_ms   超时时间
 * 输  出 :  参数           描述
*           无             无
 * 返  回 :  返回值         描述
 *           ts_null       获取失败
 *          !ts_null       获取的首帧节点句柄
 */
ts_nodebuf_s_t *ts_lib_up_get_node(ts_u32 u32_tmout_ms)
{
    ts_s32 s32_ret;
    ts_nodebuf_s_t *pst_node;

    if (sst_ctx.u32_get_cnt >= sst_ctx.u32_frm_cnt) {
        ts_cpl_log_err("The framebuf has out of range(%d).\n", sst_ctx.u32_frm_cnt);

        return ts_null;
    }

    s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_sema, (ts_long)u32_tmout_ms);
    if (s32_ret != TS_OK) {
        ts_cpl_log_err("Get framebuf timeout.\n");

        return ts_null;
    }

    /* 从完成队列中获取一帧数据 */
    if (0 == ts_cpl_list_empty((ts_cpl_list_head_s_t *)&sst_ctx.st_ready_head)) {
        pst_node = (ts_nodebuf_s_t *)sst_ctx.st_ready_head.next;

        sst_ctx.u32_get_cnt++;

        ts_cpl_list_del_init((ts_cpl_list_head_s_t *)pst_node);

        if (0 == sst_ctx.s32_is_dma) {
            ts_cpl_dma_unmap_single(ts_null, (ts_dma_addr_t)pst_node->u32_phy_addr, \
                        (ts_size_t)pst_node->u32_size, TS_CPL_DMA_FROM_DEVICE);
        }
    } else {
        pst_node = ts_null;

        ts_cpl_log_err("The queue for framebuf is empty.\n");
    }

    return pst_node;
}


/*
 * 函数名 :  ts_lib_up_get_buf
 * 描  述 :  获取指定帧数的准备好的缓冲节点
 * 输  入 :  参数           描述
 *          en_up_type     本次获取帧的类型，单帧或者全部获取
 *          u32_tmout_ms   超时时间
 * 输  出 :  参数           描述
*           无             无
 * 返  回 :  返回值         描述
 *           ts_null       获取失败
 *          !ts_null       获取的首帧节点句柄
 */
ts_void ts_lib_up_release_node(ts_nodebuf_s_t *pst_node)
{
    /* 内部有锁 */
    tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)pst_node, TS_ISP_UPL_SECONDARY_CH);
}



/*
 * 函数名 :  ts_lib_up_stop
 * 描  述 :  结束本次 upload
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK         停止成功
 */
ts_s32 ts_lib_up_stop(ts_void)
{
    if (sst_ctx.past_node != ts_null) {
        libup_release_node(ts_null);

        ts_cpl_kfree(sst_ctx.past_node);

        sst_ctx.past_node = ts_null;
        sst_ctx.u32_node_cnt = 0;
    }

#if 0
    if (sst_ctx.pv_isp != ts_null)
        tdl_isp_stop_upload(sst_ctx.pv_isp);
#endif

    return TS_OK;
}


ts_u32 ts_lib_up_get_frm_info(ts_u32 *pu32_width, ts_u32 *pu32_height, ts_u32 *pu32_bitw)
{
    ts_u32 u32_size, u32_bitw;

    if (pu32_width != ts_null)
        *pu32_width = (ts_u32)sst_ctx.u16_width;

    if (pu32_height != ts_null)
        *pu32_height = (ts_u32)sst_ctx.u16_height;

    u32_size = sst_ctx.u16_width * sst_ctx.u16_height;
    if (sst_ctx.u32_pos < 2) {
        u32_size = u32_size << 1;
        u32_bitw = 2;
    } else {
        u32_bitw = 1;
    }

    if (pu32_bitw != ts_null)
        *pu32_bitw = u32_bitw;

    return u32_size;
}


static ts_void upl_get_frm_info(ts_u32 u32_pos, ts_u32 *pu32_frm_size, ts_u32 *pu32_strid_size)
{
    ts_u32 u32_frm_size, u32_width, u32_height, u32_strid;

    if (u32_pos >= TS_UPL_POS_VPO_YUV)
        tdl_mupl_get_frm_info((ts_u32 *)&u32_width, (ts_u32 *)&u32_height, ts_null);
    else
        tdl_isp_get_frm_info(TS_ISP_MDEV0, (ts_u32 *)&u32_width, (ts_u32 *)&u32_height);

    sst_ctx.u16_width = (ts_u16)u32_width;
    sst_ctx.u16_height = (ts_u16)u32_height;
    sst_ctx.u32_pos = u32_pos;

    u32_frm_size = u32_width * u32_height;
    if (TS_UPL_POS_VPO_YUV == u32_pos) {
        u32_frm_size = u32_frm_size << 1;
        u32_strid = (u32_width * (u32_height + 4)) << 1;
    } else {
        if (u32_pos < TS_UPL_POS_ISP)
            u32_frm_size = u32_frm_size << 1;

        u32_strid = u32_frm_size;
    }

    if (pu32_frm_size != ts_null)
        *pu32_frm_size = u32_frm_size;

    if (pu32_strid_size != ts_null)
        *pu32_strid_size = u32_strid;
}


/*
 * 函数名 :  ts_lib_up_start
 * 描  述 :  启动指定帧数的 y8/y16 数据导出, 如果后续一次性获取全部缓冲，需要配置 (u32_frm_cnt + 1) 大小
 *             的总缓冲，如果单帧获取，需确保计算速度(还回帧存)与填充速度可以避免填充阻塞。
 * 输  入 :  参数           描述
 *          en_up_pos       导出的位置,0,1,2
 *          u32_frm_cnt     导出的帧数
 *          u32_node_cnt    需要配置的节点个数，需要与 (pu32_buf / u32_frm_size) 匹配
 *          s32_is_dma      是否为 DMA 内存空间，如果是，后续不会进行 DMA MAP 和 UNMAP 操作
 * 输  出 :  参数           描述
 *           pu32_buf       视频数据
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_s32 ts_lib_up_start(ts_u32 u32_up_pos, ts_u32 *pu32_buf, ts_u32 u32_frm_cnt, \
            ts_u32 u32_node_cnt, ts_s32 s32_is_dma)
{
    ts_u32 i, u32_frm_size, u32_node_size, u32_strid;
    ts_nodebuf_s_t *pst_node;
    ts_cpl_list_head_s_t *pst_cur_list;
    ts_u08 *pu08_ptr;
    ts_isp_upload_attr_s_t st_up_attr;
    ts_s32 s32_ret;

    tdl_isp_upload_init();

    /* 获取对应 ISP 句柄 */
    if (ts_null == sst_ctx.pv_isp) {
        sst_ctx.pv_isp = tdl_isp_get_dev(TS_ISP_MDEV0);
        if (ts_null == sst_ctx.pv_isp) {
            ts_cpl_log_err("Failed to get upload device.\n");

            return -TS_ENODEV;
        }
    }

    /* 初始化同步信号 */
    if (ts_null == sst_ctx.st_sema.pv_semphr) {
        s32_ret = ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_sema, 0);
        if (s32_ret != TS_OK) {
            ts_cpl_log_err("Failed to init sema.\n");

            return TS_ERR;
        }
    }

    /* 计算当前图像的大小 */
    upl_get_frm_info(u32_up_pos, (ts_u32 *)&u32_frm_size, (ts_u32 *)&u32_strid);

    /* 确保所有节点已经回收 */
    libup_release_node(ts_null);

    /* 确认是否需要重新申请节点 */
    if (u32_node_cnt > sst_ctx.u32_node_cnt) {
        if (sst_ctx.past_node != ts_null) {
            ts_cpl_kfree(sst_ctx.past_node);

            sst_ctx.past_node = ts_null;
        }

        sst_ctx.u32_node_cnt = u32_node_cnt;
    }

    /* 申请节点 */
    if (ts_null == sst_ctx.past_node) {
        u32_node_size = sizeof(ts_nodebuf_s_t) * u32_node_cnt;
        sst_ctx.past_node = (ts_nodebuf_s_t *)ts_cpl_kmalloc(u32_node_size, TS_CPL_GFP_KERNEL);
        if (ts_null == sst_ctx.past_node) {
            ts_cpl_log_err("Failed to malloc for the nodes\n");

            return -TS_ENOMEM;
        }
    }

    /* 初始化队列 */
    ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&sst_ctx.st_ready_head);

    /* 按照顺序给定 buf 地址, 并将全部节点挂载到 upload 的空闲队列上 */
    pst_node = sst_ctx.past_node;
    pu08_ptr = (ts_u08 *)pu32_buf;
    for (i = 0; i < u32_node_cnt; i++) {
        if (0 == s32_is_dma) {
            pst_node->u32_phy_addr = (ts_u32)ts_cpl_dma_map_single(ts_null, pu08_ptr, \
                                        u32_strid, TS_CPL_DMA_FROM_DEVICE);
        } else {
            pst_node->u32_phy_addr = (ts_u32)pu08_ptr;
        }

        pst_node->u32_virt_addr = (ts_u32)pu08_ptr;
        pst_node->u32_size = u32_frm_size;

        pst_cur_list = (ts_cpl_list_head_s_t *)&pst_node->st_list;
        ts_cpl_init_list_head(pst_cur_list);

        /* 加载到 upload 模块空闲节点上 */
        tdl_isp_upload_put_free_node(TS_ISP_MDEV0, pst_cur_list, TS_ISP_UPL_SECONDARY_CH);

        pst_node++;

        /* VPO 需要额外 4 行大小 */
        if (u32_node_cnt > 2)
            pu08_ptr += u32_strid;
    }

    sst_ctx.u32_get_cnt = 0;
    sst_ctx.u32_frm_cnt = u32_frm_cnt;
    sst_ctx.s32_is_dma = s32_is_dma;

    st_up_attr.u32_module = u32_up_pos;
    st_up_attr.u32_chn = TS_ISP_UPL_SECONDARY_CH;
    st_up_attr.s32_frame_num = u32_frm_cnt;
    st_up_attr.u32_tmout_ms = 500;

    tdl_isp_upload_set_cb(TS_ISP_MDEV0, TS_ISP_UPL_SECONDARY_CH, libup_ready_cb);

    return tdl_isp_run_upload(TS_ISP_MDEV0, (const ts_isp_upload_attr_s_t *)&st_up_attr);
}

static uint16_t t_swap16(uint16_t value) {
    return (value << 8) | (value >> 8);
}
/* 
 * 定义 upl 命令， 演示 libupload 使用， 函数原型为 ：
 *   static ts_s32 main_upl(ts_s32 argc, char * argv[])
 */
TS_DECLARE_CMD(upl)
{
    ts_u32 i, j, u32_frm_cnt, u32_strid, u32_node_cnt, u32_frm_size, u32_frm_sum;
    ts_s32 s32_is_dma;
    ts_u32 u32_pos;
    ts_nodebuf_s_t *pst_node;
    ts_u16 *pu16_ptr;
    ts_void *pv_tmm;

    if (argc > 1)
        u32_pos = ts_cpl_str2num((char *)argv[1]);
    else
        u32_pos = 0;

    u32_frm_cnt = 1;
    if (argc > 2)
        u32_frm_cnt = ts_cpl_str2num(argv[2]);

    ts_u16 Spot_y = ts_cpl_str2num(argv[3]);
#if 0
    /* 取消限制，如果后续申请不到相应内存就退出 */
    if (u32_frm_cnt > 10)
        u32_frm_cnt = 10;
#endif

    /* 默认节点个数为帧数 + 1； 如果需要小于 u32_frm_cnt, 需保证还节点的速度大于节点消耗完毕的速度 */
    u32_node_cnt = u32_frm_cnt + 1;

    /* 给定的是非 DMA 区域地址 */
    s32_is_dma = 0;

    /* 获取上传图像的大小 */
    upl_get_frm_info(u32_pos, (ts_u32 *)&u32_frm_size, (ts_u32 *)&u32_strid);

    u32_strid = u32_strid * u32_node_cnt;
    pv_tmm = ts_cpl_tmm_malloc(ts_null, u32_strid, 32, "upl");
    if (ts_null == pv_tmm)
        return TS_ERR;

    tdl_isp_claim_s_chn();

    ts_lib_up_start(u32_pos, (ts_u32 *)pv_tmm, u32_frm_cnt, u32_node_cnt, s32_is_dma);

    for (i = 0; i < u32_frm_cnt; i++) {
        pst_node = ts_lib_up_get_node(100);
        if (ts_null == pst_node)
            break;

        ts_cpl_printk("g:%x, u32_frm_size = %u\n", pst_node->u32_virt_addr, u32_frm_size);

        /* 计算当前节点 */
        pu16_ptr = (ts_u16 *)pst_node->u32_virt_addr;
        u32_frm_sum = 0;

        /* 注意：u32_frm_size 是帧大小，不是像素个数，此处 16bit 一个像素，因此除以 2 */
        for (j = 0; j < (u32_frm_size >> 1); j++) {
            u32_frm_sum += pu16_ptr[j];
            /* 这里的数值限制为384*288的测试 */
            ts_u32 cVal = Spot_y * 768;
            if ((j >= cVal) && (j <= (cVal + 768))) {
                ts_cpl_printk("%04d=0x%04x ", j % cVal, t_swap16(pu16_ptr[j]));
                if (j%10 == 0) {
                    ts_cpl_printk("\n");
                }
            }
            if (j > (cVal+768))
            {
                ts_cpl_printk("\n");
                break;
            }
        }
        ts_lib_up_release_node(pst_node);

        // ts_cpl_printk("sum:%d;%x\n", i, u32_frm_sum);
    }

    ts_lib_up_stop();
    tdl_isp_release_s_chn();

    ts_cpl_tmm_free(pv_tmm);

    return TS_OK;
}

