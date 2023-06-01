#include "PEScan.h"
#include "Logger.h"
#include "PEParser.h"
#include "PEPrinter.h"
#include "FileSearch.h"
#include "RegWatch.h"
#include "ProcWatch.h"
#include "IPCUtil.h"
#include "FileWatch.h"

namespace PEScan {
    ILogger* createLogger() {
        return new Logger();
    }
    ILogInfo* createCodeLogInfo(const TCHAR* message, int errorCode, BOOL detail, 
        const char* fname, const int fline) {

        auto logInfo = new CodeLogInfo(message, errorCode, fname, fline);

        if (detail) logInfo->detail();
        return logInfo;
    }
    IPEParser* createPEParser() {
        return new PEParser();
    }
    IPEPrinter* createPEPrinter() {
        return new PEPrinter();
    }
    IFileSearch* createFileSearch() {
        return new FileSearch();
    }

    IRegWatch* createRegWatch() {
        return new RegWatch();
    }

    IProcWatch* createProcWatch() {
        return new ProcWatch();
    }

    IIPCUtil* createIPCUtil() {
        return new IPCUtil();
    }
    
    IFileWatch* createFileWatch() {
        return new FileWatch();
    }
}