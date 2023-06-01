#include "Common/typedef.h"
#include <queue>

namespace PEScan {

    // 파일 이벤트 정의
    enum FileEvent
    {
        FILE_CREATED = 0x0,
        FILE_MODIFIED = 0x1,
        FILE_RENAMED_OLD = 0x2,
        FILE_RENAMED_NEW = 0x3,
        FILE_UNKNOWN_EVENT = 0xff
    };
    // 파일 정보를 추출하여 저장하기 위한 형식 정의 (파일 이름, 파일 확장자, 파일 경로, 파일 이벤트)
    typedef tuple<tstring, tstring, tstring, FileEvent> FileInfo;
    typedef queue<FileInfo> FileInfoList;

    // 탐지된 파일 정보를 저장하기 위한 형식 정의 (탐지명, 파일 경로)
    typedef tuple<tstring, tstring> DetectFileInfo;
    typedef vector<DetectFileInfo> DetectFileInfoList;

}