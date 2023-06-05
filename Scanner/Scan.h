#pragma once
#include "Loader/PEScan.h"
#include <thread>
#include <chrono>
#include <format>
#include <string>
#include <filesystem>

using namespace PEScan;

// �Ʒ� �Լ��� ��� ScannerDaemon���� IPC �޼����� ��û�� ������ ó����

// ���� ��ĵ �� �˻� ��û
void ScanFile(const tstring filePath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// ���μ��� ��ĵ �� �˻� ��û
// ���μ����� PE�� �����Ͱ� ����ǹǷ� Code ������ ���ؼ��� ���� Ž��
void ScanProcess(const DWORD pid, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// ��� �� ������ Ž���ϸ� ã�� ���Ͽ� ���� ��ĵ �� �˻� ��û
// ��� �� ���� Ž���� Scanner���� �̷����
void ScanDirectory(const tstring dirPath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// ScannerDaemon ���� IPC���� ���� ��ĵ ��û
void ScanDirectoryThread(const tstring dirPath, BOOL& scanContinue, DWORD& currentFileCount, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);
