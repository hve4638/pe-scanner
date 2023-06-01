#pragma once
// È£È¯¼º
#define LOG_LEVEL_OFF LogLevel::DISABLE
#define LOG_LEVEL_ALL   LogLevel::ALL
#define LOG_LEVEL_DEBUG LogLevel::DEBUG
#define LOG_LEVEL_INFO  LogLevel::INFO
#define LOG_LEVEL_WARN  LogLevel::WARN
#define LOG_LEVEL_ERROR LogLevel::ERR
#define LOG_LEVEL_FATAL LogLevel::FATAL

namespace PEScan {
    enum class LogLevel {
        DISABLE = 0x0,
        ALL = 0x1,
        DEBUG = 0x2,
        INFO = 0x3,
        WARN = 0x4,
        ERR = 0x5,
        FATAL = 0x6
    };
}