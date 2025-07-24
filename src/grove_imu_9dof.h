#ifndef GROVE_IMU_9DOF_H
#define GROVE_IMU_9DOF_H

#include "pico_i2c_compat.h"

// From ICM20600.h
#define ICM20600_I2C_ADDR           0x69
#define ICM20600_WHO_AM_I           0x75
#define ICM20600_PWR_MGMT_1         0x6b
#define ICM20600_PWR_MGMT_2         0x6c
#define ICM20600_ACCEL_CONFIG       0x1c
#define ICM20600_GYRO_CONFIG        0x1b
#define ICM20600_ACCEL_XOUT_H       0x3b
#define ICM20600_GYRO_XOUT_H        0x43
#define ICM20600_TEMP_OUT_H         0x41

// From AK09918.h
#define AK09918_I2C_ADDR            0x0c
#define AK09918_WIA2                0x01
#define AK09918_ST1                 0x10
#define AK09918_HXL                 0x11
#define AK09918_CNTL2               0x31
#define AK09918_CNTL3               0x32

// Operating modes
#define AK09918_POWER_DOWN          0x00
#define AK09918_CONTINUOUS_100HZ    0x08

// Sensor data structure
typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    int32_t mag_x, mag_y, mag_z;
    int16_t temperature;
} imu_data_t;

// Function prototypes
bool imu_init(void);
bool read_accel_data(int16_t *ax, int16_t *ay, int16_t *az);
bool read_gyro_data(int16_t *gx, int16_t *gy, int16_t *gz);
bool read_mag_data(int32_t *mx, int32_t *my, int32_t *mz);
bool read_all_sensors(imu_data_t *data);
int16_t read_temperature(void);

#endif