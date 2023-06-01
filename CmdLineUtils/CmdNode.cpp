#include "CmdNode.h"

namespace CmdLineUtils {
    shared_ptr<ICmdNode> CmdNode::make() {
        ICommandNodePtr node;
        new CmdNode(node);

        return node;
    }

    CmdNode::CmdNode(ICommandNodePtr& ptr) {
        ptr.reset(this);

        m_selfPtr = ptr;
    }

    shared_ptr<ICmdNode> CmdNode::findNodeForcefully(IArgsPtr args) {
        if (args->hasNext()) {
            auto key = args->next();
            if (!m_children.contains(key)) {
                ICommandNodePtr node = CmdNode::make();

                m_children[key] = node;
            }

            return m_children[key]->findNodeForcefully(args);
        }
        else {
            return getPtr();
        }
    }

    BOOL CmdNode::run(vector<tstring> args, ArgsAdditional additional) {
        if (isRunnable()) {
            vector<tstring> targs;
            m_runnable->run(args, additional);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL CmdNode::isRunnable() {
        return (m_runnable.use_count() > 0);
    }

    void CmdNode::setRunnable(shared_ptr<IRunnable> runnable) {
        m_runnable = runnable;
    }

    shared_ptr<ICmdNode> CmdNode::findNode(IArgsPtr args) {
        if (args->hasNext()) {
            auto key = args->next();
            if (m_children.contains(key)) {
                return m_children[key]->findNode(args);
            }
            else {
                args->previous();
            }
        }

        return getPtr();
    }

    shared_ptr<ICmdNode> CmdNode::getPtr() {
        return m_selfPtr.lock();
    }

    void CmdNode::setPtr(weak_ptr<ICmdNode> wkPtr) {
        m_selfPtr = wkPtr;
    }
}