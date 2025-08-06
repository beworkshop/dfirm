
#include "protocols/iic.h"
#include "modules/accelerometer.h"
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



void d_start_acc() {
    d_init_i2c();
}
void d_update_acc(accel_data_t *acc) {
    uint8_t buffer[6];
    read_registers(ICM_ADDR, ICM_ACCEL_XOUT_H, buffer, 6);
    int16_t raw_ax = (buffer[0] << 8) | buffer[1];
    int16_t raw_ay = (buffer[2] << 8) | buffer[3];
    int16_t raw_az = (buffer[4] << 8) | buffer[5];
    acc->ax = raw_ax;
    acc->ay = raw_ay;
    acc->az = raw_az;


    read_registers(ICM_ADDR, ICM_GYRO_XOUT_H, buffer, 6);
    
    int16_t raw_gx = (buffer[0] << 8) | buffer[1];
    int16_t raw_gy = (buffer[2] << 8) | buffer[3];
    int16_t raw_gz = (buffer[4] << 8) | buffer[5];
    
    acc->gx = raw_gx;
    acc->gy = raw_gy;
    acc->gz = raw_gz;
}