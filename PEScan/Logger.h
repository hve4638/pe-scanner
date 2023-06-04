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
        // 생성자에서 LogLevel과 LogDirection 초기화
        Logger();
        Logger(LogLevel);
        Logger(LogDirection);                                   
        Logger(LogLevel, LogDirection);
        Logger(LogDirection, LogLevel);


        void setLogLevel(LogLevel) override;                // LogLevel 설정
        void setLogDirection(LogDirection) override;        // LogDirection 설정 (콘솔 출력 or DebugView)
        ILogger& operator<<(LogLevel) override;             // 출력 LogLevel 설정
                                                            // 지정된 LogLevel보다 출력 레벨이 높아야 로그 출력

        ILogger& operator<<(LogDirection) override;         // LogDirection 설정 (콘솔 출력 or DebugView)

        ILogger& operator<<(const tstring&) override;       // 메세지 출력
        ILogger& operator<<(const TCHAR*) override;         // 메세지 출력

        // ILogInfo에 지정된 메세지 출력
        // CodeLogInfo와 ExceptionLogInfo는 ILogInfo의 구현
        ILogger& operator<<(ILogInfo*) override;
        ILogger& operator<<(shared_ptr<ILogInfo>) override; 
        ILogger& operator<<(CodeLogInfo);                   
        ILogger& operator<<(ExceptionLogInfo);

        // 호환성
        void log(const TCHAR* logMessage, DWORD errorCode, LogLevel logLevel);
        void log(const TCHAR* logMessage, LogLevel logLevel);
    };
}


