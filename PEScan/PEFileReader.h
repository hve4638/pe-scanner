#pragma once
#include "IPEReader.h"
#include "Logger.h"

namespace PEScan {
    class PEFileReader : public IPEReader {
        Logger m_logger = {LogLevel::ERR, LogDirection::DEBUGVIEW};
        HANDLE m_peFileHandle = INVALID_HANDLE_VALUE;
        HANDLE m_peFileMapping = INVALID_HANDLE_VALUE;
        PBYTE m_peBaseAddress = NULL;
        ULONGLONG m_curSectionStartAddress = 0;
        ULONGLONG m_curSectionEndAddress = 0;
        ULONGLONG m_curSectionVirtualAddress = 0;
        ULONGLONG m_curSectionPointerToRawData = 0;
        ULONGLONG m_sizeOfHeaders = 0;
        DWORD m_numberOfSections = 0;
        PIMAGE_SECTION_HEADER m_pSectionHeader = NULL;
        tstring m_peFilePath;

    private:
        BOOL setRvaToRawInfo(ULONGLONG rav);
        BOOL readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, DWORD& readLength, BOOL absoluteOffset = FALSE);

    public:
        PEFileReader();
        ~PEFileReader() override;
        void close(void) override;
        BOOL open(DWORD pid, const TCHAR* pfilePath, BOOL checkPE = FALSE) override;
        LPVOID getBaseAddress(void) override;
        HANDLE getHandle(void) override;
        tstring getFilePath(void) override;
        tstring getPEString(ULONGLONG rva, BOOL addBaseAddress = TRUE) override;
        DWORD readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, BOOL absoluteOffset = FALSE) override;
    };

};


