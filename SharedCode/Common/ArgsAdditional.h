#pragma once
#include "Common/typedef.h"
#include "Common/IArgs.h"
#include "Common/ICmdOptions.h"

namespace CmdLineUtils {
    struct ArgsAdditional {
        shared_ptr<ICmdOptions> options;
        vector<tstring> cmd;
    };
}