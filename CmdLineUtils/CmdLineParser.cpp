
#include "Runnable.h"
#include "Common/ArgsAdditional.h"
#include "StringArgs.h"
#include "CmdLineParser.h"
#include "CmdNode.h"

namespace CmdLineUtils {
    CmdLineParser::CmdLineParser() {
        m_root = CmdNode::make();
    }

    void CmdLineParser::run(IArgsPtr args) {
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

    void CmdLineParser::add(shared_ptr<IArgs> cmd, shared_ptr<IRunnable> runnable) {
        auto node = m_root->findNodeForcefully(cmd);

        if (!node->isRunnable()) {
            node->setRunnable(runnable);
        }
    }

    void CmdLineParser::run(tstring str) {
        run(StringArgs::make(str));
    }

    void CmdLineParser::add(tstring cmd, CmdCallback callback, tstring options, tstring booleanOptions) {
        auto args = StringArgs::make(cmd);
        auto runnable = make_shared<Runnable>(callback);
        runnable->traceOptions(options);
        runnable->traceBooleanOptions(booleanOptions);

        add(args, runnable);
    }
}