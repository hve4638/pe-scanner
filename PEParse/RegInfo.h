#pragma once
#include "typedef.h"

namespace RegUtils {

    enum RegRootKey {
        REG_HKEY_CLASSES_ROOT = reinterpret_cast<QWORD>(HKEY_CLASSES_ROOT),
        REG_HKEY_LOCAL_MACHINE = reinterpret_cast<QWORD>(HKEY_LOCAL_MACHINE),
        REG_HKEY_CURRENT_USER = reinterpret_cast<QWORD>(HKEY_CURRENT_USER),
        REG_HKEY_USERS = reinterpret_cast<QWORD>(HKEY_USERS),
    };

    enum FileEvent {
        FILE_CREATED = 0x0,
        FILE_MODIFIED = 0x1,
        FILE_RENAMED = 0x2,
        FILE_UNKNOWN_EVENT = 0xffff
    };

    struct RegPathInfo {
        tstring name;
        tstring value;
        RegRootKey key;
    };

    // 바이너리 데이터 저장을 위한 형식 정의
    typedef vector<BYTE> BinaryData;

    struct RegValue {
        DWORD valueType;
        BinaryData value;
    };

    // 레지스트리에 존재하는 파일 경로를 추출하여 저장하기 위한 형식 정의 (파일 경로, (레지스트리 Key 이름, 레지스트리 Value 이름, 레지스트리 루트 키))
    struct RegFileInfo {
        tstring path;
        RegPathInfo info;
    };
}