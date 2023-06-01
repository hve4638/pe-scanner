#pragma once
#include "typedef.h"
#include "IRunnable.h"

namespace CmdLineUtils {
    interface ICmdNode {
        virtual ~ICmdNode() {};
        virtual BOOL run(vector<tstring>, ArgsAdditional) abstract;
        virtual BOOL isRunnable() abstract;
        virtual void setRunnable(shared_ptr<IRunnable> runnable) abstract;
        virtual shared_ptr<ICmdNode> findNodeForcefully(shared_ptr<IArgs>) abstract;
        virtual shared_ptr<ICmdNode> findNode(shared_ptr<IArgs>) abstract;
        virtual shared_ptr<ICmdNode> getPtr() abstract;
        virtual void setPtr(weak_ptr<ICmdNode>) abstract;
    };

    typedef shared_ptr<ICmdNode> ICommandNodePtr;
}