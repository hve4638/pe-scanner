#pragma once
#include "api.h"
#include "Common/ICmdLineParser.h"

namespace CmdLineUtils {
    extern "C" EXPORT_API ICmdLineParser* createCmdLineParser();
}
