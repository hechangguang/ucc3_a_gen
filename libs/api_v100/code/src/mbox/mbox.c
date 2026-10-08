/* 
 * mbox.c -- mbox_v100 api 实现, 绕开文件系统实现对驱动访问，仅限于 RTOS 使用
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/ts_cpl.h>
#include <api/mbox_v100.h>
#include <otdl/mbox_v100.h>


extern mbox_attr_s_t sst_live_attr;


/* 
 * 函数名 :  ts_lib_mbox_open
 * 描  述 :  打开邮箱设备
 * 输  入 :  参数               描述
 *           pc_dev_name        邮箱设备文件名
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           > 0                邮箱设备文件句柄
 *           <=0                打开设备失败     
 */
ts_s32 ts_lib_mbox_open(ts_char *pc_dev_name)
{
    return (ts_s32)sst_live_attr.pv_priv;
}


/* 
 * 函数名 :  ts_lib_mbox_close
 * 描  述 :  关闭邮箱设备
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_mbox_close(ts_s32 s32_handle)
{
    return;
}


/* 
 * 函数名 :  ts_lib_mbox_set_live_attr
 * 描  述 :  设置心跳回调函数
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 *           pfn_cb            心跳回调函数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_mbox_set_live_attr(ts_s32 s32_handle, ts_mbox_cb_t pfn_cb)
{
    sst_live_attr.pfn_cb = pfn_cb;
}


/* 
 * 函数名 :  ts_lib_mbox_ack_live
 * 描  述 :  发送心跳应答包
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 *           u32_live_cnt      当前心跳计数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_mbox_ack_live(ts_s32 s32_handle, ts_u32 u32_live_cnt)
{
    ts_u32 u32_tmp;
    ts_mbox_dev_s_t *pst_mbox_dev;

    if (s32_handle > 0) {
        pst_mbox_dev = (ts_mbox_dev_s_t *)s32_handle;

        u32_tmp = u32_live_cnt + 1;
        u32_tmp = u32_tmp << TS_MBOX_SPEC_OFFSET;
        u32_tmp |= TS_MBOX_LIVE_ACK;

        ts_cpl_writel(u32_tmp, pst_mbox_dev->u32_wr_addr);
    }
}


/* 
 * 函数名 :  ts_lib_mbox_query_live
 * 描  述 :  发送心跳查询包
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 *           u32_live_cnt      当前心跳计数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0..3FFFFFFF       下一个心跳计数
 */
ts_u32 ts_lib_mbox_query_live(ts_s32 s32_handle, ts_u32 u32_live_cnt)
{
    ts_u32 u32_tmp;
    ts_mbox_dev_s_t *pst_mbox_dev;

    if (s32_handle > 0) {
        pst_mbox_dev = (ts_mbox_dev_s_t *)s32_handle;

        u32_tmp = u32_live_cnt << TS_MBOX_SPEC_OFFSET;
        u32_tmp |= TS_MBOX_LIVE_QUERY;

        ts_cpl_writel(u32_tmp, pst_mbox_dev->u32_wr_addr);

        u32_tmp = u32_live_cnt + 1;

        /* 去掉高 2bit */
        u32_tmp = u32_tmp << TS_MBOX_SPEC_OFFSET;
        u32_tmp = u32_tmp >> TS_MBOX_SPEC_OFFSET;

        return u32_tmp;
    } else {
        return 0;
    }
}


