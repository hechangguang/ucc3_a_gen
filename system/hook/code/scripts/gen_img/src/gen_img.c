/*
 * gen_img.c -- bootup 镜像打包工具实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <base/ts_bitop.h>
#include <bootup.h>
#include <bootup_sign.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sha256.h>

/* 打包类型定义 */
#define GI_PACKET_NONE          (0)
#define GI_PACKET_SIGN_ONLY     (1)
#define GI_PACKET_HOOK          (2)
#define GI_PACKET_INF           (3)
#define GI_PACKET_BOTH_HOOK_INF (4)

/* long_opts 定义 */
#define GI_OPTS_CONFIG          'c'
#define GI_OPTS_HOOK            'h'
#define GI_OPTS_INF             'i'
#define GI_OPTS_KEYNAME         'k'
#define GI_OPTS_OUTF            'o'
#define GI_OPTS_OFFSET          'f'
#define GI_OPTS_SVR             's'
#define GI_OPTS_PORT            'p'
#define GI_OPTS_SIGN            'S'

/* 避免文件名过长，导致栈过深 */
#define GI_MAX_FILE_NAME        (256)
static char sc_conff_name[GI_MAX_FILE_NAME];
static char sc_hookf_name[GI_MAX_FILE_NAME];
static char sc_inf_name[GI_MAX_FILE_NAME];
static char sc_outf_name[GI_MAX_FILE_NAME];

#define GI_HASH_SIZE			(32)

/* 过程上下文 */
typedef struct gi_packet_s {
    ts_s32 s32_sign_enable;
    ts_u32 u32_offset;

    /* for sign */
    ts_char ac_key_name[TS_KEY_NAME_MAX_LENGTH];
    ts_u08 au08_svr_ip[24];
    ts_s32 s32_port;

    /* 头部信息 */
    ts_u32 u32_ih_size;
    bootup_ih_s_t st_ih;

    /* 公钥 hash */
    ts_u08 au08_hash[32];

    /* hook file */
    ts_u32 u32_hook_size;
    ts_u08 *pu08_hook;

    ts_u32 u32_hook_sign_size;
    ts_bootup_sign_s_t st_hook_sign;

    /* inf file */
    ts_u32 u32_inf_size;
    ts_u08 *pu08_inf;

    ts_u32 u32_inf_sign_size;
    ts_bootup_sign_s_t st_inf_sign;
} gi_packet_s_t;


static ts_s32 gi_read_file(ts_char *pc_i_filename, ts_u32 u32_o_buf_offset, ts_u08 *pu08_o_buf, ts_u32 u32_only_get_size)
{
    FILE *fp_i_file;
    ts_s32 s32_filesize;
    ts_u08 *pu08_ptr;
    
    fp_i_file = fopen(pc_i_filename, "rb");
    if (ts_null == fp_i_file) {
        printf("Open for read failed : filename = %s!\n", pc_i_filename);

        return -1;
    }

    fseek(fp_i_file, 0, SEEK_SET);
    fseek(fp_i_file, 0, SEEK_END);
    s32_filesize = ftell(fp_i_file);
    if (-1 == s32_filesize) {
        fclose(fp_i_file);
        printf("Get size failed : filename : %s!\n", pc_i_filename);

        return -1;
    } else {
        if (0 == u32_only_get_size) {
            pu08_ptr = pu08_o_buf;
            pu08_ptr += u32_o_buf_offset;

            fseek(fp_i_file, 0, SEEK_SET);
            fread((void *)pu08_ptr, s32_filesize, 1, fp_i_file);
        }

        fclose(fp_i_file);
    }
    
    return s32_filesize;
}

static ts_u32 gi_dec2hex(ts_u32_dec_number)
{
    ts_u32 u32_hex_number;

    u32_hex_number = (ts_u32_dec_number / 10) * 0x10 + (ts_u32_dec_number - (ts_u32_dec_number / 10) * 10);

    return u32_hex_number;
}


