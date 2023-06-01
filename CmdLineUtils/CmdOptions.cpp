#include "CmdOptions.h"

namespace CmdLineUtils {
    BOOL CmdOptions::has(TCHAR option) {
        return m_options.find(option) != m_options.end();
    }
    tstring CmdOptions::get(TCHAR option) {
        return has(option) ? m_options[option] : _T("");
    }
    BOOL CmdOptions::getBoolean(TCHAR option) {
        return m_optionsBoolean.contains(option);
    }
    void CmdOptions::set(TCHAR option, tstring value) {
        m_options.insert({ option, value });
    }
    void CmdOptions::setBoolean(TCHAR option, BOOL enabled) {
        if (enabled) {
            m_optionsBoolean.insert(option);
        }
        else if (m_optionsBoolean.contains(option)) {
            m_optionsBoolean.erase(option);
        }
    }
}