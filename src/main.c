#include "pico/stdlib.h"
#include <stdio.h>
#include "include/led.h"
#include "include/uart.h"
#include "include/log.h"


int main() {
    // Inits
    stdio_init_all();
    d_led_init();
    d_uart_init();

    // Starting sequence
    d_led_blink_loop(100, 10);

    // Main loop
    while (1) {
        d_led_activate(10);
        //log_message(LOG_LEVEL_DEBUG, "Led blinked");

        char* msg = d_uart_read();

        printf("%s\n", msg);
        

        d_led_deactivate(10);
    }


    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}