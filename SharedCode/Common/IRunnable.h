#pragma once
#include <functional>
#include "Common/typedef.h"
#include "Common/IArgs.h"
#include "ArgsAdditional.h"

namespace CmdLineUtils {
    typedef function<void(vector<tstring>, ArgsAdditional)> CmdCallback;

    interface IRunnable {
        virtual ~IRunnable() {};
        virtual BOOL run(vector<tstring>, ArgsAdditional) abstract;
    };
}