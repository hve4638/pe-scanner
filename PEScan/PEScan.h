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
    extern "C" PESCAN_API ILogger* createLogger();          // 로거 클래스

    extern "C" PESCAN_API ILogInfo* createCodeLogInfo(      // 로거에서 사용하는 출력위한 클래스
        const TCHAR*, int, BOOL, const char*, const int     // LastError, __builtInFunc 등 정보 포함
    );

    extern "C" PESCAN_API IPEParser* createPEParser();      // PE파싱 클래스
    extern "C" PESCAN_API IPEPrinter* createPEPrinter();    // PE구조체 출력 클래스

    extern "C" PESCAN_API IFileSearch* createFileSearch();  // 파일탐색 클래스
    extern "C" PESCAN_API IRegWatch* createRegWatch();      // 레지스트리 감시 클래스
    extern "C" PESCAN_API IProcWatch* createProcWatch();    // 프로세스 감시 클래스
    extern "C" PESCAN_API IFileWatch * createFileWatch();   // 파일 감시 클래스

    extern "C" PESCAN_API IIPCUtil * createIPCUtil();        // IPC 유틸 클래스
}



