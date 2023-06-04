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
    extern "C" PESCAN_API ILogger* createLogger();          // �ΰ� Ŭ����

    extern "C" PESCAN_API ILogInfo* createCodeLogInfo(      // �ΰſ��� ����ϴ� ������� Ŭ����
        const TCHAR*, int, BOOL, const char*, const int     // LastError, __builtInFunc �� ���� ����
    );

    extern "C" PESCAN_API IPEParser* createPEParser();      // PE�Ľ� Ŭ����
    extern "C" PESCAN_API IPEPrinter* createPEPrinter();    // PE����ü ��� Ŭ����

    extern "C" PESCAN_API IFileSearch* createFileSearch();  // ����Ž�� Ŭ����
    extern "C" PESCAN_API IRegWatch* createRegWatch();      // ������Ʈ�� ���� Ŭ����
    extern "C" PESCAN_API IProcWatch* createProcWatch();    // ���μ��� ���� Ŭ����
    extern "C" PESCAN_API IFileWatch * createFileWatch();   // ���� ���� Ŭ����

    extern "C" PESCAN_API IIPCUtil * createIPCUtil();        // IPC ��ƿ Ŭ����
}



