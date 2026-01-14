//
// Created by AZX on 26-1-7.
//

#include "SerialCircleBufferDMATask.h"

#include <stdlib.h>

// FreeRTOS.h 必须在其他 FreeRTOS 头文件之前包含
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "cmsis_os2.h"
#include "gpio.h"
#include "usart.h"
#include "App/Func/Public/kfifo.h"
#include "App/Task/LEDTask.h"
#include "App/Types/LEDTypes.h"

#define RX_BUFFER_SIZE 256
static uint8_t           s_rxBuffer[RX_BUFFER_SIZE];
static kfifo_t *         s_kfifo;
static SemaphoreHandle_t s_xDataReadySemaphore;  // 一对一信号量

void SerialCircleBufferDMATask_QueueInit(void) {
    s_kfifo = (kfifo_t *) malloc(sizeof(kfifo_t));
    kfifo_alloc(s_kfifo, KFIFO_SIZE_1024);

    // 创建二值信号量
    s_xDataReadySemaphore = xSemaphoreCreateBinary();

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rxBuffer, RX_BUFFER_SIZE);
}

// 注释掉以避免与 SerialTask.c 中的 HAL_UART_RxCpltCallback 冲突
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size) {
    if (huart->Instance == USART2) {
        // 处理接收到的数据
        kfifo_in(s_kfifo, s_rxBuffer, Size);

        // 在中断中使用 FromISR 版本
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(s_xDataReadySemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        // 重新启动接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rxBuffer, RX_BUFFER_SIZE);
    }
}

// 解析状态机
typedef enum {
    PARSE_STATE_HEADER,   // 等待帧头 0xAA
    PARSE_STATE_LENGTH,   // 等待长度字节
    PARSE_STATE_DATA      // 接收数据
} ParseState_t;

#define MESSAGE_MAX_SIZE 256
#define HEADER_BYTE      0xAA
#define MIN_MESSAGE_LEN  4
static uint8_t  s_message[MESSAGE_MAX_SIZE];
static uint16_t s_dataLen = 0;

// 处理完整消息的回调（用户实现）
static void processCompleteMessage(uint8_t *data,uint16_t len) {
    LEDMessage *msg = pvPortMalloc(sizeof(LEDMessage));
    msg->pin        = data[2] == 0 ? GPIO_PIN_0 : GPIO_PIN_1;
    msg->state      = data[3];
    osMessageQueuePut(LEDQueueHandle, &msg, 0, 0);
}

bool isValidHeader(uint8_t data) {
    return data == HEADER_BYTE;
}

void analyzeCircleBuffer(void) {
    for (;;) {
        if (kfifo_len(s_kfifo) < MIN_MESSAGE_LEN) break;
        //读取最短长度的数据
        kfifo_fetch(s_kfifo, s_message, MIN_MESSAGE_LEN);
        //帧头异常
        if (!isValidHeader(s_message[0])) {
            kfifo_skip(s_kfifo, 1);
            continue;
        }
        s_dataLen = s_message[1];
        //数据长度异常
        if (s_dataLen > MESSAGE_MAX_SIZE || s_dataLen < MIN_MESSAGE_LEN) {
            kfifo_skip(s_kfifo, 1);
            continue;
        }
        //数据长度不足
        if (kfifo_len(s_kfifo) < s_dataLen + 2) break;
        kfifo_out(s_kfifo, s_message, s_dataLen + 2);
        //处理数据
        processCompleteMessage(s_message, s_dataLen + 2);
    }
}

void StartSerialCircleBufferDMATask(void *argument) {
    for (;;) {
        if (xSemaphoreTake(s_xDataReadySemaphore, portMAX_DELAY) == pdTRUE) {
            analyzeCircleBuffer();
        } else {
            osDelay(10);
        }
    }
}
