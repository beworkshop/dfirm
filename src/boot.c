#include "pico/bootrom.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

// Using GPIO 26 (A0) as the boot pin
#define BOOT_PIN 26

void check_boot() {
    gpio_init(BOOT_PIN);
    gpio_set_dir(BOOT_PIN, GPIO_IN);
    gpio_pull_up(BOOT_PIN);

    sleep_ms(10);
    if (!gpio_get(BOOT_PIN)) {
        uint32_t ints = save_and_disable_interrupts();
        rom_reset_usb_boot_extra(-1, 0, false);
        restore_interrupts(ints);
    }
}