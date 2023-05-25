#pragma once

#include "typedef.h"
#include "Logger.h"
#include "RegInfo.h"
using namespace LogUtils;
using namespace RegInfo;

namespace WatchUtils {
    class CProcessWatch {
    private:
        Logger m_logger;
        ProcessInfoList m_processInfoList;

    public:
        BOOL isWatching = FALSE;

    private:
        void watchProcessStart(CProcessWatch* processWatch);

    public:
        CProcessWatch();
        ~CProcessWatch();
        void startMonitoring(void);
        void stopMonitoring(void);
        void processCallback(DWORD pid, tstring processName, tstring processPath, tstring processCmdLine);
        BOOL getNextProcessInfo(ProcessInfo& processInfo);
    };

};
