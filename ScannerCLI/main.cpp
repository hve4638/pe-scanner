#include <iostream>
#include "Common/LoadPEScan.h"

using namespace std;
using namespace PEScan;

int main() {
    if (!loadPEScan()) {
        cout << "load Fail" << endl;
        exit(0);
    }
    else if (!loadLogger()) {
        cout << "fail to load : Logger" << endl;
        exit(0);
    }

    auto logger = CreateLogger();
    logger->setLogDirection(LogDirection::CONSOLE);
    logger->setLogLevel(LogLevel::ALL);

    auto hi = _T("hi???");
    *logger << hi << NL;

    cout << "load success?" << endl;
}