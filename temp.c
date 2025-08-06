#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT        i2c1
#define I2C_SDA         6
#define I2C_SCL         7
#define I2C_FREQ        1000000

// Device addresses
#define ICM_ADDR        0x69       // ICM20600 address
#define MAG_ADDR        0x0C       // AK09918 magnetometer address

// ICM20600 Registers
#define ICM_WHO_AM_I    0x75
#define ICM_PWR_MGMT_1  0x6B
#define ICM_CONFIG      0x1A
#define ICM_GYRO_CONFIG 0x1B
#define ICM_ACCEL_CONFIG 0x1C
#define ICM_ACCEL_XOUT_H 0x3B
#define ICM_GYRO_XOUT_H 0x43

// AK09918 Registers
#define MAG_WHO_AM_I    0x01
#define MAG_CNTL2       0x31
#define MAG_CNTL3       0x32
#define MAG_ST1         0x10
#define MAG_HXL         0x11

// Read a single byte from a register
uint8_t read_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(i2c, addr, &reg, 1, true);  // true to keep control of bus
    i2c_read_blocking(i2c, addr, &data, 1, false);
    return data;
}

// Write a single byte to a register
void write_register(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(i2c, addr, buf, 2, false);
}

// Read multiple bytes starting from a register
void read_registers(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len) {
    i2c_write_blocking(i2c, addr, &reg, 1, true);  // true to keep control of bus
    i2c_read_blocking(i2c, addr, buf, len, false);
}

// Initialize ICM20600
bool init_icm20600(i2c_inst_t *i2c) {
    // Check device ID
    uint8_t id = read_register(i2c, ICM_ADDR, ICM_WHO_AM_I);
    printf("ICM20600 WHO_AM_I = 0x%02X\n", id);
    
    if (id != 0x11 && id != 0x10) {
        printf("Unexpected ICM20600 ID\n");
        return false;
    }
    
    // Reset the device
    write_register(i2c, ICM_ADDR, ICM_PWR_MGMT_1, 0x80);
    sleep_ms(100);  // Wait for reset to complete
    
    // Wake up the device
    write_register(i2c, ICM_ADDR, ICM_PWR_MGMT_1, 0x01);  // Clock source = PLL
    sleep_ms(10);
    
    // Configure accelerometer: ±2g range
    write_register(i2c, ICM_ADDR, ICM_ACCEL_CONFIG, 0x00);
    
    // Configure gyroscope: ±250 dps range
    write_register(i2c, ICM_ADDR, ICM_GYRO_CONFIG, 0x00);
    
    // Configure low-pass filter
    write_register(i2c, ICM_ADDR, ICM_CONFIG, 0x03);
    
    return true;
}

// Initialize AK09918 magnetometer
bool init_ak09918(i2c_inst_t *i2c) {
    // Check device ID
    uint8_t id = read_register(i2c, MAG_ADDR, MAG_WHO_AM_I);
    printf("AK09918 WHO_AM_I = 0x%02X\n", id);
    
    // Reset the magnetometer
    write_register(i2c, MAG_ADDR, MAG_CNTL3, 0x01);
    sleep_ms(100);
    
    // Set to continuous measurement mode 4 (100Hz)
    write_register(i2c, MAG_ADDR, MAG_CNTL2, 0x08);
    
    return true;
}

// Read accelerometer data
void read_accel(i2c_inst_t *i2c, float *ax, float *ay, float *az) {
    uint8_t buffer[6];
    read_registers(i2c, ICM_ADDR, ICM_ACCEL_XOUT_H, buffer, 6);
    
    int16_t raw_ax = (buffer[0] << 8) | buffer[1];
    int16_t raw_ay = (buffer[2] << 8) | buffer[3];
    int16_t raw_az = (buffer[4] << 8) | buffer[5];
    
    // Convert to g (±2g range)
    *ax = raw_ax / 16384.0f;
    *ay = raw_ay / 16384.0f;
    *az = raw_az / 16384.0f;
}

