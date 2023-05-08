#pragma once
#include <format>
#include "PEParser.h"
#include "PEFileReader.h"
#include "PEUtils.h"
#include "ReserveDelete.h"

using namespace std;
using namespace PEUtils;

namespace PEParse {
    BOOL PEParser::parseDosHeader() {
        if (m_peReader->readData(0x0, &m_peStruct->dosHeader, sizeof(IMAGE_DOS_HEADER)) < 0) {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error: Fail to read DOS header")) << NL;

            return FALSE;
        }
        else if (m_peStruct->dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error: Invalid DOS header signature")) << NL;

            return FALSE;
        }
        else {
            return TRUE;
        }
    }

    BOOL PEParser::parseNtHeader() {
        QWORD pNtHeader = m_peStruct->dosHeader.e_lfanew;
        IMAGE_NT_HEADERS32 ntHeader = { 0, };
        if (m_peReader->readData(pNtHeader, &ntHeader, sizeof(IMAGE_NT_HEADERS32)) < 0) {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error: Fail to read NT header")) << NL;

            return FALSE;
        }
        else if (ntHeader.Signature != IMAGE_NT_SIGNATURE) {
            m_logger << LogLevel::ERR;
            m_logger << ErrorLogInfo(_T("Error: Invalid NT header signature")) << NL;

            return FALSE;
        }
        else {
            LPVOID bufferAddress;
            SIZE_T bufferSize;
            WORD machineType = ntHeader.FileHeader.Machine;

            if ((WORD)machineType == IMAGE_FILE_MACHINE_I386) {
                m_peStruct->machineType = x86;
                bufferAddress = &m_peStruct->ntHeader32;
                bufferSize = sizeof(IMAGE_NT_HEADERS32);
            }
            else {
                m_peStruct->machineType = x64;
                bufferAddress = &m_peStruct->ntHeader64;
                bufferSize = sizeof(IMAGE_NT_HEADERS64);
            }

            return (m_peReader->readData(pNtHeader, bufferAddress, bufferSize) >= 0);
        }
    }

