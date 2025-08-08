#pragma once

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
} accel_data_t;

void d_start_acc(void);
void d_update_acc(accel_data_t *acc);
void d_print_acc_compact(accel_data_t *acc);