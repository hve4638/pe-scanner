#include "Common/LoadPEScan.h"

namespace PEScan {
    static HMODULE hModule;
    CreateLoggerFunc CreateLogger = NULL;

    bool loadPEScan() {
        if ((hModule = LoadLibrary(_T("./PEScan.dll"))) == NULL) {
            return false;
        }

        return true;
    }

    void unloadPEScan() {

    }

    bool loadLogger() {
        if ((CreateLogger = (CreateLoggerFunc)GetProcAddress(hModule, "CreateLogger")) == NULL) {
            return false;
        }
        else {
            return true;
        }
    }
}