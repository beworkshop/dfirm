#pragma once

void d_led_init(void);
void d_led_activate(const int sleep_time);
void d_led_deactivate(const int sleep_time);
void d_led_blink(const int sleep_time);
void d_led_blink_loop(const int sleep_time, const int num);
void d_led_deinit();