/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ring_buffer.h
  * @brief   Ring buffer implementation for UART data
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* 环形队列大小定义 */
#define RING_BUFFER_SIZE  1024  // 可以根据需要调整大小

/* 环形队列结构体 */
typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];  // 数据缓冲区
    volatile uint16_t head;             // 写指针（头指针）
    volatile uint16_t tail;             // 读指针（尾指针）
    volatile uint16_t count;            // 当前数据量
} RingBuffer_t;

/* 函数声明 */
void RingBuffer_Init(RingBuffer_t *rb);
bool RingBuffer_IsEmpty(RingBuffer_t *rb);
bool RingBuffer_IsFull(RingBuffer_t *rb);
uint16_t RingBuffer_GetCount(RingBuffer_t *rb);
uint16_t RingBuffer_GetFreeSpace(RingBuffer_t *rb);
bool RingBuffer_Write(RingBuffer_t *rb, uint8_t data);
bool RingBuffer_WriteBuffer(RingBuffer_t *rb, const uint8_t *data, uint16_t len);
bool RingBuffer_Read(RingBuffer_t *rb, uint8_t *data);
uint16_t RingBuffer_ReadBuffer(RingBuffer_t *rb, uint8_t *data, uint16_t len);
void RingBuffer_Clear(RingBuffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUFFER_H */

