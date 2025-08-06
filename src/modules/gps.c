#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "modules/gps.h"
#include "protocols/uart.h"

#define ASSIGN_IF_NONZERO(var, val) if ((val) != 0.0f) { (var) = (val); }




// -- Helper functions - for the parsers

// Matches pattern like $GNGGA
bool str_match(char* str, char pattern[6]){
    for (int i = 0; i < 6; i++){
        if (str[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

inline bool is_hex(char c) {
    return ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
}


char parse_char(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;

    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }

    return msg[i];
}
float parse_float(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;
    
    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }
    
    return atof(msg + i);
}

int parse_int(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;
    
    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }
    
    return atoi(msg + i);
}

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

// -- Parsers - update data structures

void parse_gngga(char* msg, gps_time_t *gps_time, gps_position_t *gps_position) {
    int time_int = parse_int(msg, 1);
    gps_time->hours = time_int / 10000;
    gps_time->minutes = (time_int / 100) % 100;
    gps_time->seconds = time_int % 100;
    ASSIGN_IF_NONZERO(gps_position->f_latitude, parse_float(msg, 2));
    ASSIGN_IF_NONZERO(gps_position->f_longitude, parse_float(msg, 4));
    gps_position->c_latitude = parse_char(msg, 3);
    gps_position->c_longitude = parse_char(msg, 5);
    ASSIGN_IF_NONZERO(gps_position->altitude, parse_float(msg, 9));
}
void parse_gngll(char* msg, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    ASSIGN_IF_NONZERO(gps_position->f_latitude, parse_float(msg, 1));
    ASSIGN_IF_NONZERO(gps_position->f_longitude, parse_float(msg, 3));
    gps_position->c_latitude = parse_char(msg, 2);
    gps_position->c_longitude = parse_char(msg, 4);
    ASSIGN_IF_NONZERO(gps_satellite->pdop, parse_float(msg,12));
    ASSIGN_IF_NONZERO(gps_satellite->hdop, parse_float(msg,13));
    ASSIGN_IF_NONZERO(gps_satellite->vdop, parse_float(msg,14));
}
// Unused
void parse_gngsa(char* msg, gps_satellite_t *gps_satellite) {
    ASSIGN_IF_NONZERO(gps_satellite->pdop, parse_float(msg,15));
    ASSIGN_IF_NONZERO(gps_satellite->hdop, parse_float(msg,16));
    ASSIGN_IF_NONZERO(gps_satellite->vdop, parse_float(msg,17));
}
// Unused
void parse_gngsv(char* msg, gps_satellite_t *gps_satellite) {
    ASSIGN_IF_NONZERO(gps_satellite->snr, parse_int(msg, 7));
}
void parse_gnrmc(char* msg, gps_satellite_t *gps_satellite) {
    char status = parse_char(msg, 2);
    if (status == 'A') {gps_satellite->status = 1;}
    if (status == 'V') {gps_satellite->status = 0;}
    //gps_position.f_latitude = parse_float(msg, 3);
    //gps_position.f_longitude = parse_float(msg, 5);
    //gps_position.c_latitude = parse_char(msg, 4);
    //gps_position.c_longitude = parse_char(msg, 6);
    //gps_position.track_angle = parse_float(msg, 7);
    //gps_position.speed = parse_float(msg, 8) * 0.514444f; // knots -> m/s
}


void parse_gnvtg(char* msg, gps_position_t *gps_position) {
    ASSIGN_IF_NONZERO(gps_position->track_angle, parse_float(msg, 1));
    ASSIGN_IF_NONZERO(gps_position->speed, parse_float(msg, 5) * 0.514444f);
}
// Unused
void parse_gnzda(char* msg, gps_time_t *gps_time, gps_date_t *gps_date) {
    int time_int = parse_int(msg, 1);
    gps_time->hours = time_int / 10000;
    gps_time->minutes = (time_int / 100) % 100;
    gps_time->seconds = time_int % 100;
    if (gps_date->update) {
        gps_date->update = false;
        gps_date->day = parse_int(msg,2);
        gps_date->month = parse_int(msg,3);
        gps_date->year = parse_int(msg,4);
    }
}


void d_parse_line(char* msg, gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    if (is_nmea_incomplete(msg)){
        return;
    }
    if (str_match(msg, "$GNGGA")){
        parse_gngga(msg, gps_time, gps_position);
        return;
    }
    if (str_match(msg, "$GNGLL")){
        parse_gngll(msg, gps_position, gps_satellite);
        return;
    }
    // Unused
    /*
    if (str_match(msg, "$GNGSA")){
        parse_gngsa(msg, gps_satellite);
        return;
    }

    if (str_match(msg, "$GNGSV")){
        parse_gngsv(msg, gps_satellite);
        return;
    }
    */
    if (str_match(msg, "$GNRMC")){
        parse_gnrmc(msg, gps_satellite);
        return;
    }
    if (str_match(msg, "$GNVTG")){
        parse_gnvtg(msg, gps_position);
        return;
    }

    // Unused
    /*
    if (str_match(msg, "$GNZDA")){
        parse_gnzda(msg, gps_time, gps_date);
        return;
    }
    */


};

void d_update_gps(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    for (int i = 0; i < 10; i++){
        char* msg = d_uart_read();
        if (msg[0] != '\0'){
            LOG_INFO(msg);
            d_parse_line(msg, gps_time, gps_position, gps_satellite);
        }
        sleep_ms(5);
    }
}

// -- Utils functions

void d_print_gps_compact(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    LOG_INFO("GPS: %02d:%02d:%02d %02d/%02d/%04d | %.6f°%c %.6f°%c %.2fm| %.1fm/s %.0f° | PDOP:%.1f HDOP:%.1f VDOP:%.1f SNR:%ddB %s\n",
           gps_time->hours, gps_time->minutes, gps_time->seconds,
           gps_position->f_latitude, gps_position->c_latitude,
           gps_position->f_longitude, gps_position->c_longitude,
           gps_position->altitude,
           gps_position->speed, gps_position->track_angle,
           gps_satellite->pdop, gps_satellite->hdop, gps_satellite->vdop, gps_satellite->snr,
           gps_satellite->status == 1 ? "OK" : "POOR");
}

