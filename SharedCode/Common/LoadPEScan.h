#pragma once
#include <Windows.h>
#include "Common/ILogger.h"


namespace PEScan {
    using namespace LogUtils;
    typedef ILogger* (*CreateLoggerFunc)();

    extern CreateLoggerFunc CreateLogger;

    extern bool loadLogger();

    extern bool loadPEScan();
    extern void unloadPEScan();
}