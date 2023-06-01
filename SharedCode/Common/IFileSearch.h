#pragma once
#include "Common/typedef.h"
#include <functional>

using namespace std;

namespace PEScan {
    interface IFileSearch {
        virtual ~IFileSearch() {}

        /// <summary>
        /// �������� search() ����
        /// </summary>
        virtual void stop() abstract;

        /// <summary>
        /// ����� �Ǵ� ȯ�溯���� ���Ե� ��ηκ��� �����θ� ����
        /// </summary>
        /// <param name="orignal"></param>
        /// <param name="real"></param>
        virtual BOOL getRealPath(tstring orignal, tstring& real) abstract;
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="parentDirectory">���� ���丮</param>
        /// <param name="currentFileCount">ã�� ���ϼ�</param>
        /// <param name="scanFunction">�ݹ�</param>
        virtual void search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction = NULL) abstract;
    };
};
