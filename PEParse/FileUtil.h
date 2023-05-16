#pragma once
#include "typedef.h"
#include "Logger.h"

using namespace LogUtils;

typedef void(* ScanFileCallback)(const tstring scanFilePath);

class FileUtil {
    Logger m_logger = { LogLevel::ALL, LogDirection::DEBUGVIEW };

public:
    FileUtil();
    ~FileUtil();
    BOOL search(tstring parentDirectory, DWORD& currentFileCount, ScanFileCallback scanFunction = NULL);
};