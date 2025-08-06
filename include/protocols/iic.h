#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"


void read_registers(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
void write_reg(uint8_t addr, uint8_t reg, uint8_t value);
void d_init_i2c(void);