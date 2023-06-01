#pragma once
#include "Common/IRegWatch.h"
#include "Logger.h"

namespace PEScan {
    class RegWatch : public IRegWatch {
        Logger m_logger = {LogLevel::ALL, LogDirection::DEBUGVIEW};
        RegFileInfoList m_regFileInfoList;
        BOOL m_isWatching = FALSE;
        RegRootKey m_regRootKey;
        tstring m_curRegKeyPath;
        HKEY m_rootKey = NULL;
        HKEY m_curKey = NULL;

        void watchRegStart(RegWatch* regWatch);

    public:
        RegWatch();
        ~RegWatch() override;
        BOOL isAlive() override;
        void startMonitoring(RegRootKey rootKey, tstring subKey) override;
        void stopMonitoring() override;
        void regCallback(tstring filePath, RegPathInfo regPathInfo) override;
        BOOL getNextInfo(RegFileInfo& regFileInfo) override;
    };
};