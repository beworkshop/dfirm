#include "pico/stdlib.h"



int led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}


int led_blink(const int sleep_time) {

    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(sleep_time);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(sleep_time);
    
    return 0;
}

