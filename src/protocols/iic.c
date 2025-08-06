// Current Date and Time (UTC - YYYY-MM-DD HH:MM:SS formatted): 2025-08-06 15:18:03
// Current User's Login: emecdelam

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define I2C_SDA 6
#define I2C_SCL 7
#define I2C_FREQ 1000000


void read_registers(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len) {
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, buf, len, false);
}


void write_reg(uint8_t addr, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}


void d_init_i2c(void) {
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}