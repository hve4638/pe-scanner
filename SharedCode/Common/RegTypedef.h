#include "Common/typedef.h"
#include <queue>

namespace PEScan {
    // ������Ʈ�� ��Ʈ Ű ����
    enum RegRootKey
    {
        REG_HKEY_CLASSES_ROOT = reinterpret_cast<ULONGLONG>(HKEY_CLASSES_ROOT),
        REG_HKEY_LOCAL_MACHINE = reinterpret_cast<ULONGLONG>(HKEY_LOCAL_MACHINE),
        REG_HKEY_CURRENT_USER = reinterpret_cast<ULONGLONG>(HKEY_CURRENT_USER),
        REG_HKEY_USERS = reinterpret_cast<ULONGLONG>(HKEY_USERS),
    };

    // ������Ʈ�� ����� ���� �����ϱ� ���� ���� ���� (������Ʈ�� �� Ÿ��, ������Ʈ�� �� ������)
    typedef tuple<DWORD, BinaryData> RegValue;

    // �˻��� ������Ʈ�� ��� ����� ���� ���� ���� (������Ʈ�� Key �̸�, ������Ʈ�� Value �̸�(optional), ������Ʈ�� ��Ʈ Ű)
    typedef tuple<tstring, tstring, RegRootKey> RegPathInfo;
    typedef vector<RegPathInfo> RegPathList;

    // ������Ʈ���� �����ϴ� ���� ��θ� �����Ͽ� �����ϱ� ���� ���� ���� (���� ���, (������Ʈ�� Key �̸�, ������Ʈ�� Value �̸�, ������Ʈ�� ��Ʈ Ű))
    typedef tuple<tstring, tstring, tstring, RegRootKey> RegFileInfo;
    typedef queue<RegFileInfo> RegFileInfoList;
    typedef vector<RegFileInfo> RegFileList;

}