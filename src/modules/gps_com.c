#include "protocols/uart.h"




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
    uart_puts("*");
    uart_puts(cs);
    uart_puts("\r\n");
}

void d_uart_slow_baude() {
    d_uart_send("$PCAS01,0");
    sleep_ms(1000);

    
}
void d_uart_fast_baude() {
    d_uart_send("$PCAS01,2");
    sleep_ms(1000);

    
}

void d_gps_set_nmea() {
    // nGGA,nGLL,nGSA,nGSV,nRMC,nVTG,nZDA,nANT,nDHV,nLP
    d_uart_send("$PCAS03,1,1,0,0,1,1,0,,,,,,,,,,,");
}

void d_gps_hot_start() {
    d_uart_send("$PCAS10,rs");
}
