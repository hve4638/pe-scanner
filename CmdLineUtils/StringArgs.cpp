#include "StringArgs.h"

static vector<tstring> splitString(tstring str) {
    vector<basic_string<TCHAR>> tokens;
    basic_string<TCHAR> temp;

    for (const auto& ch : str) {
        if (ch != _T(' ')) {
            temp += ch;
        }
        else if (!temp.empty()) {
            tokens.push_back(temp);
            temp.clear();
        }
    }
    if (!temp.empty()) {
        tokens.push_back(temp);
    }
    return tokens;
}

namespace CmdLineUtils {
    shared_ptr<StringArgs> StringArgs::make(tstring str) {
        auto tokens = splitString(str);

        return make_shared<StringArgs>(tokens);
    }

    StringArgs::StringArgs(const StringArgs& copy) {
        m_argc = copy.m_argc;
        m_vector = copy.m_vector;
    }

    StringArgs::StringArgs(vector<tstring> argv) {
        m_argc = argv.size();
        m_vector = argv;
    }

    tstring StringArgs::current() {
        auto index = this->m_current;

        if (isValidIndex(index)) {
            return m_vector.at(index);
        }
        else {
            return _T("");
        }
    }

    tstring StringArgs::next() {
        m_current++;
        return current();
    }

    BOOL StringArgs::hasNext() {
        return isValidIndex(this->m_current + 1);
    }

    tstring StringArgs::previous() {
        m_current--;
        return current();
    }

    BOOL StringArgs::hasPrevious() {
        return isValidIndex(this->m_current - 1);
    }

    inline BOOL StringArgs::isValidIndex(int index) {
        return (0 <= index && index < m_argc);
    }

    int StringArgs::offset() {
        return m_current;
    }

    IArgsPtr StringArgs::copy() {
        return make_shared<StringArgs>(*this);
    }


}










