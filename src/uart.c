#include "pico/stdlib.h"
#include "include/log.h"
#include "include/utils.h"


#define DEFAULT_BAUD_RATE 9600
#define FAST_BAUD_RATE 19200
#define GPIO_TX 0
#define GPIO_RX 1
#define UART uart0
#define UART_TIMEOUT 100 //ms
#define BUFFER_SIZE 256
#define RAW_BUFFER_SIZE 4096



static char buffer[BUFFER_SIZE];
static char raw_buffer[RAW_BUFFER_SIZE];



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


void d_uart_init(bool fast) {
    if (fast) {
        uart_init(UART, FAST_BAUD_RATE); 
    } else {
        uart_init(UART, DEFAULT_BAUD_RATE);
    }
    
    gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
}
void d_uart_send(const char* str) {
    char cs[3];
    int checksum = nmea_checksum(str);
    byte_to_hex(checksum, cs);
    cs[2] = '\0';
    uart_puts(UART, str);
    uart_puts(UART, "*");
    uart_puts(UART, cs);
    uart_puts(UART, "\r\n");
}
void d_uart_slow_baude() {
    d_uart_send("$PCAS01,0");
    sleep_ms(1000);
    uart_deinit(UART);
    uart_init(UART, DEFAULT_BAUD_RATE);
    //gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    //gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
    
}
void d_uart_fast_baude() {
    d_uart_send("$PCAS01,0");
    sleep_ms(1000);
    uart_deinit(UART);
    uart_init(UART, FAST_BAUD_RATE);
    //gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    //gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
    
}

void d_uart_set_nmea() {
    // nGGA,nGLL,nGSA,nGSV,nRMC,nVTG,nZDA,nANT,nDHV,nLP
    d_uart_send("$PCAS03,1,1,0,0,1,1,1,,,,,,,,,,,");
}

void d_uart_hot_start() {
    d_uart_send("$PCAS10,rs");
}



char* d_uart_read_raw() {
    int index = 0;
    absolute_time_t timeout = make_timeout_time_ms(1000);
    raw_buffer[0] = '\0';

    while (!time_reached(timeout) && index < RAW_BUFFER_SIZE - 1) {
        if (uart_is_readable(UART)) {
            char c = uart_getc(UART);
            raw_buffer[index++] = c;
        } else {
            sleep_ms(1);
        }
    }
    raw_buffer[index] = '\0';
    return index > 0 ? raw_buffer : NULL;
}

char* d_uart_read() {
    int index = 0;
    absolute_time_t timeout = make_timeout_time_ms(UART_TIMEOUT);
    buffer[0] = '\0';
    int started = 0;

    while (!time_reached(timeout)) {
        if (uart_is_readable(UART)) {
            char c = uart_getc(UART);

            // Wait for start of sentence
            if (!started) {
                if (c == '$') {
                    started = 1;
                    buffer[index++] = c;
                }
                continue;
            }

            if (c == '\n' || c == '\r') {
                buffer[index] = '\0';
                return buffer;
            }

            if (index < BUFFER_SIZE - 1) {
                buffer[index++] = c;
            } else {
                // Buffer full
                buffer[index] = '\0';
                return buffer;
            }
        }
        sleep_ms(1);
    }
    buffer[index] = '\0';
    return index > 0 ? buffer : NULL;
}


void d_uart_deinit() {
    uart_deinit(UART);
}