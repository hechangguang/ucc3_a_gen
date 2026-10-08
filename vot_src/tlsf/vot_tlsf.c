#include<vot_tlsf.h>
#include "api.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>

VotMemPoolInfo_s_t g_VotMemPoolInfo;

/*
使用说明
1、申请的内存块，尽可能不要长时间暂用，需要及时释放
2、vot_tlsf_safe_malloc 分配的内存块，不使用时，必须主动调用vot_tlsf_safe_free进行释放 
3、查看当前内存池使用的情况，可以调用vot_tlsf_debug
*/

// 保证 8 字节对齐
// static uint8_t tlsf_pool_buffer[MY_POOL_SIZE] __attribute__((aligned(8)));


// 初始化
void vot_tlsf_mem_init(VotMemPoolInfo_s_t* mem_pool_info_ptr, ts_u32 u32_pool_size, char* pool_name) 
{
    static uint32_t mem_pool_cnt=0;
    if(mem_pool_info_ptr==NULL){
        vision_log_err("error : mem_pool_info_ptr == NULL \n");
        return;
    }

    // 1. 创建互斥锁用于多任务安全访问
	ts_cpl_mutex_init(&mem_pool_info_ptr->m_tlsf_mutex);
    mem_pool_info_ptr->m_u32_pool_size = u32_pool_size;

    snprintf(mem_pool_info_ptr->m_pool_name,64,"%s%lu",(pool_name)?pool_name:"tlsf_default",mem_pool_cnt);

    mem_pool_info_ptr->m_tlsf_pool_buffer = (ts_void*)ts_cpl_tmm_malloc(ts_null, u32_pool_size, 32, mem_pool_info_ptr->m_pool_name);
    mem_pool_info_ptr->m_pool_start_addr = (ts_u32)mem_pool_info_ptr->m_tlsf_pool_buffer;
    mem_pool_info_ptr->m_pool_end_addr = (ts_u32)mem_pool_info_ptr->m_tlsf_pool_buffer+u32_pool_size;
    
    vision_log_warn("m_pool_start_addr 0x%x  m_pool_end_addr 0x%x \n", mem_pool_info_ptr->m_pool_start_addr,  mem_pool_info_ptr->m_pool_end_addr);
    // 2. 将 u32_pool_size 大小内存块交由 TLSF 接管
    // tlsf_create_with_pool 会在 buffer 的最头部创建 control_t 管理区，剩下的作为空闲内存池
    mem_pool_info_ptr->m_tlsf_handle = tlsf_create_with_pool(mem_pool_info_ptr->m_tlsf_pool_buffer, u32_pool_size);
    if(mem_pool_info_ptr->m_tlsf_handle==NULL){
        vision_log_err("error : mem_pool_info_ptr->m_tlsf_handle == NULL \n");
    }
    TS_CPL_ASSERT(mem_pool_info_ptr->m_tlsf_handle != NULL);
    mem_pool_info_ptr->m_alloc_count=0;
    mem_pool_cnt++;
}

void vot_tlsf_mem_deinit(VotMemPoolInfo_s_t* mem_pool_info_ptr) 
{
    tlsf_destroy(mem_pool_info_ptr->m_tlsf_handle);
    if (mem_pool_info_ptr->m_tlsf_pool_buffer)
	{
		ts_cpl_tmm_free((ts_void *)mem_pool_info_ptr->m_tlsf_pool_buffer);
		mem_pool_info_ptr->m_tlsf_pool_buffer = 0;
	}

    ts_cpl_mutex_destory(&mem_pool_info_ptr->m_tlsf_mutex);
}

static void* vot_tlsf_malloc(VotMemPoolInfo_s_t* mem_pool_info_ptr,size_t size) 
{
    void* ptr = NULL;
    
    ptr = tlsf_malloc(mem_pool_info_ptr->m_tlsf_handle, size);

    if(ptr){
        mem_pool_info_ptr->m_alloc_count++; 
    }
 
    if(!ptr){
        vision_log_err("ptr 0x%x \n",(ts_u32)ptr);
    }
 
    // vision_log_warn("ptr 0x%x \n",(ts_u32)ptr);
    return ptr;
}

