#pragma once
#include <format>
#include "PEParser.h"
#include "PEFileReader.h"
#include "PEUtils.h"
#include "HashMD5Utils.h"

using namespace std;
using namespace PEUtils;

namespace PEScan {
    tstring PEParser::getString(const char* srcString, size_t __legacyArgs = IMAGE_SIZEOF_SHORT_NAME) { 
        BYTE byteBuffer[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0, };
        memcpy_s(byteBuffer, IMAGE_SIZEOF_SHORT_NAME, srcString, IMAGE_SIZEOF_SHORT_NAME);

        if (CHAR_IS_TCHAR) {
            return reinterpret_cast<const TCHAR*>(byteBuffer);
        }
        else {
            return convertToUTF8(byteBuffer, IMAGE_SIZEOF_SHORT_NAME);
        }
    }

    DWORD PEParser::getAddressOfEntryPoint() {
        if (m_peStruct->machineType == x86) {
            return m_peStruct->ntHeader32.OptionalHeader.AddressOfEntryPoint;
        }
        else {
            return m_peStruct->ntHeader64.OptionalHeader.AddressOfEntryPoint;
        }
    }

    BOOL PEParser::findSectionAsName(const TCHAR* sectionName, SectionInfo& info) {
        if (sectionName == NULL) return FALSE;
        for (SectionInfo section : m_peStruct->sectionList) {
            if (section.Name.compare(sectionName) == 0) {
                info = section;
                return TRUE;
            }
        }
        return FALSE;
    }

    BOOL PEParser::findSectionAsOffset(DWORD offset, SectionInfo& sectionInfo) {
        for (SectionInfo section : m_peStruct->sectionList) {
            DWORD offsetBegin = section.VirtualAddress;
            DWORD offsetEnd = offsetBegin + section.SizeOfRawData;

            if (offset >= offsetBegin && offset < offsetEnd) {
                sectionInfo = section;

                return TRUE;
            }
        }
        return FALSE;
    }


    BOOL PEParser::tryMakeHashMD5(DWORD rva, SIZE_T size, tstring& hash) {
        HashMD5Utils md5;
        vector<BYTE> sectionData(size);

        if (m_peReader->readData(rva, sectionData.data(), size) >= 0) {
            return md5.tryGetMD5(sectionData.data(), size, hash);
        }
        else {
            return FALSE;
        }
    }
}