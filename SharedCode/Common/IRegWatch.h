#pragma once
#include "Common/typedef.h"
#include "Common/RegTypedef.h"

namespace PEScan {
    interface IRegWatch {
        virtual ~IRegWatch() {}

        // 모니터링 중인지 확인
        virtual BOOL isAlive() abstract;

        // 주어진 레지스트리 경로에 대해 모니터링 시작
        // 모니터링는 새로운 스레드에서 일어남
        virtual void startMonitoring(RegRootKey rootKey, tstring subKey) abstract;

        // 모니터링 중이라면, 모니터링 종료 후 자원 정리
        virtual void stopMonitoring() abstract;

        // 내부적으로 사용, 감시 중 추가/제거 등의 활동 탐지시 호출됨
        virtual void regCallback(tstring filePath, RegPathInfo regPathInfo) abstract;

        // 모니터링 중 탐지한 다음 정보를 가져옴
        // 정보가 없다면 False 리턴
        virtual BOOL getNextInfo(RegFileInfo& regFileInfo) abstract;
    };
};