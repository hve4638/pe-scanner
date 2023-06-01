#include "loaddll.h"

using namespace PEScan;

int main() {
    setlocale(LC_ALL, "");

    PEScanLoader peScan = { _T("./PEScan.dll") };
    loadPEScan(peScan);


}