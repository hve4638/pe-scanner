#pragma once
#include "RegWatch.h"
#include <map>
#include <thread>
#include <chrono>
#include <filesystem>

using namespace std;

namespace PEScan {

    RegWatch::RegWatch()
    {
    };

    RegWatch::~RegWatch(void)
    {
        stopMonitoring();
    };

    BOOL RegWatch::isAlive(void)
    {
        return m_isWatching;
    };

    void RegWatch::watchRegStart(RegWatch* regWatch)
    {
        if ((regWatch != NULL) && (regWatch->m_curKey != NULL))
        {
            DWORD filter = REG_LEGAL_CHANGE_FILTER;
            HANDLE eventHandle = NULL;
            LONG errorCode = 0;
            NTSTATUS status = 0;
            DWORD waitObject = 0;
            DWORD valueCount = 0;
            DWORD valueType = 0;
            DWORD valueNameLength = 0;
            DWORD valueLength = 0;
            DWORD maxValueNameLength = 0;
            DWORD maxValueLength = 0;
            tstring nameString;
            tstring dataString;
            BinaryData nameData;
            BinaryData valueData;
            map<tstring, BOOL> m_regValue;
            map<tstring, BOOL> m_regValueExistCheck;

            // Create an event.
            eventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (eventHandle == NULL)
            {
                m_logger.log(_T("Error in CreateEvent."), GetLastError(), LOG_LEVEL_ERROR);
            }
            else
            {
                while (regWatch->m_isWatching)
                {
                    // Watch the registry key for a change of value.
                    errorCode = RegNotifyChangeKeyValue(regWatch->m_curKey, TRUE, filter, eventHandle, TRUE);
                    if (errorCode != ERROR_SUCCESS)
                    {
                        m_logger.log(_T("Error in RegNotifyChangeKeyValue."), errorCode, LOG_LEVEL_ERROR);
                    }
                    else
                    {
                        // Wait for an event to occur. 1 second
                        waitObject = WaitForSingleObject(eventHandle, 1000);
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
                            valueCount = 0;
                            valueType = 0;
                            valueNameLength = 0;
                            valueLength = 0;
                            maxValueNameLength = 0;
                            maxValueLength = 0;
                            nameString.clear();
                            dataString.clear();
                            nameData.clear();
                            valueData.clear();

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
                                                nameString = reinterpret_cast<TCHAR*>(nameData.data());
                                                if (nameString.empty())
                                                {
                                                    // (기본 값)
                                                    nameString = _T("(default)");
                                                }

                                                if ((nameString.size() > 0) && (m_regValue.find(nameString) == m_regValue.end()))
                                                {
                                                    RegPathInfo regPathInfo(regWatch->m_curRegKeyPath, nameString, regWatch->m_regRootKey);

                                                    switch (valueType)
                                                    {
                                                    case REG_SZ:
                                                    case REG_EXPAND_SZ:
                                                    {
                                                        filesystem::path filePath(reinterpret_cast<TCHAR*>(valueData.data()));
                                                        dataString = filePath.c_str();
                                                        regWatch->regCallback(dataString, regPathInfo);
                                                        break;
                                                    }
                                                    case REG_MULTI_SZ:
                                                    {
                                                        const TCHAR* data = reinterpret_cast<const TCHAR*>(valueData.data());
                                                        while (*data)
                                                        {
                                                            filesystem::path filePath(data);
                                                            dataString = filePath.c_str();
                                                            regWatch->regCallback(filePath, regPathInfo);
                                                            data += wcslen(data) + 1;
                                                        }
                                                        break;
                                                    }
                                                    }
                                                }

                                                // 현재 존재하는 항목들 이름 저장
                                                m_regValueExistCheck.insert(make_pair(nameString, TRUE));
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
                    // 현재 존재하는 항목들로 m_regValue를 관리
                    m_regValue.clear();
                    swap(m_regValueExistCheck, m_regValue);

                    // 1000 ns == 0.001 ms
                    this_thread::sleep_for(std::chrono::nanoseconds(1000));
                }

                m_logger << LogLevel::DEBUG;
                m_logger << _T("EXIT RegWatchLoop") << NL;

                // Close the handle.
                if (!CloseHandle(eventHandle))
                {
                    m_logger.log(_T("Error in CloseHandle."), GetLastError(), LOG_LEVEL_ERROR);
                }
            }
        }
        ExitThread(0);
    };

    void RegWatch::regCallback(tstring filePath, RegPathInfo regPathInfo)
    {
        if (!filePath.empty())
        {
            m_regFileInfoList.push(make_tuple(filePath, get<0>(regPathInfo), get<1>(regPathInfo), get<2>(regPathInfo)));
        }
    };

    void RegWatch::startMonitoring(RegRootKey rootKey, tstring subKey)
    {
        if (m_isWatching)
        {
            stopMonitoring();
        }
        m_isWatching = TRUE;
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
        if (m_isWatching)
        {
            m_isWatching = FALSE;
            Sleep(100);
        }
        if (m_curKey != NULL)
        {
            RegCloseKey(m_curKey);
            m_curKey = NULL;
        }
    };

    BOOL RegWatch::getNextInfo(RegFileInfo& regFileInfo)
    {
        BOOL result = !m_regFileInfoList.empty();

        if (result)
        {
            RegFileInfo& regFileInfoFirst = m_regFileInfoList.front();
            regFileInfo = make_tuple(get<0>(regFileInfoFirst), get<1>(regFileInfoFirst), get<2>(regFileInfoFirst), get<3>(regFileInfoFirst));
            m_regFileInfoList.pop();
        }
        return result;
    };

};