static ts_void gi_get_time(bootup_ih_s_t *pst_img_header)
{
    time_t now;
    struct tm *local_now;
    ts_u32 u32_year, u32_mon, u32_day, u32_hour;

    time(&now);
    local_now = localtime(&now);

    u32_year = (local_now->tm_year + 1900) - 2000;
    u32_year = gi_dec2hex(u32_year);
    u32_year = u32_year << 24;

    u32_mon = local_now->tm_mon + 1;
    u32_mon = gi_dec2hex(u32_mon);
    u32_mon = u32_mon << 16;

    u32_day = local_now->tm_mday;
    u32_day = gi_dec2hex(u32_day);
    u32_day = u32_day << 8;

    u32_hour = local_now->tm_hour;
    u32_hour = gi_dec2hex(u32_hour);

    pst_img_header->u32_ih_time = u32_year | u32_mon | u32_day | u32_hour;
}


static ts_u08 * gi_load_file(ts_char *pc_filename, ts_u32 *pu32_size)
{
    ts_s32 s32_filesize;
    ts_u08 *pu08_ptr;

    s32_filesize = gi_read_file(pc_filename, 0, ts_null, 1);
    if (-1 == s32_filesize)
        return ts_null;

	if ((s32_filesize & 0x03) > 0) {
		s32_filesize &= ~0x03;
		s32_filesize += 4;
	}

    pu08_ptr = (ts_u08 *)malloc(s32_filesize);
    if (NULL == pu08_ptr) {
        printf("Malloc for read %s failed!\n", pc_filename);

        return ts_null;
    }

	memset((void *)pu08_ptr, 0, s32_filesize);
    gi_read_file(pc_filename, 0, pu08_ptr, 0);

    *pu32_size = (ts_u32)s32_filesize;

    return pu08_ptr;
}


