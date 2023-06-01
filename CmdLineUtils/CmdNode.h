#pragma once
#include "Common/ICmdNode.h"
#include <map>

namespace CmdLineUtils {
    class CmdNode : public ICmdNode {
        std::map<tstring, ICommandNodePtr> m_children;
        shared_ptr<IRunnable> m_runnable = NULL;
        tstring m_nodeName = _T("");
        weak_ptr<ICmdNode> m_selfPtr;

        void setPtr(weak_ptr<ICmdNode>) override;
    public:
        static ICommandNodePtr make();

        CmdNode(ICommandNodePtr&);
        BOOL run(vector<tstring>, ArgsAdditional) override;
        BOOL isRunnable() override;
        void setRunnable(shared_ptr<IRunnable> runnable) override;
        shared_ptr<ICmdNode> findNodeForcefully(IArgsPtr) override;
        shared_ptr<ICmdNode> findNode(IArgsPtr) override;
        shared_ptr<ICmdNode> getPtr() override;
    };
}