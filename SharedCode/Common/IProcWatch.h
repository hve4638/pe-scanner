#pragma once
#include "Common/ProcTypedef.h"

namespace PEScan {
    interface IProcWatch {
        virtual ~IProcWatch() {}

        virtual BOOL isAlive(void) abstract;
        virtual void startMonitoring(void) abstract;
        virtual void stopMonitoring(void) abstract;
        virtual void processCallback(DWORD pid, tstring processName, tstring processPath, tstring processCmdLine) abstract;
        virtual BOOL getNextInfo(ProcessInfo& processInfo) abstract;
    };
};
