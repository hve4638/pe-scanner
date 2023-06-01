#pragma once
#include "Common/IProcWatch.h"
#include "Logger.h"

namespace PEScan {
    class ProcWatch : public IProcWatch {
        Logger m_logger;
        ProcessInfoList m_processInfoList;

    public:
        BOOL isWatching = FALSE;

    private:
        void watchProcessStart(ProcWatch* processWatch);

    public:
        ProcWatch();
        ~ProcWatch();
        BOOL isAlive(void);
        void startMonitoring(void);
        void stopMonitoring(void);
        void processCallback(DWORD pid, tstring processName, tstring processPath, tstring processCmdLine);
        BOOL getNextInfo(ProcessInfo& processInfo);
    };

};
