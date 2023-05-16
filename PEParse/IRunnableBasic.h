#pragma once
#include <functional>
#include "IRunnable.h"

namespace CommandLineUtils {
    typedef function<void(vector<tstring>)> CommandLambda;

    interface IRunnableLambda : public virtual IRunnable {
        virtual ~IRunnableLambda() {};
    };
}