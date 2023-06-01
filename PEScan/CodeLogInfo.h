#pragma once
#include "Common/typedef.h"
#include "Common/ILogInfo.h"

namespace PEScan {
    class CodeLogInfo : public ILogInfo {
        tstring m_message = _T("");
        DWORD m_errorCode = 0;
        const char* m_fname;
        int m_fline;
        BOOL m_detail = false;

    public:
        CodeLogInfo(tstring message, DWORD errorCode = GetLastError(), const char* __fname = __builtin_FUNCTION(), const int __fline = __builtin_LINE());
        tstring message() override;

        CodeLogInfo& detail();
    };
}