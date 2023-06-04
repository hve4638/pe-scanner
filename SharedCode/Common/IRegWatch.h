#pragma once
#include "Common/typedef.h"
#include "Common/RegTypedef.h"

namespace PEScan {
    interface IRegWatch {
        virtual ~IRegWatch() {}

        // ����͸� ������ Ȯ��
        virtual BOOL isAlive() abstract;

        // �־��� ������Ʈ�� ��ο� ���� ����͸� ����
        // ����͸��� ���ο� �����忡�� �Ͼ
        virtual void startMonitoring(RegRootKey rootKey, tstring subKey) abstract;

        // ����͸� ���̶��, ����͸� ���� �� �ڿ� ����
        virtual void stopMonitoring() abstract;

        // ���������� ���, ���� �� �߰�/���� ���� Ȱ�� Ž���� ȣ���
        virtual void regCallback(tstring filePath, RegPathInfo regPathInfo) abstract;

        // ����͸� �� Ž���� ���� ������ ������
        // ������ ���ٸ� False ����
        virtual BOOL getNextInfo(RegFileInfo& regFileInfo) abstract;
    };
};