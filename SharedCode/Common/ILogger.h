#pragma once
#include "Common/typedef.h"
#include "Common/LogLevel.h"
#include "Common/LogDirection.h"
#include "Common/ILogInfo.h"

namespace PEScan {
    struct ILogger {
        virtual ~ILogger() {}
        virtual void setLogLevel(LogLevel) abstract;                // LogLevel 설정
        virtual void setLogDirection(LogDirection) abstract;        // LogDirection 설정 (콘솔 출력 or DebugView)
        virtual ILogger& operator<<(LogLevel) abstract;             // 출력 LogLevel 설정
                                                                    // 지정된 LogLevel보다 출력 레벨이 높아야 로그가 출력됨

        // 메세지 출력
        virtual ILogger& operator<<(const tstring&) abstract;
        virtual ILogger& operator<<(const TCHAR*) abstract;

        // ILogInfo에 지정된 메세지 출력
        virtual ILogger& operator<<(ILogInfo*) abstract;
        virtual ILogger& operator<<(shared_ptr<ILogInfo>) abstract;

        virtual ILogger& operator<<(LogDirection) abstract;
    };
}

