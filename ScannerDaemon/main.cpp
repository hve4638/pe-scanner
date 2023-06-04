#include "loaddll.h"
#include "ScanEngine.h"
#include "watch.h"

using namespace std;
using namespace PEScan;

int main() {
    setlocale(LC_ALL, "");

    // PEScan.dll �ε�
    PEScanLoader peScan = { _T("./PEScan.dll") };
    loadPEScan(peScan);

    HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD irInBuf = { 0, };
    DWORD readLength = 0;
    shared_ptr<IProcWatch> processWatch = peScan.ProcWatch();
    shared_ptr<IFileWatch> fileWatch = peScan.FileWatch();
    shared_ptr<IRegWatch> regWatch = peScan.RegWatch();
    shared_ptr<IFileSearch> fileSearch = peScan.FileSearch();
    shared_ptr<IIPCUtil> ipcUtil = peScan.IPCUtil();

    // Ŭ���� static ������ PEScan�δ� ����
    ScanEngine::peScan = &peScan;
    
    // ���������� Logger ����
    g_logger = peScan.Logger();
    g_logger->setLogDirection(LogDirection::DEBUGVIEW);
    g_logger->setLogLevel(LogLevel::ALL);
    *g_logger << LogLevel::ALL;

    // ���μ��� ����
    processWatch->startMonitoring();
    thread processThread = thread([&]() { processWatchThread(processWatch); });
    processThread.detach();

    // C����̺� ����
    fileWatch->startMonitoring(_T("\\\\?\\c:\\")); // "\\?\c:\" 
    thread fileThread = thread([&]() { fileWatchThread(fileWatch); });
    fileThread.detach();

    // �������α׷� ������Ʈ�� ����
    regWatch->startMonitoring(REG_HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
    thread regThread = thread([&]() { regWatchThread(regWatch); });
    regThread.detach();

    // IPC ����
    ipcUtil->start(IPC_NAME, TRUE);
    thread ipcThread = thread([&]() { ipcProcThread(ipcUtil); });
    ipcThread.detach();

    // ���
    tcout << endl << _T("Scan daemon is running....\nPress any key to exit.") << endl;
    while (TRUE)
    {
        char input[15];
        cin >> input;
        if (*input != NULL) break;
        Sleep(1);
    }

    // �ڿ� ����, ����
    ipcUtil->stop();
    processWatch->stopMonitoring();
    fileWatch->stopMonitoring();
    regWatch->stopMonitoring();
}