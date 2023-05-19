#pragma once
#include "typedef.h"
#include "RegInfo.h"
#include "Logger.h"
using namespace LogUtils;
using namespace RegInfo;

namespace WatchUtils {
    class RegWatch {
        Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };
        RegFileInfoList m_regFileInfoList;

    public:
        BOOL isWatching = FALSE;
        RegRootKey m_regRootKey;
        tstring m_curRegKeyPath;
        HKEY m_rootKey = NULL;
        HKEY m_curKey = NULL;

    private:
        void watchRegStart(RegWatch* regWatch);

    public:
        RegWatch();
        ~RegWatch();
        void startMonitoring(RegRootKey rootKey, tstring subKey);
        void stopMonitoring(void);
        void regCallback(tstring filePath, RegPathInfo regPathInfo);
        BOOL getNextRegFileInfo(RegFileInfo& regFileInfo);
    };

};