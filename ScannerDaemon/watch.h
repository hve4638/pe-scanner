#pragma once
#include "Loader/PEScan.h"

using namespace PEScan;

extern shared_ptr<ILogger> g_logger;

// �־��� ��ο��� ������ ����,�̵� � ���� ����
// ������ ������ �ֿ��� ���ϰ� ��ġ�ϴ��� Ž��
void fileWatchThread(shared_ptr<IFileWatch>);

// ���μ����� ��ȭ ����
// ���μ����� ������ �ֿ��� ���ϰ� ��ġ�ϴ��� Ž��
void processWatchThread(shared_ptr<IProcWatch>);

// �־��� ������Ʈ�� ����� ��������� ����
// �߰��� ����� ������ ������ �ֿ��� ���ϰ� ��ġ�ϴ��� Ž��
void regWatchThread(shared_ptr<IRegWatch>);

// IPC �޼��� ���� ���
// Scanner.exe �κ��� �޼��� ���� ���
void ipcProcThread(shared_ptr<IIPCUtil>);