#pragma once


void d_uart_init(bool fast);
void d_uart_send(const char* str);
void d_uart_fast_baude();
void d_uart_set_nmea();
void d_uart_slow_baude();
void d_uart_hot_start();
char* d_uart_read_raw();
char* d_uart_read();
void d_uart_deinit();