#include "CharArgs.h"

namespace CmdLineUtils {
    CharArgs::CharArgs(int argc, TCHAR* argv[]) {
        m_argc = argc;
        m_argv = argv;
    }

    CharArgs::CharArgs(const CharArgs& source) {
        m_argc = source.m_argc;
        m_argv = source.m_argv;
    }

    tstring CharArgs::current() {
        auto index = this->m_current;
        if (isValidIndex(index)) {
            return m_argv[index];
        }
        else {
            return _T("");
        }
    }

    tstring CharArgs::next() {
        m_current++;
        return current();
    }

    BOOL CharArgs::hasNext() {
        return isValidIndex(this->m_current + 1);
    }

    inline BOOL CharArgs::isValidIndex(int index) {
        return (0 <= index && index < m_argc);
    }

    tstring CharArgs::previous() {
        m_current--;
        return current();
    }

    BOOL CharArgs::hasPrevious() {
        return isValidIndex(this->m_current - 1);
    }

    int CharArgs::offset() {
        return m_current;
    }

    IArgsPtr CharArgs::copy() {
        return make_shared<CharArgs>(*this);
    }
}