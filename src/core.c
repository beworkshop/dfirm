#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "log.h"
#include "modules/gps_parser.h"
#include "pico/critical_section.h"
#include "led.h"
#include "modules/accelerometer.h"


static gps_time_t gps_time = {0, 0, 0};
static gps_position_t gps_position = {0};
static gps_satellite_t gps_satellite = {0, false};


void core1_init() {

    // Core 1 main loop
    while (1) {

    }
}

void d_start_dualcore() {
    LOG_INFO("Starting dualcore");


    //multicore_launch_core1(core1_init);
    //d_start_acc();
    accel_data_t acc = {0};



    // Core 0 main loop

    while (1) {
        d_led_activate(10);
        d_update_gps(&gps_time, &gps_position, &gps_satellite, false);
        LOG_INFO("printing");
        d_print_gps_compact(&gps_time, &gps_position, &gps_satellite);
        LOG_INFO("loop end");
        //d_update_acc(&acc);
        //LOG_INFO("Accelerometer (g): X=%.2f  Y=%.2f  Z=%.2f", acc.ax, acc.ay, acc.az);
        //LOG_INFO("Gyroscope (°/s):   X=%.2f  Y=%.2f  Z=%.2f", acc.gx, acc.gy, acc.gz);
        d_led_deactivate(10);
        sleep_ms(300);
        
    }

}