static ts_void gi_check_cfg(bootup_ih_s_t *pst_ih)
{
    printf("ih_magic            = %#x\n", pst_ih->u32_ih_magic);
    printf("ih_hcrc             = %#x\n", pst_ih->u32_ih_hcrc);
    printf("ih_time             = %#x\n", pst_ih->u32_ih_time);
    printf("ih_size             = %#x\n", pst_ih->u32_ih_size);
    printf("ih_load             = %#x\n", pst_ih->u32_ih_load);
    printf("ih_ep               = %#x\n", pst_ih->u32_ih_ep);
    printf("ih_dcrc             = %#x\n", pst_ih->u32_ih_dcrc);

    if (BOOTUP_VERIFY_SUM32 == pst_ih->u08_ih_verify_type)
        printf("ih_verify_type      = SUM32\n");
    else if (BOOTUP_VERIFY_SUM8 == pst_ih->u08_ih_verify_type)
        printf("ih_verify_type      = SUM8\n");
    else if (BOOTUP_VERIFY_CRC32 == pst_ih->u08_ih_verify_type)
        printf("ih_verify_type      = CRC32\n");
    else if (BOOTUP_VERIFY_CRC8 == pst_ih->u08_ih_verify_type)
        printf("ih_verify_type      = CRC8\n");
    else if (BOOTUP_VERIFY_SIGN == pst_ih->u08_ih_verify_type)
        printf("ih_verify_type      = SIGN\n");
    else
        printf("ih_verify_type      = UNKONW\n");

    if (0 == pst_ih->u08_ignore_verify)
        printf("ignore_verify       = No\n");
    else
        printf("ignore_verify       = Yes\n");

    if (0 == pst_ih->u08_ih_sign_en)
        printf("ih_sign_en          = Disable\n");
    else
        printf("ih_sign_en          = Enable\n");

    if (BOOTUP_COMPRESS_LZO == pst_ih->u08_ih_comp)
        printf("ih_comp             = lzo\n");
    else if (BOOTUP_COMPRESS_RLE == pst_ih->u08_ih_comp)
        printf("ih_comp             = rle\n");
    else if (BOOTUP_COMPRESS_FSE == pst_ih->u08_ih_comp)
        printf("ih_comp             = fse\n");
    else
        printf("ih_comp             = none\n");

    if (TS_LOG_EMERG == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = EMERG\n");
    else if (TS_LOG_ALERT == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = ALERT\n");
    else if (TS_LOG_CRIT == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = CRIT\n");
    else if (TS_LOG_ERR == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = ERR\n");
    else if (TS_LOG_WARNING == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = WARNING\n");
    else if (TS_LOG_NOTICE == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = NOTICE\n");
    else if (TS_LOG_INFO == pst_ih->u08_ih_debug_level)
        printf("ih_debug_level      = INFO\n");
    else
        printf("ih_debug_level      = DEBUG\n");

    printf("ih_bk_cnt           = %#x\n", pst_ih->u08_ih_bk_cnt);
    printf("ih_img_bk_cnt       = %#x\n", pst_ih->u08_ih_img_bk_cnt);

    printf("ih_bk_offset        = %#x\n", pst_ih->u32_ih_bk_offset);
    printf("ih_img_bk_offset    = %#x\n", pst_ih->u32_ih_img_bk_offset);

    printf("ih_img_load_addr    = %#x\n", pst_ih->u32_ih_img_load_addr);
    printf("ih_img_ep_addr      = %#x\n", pst_ih->u32_ih_img_ep_addr);
    printf("ih_img_size         = %#x\n", pst_ih->u32_ih_img_size);
    printf("ih_img_crc          = %#x\n", pst_ih->u32_ih_img_crc);
}


static ts_s32 gi_load_cfg(ts_char *pc_cfg_filename, gi_packet_s_t *pst_packet)
{
    ts_s32 s32_filesize;
    ts_u08 *pu08_cfg_buf;

    pu08_cfg_buf = (ts_u08 *)&pst_packet->st_ih;
    s32_filesize = gi_read_file(pc_cfg_filename, 0, pu08_cfg_buf, 0);
    if (s32_filesize <= 0)
        return TS_ERR;

    gi_get_time((bootup_ih_s_t *)&pst_packet->st_ih);

    return TS_OK;
}

static ts_s32 gi_write_file(char *pc_o_filename, ts_u08 *pu8_i_bufer, ts_u32 u32_filesize)
{
    FILE *fp_write_file;

    fp_write_file = fopen(pc_o_filename, "wb");
    if (ts_null == fp_write_file) {
        printf("Open for write failed : filename %s!\n", pc_o_filename);

        return -1;
    }

    fseek(fp_write_file, 0, SEEK_SET);
    fwrite((void *)(pu8_i_bufer), 1, u32_filesize, fp_write_file);

    fclose(fp_write_file);

    return 0;
}

static ts_void gi_show_buffer(ts_u08 *pu08_buf, ts_u32 u32_cnt, ts_u32 u32_extra_cnt, ts_u32 u32_line_size)
{
    ts_u32 i, j, u32_show_cnt;

    u32_show_cnt = u32_cnt + u32_extra_cnt;
    j = 0;
    for (i = 0; i < u32_show_cnt; i++) {
        if (j > (u32_line_size - 1)) {
            printf("\n");
            j = 0;
        }

        printf("0x%02x, ", pu08_buf[i]);
        j++;
    }

    printf("\n");
}

static ts_void gi_show_sign2efuse(ts_u08 *pu08_buf)
{
	ts_u32 i, j, u32_tmp;
	ts_u32 *pu32_ptr;

	printf("Write sign buf to efuse...\n");
    j = 0;
	pu32_ptr = (ts_u32 *)pu08_buf;
    for (i = 0; i < (GI_HASH_SIZE >> 2); i++) {
        if (j > 1) {
            printf("\n");
            j = 0;
        }
		
		u32_tmp = pu32_ptr[i];
		
		printf("0x%02x, ", (ts_u08)(u32_tmp >> 24));
		printf("0x%02x, ", (ts_u08)(u32_tmp >> 16));
		printf("0x%02x, ", (ts_u08)(u32_tmp >> 8));
		printf("0x%02x, ", (ts_u08)(u32_tmp >> 0));

        j++;
    }

    printf("\n");
}


static ts_void gi_show_sign_buf(ts_sign_packet_s_t *pst_sign_buf)
{
    printf("Key name : %s\n", (ts_char *)&pst_sign_buf->ac_key_name);
    printf("hash_size = %d\n", pst_sign_buf->u32_hash_size);
    printf("sign_size = %d\n", pst_sign_buf->u32_sign_size);
    if (pst_sign_buf->u32_hash_size > 0) {
        printf("hash buffer:\n");
        gi_show_buffer((ts_u08 *)&pst_sign_buf->au08_hash, pst_sign_buf->u32_hash_size, 0x10, 16);
    }

    if (pst_sign_buf->u32_sign_size > 0) {
        printf("rsa_len = %d(%#x)\n", pst_sign_buf->st_sign.u32_ras_len, pst_sign_buf->st_sign.u32_ras_len);
        printf("rsa_n0inv = %d(%#x)\n", pst_sign_buf->st_sign.u32_rsa_n0inv, pst_sign_buf->st_sign.u32_rsa_n0inv);

        printf("=>rsa_modules buffer:\n");
        gi_show_buffer((ts_u08 *)&pst_sign_buf->st_sign.au32_rsa_modules, 64 * 4, 0, 16);

        printf("=>rsa_rr buffer:\n");
        gi_show_buffer((ts_u08 *)&pst_sign_buf->st_sign.au32_rsa_rr, 64 * 4, 0, 16);

        printf("=>sign buffer:\n");
        gi_show_buffer((ts_u08 *)&pst_sign_buf->st_sign.au08_sign, 256, 0, 16);
    }
}


static ts_void rsa_convert_big_endian(ts_u32 *buf, ts_u32 len)
{
    ts_u32 i;

    /* rsa 2048 最大 64 */
    ts_u32 au32_tmp_buf[TS_BOOTUP_RSA_DWORDS];

    for (i = 0; i < len; i++)
        au32_tmp_buf[i] = fdt32_to_cpu(buf[len - 1 - i]);

    for (i = 0; i < len; i++)
        buf[i] = au32_tmp_buf[i];
}

static ts_s32 gi_exec_sign(gi_packet_s_t *pst_packet, ts_u32 u32_type)
{
    ts_s32 s32_ret, s32_skt_fd, s32_rd_size;
    struct sockaddr_in st_skt_addr;
    ts_u32 u32_crc;
    ts_sign_packet_s_t st_sign_buf;
    ts_u08 *pu08_in_buf;
    ts_u32 u32_in_size;
    ts_u08 *pu08_sign_buf;
    ts_u32 u32_sign_size;
    ts_u08 *pu08_ptr;

    printf("ip = %s, port = %d\n", pst_packet->au08_svr_ip, pst_packet->s32_port);

    /* 确定使用的缓冲 */
    if (TS_BOOTUP_SIGN_HOOK == u32_type) {
        pu08_in_buf = pst_packet->pu08_hook;
        u32_in_size = pst_packet->u32_hook_size;
        pu08_sign_buf = (ts_u08 *)&pst_packet->st_hook_sign;
    } else {
        pu08_in_buf = pst_packet->pu08_inf;
        u32_in_size = pst_packet->u32_inf_size;

        /* 去掉头部信息 */
        u32_in_size = u32_in_size - sizeof(bootup_ih_s_t);
        pu08_in_buf += sizeof(bootup_ih_s_t);

        pu08_sign_buf = (ts_u08 *)&pst_packet->st_inf_sign;
    }

    memset((ts_void *)&st_sign_buf, 0, sizeof(ts_sign_packet_s_t));
    strcpy((ts_char *)&st_sign_buf.ac_key_name, (ts_char *)&pst_packet->ac_key_name);

    /* calc the pu08_in_buf hash */
    pu08_ptr = pu08_in_buf;
    if (pst_packet->u32_offset > 0) {
        pu08_ptr += pst_packet->u32_offset;
        u32_in_size -= pst_packet->u32_offset;
    }
    sha256_csum_wd((const ts_u08 *)(pu08_ptr), u32_in_size, (ts_u08 *)&(st_sign_buf.au08_hash), 0);
    st_sign_buf.u32_hash_size = 32;

    pu08_ptr = (ts_u08 *)&st_sign_buf;
    pu08_ptr += 4;
    u32_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pu08_ptr, (sizeof(ts_sign_packet_s_t) - 4));
    st_sign_buf.u32_packet_crc = u32_crc;

    memset(&st_skt_addr, 0, sizeof(struct sockaddr_in));
    st_skt_addr.sin_family = AF_INET;
    st_skt_addr.sin_port = htons(pst_packet->s32_port);
    inet_pton(AF_INET, pst_packet->au08_svr_ip, &st_skt_addr.sin_addr);

    s32_skt_fd = socket(AF_INET, SOCK_STREAM, 0);
    if((connect(s32_skt_fd, (struct sockaddr *)&st_skt_addr, sizeof(struct sockaddr))) < 0) {
        printf("Connect errno[%d] : %s\n", errno, strerror(errno));

        return 0;
    }

    if((send(s32_skt_fd, (ts_char *)&st_sign_buf, sizeof(ts_sign_packet_s_t), 0)) < 0) {
        printf("Send errno[%d]: %s\n", errno, strerror(errno));
        close(s32_skt_fd);

        return 0;
    }

    s32_rd_size = recv(s32_skt_fd, (ts_char *)&st_sign_buf, sizeof(ts_sign_packet_s_t), 0);
    close(s32_skt_fd);
    if (s32_rd_size > 0) {
        u32_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pu08_ptr, (sizeof(ts_sign_packet_s_t) - 4));
        if (u32_crc == st_sign_buf.u32_packet_crc) {
            u32_sign_size = st_sign_buf.u32_sign_size;

            rsa_convert_big_endian((ts_u32 *)&st_sign_buf.st_sign.au32_rsa_modules, TS_BOOTUP_RSA_DWORDS);
            rsa_convert_big_endian((ts_u32 *)&st_sign_buf.st_sign.au32_rsa_rr, TS_BOOTUP_RSA_DWORDS);

            memcpy((ts_void *)pu08_sign_buf, (ts_void *)(&st_sign_buf.st_sign), u32_sign_size);
            if (TS_BOOTUP_SIGN_HOOK == u32_type)
                pst_packet->u32_hook_sign_size = u32_sign_size;
            else
                pst_packet->u32_inf_sign_size = u32_sign_size;

            gi_show_sign_buf((ts_sign_packet_s_t *)&st_sign_buf);

            return s32_rd_size;
        } else {
            printf("read crc = %#x, calc crc = %#x\n", st_sign_buf.u32_packet_crc, u32_crc);
        }
    }

    return 0;
}


static ts_s32 gi_hook_only(gi_packet_s_t *pst_packet)
{
    bootup_ih_s_t *pst_img_header;
    ts_u32 u32_crc;

    pst_img_header = (bootup_ih_s_t *)&pst_packet->st_ih;

    u32_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pst_packet->pu08_hook, pst_packet->u32_hook_size);
    pst_img_header->u32_ih_dcrc = u32_crc;
    pst_img_header->u32_ih_size = pst_packet->u32_hook_size;

    if (1 == pst_packet->s32_sign_enable)
        gi_exec_sign(pst_packet, TS_BOOTUP_SIGN_HOOK);

    return TS_OK;
}


static ts_s32 gi_inf_only(gi_packet_s_t *pst_packet)
{
    bootup_ih_s_t *pst_img_header;
    ts_u32 u32_crc;

    pst_img_header = (bootup_ih_s_t *)&pst_packet->st_ih;

    u32_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pst_packet->pu08_hook, pst_packet->u32_hook_size);
    pst_img_header->u32_ih_img_crc = u32_crc;
    pst_img_header->u32_ih_img_size = pst_packet->u32_inf_size;

    if (1 == pst_packet->s32_sign_enable)
        gi_exec_sign(pst_packet, TS_BOOTUP_SIGN_INF);

    return TS_OK;
}

static ts_s32 gi_sign_only(gi_packet_s_t *pst_packet)
{
    gi_exec_sign(pst_packet, TS_BOOTUP_SIGN_INF);
}


static ts_s32 gi_both(gi_packet_s_t *pst_packet)
{
    ts_s32 s32_ret;

    s32_ret = gi_hook_only(pst_packet);
    if (0 == s32_ret)
        s32_ret = gi_inf_only(pst_packet);
    
    return s32_ret;
}


static ts_void gi_normal_image(ts_char *pc_ih_file, ts_char *pc_img_file, ts_char *pc_o_file, bootup_ih_s_t *pst_img_header)
{
    ts_s32 s32_filesize, s32_o_img_size;
    ts_u08 *pu08_o_img_buf;
    ts_u08 *pu08_ptr;
    ts_u32 u32_calc_crc, i;
    FILE *pf_o_file;
    
    s32_filesize = gi_read_file(pc_ih_file, 0, ts_null, 1);
    if (-1 == s32_filesize) {
        printf("Get size failed : filename : %s!\n", pc_ih_file);

        return;
    } else {
        pst_img_header->u32_ih_size = (ts_u32)s32_filesize;

        printf("Filesize of %s = %d\n", pc_ih_file, s32_filesize);
    }

    s32_o_img_size = s32_filesize + sizeof(bootup_ih_s_t);
    pu08_o_img_buf = (ts_u08 *)malloc(s32_o_img_size);
    if (NULL == pu08_o_img_buf) {
        printf("Malloc for out image failed!\n");

        return;
    }

    memset((ts_void *)(pu08_o_img_buf), 0xFF, s32_o_img_size);

    pu08_ptr = pu08_o_img_buf;
    pu08_ptr += sizeof(bootup_ih_s_t);
    gi_read_file(pc_ih_file, 0, pu08_ptr, 0);

    /* 待加载的镜像信息, 如果 u32_ih_img_load_addr = u32_ih_load, 
     * 表明 镜像紧随 hook 部分之后，使用以下配置加载
     * 如果 u32_ih_img_load_addr != u32_ih_load , 表明 镜像单独存放，且
     * 有自己的头部信息，此时仅使用 u32_ih_img_load_addr 配置，其余配置不使用.
     */
    if (pst_img_header->u32_ih_load == pst_img_header->u32_ih_img_load_addr) {
        u32_calc_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pu08_ptr, (ts_u32)s32_filesize);
        pst_img_header->u32_ih_dcrc = u32_calc_crc;

    } else {

    }

    pu08_ptr = (ts_u08 *)pst_img_header;
    pu08_ptr += IH_BOOTUP_CRC_OFFSET;
    u32_calc_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pu08_ptr, (sizeof(bootup_ih_s_t) - IH_BOOTUP_CRC_OFFSET));
    pst_img_header->u32_ih_hcrc = u32_calc_crc;

    memcpy((ts_void *)pu08_o_img_buf, (ts_void *)pst_img_header, sizeof(bootup_ih_s_t));

    gi_write_file(pc_o_file, pu08_o_img_buf, s32_o_img_size);

    free(pu08_o_img_buf);
}


