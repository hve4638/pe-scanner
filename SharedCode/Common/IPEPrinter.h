#pragma once
#include "Common/typedef.h"

namespace PEScan {
    interface IPEPrinter {
        virtual ~IPEPrinter() {}
        virtual void reset(shared_ptr<PEStructure>) abstract;   // ����� PE ����ü ����
        virtual void printPEStructure() abstract;               // PE ����ü ���
    };
};

