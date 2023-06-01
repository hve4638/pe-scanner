#pragma once
#include "PEProcessReader.h"
#include <iostream>
#include <format>

using namespace std;

namespace PEScan {
    PEProcessReader::PEProcessReader()
    {
        HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));
        if (hModule != NULL)
        {
            m_pNtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
            if (m_pNtQueryInformationProcess != NULL)
            {
                m_logger << _T("Get NtQueryInformationProcess address fail.}") << NL;
            }
        }
        m_logger.setLogDirection(LogDirection::DEBUGVIEW);
        m_logger.setLogLevel(LogLevel::ERR);
    };

    PEProcessReader::~PEProcessReader(void)
    {
        close();
    };

    void PEProcessReader::close(void)
    {
        // 핸들 초기화
        if (m_threadHandle != NULL)
        {
            CloseHandle(m_threadHandle);
            m_threadHandle = NULL;
        }
        if (m_processHandle != NULL)
        {
            CloseHandle(m_processHandle);
            m_processHandle = NULL;
        }

        // 변수 초기화
        m_processId = NULL;
        m_peBaseAddress = NULL;
        memset(&m_peb, 0, sizeof(PEB));
        m_peFilePath.clear();
    };

    BOOL PEProcessReader::open(DWORD pid, const TCHAR* pfilePath, BOOL checkPE)
    {
        BOOL result = FALSE;

        if (pid > 0x4)
        {
            m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (m_processHandle != NULL)
            {
                result = parseImageBaseAddress();
                if (result)
                {
                    m_processId = pid;
                }
                else
                {
                    // Image Base Address를 얻지 못한 경우 실패로 처리(변수 초기화)
                    close();
                    m_logger.log(_T("Open process fail : get image base address fail"), LOG_LEVEL_ERROR);
                }
            }
            else
            {
                m_logger.log(_T("Open process fail"), GetLastError(), LOG_LEVEL_ERROR);
            }
        }
        return result;
    };

    LPVOID PEProcessReader::getBaseAddress(void)
    {
        return m_peBaseAddress;
    };

    HANDLE PEProcessReader::getHandle(void)
    {
        return m_processHandle;
    };

    tstring PEProcessReader::getFilePath(void)
    {
        return m_peFilePath;
    };

    BOOL PEProcessReader::parseImageBaseAddress(void)
    {
        BOOL result = FALSE;
        NTSTATUS status = 0;
        HANDLE procHeap = NULL;
        SIZE_T readData = 0;
        PROCESS_BASIC_INFORMATION processBasicInformation = { 0, };
        BYTE moduleNameBuffer[MAX_PATH * 4] = { 0, };
        PEB_LDR_DATA pebLdrData = { 0, };
        PLDR_DATA_TABLE_ENTRY pFirstListEntry = NULL;
        LDR_DATA_TABLE_ENTRY ldrDataTable = { 0, };

        if ((m_processHandle != NULL) && (m_pNtQueryInformationProcess != NULL))
        {
            status = m_pNtQueryInformationProcess(m_processHandle, ProcessBasicInformation, &processBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), (DWORD*)&readData);
            if (NT_SUCCESS(status))
            {
                if (processBasicInformation.PebBaseAddress != NULL)
                {
                    if (ReadProcessMemory(m_processHandle, processBasicInformation.PebBaseAddress, &m_peb, sizeof(PEB), &readData))
                    {
                        // 프로세스의 Image Base Address가 저장되어 있는 필드
                        // OS 내부적으로 사용한다고 되어 있어서 향후에는 사용하지 못할 수 있음
                        // (ToolHelp32 API를 통해서 구하는 방법도 있음)
                        m_peBaseAddress = m_peb.Reserved3[1];
                        if (m_peBaseAddress != NULL)
                        {
                            m_logger << format(_T("Process image base address : 0x{:x}"), (ULONGLONG)m_peBaseAddress) << NL;

                            if (ReadProcessMemory(m_processHandle, m_peb.Ldr, &pebLdrData, sizeof(PEB_LDR_DATA), &readData))
                            {
                                // CONTAINING_RECORD 매크로
                                // 첫번째 인자인 Address에는 현재알고 있는 구조체 필드중의 포인트를 입력하고 두번째 인자는 알고자하는 구조체 변수를 세번째 인자는 첫번째 알고있는 포인트 주소의 구조체 내에서 필드변수를 넣어줌
                                // 그러면 두 번째에 입력했던 구조체 변수의 포인트 주소를 리턴함
                                pFirstListEntry = CONTAINING_RECORD(pebLdrData.InMemoryOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                                if (pFirstListEntry != NULL)
                                {
                                    if (ReadProcessMemory(m_processHandle, pFirstListEntry, &ldrDataTable, sizeof(LDR_DATA_TABLE_ENTRY), &readData))
                                    {
                                        if ((ULONGLONG)ldrDataTable.DllBase != 0x0)
                                        {
                                            // 맨 처음 항목이 프로세스 자신
                                            if (ReadProcessMemory(m_processHandle, ldrDataTable.FullDllName.Buffer, moduleNameBuffer, ldrDataTable.FullDllName.Length, &readData))
                                            {
                                                if (sizeof(TCHAR) == sizeof(char))
                                                {
                                                    wstring modulePath = (PWSTR)moduleNameBuffer;
                                                    m_peFilePath.assign(modulePath.begin(), modulePath.end());
                                                }
                                                else
                                                {
                                                    m_peFilePath = (PWSTR)moduleNameBuffer;
                                                }
                                                m_logger << format(_T("Module : 0x{:x}, {:s}"), (ULONGLONG)ldrDataTable.DllBase, m_peFilePath) << NL;
                                            }
                                        }
                                    }
                                }
                            }
                            result = TRUE;
                        }
                        else
                        {
                            m_logger << LogLevel::ERR;
                            m_logger << CodeLogInfo(_T("Get image base address fail")) << NL;
                        }
                    }
                    else
                    {
                        m_logger.log(_T("ReadProcessMemory fail"), GetLastError(), LOG_LEVEL_ERROR);
                    }
                }
            }
        }
        return result;
    };

    // PE 분석 과정에서 저장된 문자열을 읽어들이는 함수
    // 프로세스 메모리를 읽어야 하는데 문자열의 크기를 알 수 없기 때문에 1바이트 씩 읽음
    tstring PEProcessReader::getPEString(ULONGLONG rva, BOOL addBaseAddress)
    {
        DWORD readLength = 0;
        BYTE byteChar[2] = { 0, };
        ULONGLONG curOffset = rva;
        tstring readString;
        string srcString;

        do
        {
            // 한 바이트씩 문자열을 읽음
            if (readData(curOffset, &byteChar, sizeof(BYTE), readLength))
            {
                srcString.append((char*)byteChar);
                curOffset++;
            }
        } while (byteChar[0] != 0x0);

        if (sizeof(TCHAR) == sizeof(char))
        {
            readString.assign(srcString.begin(), srcString.end());
        }
        else
        {
            // PE에서의 문자열은 UTF-8로 저장되어 있기 때문에 문자열 처리가 필요
            {
                // 필요한 문자열 길이를 먼저 얻음
                int bufferLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(srcString.data()), -1, NULL, 0);
                // 필요한 문자열 길이 만큼 메모리 할당
                TCHAR* readBuffer = new TCHAR[bufferLen];
                // readString 메모리 주소를 리턴하는 .data()를 이용하여 readString 메모리에 변환된 문자열 저장
                MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(srcString.data()), -1, readBuffer, bufferLen);
                readString = readBuffer;
                delete[] readBuffer;
            }
        }
        return readString;
    };

    // 프로세스 메모리를 읽어들이는 함수
    BOOL PEProcessReader::readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, DWORD& readLength, BOOL absoluteOffset)
    {
        BOOL result = FALSE;
        SIZE_T dataReadLength = 0;

        if (m_processHandle != NULL)
        {
            if (ReadProcessMemory(m_processHandle, (LPCVOID)((ULONGLONG)m_peBaseAddress + rva), bufferAddress, bufferSize, &dataReadLength))
            {
                readLength = static_cast<DWORD>(dataReadLength);
                result = TRUE;
            }
            else
            {
                m_logger << LogLevel::ERR;
                m_logger << format(_T("ReadProcessMemory fail : 0x{:x}, 0x{:x}"), (DWORD)GetLastError(), rva) << NL;
            }
        }
        return result;
    };
    
    DWORD PEProcessReader::readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, BOOL absoluteOffset)
    {
        DWORD readLength;
        if (readData(rva, bufferAddress, bufferSize, readLength, absoluteOffset)) {
            return readLength;
        }
        else {
            return -1;
        }
    };

    BOOL PEProcessReader::createProcess(tstring filePath)
    {
        BOOL result = FALSE;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // 변수 초기화
        close();

        // Start the child process. 
        if (CreateProcess(NULL,       // No module name (use command line)
            (LPTSTR)filePath.c_str(), // Command line
            NULL,                     // Process handle not inheritable
            NULL,                     // Thread handle not inheritable
            FALSE,                    // Set handle inheritance to FALSE
            0,                        // No creation flags
            NULL,                     // Use parent's environment block
            NULL,                     // Use parent's starting directory 
            &si,                      // Pointer to STARTUPINFO structure
            &pi))                     // Pointer to PROCESS_INFORMATION structure
        {
            m_processId = pi.dwProcessId;
            m_processHandle = pi.hProcess;
            m_threadHandle = pi.hThread;

            result = parseImageBaseAddress();
            if (!result)
            {
                // Image Base Address를 얻지 못한 경우 실패로 처리(변수 초기화)
                close();
                m_logger.log(_T("Create process fail : get image base address fail"), LOG_LEVEL_ERROR);
            }
        }
        else
        {
            m_logger.log(_T("Create process fail"), GetLastError(), LOG_LEVEL_ERROR);
        }
        return result;
    };

    // CreateProcess로 생성한 경우에는 DLL 로딩전이기 때문에 출력 할 수 없음
    LoadedDllsInfo PEProcessReader::getLoadedDlls(void)
    {
        SIZE_T readData = 0;
        tstring dllPath;
        LoadedDllsInfo loadedDlls;
        PEB_LDR_DATA pebLdrData = { 0, };
        BYTE dllNameBuffer[MAX_PATH * 4] = { 0, };
        PLDR_DATA_TABLE_ENTRY pFirstListEntry = NULL;
        PLDR_DATA_TABLE_ENTRY pListEntry = NULL;
        LDR_DATA_TABLE_ENTRY ldrDataTable = { 0, };

        if ((m_peBaseAddress != NULL) && (m_peb.Ldr != NULL))
        {
            if (ReadProcessMemory(m_processHandle, m_peb.Ldr, &pebLdrData, sizeof(PEB_LDR_DATA), &readData))
            {
                // CONTAINING_RECORD 매크로
                // 첫번째 인자인 Address에는 현재알고 있는 구조체 필드중의 포인트를 입력하고 두번째 인자는 알고자하는 구조체 변수를 세번째 인자는 첫번째 알고있는 포인트 주소의 구조체 내에서 필드변수를 넣어줌
                // 그러면 두 번째에 입력했던 구조체 변수의 포인트 주소를 리턴함
                pFirstListEntry = CONTAINING_RECORD(pebLdrData.InMemoryOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                pListEntry = pFirstListEntry;
                if (pListEntry != NULL)
                {
                    do
                    {
                        if (ReadProcessMemory(m_processHandle, pListEntry, &ldrDataTable, sizeof(LDR_DATA_TABLE_ENTRY), &readData))
                        {
                            if ((ULONGLONG)ldrDataTable.DllBase == 0x0)
                            {
                                pListEntry = NULL;
                            }
                            else
                            {
                                // 맨 처음 항목이 프로세스 자신
                                if (ReadProcessMemory(m_processHandle, ldrDataTable.FullDllName.Buffer, dllNameBuffer, ldrDataTable.FullDllName.Length, &readData))
                                {
                                    if (sizeof(TCHAR) == sizeof(char))
                                    {
                                        wstring modulePath = (PWSTR)dllNameBuffer;
                                        dllPath.assign(modulePath.begin(), modulePath.end());
                                    }
                                    else
                                    {
                                        dllPath = (PWSTR)dllNameBuffer;
                                    }
                                    LoadedDllInfo a = { dllPath, (ULONGLONG)ldrDataTable.DllBase };
                                    loadedDlls.push_back(a);
                                }
                                pListEntry = CONTAINING_RECORD(ldrDataTable.InMemoryOrderLinks.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                            }
                        }
                        else
                        {
                            pListEntry = NULL;
                        }
                        memset(&ldrDataTable, 0, sizeof(LDR_DATA_TABLE_ENTRY));
                        memset(&dllNameBuffer, 0, sizeof(dllNameBuffer));
                    } while ((pListEntry != NULL) && (pListEntry != pFirstListEntry));
                }
            }
        }
        else
        {
            m_logger.log(_T("Get loaded dlls fail - PEB.Ldr is NULL."), LOG_LEVEL_ERROR);
        }
        return loadedDlls;
    };

};
