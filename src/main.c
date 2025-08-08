#include "pico/stdlib.h"
#include "led.h"
#include "protocols/uart.h"
#include "protocols/iic.h"
#include "modules/accelerometer.h"
#include "modules/gps_parser.h"
#include "log.h"
//#include "core.h"






int main() {
    // Init protocols
    stdio_init_all();
    d_led_init();
    d_uart_init(false);

    // Setting up UART at baud
    d_uart_set_nmea();
    // Setting up i2c
    d_init_i2c();


    // Starting the ICM20600
    d_start_acc();


    static accel_data_t acc = {0};
    static gps_time_t gps_time = {0, 0, 0};
    static gps_position_t gps_position = {0};
    static gps_satellite_t gps_satellite = {0, false};

    // Starting sequence
    d_led_blink_loop(100, 10);

    //d_start_dualcore();
    while (1) {
        d_led_activate(10);
        d_update_gps(&gps_time, &gps_position, &gps_satellite, true);
        d_update_acc(&acc);
        d_print_gps_compact(&gps_time, &gps_position, &gps_satellite);
        //d_print_acc_compact(&acc);
        d_led_deactivate(10);
        sleep_ms(300);
    
    }


    // Deinits
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}