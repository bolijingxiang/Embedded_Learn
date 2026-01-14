/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ring_buffer.c
  * @brief   Ring buffer implementation for UART data
  ******************************************************************************
  */
/* USER CODE END Header */

#include "ring_buffer.h"
#include <string.h>

/**
  * @brief  初始化环形队列
  * @param  rb: 环形队列指针
  * @retval None
  */
void RingBuffer_Init(RingBuffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    memset(rb->buffer, 0, RING_BUFFER_SIZE);
}

/**
  * @brief  检查环形队列是否为空
  * @param  rb: 环形队列指针
  * @retval true: 空, false: 非空
  */
bool RingBuffer_IsEmpty(RingBuffer_t *rb)
{
    return (rb->count == 0);
}

/**
  * @brief  检查环形队列是否已满
  * @param  rb: 环形队列指针
  * @retval true: 满, false: 未满
  */
bool RingBuffer_IsFull(RingBuffer_t *rb)
{
    return (rb->count >= RING_BUFFER_SIZE);
}

/**
  * @brief  获取环形队列中的数据量
  * @param  rb: 环形队列指针
  * @retval 数据量
  */
uint16_t RingBuffer_GetCount(RingBuffer_t *rb)
{
    return rb->count;
}

/**
  * @brief  获取环形队列剩余空间
  * @param  rb: 环形队列指针
  * @retval 剩余空间大小
  */
uint16_t RingBuffer_GetFreeSpace(RingBuffer_t *rb)
{
    return (RING_BUFFER_SIZE - rb->count);
}

/**
  * @brief  向环形队列写入单个字节
  * @param  rb: 环形队列指针
  * @param  data: 要写入的数据
  * @retval true: 成功, false: 失败（队列满）
  */
bool RingBuffer_Write(RingBuffer_t *rb, uint8_t data)
{
    if (RingBuffer_IsFull(rb)) {
        return false;  // 队列满，写入失败
    }
    
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->count++;
    
    return true;
}

/**
  * @brief  向环形队列写入多个字节
  * @param  rb: 环形队列指针
  * @param  data: 要写入的数据指针
  * @param  len: 数据长度
  * @retval true: 成功, false: 失败（空间不足）
  */
bool RingBuffer_WriteBuffer(RingBuffer_t *rb, const uint8_t *data, uint16_t len)
{
    if (len == 0) {
        return true;
    }
    
    if (RingBuffer_GetFreeSpace(rb) < len) {
        return false;  // 空间不足
    }
    
    for (uint16_t i = 0; i < len; i++) {
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
        rb->count++;
    }
    
    return true;
}

/**
  * @brief  从环形队列读取单个字节
  * @param  rb: 环形队列指针
  * @param  data: 读取数据的存储指针
  * @retval true: 成功, false: 失败（队列空）
  */
bool RingBuffer_Read(RingBuffer_t *rb, uint8_t *data)
{
    if (RingBuffer_IsEmpty(rb)) {
        return false;  // 队列空，读取失败
    }
    
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    rb->count--;
    
    return true;
}

/**
  * @brief  从环形队列读取多个字节
  * @param  rb: 环形队列指针
  * @param  data: 读取数据的存储指针
  * @param  len: 要读取的长度
  * @retval 实际读取的字节数
  */
uint16_t RingBuffer_ReadBuffer(RingBuffer_t *rb, uint8_t *data, uint16_t len)
{
    uint16_t read_count = 0;
    
    while (read_count < len && !RingBuffer_IsEmpty(rb)) {
        data[read_count] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
        rb->count--;
        read_count++;
    }
    
    return read_count;
}

/**
  * @brief  清空环形队列
  * @param  rb: 环形队列指针
  * @retval None
  */
void RingBuffer_Clear(RingBuffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