static void vot_tlsf_free(VotMemPoolInfo_s_t* mem_pool_info_ptr,void* ptr) 
{
    if (!ptr) return;

    // vision_log_warn("ptr 0x%x \n",(ts_u32)ptr);

    tlsf_free(mem_pool_info_ptr->m_tlsf_handle, ptr);
    if(ptr){
        mem_pool_info_ptr->m_alloc_count--;
    }


}

// void* vot_tlsf_realloc(VotMemPoolInfo_s_t* mem_pool_info_ptr,void* ptr, size_t size) 
// {
//     void* new_ptr = NULL;
   
//     new_ptr = tlsf_realloc(mem_pool_info_ptr->m_tlsf_handle, ptr, size);

//     if(new_ptr)
//     {
//         mem_pool_info_ptr->m_alloc_count++;
//     }

//     if(!new_ptr){
//         vision_log_warn("ptr 0x%x \n",(ts_u32)ptr);
//     }

//     return new_ptr;
// }



//截取文件名（兼容 Linux/Windows 的路径分隔符）
static const char* get_filename(const char* filepath)
{
    if (!filepath) return "unknown";
    
    // 优先查找 Unix 风格的斜杠 '/'
    const char* file = strrchr(filepath, '/');
    if (file) {
        return file + 1; // 跳过 '/'，指向文件名首字符
    }
    
    // 兼容可能存在的 Windows 风格反斜杠 '\'
    file = strrchr(filepath, '\\');
    if (file) {
        return file + 1;
    }

    return filepath; // 如果没有路径分隔符，说明本身就是文件名
}

void vot_default_walker(void* ptr, size_t size, int used, void* user)
{
    (void)user;

    void* raw_ptr = ptr;
    SafeHeader_t* header = (SafeHeader_t*)raw_ptr;
    if(used){
        vision_log_warn("\t%p %s size: %x  (%s:%lu)\n", ptr, used ? "used" : "free",  (unsigned int)size,get_filename(header->file),header->line);
    }
    else{
        vision_log_warn("\t%p %s size: %x  \n", ptr, used ? "used" : "free",  (unsigned int)size); 
    }

}

void vot_tlsf_debug(VotMemPoolInfo_s_t* mem_pool_info_ptr) 
{
    pool_t m_pool = tlsf_get_pool(mem_pool_info_ptr->m_tlsf_handle);

    tlsf_walk_pool(m_pool, &vot_default_walker,NULL);

    vision_log_warn("m_alloc_count %d \n",mem_pool_info_ptr->m_alloc_count);
}

//压力测试函数，多线程下，同时运行调用，进行压力测试
void vot_tlsf_stress_test(VotMemPoolInfo_s_t* mem_pool_info_ptr) 
{
    #define MAX_ITEMS 100
    void* test_ptrs[MAX_ITEMS] = {NULL};

    for (int i = 0; i < 1000; i++) { // 循环 10 万次
        int index = rand() % MAX_ITEMS;

        // 如果该位置已有内存，将其释放
        if (test_ptrs[index] != NULL) {
            vot_tlsf_safe_free(mem_pool_info_ptr,&test_ptrs[index]);
            test_ptrs[index] = NULL;
        } else {
            // 随机生成 8 字节到 4KB 的分配尺寸
            size_t size = (rand() % 4096) + 8;
            test_ptrs[index] = vot_tlsf_safe_malloc(mem_pool_info_ptr,size);

            if (test_ptrs[index]) {
                // 填充伪随机数据以检测是否越界写入
                memset(test_ptrs[index], 0x5A, size);
            }
        }
        ts_cpl_fix_msleep(100);
    }

    // 最终全部释放，检测内存池是否完好如初
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (test_ptrs[i]) vot_tlsf_safe_free(mem_pool_info_ptr,&test_ptrs[i]);
    }

    // 验证：此时重新申请一个几乎等于 XXM 的整块，必须成功！
    // void* final_check = vot_tlsf_safe_malloc(mem_pool_info_ptr,5 * 1024 * 1024 - 10 * 1024); // 减去少量控制开销
    // TS_CPL_ASSERT(final_check != NULL);
    // vot_tlsf_safe_free(mem_pool_info_ptr,(void**)&final_check);

}


