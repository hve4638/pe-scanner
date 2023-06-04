#pragma once
#include "Loader/PEScan.h"
#include <thread>
#include <chrono>
#include <format>
#include <string>
#include <filesystem>

using namespace PEScan;

void ScanFile(const tstring filePath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);
void ScanProcess(const DWORD pid, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);
void ScanDirectoryThread(const tstring dirPath, BOOL& scanContinue, DWORD& currentFileCount, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);
void ScanDirectory(const tstring dirPath, IIPCUtil& ipcUtil, DetectFileInfoList& detectFileList);