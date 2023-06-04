#pragma once
#include "Loader/PEScan.h"

using namespace PEScan;

extern shared_ptr<ILogger> g_logger;

// 주어진 경로에서 파일의 생성,이동 등에 대해 감시
// 파일이 엔진의 멀웨어 패턴과 일치하는지 탐지
void fileWatchThread(shared_ptr<IFileWatch>);

// 프로세스의 변화 감시
// 프로세스가 엔진의 멀웨어 패턴과 일치하는지 탐지
void processWatchThread(shared_ptr<IProcWatch>);

// 주어진 레지스트리 경로의 변경사항을 감시
// 추가된 경로의 파일이 엔진의 멀웨어 패턴과 일치하는지 탐지
void regWatchThread(shared_ptr<IRegWatch>);

// IPC 메세지 수신 대기
// Scanner.exe 로부터 메세지 수신 대기
void ipcProcThread(shared_ptr<IIPCUtil>);