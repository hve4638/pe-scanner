#include "CodeLogInfo.h"
#include <format>

namespace PEScan{
    CodeLogInfo::CodeLogInfo(tstring message, DWORD errorCode, const char* __fname, const int __fline) {
        m_message = message;
        m_errorCode = errorCode;
        m_fname = __fname;
        m_fline = __fline;
    }

    tstring CodeLogInfo::message() {
        tstring message;

        if (m_detail) {
            string fname = m_fname;
            tstring tfname = tstring().assign(fname.begin(), fname.end());

            message = format(_T("Function = {:s}({:d}), Code = 0x{:x}({:d}), Msg = {:s}"),
                tfname,
                m_fline,
                m_errorCode,
                m_errorCode,
                m_message
            );
        }
        else {
            message = format(_T("Code = 0x{:x}({:d}), Msg = {:s}"),
                m_errorCode,
                m_errorCode,
                m_message
            );
        }

        return message;
    }

    CodeLogInfo& CodeLogInfo::detail() {
        m_detail = TRUE;
        return *this;
    }
}