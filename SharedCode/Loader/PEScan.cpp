#include "Loader/PEScan.h"

namespace PEScan {
    PEScanLoader::PEScanLoader(tstring path) {
        m_module = LoadLibrary(path.c_str());
        m_loaded = (m_module != NULL);
    }

    PEScanLoader::~PEScanLoader() {
        if (m_module != NULL) {
            FreeLibrary(m_module);
            m_module = NULL;
        }
    }

    void PEScanLoader::error(tstring message) {
        tcout << message << endl;
        abort();
    }

    BOOL PEScanLoader::isLoaded() {
        return m_loaded;
    }

    BOOL PEScanLoader::loadLogger() {
        return DLL_PROC_LOAD(m_createLogger, "createLogger", CreateLoggerFunc);
    }
    BOOL PEScanLoader::loadPEParser() {
        return DLL_PROC_LOAD(m_createPEParser, "createPEParser", CreatePEParserFunc);
    }
    BOOL PEScanLoader::loadPEPrinter() {
        return DLL_PROC_LOAD(m_createPEPrinter, "createPEPrinter", CreatePEPrinterFunc);
    }
    BOOL PEScanLoader::loadCodeLogInfo() {
        return DLL_PROC_LOAD(m_createCodeLogInfo, "createCodeLogInfo", CreateCodeLogInfoFunc);
    }
    BOOL PEScanLoader::loadFileSearch() {
        return DLL_PROC_LOAD(m_createFileSearch, "createFileSearch", CreateFileSearchFunc);
    }
    BOOL PEScanLoader::loadRegWatch() {
        return DLL_PROC_LOAD(m_createRegWatch, "createRegWatch", CreateRegWatchFunc);
    }
    BOOL PEScanLoader::loadProcWatch() {
        return DLL_PROC_LOAD(m_createProcWatch, "createProcWatch", CreateProcWatchFunc);
    }
    BOOL PEScanLoader::loadIPCUtil() {
        return DLL_PROC_LOAD(m_createIPCUtil, "createIPCUtil", CreateIPCUtilFunc);
    }
    BOOL PEScanLoader::loadFileWatch() {
        return DLL_PROC_LOAD(m_createFileWatch, "createFileWatch", CreateFileWatch);
    }

    shared_ptr<ILogger> PEScanLoader::Logger() {
        DLL_PROC_CHECK(m_createLogger, loadLogger);

        shared_ptr<ILogger> ptr;
        ptr.reset(m_createLogger());
        return ptr;
    }

    shared_ptr<IPEParser> PEScanLoader::PEParser() {
        DLL_PROC_CHECK(m_createPEParser, loadPEParser);

        shared_ptr<IPEParser> ptr;
        ptr.reset(m_createPEParser());
        return ptr;
    }

    shared_ptr<IPEPrinter> PEScanLoader::PEPrinter() {
        DLL_PROC_CHECK(m_createPEPrinter, loadPEPrinter);

        shared_ptr<IPEPrinter> ptr;
        ptr.reset(m_createPEPrinter());
        return ptr;
    }

    shared_ptr<ILogInfo> PEScanLoader::CodeLogInfo(const TCHAR* message, int errorCode, BOOL detail, const char* __fname, const int __fline) {
        DLL_PROC_CHECK(m_createCodeLogInfo, loadCodeLogInfo);

        shared_ptr<ILogInfo> ptr;
        ptr.reset(m_createCodeLogInfo(message, errorCode, detail, __fname, __fline));
        return ptr;
    }

    shared_ptr<IFileSearch> PEScanLoader::FileSearch() {
        DLL_PROC_CHECK(m_createFileSearch, loadFileSearch);

        shared_ptr<IFileSearch> ptr;
        ptr.reset(m_createFileSearch());
        return ptr;
    }

    shared_ptr<IRegWatch> PEScanLoader::RegWatch() {
        DLL_PROC_CHECK(m_createRegWatch, loadRegWatch);

        shared_ptr<IRegWatch> ptr;
        ptr.reset(m_createRegWatch());
        return ptr;
    }

    shared_ptr<IProcWatch> PEScanLoader::ProcWatch() {
        DLL_PROC_CHECK(m_createProcWatch, loadProcWatch);

        shared_ptr<IProcWatch> ptr;
        ptr.reset(m_createProcWatch());
        return ptr;
    }
    shared_ptr<IIPCUtil> PEScanLoader::IPCUtil() {
        DLL_PROC_CHECK(m_createIPCUtil, loadIPCUtil);

        shared_ptr<IIPCUtil> ptr;
        ptr.reset(m_createIPCUtil());
        return ptr;
    }
    shared_ptr<IFileWatch> PEScanLoader::FileWatch() {
        DLL_PROC_CHECK(m_createFileWatch, loadFileWatch);

        shared_ptr<IFileWatch> ptr;
        ptr.reset(m_createFileWatch());
        return ptr;
    }

}