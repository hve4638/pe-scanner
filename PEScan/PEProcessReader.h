#pragma once
#include "IPEReader.h"
#include <winternl.h>
#include "Logger.h"

namespace PEScan {
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass,
        OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);

    class PEProcessReader : public IPEReader {
        Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };
        DWORD m_processId = NULL;
        HANDLE m_processHandle = NULL;
        HANDLE m_threadHandle = NULL;
        LPVOID m_peBaseAddress = NULL;
        tstring m_peFilePath;
        PEB m_peb = { 0, };
        pNtQueryInformationProcess m_pNtQueryInformationProcess = NULL;

    private:
        BOOL parseImageBaseAddress(void);

    public:
        PEProcessReader();
        ~PEProcessReader() override;
        void close(void) override;
        BOOL open(DWORD pid, const TCHAR* pfilePath, BOOL checkPE = FALSE) override;
        tstring getPEString(ULONGLONG rva, BOOL addBaseAddress = TRUE) override;
        BOOL readData(ULONGLONG rva, LPVOID bufferAddress, DWORD bufferSize, DWORD& readLength, BOOL absoluteOffset = FALSE) override;
        LPVOID getBaseAddress(void) override;
        HANDLE getHandle(void) override;
        tstring getFilePath(void) override;

    public:
        BOOL createProcess(tstring filePath);
        LoadedDllsInfo getLoadedDlls(void);
    };

};


