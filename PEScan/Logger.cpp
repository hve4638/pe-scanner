#include "Logger.h"
#include "CodeLogInfo.h"
#include <format>

namespace PEScan {
    Logger::Logger() {
    
    }
    Logger::Logger(LogLevel logLevel) {
        setLogLevel(logLevel);
    }
    Logger::Logger(LogDirection logDirection) {
        setLogDirection(logDirection);
    }
    Logger::Logger(LogLevel logLevel, LogDirection logDirection) {
        setLogLevel(logLevel);
        setLogDirection(logDirection);
    }
    Logger::Logger(LogDirection logDirection, LogLevel logLevel) {
        setLogLevel(logLevel);
        setLogDirection(logDirection);
    }

    void Logger::setLogLevel(LogLevel logLevel) {
        m_logLevel = logLevel;
    }
    void Logger::setLogDirection(LogDirection logDirection) {
        m_logDirection = logDirection;
    }

    BOOL Logger::enabled() {
        return (m_currentLogLevel >= m_logLevel);
    }

    void Logger::write(const tstring& message) {
        if (m_logDirection == LogDirection::DEBUGVIEW) {
            OutputDebugStringT(message.c_str());
        }
        else if (m_logDirection == LogDirection::CONSOLE) {
            tcout << message;
        }
    };

    void Logger::writeln(const tstring& message) {
        write(message);
        if (m_logDirection == LogDirection::DEBUGVIEW) {
            OutputDebugStringT(_T("\n"));
        }
        else if (m_logDirection == LogDirection::CONSOLE) {
            tcout << endl;
        }
    }

    ILogger& Logger::operator<<(const tstring& message) {
        if (enabled()) {
            write(message);
        }
        return *this;
    }

    ILogger& Logger::operator<<(const TCHAR* message) {
        if (enabled()) {
            write(format(_T("{:s}"), message));
        }
        return *this;
    }

    ILogger& Logger::operator<<(ILogInfo* info) {
        if (enabled()) {
            write(info->message());
        }
        return *this;
    }

    ILogger& Logger::operator<<(shared_ptr<ILogInfo> info) {
        if (enabled()) {
            write(info->message());
        }
        return *this;
    }

    ILogger& Logger::operator<<(CodeLogInfo info) {
        return *this << &info;
    }

    ILogger& Logger::operator<<(ExceptionLogInfo info) {
        return *this << &info;
    }

    ILogger& Logger::operator<<(LogLevel logLevel) {
        m_currentLogLevel = logLevel;
        return *this;
    }

    ILogger& Logger::operator<<(LogDirection logDirection) {
        m_logDirection = logDirection;
        return *this;
    }

    void Logger::log(const TCHAR* logMessage, DWORD errorCode, LogLevel logLevel) {
        CodeLogInfo logInfo = { logMessage, errorCode };

        *this << logLevel;
        *this << &logInfo << NL;
    }

    void Logger::log(const TCHAR* logMessage, LogLevel logLevel) {
        CodeLogInfo logInfo = { logMessage, GetLastError()};

        *this << logLevel;
        *this << &logInfo << NL;
    }
}