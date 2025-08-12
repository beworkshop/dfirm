#pragma once

#include "pico/stdlib.h"

// -- Core gps data structures
typedef struct {
    int hours;
    int minutes;
    int seconds;
} gps_time_t;

typedef struct {
    float f_latitude;
    float f_longitude;
    float speed; //   m/s
    float altitude;
    float track_angle;
    char c_latitude;
    char c_longitude;
} gps_position_t;

typedef struct {
    int day;
    int month;
    int year;
    bool update;
} gps_date_t;

typedef struct {
    float pdop;
    float hdop;
    float vdop;
    int snr;
    bool status; // measurement of reception
} gps_satellite_t;


void d_update_gps(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite, bool print);
void d_print_gps_compact(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite);