static ts_void gi_help(ts_void)
{
    printf("gen_img ih_cfg_file in_ih_file [in_img_file] out_file\n");
}


static ts_void gi_free(gi_packet_s_t *pst_packet)
{
    if (pst_packet->pu08_hook != ts_null) {
        free(pst_packet->pu08_hook);
        pst_packet->pu08_hook = ts_null;
    }

    if (pst_packet->pu08_inf != ts_null) {
        free(pst_packet->pu08_inf);
        pst_packet->pu08_inf = ts_null;
    }
}


static struct option long_opts[] = {
    {"config",  required_argument,  NULL, GI_OPTS_CONFIG},
    {"hook",    required_argument,  NULL, GI_OPTS_HOOK},
    {"inf",     optional_argument,  NULL, GI_OPTS_INF},
    {"keyname", optional_argument,  NULL, GI_OPTS_KEYNAME},
    {"offset",  required_argument,  NULL, GI_OPTS_OFFSET},
    {"outf",    required_argument,  NULL, GI_OPTS_OUTF},
    {"svr",     optional_argument,  NULL, GI_OPTS_SVR},
    {"port",    optional_argument,  NULL, GI_OPTS_PORT},
    {"sign",    optional_argument,  NULL, GI_OPTS_SIGN},
    {NULL,      0,                  NULL, 0}
};

