#pragma once
#include "Loader/PEScanExport.h"

namespace PEScan {
    class PEScanLoader {
        HMODULE m_module = NULL;
        BOOL m_loaded = FALSE;

        CreateLoggerFunc m_createLogger = NULL;
        CreatePEParserFunc m_createPEParser = NULL;
        CreatePEPrinterFunc m_createPEPrinter = NULL;
        CreateCodeLogInfoFunc m_createCodeLogInfo = NULL;
        CreateFileSearchFunc m_createFileSearch = NULL;
        CreateRegWatchFunc m_createRegWatch = NULL;
        CreateProcWatchFunc m_createProcWatch = NULL;
        CreateIPCUtilFunc m_createIPCUtil = NULL;
        CreateFileWatch m_createFileWatch = NULL;

        void error(tstring message = _T("PEScan : error occured"));
    public:
        PEScanLoader(tstring path);
        ~PEScanLoader();

        BOOL isLoaded();

        BOOL loadLogger();
        BOOL loadPEParser();
        BOOL loadPEPrinter();
        BOOL loadCodeLogInfo();
        BOOL loadFileSearch();
        BOOL loadRegWatch();
        BOOL loadProcWatch();
        BOOL loadIPCUtil();
        BOOL loadFileWatch();

        shared_ptr<ILogger> Logger();
        shared_ptr<IPEParser> PEParser();
        shared_ptr<IPEPrinter> PEPrinter();
        shared_ptr<IFileSearch> FileSearch();
        shared_ptr<ILogInfo> CodeLogInfo(
            const TCHAR* message,
            int errorCode = GetLastError(),
            BOOL detail = FALSE,
            const char* __fname = __builtin_FUNCTION(),
            const int __fline = __builtin_LINE()
        );
        shared_ptr<IRegWatch> RegWatch();
        shared_ptr<IProcWatch> ProcWatch();
        shared_ptr<IIPCUtil> IPCUtil();
        shared_ptr<IFileWatch> FileWatch();
    };
}
