#include <format>
#include "Loader/PEScan.h"
#include "ScanEngine.h"

using namespace PEScan;

shared_ptr<ILogger> g_logger;

void fileWatchThread(shared_ptr<IFileWatch> fileWatch) {
    ScanEngine scanEngine;
    FileInfo fileInfo;
    tstring detectName;

    do
    {
        if (fileWatch->getNextInfo(fileInfo))
        {
            *g_logger << NL << format(_T("fileWatch : {:s}"), get<2>(fileInfo)) << NL;
            if (scanEngine.scanFile(get<2>(fileInfo), detectName, SCAN_TYPE_FILE))
            {
                tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<2>(fileInfo)) << endl;
            }
        }

        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (fileWatch->isAlive());
};

void processWatchThread(shared_ptr<IProcWatch> processWatch)
{
    ScanEngine scanEngine;
    ProcessInfo processInfo;
    tstring detectName;
    
    do
    {
        if (processWatch->getNextInfo(processInfo))
        {
            *g_logger << format(_T("threadWatch : {:s}"), get<2>(processInfo)) << NL;
            if (scanEngine.scanFile(get<2>(processInfo), detectName, SCAN_TYPE_FILE))
            {
                tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<2>(processInfo)) << endl;
            }
        }
        // 1000 ns == 0.001 ms
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (processWatch->isAlive());
};

void regWatchThread(shared_ptr<IRegWatch> regWatch)
{
    ScanEngine scanEngine;
    RegFileInfo regFileInfo;
    tstring detectName;

    do
    {
        if (regWatch->getNextInfo(regFileInfo))
        {
            *g_logger << format(_T("regWatch : {:s}"), get<0>(regFileInfo)) << NL;
            if (scanEngine.scanFile(get<0>(regFileInfo), detectName, SCAN_TYPE_FILE))
            {
                tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<0>(regFileInfo)) << endl;
            }
        }
        // 1000 ns == 0.001 ms
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (regWatch->isAlive());
};


void ipcProcThread(shared_ptr<IIPCUtil> ipcUtil)
{
    BOOL result = FALSE;
    ScanEngine scanEngine;
    IPC_MESSAGE ipcData;
    DWORD scanPID = 0;
    tstring scanPath;
    tstring detectName;
    IPCCommandType acceptCommandType = static_cast<IPCCommandType>(IPC_SCAN_FILE | IPC_SCAN_PID);

    while (ipcUtil->isAlive())
    {
        if (ipcUtil->getNextInfo(ipcData, acceptCommandType))
        {
            result = FALSE;

            if (ipcData.command == IPC_SCAN_FILE)
            {
                scanPath = reinterpret_cast<TCHAR*>(ipcData.data.data());
                if (scanEngine.scanFile(scanPath, detectName, static_cast<ScanFileType>(ipcData.scanType)))
                {
                    result = TRUE;
                }
            }
            else if (ipcData.command == IPC_SCAN_PID)
            {
                scanPID = (*(reinterpret_cast<PDWORD>(ipcData.data.data())));
                if (scanEngine.scanProcess(scanPID, detectName, static_cast<ScanFileType>(ipcData.scanType)))
                {
                    result = TRUE;
                }
            }
            if (result)
            {
                ipcData.command = IPC_SCAN_RESULT_DETECT;
                ipcData.scanType = 0;
                ipcData.size = static_cast<DWORD>(detectName.size() + 1) * sizeof(TCHAR);
                ipcData.data.assign(ipcData.size, 0x0);
                memcpy_s(ipcData.data.data(), ipcData.size, detectName.data(), ipcData.size);
            }
            else
            {
                ipcData.command = IPC_SCAN_RESULT_NOT_DETECT;
                ipcData.scanType = 0;
                ipcData.size = 0;
                ipcData.data.clear();
            }
            ipcUtil->sendMsg(ipcData);
        }
        // 1000 ns == 0.001 ms
        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    };
};

