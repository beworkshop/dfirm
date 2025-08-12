#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "log.h"
#include "modules/gps.h"
#include "pico/critical_section.h"
#include "led.h"
#include "modules/accelerometer.h"

static gps_time_t gps_time = {0, 0, 0};
static gps_position_t gps_position = {0};
static gps_satellite_t gps_satellite = {0, false};
static accel_data_t acc = {0};

void core1_init() {
    // Core 1 main loop
    while (1) {
        d_update_gps(&gps_time, &gps_position, &gps_satellite, true);
        d_print_gps_compact(&gps_time, &gps_position, &gps_satellite);
        sleep_ms(10);
    }
}

void d_start_dualcore() {
    LOG_INFO("Starting dualcore");
    
    // Starting second core
    multicore_launch_core1(core1_init);

    // Starting sequence
    d_led_blink_loop(100, 10);
    // Core 0 main loop
    while (1) {
        d_led_activate(10);
        d_update_acc(&acc);
        d_print_acc_compact(&acc);        
        d_led_deactivate(10);
        sleep_ms(10);
    }
}