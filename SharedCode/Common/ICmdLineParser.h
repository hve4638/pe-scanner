#pragma once
#include "Common/typedef.h"
#include "Common/IArgs.h"
#include "Common/IRunnable.h"
#include "Common/ArgsAdditional.h"

namespace CmdLineUtils {
    interface ICmdLineParser {
        // ���� ���ڸ� �ؼ� �� ��� ����
        virtual void run(IArgsPtr) abstract;

        // �־��� ���ڿ� ������ �� ������ ��� �߰�
        virtual void add(IArgsPtr cmd, shared_ptr<IRunnable> runnable) abstract;
        

        // ����� �����ϰ� �ϱ� ���� wrapper �Լ�
        virtual void run(int args, TCHAR* argv[]) abstract;
        virtual void run(tstring) abstract;
        virtual void add(
            tstring path,                       // �����
            CmdCallback callback,               // ���� �Լ�
            tstring options = _T(""),           // key-value ������ �ɼ�
            tstring booleanOptions = _T("")     // key�� ������ �ɼ�
        ) abstract;
    };
}
