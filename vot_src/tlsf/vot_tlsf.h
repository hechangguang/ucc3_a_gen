#ifndef  __VOT_TLSF_H__
#define  __VOT_TLSF_H__

#if defined(__cplusplus)
extern "C" {
#endif


#include <stdio.h>
#include <cpl/ts_cpl.h>
#include <tlsf.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined (TLSF_64BIT)

#define SAFE_ALIGN_SIZE     8  //地址对其的字节数 4字节或者8字节
#else

#define SAFE_ALIGN_SIZE     4  //地址对其的字节数 4字节或者8字节
#endif

#define SAFE_MAGIC_ALLOC    0x5A5A1234  // 已分配状态的魔术字
#define SAFE_MAGIC_FREE     0xDEADBEEF  // 已释放状态的魔术字

#define SAFE_CANARY_VALUE   0x12345678

/* 包装安全头：位于 TLSF 实际分配的内存头部 */
typedef struct {
    uint32_t magic;         // 魔术字，用于识别块状态和防踩内存  // 4 字节
    size_t size;            // 记录用户申请的真实尺寸  // 4 字节
    const char* file;       // 调用者所在的文件名指针（指向 Flash 常量区，只占 4 字节）
    uint32_t line;          // 调用者所在的行号，占用 4 字节
    uint32_t canary;        // 专门用于判断是否是越界写入
    uintptr_t checksum;     // 校验和，防止整个 Header 被意外篡改 // 4 字节
    uintptr_t raw_ptr;
    uint32_t reserved;
} SafeHeader_t; //总大小保持32字节对齐，用于兼容4字节和8字节对齐



typedef struct VotMemPoolInfo_s
{
    ts_u32 m_u32_pool_size;
    tlsf_t m_tlsf_handle;
    ts_void*  m_tlsf_pool_buffer;
    char m_pool_name[64];
    ts_cpl_mutex_s_t m_tlsf_mutex;
    ts_u32 m_alloc_count;
    ts_u32 m_pool_start_addr;
    ts_u32 m_pool_end_addr;


}VotMemPoolInfo_s_t;


extern VotMemPoolInfo_s_t g_VotMemPoolInfo;

/* 宏包装：无缝替换 */
#define vot_tlsf_safe_malloc(mem_pool_info_ptr,size)   tlsf_safe_malloc_debug(mem_pool_info_ptr,size, __FILE__, __LINE__)

void vot_tlsf_mem_init(VotMemPoolInfo_s_t* mem_pool_info_ptr, ts_u32 u32_pool_size, char* pool_name);
void vot_tlsf_mem_deinit(VotMemPoolInfo_s_t* mem_pool_info_ptr);
void vot_tlsf_safe_free(VotMemPoolInfo_s_t* mem_pool_info_ptr,void** p_ptr);
void* tlsf_safe_malloc_debug(VotMemPoolInfo_s_t* mem_pool_info_ptr,size_t size, const char* file, uint32_t line);
void vot_tlsf_debug(VotMemPoolInfo_s_t* mem_pool_info_ptr);
void vot_tlsf_stress_test(VotMemPoolInfo_s_t* mem_pool_info_ptr);

#if defined(__cplusplus)
};
#endif


#endif



