#include "pico/stdlib.h"
#define BAUD_RATE 9600
#define GPIO_TX 0
#define GPIO_RX 1
#define UART uart0
#define UART_TIMEOUT 50 //ms
#define BUFFER_SIZE 256


static char buffer[BUFFER_SIZE];

void d_uart_init() {
    uart_init(UART, BAUD_RATE); 
    gpio_set_function(GPIO_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_RX, GPIO_FUNC_UART);
}

void d_uart_send(const char* str) {
    uart_puts(UART, str);
}

char* d_uart_read() {
    int index = 0;
    absolute_time_t timeout = make_timeout_time_ms(UART_TIMEOUT);
    
    buffer[0] = '\0';
    
    while (!time_reached(timeout) && index < sizeof(buffer) - 1) {
        if (uart_is_readable(UART)) {
            char c = uart_getc(UART);
            
            if (c == '\n') {
                buffer[index] = '\0';
                return buffer;
            }
            
            if ((c >= 32 && c <= 126) || c == '\r') {
                buffer[index++] = c;
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