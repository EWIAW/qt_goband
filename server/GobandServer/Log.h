#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

// 日志等级枚举
typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_ERROR = 2
} LogLevel;

// 默认日志级别
extern LogLevel g_log_level;

// 获取当前时间字符串（线程不安全，仅用于简单日志）
static inline const char *current_time_str()
{
    static char time_buf[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);
    return time_buf;
}

// 内部日志函数
static inline void _log_message(LogLevel level, const char *level_str,
                                const char *func, int line, const char *fmt, ...)
{
    if (level < g_log_level)
        return; // 低于阈值不输出

    va_list args;
    va_start(args, fmt);

    const char *color = "";
    const char *reset = "\033[0m";
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        color = "\033[36m";
        break; // Cyan
    case LOG_LEVEL_WARNING:
        color = "\033[33m";
        break; // Yellow
    case LOG_LEVEL_ERROR:
        color = "\033[31m";
        break; // Red
    }

    fprintf(stderr, "%s[%s] %s:%d - ", color, current_time_str(), func, line);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "%s\n", reset);

    va_end(args);

    if (level == LOG_LEVEL_ERROR)
    {
        exit(EXIT_FAILURE);
    }
}

// 宏定义（自动传入函数名和行号）
#define LOG_DEBUG(fmt, ...) \
    _log_message(LOG_LEVEL_DEBUG, "DEBUG", __func__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_WARNING(fmt, ...) \
    _log_message(LOG_LEVEL_WARNING, "WARNING", __func__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    _log_message(LOG_LEVEL_ERROR, "ERROR", __func__, __LINE__, fmt, ##__VA_ARGS__)