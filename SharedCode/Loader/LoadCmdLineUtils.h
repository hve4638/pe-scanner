#pragma once
//#include <Windows.h>
#include "Common/typedef.h"
#include "Common/ICmdLineParser.h"


namespace CmdLineUtils {
    typedef ICmdLineParser*(*CreateCmdLineParser)();
    
    class CmdLineUtilsLoader {
        HMODULE m_module = NULL;
        BOOL m_loaded = FALSE;

        CreateCmdLineParser m_createCmdLineParser = NULL;

        void error(tstring message = _T("PEScan : error occured"));
    public:
        CmdLineUtilsLoader(tstring path);
        ~CmdLineUtilsLoader();

        BOOL isLoaded();

        BOOL loadCmdLineParser();

        shared_ptr<ICmdLineParser> CmdLineParser();
    };
}