#pragma once
#include "api.h"
#include "Common/ICmdLineParser.h"

namespace CmdLineUtils {
    // ����� �ؼ� �� ���� Ŭ����
    extern "C" EXPORT_API ICmdLineParser* createCmdLineParser();
}
