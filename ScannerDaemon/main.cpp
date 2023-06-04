#include "loaddll.h"
#include "ScanEngine.h"
#include "watch.h"

using namespace std;
using namespace PEScan;

int main() {
    setlocale(LC_ALL, "");

    // PEScan.dll 로드
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

    // 클래스 static 변수에 PEScan로더 저장
    ScanEngine::peScan = &peScan;
    
    // 전역변수에 Logger 저장
    g_logger = peScan.Logger();
    g_logger->setLogDirection(LogDirection::DEBUGVIEW);
    g_logger->setLogLevel(LogLevel::ALL);
    *g_logger << LogLevel::ALL;

    // 프로세스 감시
    processWatch->startMonitoring();
    thread processThread = thread([&]() { processWatchThread(processWatch); });
    processThread.detach();

    // C드라이브 감시
    fileWatch->startMonitoring(_T("\\\\?\\c:\\")); // "\\?\c:\" 
    thread fileThread = thread([&]() { fileWatchThread(fileWatch); });
    fileThread.detach();

    // 시작프로그램 레지스트리 감시
    regWatch->startMonitoring(REG_HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));
    thread regThread = thread([&]() { regWatchThread(regWatch); });
    regThread.detach();

    // IPC 생성
    ipcUtil->start(IPC_NAME, TRUE);
    thread ipcThread = thread([&]() { ipcProcThread(ipcUtil); });
    ipcThread.detach();

    // 대기
    tcout << endl << _T("Scan daemon is running....\nPress any key to exit.") << endl;
    while (TRUE)
    {
        char input[15];
        cin >> input;
        if (*input != NULL) break;
        Sleep(1);
    }

    // 자원 정리, 종료
    ipcUtil->stop();
    processWatch->stopMonitoring();
    fileWatch->stopMonitoring();
    regWatch->stopMonitoring();
}