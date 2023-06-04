#pragma once
#include <format>
#include "PEParser.h"
#include "PEFileReader.h"
#include "PEProcessReader.h"
#include "PEUtils.h"
#include "HashMD5Utils.h"

using namespace std;
using namespace PEUtils;
//using namespace PEMemMapUtil;
//using namespace PEProcessUtil;

namespace PEScan {
    PEParser::PEParser() {
        m_peStruct.reset(new PEStructure());
    }

    PEParser::~PEParser() {
        close();
    }

    void PEParser::close() {
        m_peStruct->machineType = other;
        m_peStruct->sectionList.clear();
        m_peStruct->exportList.clear();
        m_peStruct->importList.clear();
        m_peStruct->tlsCallbackList.clear();
    };

    BOOL PEParser::parsePEFile(const TCHAR* filePath, PEElement elements) {
        return parsePE(NULL, filePath, elements);
    }
    BOOL PEParser::parsePEProcess(DWORD pid, PEElement elements) {
        return parsePE(pid, NULL, elements);
    }

    BOOL PEParser::parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) {
        BOOL isProc;
        if (pfilePath == NULL) {
            m_peReader.reset(new PEProcessReader());
            isProc = TRUE;
        }
        else {
            m_peReader.reset(new PEFileReader());
            isProc = FALSE;
        }

        auto noValidation = !!BITFLAG_CHECK(elements, PE_NO_VALIDATION);
        if (m_peReader->open(pid, pfilePath, !noValidation)) {
            
            if (parseDosHeader() && parseNtHeader()) {
                parseSectionHeader();
                parseDataDirectory();

                if (BITFLAG_CHECK(elements, PE_PARSE_EAT)) parseEAT();
                if (BITFLAG_CHECK(elements, PE_PARSE_IAT)) parseIAT();
                if (BITFLAG_CHECK(elements, PE_PARSE_TLS)) parseTLS();
                parseDebug();

                m_peStruct->baseAddress = m_peReader->getBaseAddress();
                m_peStruct->filePath = m_peReader->getFilePath();

                if (isProc) {
                    // 메모리에서의 PE 이미지 크기
                    if (m_peStruct->machineType == x86) { 
                        m_peSize = m_peStruct->ntHeader32.OptionalHeader.SizeOfImage;
                    }
                    else {
                        m_peSize = m_peStruct->ntHeader64.OptionalHeader.SizeOfImage;
                    }
                }
                else {
                    LARGE_INTEGER fileSize;
                    if (GetFileSizeEx(m_peReader->getHandle(), &fileSize)) {
                        m_peSize = static_cast<DWORD>(fileSize.QuadPart);
                    }
                }

                return TRUE;
            }
            else {
                return FALSE;
            }
        }
        else {
            close();
            return FALSE;
        }
    }

    shared_ptr<PEStructure> PEParser::getPEStructure() {
        return m_peStruct;
    }

    BOOL PEParser::tryGetSectionHash(const TCHAR* sectionName, tstring& hash) {
        SectionInfo section;
        if (findSectionAsName(sectionName, section)) {
            DWORD va = section.VirtualAddress;
            DWORD size = section.SizeOfRawData;
            return tryMakeHashAsRVA(va, size, hash);
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::tryGetCodeSectionHash(tstring& hash) {
        DWORD offset;
        DWORD size;

        if (m_peStruct->machineType == x86) {
            offset = m_peStruct->ntHeader64.OptionalHeader.BaseOfCode;
            size = m_peStruct->ntHeader64.OptionalHeader.SizeOfCode;
        }
        else {
            offset = m_peStruct->ntHeader64.OptionalHeader.BaseOfCode;
            size = m_peStruct->ntHeader64.OptionalHeader.SizeOfCode;
        }
        return tryMakeHashAsRVA(offset, size, hash);
    }

    BOOL PEParser::tryGetEntryPointSectionHash(tstring& hash) {
        SectionInfo section;

        DWORD entryPoint = getAddressOfEntryPoint();
        if (findSectionAsOffset(entryPoint, section)) {
            DWORD va = section.VirtualAddress;
            DWORD size = section.SizeOfRawData;
            return tryMakeHashAsRVA(va, size, hash);
        }
        else {
            return FALSE;
        }
    };

    BOOL PEParser::tryGetPDBFilePathHash(tstring& hash) {
        SectionInfo section;

        tstring& pdbPath = m_peStruct->pdbPath;
        auto bytes = reinterpret_cast<const BYTE*>(pdbPath.c_str());
        auto length = (DWORD)(pdbPath.size()) * sizeof(TCHAR);

        return tryMakeHashAsBytes(bytes, length, hash);
    }

    BOOL PEParser::tryGetPEHash(tstring& fileHash) {
        BOOL result = FALSE;
        if (m_hash.open())
        {
            DWORD readLength = 0;
            DWORD totalReadLength = 0;
            DWORD bufferSize = 1024 * 16;
            DWORD nextReadLength = bufferSize;
            BinaryData dataBuffer(bufferSize);  // 필요한 메모리 할당

            // 파일 전체 해시 값 추출
            if (m_peSize < bufferSize)
            {
                // 파일 크기가 버퍼 크기보다 작을 때 는 파일 크기만큼 읽음
                nextReadLength = m_peSize;
            }
            while (TRUE)
            {
                if ((readLength = m_peReader->readData(totalReadLength, dataBuffer.data(), nextReadLength, TRUE)) >= 0)
                {
                    if (!m_hash.calculateHash(dataBuffer.data(), readLength))
                    {
                        // Error
                        break;
                    }
                    totalReadLength += readLength;
                    if (totalReadLength == m_peSize)
                    {
                        // Calculate complete
                        break;
                    }
                    if ((m_peSize - totalReadLength) < bufferSize)
                    {
                        nextReadLength = m_peSize - totalReadLength;
                    }
                }
                else
                {
                    // readData error
                    break;
                }
            }
            if (totalReadLength == m_peSize)
            {
                result = m_hash.getMD5Hash(fileHash);
            }
            m_hash.close();
        }
        return result;
    };

}