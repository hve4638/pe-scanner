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
    CmdLineUtilsLoader cmdUtils = { _T("./CmdLineUtils.dll") };;
    PEScanLoader peScan = { _T("./PEScan.dll") };

    setlocale(LC_ALL, "");
    loadPEScan(peScan);
    loadCmdLineUtils(cmdUtils);

    ipcUtil = peScan.IPCUtil();

    auto cmdParser = cmdUtils.CmdLineParser();
    setCmd(cmdParser);

    if (argc == 1) { //���ͷ�Ƽ�� ���
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
    cmdParser->add(
        _T("scan"),
        [](vector<tstring> args, ArgsAdditional add) {
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
                    tcout << endl << get<0>(element) << _T(" detected!") << endl << _T("    >> ") << get<1>(element) << endl;
                }
            }
        },
        _T("fdp")
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
