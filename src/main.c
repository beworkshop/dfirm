#include "pico/stdlib.h"
#include "led.h"
#include "protocols/uart.h"
#include "protocols/iic.h"
#include "core.h"






int main() {
    // Init protocols
    stdio_init_all();
    d_led_init();
    d_uart_init(false);

    // Setting up UART at baud
    d_uart_set_nmea();
    // Setting up i2c
    d_init_i2c();

    // Starting sequence
    d_led_blink_loop(100, 10);

    d_start_dualcore();

    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}