#include "chaos_a.h"

typedef struct
{
    int used_flag : 1;
    int sect_n : 31;
} sect_i;

typedef struct
{
    short used_b;
    short used_e;
    short free_b;
    short free_e;
    sect_i sects[];
} sects_meta;

typedef struct
{
    unsigned sect_size;
    unsigned sect_n;
    short merge_mem_delay;
    short merge_mem_delay_count;
    sects_meta meta;
} chaos_meta;

#define ChaosPtr(ptr) ((chaos_meta *)ptr)
#define Muti1_5(x) ((x << 1) + x)
#define Ceil(x, m) (((x & (m - 1)) != 0) + x / m) // 计算 x / m + (x % m != 0)，这就是为什么强调 m 必须为2的幂的原因

int EVILAPI init_chaosA(void *arr, unsigned sect_n, unsigned sect_size)
{
    if (!arr)
        return -1;
    chaos_meta *c = ChaosPtr(arr);
    c->sect_n = sect_n;
    c->sect_size = sect_size;
    c->merge_mem_delay = 64; // ? Magic Number
    c->merge_mem_delay_count = 0;
    // 初始化所有 Section 索引
    for (unsigned i = 0; i < sect_n; ++i)
    {
        c->meta.sects[i] = (sect_i){};
    }
    // 计算 Chaos 元数据占用 Section 数量
    unsigned sect_used_n = sect_n * sizeof(sect_i) + sizeof(chaos_meta);            // 大小（字节数）
    sect_used_n = ((sect_used_n & (sect_size - 1)) != 0) + sect_used_n / sect_size; // 大小（Section 数
    // 标记两个内存块，前面那个是 Chaos 元数据，后面那个是可用的空闲内存块
    c->meta.sects[0] = (sect_i){.sect_n = sect_used_n, .used_flag = 1};
    c->meta.sects[sect_used_n] = (sect_i){.sect_n = sect_n - sect_used_n, .used_flag = 0};
    // 更新两条链表 (used, free)
    // 因为 meta.sects 是弹性数组(autostorage)，故只更新前面4个成员
    // c->meta = (sects_meta){.used_b = 0, .used_e = 0, .free_b = sect_used_n, .free_e = sect_used_n};
    return 0;
}

int EVILAPI chaos_set_merge_delay(void *arr, short delay)
{
    if (!arr)
        return -1;
    chaos_meta *c = ChaosPtr(arr);
    c->merge_mem_delay = delay;
    if (delay > 0)
        c->merge_mem_delay_count = c->merge_mem_delay_count % delay; // 为什么会有顾客进入酒吧点炒饭呢...
    return 0;
}

int EVILAPI chaos_get_merge_delay(void *arr)
{
    if (!arr)
        return -1;
    chaos_meta *c = ChaosPtr(arr);
    return c->merge_mem_delay;
}

int EVILAPI chaos_merge_free_block(void *arr)
{
    if (!arr)
        return -1;
    chaos_meta *c = ChaosPtr(arr);
    // 遍历所有内存块
    for (unsigned i = 0; c->meta.sects[i].sect_n + i < c->sect_n /*判断此内存块释放为最末尾的内存块*/;)
    {
        // 获取下一个内存块基于当前内存块偏移地址
        unsigned s = c->meta.sects[i].sect_n;
        // 当前内存块：c->meta.sects[i]
        // 下一个内存块：c->meta.sects[i + s]
        if (c->meta.sects[i].used_flag == 0 && c->meta.sects[i + s].used_flag == 0) // 当前内存块空闲且下一个内存块空闲才能合并
        {
            c->meta.sects[i].sect_n += c->meta.sects[i + s].sect_n; // 将当前内存块与下一个内存块合并
            // c->meta.sects[i + s] = (sect_i){}; // 清除下一个内存块的数据（好像没必要？）
            continue;
        }
        else
        {
            i = c->meta.sects[i].sect_n + i;
            continue;
        }
    }
    c->merge_mem_delay_count = 0;
    return 0;
}

// 临时用函数
void *_chaos_allocA(void *arr, unsigned size)
{
    chaos_meta *c = ChaosPtr(arr);
    for (unsigned i = c->meta.sects[0].sect_n; i < c->sect_n; i += c->meta.sects[i].sect_n) // 最前面那部分内存被 Chaos 的元数据占用了，所以当然可用不用搜索它啊(理直气壮)
    {
        if (c->meta.sects[i].used_flag)
        { // 跳过已被使用的内存块
            continue;
        }
        unsigned s = c->meta.sects[i].sect_n * c->sect_size;
        if (s < size)
        { // 跳过小于指定要求的内存块
            continue;
        }
        if ((s - (s >> 4)) < size)
        { // 分配大小占内存块大小75%以上时 (这个时候内存块大小肯定大于等于分配大小的嘛)
            // 直接分配出去
            c->meta.sects[i].used_flag = 1;
            return (char *)arr + i * c->sect_size; // 别忘了，i 是当前内存块所在位置
        }
        else
        { // 分割，前半部分分配出去
            // 获取至少需要的 Section 数量
            unsigned ms = Ceil(size, c->sect_size);
            // 后半块内存
            c->meta.sects[i + ms] = (sect_i){.used_flag = 0, .sect_n = c->sect_n - ms};
            // 前半块内存
            c->meta.sects[i] = (sect_i){.used_flag = 1, .sect_n = ms};
            return (char *)arr + i * c->sect_size; // 别忘了，i 是当前内存块所在位置
        }
    }
    return ((void *)0x0);
}

void *chaos_allocA(void *arr, unsigned size)
{
    if (!arr)
        return ((void *)0x0);
    if (!size)
        return ((void *)0x0);             // 总会有顾客在酒吧点炒饭的...
    void *ptr = _chaos_allocA(arr, size); // 第一次尝试
    if (ptr)
        return ptr;
    // 失败后...
    chaos_merge_free_block(arr);
    ptr = _chaos_allocA(arr, size); // 第二次尝试
    return ptr;                     // 不得就不得了
}

int chaos_freeA(void *arr, void *ptr)
{
    if (!arr || !ptr)
        return -1;
    chaos_meta *c = ChaosPtr(arr);
    unsigned i = ((char *)ptr - (char *)arr) / c->sect_size; // 内存块位置
    c->meta.sects[i].used_flag = 0;

    // 定时合并内存块
    if (c->merge_mem_delay > 0)
    {
        c->merge_mem_delay_count += 1;
        if (c->merge_mem_delay_count >= c->merge_mem_delay)
        {
            chaos_merge_free_block(arr);
        }
    }
    return 0;
}
