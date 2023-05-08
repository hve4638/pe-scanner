#include "CommandLineParser.h"
#include "CommandNode.h"
#include "TStringArgs.h"

namespace CommandLineUtils {
    CommandLineParser::CommandLineParser() {
        m_root = CommandNode::make();
    }

    void CommandLineParser::runCommand(IArgsPtr args) {
        auto wkPtr = m_root->findNode(args);
        shared_ptr<ICommandNode> node = wkPtr.lock();

        if (node && node->isRunnable()) {
            node->run(args);
        }
        else {
            
            //m_logger << 
        }
    }

    void CommandLineParser::runCommand(tstring str) {
        shared_ptr<TStringArgs> args = makeArgs(str);

        runCommand(args);
    }

    void CommandLineParser::addCommand(IArgsPtr args, shared_ptr<IRunnable> runnable) {
        // root������ �� ���ڿ� �ش��ϴ� ��带 ã�Ƽ� �����ϰ�, ���ٸ� �����ؼ� �����Ѵ�.
        auto wkPtr = m_root->findNodeForcefully(args);
        shared_ptr<ICommandNode> node = wkPtr.lock();

        if (node && !node->isRunnable()) {
            node->setRunnable(runnable);
        }
    }

    void CommandLineParser::addCommand(tstring str, shared_ptr<IRunnable> runnable) {
        shared_ptr<TStringArgs> args = makeArgs(str);

        addCommand(args, runnable);
    }

    void CommandLineParser::addCommand(shared_ptr<IArgs> args, CommandLambda lambda) {
        shared_ptr<RunnableLambda> runnable = make_shared<RunnableLambda>(lambda);

        addCommand(args, runnable);
    }

    void CommandLineParser::addCommand(tstring str, CommandLambda runnable) {
        shared_ptr<TStringArgs> args = makeArgs(str);

        addCommand(args, runnable);
    }
}