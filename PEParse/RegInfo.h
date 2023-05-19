#pragma once
#include "typedef.h"
#include <queue>

namespace RegInfo {
    // ������Ʈ�� ��Ʈ Ű ����
    enum RegRootKey
    {
        REG_HKEY_CLASSES_ROOT = reinterpret_cast<ULONGLONG>(HKEY_CLASSES_ROOT),
        REG_HKEY_LOCAL_MACHINE = reinterpret_cast<ULONGLONG>(HKEY_LOCAL_MACHINE),
        REG_HKEY_CURRENT_USER = reinterpret_cast<ULONGLONG>(HKEY_CURRENT_USER),
        REG_HKEY_USERS = reinterpret_cast<ULONGLONG>(HKEY_USERS),
    };

    // ���� �̺�Ʈ ����
    enum FileEvent
    {
        FILE_CREATED = 0x0,
        FILE_MODIFIED = 0x1,
        FILE_RENAMED = 0x2,
        FILE_UNKNOWN_EVENT = 0xffff
    };

    typedef vector<BYTE> BinaryData;

    // ������Ʈ�� ����� ���� �����ϱ� ���� ���� ���� (������Ʈ�� �� Ÿ��, ������Ʈ�� �� ������)
    typedef tuple<DWORD, BinaryData> RegValue;

    // �˻��� ������Ʈ�� ��� ����� ���� ���� ���� (������Ʈ�� Key �̸�, ������Ʈ�� Value �̸�(optional), ������Ʈ�� ��Ʈ Ű)
    typedef tuple<tstring, tstring, RegRootKey> RegPathInfo;
    typedef vector<RegPathInfo> RegPathList;

    // ������Ʈ���� �����ϴ� ���� ��θ� �����Ͽ� �����ϱ� ���� ���� ���� (���� ���, (������Ʈ�� Key �̸�, ������Ʈ�� Value �̸�, ������Ʈ�� ��Ʈ Ű))
    typedef tuple<tstring, tuple<tstring, tstring, RegRootKey>> RegFileInfo;
    typedef queue<RegFileInfo> RegFileInfoList;
    typedef vector<RegFileInfo> RegFileList;

    // ���μ��� ������ �����Ͽ� �����ϱ� ���� ���� ���� (PID, ���μ��� �̸�, ���μ��� ���, ���μ��� Ŀ�ǵ����)
    typedef tuple<DWORD, tstring, tstring, tstring> ProcessInfo;
    typedef queue<ProcessInfo> ProcessInfoList;

    // ���� ������ �����Ͽ� �����ϱ� ���� ���� ���� (���� �̸�, ���� Ȯ����, ���� ���, ���� �̺�Ʈ)
    typedef tuple<tstring, tstring, tstring, FileEvent> FileInfo;
    typedef queue<FileInfo> FileInfoList;
}