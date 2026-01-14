//
// Created by AZX on 26-1-5.
//

#include "LEDTask.h"
#include "cmsis_os2.h"
#include "App/Types/LEDTypes.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"

// 定义 LEDQueueHandle 变量
osMessageQueueId_t LEDQueueHandle;

static const osMessageQueueAttr_t LEDQueue_attributes = {
	.name = "LEDQueue"
};

void StartLedTask(void *argument) {
  for (;;) {
	LEDMessage *msg;
	osMessageQueueGet(LEDQueueHandle, &msg, 0, osWaitForever);
	HAL_GPIO_WritePin(GPIOB, msg->pin, (GPIO_PinState)msg->state);
	vPortFree(msg);
  }
}
void LEDTask_QueueInit(void) {
  LEDQueueHandle = osMessageQueueNew(16, sizeof(LEDMessage *), &LEDQueue_attributes);
}
