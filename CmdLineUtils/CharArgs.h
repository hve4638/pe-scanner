#pragma once
#include "Common/IArgs.h"

namespace CmdLineUtils {
    class CharArgs : public IArgs {
        TCHAR** m_argv = NULL;
        int m_current = -1;
        int m_argc = 0;

        BOOL isValidIndex(int index);
    public:
        CharArgs(int argc, TCHAR* argv[]);
        CharArgs(const CharArgs&);

        tstring current() override;
        tstring next() override;
        BOOL hasNext() override;
        tstring previous() override;
        BOOL hasPrevious() override;
        int offset() override;
        IArgsPtr copy() override;
    };
}