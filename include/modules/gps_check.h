#pragma once
#include "modules/gps.h"

typedef struct {
    float temp_f_latitude, temp_f_longitude;
    char temp_c_latitude, temp_c_longitude;
} gps_temp_pos_t;

typedef struct {
    float temp_speed; //   m/s
    float temp_track_angle;
} gps_temp_velocity;

void check_gps_pos(gps_position_t* gps_pos, float f_latitude, float f_longitude, char c_latitude, char c_longitude, int code);
void check_gps_velocity(gps_position_t* gps_pos, float speed, float track_angle, int code);