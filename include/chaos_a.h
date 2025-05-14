/*
 * Copyright (c) 2025 Qiong-Mengzi
 *
 * SGVector 项目的定长内存池喵
 * 简易的实现，至少单线程性能到位了 (?)
 * 好乱...好累...
 *
 * 我曾经想到过使用单链表，这样分配是O(1)~O(n)，释放是O(1)，
 * 但需要花费时间 GC (释放已经释放的内存) 和链表排序(不会写归并排序QwQ)
 */

#ifndef __SG_CHAOS_ARRAY__
#define __SG_CHAOS_ARRAY__ 1

#ifndef EVILAPI
// EVILAPI 调用约定 `__cdecl`
#define EVILAPI __cdecl
#endif

/*
 * 「好想成为喜欢之人的女仆啊...」
 *
 * 对一块内存初始化为ChaosArray内存池
 * arr: void*, 一块可用的内存
 * sect_n: unsigned, Section 数量，不超过 32767
 * sect_size: unsigned, Section 的大小，必须为2的幂（至少为512）
 */
int EVILAPI init_chaosA(void *arr, unsigned sect_n, unsigned sect_size);

/*
 * 「如果那是真的就好了...」
 * 释放ChaosArray内存池，其实并没有什么卵用 (这个内存应该自己释放，Chaos酱只负责管理)
*/
int EVILAPI release_chaosA(void *arr) { return 0; }

/*
 * 「只需要乖乖听主人的话就行了...不用再想其他事情了呢...」
 * 「只是活着就足够了啊...」
 *
 * 合并内存块操作
 * 在进行释放操作一定次数后调用 (默认为64次)
 */
int EVILAPI chaos_merge_free_block(void *arr);

/*
 * 「在下到底是一个什么样的人呢...」
 * 「这个问题显然并不能从其他人的言语中得到答案...」
 *
 * 设置合并内存块操作间隔，-1 为不合并
 *
 * 时间复杂度 `O(n)`
 */
int EVILAPI chaos_set_merge_delay(void *arr, short delay);

/*
 * 「无论怎样，主人都可以随便玩弄在下的...」
 * 「没关系的...」
 *
 * 获取当前合并内存块操作间隔
 * ~~至于为什么返回值是 `int` 而不是 `short`, 不知道呢~~
 */
int EVILAPI chaos_get_merge_delay(void *arr);

/*
 * 「不要再问我以后想考什么样的大学，做什么工作了啊...」
 * 「*独立自主* 这种事情在下根本做不到的啊...」
 * 「能够在主人身边已经是一件很幸福的事情了呢...」
 *
 * 分配一块大内存 (返回的内存块大小为 Section 大小的整数倍)
 * 分配 0 字节内存我会想拍死你 (return `NULL`)
 * 分配失败先合并内存块再尝试分配，若仍失败返回 `NULL`
 *
 * 时间复杂度 `O(n)`
 */
void *chaos_allocA(void *arr, unsigned size);

/*
 * 「*憎恨*过往，*逃避*现在，*诅咒*未来」
 * 「我，究竟因何而存在...」
 *
 * 释放分配的内存
 * 自己心里应该清楚哪块内存是从这来的，有没有释放过
 * 出问题了我可不管，那是你自己的问题
 *
 * 时间复杂度 `O(1)`，如果同时合并内存块，则为 `O(n)`
 */
int chaos_freeA(void *arr, void *ptr);

#endif

// * 这小小的一部分结束了，「该睡觉了，梦里面什么都有呢...」
// * 「生命因何而沉睡...? - 因为我们害怕从梦中醒来...」
