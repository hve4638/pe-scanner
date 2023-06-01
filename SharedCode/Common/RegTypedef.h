#include "Common/typedef.h"
#include <queue>

namespace PEScan {
    // 레지스트리 루트 키 정의
    enum RegRootKey
    {
        REG_HKEY_CLASSES_ROOT = reinterpret_cast<ULONGLONG>(HKEY_CLASSES_ROOT),
        REG_HKEY_LOCAL_MACHINE = reinterpret_cast<ULONGLONG>(HKEY_LOCAL_MACHINE),
        REG_HKEY_CURRENT_USER = reinterpret_cast<ULONGLONG>(HKEY_CURRENT_USER),
        REG_HKEY_USERS = reinterpret_cast<ULONGLONG>(HKEY_USERS),
    };

    // 레지스트리 경로의 값을 저장하기 위한 형식 정의 (레지스트리 값 타입, 레지스트리 값 데이터)
    typedef tuple<DWORD, BinaryData> RegValue;

    // 검사할 레지스트리 경로 목록을 위한 형식 정의 (레지스트리 Key 이름, 레지스트리 Value 이름(optional), 레지스트리 루트 키)
    typedef tuple<tstring, tstring, RegRootKey> RegPathInfo;
    typedef vector<RegPathInfo> RegPathList;

    // 레지스트리에 존재하는 파일 경로를 추출하여 저장하기 위한 형식 정의 (파일 경로, (레지스트리 Key 이름, 레지스트리 Value 이름, 레지스트리 루트 키))
    typedef tuple<tstring, tstring, tstring, RegRootKey> RegFileInfo;
    typedef queue<RegFileInfo> RegFileInfoList;
    typedef vector<RegFileInfo> RegFileList;

}