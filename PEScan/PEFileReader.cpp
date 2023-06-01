#pragma once

#include "PEFileReader.h"
#include <iostream>
#include <format>
#include <codecvt>

using namespace std;

namespace PEScan {

    PEFileReader::PEFileReader()
    {
    };

    PEFileReader::~PEFileReader(void)
    {
        close();
    };

    void PEFileReader::close(void)
    {
        // �ڵ� �ʱ�ȭ
        if (m_peFileMapping != INVALID_HANDLE_VALUE)
        {
            UnmapViewOfFile(m_peBaseAddress);
            CloseHandle(m_peFileMapping);
            m_peFileMapping = INVALID_HANDLE_VALUE;
        }
        if (m_peFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_peFileHandle);
            m_peFileHandle = INVALID_HANDLE_VALUE;
        }

        // ���� �� �ʱ�ȭ
        m_peBaseAddress = NULL;
        m_curSectionStartAddress = 0;
        m_curSectionEndAddress = 0;
        m_curSectionVirtualAddress = 0;
        m_curSectionPointerToRawData = 0;
        m_sizeOfHeaders = 0;
        m_numberOfSections = 0;
        m_pSectionHeader = NULL;
        m_peFilePath.clear();
    };

    LPVOID PEFileReader::getBaseAddress(void)
    {
        return m_peBaseAddress;
    };

    HANDLE PEFileReader::getHandle(void)
    {
        return m_peFileHandle;
    };

    tstring PEFileReader::getFilePath(void)
    {
        return m_peFilePath;
    };

    // rva �ּҰ� ������ ���� ������ �ִ��� üũ�ؼ� ������ ������ �ش� rva�� ���� ������ ã�Ƽ� ���� ���� ���� �缳��
    BOOL PEFileReader::setRvaToRawInfo(ULONGLONG rva)
    {
        BOOL result = FALSE;

        if (rva < m_sizeOfHeaders)
        {
            // PE ��� ���� �������� RAW ����� �ʿ� ����
            result = TRUE;
        }
        else
        {
            if ((rva >= m_curSectionStartAddress) && (rva < m_curSectionEndAddress))
            {
                // ���� ������ ���� ������ �ִ��� Ȯ��
                result = TRUE;
            }
            else
            {
                // ���� ��� ���� ����
                if (m_pSectionHeader == NULL)
                {
                    PIMAGE_NT_HEADERS32 pNtHeader32 = reinterpret_cast<PIMAGE_NT_HEADERS32>(m_peBaseAddress + (LONG)((PIMAGE_DOS_HEADER)m_peBaseAddress)->e_lfanew);
                    if (pNtHeader32->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
                    {
                        PIMAGE_NT_HEADERS64 pNtHeader64 = reinterpret_cast<PIMAGE_NT_HEADERS64>(m_peBaseAddress + (LONG)((PIMAGE_DOS_HEADER)m_peBaseAddress)->e_lfanew);
                        m_sizeOfHeaders = pNtHeader64->OptionalHeader.SizeOfHeaders;
                        m_numberOfSections = pNtHeader64->FileHeader.NumberOfSections;
                        m_pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>((PBYTE)pNtHeader64 + sizeof(IMAGE_NT_HEADERS64));
                    }
                    else
                    {
                        m_sizeOfHeaders = pNtHeader32->OptionalHeader.SizeOfHeaders;
                        m_numberOfSections = pNtHeader32->FileHeader.NumberOfSections;
                        m_pSectionHeader = reinterpret_cast<PIMAGE_SECTION_HEADER>((PBYTE)pNtHeader32 + sizeof(IMAGE_NT_HEADERS32));
                    }
                }
                if (rva < m_sizeOfHeaders)
                {
                    // PE ��� ���� �������� RAW ����� �ʿ� ����
                    result = TRUE;
                }
                else
                {
                    // RvaToRaw ��꿡 �ʿ��� ���� ����
                    if (m_pSectionHeader != NULL)
                    {
                        // RvaToRaw ��꿡 �ʿ��� ���� �ʱ�ȭ
                        m_curSectionStartAddress = 0;
                        m_curSectionEndAddress = 0;
                        m_curSectionVirtualAddress = 0;
                        m_curSectionPointerToRawData = 0;

                        for (DWORD index = 0; index < m_numberOfSections; index++)
                        {
                            // VirtualAddress �ּҿ� ���ų� ũ�� (VirtualAddress + SizeOfRawData) ���ٴ� �۾ƾ� ��
                            if ((rva >= (ULONGLONG)m_pSectionHeader[index].VirtualAddress) && (rva < (ULONGLONG)((ULONGLONG)m_pSectionHeader[index].VirtualAddress + (ULONGLONG)m_pSectionHeader[index].SizeOfRawData)))
                            {
                                // RvaToRaw ��꿡 �ʿ��� ���� ����
                                m_curSectionStartAddress = (ULONGLONG)m_pSectionHeader[index].VirtualAddress;
                                m_curSectionEndAddress = (ULONGLONG)m_curSectionStartAddress + m_pSectionHeader[index].SizeOfRawData;
                                m_curSectionVirtualAddress = (ULONGLONG)m_pSectionHeader[index].VirtualAddress;
                                m_curSectionPointerToRawData = (ULONGLONG)m_pSectionHeader[index].PointerToRawData;

                                result = TRUE;
                                break;
                            }
                        }
                    }
                }
            }
        }
        return result;
    };

    BOOL PEFileReader::open(DWORD pid, const TCHAR* pfilePath, BOOL checkPE)
    {
        BOOL result = FALSE;
        DWORD readLength = 0;
        WORD peCheckBuffer = 0;

        if (pfilePath != NULL)
        {
            m_peFilePath = pfilePath;
            m_logger << LogLevel::DEBUG;
            m_logger << format(_T("Create memory map : {:s}"), m_peFilePath) << NL;

            m_peFileHandle = CreateFile(m_peFilePath.c_str(),
                GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY, NULL);
            if (m_peFileHandle == INVALID_HANDLE_VALUE)
            {
                m_logger << LogLevel::DEBUG;
                m_logger << _T("Error: Cannot open file") << NL;
                m_logger << format(_T("  >> {:s}"), m_peFilePath) << NL;
            }
            else
            {
                if (checkPE)
                {
                    if (!ReadFile(m_peFileHandle, &peCheckBuffer, sizeof(WORD), &readLength, NULL) || (peCheckBuffer != IMAGE_DOS_SIGNATURE))
                    {
                        m_logger << LogLevel::DEBUG;
                        m_logger << _T("Info: not PE file") << NL;
                        m_logger << format(_T("  >> {:s}"), m_peFilePath) << NL;
                        close();
                    }
                }
                if (m_peFileHandle != INVALID_HANDLE_VALUE)
                {
                    m_peFileMapping = CreateFileMapping(m_peFileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
                    if (m_peFileMapping == NULL)
                    {
                        CloseHandle(m_peFileHandle);
                        m_peFileHandle = NULL;
                        m_logger.log(_T("Error: Cannot create file mapping"), GetLastError(), LOG_LEVEL_ERROR);
                        m_logger << format(_T("  >> {:s}"), m_peFilePath) << NL;
                    }
                    else
                    {
                        m_peBaseAddress = static_cast<PBYTE>(MapViewOfFile(m_peFileMapping, FILE_MAP_READ, 0, 0, 0));
                        if (m_peBaseAddress != NULL)
                        {
                            result = TRUE;
                        }
                        else
                        {
                            CloseHandle(m_peFileMapping);
                            CloseHandle(m_peFileHandle);
                            m_peFileMapping = INVALID_HANDLE_VALUE;
                            m_peFileHandle = INVALID_HANDLE_VALUE;
                            m_logger.log(_T("Error: Cannot map view of file"), GetLastError(), LOG_LEVEL_ERROR);
                        }
                    }
                }
            }
        }
        return result;
    };

    DWORD PEFileReader::readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, BOOL absoluteOffset) {
        DWORD readLength;
        if (readData(rva, bufferAddress, bufferSize, readLength, absoluteOffset)) {
            return readLength;
        }
        else {
            return -1;
        }
    }

    BOOL PEFileReader::readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, DWORD& readLength, BOOL absoluteOffset)
    {
        BOOL result = FALSE;
        LPVOID realAddress = NULL;

        if (absoluteOffset)
        {
            // realAddress = reinterpret_cast<LPVOID>(reinterpret_cast<ULONGLONG>(m_peBaseAddress) + rva);
            realAddress = (LPVOID)((ULONGLONG)m_peBaseAddress + (ULONGLONG)(rva));
            memcpy_s(bufferAddress, bufferSize, realAddress, bufferSize);
            readLength = bufferSize;
            result = TRUE;
        }
        else
        {
            if (setRvaToRawInfo(rva))
            {
                // RVA to RAW ��� (RAW = RVA - VirtualAddress + PointerToRawData)
                realAddress = (LPVOID)((ULONGLONG)m_peBaseAddress + (ULONGLONG)(rva - m_curSectionVirtualAddress + m_curSectionPointerToRawData));
                memcpy_s(bufferAddress, bufferSize, realAddress, bufferSize);
                readLength = bufferSize;
                result = TRUE;
            }
            else
            {
                // ������ ��� TLS�� ���� ��� setRvaToRawInfo �Լ��� ���� ��(���� ���� �����ϴ� �ּҰ� �ƴ�)
                m_logger << LogLevel::DEBUG;
                m_logger << format(_T("RVA to RAW fail : 0x{:x}, 0x{:x}"), GetLastError(), rva) << NL;
            }
        }
        return result;
    };

    // PE �м� �������� ����� ���ڿ��� �о���̴� �Լ�
    tstring PEFileReader::getPEString(ULONGLONG rva, BOOL addBaseAddress)
    {
        tstring readString;
        LPVOID realAddress = NULL;

        if (setRvaToRawInfo(rva))
        {
            if (addBaseAddress)
            {
                realAddress = (LPVOID)((ULONGLONG)m_peBaseAddress + (ULONGLONG)(rva - m_curSectionVirtualAddress + m_curSectionPointerToRawData));
            }
            else
            {
                realAddress = (LPVOID)((ULONGLONG)(rva - m_curSectionVirtualAddress + m_curSectionPointerToRawData));
            }
            if (sizeof(TCHAR) == sizeof(char))
            {
                readString = static_cast<TCHAR*>(realAddress);
            }
            else
            {
                // PE������ ���ڿ��� UTF-8�� ����Ǿ� �ֱ� ������ ���ڿ� ó���� �ʿ�
                {
                    // �ʿ��� ���ڿ� ���̸� ���� ����
                    int bufferLen = MultiByteToWideChar(CP_UTF8, 0, static_cast<LPCCH>(realAddress), -1, NULL, 0);
                    // �ʿ��� ���ڿ� ���� ��ŭ �޸� �Ҵ�
                    TCHAR* readBuffer = new TCHAR[bufferLen];
                    // readString �޸� �ּҸ� �����ϴ� .data()�� �̿��Ͽ� readString �޸𸮿� ��ȯ�� ���ڿ� ����
                    MultiByteToWideChar(CP_UTF8, 0, static_cast<LPCCH>(realAddress), -1, readBuffer, bufferLen);
                    readString = readBuffer;
                    delete[] readBuffer;
                }
            }
        }
        else
        {
            m_logger << LogLevel::DEBUG;
            m_logger << format(_T("RVA to RAW fail : 0x{:x}, 0x{:x}"), GetLastError(), rva) << NL;
        }
        return readString;
    };
     
};
