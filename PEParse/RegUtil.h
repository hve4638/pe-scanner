#pragma once

#include "typedef.h"
#include "RegInfo.h"
#include "Logger.h"

using namespace LogUtils;

namespace RegUtils {
    class RegUtil {
        Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };

        //RegFileInfo getFileInfo(RegPathInfo& regPathList);
        SIZE_T addRegFiles(const RegPathInfo& regPath, vector<RegFileInfo>& regFiles);
    public:
        RegUtil();
        ~RegUtil();
        BOOL getRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue);
        BOOL getRegFileList(IN vector<RegPathInfo>& regPathList, OUT vector<RegFileInfo>& regFileList);
        BOOL setRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue, BOOL removeExistValue = FALSE, BOOL createKey = FALSE);
        BOOL deleteRegKey(RegRootKey regRootKey, tstring keyName);
        BOOL deleteRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName);
    };
}
