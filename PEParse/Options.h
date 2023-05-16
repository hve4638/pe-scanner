#pragma once
#include <map>
#include <set>
#include "IOptions.h"

namespace CommandLineUtils {
    class Options : public IOptions {
        map<TCHAR, tstring> m_options;
        set<TCHAR> m_optionsBoolean;

    public:
        BOOL has(TCHAR option) override;
        tstring get(TCHAR option) override;
        BOOL getBoolean(TCHAR option) override;
        void set(TCHAR option, tstring value);
        void setBoolean(TCHAR option, BOOL enabled);
    };
}