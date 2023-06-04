#pragma once
#include "Common/typedef.h"
#include "Common/IPCTypedef.h"
#include "mutex"

namespace PEScan {
    interface IIPCUtil {
        virtual ~IIPCUtil() {}

        // IPC�� ���ȴ��� Ȯ��
        virtual BOOL isAlive(void) abstract;
        virtual BOOL isAvailable(void) abstract;

        // ���������� ȣ��
        // ���� �޸� ���� �� ����, Mutex ����
        virtual BOOL create(const tstring ipcName, DWORD bufferSize = 8192) abstract; 

        // �۽������� ȣ��
        // ���� �޸� ���� �� ����, Mutex ����
        virtual BOOL open(const tstring ipcName, DWORD bufferSize = 8192) abstract;

        // �޼��� �۽�
        // Mutex ��� �� ȹ�� �� �޽��� ���� (�޸� ����)
        virtual BOOL sendMsg(IPC_MESSAGE ipcData) abstract;

        // �޼��� ����
        // Mutex ��� �� ȹ�� �� �޽����� �����ϴ� ��� �������� (�޸� ����)
        virtual BOOL receiveMsg(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;

        // IPC ���� �޸� ���� �Ǵ� ����
        virtual BOOL start(const tstring ipcName, BOOL create, DWORD bufferSize = 8192) abstract;

        // �ڿ� ����
        virtual void stop(void) abstract;

        // �޼����� �����Ѵٸ� ����
        virtual BOOL getNextInfo(IPC_MESSAGE& ipcData, IPCCommandType acceptCommandType) abstract;
    };
};

