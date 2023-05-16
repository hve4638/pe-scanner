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

    // ���̳ʸ� ������ ������ ���� ���� ����
    typedef vector<BYTE> BinaryData;

    struct RegValue {
        DWORD valueType;
        BinaryData value;
    };

    // ������Ʈ���� �����ϴ� ���� ��θ� �����Ͽ� �����ϱ� ���� ���� ���� (���� ���, (������Ʈ�� Key �̸�, ������Ʈ�� Value �̸�, ������Ʈ�� ��Ʈ Ű))
    struct RegFileInfo {
        tstring path;
        RegPathInfo info;
    };
}