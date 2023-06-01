#include "Common/typedef.h"
#include <queue>

namespace PEScan {

    // ���� �̺�Ʈ ����
    enum FileEvent
    {
        FILE_CREATED = 0x0,
        FILE_MODIFIED = 0x1,
        FILE_RENAMED_OLD = 0x2,
        FILE_RENAMED_NEW = 0x3,
        FILE_UNKNOWN_EVENT = 0xff
    };
    // ���� ������ �����Ͽ� �����ϱ� ���� ���� ���� (���� �̸�, ���� Ȯ����, ���� ���, ���� �̺�Ʈ)
    typedef tuple<tstring, tstring, tstring, FileEvent> FileInfo;
    typedef queue<FileInfo> FileInfoList;

    // Ž���� ���� ������ �����ϱ� ���� ���� ���� (Ž����, ���� ���)
    typedef tuple<tstring, tstring> DetectFileInfo;
    typedef vector<DetectFileInfo> DetectFileInfoList;

}