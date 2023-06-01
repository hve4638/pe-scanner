#pragma once
#include "Common/ILogger.h"
#include "CodeLogInfo.h"
#include "ExceptionLogInfo.h"

namespace PEScan {
    class Logger : public ILogger {
        LogLevel m_logLevel = LogLevel::ALL;
        LogDirection m_logDirection = LogDirection::CONSOLE;
        LogLevel m_currentLogLevel = LogLevel::ALL;

        BOOL enabled();
        void write(const tstring&);
        void writeln(const tstring&);
    public:
        Logger();
        Logger(LogLevel);
        Logger(LogDirection);
        Logger(LogLevel, LogDirection);
        Logger(LogDirection, LogLevel);
        void setLogLevel(LogLevel) override;
        void setLogDirection(LogDirection) override;
        ILogger& operator<<(const tstring&) override;
        ILogger& operator<<(const TCHAR*) override;
        ILogger& operator<<(LogLevel) override;
        ILogger& operator<<(LogDirection) override;
        ILogger& operator<<(ILogInfo*) override;
        ILogger& operator<<(shared_ptr<ILogInfo>) override;

        ILogger& operator<<(CodeLogInfo);
        ILogger& operator<<(ExceptionLogInfo);

        // ȣȯ��
        void log(const TCHAR* logMessage, DWORD errorCode, LogLevel logLevel);
        void log(const TCHAR* logMessage, LogLevel logLevel);
    };
}


