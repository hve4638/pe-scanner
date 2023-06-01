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
        // �ڵ� �ʱ�ȭ
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

        // ���� �ʱ�ȭ
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
                    // Image Base Address�� ���� ���� ��� ���з� ó��(���� �ʱ�ȭ)
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
                        // ���μ����� Image Base Address�� ����Ǿ� �ִ� �ʵ�
                        // OS ���������� ����Ѵٰ� �Ǿ� �־ ���Ŀ��� ������� ���� �� ����
                        // (ToolHelp32 API�� ���ؼ� ���ϴ� ����� ����)
                        m_peBaseAddress = m_peb.Reserved3[1];
                        if (m_peBaseAddress != NULL)
                        {
                            m_logger << format(_T("Process image base address : 0x{:x}"), (ULONGLONG)m_peBaseAddress) << NL;

                            if (ReadProcessMemory(m_processHandle, m_peb.Ldr, &pebLdrData, sizeof(PEB_LDR_DATA), &readData))
                            {
                                // CONTAINING_RECORD ��ũ��
                                // ù��° ������ Address���� ����˰� �ִ� ����ü �ʵ����� ����Ʈ�� �Է��ϰ� �ι�° ���ڴ� �˰����ϴ� ����ü ������ ����° ���ڴ� ù��° �˰��ִ� ����Ʈ �ּ��� ����ü ������ �ʵ庯���� �־���
                                // �׷��� �� ��°�� �Է��ߴ� ����ü ������ ����Ʈ �ּҸ� ������
                                pFirstListEntry = CONTAINING_RECORD(pebLdrData.InMemoryOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                                if (pFirstListEntry != NULL)
                                {
                                    if (ReadProcessMemory(m_processHandle, pFirstListEntry, &ldrDataTable, sizeof(LDR_DATA_TABLE_ENTRY), &readData))
                                    {
                                        if ((ULONGLONG)ldrDataTable.DllBase != 0x0)
                                        {
                                            // �� ó�� �׸��� ���μ��� �ڽ�
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

    // PE �м� �������� ����� ���ڿ��� �о���̴� �Լ�
    // ���μ��� �޸𸮸� �о�� �ϴµ� ���ڿ��� ũ�⸦ �� �� ���� ������ 1����Ʈ �� ����
    tstring PEProcessReader::getPEString(ULONGLONG rva, BOOL addBaseAddress)
    {
        DWORD readLength = 0;
        BYTE byteChar[2] = { 0, };
        ULONGLONG curOffset = rva;
        tstring readString;
        string srcString;

        do
        {
            // �� ����Ʈ�� ���ڿ��� ����
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
            // PE������ ���ڿ��� UTF-8�� ����Ǿ� �ֱ� ������ ���ڿ� ó���� �ʿ�
            {
                // �ʿ��� ���ڿ� ���̸� ���� ����
                int bufferLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(srcString.data()), -1, NULL, 0);
                // �ʿ��� ���ڿ� ���� ��ŭ �޸� �Ҵ�
                TCHAR* readBuffer = new TCHAR[bufferLen];
                // readString �޸� �ּҸ� �����ϴ� .data()�� �̿��Ͽ� readString �޸𸮿� ��ȯ�� ���ڿ� ����
                MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(srcString.data()), -1, readBuffer, bufferLen);
                readString = readBuffer;
                delete[] readBuffer;
            }
        }
        return readString;
    };

    // ���μ��� �޸𸮸� �о���̴� �Լ�
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

        // ���� �ʱ�ȭ
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
                // Image Base Address�� ���� ���� ��� ���з� ó��(���� �ʱ�ȭ)
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

    // CreateProcess�� ������ ��쿡�� DLL �ε����̱� ������ ��� �� �� ����
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
                // CONTAINING_RECORD ��ũ��
                // ù��° ������ Address���� ����˰� �ִ� ����ü �ʵ����� ����Ʈ�� �Է��ϰ� �ι�° ���ڴ� �˰����ϴ� ����ü ������ ����° ���ڴ� ù��° �˰��ִ� ����Ʈ �ּ��� ����ü ������ �ʵ庯���� �־���
                // �׷��� �� ��°�� �Է��ߴ� ����ü ������ ����Ʈ �ּҸ� ������
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
                                // �� ó�� �׸��� ���μ��� �ڽ�
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
