#pragma once
#include "Common/typedef.h"
#include <queue>

namespace PEScan {
    // 프로세스 정보를 추출하여 저장하기 위한 형식 정의 (PID, 프로세스 이름, 프로세스 경로, 프로세스 커맨드라인)
    typedef tuple<DWORD, tstring, tstring, tstring> ProcessInfo;
    typedef queue<ProcessInfo> ProcessInfoList;
}