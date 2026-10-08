#ifndef __LINUX_OF_H__
#define __LINUX_OF_H__

ts_s32 of_match_compatible(ts_s32 s32_offset, char *pc_compatible);
ts_s32 of_get_reg(ts_s32 s32_node_offset, ts_cpl_res_s_t *pst_reg_res, ts_u32 u32_num);
ts_s32 of_get_irq(ts_s32 s32_node_offset, ts_u32 *pu32_cnt, ts_u32 u32_num);
ts_s32 of_get_clk(ts_s32 s32_node_offset, ts_u32 u32_num);

ts_s32 of_get_int(ts_s32 s32_node_offset, ts_char *pv_prop_name, ts_u32 u32_name_len, \
            ts_u32 u32_num, ts_s32 s32_def_val);
ts_char *of_get_string(ts_s32 s32_node_offset, ts_char *pv_prop_name, ts_u32 u32_name_len);

#endif
