#pragma once
#include "Common/IFileWatch.h"
#include "Logger.h"

namespace PEScan {
    class FileWatch : public IFileWatch {
        Logger m_logger;
        FileInfoList m_fileInfoList;
        BOOL isWatching = FALSE;
        tstring m_rootDirectory;

        void watchFileStart(FileWatch* processWatch);

    public:
        FileWatch();
        ~FileWatch() override;
        BOOL isAlive(void) override;
        void startMonitoring(tstring rootDir) override;
        void stopMonitoring(void) override;
        void fileCallback(tstring fileName, tstring fileExt, tstring filePath, FileEvent fileEvent) override;
        BOOL getNextInfo(FileInfo& fileInfo) override;
    };
};