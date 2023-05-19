#include <iostream>
#include <string>
#include "Logger.h"
#include "PEParser.h"
#include "PEPrinter.h"
#include "PEUtils.h"
#include "HashMD5Utils.h"
#include "TCharArgs.h"
#include "TStringArgs.h"
#include "CommandNode.h"
#include "CommandLineParser.h"
#include "PECommandLineParser.h"
#include "RunnableExtend.h"
#include "FileUtil.h"
#include "PETests.h"

using namespace CommandLineUtils;
using namespace PEParse;
using namespace PELog;
using namespace PEUtils;
using namespace LogUtils;

void runCommandLine(int argc, TCHAR* argv[]) {
    PECommandLineParser cmdRunner;

    IArgsPtr args = make_shared<TCharArgs>(argc - 1, argv + 1);
    cmdRunner.runCommand(args);
}

void runInteractive() {
    PECommandLineParser cmdRunner;
    string input;
    while (true) {
        tcout << _T("> ");
        if (std::getline(cin, input)) {
            tstring tinput = copyStringToTString(input);
            cmdRunner.runCommand(tinput);
        }
        else {
            break;
        }
    }
}

int _tmain(int argc, TCHAR* argv[]) {
    setlocale(LC_ALL, "");
    testReg();
    //testPrint();
    //testFileScan();
    return 0;

    if (argc == 1) {
        runInteractive();
    }
    else {
        runCommandLine(argc, argv);
    }

    return 0;
}