static ts_s32 mbox_create(ts_s32 s32_handle, ts_u32 u32_id, ts_void *pv_func, ts_void *pv_priv, ts_u32 u32_type)
{
    ts_u32 u32_attr_id;
    ts_mbox_dev_s_t *pst_mbox_dev;
    mbox_attr_s_t *pst_attr;

    if (s32_handle > 0) {
        pst_mbox_dev = (ts_mbox_dev_s_t *)s32_handle;
        u32_attr_id = TS_MBOX_GET_CHN_ID(u32_id);
        if (u32_attr_id >= pst_mbox_dev->u16_max_num)
            return -TS_ENOMEM;

        pst_attr = pst_mbox_dev->pst_attrs;
        pst_attr += u32_attr_id;

        if (MBOX_ATTR_CBT == u32_type) {
            pst_attr->u32_type = MBOX_ATTR_CBT;
            pst_attr->pfn_cb = (ts_mbox_cb_t)pv_func;
            pst_attr->pv_priv = pv_priv;
        } else {
            pst_attr->u32_type = MBOX_ATTR_SEMPHRT;
            pst_attr->pst_sem = (ts_cpl_semphr_s_t *)pv_func;
        }

        return TS_OK;
    } else {
        return -TS_ENODEV;
    }
}


/* 
 * 函数名 :  ts_lib_mbox_create_cb
 * 描  述 :  创建一个基于执行回调函数的邮箱
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 *           u32_id             邮箱 ID
 *           pfn_cb             邮箱回调处理函数
 *           pv_priv            回调函数私有变量
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *           TS_ENODEV          传入句柄属于无效设备
 *           TS_ENOMEM          分配内存失败
 */
ts_s32 ts_lib_mbox_create_cb(ts_s32 s32_handle, ts_u32 u32_id, ts_mbox_cb_t pfn_cb, ts_void *pv_priv)
{
    return mbox_create(s32_handle, u32_id, (ts_void *)pfn_cb, pv_priv, MBOX_ATTR_CBT);
}


/* 
 * 函数名 :  ts_lib_mbox_create_sem
 * 描  述 :  创建一个基于执行同步信号的邮箱
 * 输  入 :  参数               描述
 *           s32_handle        邮箱设备句柄
 *           u32_id             邮箱 ID
 *           pst_sema           同步信号量
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              创建成功
 *           TS_ENODEV          传入句柄属于无效设备
 *           TS_ENOMEM          分配内存失败
 */
ts_s32 ts_lib_mbox_create_sem(ts_s32 s32_handle, ts_u32 u32_id, ts_cpl_semphr_s_t *pst_sema)
{
    return mbox_create(s32_handle, u32_id, (ts_void *)pst_sema, ts_null, MBOX_ATTR_SEMPHRT);
}


/* 
 * 函数名 :  mbox_get_free_mail
 * 描  述 :  从缓冲中获取一个空闲邮件
 * 输  入 :  参数               描述
 *           pst_mbox_dev       邮箱设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            申请失败
 *           非 ts_null         邮件指针
 */
static ts_mail_s_t *mbox_get_free_mail(ts_mbox_dev_s_t *pst_mbox_dev)
{
    ts_cpl_mutex_s_t *pst_mutex;
    ts_mail_s_t *pst_mail;
    ts_u32 i;

    pst_mutex = (ts_cpl_mutex_s_t *)&pst_mbox_dev->st_mutex;
    i = 0;

    ts_cpl_mutex_lock(pst_mutex);
    while (1) {
        if (i >= pst_mbox_dev->u32_mail_num) {
            pst_mail = ts_null;

            break;
        }

        pst_mail = pst_mbox_dev->pst_next_mail;
        pst_mbox_dev->u32_mail_last++;
        pst_mbox_dev->pst_next_mail++;
        if (pst_mbox_dev->u32_mail_last >= pst_mbox_dev->u32_mail_num) {
            pst_mbox_dev->pst_next_mail = (ts_mail_s_t *)pst_mbox_dev->pu32_mail_addr;
            pst_mbox_dev->u32_mail_last = 0;
        }

        if (0 == pst_mail->st_hdr.u32_from_id)
            break;

        i++;
    }

    ts_cpl_mutex_unlock(pst_mutex);

    return pst_mail;
}


