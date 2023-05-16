#pragma once
#include <map>
#include "ICommandNode.h"

using namespace std;

namespace CommandLineUtils {
    class CommandNode : public ICommandNode {
        std::map<tstring, ICommandNodePtr> m_children;
        shared_ptr<IRunnable> m_runnable = NULL;
        tstring m_nodeName = _T("");
        weak_ptr<ICommandNode> m_selfPtr;

        void setPtr(weak_ptr<ICommandNode>) override;
    public:
        //직접 생성하지 않고 make를 사용해 생성할 것
        static ICommandNodePtr make();

        CommandNode(ICommandNodePtr&);
        BOOL run(vector<tstring>, ArgsAdditional) override;
        BOOL isRunnable() override;
        void setRunnable(shared_ptr<IRunnable> runnable) override;
        shared_ptr<ICommandNode> findNodeForcefully(IArgsPtr) override;
        shared_ptr<ICommandNode> findNode(IArgsPtr) override;
        shared_ptr<ICommandNode> getPtr() override;
    };
}