static inline uintptr_t calculate_header_checksum(SafeHeader_t* header) 
{
    // 异或校验和：用 magic 和 size 异或，增加篡改识别率
    return (uintptr_t)header->magic ^ (uintptr_t)header->size ^ (uintptr_t)header ^ 0xAAAAAAAA;
}

/* 核心防线：安全校验指针函数 */
static bool is_pointer_valid(VotMemPoolInfo_s_t* mem_pool_info_ptr,void* ptr, SafeHeader_t** out_header) 
{
    if (ptr == NULL) {
        return false;
    }

    uintptr_t addr = (uintptr_t)ptr;

    // 1. 物理边界校验 (Range Check)
    // 指针必须落在  内存池的物理区间范围内
    if (addr <  mem_pool_info_ptr->m_pool_start_addr + sizeof(SafeHeader_t) || addr >= mem_pool_info_ptr->m_pool_end_addr) {
        vision_log_warn("error addr  0x%x \n", addr);
        return false; 
    }

    // 2. 对齐校验 (Alignment Check)
    // 32位系统下通常要求 4 或 8 字节对齐
    if (addr & (SAFE_ALIGN_SIZE - 1)) {
        vision_log_warn(" SAFE_ALIGN_SIZE \n");
        return false;
    }


    // 找到我们包装的安全控制头
    SafeHeader_t* header = (SafeHeader_t*)(addr - sizeof(SafeHeader_t));
    *out_header = header;

    if (header->canary != SAFE_CANARY_VALUE) {
        vision_log_warn(" header->canary 0x%lx \n",header->canary);
    }

    // 3. 校验和验证 (Checksum Check)
    // 防止此 Header 被相邻的越界内存写坏（踩内存）
    uintptr_t calc_sum = calculate_header_checksum(header);
    if (header->checksum != calc_sum) {
        vision_log_warn("--- MEMORY CORRUPTION DETECTED ---\n");
        vision_log_warn("Ptr: %p, Header: %p\n", ptr, (void*)header);
        vision_log_warn("Magic: 0x%lx (Expected: 0x%x)\n", header->magic, SAFE_MAGIC_ALLOC);
        vision_log_warn("Size: %d bytes\n", header->size);
        // vision_log_warn("File/Line: %s:%d\n", header->file ? header->file : "Unknown", header->line);
        vision_log_warn("Checksum in Header: 0x%x, Calculated: 0x%x\n", header->checksum, calc_sum);
        vision_log_warn(" header->canary 0x%lx \n",header->canary);
        vision_log_warn("----------------------------------\n");
        vision_log_warn("error calculate_header_checksum  header->checksum 0x%x 0x%x \n",header->checksum ,calc_sum);
        return false; // Header 数据已被损坏
    }

    // 4. 魔术字状态校验 (Status Check)
    if (header->magic == SAFE_MAGIC_FREE) {
        vision_log_warn(" SAFE_MAGIC_FREE \n");
        // 重复释放 (Double Free) 拦截
        return false; 
    }
    
    if (header->magic != SAFE_MAGIC_ALLOC) {
        vision_log_warn(" SAFE_MAGIC_ALLOC \n");
        // 未知非法内存或严重损坏
        return false;
    }

    return true;
}

/* 安全分配包装 */
// void* vot_tlsf_safe_malloc_nodebug(VotMemPoolInfo_s_t* mem_pool_info_ptr,size_t size) 
// {
//     if (size == 0) return NULL;

    
//     ts_cpl_mutex_lock(&mem_pool_info_ptr->m_tlsf_mutex);

//     // 我们实际向 TLSF 申请的空间 = 用户尺寸 + 安全头大小
//     size_t total_size = size + sizeof(SafeHeader_t);

//     void* raw_ptr = vot_tlsf_malloc(mem_pool_info_ptr, total_size);

