#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <cstring>

// 定义日志等级
#define INFO "INFO"   // 信息日志宏
#define DEBUG "DEBUG" // 调试日志宏
#define ERROR "ERROR" // 错误日志宏
#define FATAL "FATAL" // 错误日志宏

// 默认日志等级为信息类日志
// #define DEFAULT_LOG_LEVEL

// 日志宏函数
// 需要输出            时间        文件名   第几行    日志信息
// 例如      [2024-10-27 21:50:43 log.hpp:34][DEBUG] message
#define LOG(LEVEL, format, ...)                                                                                                         \
    do                                                                                                                                  \
    {                                                                                                                                   \
        time_t tp = time(nullptr);                                                                                                      \
        struct tm *lt = localtime(&tp);                                                                                                 \
        char buffer[32];                                                                                                                \
        strftime(buffer, sizeof(buffer) - 1, "%Y-%m-%d %H:%M:%S", lt);                                                                  \
        fprintf(stdout, "[%s %s:%d][%s] " format "\n", buffer, basename(const_cast<char *>(__FILE__)), __LINE__, LEVEL, ##__VA_ARGS__); \
        if (strcmp(LEVEL, FATAL) == 0)                                                                                                  \
            exit(-1);                                                                                                                   \
    } while (0)

#define LOG_INFO(format, ...) LOG(INFO, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG(DEBUG, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(ERROR, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) LOG(FATAL, format, ##__VA_ARGS__)

// if (DEFAULT_LOG_LEVEL == "NOT")
//     break;