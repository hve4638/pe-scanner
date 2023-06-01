#pragma once
#include "Common/typedef.h"
#include "Common/LogLevel.h"
#include "Common/LogDirection.h"
#include "Common/NL.h"
#include "Common/ILogInfo.h"

namespace PEScan {
    interface ILogger {
        virtual void setLogLevel(LogLevel) abstract;
        virtual void setLogDirection(LogDirection) abstract;
        virtual ILogger& operator<<(const tstring&) abstract;
        virtual ILogger& operator<<(const TCHAR*) abstract;
        virtual ILogger& operator<<(LogLevel) abstract;
        virtual ILogger& operator<<(LogDirection) abstract;
        virtual ILogger& operator<<(const ILogInfo*) abstract;
    };
}