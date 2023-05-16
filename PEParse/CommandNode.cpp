#include "CommandNode.h"

namespace CommandLineUtils {
    ICommandNodePtr CommandNode::make() {
        ICommandNodePtr node;
        new CommandNode(node);

        return node;
    }

    CommandNode::CommandNode(ICommandNodePtr& ptr) {
        ptr.reset(this);

        m_selfPtr = ptr;
    }

    shared_ptr<ICommandNode> CommandNode::findNodeForcefully(IArgsPtr args) {
        if (args->hasNext()) {
            auto key = args->next();
            if (!m_children.contains(key)) {
                ICommandNodePtr node = CommandNode::make();

                m_children[key] = node;
            }
            
            return m_children[key]->findNodeForcefully(args);
        }
        else {
            return getPtr();
        }
    }

    BOOL CommandNode::run(vector<tstring> args, ArgsAdditional additional) {
        if (isRunnable()) {
            vector<tstring> targs;
            m_runnable->run(args, additional);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL CommandNode::isRunnable() {
        return (m_runnable.use_count() > 0);
    }

    void CommandNode::setRunnable(shared_ptr<IRunnable> runnable) {
        m_runnable = runnable;
    }

    shared_ptr<ICommandNode> CommandNode::findNode(IArgsPtr args) {
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

    shared_ptr<ICommandNode> CommandNode::getPtr() {
        return m_selfPtr.lock();
    }

    void CommandNode::setPtr(weak_ptr<ICommandNode> wkPtr) {
        m_selfPtr = wkPtr;
    }
}