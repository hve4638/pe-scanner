#pragma once

#include "RegUtil.h"

using namespace std;

namespace RegUtils {

    RegUtil::RegUtil()
    {

    };

    RegUtil::~RegUtil(void)
    {
    };

    BOOL RegUtil::getRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue)
    {
        BOOL result = FALSE;
        LSTATUS status = NULL;
        HKEY curKey = NULL;
        DWORD valueLength = 0;

        status = RegOpenKeyEx(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), 0, KEY_READ, &curKey);
        if (status == ERROR_SUCCESS)
        {
            status = RegQueryValueEx(curKey, valueName.c_str(), NULL, NULL, NULL, &valueLength);
            if (status == ERROR_SUCCESS)
            {
                DWORD& regType = get<0>(regValue);
                BinaryData& data = get<1>(regValue);

                valueLength += sizeof(TCHAR) * 2; // RegGetValue() 함수는  NULL 문자를 포함하여 리턴하기 때문에 NULL 문자 두 개 크기 만큼 버퍼를 할당
                status = RegGetValue(curKey, NULL, valueName.c_str(), RRF_RT_ANY, &regType, data.data(), &valueLength);
                if (status == ERROR_SUCCESS)
                {
                    result = TRUE;
                }
                else
                {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Error : get value"), status) << NL;

                }
            }
            else
            {
                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo(_T("Error : querying value info"), status) << NL;
            }
            RegCloseKey(curKey);
        }
        else
        {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error : open key"), status) << NL;
        }
        return result;
    };

    BOOL RegUtil::setRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName, RegValue& regValue, BOOL removeExistValue, BOOL createKey)
    {
        BOOL result = FALSE;
        LSTATUS status = NULL;
        HKEY curKey = NULL;
        DWORD valueLength = 0;
        DWORD& regType = get<0>(regValue);
        BinaryData& data = get<1>(regValue);

        status = RegOpenKeyEx(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), 0, KEY_READ | KEY_WRITE, &curKey);
        if (status == ERROR_FILE_NOT_FOUND)
        {
            if (createKey)
            {
                status = RegSetKeyValue(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), valueName.c_str(), regType, data.data(), static_cast<DWORD>(data.size()));
                if (status == ERROR_SUCCESS)
                {
                    result = TRUE;
                }
                else
                {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Error : RegSetKeyValue fail"), status) << NL;
                }
            }
        }
        else if (status == ERROR_SUCCESS)
        {
            status = RegQueryValueEx(curKey, valueName.c_str(), NULL, NULL, NULL, &valueLength);
            if (status == ERROR_SUCCESS)
            {
                if (removeExistValue)
                {
                    status = RegDeleteValue(curKey, valueName.c_str());
                    if (status == ERROR_SUCCESS)
                    {
                        status = ERROR_FILE_NOT_FOUND;
                    }
                }
            }
            if (status == ERROR_FILE_NOT_FOUND)
            {
                status = RegSetValueEx(curKey, valueName.c_str(), NULL, regType, data.data(), static_cast<DWORD>(data.size()));
                if (status == ERROR_SUCCESS)
                {
                    result = TRUE;
                }
                else
                {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Error : set value"), status) << NL;
                }
            }
            else
            {
                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo(_T("Error : set value(value name alrady exist)"), status) << NL;
            }
            RegCloseKey(curKey);
        }
        else
        {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error : open key"), status) << NL;
        }
        return result;
    };

    BOOL RegUtil::deleteRegKey(RegRootKey regRootKey, tstring keyName)
    {
        BOOL result = FALSE;
        LSTATUS status = NULL;

        status = RegDeleteKeyEx(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), KEY_WOW64_64KEY, NULL);
        if (status == ERROR_SUCCESS)
        {
            result = TRUE;
        }
        else
        {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error : delete key"), status) << NL;
        }
        return result;
    };

    BOOL RegUtil::deleteRegValue(RegRootKey regRootKey, tstring keyName, tstring valueName)
    {
        BOOL result = FALSE;
        LSTATUS status = NULL;
        HKEY curKey = NULL;
        DWORD valueLength = 0;

        status = RegOpenKeyEx(reinterpret_cast<HKEY>(regRootKey), keyName.c_str(), 0, KEY_READ | KEY_WRITE | DELETE, &curKey);
        if (status == ERROR_SUCCESS)
        {
            status = RegDeleteValue(curKey, valueName.c_str());
            if (status == ERROR_SUCCESS)
            {
                result = TRUE;
            }
            else
            {
                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo(_T("Error : delete value"), status) << NL;
            }
            RegCloseKey(curKey);
        }
        else
        {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error : open key"), status) << NL;
        }
        return result;
    };

    BOOL RegUtil::getRegFileList(RegPathList& regPathList, RegFileList& regFileList)
    {
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

        for (auto const& element : regPathList)
        {
            status = RegOpenKeyEx(reinterpret_cast<HKEY>(get<2>(element)), get<0>(element).c_str(), 0, KEY_READ, &curKey);
            if (status == ERROR_SUCCESS)
            {
                if (get<1>(element).empty())
                {
                    status = RegQueryInfoKey(curKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL);
                    if (status == ERROR_SUCCESS)
                    {
                        status = RegQueryInfoKey(curKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxValueNameLength, &maxValueLength, NULL, NULL);
                        if (status == ERROR_SUCCESS)
                        {
                            // valueNameLength 는 문자 갯수
                            nameData.assign((maxValueNameLength + 1) * sizeof(TCHAR), 0x0);

                            // valueLength 는 바이트 수
                            valueData.assign((maxValueLength + 1) * sizeof(TCHAR), 0x0);

                            for (DWORD index = 0; index < valueCount; index++)
                            {
                                // valueNameLength 는 문자 갯수
                                valueNameLength = maxValueNameLength + 1;

                                // valueLength 는 바이트 수
                                valueLength = (maxValueLength + 1) * sizeof(TCHAR);

                                status = RegEnumValue(curKey, index, reinterpret_cast<TCHAR*>(nameData.data()), &valueNameLength, NULL, &valueType, valueData.data(), &valueLength);
                                if (status == ERROR_SUCCESS)
                                {
                                    switch (valueType)
                                    {
                                    case REG_SZ:
                                    case REG_EXPAND_SZ:
                                        regFileList.push_back(make_tuple(reinterpret_cast<TCHAR*>(valueData.data()), make_tuple(get<0>(element), reinterpret_cast<TCHAR*>(nameData.data()), get<2>(element))));
                                        result = TRUE;
                                        break;
                                    case REG_MULTI_SZ:
                                        const TCHAR* data = reinterpret_cast<const TCHAR*>(valueData.data());
                                        while (*data)
                                        {
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
                            m_logger << LogLevel::ERR;
                            m_logger << ErrorLogInfo(_T("Error : get value"), status) << NL;
                        }
                        data.clear();
                    }
                    else
                    {
                        m_logger << LogLevel::ERR;
                        m_logger << ErrorLogInfo(_T("Error : query value"), status) << NL;
                    }
                }
                RegCloseKey(curKey);
            }
        }
        return result;
    };

    /*
    void CRegUtil::printRegValue(HKEY hKey)
    {
        DWORD valueCount;
        LSTATUS result = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL);
        if (result != ERROR_SUCCESS) {
            std::wcout << L"Error querying key info" << std::endl;
            RegCloseKey(hKey);
            return;
        }

        DWORD maxValueNameLen, maxValueLen;
        result = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxValueNameLen, &maxValueLen, NULL, NULL);
        if (result != ERROR_SUCCESS) {
            std::wcout << L"Error querying value info" << std::endl;
            RegCloseKey(hKey);
            return;
        }

        maxValueNameLen++; // add one for null terminator
        wchar_t* valueNameBuffer = new wchar_t[maxValueNameLen + 1];
        BYTE* valueBuffer = new BYTE[maxValueLen + sizeof(wchar_t)];
        std::wstring valueName;

        memset((PBYTE)valueNameBuffer, 0, (maxValueNameLen + 1) * sizeof(wchar_t));
        memset((PBYTE)valueBuffer, 0, maxValueLen + sizeof(wchar_t));

        for (DWORD i = 0; i < valueCount; i++)
        {
            DWORD valueNameLen = maxValueNameLen;
            DWORD valueType, valueLen = maxValueLen;
            result = RegEnumValue(hKey, i, valueNameBuffer, &valueNameLen, NULL, &valueType, valueBuffer, &valueLen);
            if (result != ERROR_SUCCESS) {
                std::wcout << L"Error enumerating values" << std::endl;
                continue;
            }
            valueName = valueNameBuffer;
            std::wcout << valueName << L" : ";

            DWORD dwValue = 0;
            ULONGLONG qValue = 0;
            const wchar_t* p = NULL;

            switch (valueType)
            {
            case REG_DWORD:
                std::wcout << L"DWORD data: ";
                dwValue = *(DWORD*)valueBuffer;
                std::wcout << dwValue << std::endl;
                break;
            case REG_SZ:
                std::wcout << L"String data: ";
                std::wcout << (wchar_t*)valueBuffer << std::endl;
                break;
            case REG_BINARY:
                std::wcout << L"Binary data: ";
                for (DWORD j = 0; j < valueLen; j++) {
                    std::wcout << std::hex << (int)valueBuffer[j];
                }
                std::wcout << std::endl;
                break;
            case REG_QWORD:
                std::wcout << L"QWORD data: ";
                qValue = *(ULONGLONG*)valueBuffer;
                std::wcout << qValue << std::endl;
                break;
            case REG_MULTI_SZ:
                std::wcout << L"Multi string data:" << std::endl;
                p = (const wchar_t*)valueBuffer;
                while (*p) {
                    std::wcout << L"  " << p << std::endl;
                    p += wcslen(p) + 1;
                }
                break;
            case REG_EXPAND_SZ:
                std::wcout << L"Expand string data:" << std::endl;
                std::wcout << (wchar_t*)valueBuffer << std::endl;
                break;
            default:
                std::wcout << L"Unsupported value type" << std::endl;
            }
        }

        delete[] valueNameBuffer;
        delete[] valueBuffer;
    }

    void CRegUtil::traverseRegistry(HKEY hKey, tstring path)
    {
        DWORD index = 0;
        while (true) {
            wchar_t subKeyName[MAX_PATH * 4];
            DWORD subKeyNameLength = MAX_PATH * 4;
            LONG result = RegEnumKeyEx(hKey, index++, subKeyName, &subKeyNameLength, NULL, NULL, NULL, NULL);
            if (result != ERROR_SUCCESS) {
                break;
            }

            HKEY subKey;
            wstring keyString;
            if (path.empty())
            {
                keyString = subKeyName;
            }
            else
            {
                keyString = path + _T("\\") + subKeyName;
            }
            result = RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &subKey);
            if (result == ERROR_SUCCESS) {
                traverseRegistry(subKey, keyString);
                RegCloseKey(subKey);
            }
        }
        printRegValue(hKey);
    }
    */
}

