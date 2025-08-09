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
static accel_data_t acc = {0};

void core1_init() {
    // Core 1 main loop
    while (1) {
        d_update_gps(&gps_time, &gps_position, &gps_satellite, true);
        printf("\033[s\033[16;1H\033[K");
        d_print_gps_compact(&gps_time, &gps_position, &gps_satellite);
        printf("\033[u");
        fflush(stdout);
        
        sleep_ms(50);
    }
}

void d_start_dualcore() {
    LOG_INFO("Starting dualcore");
    
    // Create display area with 3 empty lines
    printf("\n\n\n");
    
    multicore_launch_core1(core1_init);

    // Starting sequence
    d_led_blink_loop(100, 10);
    // Core 0 main loop
    while (1) {
        d_led_activate(10);
        d_update_acc(&acc);
        
        printf("\033[s\033[17;1H\033[K");
        d_print_acc_compact(&acc);
        printf("\033[u");
        fflush(stdout);
        
        d_led_deactivate(10);
        sleep_ms(10);
    }
}