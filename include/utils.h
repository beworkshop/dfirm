#pragma once



#include "pico/stdlib.h"

// Function declarations only (no definitions)
bool str_match(const char* str1, const char* str2);
char parse_char(const char* msg, int field_index);
float parse_float(const char* msg, int field_index);
int parse_int(const char* msg, int field_index);
void print_literal(const char* str);

