#pragma once
#include <thread>
#include <chrono>
#include <filesystem>
#include "FileWatch.h"

using namespace std;

namespace PEScan {

    FileWatch::FileWatch()
    {
    };

    FileWatch::~FileWatch(void)
    {
        stopMonitoring();
    };

    BOOL FileWatch::isAlive(void)
    {
        return isWatching;
    };

    void FileWatch::watchFileStart(FileWatch* fileWatch)
    {
        if (fileWatch != NULL)
        {
            HANDLE dirHadle = CreateFile(fileWatch->m_rootDirectory.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS,  // FILE_FLAG_BACKUP_SEMANTICS 필수
                NULL
            );
            if (dirHadle == INVALID_HANDLE_VALUE)
            {
                m_logger.log(_T("Failed to open directory."), GetLastError(), LOG_LEVEL_ERROR);
            }
            else
            {
                FileEvent fileEvent = FILE_UNKNOWN_EVENT;
                DWORD bytesReturned = 0;
                DWORD watchBufferSize = 32 * 1024;
                DWORD nameBufferSize = 8192;
                tstring fileFullPath;
                tstring fileExt;
                std::vector<BYTE> nameBuffer(nameBufferSize);
                std::vector<BYTE> watchBuffer(watchBufferSize);

                while (fileWatch->isWatching)
                {
                    BOOL result = ReadDirectoryChangesW(
                        dirHadle,
                        watchBuffer.data(),
                        watchBufferSize,
                        TRUE,   // watch sub directory
                        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION,
                        &bytesReturned,
                        NULL,
                        NULL
                    );
                    if (!result)
                    {
                        m_logger.log(_T("Error reading directory changes."), GetLastError(), LOG_LEVEL_ERROR);
                        break;
                    }
                    else
                    {
                        if (bytesReturned == 0)
                        {
                            m_logger.log(_T("No changes detected."), GetLastError(), LOG_LEVEL_DEBUG);
                            continue;
                        }
                    }
                    FILE_NOTIFY_INFORMATION* pfni = (FILE_NOTIFY_INFORMATION*)watchBuffer.data();
                    while (TRUE)
                    {
                        switch (pfni->Action)
                        {
                        case FILE_ACTION_ADDED:
                            fileEvent = FILE_CREATED;
                            break;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            fileEvent = FILE_RENAMED_NEW;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            // fileEvent = FILE_RENAMED_OLD;
                            fileEvent = FILE_UNKNOWN_EVENT;
                            break;
                        default:
                            fileEvent = FILE_UNKNOWN_EVENT;
                            break;
                        }
                        if (fileEvent != FILE_UNKNOWN_EVENT)
                        {
                            if (nameBuffer.size() < (pfni->FileNameLength + sizeof(TCHAR)))
                            {
                                nameBufferSize = pfni->FileNameLength + sizeof(TCHAR);
                                nameBuffer.resize(static_cast<size_t>(nameBufferSize));
                            }
                            memset(nameBuffer.data(), 0, nameBuffer.size());
                            memcpy(nameBuffer.data(), reinterpret_cast<PBYTE>(pfni->FileName), static_cast<size_t>(pfni->FileNameLength));
                            filesystem::path filePath(reinterpret_cast<TCHAR*>(nameBuffer.data()));
                            fileFullPath = fileWatch->m_rootDirectory.c_str();
                            fileFullPath += filePath.native().c_str();
                            fileExt = filePath.extension().c_str();
                            if (!fileExt.empty())
                            {
                                if (fileExt.front() == _T('.'))
                                {
                                    fileExt.erase(0, 1);
                                }
                            }
                            fileWatch->fileCallback(filePath.filename().c_str(), fileExt, fileFullPath.c_str(), fileEvent);
                        }
                        if (pfni->NextEntryOffset == 0)
                        {
                            // pfni->NextEntryOffset가 0일 때도 마지막 항목이 저장되어 있기 때문에 확인 지점 설정이 중요
                            break;
                        }
                        pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
                    }
                    // 1000 ns == 0.001 ms
                    this_thread::sleep_for(std::chrono::nanoseconds(1000));
                }
                CloseHandle(dirHadle);
            }
        }
        ExitThread(0);
    };

    void FileWatch::fileCallback(tstring fileName, tstring fileExt, tstring filePath, FileEvent fileEvent)
    {
        m_fileInfoList.push(make_tuple(fileName, fileExt, filePath, fileEvent));
    };

    void FileWatch::startMonitoring(tstring rootDir)
    {
        if (isWatching)
        {
            stopMonitoring();
        }
        isWatching = TRUE;
        m_rootDirectory = rootDir;
        thread fileWatchThread = thread([&]() { watchFileStart(this); });
        fileWatchThread.detach();
    };

    void FileWatch::stopMonitoring(void)
    {
        if (isWatching)
        {
            isWatching = FALSE;
            Sleep(100);
        }
    };

    BOOL FileWatch::getNextInfo(FileInfo& fileInfo)
    {
        BOOL result = !m_fileInfoList.empty();

        if (result)
        {
            fileInfo = m_fileInfoList.front();
            m_fileInfoList.pop();
        }
        return result;
    };

};