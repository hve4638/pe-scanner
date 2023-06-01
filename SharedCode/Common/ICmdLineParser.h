#pragma once
#include "Common/typedef.h"
#include "Common/IArgs.h"
#include "Common/IRunnable.h"
#include "Common/ArgsAdditional.h"

namespace CmdLineUtils {
    interface ICmdLineParser {
        virtual void run(IArgsPtr) abstract;
        virtual void add(IArgsPtr cmd, shared_ptr<IRunnable> runnable) abstract;

        virtual void run(tstring) abstract;
        virtual void add(tstring path, CmdCallback callback,
            tstring options = _T(""), tstring booleanOptions = _T("")) abstract;
    };
}
