#pragma once

#include "typedef.h"
#include "RegInfo.h"
#include "Logger.h"

using namespace LogUtils;

namespace RegUtils {
    using namespace RegInfo;

    class RegUtil {
        Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };

    public:
        RegUtil();
        ~RegUtil();
        BOOL getRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue);
        BOOL getRegFileList(RegPathList& regPathList, RegFileList& regFileList);
        BOOL setRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue, BOOL removeExistValue = FALSE, BOOL createKey = FALSE);
        BOOL deleteRegKey(RegRootKey regRootKey, tstring keyName);
        BOOL deleteRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName);
    };
}
