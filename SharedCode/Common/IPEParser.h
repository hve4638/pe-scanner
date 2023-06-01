#pragma once
#include "Common/typedef.h"
#include "Common/PEElement.h"

namespace PEScan {
    interface IPEParser {
        virtual ~IPEParser() {};
        virtual void close() abstract;
        virtual BOOL parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) abstract;
        virtual shared_ptr<PEStructure> getPEStructure() abstract;
    };
}
