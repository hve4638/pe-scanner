#pragma once
#include "Common/typedef.h"
#include "Common/IPCTypedef.h"
#include "mutex"

namespace PEScan {
    interface IIPCUtil {
        virtual ~IIPCUtil() {}
        virtual BOOL isAlive(void) abstract;
        virtual BOOL isAvailable(void) abstract;
        virtual BOOL create(const tstring ipcName, DWORD bufferSize = 8192) abstract;  // 기본적으로 8K 버퍼 사용
        virtual BOOL open(const tstring ipcName, DWORD bufferSize = 8192) abstract;
        virtual BOOL sendMsg(IPC_MESSAGE ipcData) abstract;
        virtual BOOL receiveMsg(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;
        virtual BOOL start(const tstring ipcName, BOOL create, DWORD bufferSize = 8192) abstract;
        virtual void stop(void) abstract;
        virtual BOOL getNextInfo(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;
    };
};

