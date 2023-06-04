#pragma once
#include "Common/FileTypedef.h"

namespace PEScan {
    interface IFileWatch {
        virtual ~IFileWatch() {}

        // ����͸� ������ Ȯ��
        virtual BOOL isAlive(void) abstract;

        // �־��� ���� ��ο� ���� ����͸� ����
        // ����͸��� ���ο� �����忡�� �Ͼ
        virtual void startMonitoring(tstring rootDir) abstract;

        // ����͸� ���̶��, ����͸� ���� �� �ڿ� ����
        virtual void stopMonitoring(void) abstract;

        // ���������� ���, ���� �� �߰�/���� ���� Ȱ�� Ž���� ȣ���
        virtual void fileCallback(
            tstring fileName, tstring fileExt, tstring filePath, FileEvent fileEvent
        ) abstract;

        // ����͸� �� Ž���� ���� ������ ������
        // ������ ���ٸ� False ����
        virtual BOOL getNextInfo(FileInfo& fileInfo) abstract;
    };
};