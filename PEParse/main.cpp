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

using namespace CommandLineUtils;
using namespace PEParse;
using namespace PELog;
using namespace PEUtils;
using namespace LogUtils;

void testPrint();
void testScan();
void testLogging();

int _tmain(int argc, TCHAR* argv[]) {
    PECommandLineParser cmdRunner; 
    setlocale(LC_ALL, "");

    // ù ���ڴ� ���α׷� ����̹Ƿ� ����
    IArgsPtr args = make_shared<TCharArgs>(argc-1, argv+1);
    cmdRunner.runCommand(args);

    return 0;
}

void testLogging() {
    // ��� ������ �α׸� CONSOLE�� ����ϴ� logger ����
    Logger logger = { LogLevel::ALL, LogDirection::CONSOLE }; 
    {
        // DEBUG �α׷����� �޼��� ���
        logger << LogLevel::DEBUG;
        logger << _T("Debug Message") << NL;
    }
    {
        // ERR �α׷����� ErrorLogInfo ���
        // �޼���, LastError ��, ������ �߻��� �Լ� �̸�, ��ġ�� ��µ�
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

    tstring hash;
    parser.tryGetSectionHash(_T(".text"), hash);
    tcout << "hash: " << hash << endl;

    hash.clear();
    parser.tryGetCodeSectionHash(hash);
    tcout << "hash: " << hash << endl;

    hash.clear();
    parser.tryGetEntryPointSectionHash(hash);
    tcout << "hash: " << hash << endl;

    hash.clear();
    parser.tryGetPDBFilePathHash(hash);
    tcout << "hash: " << hash << endl;
}