#include "pico/stdlib.h"
#include "led.h"
#include "protocols/uart.h"
#include "core.h"






int main() {
    // Init protocols
    stdio_init_all();
    d_led_init();
    d_uart_init(false);

    // Starting sequence
    d_led_blink_loop(100, 10);

    // Setting up UART at baud
    d_uart_set_nmea();
    sleep_ms(200);

    d_start_dualcore();

    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}