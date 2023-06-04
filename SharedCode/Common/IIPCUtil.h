#pragma once
#include "Common/typedef.h"
#include "Common/IPCTypedef.h"
#include "mutex"

namespace PEScan {
    interface IIPCUtil {
        virtual ~IIPCUtil() {}

        // IPC가 열렸는지 확인
        virtual BOOL isAlive(void) abstract;
        virtual BOOL isAvailable(void) abstract;

        // 수신측에서 호출
        // 공유 메모리 생성 및 매핑, Mutex 생성
        virtual BOOL create(const tstring ipcName, DWORD bufferSize = 8192) abstract; 

        // 송신측에서 호출
        // 공유 메모리 열기 및 매핑, Mutex 열기
        virtual BOOL open(const tstring ipcName, DWORD bufferSize = 8192) abstract;

        // 메세지 송신
        // Mutex 대기 및 획득 후 메시지 전송 (메모리 복사)
        virtual BOOL sendMsg(IPC_MESSAGE ipcData) abstract;

        // 메세지 수신
        // Mutex 대기 및 획득 후 메시지가 존재하는 경우 가져오기 (메모리 복사)
        virtual BOOL receiveMsg(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;

        // IPC 공유 메모리 생성 또는 열기
        virtual BOOL start(const tstring ipcName, BOOL create, DWORD bufferSize = 8192) abstract;

        // 자원 정리
        virtual void stop(void) abstract;

        // 메세지가 존재한다면 복사
        virtual BOOL getNextInfo(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;
    };
};

