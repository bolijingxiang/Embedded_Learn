#include "kfifo.h"

#include <stdlib.h>
#include <string.h>

/* 判断是否 2 的幂 */
static bool is_power_of_two(uint32_t x) {
    return x && !(x & (x - 1));
}

bool kfifo_init(kfifo_t *fifo,uint8_t *buffer,uint32_t size) {
    if (!fifo || !buffer || !is_power_of_two(size)) {
        return false;
    }

    fifo->buffer  = buffer;
    fifo->size    = size;
    fifo->in      = 0;
    fifo->out     = 0;
    fifo->dynamic = false;
    return true;
}

bool kfifo_alloc(kfifo_t *fifo,uint32_t size) {
    if (!fifo || !is_power_of_two(size)) {
        return false;
    }

    uint8_t *buf = (uint8_t *) malloc(size);
    if (!buf) {
        return false;
    }

    fifo->buffer  = buf;
    fifo->size    = size;
    fifo->in      = 0;
    fifo->out     = 0;
    fifo->dynamic = true;

    return true;
}

void kfifo_free(kfifo_t *fifo) {
    if (!fifo) return;

    if (fifo->dynamic && fifo->buffer) {
        free(fifo->buffer);
    }

    fifo->buffer  = NULL;
    fifo->size    = 0;
    fifo->in      = 0;
    fifo->out     = 0;
    fifo->dynamic = false;
}

void kfifo_reset(kfifo_t *fifo) {
    if (!fifo) return;

    fifo->in  = 0;
    fifo->out = 0;
}

uint32_t kfifo_size(const kfifo_t *fifo) {
    return fifo ? fifo->size : 0;
}

uint32_t kfifo_len(const kfifo_t *fifo) {
    return fifo ? (fifo->in - fifo->out) : 0;
}

uint32_t kfifo_avail(const kfifo_t *fifo) {
    return fifo ? (fifo->size - kfifo_len(fifo)) : 0;
}

bool kfifo_is_empty(const kfifo_t *fifo) {
    return !fifo || (fifo->in == fifo->out);
}

bool kfifo_is_full(const kfifo_t *fifo) {
    return fifo && (kfifo_len(fifo) == fifo->size);
}

/* 单字节写 */
bool kfifo_put(kfifo_t *fifo,uint8_t data) {
    if (!fifo || !fifo->buffer || kfifo_is_full(fifo)) {
        return false;
    }

    uint32_t in                         = fifo->in;
    fifo->buffer[in & (fifo->size - 1)] = data;
    fifo->in                            = in + 1;

    return true;
}

/* 单字节读 */
bool kfifo_get(kfifo_t *fifo,uint8_t *data) {
    if (!fifo || !fifo->buffer || !data || kfifo_is_empty(fifo)) {
        return false;
    }

    uint32_t out = fifo->out;
    *data        = fifo->buffer[out & (fifo->size - 1)];
    fifo->out    = out + 1;

    return true;
}

/* 批量写入（DMA / ISR） */
uint32_t kfifo_in(kfifo_t *fifo,const uint8_t *buf,uint32_t len) {
    if (!fifo || !fifo->buffer || !buf || len == 0) {
        return 0;
    }

    uint32_t size = fifo->size;
    uint32_t mask = size - 1;
    uint32_t in   = fifo->in;

    len = (len > kfifo_avail(fifo)) ? kfifo_avail(fifo) : len;

    uint32_t l = len;
    if (l > size - (in & mask)) {
        l = size - (in & mask);
    }

    memcpy(fifo->buffer + (in & mask), buf, l);
    memcpy(fifo->buffer, buf + l, len - l);
    fifo->in = in + len;

    return len;
}

/* 批量读出（任务） */
uint32_t kfifo_out(kfifo_t *fifo,uint8_t *buf,uint32_t len) {
    if (!fifo || !fifo->buffer || !buf || len == 0) {
        return 0;
    }

    uint32_t size = fifo->size;
    uint32_t mask = size - 1;
    uint32_t out  = fifo->out;

    len = (len > kfifo_len(fifo)) ? kfifo_len(fifo) : len;

    uint32_t l = len;
    if (l > size - (out & mask)) {
        l = size - (out & mask);
    }

    memcpy(buf, fifo->buffer + (out & mask), l);
    memcpy(buf + l, fifo->buffer, len - l);
    fifo->out = out + len;

    return len;
}

uint32_t kfifo_fetch(kfifo_t *fifo,uint8_t *buf,int len) {
    if (!fifo || !fifo->buffer || !buf || len == 0) {
        return 0;
    }

    uint32_t size = fifo->size;
    uint32_t mask = size - 1;
    uint32_t out  = fifo->out;

    len = (len > kfifo_len(fifo)) ? kfifo_len(fifo) : len;

    uint32_t l = len;
    if (l > size - (out & mask)) {
        l = size - (out & mask);
    }
    memcpy(buf, fifo->buffer + (out & mask), l);
    memcpy(buf + l, fifo->buffer, len - l);
    return len;
}

uint32_t kfifo_skip(kfifo_t *fifo,uint32_t len) {
    len       = (len > kfifo_len(fifo)) ? kfifo_len(fifo) : len;
    fifo->out = fifo->out + len;
    return len;
}
