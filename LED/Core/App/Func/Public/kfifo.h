#ifndef __KFIFO_H__
#define __KFIFO_H__

#include <stdbool.h>
#include <stdint.h>

/* 常用缓冲区大小 */
#define KFIFO_SIZE_16    16
#define KFIFO_SIZE_32    32
#define KFIFO_SIZE_64    64
#define KFIFO_SIZE_128   128
#define KFIFO_SIZE_256   256
#define KFIFO_SIZE_1024  1024

/*
 * kfifo 结构体
 * SPSC 模型：中断写 / 任务读
 */
typedef struct {
    uint8_t *buffer;
    uint32_t size;      // 必须是 2^n
    uint32_t in;        // 写计数
    uint32_t out;       // 读计数
    bool     dynamic;   // 是否 malloc 分配
} kfifo_t;

/* 初始化（用户提供缓冲区） */
bool kfifo_init(kfifo_t *fifo,uint8_t *buffer,uint32_t size);

/* 初始化（内部 malloc） */
bool kfifo_alloc(kfifo_t *fifo,uint32_t size);

/* 释放（仅释放 malloc 的 buffer） */
void kfifo_free(kfifo_t *fifo);

/* 重置 */
void kfifo_reset(kfifo_t *fifo);

/* 状态 */
uint32_t kfifo_size(const kfifo_t *fifo);

uint32_t kfifo_len(const kfifo_t *fifo);

uint32_t kfifo_avail(const kfifo_t *fifo);

bool kfifo_is_empty(const kfifo_t *fifo);

bool kfifo_is_full(const kfifo_t *fifo);

/* 数据操作 */
bool kfifo_put(kfifo_t *fifo,uint8_t data);

bool kfifo_get(kfifo_t *fifo,uint8_t *data);

uint32_t kfifo_in(kfifo_t *fifo,const uint8_t *buf,uint32_t len);

uint32_t kfifo_out(kfifo_t *fifo,uint8_t *buf,uint32_t len);

/**
 * @brief 读取数据不移动位置
 * @param fifo
 * @param data
 * @param dataLen
 * @return
 */
uint32_t kfifo_fetch(kfifo_t *fifo,uint8_t *data,int dataLen);

uint32_t kfifo_skip(kfifo_t *fifo,uint32_t len);

#endif /* __KFIFO_H__ */