/* 
 * 函数名 :  ts_lib_mbox_new_mail
 * 描  述 :  申请一个新的空邮件
 * 输  入 :  参数               描述
 *           s32_handle         邮箱设备句柄
 *           u32_from_id        发件 ID
 *           u32_to_id          接收 ID
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            申请失败
 *           非 ts_null         邮件正文指针，内部最大缓冲为 MBOX_MAX_PARAM 指定
 */
ts_u32 *ts_lib_mbox_new_mail(ts_s32 s32_handle, ts_u32 u32_from_id, ts_u32 u32_to_id)
{
    ts_mbox_dev_s_t *pst_mbox_dev;
    ts_mail_s_t *pst_mail;

    if (s32_handle > 0) {
        pst_mbox_dev = (ts_mbox_dev_s_t *)s32_handle;
        pst_mail = mbox_get_free_mail(pst_mbox_dev);
        if (ts_null == pst_mail)
            return ts_null;

        pst_mail->st_hdr.u32_from_id = u32_from_id;
        pst_mail->st_hdr.u32_to_id = u32_to_id;

        return (ts_u32 *)&pst_mail->au32_content;
    } else {
        return ts_null;
    }
}


/* 
 * 函数名 :  ts_lib_mbox_modify_mail
 * 描  述 :  修改一个已经存在的邮件的发送接收 ID
 * 输  入 :  参数               描述
 *           pu32_content       邮件正文指针
 *           u32_from_id        发件 ID
 *           u32_to_id          接收 ID
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_mbox_modify_mail(ts_u32 *pu32_content, ts_u32 u32_from_id, ts_u32 u32_to_id)
{
    ts_u32 u32_mail_addr;
    ts_mail_s_t *pst_mail;

    /* 偏移到邮件头 */
    u32_mail_addr = (ts_u32)pu32_content;
    u32_mail_addr -= sizeof(ts_mail_hdr_s_t);

    pst_mail = (ts_mail_s_t *)u32_mail_addr;
    pst_mail->st_hdr.u32_from_id = u32_from_id;
    pst_mail->st_hdr.u32_to_id = u32_to_id;
}


/* 
 * 函数名 :  ts_lib_mbox_free_mail
 * 描  述 :  释放一个邮件
 * 输  入 :  参数               描述
 *           pu32_content       邮件正文指针
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_mbox_free_mail(ts_u32 *pu32_content)
{
    ts_u32 u32_mail_addr;
    ts_mail_s_t *pst_mail;

    /* 偏移到邮件头 */
    u32_mail_addr = (ts_u32)pu32_content;
    u32_mail_addr -= sizeof(ts_mail_hdr_s_t);

    pst_mail = (ts_mail_s_t *)u32_mail_addr;
    pst_mail->st_hdr.u32_from_id = 0;
    pst_mail->st_hdr.u32_to_id = 0;
}


/* 
 * 函数名 :  ts_lib_mbox_send_mail
 * 描  述 :  发送一份邮件
 * 输  入 :  参数               描述
 *           s32_handle         邮箱设备句柄
 *           pu32_mail          邮件指针
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_EINVAL         入参无效
 *           TS_OK             发送完成
 */
ts_s32 ts_lib_mbox_send_mail(ts_s32 s32_handle, ts_u32 *pu32_mail)
{
    ts_u32 u32_mail_addr;
    ts_mbox_dev_s_t *pst_mbox_dev;

    if ((s32_handle <= 0) || (ts_null == pu32_mail))
        return -TS_EINVAL;

    pst_mbox_dev = (ts_mbox_dev_s_t *)s32_handle;

    /* 偏移到邮件头 */
    u32_mail_addr = (ts_u32)pu32_mail;
    u32_mail_addr -= sizeof(ts_mail_hdr_s_t);

    /* cache to ram */
    ts_cpl_sync_dma_for_device(ts_null, (ts_phys_addr_t)u32_mail_addr, sizeof(ts_mail_s_t));

    /* 效率考虑，不做溢出判断 */
    ts_cpl_writel(u32_mail_addr, pst_mbox_dev->u32_wr_addr);

    return TS_OK;
}
