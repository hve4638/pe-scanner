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

    // 프로세스간 통신을 위한 데이터 타입 정의
    typedef struct _IPC_MESSAGE
    {
        WORD command;
        DWORD scanType;
        DWORD size;
        BinaryData data;
    } IPC_MESSAGE, * PIPC_MESSAGE;
}