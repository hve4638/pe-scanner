#pragma once
#include "typedef.h"
#include "IArgs.h"
#include "IRunnable.h"
#include "ArgsAdditional.h"


namespace CommandLineUtils {
    typedef void (*CMDCallback)(vector<tstring>, ArgsAdditional);

    interface ICommandLineParser {
        virtual void runCommand(IArgsPtr) abstract;
        virtual void addCommand(IArgsPtr cmd, IRunnablePtr runnable) abstract;
        virtual void addCommand(IArgsPtr path, CMDCallback callback, tstring options, tstring booleanOptions) abstract;
    };
}
