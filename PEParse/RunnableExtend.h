#pragma once
#include <functional>
#include <set>
#include <map>
#include "RunnableBasic.h"
#include "ArgsAdditional.h"

namespace CommandLineUtils {
    typedef function<void(
        vector<tstring>,
        ArgsAdditional
    )> CommandWithOptionCallback;

    class RunnableExtend : public virtual IRunnable {
        set<TCHAR> m_traceOption;
        set<TCHAR> m_traceBooleanOption;
        CommandWithOptionCallback m_call;

    public:
        RunnableExtend(CommandWithOptionCallback call);
        RunnableExtend& traceOptions(tstring);
        RunnableExtend& traceBooleanOptions(tstring);
        BOOL run(vector<tstring>, ArgsAdditional) override;
    };
}