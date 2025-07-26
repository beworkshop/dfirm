#include "pico/stdlib.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Matches pattern like $GNGGA
bool str_match(char* str, char pattern[6]){
    for (int i = 0; i < 6; i++){
        if (str[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

inline bool is_hex(char c) {
    return ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
}


char parse_char(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;

    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }

    return msg[i];
}
float parse_float(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;
    
    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }
    
    return atof(msg + i);
}

int parse_int(char* msg, int field_index) {
    int current_field = 0;
    int i = 0;
    
    while (msg[i] && current_field < field_index) {
        if (msg[i] == ',') current_field++;
        i++;
    }
    
    return atoi(msg + i);
}


void print_literal(const char* str) {
    for (; *str; str++) {
        switch (*str) {
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            case '\\': printf("\\\\"); break;
            case '\"': printf("\\\""); break;
            default:
                if (isprint((unsigned char)*str))
                    putchar(*str);
                else
                    printf("\\x%02X", (unsigned char)*str);
        }
    }
    printf("\n");
}

