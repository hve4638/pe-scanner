#include "Common/typedef.h"

namespace PEScan {
    // 프로세스간 통신을 위한 명령 타입 정의
    enum IPCCommandType
    {
        IPC_SCAN_NONE = 0x0,
        IPC_SCAN_FILE = 0x1,
        IPC_SCAN_PID = 0x2,
        IPC_SCAN_RESULT_DETECT = 0x4,
        IPC_SCAN_RESULT_NOT_DETECT = 0x8,
        IPC_SCAN_UNKNOWN = 0xff
    };


    // 파일 스캔 시 검사 영역을 지정하기 위한 타입 정의
    enum ScanFileType
    {
        SCAN_TYPE_NONE = 0x0,
        SCAN_TYPE_FILE = 0x1,
        SCAN_TYPE_PROCESS = 0x2,
        SCAN_TYPE_CODE = 0x4,
        SCAN_TYPE_PDB = 0x8
    };


    // 프로세스간 통신을 위한 데이터 타입 정의
    typedef struct _IPC_MESSAGE
    {
        WORD command;
        DWORD scanType;
        DWORD size;
        BinaryData data;
    } IPC_MESSAGE, * PIPC_MESSAGE;
}