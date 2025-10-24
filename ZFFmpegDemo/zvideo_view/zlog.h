#pragma once
#include <iostream>

enum ZLogLevel {
    ZLOG_DEBUG = 0,
    ZLOG_INFO,
    ZLOG_ERROR,
    ZLOG_FATAL
};

static std::string GetLogLevelStr(ZLogLevel level) {
    switch (level)
    {
    case ZLOG_DEBUG:
        return "DEBUG";
    case ZLOG_INFO:
        return "INFO";
    case ZLOG_ERROR:
        return "ERROR";
    case ZLOG_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

#define LOG_MIN_LEVEL ZLOG_DEBUG
#define ZLOG(str,level) \
        if (level >= LOG_MIN_LEVEL) \
        std::cout << "[" << GetLogLevelStr(level) << "] " << __FILE__ << ": " << __LINE__ << ": " \
        << str << std::endl;

#define ZLOGDEBUG(str) ZLOG(str,ZLOG_DEBUG)
#define ZLOGINFO(str) ZLOG(str,ZLOG_INFO)
#define ZLOGERROR(str) ZLOG(str,ZLOG_ERROR)
#define ZLOGFATAL(str) ZLOG(str,ZLOG_FATAL)