    BOOL PEParser::parseSectionHeader() {
        size_t sectionHeaderOffset;
        WORD numberOfSections;
        
        if (m_peStruct->machineType == x86) {
            sectionHeaderOffset = m_peStruct->dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS32);
            numberOfSections = m_peStruct->ntHeader32.FileHeader.NumberOfSections;
        }
        else {
            sectionHeaderOffset = m_peStruct->dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS64);
            numberOfSections = m_peStruct->ntHeader64.FileHeader.NumberOfSections;
        }

        SIZE_T count;
        count = updateSectionHeaders(sectionHeaderOffset, numberOfSections);

        return (count > 0);
    }

    SIZE_T PEParser::updateSectionHeaders(size_t sectionHeaderOffset, WORD numberOfSections) {
        SIZE_T count = 0;
        SectionInfo sectionInfo;
        IMAGE_SECTION_HEADER sectionHeader = { 0, };

        for (WORD index = 0; index < numberOfSections; index++) {
            if (m_peReader->readData(sectionHeaderOffset, &sectionHeader, sizeof(IMAGE_SECTION_HEADER)) >= 0) {
                sectionInfo.Name = getString((const char*)sectionHeader.Name, IMAGE_SIZEOF_SHORT_NAME);
                sectionInfo.VirtualAddress = sectionHeader.VirtualAddress;
                sectionInfo.PointerToRawData = sectionHeader.PointerToRawData;
                sectionInfo.SizeOfRawData = sectionHeader.SizeOfRawData;
                sectionInfo.Characteristics = sectionHeader.Characteristics;

                m_peStruct->sectionList.push_back(sectionInfo);
                count++;
            }
            sectionHeaderOffset += sizeof(IMAGE_SECTION_HEADER);
            memset(&sectionHeader, 0, sizeof(IMAGE_SECTION_HEADER));
        }
        return count;
    }

    BOOL PEParser::parseDataDirectory() {
        size_t dataSize = sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
        IMAGE_DATA_DIRECTORY *dataDirectory;
        if (m_peStruct->machineType == x86) {
            dataDirectory = m_peStruct->ntHeader32.OptionalHeader.DataDirectory;
        }
        else {
            dataDirectory = m_peStruct->ntHeader64.OptionalHeader.DataDirectory;
        }

        memcpy_s(&m_peStruct->dataDirectory, dataSize, dataDirectory, dataSize);
        return TRUE;
    }


    BOOL PEParser::parseEAT() {
        ReserveDelete reserveDelete;
        vector<PEFunctionInfo> funcInfoVector;
        IMAGE_EXPORT_DIRECTORY exportDirectory = { 0, };
        WORD* nameOrdinals = NULL;
        DWORD* nameAddresses = NULL;
        DWORD* functionAddresses = NULL;
        DWORD* functionNamesRVA = NULL;
        DWORD va = m_peStruct->dataDirectory[0].VirtualAddress;
        if (va == 0x0) {
            return FALSE;
        }
        else if (m_peReader->readData(va, &exportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY)) < 0) {
            return FALSE;
        }

        // ����� ���� �Ҵ�� �ּҸ� �Լ� ����� �Ҵ������ϴ� �ڵ�
        reserveDelete
            .addRef((void**)&nameOrdinals)
            .addRef((void**)&nameAddresses)
            .addRef((void**)&functionAddresses)
            .addRef((void**)&functionNamesRVA);

        nameOrdinals = new WORD[exportDirectory.NumberOfNames];
        nameAddresses = new DWORD[exportDirectory.NumberOfNames];
        functionAddresses = new DWORD[exportDirectory.NumberOfFunctions];

        // ��ü �Լ� �ּ� �迭���� �Լ� �̸��� �����ϴ� �Լ��� üũ�ϱ� ���� �迭 ����
        // �Լ� �̸��� �����ϴ� �Լ� �ּ��� ��ġ�� �̸� ���ڿ��� RVA�� �迭�� ����
        functionNamesRVA = new DWORD[exportDirectory.NumberOfFunctions];
        memset(functionNamesRVA, NULL, sizeof(DWORD) * exportDirectory.NumberOfFunctions);

        if (tryReadExportDirectoryInfo(exportDirectory, functionAddresses, nameAddresses, nameOrdinals)) {
            // �̸��� �����ϴ� �Լ����� �̸� RVA�� pFunctionList�� ����
            for (DWORD i = 0; i < exportDirectory.NumberOfNames; i++) {
                WORD index = nameOrdinals[i];
                functionNamesRVA[index] = nameAddresses[i];
            }

            DWORD ordinalBase = (DWORD)exportDirectory.Base;
            PEFunctionInfo funcInfo = { 0, };
            for (DWORD i = 0; i < exportDirectory.NumberOfFunctions; i++) {
                DWORD nameRVA = functionNamesRVA[i];             // �̸��� �����ϴ� �Լ�
                DWORD addressOfIAT = functionAddresses[i];       // Ordinal�θ� Export�ϴ� �Լ�
                DWORD ordinal = ordinalBase + i;                 // ordinal = Base + functionAddress index(nameOrdinal)

                if (nameRVA != NULL && addressOfIAT != NULL) {
                    m_logger << LogLevel::DEBUG;
                    m_logger << ErrorLogInfo(_T("Export address is invalid > 0x{:x}, 0x{:x}")) << NL;
                }
                else {
                    funcInfo.AddressOfIAT = addressOfIAT;
                    funcInfo.Ordinal = ordinal;

                    if (nameRVA == NULL) {
                        funcInfo.Name = tstring(_T(""));
                    }
                    else {
                        funcInfo.Name = m_peReader->getPEString(nameRVA);
                    }

                    funcInfoVector.push_back(funcInfo);
                }
            }
            
            if (funcInfoVector.empty()) {
                return FALSE;
            }
            else {
                PEExportInfo exportInfo;
                tstring moduleName = m_peReader->getPEString(exportDirectory.Name);
                exportInfo.Name = moduleName;
                exportInfo.FunctionInfo = funcInfoVector;

                m_peStruct->exportList.push_back(exportInfo);

                return TRUE;
            }
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::tryReadExportDirectoryInfo(IMAGE_EXPORT_DIRECTORY exportDirectory, DWORD* pFuncAddress, DWORD* pNameAddress, WORD* pNameOrdinal) {
        if (m_peReader->readData(exportDirectory.AddressOfFunctions, pFuncAddress, sizeof(DWORD) * exportDirectory.NumberOfFunctions) < 0) return FALSE;
        if (m_peReader->readData(exportDirectory.AddressOfNames, pNameAddress, sizeof(DWORD) * exportDirectory.NumberOfNames) < 0) return FALSE;
        if (m_peReader->readData(exportDirectory.AddressOfNameOrdinals, pNameOrdinal, sizeof(WORD) * exportDirectory.NumberOfNames) < 0) return FALSE;
        return TRUE;
    }

    BOOL PEParser::parseIAT() {
        BOOL result = FALSE;
        IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0, };
        DWORD va = m_peStruct->dataDirectory[1].VirtualAddress;

        if (va == 0x0) {
            return FALSE;
        }
        else {
            // ImportDescriptor ����ü�� �迭�� ���ʴ�� �о��
            // �迭�� ���� NULL ����ü
            while (m_peReader->readData(va, &importDescriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR)) >= 0 && (importDescriptor.OriginalFirstThunk != 0x0)) {
                if (m_peStruct->machineType == x86) {
                    if (parseImportDescriptor32(importDescriptor)) {
                        result = TRUE;
                    }
                }
                else {
                    if (parseImportDescriptor64(importDescriptor)) {
                        result = TRUE;
                    }
                }

                va += sizeof(IMAGE_IMPORT_DESCRIPTOR);
            }
        }

        return result;
    }
    
    BOOL PEParser::parseImportDescriptor32(IMAGE_IMPORT_DESCRIPTOR& importDescriptor) {
        const ULONGLONG TRUNK_DATASIZE = sizeof(IMAGE_THUNK_DATA32);
        IMAGE_THUNK_DATA32 thunkData = { 0, };

        vector<PEFunctionInfo> funcInfoVector;
        WORD funcNameOrdinal = 0;

        // IMAGE_THUNK_DATA32 (Import Name Table) ����ü�� �迭�� ���ʴ�� �о��
        // �迭�� ���� NULL ����ü
        DWORD firstThunkAddress = importDescriptor.FirstThunk;
        DWORD importNameTablePosition = importDescriptor.OriginalFirstThunk;
        while ((m_peReader->readData((DWORD)firstThunkAddress, &thunkData, TRUNK_DATASIZE) >= 0) && (thunkData.u1.AddressOfData != 0x0)) {
            DWORD addressOfIAT = thunkData.u1.AddressOfData;

            if (m_peReader->readData((DWORD)importNameTablePosition, &thunkData, TRUNK_DATASIZE) >= 0) {
                DWORD ordinalValue = (thunkData.u1.Ordinal << 1) >> 1; // �ֻ��� ��Ʈ ����

                // ���� ���� ���ؼ� �ٸ��� �ֻ��� ��Ʈ�� 1�� ���� �ƴٴ� �ǹ�
                // �ֻ��� ��Ʈ�� 1�� ������ ��쿡�� �̸����� Ordinal�� �ִ� �Լ�
                if (ordinalValue != thunkData.u1.Ordinal) {
                    PEFunctionInfo funcInfo;
                    funcInfo.AddressOfIAT = addressOfIAT;
                    funcInfo.Ordinal = ordinalValue;
                    funcInfo.Name = tstring(_T(""));

                    funcInfoVector.push_back(funcInfo);
                }
                // Ordinal, �Լ� �̸� �б�
                else if (m_peReader->readData(thunkData.u1.AddressOfData, &funcNameOrdinal, sizeof(WORD)) >= 0) {
                    PEFunctionInfo funcInfo;
                    funcInfo.AddressOfIAT = addressOfIAT;
                    funcInfo.Ordinal = funcNameOrdinal;
                    funcInfo.Name = m_peReader->getPEString((DWORD)(thunkData.u1.AddressOfData + sizeof(WORD)));

                    funcInfoVector.push_back(funcInfo);
                }
            }
            firstThunkAddress += TRUNK_DATASIZE;
            importNameTablePosition += TRUNK_DATASIZE;
        }

        if (funcInfoVector.size() > 0) {
            PEImportInfo importInfo;
            importInfo.Name = m_peReader->getPEString(importDescriptor.Name);
            importInfo.FunctionInfo = funcInfoVector;

            m_peStruct->importList.push_back(importInfo);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::parseImportDescriptor64(IMAGE_IMPORT_DESCRIPTOR& importDescriptor) {
        const ULONGLONG TRUNK_DATASIZE = sizeof(IMAGE_THUNK_DATA64);
        IMAGE_THUNK_DATA64 thunkData = { 0, };

        vector<PEFunctionInfo> funcInfoVector;
        WORD funcNameOrdinal = 0;

        DWORD firstThunkAddress = importDescriptor.FirstThunk;
        DWORD importNameTablePosition = importDescriptor.OriginalFirstThunk;
        while ((m_peReader->readData((DWORD)firstThunkAddress, &thunkData, TRUNK_DATASIZE) >= 0) && (thunkData.u1.AddressOfData != 0x0)) {
            QWORD addressOfIAT = thunkData.u1.AddressOfData;

            if (m_peReader->readData((DWORD)importNameTablePosition, &thunkData, TRUNK_DATASIZE) >= 0) {
                QWORD ordinalValue = (thunkData.u1.Ordinal << 1) >> 1;

                if (ordinalValue != thunkData.u1.Ordinal) {
                    PEFunctionInfo funcInfo;
                    funcInfo.AddressOfIAT = addressOfIAT;
                    funcInfo.Ordinal = (DWORD)ordinalValue;
                    funcInfo.Name = tstring(_T(""));

                    funcInfoVector.push_back(funcInfo);
                }
                else {
                    if (m_peReader->readData(thunkData.u1.AddressOfData, &funcNameOrdinal, sizeof(WORD)) >= 0) {
                        PEFunctionInfo funcInfo;
                        funcInfo.AddressOfIAT = addressOfIAT;
                        funcInfo.Ordinal = funcNameOrdinal;
                        funcInfo.Name = m_peReader->getPEString(thunkData.u1.AddressOfData + sizeof(WORD));

                        funcInfoVector.push_back(funcInfo);
                    }
                }
            }
            firstThunkAddress += TRUNK_DATASIZE;
            importNameTablePosition += TRUNK_DATASIZE;
        }

        if (funcInfoVector.size() > 0) {
            PEImportInfo importInfo;
            importInfo.Name = m_peReader->getPEString(importDescriptor.Name);
            importInfo.FunctionInfo = funcInfoVector;

            m_peStruct->importList.push_back(importInfo);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::parseTLS() {
        auto va = m_peStruct->dataDirectory[9].VirtualAddress;
        if (va == 0x0) {
            return FALSE;
        }
        if (m_peStruct->machineType == x86) {
            return parseTLS32();
        }
        else {
            return parseTLS64();
        }
    }

    BOOL PEParser::parseTLS32() {
        auto va = m_peStruct->dataDirectory[9].VirtualAddress;
        IMAGE_TLS_DIRECTORY32 tlsDirectory = { 0, };
        DWORD callbackAddress = 0;

        if (m_peReader->readData((DWORD)va, &tlsDirectory, sizeof(IMAGE_TLS_DIRECTORY32)) >= 0) {
            DWORD curCallbackArrayAddress = tlsDirectory.AddressOfCallBacks;

            // ���μ��������� IMAGE_TLS_DIRECTORY32 ����ü ���� ������ RVA�� �ƴ� ���� �ּҷ� ����Ǿ� �������� readDataNoBase�� ���
            // �������� NULL ����ü�� �迭 ����
            while ((m_peReader->readDataNoBase((DWORD)curCallbackArrayAddress, &callbackAddress, sizeof(DWORD)) >= 0) && (callbackAddress != 0x0)) {
                m_peStruct->tlsCallbackList.push_back((QWORD)callbackAddress);
                curCallbackArrayAddress += sizeof(DWORD);
            }

            // ���Ͽ����� �ݹ� �Լ� �ּ� �迭�� ���� �� ���� ������ ����ü�� ����� �ּҸ� �״�� ����
            if (m_peStruct->tlsCallbackList.size() == 0) {
                m_peStruct->tlsCallbackList.push_back((DWORD)tlsDirectory.AddressOfCallBacks);
            }
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::parseTLS64() {
        auto va = m_peStruct->dataDirectory[9].VirtualAddress;
        IMAGE_TLS_DIRECTORY64 tlsDirectory = { 0, };
        QWORD callbackAddress = 0;

        if (m_peReader->readData(va, &tlsDirectory, sizeof(IMAGE_TLS_DIRECTORY64)) >= 0) {
            QWORD curCallbackArrayAddress = tlsDirectory.AddressOfCallBacks;

            // ���μ��������� IMAGE_TLS_DIRECTORY32 ����ü ���� ������ RVA�� �ƴ� ���� �ּ�(VA)�� ����Ǿ� �ֱ� ������ m_peBaseAddress�� �������� �ʰ� �޸� �ּ� �״�� ����
            while ((m_peReader->readDataNoBase((QWORD)curCallbackArrayAddress, &callbackAddress, sizeof(QWORD)) >= 0) && (callbackAddress != 0x0)) {
                m_peStruct->tlsCallbackList.push_back((ULONGLONG)callbackAddress);
                curCallbackArrayAddress += sizeof(QWORD);
            }

            if (m_peStruct->tlsCallbackList.size() == 0) {
                // ���Ͽ����� �ݹ� �Լ� �ּ� �迭�� ���� �� ���� ������ ����ü�� ����� �ּҸ� �״�� ����
                m_peStruct->tlsCallbackList.push_back((QWORD)tlsDirectory.AddressOfCallBacks);
            }
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL PEParser::parseDebug() {
        DWORD va = m_peStruct->dataDirectory[6].VirtualAddress;
        BOOL result = FALSE;

        if (va == 0x0) {
            return FALSE;
        }
        else {
            IMAGE_DEBUG_DIRECTORY debugDirectory = { 0, };
            DWORD callbackAddress = 0;
            IMAGE_PDB_INFO pdbInfo = { 0, };

            if (m_peReader->readData(va, &debugDirectory, sizeof(IMAGE_DEBUG_DIRECTORY)) < 0) {
                return FALSE;
            }
            else if ((debugDirectory.Type != IMAGE_DEBUG_TYPE_CODEVIEW)) {
                return FALSE;
            }
            else if (m_peReader->readData((DWORD)debugDirectory.AddressOfRawData, &pdbInfo, sizeof(IMAGE_PDB_INFO)) < 0) {
                return FALSE;
            }
            else if (pdbInfo.Signature != IMAGE_PDB_SIGNATURE) {
                return FALSE;
            }
            else {
                // PDB ��� ����
                QWORD rva = (DWORD)debugDirectory.AddressOfRawData + (sizeof(DWORD) * 2) + (sizeof(BYTE) * 16);
                m_peStruct->pdbPath = m_peReader->getPEString(rva);

                return TRUE;
            }
        }
        return result;
    };

}