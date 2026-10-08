/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __RISCV_BITS_H__
#define __RISCV_BITS_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* for riscv32 */
#define ts_rv_int_t     unsigned int

#if ((1 == CONFIG_SUPPORT_FLOAT) || (1 == CONFIG_SUPPORT_DOUBLE))
    #define TSP_SUPPORT_FLOAT
    #ifdef CONFIG_SUPPORT_FLOAT
        #define TSP_FLOAT_REGBW         4
        #define TSP_SP_OFFSET           30
        #define TSP_SP_FSW              fsw
        #define TSP_SP_FLW              flw
        #define ts_rv_float_t           unsigned int
        #define TSP_SP_ALIGN_BW         4
    #else
        #define TSP_FLOAT_REGBW         8
        #define TSP_SP_OFFSET           15
        #define TSP_SP_FSW              fsd
        #define TSP_SP_FLW              fld
        #define ts_rv_float_t           unsigned long long
        #define TSP_SP_ALIGN_BW         8
    #endif
#else
    #undef TSP_SUPPORT_FLOAT
    #define TSP_SP_ALIGN_BW             4
#endif


#ifdef __cplusplus
}
#endif

#endif /** __RISCV_BITS_H__  */
