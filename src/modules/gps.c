#include "pico/stdlib.h"
#include <stdlib.h>
#include "log.h"
#include "modules/gps.h"
#include "modules/gps_check.h"
#include "protocols/uart.h"
#include <math.h>


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
    int start = i;
    return atof(msg + start);
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
        if (*p < 32 || *p > 126) return true;  // Invalid character
        calculated_checksum ^= *p;
        p++;
        len++;
    }
    
    if (*p != '*') return true;
    p++;
    

    if (!*p || !*(p+1)) return true;
    
    int received_checksum = 0;
    for (int i = 0; i < 2; i++) {
        char c = p[i];
        if (c >= '0' && c <= '9') {
            received_checksum = (received_checksum << 4) + (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            received_checksum = (received_checksum << 4) + (c - 'A' + 10);
        } else if (c >= 'a' && c <= 'f') {
            received_checksum = (received_checksum << 4) + (c - 'a' + 10);
        } else {
            return true;  // Invalid hex character
        }
    }
    
    if (calculated_checksum != received_checksum) {
        return true;
    }
    
    return false;
}
bool altitude_check(float altitude) {
    if (altitude > 300.0f || altitude < 0.0f) {
        return false;
    }
    return true;
}

// -- Parsers - update data structures

void parse_gngga(char* msg, gps_time_t *gps_time, gps_position_t *gps_position) {
    int time_int = parse_int(msg, 1);
    gps_time->hours = time_int / 10000;
    gps_time->minutes = (time_int / 100) % 100;
    gps_time->seconds = time_int % 100;
    check_gps_pos(gps_position, parse_float(msg, 2), parse_float(msg, 4), parse_char(msg, 3), parse_char(msg, 5), 0x0);
    float altitude = parse_float(msg, 9);
    if (altitude_check(altitude)){
        gps_position->altitude = altitude;
    }
}
void parse_gngll(char* msg, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    check_gps_pos(gps_position, parse_float(msg, 1), parse_float(msg, 3), parse_char(msg, 2), parse_char(msg, 4), 0x1);
}
// Unused
void parse_gngsa(char* msg, gps_satellite_t *gps_satellite) {
    gps_satellite->pdop = parse_float(msg,15);
    gps_satellite->hdop = parse_float(msg,16);
    gps_satellite->vdop = parse_float(msg,17);
}
// Unused
void parse_gngsv(char* msg, gps_satellite_t *gps_satellite) {
    gps_satellite->snr = parse_int(msg, 7);
}
void parse_gnrmc(char* msg, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    char status = parse_char(msg, 2);
    if (status == 'A') {gps_satellite->status = 1;}
    if (status == 'V') {gps_satellite->status = 0;}
    check_gps_pos(gps_position, parse_float(msg, 3), parse_float(msg, 5), parse_char(msg, 4), parse_char(msg, 6), 0x2);
    check_gps_velocity(gps_position, parse_float(msg, 7) * 0.514444f, parse_float(msg, 8), 0x2); // knots -> m/s
}

void parse_gnvtg(char* msg, gps_position_t *gps_position) {
    check_gps_velocity(gps_position, parse_float(msg, 5) * 0.514444f, parse_float(msg, 1), 0x3); // knots -> m/s
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
        LOG_DEBUG("NMEA check fail");
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
        parse_gnrmc(msg, gps_position, gps_satellite);
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

void d_update_gps(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite, bool print) {
    for (int i = 0; i < 10; i++){
        char* msg = d_uart_read_nmea();
        if (print && msg != "\n") {
            LOG_DEBUG(msg);
        }
        if (msg[0] != '\0'){
            d_parse_line(msg, gps_time, gps_position, gps_satellite);
        }
    }
}

// -- Utils functions

void d_print_gps_compact(gps_time_t *gps_time, gps_position_t *gps_position, gps_satellite_t *gps_satellite) {
    LOG_INFO(" === GPS: %02d:%02d:%02d | %.6f°%c %.6f°%c %.2fm | %.1fm/s %.0f° | %s === ",
           gps_time->hours, gps_time->minutes, gps_time->seconds,
           gps_position->f_latitude, gps_position->c_latitude,
           gps_position->f_longitude, gps_position->c_longitude,
           gps_position->altitude,
           gps_position->speed, gps_position->track_angle,
           gps_satellite->status ? "OK" : "POOR");
}
