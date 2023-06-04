#pragma once
#include "Common/typedef.h"
#include "Common/PEElement.h"

namespace PEScan {
    interface IPEParser {
        virtual ~IPEParser() {};
        virtual void close() abstract;

        // PE������ ���� �Ǵ� ���μ����� �Ľ�
        // PEElement : PE������ ��� �κи��� �Ľ�����, ��ȿ�� PE�������� üũ������ �����ϴ� ��Ʈ�÷���
        virtual BOOL parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) abstract;

        // �Ľ� ����� ��� PE����ü�� ����
        virtual shared_ptr<PEStructure> getPEStructure() abstract;

        // ���� �̸��� �ش��ϴ� ������ �ؽ� ����
        virtual BOOL tryGetSectionHash(const TCHAR* sectionName, tstring& hash) abstract;

        // �������� ���Ե� ������ �ؽ� ����
        virtual BOOL tryGetEntryPointSectionHash(tstring& hash) abstract;

        // PDF ���� �̸� �ؽ� ����
        virtual BOOL tryGetPDBFilePathHash(tstring& hash) abstract;

        // �ڵ� ���� �ؽ� ����
        virtual BOOL tryGetCodeSectionHash(tstring& hash) abstract;

        // PE ���� ��ü �ؽ� ����
        virtual BOOL tryGetPEHash(tstring& hash) abstract;
    };
}


