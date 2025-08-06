#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "log.h"
#include "modules/gps.h"
#include "pico/critical_section.h"
#include "led.h"
#include "modules/accelerometer.h"

// Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-08-06 15:58:46
// Current User's Login: emecdelam

static gps_time_t gps_time = {0, 0, 0};
static gps_position_t gps_position = {0};
static gps_satellite_t gps_satellite = {0, false};

// ✅ FIX: Separate critical sections
static critical_section_t gps_lock;      // For GPS data only
static critical_section_t stdio_lock;    // For printf/LOG_INFO

static volatile bool gps_update = true;

int64_t stop_gps_loop(alarm_id_t id, void *user_data) {
    gps_update = false;
    return 0;
}

void core1_init() {
    printf("Core 1: GPS loop started\n");  // Test print
    
    uint32_t iteration = 0;
    
    while (1) {
        iteration++;
        
        absolute_time_t iteration_start = get_absolute_time();
        absolute_time_t iteration_end = delayed_by_ms(iteration_start, 100);

        gps_update = true;
        alarm_id_t id = add_alarm_at(iteration_end, stop_gps_loop, NULL, false);
        
        // ✅ FIX: Only lock GPS data, not stdio
        critical_section_enter_blocking(&gps_lock);
        d_update_gps(&gps_time, &gps_position, &gps_satellite);
        critical_section_exit(&gps_lock);
        
        // ✅ FIX: Print occasionally to verify Core 1 is working
        if (iteration % 10 == 0) {
            critical_section_enter_blocking(&stdio_lock);
            printf("Core 1: GPS iteration %lu\n", iteration);
            critical_section_exit(&stdio_lock);
        }
        
        while (gps_update) {
            tight_loop_contents();
        }
    }
}

void d_start_dualcore() {
    // ✅ FIX: Initialize stdio first
    stdio_init_all();
    sleep_ms(2000);  // Wait for USB connection
    
    printf("Main: Starting dualcore system\n");
    LOG_INFO("Starting dualcore");

    // ✅ FIX: Initialize both critical sections
    critical_section_init(&gps_lock);
    critical_section_init(&stdio_lock);

    multicore_launch_core1(core1_init);
    
    // ✅ FIX: Give Core 1 time to start
    sleep_ms(500);

    accel_data_t acc = {0};

    printf("Core 0: Starting main loop\n");

    // Core 0 main loop
    uint32_t loop_count = 0;
    
    while (1) {
        loop_count++;
        
        d_led_activate(10);
        d_update_acc(&acc);
        
        // ✅ FIX: Protect stdio access
        critical_section_enter_blocking(&stdio_lock);
        printf("Loop %lu: Accelerometer (g): X=%.2f  Y=%.2f  Z=%.2f\n", 
                 loop_count, acc.ax, acc.ay, acc.az);
        printf("Loop %lu: Gyroscope (°/s):   X=%.2f  Y=%.2f  Z=%.2f\n", 
                 loop_count, acc.gx, acc.gy, acc.gz);
        critical_section_exit(&stdio_lock);
        
        sleep_ms(1000);  // Slower than Core 1 for easier debugging
    }
}