#pragma once
#include "Common/typedef.h"
#include "Common/PEElement.h"

namespace PEScan {
    interface IPEParser {
        virtual ~IPEParser() {};
        virtual void close() abstract;
        virtual BOOL parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) abstract;
        virtual shared_ptr<PEStructure> getPEStructure() abstract;
        virtual BOOL tryGetSectionHash(const TCHAR* sectionName, tstring& hash) abstract;
        virtual BOOL tryGetEntryPointSectionHash(tstring& hash) abstract;
        virtual BOOL tryGetPDBFilePathHash(tstring& hash) abstract;
        virtual BOOL tryGetCodeSectionHash(tstring& hash) abstract;
    };
}
