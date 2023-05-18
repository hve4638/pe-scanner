#include "RegUtil.h"
#include "ReserveCloseRegKey.h"

namespace RegUtils {
    SIZE_T RegUtil::addRegFiles(const RegPathInfo& regPath, vector<RegFileInfo>& regFiles) {
        SIZE_T count = 0;
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

        // 검사할 레지스트리 경로 목록을 위한 형식 정의 (레지스트리 Key 이름, 레지스트리 Value 이름(optional), 레지스트리 루트 키)
        //typedef tuple<tstring, tstring, RegRootKey> RegPathInfo; (element)
        // 레지스트리에 존재하는 파일 경로를 추출하여 저장하기 위한 형식 정의 (파일 경로, (레지스트리 Key 이름, 레지스트리 Value 이름, 레지스트리 루트 키))
        // typedef tuple<tstring, tuple<tstring, tstring, RegRootKey>> RegFileInfo;

        auto element = regPath;
        auto rootKey = reinterpret_cast<HKEY>(element.rootKey);
        auto keyName = element.keyName.c_str();
        auto value = element.valueName;
        auto valueName = element.valueName.c_str();
        status = RegOpenKeyEx(rootKey, keyName, 0, KEY_READ, &curKey);
        if (status == ERROR_SUCCESS) {
            ReserveCloseRegKey reserveClose = curKey;

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
                                switch (valueType) {
                                case REG_SZ:
                                case REG_EXPAND_SZ:
                                {
                                    RegFileInfo info;
                                    info.path = reinterpret_cast<TCHAR*>(valueData.data());
                                    info.info.keyName = keyName;
                                    info.info.valueName = reinterpret_cast<TCHAR*>(nameData.data());
                                    info.info.rootKey = element.rootKey;

                                    regFiles.push_back(info);
                                    count++;
                                    break;

                                }
                                case REG_MULTI_SZ:
                                    const TCHAR* data = reinterpret_cast<const TCHAR*>(valueData.data());
                                    while (*data) {
                                        RegFileInfo info;
                                        info.path = reinterpret_cast<TCHAR*>(valueData.data());
                                        info.info.keyName = keyName;
                                        info.info.valueName = reinterpret_cast<TCHAR*>(nameData.data());
                                        info.info.rootKey = element.rootKey;

                                        regFiles.push_back(info);
                                        count++;
                                        data += wcslen(data) + 1;
                                    }
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
            else {
                status = RegQueryValueEx(curKey, keyName, NULL, NULL, NULL, &valueLength);
                if (status == ERROR_SUCCESS) {
                    DWORD& regType = regValue.valueType;
                    BinaryData& data = regValue.data;

                    valueLength += sizeof(TCHAR) * 2; // RegGetValue() 함수는  NULL 문자를 포함하여 리턴하기 때문에 NULL 문자 두 개 크기 만큼 버퍼를 할당
                    status = RegGetValue(curKey, NULL, valueName, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_RT_REG_MULTI_SZ, &regType, data.data(), &valueLength);
                    if (status == ERROR_SUCCESS) {
                        RegFileInfo info;
                        info.path = reinterpret_cast<TCHAR*>(valueData.data());
                        info.info = element;

                        regFiles.push_back(info);
                        count++;
                    }
                    else {
                        m_logger << LogLevel::ERR;
                        m_logger << ErrorLogInfo(_T("Error : get value"), status) << NL;
                    }
                    data.clear();
                }
                else {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Error : query value"), status) << NL;
                }
            }
        }

        return count;
    }
}