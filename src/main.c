#include "pico/stdlib.h"
#include "led.h"
#include "protocols/uart.h"
#include "protocols/iic.h"
#include "modules/accelerometer.h"
#include "modules/gps_parser.h"
#include "modules/gps_com.h"
#include "log.h"
#include "boot.h"
#include "core.h"






int main() {
    check_boot();
    // Init protocols
    stdio_init_all();
    d_led_init();
    d_uart_init(false);
    d_i2c_init();

    // Starting the Air530Z
    d_start_gps();

    // Starting the ICM20600
    d_start_acc();

    // Starting multicore
    d_start_dualcore();

    // Deinits
    d_i2c_deinit();
    d_uart_deinit();
    d_led_deinit();
    stdio_deinit_all();
}