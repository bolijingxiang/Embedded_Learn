//
// Created by AZX on 26-1-5.
//

#include <stdbool.h>
#include "KeyTask.h"
#include "cmsis_os2.h"
#include "gpio.h"
#include "App/Types/LEDTypes.h"
#include "FreeRTOS.h"
#include "LEDTask.h"
#include "usart.h"

typedef struct {
  uint16_t keyPin;
  uint64_t last;
  uint16_t ledPin;
  bool pressed;
  bool targetLedState;
} GPIOState;

GPIOState gpioStateTable[2] = {
	{GPIO_PIN_2, 0, GPIO_PIN_0, false, true},
	{GPIO_PIN_3, 0, GPIO_PIN_1, false, true}};

#define GPIOSTATE_TABLE_SIZE sizeof(gpioStateTable) / sizeof(gpioStateTable[0])

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//  GPIOState curGPIOState;
//  for (int i = 0; i < sizeof(gpioStateTable) / sizeof(gpioStateTable[0]); i++) {
//	if (gpioStateTable[i].keyPin == GPIO_Pin) {
//	  if (HAL_GetTick() - gpioStateTable[i].last < 20) {
//		return;
//	  }
//	  gpioStateTable[i].last = HAL_GetTick();
//	  gpioStateTable[i].led_state = !gpioStateTable[i].led_state;
//	  curGPIOState = gpioStateTable[i];
//	  break;
//	}
//  }
//  LEDMessage *msg = pvPortMalloc(sizeof(LEDMessage));
//  msg->state = curGPIOState.led_state;
//  msg->pin = curGPIOState.ledPin;
//  osMessageQueuePut(LEDQueueHandle, &msg, 0, 0);

//  int8_t pinState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
//  HAL_UART_Transmit_IT(&huart2, (uint8_t *)&pinState, 1);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, !(GPIO_PinState)pinState);
}

/**
 *
 * @param argument
 */
void pinStateChangeSendMessage(void) {
  for (unsigned int i = 0; i < GPIOSTATE_TABLE_SIZE; i++) {
	if (HAL_GetTick() - gpioStateTable[i].last < 20) {
	  continue;
	}
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOH, gpioStateTable[i].keyPin);
	if (pinState == GPIO_PIN_RESET && !gpioStateTable[i].pressed) {
	  GPIO_PinState targetPinState = HAL_GPIO_ReadPin(GPIOB, gpioStateTable[i].ledPin);
	  LEDMessage *msg = pvPortMalloc(sizeof(LEDMessage));
	  msg->state = !(LED_State)targetPinState;
	  msg->pin = gpioStateTable[i].ledPin;
	  osMessageQueuePut(LEDQueueHandle, &msg, 0, 0);
	  gpioStateTable[i].targetLedState = targetPinState;
	  gpioStateTable[i].pressed = true;
	} else if (pinState == GPIO_PIN_SET) {
	  gpioStateTable[i].pressed = false;
	}
	gpioStateTable[i].last = HAL_GetTick();
  }
}

void StartKeyTask(void *argument) {
  for (;;) {
	pinStateChangeSendMessage();
	osDelay(10);
  }
}
