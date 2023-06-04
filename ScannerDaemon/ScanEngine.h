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
    // �ֿ��� ���� �ҷ�����
    // ���� DB ����� DB�� �ҷ����� �κ�
    void loadMalwarePattern(void);

    // PE ����/���μ����� �ؽ� ����
    // ��쿡 ���� ���� ��ü, �Ǵ� �ڵ念���� �������⵵ ��
    BOOL scanPE(ScanFileType scnaFileType, tstring& detectName);

public:
    static PEScanLoader* peScan;

    ScanEngine();
    ~ScanEngine();
    void Initialize(void);

    // ������ �Ľ��� �ֿ��� ���ϰ� ��ġ�ϴ��� ���θ� ����
    BOOL scanFile(
        const tstring filePath, tstring& detectName, ScanFileType scanFileType = SCAN_TYPE_FILE
    );
    // ���μ����� �Ľ��� �ֿ��� ���ϰ� ��ġ�ϴ��� ���θ� ����
    BOOL scanProcess(
        const DWORD pid, tstring& detectName, ScanFileType scanFileType = SCAN_TYPE_FILE
    );
};