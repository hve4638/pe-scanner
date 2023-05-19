#pragma once

#include <map>
#include <format>
#include <filesystem>
#include "RegWatch.h"

using namespace std;

namespace WatchUtils {

    RegWatch::RegWatch()
    {
    };

    RegWatch::~RegWatch(void)
    {
        stopMonitoring();
    };

    void RegWatch::watchRegStart(RegWatch* regWatch)
    {
        if ((regWatch != NULL) && (regWatch->m_curKey != NULL))
        {
            DWORD  dwFilter = REG_LEGAL_CHANGE_FILTER;
            HANDLE hEvent = NULL;
            LONG lErrorCode = 0;
            NTSTATUS status = 0;
            map<tstring, BOOL> m_regValue;

            // Create an event.
            hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (hEvent == NULL)
            {
                m_logger.log(_T("Error in CreateEvent."), GetLastError(), LOG_LEVEL_ERROR);
            }
            else
            {
                while (regWatch->isWatching)
                {
                    // Watch the registry key for a change of value.
                    lErrorCode = RegNotifyChangeKeyValue(regWatch->m_curKey, TRUE, dwFilter, hEvent, TRUE);
                    if (lErrorCode != ERROR_SUCCESS)
                    {
                        m_logger.log(_T("Error in RegNotifyChangeKeyValue."), lErrorCode, LOG_LEVEL_ERROR);
                    }
                    else
                    {
                        // Wait for an event to occur. 1 second
                        DWORD waitObject = WaitForSingleObject(hEvent, 1000);
                        if (waitObject == WAIT_FAILED)
                        {
                            m_logger.log(_T("Error in WaitForSingleObject."), GetLastError(), LOG_LEVEL_ERROR);
                            break;
                        }
                        else if (waitObject == WAIT_TIMEOUT)
                        {
                            continue;
                        }
                        else
                        {
                            // Registry event occurs.
                            DWORD valueCount = 0;
                            DWORD valueType = 0;
                            DWORD valueNameLength = 0;
                            DWORD valueLength = 0;
                            DWORD maxValueNameLength = 0;
                            DWORD maxValueLength = 0;
                            tstring dataString;
                            BinaryData nameData;
                            BinaryData valueData;

                            status = RegQueryInfoKey(regWatch->m_curKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL);
                            if (status == ERROR_SUCCESS)
                            {
                                status = RegQueryInfoKey(regWatch->m_curKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxValueNameLength, &maxValueLength, NULL, NULL);
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

                                        status = RegEnumValue(regWatch->m_curKey, index, reinterpret_cast<TCHAR*>(nameData.data()), &valueNameLength, NULL, &valueType, valueData.data(), &valueLength);
                                        if (status == ERROR_SUCCESS)
                                        {
                                            if ((valueType == REG_SZ) || (valueType == REG_EXPAND_SZ) || (valueType == REG_MULTI_SZ))
                                            {
                                                dataString = reinterpret_cast<TCHAR*>(nameData.data());
                                                if ((dataString.size() > 0) && (m_regValue.find(dataString) == m_regValue.end()))
                                                {
                                                    m_regValue.insert(make_pair(dataString, TRUE));
                                                    RegPathInfo regPathInfo(regWatch->m_curRegKeyPath, dataString, regWatch->m_regRootKey);

                                                    switch (valueType)
                                                    {
                                                    case REG_SZ:
                                                    case REG_EXPAND_SZ:
                                                    {
                                                        filesystem::path filePath(reinterpret_cast<TCHAR*>(valueData.data()));
                                                        regWatch->regCallback(filePath.c_str(), regPathInfo);
                                                        break;
                                                    }
                                                    case REG_MULTI_SZ:
                                                    {
                                                        const TCHAR* data = reinterpret_cast<const TCHAR*>(valueData.data());
                                                        filesystem::path filePath(data);
                                                        while (*data)
                                                        {
                                                            regWatch->regCallback(filePath.c_str(), regPathInfo);
                                                            data += wcslen(data) + 1;
                                                        }
                                                        break;
                                                    }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                m_logger.log(_T("Error : invalid value type"), status, LOG_LEVEL_DEBUG);
                                            }
                                        }
                                        else
                                        {
                                            m_logger.log(_T("Error : enum value"), status, LOG_LEVEL_ERROR);
                                        }
                                        memset(nameData.data(), 0x0, nameData.size());
                                        memset(valueData.data(), 0x0, valueData.size());
                                    }
                                    nameData.clear();
                                    valueData.clear();
                                }
                            }
                        }
                    }
                }

                // Close the handle.
                if (!CloseHandle(hEvent))
                {
                    m_logger.log(_T("Error in CloseHandle."), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
        }
    };

    void RegWatch::regCallback(tstring filePath, RegPathInfo regPathInfo)
    {
        // if (filesystem::is_regular_file(filesystem::absolute(filePath)))
        if (!filePath.empty())
        {
            m_regFileInfoList.push(make_tuple(filePath, regPathInfo));
        }
    };

    void RegWatch::startMonitoring(RegRootKey rootKey, tstring subKey)
    {
        if (isWatching)
        {
            stopMonitoring();
        }
        isWatching = TRUE;
        m_regRootKey = rootKey;
        m_curRegKeyPath = subKey;
        m_rootKey = reinterpret_cast<HKEY>(rootKey);
        if (!subKey.empty())
        {
            NTSTATUS status = RegOpenKeyEx(m_rootKey, subKey.c_str(), 0, KEY_READ, &m_curKey);
            if (status == ERROR_SUCCESS)
            {
                thread regWatchThread = thread([&]() { watchRegStart(this); });
                regWatchThread.detach();
            }
            else
            {
                m_rootKey = NULL;
                m_logger.log(_T("RegOpenKeyEx fail :"), status, LOG_LEVEL_ERROR);
            }
        }
    };

    void RegWatch::stopMonitoring(void)
    {
        if (isWatching)
        {
            isWatching = FALSE;
            Sleep(1000);
        }
        if (m_curKey != NULL)
        {
            RegCloseKey(m_curKey);
            m_curKey = NULL;
        }
    };

    BOOL RegWatch::getNextRegFileInfo(RegFileInfo& regFileInfo)
    {
        BOOL result = !m_regFileInfoList.empty();

        if (result)
        {
            RegFileInfo& regFileInfoFirst = m_regFileInfoList.front();
            regFileInfo = make_tuple(get<0>(regFileInfoFirst), get<1>(regFileInfoFirst));
            m_regFileInfoList.pop();
        }
        return result;
    };

};