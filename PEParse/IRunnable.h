#pragma once
#include "typedef.h"
#include "IArgs.h"
#include "ArgsAdditional.h"

namespace CommandLineUtils {
    interface IRunnable {
        virtual ~IRunnable() {};
        virtual BOOL run(vector<tstring>, ArgsAdditional) abstract;
    };

    typedef shared_ptr<IRunnable> IRunnablePtr;
}