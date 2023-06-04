#pragma once
#include "Common/typedef.h"

namespace PEScan {
    interface IPEPrinter {
        virtual ~IPEPrinter() {}
        virtual void reset(shared_ptr<PEStructure>) abstract;   // 출력할 PE 구조체 지정
        virtual void printPEStructure() abstract;               // PE 구조체 출력
    };
};

