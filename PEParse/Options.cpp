#include "Options.h"


namespace CommandLineUtils {
    BOOL Options::has(TCHAR option) {
        return m_options.find(option) != m_options.end();
    }
    tstring Options::get(TCHAR option) {
        return has(option) ? m_options[option] : _T("");
    }
    BOOL Options::getBoolean(TCHAR option) {
        return m_optionsBoolean.contains(option);
    }
    void Options::set(TCHAR option, tstring value) {
        m_options.insert({ option, value });
    }
    void Options::setBoolean(TCHAR option, BOOL enabled) {
        if (enabled) {
            m_optionsBoolean.insert(option);
        }
        else if (m_optionsBoolean.contains(option)) {
            m_optionsBoolean.erase(option);
        }
    }
}