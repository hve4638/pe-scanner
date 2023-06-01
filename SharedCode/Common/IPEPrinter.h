#pragma once
#include "Common/typedef.h"

namespace PEScan {
    interface IPEPrinter {
        virtual ~IPEPrinter() {}
        virtual void reset(shared_ptr<PEStructure>) abstract;

        virtual void printPEStructure() abstract;
    };
};

