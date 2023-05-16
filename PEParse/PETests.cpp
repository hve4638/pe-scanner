#pragma once
#include "PETests.h"
#include "Logger.h"
#include "PECommandLineParser.h"
#include "PEParser.h"
#include "PEPrinter.h"
#include "FileUtil.h"

using namespace CommandLineUtils;
using namespace PEParse;
using namespace PELog;
using namespace LogUtils;

void testLogging() {
    // 모든 레벨의 로그를 CONSOLE로 출력하는 logger 생성
    Logger logger = { LogLevel::ALL, LogDirection::CONSOLE };
    {
        // DEBUG 로그레벨로 메세지 출력
        logger << LogLevel::DEBUG;
        logger << _T("Debug Message") << NL;
    }
    {
        // ERR 로그레벨로 ErrorLogInfo 출력
        // 메세지, LastError 값, 에러가 발생한 함수 이름, 위치가 출력됨
        logger << LogLevel::ERR;
        logger << ErrorLogInfo(_T("Debug Message"), GetLastError(), ERRLOG_EXTRA_FUNCTION) << NL;
    }
}

void testScan() {
    PECommandLineParser cmdRunner;

    cmdRunner.runCommand(_T("scan file C:\\Temp\\DetectMe.exe 60754a02d83c8dca4384b1f2bdeb86a8"));
    cmdRunner.runCommand(_T("scan file  C:\\Temp\\DetectMe.exe 26978c26dfd84a5645d0190214bbada7"));
}

void testPrint() {
    PEParser parser;
    PEPrinter printer;
    auto path = _T("C:\\Windows\\System32\\shell32.dll");

    parser.parsePEFile(path);

    auto pe = parser.getPEStructure();

    tcout << "sizeOfHeaders : " << pe->ntHeader64.OptionalHeader.SizeOfHeaders << endl;
    tcout << "baseOfCode: " << pe->ntHeader64.OptionalHeader.BaseOfCode << endl;

    printer.reset(pe);
    printer.printPEStructure();


    tcout << _T("<hash>") << endl;
    tstring hash;
    parser.tryGetSectionHash(_T(".text"), hash);
    tcout << " .text section - " << hash << endl;

    hash.clear();
    parser.tryGetCodeSectionHash(hash);
    tcout << " Code section - " << hash << endl;

    hash.clear();
    parser.tryGetEntryPointSectionHash(hash);
    tcout << " EntryPoint section - " << hash << endl;

    hash.clear();
    parser.tryGetPDBFilePathHash(hash);
    tcout << " PDBFilePath - " << hash << endl;
}

void testFileScan() {
    ScanFileCallback scanCallback = [](const tstring scanFilePath) {
        PEParser peParser;

        if (peParser.parsePE(NULL, scanFilePath.c_str())) {
            tcout << scanFilePath << endl;
        }
        else {
            tcout << _T("(skip) ") << scanFilePath << endl;
        }
    };

    FileUtil fileUtil;
    DWORD count = 0;
    fileUtil.search(_T("C:\\$Recycle.Bin\\"), count, scanCallback);
    //fileUtil.search(_T("C:\\Temp\\"), count, scanCallback);
}