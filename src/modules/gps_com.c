#include "protocols/uart.h"
#include "pico/time.h"



void byte_to_hex(int value, char* out) {
    static const char hex[] = "0123456789ABCDEF";
    out[0] = hex[(value >> 4) & 0x0F];
    out[1] = hex[value & 0x0F];
}
int nmea_checksum(const char* sentence) {
    int checksum = 0;
    if (*sentence == '$') {
        sentence++;
    }
    while (*sentence && *sentence != '*') {
        checksum ^= (int)(*sentence);
        sentence++;
    }
    return checksum;
}

void gps_send(const char* str) {
    char cs[3];
    int checksum = nmea_checksum(str);
    byte_to_hex(checksum, cs);
    cs[2] = '\0';
    d_uart_send(str);
    d_uart_send("*");
    d_uart_send(cs);
    d_uart_send("\r\n");
}


void d_uart_slow_baude() {
    gps_send("$PCAS01,0");
    sleep_ms(500);
    d_uart_change_baude(4800);
}
void d_gps_fast_baude() {
    gps_send("$PCAS01,5");
    sleep_ms(500);
    //d_uart_change_baude(115200); // TOFIX : can t read at 115200 even tho data is sent faster
}

void d_gps_update_rate() {
    gps_send("$PCAS02,100");
}
void d_gps_set_nmea() {
    // nGGA,nGLL,nGSA,nGSV,nRMC,nVTG,nZDA,nANT,nDHV,nLP
    gps_send("$PCAS03,1,1,0,0,1,1,0,,,,,,,,,,,");
}
void d_gps_set_sattelite() {
    gps_send("$PCAS04,7");
}
void d_gps_hot_start() {
    gps_send("$PCAS10,0");
}

void d_start_gps() {
    d_gps_hot_start();
    d_gps_fast_baude();
    d_gps_update_rate();
    d_gps_set_nmea();
    d_gps_set_sattelite();
}