#pragma once
#include "typedef.h"
#include <queue>

namespace RegInfo {
    // 레지스트리 루트 키 정의
    enum RegRootKey
    {
        REG_HKEY_CLASSES_ROOT = reinterpret_cast<ULONGLONG>(HKEY_CLASSES_ROOT),
        REG_HKEY_LOCAL_MACHINE = reinterpret_cast<ULONGLONG>(HKEY_LOCAL_MACHINE),
        REG_HKEY_CURRENT_USER = reinterpret_cast<ULONGLONG>(HKEY_CURRENT_USER),
        REG_HKEY_USERS = reinterpret_cast<ULONGLONG>(HKEY_USERS),
    };

    // 파일 이벤트 정의
    enum FileEvent
    {
        FILE_CREATED = 0x0,
        FILE_MODIFIED = 0x1,
        FILE_RENAMED = 0x2,
        FILE_UNKNOWN_EVENT = 0xffff
    };

    typedef vector<BYTE> BinaryData;

    // 레지스트리 경로의 값을 저장하기 위한 형식 정의 (레지스트리 값 타입, 레지스트리 값 데이터)
    typedef tuple<DWORD, BinaryData> RegValue;

    // 검사할 레지스트리 경로 목록을 위한 형식 정의 (레지스트리 Key 이름, 레지스트리 Value 이름(optional), 레지스트리 루트 키)
    typedef tuple<tstring, tstring, RegRootKey> RegPathInfo;
    typedef vector<RegPathInfo> RegPathList;

    // 레지스트리에 존재하는 파일 경로를 추출하여 저장하기 위한 형식 정의 (파일 경로, (레지스트리 Key 이름, 레지스트리 Value 이름, 레지스트리 루트 키))
    typedef tuple<tstring, tuple<tstring, tstring, RegRootKey>> RegFileInfo;
    typedef queue<RegFileInfo> RegFileInfoList;
    typedef vector<RegFileInfo> RegFileList;

    // 프로세스 정보를 추출하여 저장하기 위한 형식 정의 (PID, 프로세스 이름, 프로세스 경로, 프로세스 커맨드라인)
    typedef tuple<DWORD, tstring, tstring, tstring> ProcessInfo;
    typedef queue<ProcessInfo> ProcessInfoList;

    // 파일 정보를 추출하여 저장하기 위한 형식 정의 (파일 이름, 파일 확장자, 파일 경로, 파일 이벤트)
    typedef tuple<tstring, tstring, tstring, FileEvent> FileInfo;
    typedef queue<FileInfo> FileInfoList;
}