#pragma once
#include "Common/typedef.h"
#include "Common/PEElement.h"

namespace PEScan {
    interface IPEParser {
        virtual ~IPEParser() {};
        virtual void close() abstract;

        // PE구조의 파일 또는 프로세스를 파싱
        // PEElement : PE구조의 어느 부분만을 파싱할지, 유효한 PE구조인지 체크하지는 결정하는 비트플래그
        virtual BOOL parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) abstract;

        // 파싱 결과가 담긴 PE구조체를 리턴
        virtual shared_ptr<PEStructure> getPEStructure() abstract;

        // 지정 이름에 해당하는 섹션의 해시 추출
        virtual BOOL tryGetSectionHash(const TCHAR* sectionName, tstring& hash) abstract;

        // 진입점이 포함된 섹션의 해시 추출
        virtual BOOL tryGetEntryPointSectionHash(tstring& hash) abstract;

        // PDF 파일 이름 해시 추출
        virtual BOOL tryGetPDBFilePathHash(tstring& hash) abstract;

        // 코드 섹션 해시 추출
        virtual BOOL tryGetCodeSectionHash(tstring& hash) abstract;

        // PE 구조 전체 해시 추출
        virtual BOOL tryGetPEHash(tstring& hash) abstract;
    };
}


