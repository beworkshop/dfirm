#include "pico/stdlib.h"
#include <stdio.h>
#include "include/led.h"


int main() {
    stdio_init_all();
    led_init();

    while (1) {
        led_blink(500);
    }
}