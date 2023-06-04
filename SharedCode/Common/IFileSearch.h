#pragma once
#include "Common/typedef.h"
#include <functional>

using namespace std;

namespace PEScan {
    interface IFileSearch {
        virtual ~IFileSearch() {}

        // �������� search() �ߴ�
        // �ٸ� �����忡�� �������϶� ���
        virtual void stop() abstract;

        // ����� �Ǵ� ȯ�溯���� ���Ե� ��ηκ��� �����θ� ��ȯ
        virtual BOOL getRealPath(tstring orignal, tstring& real) abstract;
        
        // �־��� ��κ��� ��������� ������ Ž���� �ݹ� �Լ��� ȣ��
        virtual void search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction = NULL) abstract;
    };
};
