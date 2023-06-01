#pragma once
#include "testdll.h"
#include <format>
#include <thread>
#include <chrono>
#include <filesystem>

using namespace std;

void testLogger(PEScanLoader& peScan) {
    shared_ptr<ILogger> logger = peScan.Logger();
    *logger << peScan.CodeLogInfo(_T("Log Test"), 0, TRUE) << NL;
}

void testScan(PEScanLoader& peScan) {
    shared_ptr<IPEParser> peParser = peScan.PEParser();
    shared_ptr<IPEPrinter> pePrinter = peScan.PEPrinter();
    if (peParser->parsePE(0, _T("D:/DetectMe.exe"), PE_PARSE_HEADER)) {
        pePrinter->reset(peParser->getPEStructure());

        pePrinter->printPEStructure();
    }
}

void testCmd(CmdLineUtilsLoader& cmdUtils) {
    shared_ptr<ICmdLineParser> cmdLineParser = cmdUtils.CmdLineParser();

    cmdLineParser->add(
        _T("echo"),
        [](vector<tstring> args, ArgsAdditional add) {
            tcout << "echo > ";
            for (tstring arg : args) {
                tcout << arg << _T(" ");
            }
            tcout << endl;
            tcout << _T("-i : ") << add.options->get(_T('i')) << endl;

        }, 
        _T("i")
    );
    cmdLineParser->run(_T("echo hello world -i 1234"));
}
void testFileSearch(PEScanLoader& peScan) {
    auto fileSearch = peScan.FileSearch();

    DWORD count = 0;
    fileSearch->search(_T("D:\\Images\\"), count,
        [](tstring fname) {
            tcout << _T("search : ") << fname << endl;
        }
    );
    tcout << _T("count : ") << count << endl;
}

void testRegWatch(PEScanLoader& peScan) {
    auto regWatch = peScan.RegWatch();
    RegFileInfo regFileInfo;

    regWatch->startMonitoring(REG_HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
    do {
        if (regWatch->getNextInfo(regFileInfo)) {
            tcout << format(_T("[RegWatch] Watched : {:s}"), get<0>(regFileInfo)) << endl;
        }
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (regWatch->isAlive());
}

void testProcWatch(PEScanLoader& peScan) {
    ProcessInfo processInfo;
    auto procWatch = peScan.ProcWatch();

    procWatch->startMonitoring();

    do {
        if (procWatch->getNextInfo(processInfo)) {
            tcout << format(_T("[*ProcWatch] Watch : {:s}"), get<2>(processInfo)) << endl;
        }
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (procWatch->isAlive());
}

void testFileWatch(PEScanLoader& peScan) {
    ProcessInfo processInfo;
    FileInfo fileInfo;
    auto fileWatch = peScan.FileWatch();

    fileWatch->startMonitoring(_T("\\\\?\\d:\\"));

    do {
        if (fileWatch->getNextInfo(fileInfo)) {
            tcout << format(_T("[FileWatch] Watch : {:s}"), get<2>(fileInfo)) << endl;
        }
        // Sleep(0);
        // 1000 ns == 0.001 ms
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (fileWatch->isAlive());
}