// Read gyroscope data
void read_gyro(i2c_inst_t *i2c, float *gx, float *gy, float *gz) {
    uint8_t buffer[6];
    read_registers(i2c, ICM_ADDR, ICM_GYRO_XOUT_H, buffer, 6);
    
    int16_t raw_gx = (buffer[0] << 8) | buffer[1];
    int16_t raw_gy = (buffer[2] << 8) | buffer[3];
    int16_t raw_gz = (buffer[4] << 8) | buffer[5];
    
    // Convert to degrees per second (±250dps range)
    *gx = raw_gx / 131.0f;
    *gy = raw_gy / 131.0f;
    *gz = raw_gz / 131.0f;
}

// Read magnetometer data
bool read_mag(i2c_inst_t *i2c, float *mx, float *my, float *mz) {
    // Check if data is ready
    uint8_t st1 = read_register(i2c, MAG_ADDR, MAG_ST1);
    if (!(st1 & 0x01)) {
        return false;  // No new data
    }
    
    uint8_t buffer[6];
    read_registers(i2c, MAG_ADDR, MAG_HXL, buffer, 6);
    
    // Magnetometer data is stored as little-endian
    int16_t raw_mx = (buffer[1] << 8) | buffer[0];
    int16_t raw_my = (buffer[3] << 8) | buffer[2];
    int16_t raw_mz = (buffer[5] << 8) | buffer[4];
    
    // Convert to microtesla
    *mx = raw_mx * 0.15f;  // AK09918: 0.15 μT/LSB
    *my = raw_my * 0.15f;
    *mz = raw_mz * 0.15f;
    
    return true;
}

int t() {
    // Initialize stdio
    stdio_init_all();
    sleep_ms(3000);  // Wait for serial connection
    
    printf("\nGrove 9DOF IMU Example\n");
    printf("=====================\n");
    
    // Initialize I2C
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    printf("I2C initialized: SDA=%d, SCL=%d @ %d Hz\n", I2C_SDA, I2C_SCL, I2C_FREQ);
    
    // Initialize sensors
    bool icm_ok = init_icm20600(I2C_PORT);
    bool mag_ok = init_ak09918(I2C_PORT);
    
    if (!icm_ok) {
        printf("Failed to initialize ICM20600!\n");
        return -1;
    }
    
    printf("\nReading sensor data...\n");
    
    // Main loop
    while (1) {
        // Read accelerometer
        float ax, ay, az;
        read_accel(I2C_PORT, &ax, &ay, &az);
        
        // Read gyroscope
        float gx, gy, gz;
        read_gyro(I2C_PORT, &gx, &gy, &gz);
        
        // Read magnetometer
        float mx = 0, my = 0, mz = 0;
        bool mag_data_ready = read_mag(I2C_PORT, &mx, &my, &mz);
        
        // Calculate acceleration magnitude
        float accel_mag = sqrtf(ax*ax + ay*ay + az*az);
        
        // Display data
        printf("\033[2J\033[H");  // Clear screen
        printf("Grove 9DOF IMU Data\n");
        printf("------------------\n");
        printf("Accelerometer (g): X=%.2f  Y=%.2f  Z=%.2f  |  Mag=%.2f\n", ax, ay, az, accel_mag);
        printf("Gyroscope (°/s):   X=%.2f  Y=%.2f  Z=%.2f\n", gx, gy, gz);
        
        if (mag_ok && mag_data_ready) {
            printf("Magnetometer (μT): X=%.2f  Y=%.2f  Z=%.2f\n", mx, my, mz);
        } else if (mag_ok) {
            printf("Magnetometer: waiting for data...\n");
        } else {
            printf("Magnetometer: not initialized\n");
        }
        
        sleep_ms(100);  // Update 10 times per second
    }
    
    return 0;
}
