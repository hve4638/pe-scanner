#pragma once
#include "Common/FileTypedef.h"

namespace PEScan {
    interface IFileWatch {
        virtual ~IFileWatch() {}
        virtual BOOL isAlive(void) abstract;
        virtual void startMonitoring(tstring rootDir) abstract;
        virtual void stopMonitoring(void) abstract;
        virtual void fileCallback(tstring fileName, tstring fileExt, tstring filePath, FileEvent fileEvent) abstract;
        virtual BOOL getNextInfo(FileInfo& fileInfo) abstract;
    };
};