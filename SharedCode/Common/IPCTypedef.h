#include "Common/typedef.h"

namespace PEScan {
    // ���μ����� ����� ���� ��� Ÿ�� ����
    enum IPCCommandType
    {
        IPC_SCAN_NONE = 0x0,
        IPC_SCAN_FILE = 0x1,
        IPC_SCAN_PID = 0x2,
        IPC_SCAN_RESULT_DETECT = 0x4,
        IPC_SCAN_RESULT_NOT_DETECT = 0x8,
        IPC_SCAN_UNKNOWN = 0xff
    };

    // ���μ����� ����� ���� ������ Ÿ�� ����
    typedef struct _IPC_MESSAGE
    {
        WORD command;
        DWORD scanType;
        DWORD size;
        BinaryData data;
    } IPC_MESSAGE, * PIPC_MESSAGE;
}