#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <stdio.h>
#include "led.h"
#include "protocols/uart.h"
#include "log.h"
#include "modules/gps.h"







int main() {
    // Inits
    stdio_init_all();
    d_led_init();
    d_uart_init(false);

    // Starting sequence
    d_led_blink_loop(100, 10);

    // Setting up UART at baud
    d_uart_fast_baude();
    d_uart_set_nmea();
    d_uart_hot_start();
    sleep_ms(200);



    // Main loop
    while (1) {
        d_led_activate(10);
        //log_message(LOG_LEVEL_DEBUG, "Led blinked");
        for (int i = 0; i < 10; i++){
            char* msg = d_uart_read();
            if (msg[0] != '\0'){
                printf(msg);
                d_parse_line(msg);
            }
            sleep_ms(200);
        }

        d_print_gps_compact();




    }


    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}