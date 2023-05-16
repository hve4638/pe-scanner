#include "ArgsAdditional.h"
#include "CommandLineParser.h"
#include "CommandNode.h"
#include "TStringArgs.h"
#include "RunnableExtend.h"

namespace CommandLineUtils {
    CommandLineParser::CommandLineParser() {
        m_root = CommandNode::make();
    }

    void CommandLineParser::runCommand(tstring str) {
        runCommand(makeArgs(str));
    }

    void CommandLineParser::runCommand(IArgsPtr args) {
        auto original = args->copy();
        auto node = m_root->findNode(args);

        if (node->isRunnable()) {
            vector<tstring> rawArgs;
            vector<tstring> cmd;
            
            while (original->offset() < args->offset()) {
                cmd.push_back(original->next());
            }
            while (args->hasNext()) {
                rawArgs.push_back(args->next());
            }

            ArgsAdditional additional;
            additional.cmd = cmd;
            node->run(rawArgs, additional);
        }
    }

    void CommandLineParser::addCommand(IArgsPtr cmd, IRunnablePtr runnable) {
        auto node = m_root->findNodeForcefully(cmd);

        if (!node->isRunnable()) {
            node->setRunnable(runnable);
        }
    }

    void CommandLineParser::addCommand(IArgsPtr cmd, CMDCallback callback, tstring options, tstring booleanOptions) {
        auto runnable = make_shared<RunnableExtend>(callback);
        runnable->traceOptions(options);
        runnable->traceBooleanOptions(booleanOptions);

        addCommand(cmd, runnable);
    }

    void CommandLineParser::addCommand(tstring cmd, CMDCallback callback, tstring options, tstring booleanOptions) {
        addCommand(makeArgs(cmd), callback, options, booleanOptions);
    }
}