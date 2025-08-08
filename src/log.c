#include "log.h"
#include "pico/time.h"

static log_level_t runtime_log_level = LOG_LEVEL_DEBUG;

static const char* level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};


void log_message(log_level_t level, const char* format, ...) {
    if (level < runtime_log_level) return;
    
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    printf("[%lu.%03lu] [%-5s] ", ms/1000, ms%1000, level_names[level]);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}