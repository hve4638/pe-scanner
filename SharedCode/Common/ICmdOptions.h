#pragma once
#include "Common/typedef.h"

namespace CmdLineUtils {
    interface ICmdOptions {
        virtual ~ICmdOptions() {}
        virtual BOOL has(TCHAR option) abstract;
        virtual tstring get(TCHAR option) abstract;
        virtual BOOL getBoolean(TCHAR option) abstract;
    };
}