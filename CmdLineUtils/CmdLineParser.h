#pragma once
#include "Common/ICmdLineParser.h"
#include "Common/ICmdNode.h"
#include <functional>

namespace CmdLineUtils {
    typedef function<void(vector<tstring>, ArgsAdditional)> CmdCallback;

    class CmdLineParser : public ICmdLineParser {
        shared_ptr<ICmdNode> m_root;

    public:
        CmdLineParser();
        void run(IArgsPtr cmd) override;
        void add(IArgsPtr cmd, shared_ptr<IRunnable> runnable) override;

        void run(tstring cmd) override;
        void add(tstring cmd, CmdCallback callback, tstring options = _T(""), tstring booleanOptions = _T("")) override;
    };
}