/*
 * @Author: shixiaobo 328494516@qq.com
 * @Date: 2025-08-01 16:56:53
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-08-28 15:54:32
 * @FilePath: /ucc3_a_gen/vot_src/inc/sffs_files_vot.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __SFFS_FILES_VOT_H__
#define __SFFS_FILES_VOT_H__

#define FN_APP_CFG                      SFFS_GEN_FULLNAME("base/fn_app_cfg")
#define FN_APP_FILE_SIZE                (1024 * 1024 * 1)

#define FN_APP_MENU2_CFG                SFFS_GEN_FULLNAME("base/fn_app_menu2_cfg")
#define FN_APP_MENU2_FILE_SIZE          (1024 * 1024 * 1)

#define FN_MENU1_DEF_PARA_CFG           SFFS_GEN_FULLNAME("base/fn_user_para_cfg")
#define FN_MENU1_DEF_PARA_SIZE          (1024 * 1024)

#define FN_MENU2_DEF_PARA_CFG            SFFS_GEN_FULLNAME("base/fn_sys_para_cfg")
#define FN_MENU2_DEF_PARA_SIZE           (1024 * 1024)

#define FN_VOT_ISP_CFG                  SFFS_GEN_FULLNAME("base/fn_vot_isp_cfg")
#define FN_VOT_ISP_FILE_SIZE            (1024 * 1024 * 1)


#define FN_VOT_NUC_KB_CFG               SFFS_GEN_FULLNAME("base/fn_vot_nuckb_cfg")
#define FN_VOT_NUC_KB_FILE_SIZE         (1280 * 1024 * 2 * 4 + 4096)

#define FN_MOTORSTORAGE_PARA_CFG         SFFS_GEN_FULLNAME("base/fn_motorstorage_para_cfg")
#define FN_MOTORSTORAGE_PARA_FILE_SIZE   (1024 * 200 * 1)

#define FN_VOT_TEMPPARA_CFG             SFFS_GEN_FULLNAME("base/fn_vot_temppara_cfg")
#define FN_VOT_TEMPPARA_FILE_SIZE       (1024 * 256 * 1)

#define N6E_FN_OCC_FILE_SIZE            (700 * 600 * 1 + 4096)
#define N6E_FN_K_FILE_SIZE              (640 * 512 * 2 + 4096)
#define N6E_FN_B_FILE_SIZE              (640 * 512 * 4 + 4096)
#define N6E_FN_SFFC_FILE_SIZE           (640 * 512 * 6 + 4096)
#define N6E_FN_AISP_FILE_SIZE           (1024 * 64)
#define N6E_FN_VOT_NUC_KB_FILE_SIZE     (1024 * 45)   

#endif