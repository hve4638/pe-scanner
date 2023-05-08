//#include <iostream>
#include <format>
#include "PEFileReader.h"
#include "PEUtils.h"

using namespace PEUtils;

namespace PEParse {
    QWORD PEFileReader::rvaToRaw(QWORD rva, QWORD addPosition = 0) {
        // RAW = RVA - VirtualAddress + PointerToRawData
        if (addPosition != 0) {
            return addPosition + (rva - m_rawInfo.virtualAddress + m_rawInfo.pointerToRawData);
        }
        else {
            return (rva - m_rawInfo.virtualAddress + m_rawInfo.pointerToRawData);
        }
    }


    BOOL PEFileReader::setRvaToRawInfo(QWORD rva) {
        if (m_sectionInfo.headerAddress == NULL) {
            // rvaToRaw�� ���� ������ ������Ʈ
            updateSectionInfo();
        }
        
        // rva �ּҰ� PEHeader �κ��� ���
        // PEHeader�� rva�� raw�� ����
        if (rva < m_sectionInfo.sizeOfHeaders) {
            m_rawInfo.startAddress = 0;
            m_rawInfo.endAddress = 0;
            m_rawInfo.virtualAddress = 0;
            m_rawInfo.pointerToRawData = 0;

            return TRUE;
        }
        // �̹� ��ȯ �غ� �Ǿ�����
        else if (rva >= m_rawInfo.startAddress && rva < m_rawInfo.endAddress) { 
            return TRUE;
        }
        else {
            return tryUpdateRawInfo(rva);
        }
    }

    // SectionHeader ������ ��������� ����
    void PEFileReader::updateSectionInfo() {
        IMAGE_NT_HEADERS32* pNtHeader32 = reinterpret_cast<IMAGE_NT_HEADERS32*>(m_baseAddress + (LONG)((IMAGE_DOS_HEADER*)m_baseAddress)->e_lfanew);

        if (pNtHeader32->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
            IMAGE_NT_HEADERS64* pNtHeader64 = reinterpret_cast<IMAGE_NT_HEADERS64*>(m_baseAddress + (LONG)((IMAGE_DOS_HEADER*)m_baseAddress)->e_lfanew);
            
            m_sectionInfo.sizeOfHeaders = pNtHeader64->OptionalHeader.SizeOfHeaders;
            m_sectionInfo.numberOfSections = pNtHeader64->FileHeader.NumberOfSections;
            m_sectionInfo.headerAddress = reinterpret_cast<PIMAGE_SECTION_HEADER>((PBYTE)pNtHeader64 + sizeof(IMAGE_NT_HEADERS64));
        }
        else {
            m_sectionInfo.sizeOfHeaders = pNtHeader32->OptionalHeader.SizeOfHeaders;
            m_sectionInfo.numberOfSections = pNtHeader32->FileHeader.NumberOfSections;
            m_sectionInfo.headerAddress = reinterpret_cast<PIMAGE_SECTION_HEADER>((PBYTE)pNtHeader32 + sizeof(IMAGE_NT_HEADERS32));
        }
    }

    BOOL PEFileReader::tryUpdateRawInfo(QWORD rva) {
        m_rawInfo.startAddress = 0;
        m_rawInfo.endAddress = 0;
        m_rawInfo.virtualAddress = 0;
        m_rawInfo.pointerToRawData = 0;

        /*
            �� ������ VirtualAddress���� Virtualaddress + SizeOfRawData ���� ���� �ִ��� üũ�� rawInfo ����
            VirtualSize���� ���ԵǾ SizeOfRawData�� ����ٸ� raw�� ������ �� ����
            � ������ �������� ���Ե��� �ʴ´ٸ� raw�� ��ȯ�� �� ����
        */
        auto count = m_sectionInfo.numberOfSections;
        for (DWORD i = 0; i < count; i++) {
            IMAGE_SECTION_HEADER& header = (m_sectionInfo.headerAddress[i]);

            QWORD rangeStart = header.VirtualAddress;
            QWORD rangeEnd = rangeStart + header.SizeOfRawData;
            if (rva >= rangeStart && rva < rangeEnd) {
                m_rawInfo.startAddress = (ULONGLONG)header.VirtualAddress;
                m_rawInfo.endAddress = (ULONGLONG)m_rawInfo.startAddress + header.SizeOfRawData;
                m_rawInfo.virtualAddress = (ULONGLONG)header.VirtualAddress;
                m_rawInfo.pointerToRawData = (ULONGLONG)header.PointerToRawData;

                return TRUE;
            }
        }
        return FALSE;
    }
}