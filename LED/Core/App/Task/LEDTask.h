//
// Created by AZX on 26-1-5.
//

#ifndef LED_CORE_APP_TASK_LEDTASK_H_
#define LED_CORE_APP_TASK_LEDTASK_H_

#include "cmsis_os2.h"

extern osMessageQueueId_t LEDQueueHandle;

void LEDTask_QueueInit(void);

void StartLedTask(void *argument);
#endif //LED_CORE_APP_TASK_LEDTASK_H_
