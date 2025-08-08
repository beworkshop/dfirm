#pragma once

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO  = 2,
    LOG_LEVEL_WARN  = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_FATAL = 5,
    LOG_LEVEL_NONE  = 6   // Disable all logging
} log_level_t;

void log_message(log_level_t level, const char* format, ...);

#define LOG_TRACE(...) log_message(LOG_LEVEL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) log_message(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) log_message(LOG_LEVEL_FATAL, __VA_ARGS__)


#ifndef LOG_COMPILE_LEVEL
#define LOG_COMPILE_LEVEL LOG_LEVEL_TRACE
#endif


#if LOG_COMPILE_LEVEL > LOG_LEVEL_TRACE
#undef LOG_TRACE
#define LOG_TRACE(...) ((void)0)  // No-op
#endif

#if LOG_COMPILE_LEVEL > LOG_LEVEL_DEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(...) ((void)0)  // No-op
#endif

#if LOG_COMPILE_LEVEL > LOG_LEVEL_INFO
#undef LOG_INFO
#define LOG_INFO(...) ((void)0)   // No-op
#endif

#if LOG_COMPILE_LEVEL > LOG_LEVEL_WARN
#undef LOG_WARN
#define LOG_WARN(...) ((void)0)   // No-op
#endif

#if LOG_COMPILE_LEVEL > LOG_LEVEL_ERROR
#undef LOG_ERROR
#define LOG_ERROR(...) ((void)0)  // No-op
#endif
