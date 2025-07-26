#pragma once


void d_uart_init();
void d_uart_send(const char* str);
void d_uart_fast_baude();
void d_uart_set_nmea();
char* d_uart_read_raw();
char* d_uart_read();
void d_uart_deinit();