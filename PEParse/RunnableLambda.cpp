#include "RunnableBasic.h"

namespace CommandLineUtils {
    RunnableBasic::RunnableBasic(CommandLambda call) {
        m_call = call;
    }

    BOOL RunnableBasic::run(vector<tstring> args, ArgsAdditional additional) {
        m_call(args);

        return TRUE;
    }
}