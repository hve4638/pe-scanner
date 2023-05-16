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
                BinaryData& data = regValue.value;
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
        BOOL result = FALSE;
        LSTATUS status = NULL;
        HKEY curKey = NULL;
        DWORD valueType = 0;
        DWORD valueCount = 0;
        DWORD valueLength = 0;
        DWORD valueNameLength = 0;
        DWORD maxValueLength = 0;
        DWORD maxValueNameLength = 0;
        RegValue regValue;
        BinaryData nameData;
        BinaryData valueData;
        m_logger << LogLevel::DEBUG;
        m_logger << _T("call getRegFileList()") << NL;

        for (auto const& element : regPathList) {
            auto key = reinterpret_cast<HKEY>(element.key);
            auto name = element.name.c_str();
            auto value = element.value;
            status = RegOpenKeyEx(key, name, 0, KEY_READ, &curKey);
            if (status == ERROR_SUCCESS) {
                if (value.empty()) {
                    status = RegQueryInfoKey(curKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL);
                    if (status == ERROR_SUCCESS) {
                        status = RegQueryInfoKey(curKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxValueNameLength, &maxValueLength, NULL, NULL);
                        if (status == ERROR_SUCCESS) {
                            nameData.assign((maxValueNameLength + 1) * sizeof(TCHAR), 0x0);
                            valueData.assign((maxValueLength + 1) * sizeof(TCHAR), 0x0);

                            for (DWORD index = 0; index < valueCount; index++) {
                                valueNameLength = maxValueNameLength + 1;
                                valueLength = (maxValueLength + 1) * sizeof(TCHAR);

                                status = RegEnumValue(curKey, index, reinterpret_cast<TCHAR*>(nameData.data()), &valueNameLength, NULL, &valueType, valueData.data(), &valueLength);
                                if (status == ERROR_SUCCESS) {
                                    switch (valueType)
                                    {
                                    case REG_SZ:
                                    case REG_EXPAND_SZ:
                                        regFileList.push_back(make_tuple(reinterpret_cast<TCHAR*>(valueData.data()), make_tuple(get<0>(element), reinterpret_cast<TCHAR*>(nameData.data()), get<2>(element))));
                                        result = TRUE;
                                        break;
                                    case REG_MULTI_SZ:
                                        const TCHAR* data = reinterpret_cast<const TCHAR*>(valueData.data());
                                        while (*data) {

                                            regFileList.push_back(make_tuple(data, make_tuple(get<0>(element), reinterpret_cast<TCHAR*>(nameData.data()), get<2>(element))));
                                            data += wcslen(data) + 1;
                                        }
                                        result = TRUE;
                                        break;
                                    }
                                }
                                else
                                {
                                    m_logger << LogLevel::ERR;
                                    m_logger << ErrorLogInfo(_T("Error : enum value"), status) << NL;
                                }
                                memset(nameData.data(), 0x0, nameData.size());
                                memset(valueData.data(), 0x0, valueData.size());
                            }
                            nameData.clear();
                            valueData.clear();
                        }
                    }
                }
                else
                {
                    status = RegQueryValueEx(curKey, get<1>(element).c_str(), NULL, NULL, NULL, &valueLength);
                    if (status == ERROR_SUCCESS)
                    {
                        DWORD& regType = get<0>(regValue);
                        BinaryData& data = get<1>(regValue);

                        valueLength += sizeof(TCHAR) * 2; // RegGetValue() 함수는  NULL 문자를 포함하여 리턴하기 때문에 NULL 문자 두 개 크기 만큼 버퍼를 할당
                        status = RegGetValue(curKey, NULL, get<1>(element).c_str(), RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_RT_REG_MULTI_SZ, &regType, data.data(), &valueLength);
                        if (status == ERROR_SUCCESS)
                        {
                            regFileList.push_back(make_tuple(reinterpret_cast<TCHAR*>(data.data()), element));
                            result = TRUE;
                        }
                        else
                        {
                            m_logger.log(_T("Error : get value"), status, LOG_LEVEL_ERROR);
                        }
                        data.clear();
                    }
                    else
                    {
                        m_logger.log(_T("Error : query value"), status, LOG_LEVEL_ERROR);
                    }
                }
                RegCloseKey(curKey);
            }
        }
        return result;
    }
    BOOL RegUtil::setRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue, BOOL removeExistValue = FALSE, BOOL createKey = FALSE);
    BOOL RegUtil::deleteRegKey(RegRootKey regRootKey, tstring keyName);
    BOOL RegUtil::deleteRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName);
}
