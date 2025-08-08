#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

uint8_t read_register(uint8_t addr, uint8_t reg);
void read_registers(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
void write_reg(uint8_t addr, uint8_t reg, uint8_t value);
void d_i2c_init(void);
void d_i2c_deinit(void);