#include <iostream>
#include <string>
#include "loaddll.h"
#include "testdll.h"
#include "Scan.h"

using namespace std;
using namespace PEScan;
using namespace CmdLineUtils;

static shared_ptr<IIPCUtil> ipcUtil;
DetectFileInfoList detectFileList;

void setCmd(shared_ptr<ICmdLineParser> cmdParser); 

int _tmain(int argc, TCHAR* argv[]) {
    // PEScan.dll cmdLineUtils.dll 로드
    CmdLineUtilsLoader cmdUtils = { _T("./CmdLineUtils.dll") };
    PEScanLoader peScan = { _T("./PEScan.dll") };
    loadPEScan(peScan);
    loadCmdLineUtils(cmdUtils);

    setlocale(LC_ALL, "");
    
    ipcUtil = peScan.IPCUtil();

    // 명령 세팅
    auto cmdParser = cmdUtils.CmdLineParser();
    setCmd(cmdParser);
    
    if (argc == 1) { //인터렉티브 모드
        while (1) {
            tstring tinput;
            string input;
            tcout << "> ";
            getline(cin, input);
            tinput.assign(input.begin(), input.end());

            cmdParser->run(tinput);
        }
    }
    else {
        cmdParser->run(argc - 1, argv + 1);
    }

    ipcUtil.reset();

    return 0;
}

void setCmd(shared_ptr<ICmdLineParser> cmdParser) {
    auto callScan = [](vector<tstring> args, ArgsAdditional add) {
        tcout << _T("Scanning...") << endl;
        tstring parameter;
        auto options = add.options;
        auto f = options->has(_T('f'));
        auto p = options->has(_T('p'));
        auto d = options->has(_T('d'));
        if (ipcUtil->start(IPC_NAME, FALSE)) {
            if (f) {
                parameter = options->get(_T('f'));
                ScanFile(parameter, *ipcUtil, detectFileList);
            }
            else if (p) {
                parameter = options->get(_T('p'));
                ScanProcess(wcstoul(parameter.c_str(), 0, 10), *ipcUtil, detectFileList);
            }
            else if (d) {
                parameter = options->get(_T('d'));
                ScanDirectory(parameter, *ipcUtil, detectFileList);
            }
        }
        ipcUtil->stop();
        if (!detectFileList.empty()) {
            for (auto const& element : detectFileList) {
                tcout << endl << get<0>(element) << _T(" detected!");
                tcout << endl << _T("    >> ") << get<1>(element) << endl;
            }
        }
    };

    // 명령어
    // scan -p <pid>        : 특정 프로세스 스캔 및 멀웨어 탐지
    // scan -f <파일명>     : 특정 파일 스캔 및 멀웨어 탐지
    // scan -g <경로>       : 경로 내에 있는 파일 스캔 및 멀웨어 탐지
    cmdParser->add(
        _T("scan"), // "scan" 명령어를 받으면 처리
        callScan,   // 실행 함수
        _T("fdp")   // -f -d -p 옵션을 처리
    );

    cmdParser->add(
        _T(""),
        [](vector<tstring> args, ArgsAdditional add) {
            tcout << "Using:" << endl;
            tcout << "    scan -f <file>" << endl;
            tcout << "    scan -d <directory>" << endl;
            tcout << "    scan -p <pid>" << endl;
        }
    );
}
