#pragma once


void d_uart_init();
void d_uart_send(const char* str);
char* d_uart_read();
void d_uart_deinit();