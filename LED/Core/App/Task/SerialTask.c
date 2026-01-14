//
// Created by AZX on 26-1-6.
//

#include <stdio.h>
#include "SerialTask.h"
#include "cmsis_os2.h"
#include "gpio.h"
#include "usart.h"
#include "App/Types/LEDTypes.h"
#include "LEDTask.h"
#include "FreeRTOS.h"

osMessageQueueId_t SerialQueueHandle;

// 接收缓冲区
#define RX_BUFFER_SIZE 1
static uint8_t rxBuffer;

void StartAnalyticSerialTask(void *argument) {
	// 启动串口接收中断
	HAL_UART_Receive_IT(&huart2, &rxBuffer, RX_BUFFER_SIZE);

	uint8_t txBuffer    = 0;
	uint8_t curIndex    = 0;
	uint8_t dataLen     = 0;
	uint8_t message[30] = {0};
	for (;;) {
		osMessageQueueGet(SerialQueueHandle, &txBuffer, 0, osWaitForever);
		if (curIndex == 0) {
			if (txBuffer == 0xAA) {
				message[curIndex++] = txBuffer;
			}
		} else if (curIndex == 1) {
			dataLen             = txBuffer;
			message[curIndex++] = txBuffer;
			if (dataLen > sizeof(message)) {
				curIndex = 0;
				continue;
			}
		} else if (curIndex < dataLen + 2) {
			message[curIndex++] = txBuffer;
			if (curIndex == dataLen + 2) {
				curIndex        = 0;
				LEDMessage *msg = pvPortMalloc(sizeof(LEDMessage));
				msg->pin        = message[2] == 0 ? GPIO_PIN_0 : GPIO_PIN_1;
				msg->state      = message[3];
				osMessageQueuePut(LEDQueueHandle, &msg, 0, 0);
			}
		}
	}
}

void SerialTask_QueueInit(void) {
	SerialQueueHandle = osMessageQueueNew(16, 1, NULL);
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size) {
// 	if (huart->Instance == USART2) {
// 		osMessageQueuePut(SerialQueueHandle, &rxBuffer, 0, 0);
// 		// 重新启动接收
// 		HAL_UARTEx_ReceiveToIdle_IT(&huart2, &rxBuffer, RX_BUFFER_SIZE);
// 	}
// }

// 接收完成回调（当接收缓冲区满时触发）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		osMessageQueuePut(SerialQueueHandle, &rxBuffer, 0, 0);
		// 重新启动接收
		HAL_UART_Receive_IT(&huart2, &rxBuffer, RX_BUFFER_SIZE);
	}
}

// 处理 UART 错误，防止错误后无法继续接收
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		// 清除错误标志并重新启动接收
		__HAL_UART_CLEAR_OREFLAG(huart);
		__HAL_UART_CLEAR_NEFLAG(huart);
		__HAL_UART_CLEAR_FEFLAG(huart);
		HAL_UART_Receive_IT(&huart2, &rxBuffer, RX_BUFFER_SIZE);
	}
}

