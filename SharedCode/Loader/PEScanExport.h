#pragma once
#include "Common/typedef.h"
#include "Common/ILogger.h"
#include "Common/IPEParser.h"
#include "Common/IPEPrinter.h"
#include "Common/IFileSearch.h"
#include "Common/IRegWatch.h"
#include "Common/IProcWatch.h"
#include "Common/IIPCUtil.h"
#include "Common/IFileWatch.h"

namespace PEScan {
    typedef ILogger* (*CreateLoggerFunc)();
    typedef IPEParser* (*CreatePEParserFunc)();
    typedef IPEPrinter* (*CreatePEPrinterFunc)();
    typedef ILogInfo* (*CreateCodeLogInfoFunc)(
        const TCHAR*,   // message
        int,            // lasterror
        BOOL,           // detail
        const char*,    // __fname
        const int       // __fline
    );
    typedef IFileSearch* (*CreateFileSearchFunc)();
    typedef IRegWatch* (*CreateRegWatchFunc)();
    typedef IProcWatch* (*CreateProcWatchFunc)();
    typedef IIPCUtil *(*CreateIPCUtilFunc)();
    typedef IFileWatch *(*CreateFileWatch)();
}