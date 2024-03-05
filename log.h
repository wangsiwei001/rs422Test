//
// Created by 10593 on 2023/12/21.
//
#ifndef MY_HEADER_FILE_H  // 如果 MY_HEADER_FILE_H 这个宏未定义，则执行下面的代码
#define MY_HEADER_FILE_H  // 定义 MY_HEADER_FILE_H 宏，表示这个头文件已经被包含了
#ifndef STARSOFTWARE_LOG_H
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#define STARSOFTWARE_LOG_H
#define FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

//typedef enum {
//    DEBUG,
//    INFO,
//    WARNING,
//    ERROR,
//    FATAL
//} LogLevel;
//
//LogLevel logLevel = INFO; // 默认为 INFO 级别
//
//LogLevel getLogLevel() {
//    return logLevel;
//}
//
//void setLogLevel(LogLevel level) {
//    logLevel = level;
//}
//
//// 判断当前日志级别是否支持某个级别
//int isLogLevelEnabled(LogLevel level) {
//    return level >= logLevel;
//}

void logToFiles(const char* level, const char* fileName,int line,const char* fmt, ...);
#define LOG_INFO(fmt, ...) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    struct tm tm = *localtime(&ts.tv_sec); \
    char timestamp[30]; \
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm); \
    printf("%s.%03ld INFO [%s:%d] " fmt "\n", timestamp, ts.tv_nsec / 1000000, FILENAME, __LINE__, ##__VA_ARGS__); \
    logToFiles("INFO",FILENAME,__LINE__,fmt, ##__VA_ARGS__);  \
} while (0)

#define LOG_ERROR(fmt, ...) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    struct tm tm = *localtime(&ts.tv_sec); \
    char timestamp[30]; \
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm); \
    printf("%s.%03ld ERROR [%s:%d] " fmt "\n", timestamp, ts.tv_nsec / 1000000, FILENAME, __LINE__, ##__VA_ARGS__); \
    logToFiles("INFO",FILENAME,__LINE__,fmt, ##__VA_ARGS__);  \
} while (0)

#define LOG_WARN(fmt, ...) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    struct tm tm = *localtime(&ts.tv_sec); \
    char timestamp[30]; \
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm); \
    printf("%s.%03ld WARN [%s:%d] " fmt "\n", timestamp, ts.tv_nsec / 1000000, FILENAME, __LINE__, ##__VA_ARGS__); \
    logToFiles("INFO",FILENAME,__LINE__,fmt, ##__VA_ARGS__);  \
} while (0)

#define LOG_DEBUG(fmt, ...) do { \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    struct tm tm = *localtime(&ts.tv_sec); \
    char timestamp[30]; \
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm); \
    printf("%s.%03ld DEBUG [%s:%d] " fmt "\n", timestamp, ts.tv_nsec / 1000000, FILENAME, __LINE__, ##__VA_ARGS__); \
    logToFiles("INFO",FILENAME,__LINE__,fmt, ##__VA_ARGS__);  \
} while (0)


#endif //STARSOFTWARE_LOG_H
#endif  // 结束头文件保护的标记
