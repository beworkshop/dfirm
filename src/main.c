#include "pico/stdlib.h"
#include <stdio.h>
#include "include/led.h"
#include "include/uart.h"
#include "include/log.h"
#include "include/gps.h"
#include "include/utils.h"

int main() {
    // Inits
    stdio_init_all();
    d_led_init();
    d_uart_init();

    // Starting sequence
    d_led_blink_loop(100, 10);

    //d_uart_fast_baude();
    d_uart_set_nmea();

    // Main loop
    while (1) {
        d_led_activate(10);
        //log_message(LOG_LEVEL_DEBUG, "Led blinked");
        for (int i = 0; i < 10; i++){
            char* msg = d_uart_read();
            if (msg[0] != '\0'){
                print_literal(msg);
                d_parse_line(msg);
            }
            sleep_ms(100);
        }

        d_print_gps_compact();



        d_led_deactivate(10);
    }


    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}