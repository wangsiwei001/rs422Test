#include "log.h"

void logToFiles(const char* level, const char* fileName,const int line,const char* fmt, ...) {
    FILE* logFile = fopen("StarSoftware.log", "a");
    if (logFile == NULL) {
        LOG_ERROR("Unable to open log file");
        return;
    }

    va_list args;
    va_start(args, fmt);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm = *localtime(&ts.tv_sec);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm);

    fprintf(logFile, "%s.%03ld %s [%s:%d] ", timestamp, ts.tv_nsec / 1000000, level, fileName, line);
    vfprintf(logFile, fmt, args);
    fprintf(logFile, "\n");

    va_end(args);
    fclose(logFile);
}

