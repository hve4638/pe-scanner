#pragma once
#include "api.h"
#include "Common/ILogger.h"
#include "Common/IPEParser.h"
#include "Common/IPEPrinter.h"
#include "Common/IFileSearch.h"
#include "Common/IRegWatch.h"
#include "Common/IProcWatch.h"
#include "Common/IIPCUtil.h"
#include "Common/IFileWatch.h"

namespace PEScan {
    extern "C" PESCAN_API ILogger* createLogger();
    extern "C" PESCAN_API ILogInfo* createCodeLogInfo(const TCHAR*, int, BOOL, const char*, const int);

    extern "C" PESCAN_API IPEParser* createPEParser();
    extern "C" PESCAN_API IPEPrinter* createPEPrinter();

    extern "C" PESCAN_API IFileSearch* createFileSearch();
    extern "C" PESCAN_API IRegWatch* createRegWatch();
    extern "C" PESCAN_API IProcWatch* createProcWatch();
    extern "C" PESCAN_API IProcWatch* createProcWatch();
    extern "C" PESCAN_API IIPCUtil* createIPCUtil();
    extern "C" PESCAN_API IFileWatch * createFileWatch();
}
