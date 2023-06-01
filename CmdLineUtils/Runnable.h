#pragma once
#include "Common/IRunnable.h"
#include "Common/ArgsAdditional.h"
#include <set>
//#include <map>

namespace CmdLineUtils {
    class Runnable : public IRunnable {
        set<TCHAR> m_traceOption;
        set<TCHAR> m_traceBooleanOption;
        CmdCallback m_call;

    public:
        Runnable(CmdCallback);
        Runnable& traceOptions(tstring);
        Runnable& traceBooleanOptions(tstring);
        BOOL run(vector<tstring>, ArgsAdditional) override;
    };
}