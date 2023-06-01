#pragma once
#include "Common/typedef.h"
#include "Common/RegTypedef.h"

namespace PEScan {
    interface IRegWatch {
        virtual ~IRegWatch() {}
        virtual BOOL isAlive() abstract;
        virtual void startMonitoring(RegRootKey rootKey, tstring subKey) abstract;
        virtual void stopMonitoring() abstract;
        virtual void regCallback(tstring filePath, RegPathInfo regPathInfo) abstract;
        virtual BOOL getNextInfo(RegFileInfo& regFileInfo) abstract;
    };
};