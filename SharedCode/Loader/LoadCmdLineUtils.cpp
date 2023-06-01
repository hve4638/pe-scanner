#include "Loader/LoadCmdLineUtils.h"

namespace CmdLineUtils {
    CmdLineUtilsLoader ::CmdLineUtilsLoader (tstring path) {
        m_module = LoadLibrary(path.c_str());
        m_loaded = (m_module != NULL);
    }

    CmdLineUtilsLoader ::~CmdLineUtilsLoader () {
        if (m_module != NULL) {
            FreeLibrary(m_module);
            m_module = NULL;
        }
     }

    void CmdLineUtilsLoader ::error(tstring message) {
        tcout << message << endl;
        abort();
    }

    BOOL CmdLineUtilsLoader ::isLoaded() {
        return m_loaded;
    }

    BOOL CmdLineUtilsLoader ::loadCmdLineParser() {
        return DLL_PROC_LOAD(m_createCmdLineParser, "createCmdLineParser", CreateCmdLineParser);
    }

    shared_ptr<ICmdLineParser> CmdLineUtilsLoader ::CmdLineParser() {
        DLL_PROC_CHECK(m_createCmdLineParser, loadCmdLineParser);
        
        shared_ptr<ICmdLineParser> ptr;
        ptr.reset(m_createCmdLineParser());
        return ptr;
    }

}