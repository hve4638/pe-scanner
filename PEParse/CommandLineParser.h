#pragma once
#include "ICommandLineParser.h"
#include "ICommandNode.h"
#include "RunnableBasic.h"
#include "Logger.h"
#include <functional>
using namespace LogUtils;

namespace CommandLineUtils {
    typedef void (*CMDCallback)(vector<tstring>, ArgsAdditional);

    class CommandLineParser : public ICommandLineParser {
        shared_ptr<ICommandNode> m_root;
        Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };

    public:
        CommandLineParser();
        void runCommand(IArgsPtr cmd) override;
        void runCommand(tstring cmd);
        void addCommand(IArgsPtr cmd, IRunnablePtr runnable) override;
        void addCommand(IArgsPtr cmd, CMDCallback callback, tstring options = _T(""), tstring booleanOptions = _T("")) override;
        void addCommand(tstring cmd, CMDCallback callback, tstring options = _T(""), tstring booleanOptions = _T(""));
    };
}