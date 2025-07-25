#include "pico/stdlib.h"



void d_led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}
void d_led_activate(const int sleep_time) {
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(sleep_time);
}
void d_led_deactivate(const int sleep_time) {
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(sleep_time);
}

void d_led_blink(const int sleep_time) {
    d_led_activate(sleep_time);
    d_led_deactivate(sleep_time);
}

void d_led_blink_loop(const int sleep_time, const int num) {
    for (int i = 0; i < num; i++){
        d_led_blink(sleep_time);
    }
}

void d_led_deinit() {
    gpio_deinit(PICO_DEFAULT_LED_PIN);
}