//     if (!raw_ptr) return NULL;

//     // 在头部填充安全信息
//     SafeHeader_t* header = (SafeHeader_t*)raw_ptr;
//     header->magic = SAFE_MAGIC_ALLOC;
//     header->size = size;
//     header->checksum = calculate_header_checksum(header);

//     ts_cpl_mutex_unlock(&mem_pool_info_ptr->m_tlsf_mutex);
//     // 返回越过安全头后的用户可用地址
//     return (void*)((uint8_t*)raw_ptr + sizeof(SafeHeader_t));
// }

/* 安全释放包装 (传入指针的指针，释放后自动清空，防止野指针) */
void vot_tlsf_safe_free(VotMemPoolInfo_s_t* mem_pool_info_ptr,void** p_ptr) 
{
    if (p_ptr == NULL || *p_ptr == NULL) {
        return; 
    }

    ts_cpl_mutex_lock(&mem_pool_info_ptr->m_tlsf_mutex);

    void* ptr = *p_ptr;
    SafeHeader_t* header = NULL;

    // 执行严格的四重校验
    if (!is_pointer_valid(mem_pool_info_ptr,ptr, &header)) {
        ts_cpl_mutex_unlock(&mem_pool_info_ptr->m_tlsf_mutex);
        // 【核心安全动作】捕获到非法指针输入！
        // 打印日志/触发断言/调用故障恢复系统，阻止非法释放传入 TLSF 导致系统 HardFault
        // #ifdef DEBUG
        vision_log_warn("Error: Invalid pointer free detected at %p!\n", ptr);
        // #endif
        return; 
    }

    // 释放前将魔术字“毒化”为 FREE 状态，预防未来可能发生的 Double Free
    header->magic = SAFE_MAGIC_FREE;
    header->checksum = calculate_header_checksum(header);

    // 还原出 TLSF 原始分配的头部指针进行释放
    vot_tlsf_free(mem_pool_info_ptr, (void*)header->raw_ptr);

    // 指针清空，彻底切断应用层的野指针复用
    *p_ptr = NULL;
    ts_cpl_mutex_unlock(&mem_pool_info_ptr->m_tlsf_mutex);
}


#define VOT_TLSF_ALIGN_UP(addr, align) (((uintptr_t)(addr) + ((align) - 1)) & (~((uintptr_t)(align) - 1)))
#define VOT_TLSF_ALIGN_SIZE 32 //ts_lib_up_start 函数需要内存地址32字节对齐，故此处对地址和SafeHeader_t分别进行32字节对齐

void* tlsf_safe_malloc_debug(VotMemPoolInfo_s_t* mem_pool_info_ptr,size_t size, const char* file, uint32_t line) 
{
    if (size == 0) return NULL;

    size_t total_size = size + sizeof(SafeHeader_t) + VOT_TLSF_ALIGN_SIZE - 1;

    ts_cpl_mutex_lock(&mem_pool_info_ptr->m_tlsf_mutex);
    void* raw_ptr = vot_tlsf_malloc(mem_pool_info_ptr, total_size);

    if (!raw_ptr) {
        ts_cpl_mutex_unlock(&mem_pool_info_ptr->m_tlsf_mutex);
        return NULL;
    }
 
    uint8_t * align_addr = (void*)(VOT_TLSF_ALIGN_UP(raw_ptr,VOT_TLSF_ALIGN_SIZE));

    SafeHeader_t* header = (SafeHeader_t*)align_addr;
    header->magic = SAFE_MAGIC_ALLOC;
    header->size = size;
    header->file = file; // 仅存指针，指向 Flash 中的字符串常量
    header->line = line;
    header->canary = SAFE_CANARY_VALUE;
    header->checksum = calculate_header_checksum(header);
    header->raw_ptr = (uintptr_t)raw_ptr;
   
    ts_cpl_mutex_unlock(&mem_pool_info_ptr->m_tlsf_mutex);
    // vision_log_warn("header->checksum 0x%x  \n",header->checksum );

    return (void*)((uint8_t*)align_addr + sizeof(SafeHeader_t));
}

