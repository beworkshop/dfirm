#include "pico_i2c_compat.h"

void pico_i2c_init(void) {
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    sleep_ms(100);
}

int8_t pico_i2c_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t* data) {
    int result = i2c_write_blocking(I2C_PORT, dev_addr, &reg_addr, 1, true);
    if (result < 0) return -1;
    
    result = i2c_read_blocking(I2C_PORT, dev_addr, data, length, false);
    return (result == length) ? length : -1;
}

bool pico_i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    uint8_t buf[2] = {reg_addr, data};
    int result = i2c_write_blocking(I2C_PORT, dev_addr, buf, 2, false);
    return (result == 2);
}

bool pico_i2c_write_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t* data) {
    uint8_t* buf = malloc(length + 1);
    if (!buf) return false;
    
    buf[0] = reg_addr;
    memcpy(&buf[1], data, length);
    
    int result = i2c_write_blocking(I2C_PORT, dev_addr, buf, length + 1, false);
    free(buf);
    return (result == (length + 1));
}