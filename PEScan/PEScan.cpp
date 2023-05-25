#include "pch.h"
#include "PEScan.h"
#include "Logger.h"

LogUtils::ILogger* CreateLogger() {
    return new LogUtils::Logger();
}