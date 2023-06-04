#pragma once
#include "Common/ProcTypedef.h"

namespace PEScan {
    interface IProcWatch {
        virtual ~IProcWatch() {}

        // 모니터링 중인지 확인
        virtual BOOL isAlive(void) abstract;

        // 프로세스 활동에 대한 모니터링 시작
        // 모니터링는 새로운 스레드에서 일어남
        virtual void startMonitoring(void) abstract;

        // 모니터링 종료
        virtual void stopMonitoring(void) abstract;

        // 내부적으로 사용, 감시 중 활동 탐지시 호출됨
        virtual void processCallback(
            DWORD pid, tstring processName, tstring processPath, tstring processCmdLine
        ) abstract;

        // 모니터링 중 탐지한 다음 정보를 가져옴
        // 정보가 없다면 False 리턴
        virtual BOOL getNextInfo(ProcessInfo& processInfo) abstract;
    };
};
