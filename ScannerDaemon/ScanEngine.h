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
    // 멀웨어 패턴 불러오기
    // 이후 DB 구축시 DB를 불러오는 부분
    void loadMalwarePattern(void);

    // PE 파일/프로세스의 해시 추출
    // 경우에 따라 영역 전체, 또는 코드영역만 가져오기도 함
    BOOL scanPE(ScanFileType scnaFileType, tstring& detectName);

public:
    static PEScanLoader* peScan;

    ScanEngine();
    ~ScanEngine();
    void Initialize(void);

    // 파일을 파싱해 멀웨어 패턴과 일치하는지 여부를 리턴
    BOOL scanFile(
        const tstring filePath, tstring& detectName, ScanFileType scanFileType = SCAN_TYPE_FILE
    );
    // 프로세스를 파싱해 멀웨어 패턴과 일치하는지 여부를 리턴
    BOOL scanProcess(
        const DWORD pid, tstring& detectName, ScanFileType scanFileType = SCAN_TYPE_FILE
    );
};