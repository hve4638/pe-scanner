#pragma once
#include "Common/typedef.h"
#include "Common/LogLevel.h"
#include "Common/LogDirection.h"
#include "Common/ILogInfo.h"

namespace PEScan {
    struct ILogger {
        virtual ~ILogger() {}
        virtual void setLogLevel(LogLevel) abstract;
        virtual void setLogDirection(LogDirection) abstract;
        virtual ILogger& operator<<(const tstring&) abstract;
        virtual ILogger& operator<<(const TCHAR*) abstract;
        virtual ILogger& operator<<(LogLevel) abstract;
        virtual ILogger& operator<<(LogDirection) abstract;
        virtual ILogger& operator<<(ILogInfo*) abstract;
        virtual ILogger& operator<<(shared_ptr<ILogInfo>) abstract;
    };
}