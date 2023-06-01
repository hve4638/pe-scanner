#include "Loader/PEScan.h"

using namespace PEScan;

void fileWatchThread(shared_ptr<IFileWatch> fileWatch) {
    FileInfo fileInfo;
    tstring detectName;

    do
    {
        if (fileWatch->getNextInfo(fileInfo))
        {
            if (scanEngine.scanFile(get<2>(fileInfo), detectName, SCAN_TYPE_FILE))
            {
                tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<2>(fileInfo)) << endl;
            }
        }

        this_thread::sleep_for(std::chrono::nanoseconds(1000));
    } while (fileWatch->isAlive());
};

void processWatchThread(CProcessWatch* processWatch)
{
    CScanEngine scanEngine;
    ProcessInfo processInfo;
    tstring detectName;

    if (processWatch != NULL)
    {
        do
        {
            if (processWatch->getNextInfo(processInfo))
            {
                if (scanEngine.scanFile(get<2>(processInfo), detectName, SCAN_TYPE_FILE))
                {
                    tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<2>(processInfo)) << endl;
                }
            }
            // 1000 ns == 0.001 ms
            this_thread::sleep_for(std::chrono::nanoseconds(1000));
        } while (processWatch->isAlive());
    }
};

void regWatchThread(CRegWatch* regWatch)
{
    CScanEngine scanEngine;
    RegFileInfo regFileInfo;
    tstring detectName;

    if (regWatch != NULL)
    {
        do
        {
            if (regWatch->getNextInfo(regFileInfo))
            {
                if (scanEngine.scanFile(get<0>(regFileInfo), detectName, SCAN_TYPE_FILE))
                {
                    tcout << endl << format(_T("{:s} detected!\n{:s}"), detectName, get<0>(regFileInfo)) << endl;
                }
            }
            // 1000 ns == 0.001 ms
            this_thread::sleep_for(std::chrono::nanoseconds(1000));
        } while (regWatch->isAlive());
    }
};