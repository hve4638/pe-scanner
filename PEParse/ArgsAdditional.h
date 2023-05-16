#pragma once
#include "typedef.h"
#include "IArgs.h"
#include "IOptions.h"

namespace CommandLineUtils {
    struct ArgsAdditional {
        IOptionsPtr options;
        vector<tstring> cmd;
    };
}