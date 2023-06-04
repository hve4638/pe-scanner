#pragma once
#include "Common/typedef.h"
#include "Common/IArgs.h"
#include "Common/IRunnable.h"
#include "Common/ArgsAdditional.h"

namespace CmdLineUtils {
    interface ICmdLineParser {
        // 들어온 인자를 해석 후 명령 실행
        virtual void run(IArgsPtr) abstract;

        // 주어진 인자에 들어왔을 때 실행할 명령 추가
        virtual void add(IArgsPtr cmd, shared_ptr<IRunnable> runnable) abstract;
        

        // 사용을 간편하게 하기 위한 wrapper 함수
        virtual void run(int args, TCHAR* argv[]) abstract;
        virtual void run(tstring) abstract;
        virtual void add(
            tstring path,                       // 명령줄
            CmdCallback callback,               // 실행 함수
            tstring options = _T(""),           // key-value 형태의 옵션
            tstring booleanOptions = _T("")     // key만 가지는 옵션
        ) abstract;
    };
}
