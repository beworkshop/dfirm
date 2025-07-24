#ifndef PICO_I2C_COMPAT_H
#define PICO_I2C_COMPAT_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdint.h>

// I2C Configuration
#define I2C_PORT i2c0
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_FREQ 400000

// Initialize I2C for pico
void pico_i2c_init(void);

// I2C read/write functions to replace Arduino Wire
int8_t pico_i2c_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t* data);
bool pico_i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
bool pico_i2c_write_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t length, uint8_t* data);

#endif