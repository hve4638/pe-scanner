#pragma once
#include "Common/PEScanAPI.h"
#include "Common/LogLevel.h"
#include "Common/LogDirection.h"
#include "Common/LogNL.h"
#include "Common/ErrorLogInfo.h"
#include "Common/typedef.h"

namespace LogUtils {
    struct ILogger {
        virtual void setLogLevel(LogLevel) abstract;
        virtual void setLogDirection(LogDirection) abstract;
        virtual ILogger& operator<<(const tstring&) abstract;
        virtual ILogger& operator<<(const TCHAR*) abstract;
        virtual ILogger& operator<<(LogLevel) abstract;
        virtual ILogger& operator<<(LogDirection) abstract;
        virtual ILogger& operator<<(const ErrorLogInfo&) abstract;
    };
}