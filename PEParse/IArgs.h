#pragma once
#include "typedef.h"

namespace CommandLineUtils {
    interface IArgs {
        virtual tstring current() abstract;
        virtual tstring next() abstract;
        virtual BOOL hasNext() abstract;
        virtual tstring previous() abstract;
        virtual BOOL hasPrevious() abstract;
        virtual int offset() abstract;
        virtual shared_ptr<IArgs> copy() abstract;
    };

    typedef shared_ptr<IArgs> IArgsPtr;
}