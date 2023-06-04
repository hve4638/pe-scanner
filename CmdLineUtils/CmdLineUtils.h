#pragma once
#include "api.h"
#include "Common/ICmdLineParser.h"

namespace CmdLineUtils {
    // 명령줄 해석 및 실행 클래스
    extern "C" EXPORT_API ICmdLineParser* createCmdLineParser();
}
