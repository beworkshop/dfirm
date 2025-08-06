#pragma once

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
} accel_data_t;
void d_start_acc();
void d_update_acc(accel_data_t *acc);