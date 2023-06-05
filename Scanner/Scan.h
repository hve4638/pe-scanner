#pragma once
#include "Loader/PEScan.h"
#include <thread>
#include <chrono>
#include <format>
#include <string>
#include <filesystem>

using namespace PEScan;

// 아래 함수는 모두 ScannerDaemon에게 IPC 메세지로 요청을 보내서 처리함

// 파일 스캔 및 검사 요청
void ScanFile(const tstring filePath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// 프로세스 스캔 및 검사 요청
// 프로세스는 PE내 데이터가 변경되므로 Code 영역에 대해서만 패턴 탐지
void ScanProcess(const DWORD pid, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// 경로 내 파일을 탐색하며 찾은 파일에 대해 스캔 및 검사 요청
// 경로 내 파일 탐색은 Scanner에서 이루어짐
void ScanDirectory(const tstring dirPath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);

// ScannerDaemon 에게 IPC통해 파일 스캔 요청
void ScanDirectoryThread(const tstring dirPath, BOOL& scanContinue, DWORD& currentFileCount, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);
