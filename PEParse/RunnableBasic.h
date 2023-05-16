#pragma once
#include <functional>
#include "IRunnableBasic.h"

namespace CommandLineUtils {
    class RunnableBasic : public virtual IRunnableLambda {
        CommandLambda m_call;

    public:
        RunnableBasic(CommandLambda call);
        BOOL run(vector<tstring>, ArgsAdditional) override;
    };
}