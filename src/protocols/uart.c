#include "pico/stdlib.h"
#include "log.h"


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




void d_uart_init() {
    uart_init(UART, DEFAULT_BAUD_RATE);
    gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
}

void d_uart_send(const char* str) {
    uart_puts(UART, str);
}

void d_uart_change_baude(int rate) {
    uart_deinit(UART);
    uart_init(UART, rate);
    //gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    //gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
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

char* d_uart_read_nmea() {
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