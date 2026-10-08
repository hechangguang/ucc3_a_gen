#ifndef _LINUX_MODULE_H
#define _LINUX_MODULE_H
/*
 * Dynamic loading of modules into the kernel.
 *
 * Rewritten by Richard Henderson <rth@tamu.edu> Dec 1996
 * Rewritten again by Rusty Russell, 2002
 */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#ifndef __init
#define __init      __attribute__((section(".init")))
#define _ts_init_   __init
#endif

#ifndef __exit
#define __exit      __attribute__((unused))
#define _ts_exit_   __exit
#endif

/* For every exported symbol, place a struct in the __ksymtab section */
#define __EXPORT_SYMBOL(sym, sec)

#define EXPORT_SYMBOL(sym)					\
	__EXPORT_SYMBOL(sym, "")

#define EXPORT_SYMBOL_GPL(sym)					\
	__EXPORT_SYMBOL(sym, "_gpl")

#define EXPORT_SYMBOL_GPL_FUTURE(sym)				\
	__EXPORT_SYMBOL(sym, "_gpl_future")

struct module {
    ts_void *pv_parent;
};

typedef ts_s32 (*module_init_call_t)(ts_void);

typedef struct module_data_s {
    const ts_char *pc_name;
    module_init_call_t pfn_init;
} module_data_s_t;

typedef struct module_param_data_s {
    const ts_char *pc_module_name;
    const ts_char *pc_param_name;
    const ts_char *pc_param_type;
    ts_phys_addr_t *ppa_param_addr;
} module_param_data_s_t;

#define TS_KMOD_SECTION 		__attribute__((section(".kmod")))

#define module_init(init_call)  volatile module_init_call_t TS_KMOD_SECTION _initcall_##init_call = (module_init_call_t)init_call
#define module_exit(exit_call) 

#define module_param(name, type, perm)

/* for tdk */
extern ts_void insmod_parse_param(ts_char *pc_mod_name, ts_char *pc_mod_args);

#ifdef CONFIG_TDK_INSMOD
#define TS_INSMOD_SECTION 		__attribute__((section(".insmod")))
#define TS_MODPARAM_SECTION 	__attribute__((section(".modparam")))

#define ins_module_init(name, init_call)  \
        volatile const static TS_INSMOD_SECTION module_data_s_t _module_##name = {    \
            .pc_name = #name,     \
            .pfn_init = init_call,           \
        };

#define ins_module_param(mod_name, name, type, perm)     \
        const module_param_data_s_t TS_MODPARAM_SECTION modp_list_##mod_name_##name = { \
            .pc_module_name = #mod_name,    \
            .pc_param_name = #name,    \
            .pc_param_type = #type,    \
            .ppa_param_addr = (ts_phys_addr_t *)&name,    \
        }; 
#else
#define ins_module_init(name, init_call)                 module_init(init_call)
#define ins_module_param(mod_name, name, type, perm)     module_param(name, type, perm)
#endif


#define ts_cpl_insmod_declare(mod)  volatile module_data_s_t * TS_INSMOD_SECTION _insm_##mod = (module_data_s_t *)&mod
#define ts_cpl_modp_declare(param)  volatile module_param_data_s_t * TS_MODPARAM_SECTION _modp_##param = (module_param_data_s_t *)&param


#define THIS_MODULE ((struct module *)0)

#endif /* _LINUX_MODULE_H */
