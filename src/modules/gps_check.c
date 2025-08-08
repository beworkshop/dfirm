#include "modules/gps_check.h"
#include "log.h"
#include <math.h>


static gps_temp_pos_t temp_gps_pos_gngga = {0};
static gps_temp_pos_t temp_gps_pos_gngll = {0};
static gps_temp_pos_t temp_gps_pos_gnrmc = {0};
static gps_temp_velocity temp_gps_vel_gnrmc = {0};
static gps_temp_velocity temp_gps_vel_gnvtg = {0};


bool positions_match(gps_temp_pos_t *pos1, gps_temp_pos_t *pos2, float tolerance) {
    if (pos1->temp_f_latitude == 0.0f && pos1->temp_f_longitude == 0.0f &&
        pos2->temp_f_latitude == 0.0f && pos2->temp_f_longitude == 0.0f) {
        return false;
    }
    
    return (fabs(pos1->temp_f_latitude - pos2->temp_f_latitude) < tolerance) &&
           (fabs(pos1->temp_f_longitude - pos2->temp_f_longitude) < tolerance) &&
           (pos1->temp_c_latitude == pos2->temp_c_latitude) &&
           (pos1->temp_c_longitude == pos2->temp_c_longitude);
}
void update_gps_pos(gps_position_t *gps_position) {
    const float GPS_TOLERANCE = 0.001f;
    
    if (positions_match(&temp_gps_pos_gngga, &temp_gps_pos_gngll, GPS_TOLERANCE)) {
        gps_position->f_latitude = temp_gps_pos_gngga.temp_f_latitude;
        gps_position->f_longitude = temp_gps_pos_gngga.temp_f_longitude;
        gps_position->c_latitude = temp_gps_pos_gngga.temp_c_latitude;
        gps_position->c_longitude = temp_gps_pos_gngga.temp_c_longitude;

        LOG_INFO("GPS: Using GNGGA/GNGLL validated position");
        return;
    }
    
    if (positions_match(&temp_gps_pos_gngga, &temp_gps_pos_gnrmc, GPS_TOLERANCE)) {
        gps_position->f_latitude = temp_gps_pos_gngga.temp_f_latitude;
        gps_position->f_longitude = temp_gps_pos_gngga.temp_f_longitude;
        gps_position->c_latitude = temp_gps_pos_gngga.temp_c_latitude;
        gps_position->c_longitude = temp_gps_pos_gngga.temp_c_longitude;
        LOG_INFO("GPS: Using GNGGA/GNRMC validated position");
        return;
    }

    if (positions_match(&temp_gps_pos_gngll, &temp_gps_pos_gnrmc, GPS_TOLERANCE)) {
        gps_position->f_latitude = temp_gps_pos_gngll.temp_f_latitude;
        gps_position->f_longitude = temp_gps_pos_gngll.temp_f_longitude;
        gps_position->c_latitude = temp_gps_pos_gngll.temp_c_latitude;
        gps_position->c_longitude = temp_gps_pos_gngll.temp_c_longitude;
        LOG_INFO("GPS: Using GNGLL/GNRMC validated position");
        return;
    }
    LOG_DEBUG("GPS: No matching positions found, keeping previous data");
}
// More robust gps update to avoid corrupted data being updated
void check_gps_pos(gps_position_t* gps_pos, float f_latitude, float f_longitude, char c_latitude, char c_longitude, int code) {
    switch (code) {
    case 0x0:
        temp_gps_pos_gngga.temp_f_latitude = f_latitude;
        temp_gps_pos_gngga.temp_f_longitude = f_longitude;
        temp_gps_pos_gngga.temp_c_latitude = c_latitude;
        temp_gps_pos_gngga.temp_c_longitude = c_longitude;
        break;
    case 0x1:
        temp_gps_pos_gngll.temp_f_latitude = f_latitude;
        temp_gps_pos_gngll.temp_f_longitude = f_longitude;
        temp_gps_pos_gngll.temp_c_latitude = c_latitude;
        temp_gps_pos_gngll.temp_c_longitude = c_longitude;
        break;
    case 0x2:
        temp_gps_pos_gnrmc.temp_f_latitude = f_latitude;
        temp_gps_pos_gnrmc.temp_f_longitude = f_longitude;
        temp_gps_pos_gnrmc.temp_c_latitude = c_latitude;
        temp_gps_pos_gnrmc.temp_c_longitude = c_longitude;
        update_gps_pos(gps_pos);
        break;
    default:
        LOG_ERROR("Invalid code to update gps pos");
        break;
    }
}

void update_gps_vel(gps_position_t* gps_pos) {
    if (fabs(temp_gps_vel_gnrmc.temp_speed - temp_gps_vel_gnvtg.temp_speed) < 1 &&
        fabs(temp_gps_vel_gnrmc.temp_track_angle - temp_gps_vel_gnvtg.temp_track_angle) < 5){
            
        gps_pos->speed = temp_gps_vel_gnvtg.temp_speed;
        gps_pos->track_angle = temp_gps_vel_gnvtg.temp_track_angle;

        LOG_INFO("GPS: Using GNRMC/GNVTG validated velocity");
        return;
    }
    LOG_DEBUG("GPS: No matching velocity found, keeping previous data");
}
void check_gps_velocity(gps_position_t* gps_pos, float speed, float track_angle, int code) {
    switch (code) {
    case 0x2:
        temp_gps_vel_gnrmc.temp_speed = speed;
        temp_gps_vel_gnrmc.temp_track_angle = track_angle;
        break;
    case 0x3:
        temp_gps_vel_gnvtg.temp_speed = speed;
        temp_gps_vel_gnvtg.temp_track_angle = track_angle;
        update_gps_vel(gps_pos);
        break;
    default:
        LOG_ERROR("Invalid code to update gps pos");
        break;
    }
}