#pragma once
#include "Common/typedef.h"
#include "Loader/PEScan.h"
#include "ScanTypedef.h"
#include <map>

using namespace std;
using namespace PEScan;

class ScanEngine {
    shared_ptr<ILogger> m_logger;
    shared_ptr<IPEParser> m_peParser;
    shared_ptr<IFileSearch> m_fileSearch;
    map<tstring, tstring> m_malwarePattern;
    BOOL initialized = FALSE;

private:
    void loadMalwarePattern(void);
    BOOL scanPE(ScanFileType scnaFileType, tstring& detectName);

public:
    ScanEngine();
    ~ScanEngine();
    void Initialize(void);
    BOOL scanFile(const tstring filePath, tstring& detectName, ScanFileType scnaFileType = SCAN_TYPE_FILE);
    BOOL scanProcess(const DWORD pid, tstring& detectName, ScanFileType scnaFileType = SCAN_TYPE_FILE);
};