#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "grove_imu_9dof.h"

int main() {
    stdio_init_all();
    
    printf("Grove IMU 9DOF (LCM20600+AK09918) Test\n");
    
    if (!imu_init()) {
        printf("IMU initialization failed!\n");
        return -1;
    }
    
    imu_data_t sensor_data;
    
    while (1) {
        if (read_all_sensors(&sensor_data)) {
            printf("Accel: X=%d, Y=%d, Z=%d (mg)\n", 
                   sensor_data.accel_x, sensor_data.accel_y, sensor_data.accel_z);
            printf("Gyro:  X=%d, Y=%d, Z=%d (dps)\n", 
                   sensor_data.gyro_x, sensor_data.gyro_y, sensor_data.gyro_z);
            printf("Mag:   X=%ld, Y=%ld, Z=%ld (uT)\n", 
                   sensor_data.mag_x, sensor_data.mag_y, sensor_data.mag_z);
            printf("Temp:  %d°C\n", sensor_data.temperature);
            printf("---\n");
        } else {
            printf("Failed to read sensor data\n");
        }
        
        sleep_ms(100);
    }
    
    return 0;
}