static void gen_usage(ts_void)
{
    printf("[option] is _one_ of the following:\n");
    printf("  --config          Image header configuration file\n");
    printf("  --hook            Hook file\n");
    printf("  --inf             Second boot file\n");
    printf("  --keyname         Key name\n");
    printf("  --offset          The offset by inf\n");
    printf("  --outf            The final generated file\n");
    printf("  --svr             Sign server ip\n");
    printf("  --port            Sign server port\n");
    printf("  --sign            Generated sign, if it is set to y\n");
}

static int ts_atoi(char *s)
{
    ts_char *pc_ptr = (char *)s;
    ts_char c;
    ts_s32 s32_number = 0;

    while ((c = *pc_ptr++) > 0) {
        if( (c >= '0') && (c <= '9'))
            s32_number = s32_number * 10 + (c - '0');
        else
            return -1;
    }

    return s32_number;
}

static int ts_atoh(char *s)
{
    ts_u08 u08_bit_cnt;
    ts_s32 s32_number;

    u08_bit_cnt = 0;
    s32_number = 0;
    while (*s != '\0') {
        if ((*s >= '0') && (*s <= '9'))
            s32_number = s32_number * 16 + *s++ - '0';
        else if ((*s >= 'a') && (*s <= 'f'))
            s32_number = s32_number * 16 + *s++ - 'a' + 10;
        else if ((*s >= 'A') && (*s <= 'F'))
            s32_number = s32_number * 16 + *s++ - 'A' + 10;
        else
            break;

        u08_bit_cnt++;
        if (u08_bit_cnt >= 8)
            break;      /* 8 Hex bits Max: tU4 Para */
    }

    return (s32_number);
}

