#pragma once
#include "Common/typedef.h"
#include <functional>

using namespace std;

namespace PEScan {
    interface IFileSearch {
        virtual ~IFileSearch() {}

        // 실행중인 search() 중단
        // 다른 스레드에서 실행중일때 사용
        virtual void stop() abstract;

        // 상대경로 또는 환경변수가 포함된 경로로부터 절대경로를 변환
        virtual BOOL getRealPath(tstring orignal, tstring& real) abstract;
        
        // 주어진 경로부터 재귀적으로 파일을 탐색해 콜백 함수를 호출
        virtual void search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction = NULL) abstract;
    };
};
