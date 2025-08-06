#include "pico/stdlib.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "include/log.h"


#define ASSIGN_IF_NONZERO(var, val) if ((val) != 0.0f) { (var) = (val); }

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

static gps_time_t gps_time = {0, 0, 0};
static gps_position_t gps_position = {0};
static gps_date_t gps_date = {0, 0, 0, true};
static gps_satellite_t gps_satellite = {0, false};


bool is_nmea_incomplete(char* msg) {
if (!msg) return true;

    // A valid sentence starts with "$"
    if (*msg++ != '$') return true;

    int calculated_checksum = 0x00;
    char *p = msg;
    int len = 0;
    while (*p && *p != '*' && len < 120) {
        if (*p < 32 || *p > 126) return true;
        calculated_checksum ^= *p;
        p++;
        len++;
    }
    
    if (*p != '*') return true;
    
    
    return false;
}


void parse_gngga(char* msg) {
    int time_int = parse_int(msg, 1);
    gps_time.hours = time_int / 10000;
    gps_time.minutes = (time_int / 100) % 100;
    gps_time.seconds = time_int % 100;
    ASSIGN_IF_NONZERO(gps_position.f_latitude, parse_float(msg, 2));
    ASSIGN_IF_NONZERO(gps_position.f_longitude, parse_float(msg, 4));
    gps_position.c_latitude = parse_char(msg, 3);
    gps_position.c_longitude = parse_char(msg, 5);
    ASSIGN_IF_NONZERO(gps_position.altitude, parse_float(msg, 9));
}
void parse_gngll(char* msg) {
    ASSIGN_IF_NONZERO(gps_position.f_latitude, parse_float(msg, 1));
    ASSIGN_IF_NONZERO(gps_position.f_longitude, parse_float(msg, 3));
    gps_position.c_latitude = parse_char(msg, 2);
    gps_position.c_longitude = parse_char(msg, 4);
    ASSIGN_IF_NONZERO(gps_satellite.pdop, parse_float(msg,12));
    ASSIGN_IF_NONZERO(gps_satellite.hdop, parse_float(msg,13));
    ASSIGN_IF_NONZERO(gps_satellite.vdop, parse_float(msg,14));
}
// Unused
void parse_gngsa(char* msg) {
    ASSIGN_IF_NONZERO(gps_satellite.pdop, parse_float(msg,15));
    ASSIGN_IF_NONZERO(gps_satellite.hdop, parse_float(msg,16));
    ASSIGN_IF_NONZERO(gps_satellite.vdop, parse_float(msg,17));
}
// Unused
void parse_gngsv(char* msg) {
    ASSIGN_IF_NONZERO(gps_satellite.snr, parse_int(msg, 7));
}
void parse_gnrmc(char* msg) {
    char status = parse_char(msg, 2);
    if (status == 'A') {gps_satellite.status = 1;}
    if (status == 'V') {gps_satellite.status = 0;}
    //gps_position.f_latitude = parse_float(msg, 3);
    //gps_position.f_longitude = parse_float(msg, 5);
    //gps_position.c_latitude = parse_char(msg, 4);
    //gps_position.c_longitude = parse_char(msg, 6);
    //gps_position.track_angle = parse_float(msg, 7);
    //gps_position.speed = parse_float(msg, 8) * 0.514444f; // knots -> m/s
}


void parse_gnvtg(char* msg) {
    ASSIGN_IF_NONZERO(gps_position.track_angle, parse_float(msg, 1));
    ASSIGN_IF_NONZERO(gps_position.speed, parse_float(msg, 5) * 0.514444f);
}

void parse_gnzda(char* msg) {
    int time_int = parse_int(msg, 1);
    gps_time.hours = time_int / 10000;
    gps_time.minutes = (time_int / 100) % 100;
    gps_time.seconds = time_int % 100;
    if (gps_date.update) {
        gps_date.update = false;
        gps_date.day = parse_int(msg,2);
        gps_date.month = parse_int(msg,3);
        gps_date.year = parse_int(msg,4);
    }
}

void d_parse_line(char* msg) {
    if (is_nmea_incomplete(msg)){
        return;
    }
    if (str_match(msg, "$GNGGA")){
        parse_gngga(msg);
        return;
    }
    if (str_match(msg, "$GNGLL")){
        parse_gngll(msg);
        return;
    }
    // Unused
    /*
    if (str_match(msg, "$GNGSA")){
        parse_gngsa(msg);
        return;
    }

    if (str_match(msg, "$GNGSV")){
        parse_gngsv(msg);
        return;
    }
    */
    if (str_match(msg, "$GNRMC")){
        parse_gnrmc(msg);
        return;
    }
    if (str_match(msg, "$GNVTG")){
        parse_gnvtg(msg);
        return;
    }
    if (str_match(msg, "$GNZDA")){
        parse_gnzda(msg);
        return;
    }

};

void d_print_gps_compact(void) {
    printf("GPS: %02d:%02d:%02d %02d/%02d/%04d | %.6f°%c %.6f°%c %.2fm| %.1fm/s %.0f° | PDOP:%.1f HDOP:%.1f VDOP:%.1f SNR:%ddB %s\n",
           gps_time.hours, gps_time.minutes, gps_time.seconds,
           gps_date.day, gps_date.month, gps_date.year,
           gps_position.f_latitude, gps_position.c_latitude,
           gps_position.f_longitude, gps_position.c_longitude,
           gps_position.altitude,
           gps_position.speed, gps_position.track_angle,
           gps_satellite.pdop, gps_satellite.hdop, gps_satellite.vdop, gps_satellite.snr,
           gps_satellite.status == 1 ? "OK" : "POOR");
}

bool d_should_fly() {
    return gps_position.f_latitude != 0.0 && gps_position.f_longitude != 0.0 && gps_satellite.status == 1;
}