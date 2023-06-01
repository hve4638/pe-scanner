#include "Runnable.h"
#include "CmdOptions.h"
//#include "TStringArgs.h"

namespace CmdLineUtils {
    Runnable::Runnable(CmdCallback call) {
        m_call = call;
    }

    Runnable& Runnable::traceOptions(tstring optionCharacters) {
        for (const auto& c : optionCharacters) {
            m_traceOption.insert(c);
        }

        return *this;
    }

    Runnable& Runnable::traceBooleanOptions(tstring optionCharacters) {
        for (const auto& c : optionCharacters) {
            m_traceBooleanOption.insert(c);
        }

        return *this;
    }

    BOOL Runnable::run(vector<tstring> args, ArgsAdditional additional) {
        BOOL optionMode = FALSE;
        tstring optionFullWord = _T("");
        TCHAR optionKey = _T('\0');

        vector<tstring> rargs;
        auto options = make_shared<CmdOptions>();
        for (const tstring next : args) {
            if (optionMode) {
                options->set(optionKey, next);
                optionMode = FALSE;
            }
            else if (!next.empty() && next.front() == _T('-')) {
                TCHAR last = next[next.length() - 1];

                if (m_traceOption.contains(last)) {
                    optionMode = TRUE;
                    optionFullWord = next;
                    optionKey = last;
                }
                else if (m_traceBooleanOption.contains(last)) {
                    options->setBoolean(last, TRUE);
                }
                else {
                    rargs.push_back(next);
                }
            }
            else {
                rargs.push_back(next);
            }
        }

        additional.options = options;
        m_call(rargs, additional);
        return FALSE;
    }
}