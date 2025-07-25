#ifndef LED_H
#define LED_H

#include "pico/stdlib.h"

// Function declarations
int led_init(void);
int led_blink(uint32_t sleep_time);

#endif // LED_H