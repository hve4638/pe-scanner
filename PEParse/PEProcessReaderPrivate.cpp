#pragma once

#include "PEProcessReader.h"
#include "PEUtils.h"
#include <iostream>
#include <format>

using namespace std;
using namespace PEUtils;

// CONTAINING_RECORD ��ũ��
// ù��° ������ Address���� ����˰� �ִ� ����ü �ʵ����� ����Ʈ�� �Է��ϰ� �ι�° ���ڴ� �˰����ϴ� ����ü ������ ����° ���ڴ� ù��° �˰��ִ� ����Ʈ �ּ��� ����ü ������ �ʵ庯���� �־���
// �׷��� �� ��°�� �Է��ߴ� ����ü ������ ����Ʈ �ּҸ� ������

namespace PEParse {
    BOOL PEProcessReader::parseImageBaseAddress() {
        NTSTATUS status = 0;
        SIZE_T readSize = 0;
        PROCESS_BASIC_INFORMATION processBasicInformation = { 0, };

        if (m_processHandle == NULL || m_pNtQueryInformationProcess == NULL) {
            return FALSE;
        }
        else {
            status = m_pNtQueryInformationProcess(m_processHandle, ProcessBasicInformation, &processBasicInformation, sizeof(PROCESS_BASIC_INFORMATION), (DWORD*)&readSize);
            if (!NT_SUCCESS(status)) {
                return FALSE;
            }
            else if (processBasicInformation.PebBaseAddress == NULL) {
                return FALSE;
            }
            else if (!ReadProcessMemory(m_processHandle, processBasicInformation.PebBaseAddress, &m_peb, sizeof(PEB), &readSize)) {
                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo(_T("ReadProcessMemory fail")) << NL;
                return FALSE;
            }
            else {
                m_peBaseAddress = m_peb.Reserved3[1]; // ���μ����� ImageBaseAddress�� ����� �ʵ�
                if (m_peBaseAddress == NULL) {
                    m_logger << LogLevel::ERR;
                    m_logger << ErrorLogInfo(_T("Get ImageBaseAddress fail")) << NL;
                    return FALSE;
                }
                else {
                    m_logger << LogLevel::DEBUG;
                    m_logger << format(_T("Process ImageBaseAddress : 0x{:x}"), (ULONGLONG)m_peBaseAddress) << NL;

                    parsePEFilePath();                    
                    return TRUE;
                }
            }
        }
    };

    BOOL PEProcessReader::parsePEFilePath() {
        PEB_LDR_DATA pebLdrData = { 0, };
        PLDR_DATA_TABLE_ENTRY firstListEntry = NULL;
        LDR_DATA_TABLE_ENTRY ldrDataTable = { 0, };
        BYTE moduleNameBuffer[MAX_PATH * 4] = { 0, };
        SIZE_T readSize = 0; 

        if (ReadProcessMemory(m_processHandle, m_peb.Ldr, &pebLdrData, sizeof(PEB_LDR_DATA), &readSize)) {
            auto pInMemoryOrderLinks = pebLdrData.InMemoryOrderModuleList.Flink;
            // pInMemoryOrderLinks�� LDR_DATA_TABLE_ENTRY.InMemoryOrderLinks �ʵ��� �ּ�
            // �����Ͽ� LDR_DATA_TABLE_ENTRY ����ü�� ù �ּҸ� �����´�
            firstListEntry = CONTAINING_RECORD(pInMemoryOrderLinks, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

            if (firstListEntry == NULL) {}
            else if (ReadProcessMemory(m_processHandle, firstListEntry, &ldrDataTable, sizeof(LDR_DATA_TABLE_ENTRY), &readSize)) {}
            else if ((ULONGLONG)ldrDataTable.DllBase == NULL) {}
            else {
                // �� ó�� �׸��� ���μ��� �ڽ�
                if (ReadProcessMemory(m_processHandle, ldrDataTable.FullDllName.Buffer, moduleNameBuffer, ldrDataTable.FullDllName.Length, &readSize)) {
                    if (CHAR_IS_TCHAR) {
                        wstring modulePath = (PWSTR)moduleNameBuffer;
                        m_peFilePath.assign(modulePath.begin(), modulePath.end());
                    }
                    else {
                        m_peFilePath = (PWSTR)moduleNameBuffer;
                    }

                    m_logger << LogLevel::DEBUG;
                    m_logger << format(_T("Module : 0x{:x}, {:s}"), (ULONGLONG)ldrDataTable.DllBase, m_peFilePath) << NL;
                }
            }
        }
        return TRUE;
    }
}