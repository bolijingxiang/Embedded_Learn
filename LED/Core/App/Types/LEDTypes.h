//
// Created by AZX on 26-1-5.
//

#ifndef LED_LEDTYPES_H
#define LED_LEDTYPES_H
#include <stdint.h>

typedef enum {
  LED_RED = 0,
  LED_Green
} LED_Color;

typedef enum {
  LED_ON = 0,
  LED_OFF,
} LED_State;

typedef struct {
  LED_Color color;
  LED_State state;
  uint16_t pin;
} LEDMessage;

#endif //LED_LEDTYPES_H
