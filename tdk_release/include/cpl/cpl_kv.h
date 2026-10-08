/*
 * cpl_kv.h -- string based key-value library
 */
#ifndef __CPL_KV_H__
#define __CPL_KV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

typedef struct ts_cpl_kv_hdl_s {
    ts_char *pc_mem;
    ts_u32 u32_size;
    ts_s32 s32_inited;
} ts_cpl_kv_hdl_s_t;

/*
 * Memory format:
 *     key=value\0key=value\0\0
 *
 * The registered memory stores strings only. The context information is kept in
 * kv_handle_s_t, outside of the registered memory.
 *
 * The library does not do internal locking. Users should protect the same
 * kv_handle_s_t from concurrent access if it is used by multiple threads.
 */
ts_s32 ts_cpl_kv_init(ts_void *pv_handle, ts_char *pc_mem, ts_size_t u32_size);
ts_void ts_cpl_kv_destroy(ts_void *pv_handle);
ts_void ts_cpl_kv_clear(ts_void *pv_handle);

ts_s32 ts_cpl_kv_del(ts_void *pv_handle, const ts_char *pc_key);
ts_s32 ts_cpl_kv_exists(ts_void *pv_handle, const ts_char *pc_key);

ts_size_t ts_cpl_kv_used(ts_void *pv_handle);
ts_size_t ts_cpl_kv_capacity(ts_void *pv_handle);

ts_s32 ts_cpl_kv_set_str(ts_void *pv_handle, const ts_char *pc_key, const ts_char *pc_value);
ts_s32 ts_cpl_kv_get_str(ts_void *pv_handle, const ts_char *pc_key, ts_char *pc_value, ts_size_t *pu32_size);
ts_s32 ts_cpl_kv_set_u32(ts_void *pv_handle, const ts_char *pc_key, const ts_u32 *pu32_value, ts_size_t u32_count);
ts_s32 ts_cpl_kv_get_u32(ts_void *pv_handle, const ts_char *pc_key, ts_u32 *pu32_value, ts_size_t *pu32_count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CPL_KV_H__ */

