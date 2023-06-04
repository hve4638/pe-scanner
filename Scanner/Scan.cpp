#include "Scan.h"

using namespace std;
using namespace filesystem;

void ScanFile(const tstring filePath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList)
{
    IPC_MESSAGE ipcSendData;
    IPC_MESSAGE ipcRecvData;

    // 파일 검사 요청을 위한 정보 설정
    ipcSendData.command = IPC_SCAN_FILE;
    ipcSendData.scanType = SCAN_TYPE_FILE;
    ipcSendData.size = static_cast<DWORD>(filePath.size() + 1) * sizeof(TCHAR);
    ipcSendData.data.assign(ipcSendData.size, 0x0);
    memcpy_s(ipcSendData.data.data(), ipcSendData.size, filePath.data(), ipcSendData.size);

    while (!ipcUtil.isAvailable())
    {
        Sleep(0);
    }

    // 파일 검사 요청
    ipcUtil.sendMsg(ipcSendData);

    IPCCommandType acceptCommandType = static_cast<IPCCommandType>(IPC_SCAN_RESULT_DETECT | IPC_SCAN_RESULT_NOT_DETECT);
    do
    {
        // 검사 결과 수신 대기
        if (ipcUtil.getNextInfo(ipcRecvData, acceptCommandType))
        {
            if (ipcRecvData.command == IPC_SCAN_RESULT_DETECT)
            {
                detectFileList.push_back(make_tuple(reinterpret_cast<TCHAR*>(ipcRecvData.data.data()), filePath));
            }
            break;
        }
        Sleep(0);
    } while (TRUE);
};

void ScanProcess(const DWORD pid, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList)
{
    IPC_MESSAGE ipcSendData;
    IPC_MESSAGE ipcRecvData;

    // 파일 검사 요청을 위한 정보 설정
    ipcSendData.command = IPC_SCAN_PID;
    ipcSendData.scanType = SCAN_TYPE_CODE;  // 프로세스의 경우 메모리에 적재 되면서 PE 내의 많은 데이터가 변경되기 때문에 코드 영역만 검사해야 함 
    ipcSendData.size = sizeof(DWORD);
    ipcSendData.data.assign(ipcSendData.size, 0x0);
    memcpy_s(ipcSendData.data.data(), ipcSendData.size, &pid, ipcSendData.size);

    while (!ipcUtil.isAvailable())
    {
        Sleep(0);
    }

    // 파일 검사 요청
    ipcUtil.sendMsg(ipcSendData);

    do
    {
        // 검사 결과 수신 대기
        if (ipcUtil.getNextInfo(ipcRecvData, static_cast<IPCCommandType>(IPC_SCAN_RESULT_DETECT | IPC_SCAN_RESULT_NOT_DETECT)))
        {
            if (ipcRecvData.command == IPC_SCAN_RESULT_DETECT)
            {
                detectFileList.push_back(make_tuple(reinterpret_cast<TCHAR*>(ipcRecvData.data.data()), format(_T("{:d}"), pid)));
            }
            break;
        }
        Sleep(0);
    } while (TRUE);
};

void ScanDirectoryThread(const tstring dirPath, BOOL& scanContinue, DWORD& currentFileCount, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList)
{
    tstring filePath;

    try
    {
        // 최상위 디렉토리 설정
        path parent(dirPath);

        // Traverse all directories and files recursively
        for (const auto& entry : recursive_directory_iterator(parent, directory_options::skip_permission_denied))
        {
            try
            {
                if (entry.is_regular_file())
                {
                    filePath = entry.path().c_str();
                    currentFileCount++;

                    // 10개 파일 마다 화면에 검사 중 파일 출력 (성능 문제)
                    if ((currentFileCount % 10) == 0)
                    {
                        tcout << format(_T("Scan file [{:d}] : {:s}"), currentFileCount, filePath) << endl;
                    }
                    ScanFile(filePath, ipcUtil, detectFileList);
                }
                // 1000 ns == 0.001 ms
                this_thread::sleep_for(std::chrono::nanoseconds(1000));
            }
            catch (...)
            {
                continue;
            }
            if (!scanContinue)
            {
                // 스캔 중지
                break;
            }
        }
    }
    catch (...)
    {
    }
    scanContinue = FALSE;
    ExitThread(0);
};

void ScanDirectory(const tstring dirPath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList)
{
    HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD irInBuf = { 0, };
    DWORD readLength = 0;
    DWORD currentFileCount = 0;
    BOOL scanContinue = TRUE;

    thread scanDirectoryThread = thread([&]() { ScanDirectoryThread(dirPath, scanContinue, currentFileCount, ipcUtil, detectFileList); });
    scanDirectoryThread.detach();

    // Thread 종료시 까지 대기 (중간에 사용자 입력이 있으면 종료)
    while (scanContinue)
    {
        DWORD rc = WaitForSingleObject(stdinHandle, 100);
        if (rc == WAIT_TIMEOUT)
        {
            continue;
        }
        else if (rc == WAIT_OBJECT_0)
        {
            if (ReadConsoleInput(stdinHandle, &irInBuf, 1, &readLength))
            {
                if (irInBuf.EventType == KEY_EVENT)
                {
                    scanContinue = FALSE;
                    Sleep(100);
                }
            }
        }
        Sleep(1);
    }
    tcout << endl << format(_T("Scan directory complete : {:d} files scanned, {:s}"), currentFileCount, dirPath) << endl;
}