static int ts_str2num(const char *s)
{
    ts_char *pc_ptr = (char *)s;
    ts_s32 s32_number;

    if (('0' == pc_ptr[0]) && (('x' == pc_ptr[1]) || ('X' == pc_ptr[1]))) {
        pc_ptr += 2;
        s32_number = ts_atoh(pc_ptr);
    } else {
        s32_number = ts_atoi(pc_ptr);
    }

    return s32_number;
}


/* 
 * 函数名 :  main
 * 描  述 :  主入口
 * 输  入 :  参数               描述
 *           argc                参数个数
 *           argv               输入参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
int main(int argc, char **argv)
{
    ts_s32 i, s32_ret;
    gi_packet_s_t st_packet;
    char c_opt;
    char *pc_cfgf_name;
    char *pc_hookf_name;
    char *pc_inf_name;
    char *pc_outf_name;
    ts_u08 *pu08_ptr;
    ts_u32 u32_size, u32_packet_opt, u32_calc_crc;
    FILE *fp_out_file;

    /* 显示编译信息 */
    printf("ts-tdk gen image V1.1 build %s : %s\n", __DATE__, __TIME__);

    pc_cfgf_name = (char *)&sc_conff_name;
    pc_hookf_name = (char *)&sc_hookf_name;
    pc_inf_name = (char *)&sc_inf_name;
    pc_outf_name = (char *)&sc_outf_name;

    memset((ts_void *)&st_packet, 0x00, sizeof(gi_packet_s_t));

    pc_cfgf_name[0] = '\0';
    pc_hookf_name[0] = '\0';
    pc_inf_name[0] = '\0';
    pc_outf_name[0] = '\0';
    st_packet.s32_sign_enable = 0;
    st_packet.s32_port = -1;
    while ((c_opt = getopt_long(argc, argv, "s", long_opts, NULL)) != -1) {
        switch (c_opt) {
            case GI_OPTS_CONFIG: {
                strcpy(pc_cfgf_name, optarg);

                break;
            }

            case GI_OPTS_HOOK: {
                strcpy(pc_hookf_name, optarg);

                break;
            }

            case GI_OPTS_INF: {
                strcpy(pc_inf_name, optarg);

                break;
            }

            case GI_OPTS_KEYNAME: {
                strcpy((ts_char *)&st_packet.ac_key_name, optarg);

                break;
            }

            case GI_OPTS_OUTF: {
                strcpy(pc_outf_name, optarg);

                break;
            }

            case GI_OPTS_OFFSET: {
                st_packet.u32_offset = ts_str2num(optarg);

                break;
            }

            case GI_OPTS_SVR: {
                strcpy((char *)&st_packet.au08_svr_ip, optarg);

                break;
            }

            case GI_OPTS_PORT: {
                st_packet.s32_port = ts_str2num(optarg);

                break;
            }

            case GI_OPTS_SIGN: {
                if (('y' == optarg[0]) || ('Y' == optarg[0]))
                    st_packet.s32_sign_enable = 1;
                else
                    st_packet.s32_sign_enable = 0;

                break;
            }

            case '?': {
                gen_usage();
                exit(1);

                break;
            }
        }
    }

    /* 根据输入组合，判断打包类型 */
    if (pc_hookf_name[0] != '\0')
        u32_packet_opt = GI_PACKET_HOOK;
    else if (pc_inf_name[0] != '\0')
        u32_packet_opt = GI_PACKET_INF;
    else
        u32_packet_opt = GI_PACKET_NONE;

    if (GI_PACKET_NONE == u32_packet_opt) {
        gen_usage();
        gi_help();
        exit(1);
    }

    /* 按照配置预读数据 */
    if (pc_cfgf_name[0] != '\0') {
        s32_ret = gi_load_cfg(pc_cfgf_name, (gi_packet_s_t *)&st_packet);
        if (TS_ERR == s32_ret)
            exit(1);

        st_packet.u32_ih_size = sizeof(bootup_ih_s_t);
    }

    /* read hook */
    if (pc_hookf_name[0] != '\0') {
        pu08_ptr = gi_load_file(pc_hookf_name, (ts_u32 *)&u32_size);
        if ((ts_null == pu08_ptr) || (0 == u32_size))
            goto _exit_1_;

        st_packet.pu08_hook = pu08_ptr;
        st_packet.u32_hook_size = u32_size;
    }

    /* read inf */
    if (pc_inf_name[0] != '\0') {
        pu08_ptr = gi_load_file(pc_inf_name, (ts_u32 *)&u32_size);
        if ((ts_null == pu08_ptr) || (0 == u32_size))
            goto _exit_1_;

        st_packet.pu08_inf = pu08_ptr;
        st_packet.u32_inf_size = u32_size;
    }

    printf("u32_packet_opt:%d\n", u32_packet_opt);

    if (GI_PACKET_INF == u32_packet_opt) {
        gi_inf_only((gi_packet_s_t *)&st_packet);

        

        fp_out_file = fopen(pc_outf_name, "wb");
        if (ts_null == fp_out_file) {
            printf("Open for write failed : filename %s!\n", pc_outf_name);

            goto _exit_1_;
        }

        fseek(fp_out_file, 0, SEEK_SET);

        /* 写回原始文件 */
        if (st_packet.u32_inf_size > 0) {
            fwrite((void *)(st_packet.pu08_inf), 1, st_packet.u32_inf_size, fp_out_file);
            if (st_packet.u32_inf_sign_size > 0)
                fwrite((void *)(&st_packet.st_inf_sign), 1, st_packet.u32_inf_sign_size, fp_out_file);  
        }

        fclose(fp_out_file);

    } else {
        gi_hook_only((gi_packet_s_t *)&st_packet);
        pu08_ptr = (ts_u08 *)&st_packet.st_ih;
        pu08_ptr += IH_BOOTUP_CRC_OFFSET;
        u32_calc_crc = verify_lib_calc_crc32(TS_CRC_INIT_VALUE, pu08_ptr, (sizeof(bootup_ih_s_t) - IH_BOOTUP_CRC_OFFSET));
        st_packet.st_ih.u32_ih_hcrc = u32_calc_crc;

        /* 按照 头部信息 + hook + [sign] */
        fp_out_file = fopen(pc_outf_name, "wb");
        if (ts_null == fp_out_file) {
            printf("Open for write failed : filename %s!\n", pc_outf_name);

            goto _exit_1_;
        }

        fseek(fp_out_file, 0, SEEK_SET);

        if (st_packet.u32_ih_size > 0)
            fwrite((void *)(&st_packet.st_ih), 1, sizeof(bootup_ih_s_t), fp_out_file);

        if (st_packet.u32_hook_size > 0) {
            fwrite((void *)(st_packet.pu08_hook), 1, st_packet.u32_hook_size, fp_out_file);
            if (st_packet.u32_hook_sign_size > 0)
                fwrite((void *)(&st_packet.st_hook_sign), 1, st_packet.u32_hook_sign_size, fp_out_file);
        }

        fclose(fp_out_file);

        /* 输出检查信息 */
        gi_check_cfg((bootup_ih_s_t *)&st_packet.st_ih);
    }

    if (1 == st_packet.s32_sign_enable) {
        if (st_packet.u32_hook_sign_size > 0)
            pu08_ptr = (ts_u08 *)&st_packet.st_hook_sign;
        else if (st_packet.u32_inf_sign_size > 0)
            pu08_ptr = (ts_u08 *)&st_packet.st_inf_sign;
        else
            pu08_ptr = ts_null;
        
        if (pu08_ptr != ts_null) {
            sha256_csum_wd((ts_u08 *)(pu08_ptr), TS_BOOTUP_RSA_KEY_LEN, (ts_u08 *)&(st_packet.au08_hash), 0);

            //gi_show_buffer((ts_u08 *)&(st_packet.au08_hash), 32, 0, 8);
            gi_show_sign2efuse((ts_u08 *)&(st_packet.au08_hash));
        }
    }

    printf("GI done\n");

_exit_1_:
    gi_free((gi_packet_s_t *)&st_packet);

    return 0;
}




