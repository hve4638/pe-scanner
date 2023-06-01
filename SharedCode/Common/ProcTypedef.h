#pragma once
#include "Common/typedef.h"
#include <queue>

namespace PEScan {
    // ���μ��� ������ �����Ͽ� �����ϱ� ���� ���� ���� (PID, ���μ��� �̸�, ���μ��� ���, ���μ��� Ŀ�ǵ����)
    typedef tuple<DWORD, tstring, tstring, tstring> ProcessInfo;
    typedef queue<ProcessInfo> ProcessInfoList;
}