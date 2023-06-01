#include "ExceptionLogInfo.h"
#include <format>

namespace PEScan {
    ExceptionLogInfo::ExceptionLogInfo(const std::exception& ex, DWORD errorCode,
        const char* __fname, const int __fline) {
        m_what = ex.what();
        m_errorCode = errorCode;
        m_fname = __fname;
        m_fline = __fline;
    }

    tstring ExceptionLogInfo::message() {
        tstring message;

        string what = m_what;
        tstring twhat = tstring().assign(what.begin(), what.end());
        if (m_detail) {
            string fname = m_fname;
            tstring tfname = tstring().assign(fname.begin(), fname.end());

            message = format(_T("Function : {:s}({:d}), Code : 0x{:x}({:d}), Error: {:s}"),
                tfname,
                m_fline,
                m_errorCode,
                m_errorCode,
                twhat
            );
        }
        else {
            message = format(_T("Error: {:s}, (Code: {:x})"),
                twhat,
                m_errorCode
            );
        }

        return message;
    }
}