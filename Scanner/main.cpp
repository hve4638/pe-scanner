#include <iostream>
#include "loaddll.h"
#include "testdll.h"

using namespace std;
using namespace PEScan;
using namespace CmdLineUtils;


int main() {
    CmdLineUtilsLoader cmdUtils = { _T("./CmdLineUtils.dll") };;
    PEScanLoader peScan = { _T("./PEScan.dll") };

    setlocale(LC_ALL, "");
    loadPEScan(peScan);
    loadCmdLineUtils(cmdUtils);

    //testLogger();
    //testCmd();
    //testFileSearch(peScan);
    //testProcWatch(peScan);
    //testFileWatch(peScan);
    //auto pa = _T("D:\\Project\\Git\\HEMacro\\x64\\Debug");
    auto pa = _T("E:\\230531\\HEMacro\\Debug\\HEMacro.dll");
    //auto pa = _T("D:\\Project\\Git\\HEMacro\\x64\\Debug\\HEMacro.dll");
    auto parser = peScan.PEParser();
    auto printer = peScan.PEPrinter();
    parser->parsePE(0, pa, PE_PARSE_ALL);
    
    auto pestruct = parser->getPEStructure();
    printer->reset(pestruct);
    printer->printPEStructure();

    return 0;
}
