#include "RegUtil.h"

namespace RegUtils {
    RegUtil::RegUtil() {

    }
    RegUtil::~RegUtil() {

    }

    BOOL RegUtil::getRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue) {
        LSTATUS status = NULL;
        HKEY curKey = NULL;

        m_logger << LogLevel::DEBUG;
        m_logger << _T("call getRegValue()") << NL;

        status = RegOpenKeyEx(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), 0, KEY_READ, &curKey);
        if (status == ERROR_SUCCESS) {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error : open key"), status) << NL;
            return FALSE;
        }
        else {
            BOOL result = FALSE;
            DWORD valueLength = 0;
            status = RegQueryValueEx(curKey, valueName.c_str(), NULL, NULL, NULL, &valueLength);
            if (status != ERROR_SUCCESS) {
                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo(_T("Error : querying value info"), status) << NL;
            }
            else {
                DWORD& regType = regValue.valueType;
                BinaryData& data = regValue.data;
                // RegGetValue() 함수는  NULL 문자를 포함하여 리턴하기 때문에 NULL 문자 두 개 크기 만큼 버퍼를 할당
                valueLength += sizeof(TCHAR) * 2;
                status = RegGetValue(curKey, NULL, valueName.c_str(), RRF_RT_ANY, &regType, data.data(), &valueLength);
                if (status != ERROR_SUCCESS) {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Error : get value"), status) << NL;
                    result = FALSE;
                }
                else {
                    result = TRUE;
                }
            }
            RegCloseKey(curKey);
            return result;
        }
    };

    BOOL RegUtil::getRegFileList(IN vector<RegPathInfo>& regPathList, OUT vector<RegFileInfo>& regFileList) {
        SIZE_T count = 0;
        m_logger << LogLevel::DEBUG;
        m_logger << _T("call getRegFileList()") << NL;

        for (auto const& element : regPathList) {
            count += addRegFiles(element, regFileList);
        }

        return count > 0;
    }
    //BOOL RegUtil::setRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue, BOOL removeExistValue = FALSE, BOOL createKey = FALSE);
    //BOOL RegUtil::deleteRegKey(RegRootKey regRootKey, tstring keyName);
    //BOOL RegUtil::deleteRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName);
}
