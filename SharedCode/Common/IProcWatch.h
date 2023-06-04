#pragma once
#include "Common/ProcTypedef.h"

namespace PEScan {
    interface IProcWatch {
        virtual ~IProcWatch() {}

        // ����͸� ������ Ȯ��
        virtual BOOL isAlive(void) abstract;

        // ���μ��� Ȱ���� ���� ����͸� ����
        // ����͸��� ���ο� �����忡�� �Ͼ
        virtual void startMonitoring(void) abstract;

        // ����͸� ����
        virtual void stopMonitoring(void) abstract;

        // ���������� ���, ���� �� Ȱ�� Ž���� ȣ���
        virtual void processCallback(
            DWORD pid, tstring processName, tstring processPath, tstring processCmdLine
        ) abstract;

        // ����͸� �� Ž���� ���� ������ ������
        // ������ ���ٸ� False ����
        virtual BOOL getNextInfo(ProcessInfo& processInfo) abstract;
    };
};
