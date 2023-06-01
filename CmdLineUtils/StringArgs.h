#pragma once
#include "Common/IArgs.h"

namespace CmdLineUtils {
    class StringArgs : public IArgs {
        vector<tstring> m_vector;
        int m_current = -1;
        SIZE_T m_argc = 0;

        BOOL isValidIndex(int index);
    public:
        StringArgs(vector<tstring>);
        StringArgs(const StringArgs&);

        static shared_ptr<StringArgs> make(tstring);

        tstring current() override;
        tstring next() override;
        BOOL hasNext() override;
        tstring previous() override;
        BOOL hasPrevious() override;
        shared_ptr<IArgs> copy() override;
        int offset() override;
    };
}
