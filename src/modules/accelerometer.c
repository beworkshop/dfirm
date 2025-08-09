
#include "protocols/iic.h"
#include "modules/accelerometer.h"
#include "log.h"



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
#define MAG_WIA2        0x01


void d_start_acc() {
    write_reg(ICM_ADDR, ICM_PWR_MGMT_1, 0x80);
    sleep_ms(50);
    write_reg(ICM_ADDR, ICM_PWR_MGMT_1, 0x00);

    /*
    uint8_t id = read_register(MAG_ADDR, MAG_WIA2);
    if (id != 0x05) {
        printf("Magnetometer not found (ID: 0x%02X)\n", id);
        return;
    }
    
    // Reset and start continuous mode
    write_reg(MAG_ADDR, MAG_CNTL3, 0x01);  // Reset
    sleep_ms(10);
    write_reg(MAG_ADDR, MAG_CNTL2, 0x02);  // 10Hz continuous mode
    printf("Magnetometer ready\n");
    */
}
void d_update_acc(accel_data_t *acc) {
    uint8_t buffer[6];
    read_registers(ICM_ADDR, ICM_ACCEL_XOUT_H, buffer, 6);
    int16_t raw_ax = (buffer[0] << 8) | buffer[1];
    int16_t raw_ay = (buffer[2] << 8) | buffer[3];
    int16_t raw_az = (buffer[4] << 8) | buffer[5];
    acc->ax = raw_ax / 16384.0f;
    acc->ay = raw_ay / 16384.0f;
    acc->az = raw_az / 16384.0f;


    read_registers(ICM_ADDR, ICM_GYRO_XOUT_H, buffer, 6);
    
    int16_t raw_gx = (buffer[0] << 8) | buffer[1];
    int16_t raw_gy = (buffer[2] << 8) | buffer[3];
    int16_t raw_gz = (buffer[4] << 8) | buffer[5];
    
    acc->gx = raw_gx / 131.0f;
    acc->gy = raw_gy / 131.0f;
    acc->gz = raw_gz / 131.0f;


    
}


void d_print_acc_compact(accel_data_t *acc) {
    LOG_INFO(" === Accelerometer (g): X=%-8.2f  Y=%-8.2f  Z=%-8.2f\tGyroscope (°/s):   X=%-8.2f  Y=%-8.2f  Z=%-8.2f === ", 
        acc->ax, acc->ay, acc->az, 
        acc->gx, acc->gy, acc->gz);
}