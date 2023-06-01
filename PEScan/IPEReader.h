#pragma once
#include "Common/typedef.h"

namespace PEScan {
    interface IPEReader {
    public:
        virtual ~IPEReader() {};
        virtual void close(void) abstract;
        virtual BOOL open(DWORD pid, const TCHAR* pfilePath, BOOL checkPE = FALSE) abstract;
        virtual LPVOID getBaseAddress(void) abstract;
        virtual HANDLE getHandle(void) abstract;
        virtual tstring getFilePath(void) abstract;
        virtual tstring getPEString(ULONGLONG rva, BOOL addBaseAddress = TRUE) abstract;
        virtual DWORD readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, BOOL absoluteOffset = FALSE) abstract;
    };
}
