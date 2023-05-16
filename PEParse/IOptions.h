#pragma once
#include "typedef.h"

namespace CommandLineUtils {
    interface IOptions {
        virtual ~IOptions() {}
        virtual BOOL has(TCHAR option) abstract;
        virtual tstring get(TCHAR option) abstract;
        virtual BOOL getBoolean(TCHAR option) abstract;
    };

    typedef shared_ptr<IOptions> IOptionsPtr;
}