#pragma once
#include "Common/IIPCUtil.h"
#include "Logger.h"

namespace PEScan {
    class IPCUtil : public IIPCUtil {
    private:
        BOOL m_isWatching = FALSE;
        Logger m_logger;
        HANDLE m_mutexHandle = NULL;
        HANDLE m_sharedMemHandle = NULL;
        LPVOID m_sharedMemBuffer = NULL;
        DWORD m_sharedMemSize = 0;
        PDWORD m_command = NULL;
        tstring m_mutexName;
        tstring m_sharedMemName;

    private:
        void clearSharedMemory(void);

    public:
        IPCUtil();
        ~IPCUtil() override;
        BOOL isAlive(void) override;
        BOOL isAvailable(void) override;
        BOOL create(const tstring ipcName, DWORD bufferSize = 8192) override;
        BOOL open(const tstring ipcName, DWORD bufferSize = 8192) override;
        BOOL sendMsg(IPC_MESSAGE ipcData) override;
        BOOL receiveMsg(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) override;
        BOOL start(const tstring ipcName, BOOL create, DWORD bufferSize = 8192) override;
        void stop(void) override;
        BOOL getNextInfo(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) override;
    };
};


