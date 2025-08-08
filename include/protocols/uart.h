#pragma once


void d_uart_init();
void d_uart_send(const char* str);
void d_uart_change_baude(int rate);
char* d_uart_read_raw();
char* d_uart_read_nmea();
void d_uart_deinit();