#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "log.h"
#include "modules/gps.h"
#include "pico/critical_section.h"
#include "led.h"


static gps_time_t gps_time = {0, 0, 0};
static gps_position_t gps_position = {0};
static gps_satellite_t gps_satellite = {0, false};

static critical_section_t lock;
static volatile bool gps_update = true;


int64_t stop_gps_loop(alarm_id_t id, void *user_data) {
    gps_update = false;
    return 0;
}
void core1_init() {

    // Core 1 main loop
    while (1) {
        absolute_time_t iteration_start = get_absolute_time();
        absolute_time_t iteration_end = delayed_by_ms(iteration_start, 100);

        gps_update = true;
        alarm_id_t id = add_alarm_at(iteration_end, stop_gps_loop, NULL, false);
        critical_section_enter_blocking(&lock);
        d_update_gps(&gps_time, &gps_position, &gps_satellite);
        critical_section_exit(&lock);
        while (gps_update) {
            tight_loop_contents();
        }
        LOG_DEBUG("Looped 100ms for gps update");
    }
}

void d_start_dualcore() {
    LOG_INFO("Starting dualcore");

    critical_section_init(&lock);

    multicore_launch_core1(core1_init);





    // Core 0 main loop

    while (1) {
        d_led_activate(10);
    }

}