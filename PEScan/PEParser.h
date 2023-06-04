#pragma once
#include "Common/IPEParser.h"
#include "IPEReader.h"
#include "Logger.h"
#include "HashMD5Utils.h"

namespace PEScan {
    class PEParser : public IPEParser {
        shared_ptr<PEStructure> m_peStruct;
        shared_ptr<IPEReader> m_peReader;
        Logger m_logger = { LogDirection::DEBUGVIEW, LogLevel::ALL };
        HashMD5Utils m_hash; 

        DWORD m_peSize = 0;
        tstring getString(const char* srcString, size_t srcLength);

        // PE의 각 부분을 읽어와 파싱하는 
        BOOL parseDosHeader();
        BOOL parseNtHeader();
        BOOL parseSectionHeader();
        SIZE_T updateSectionHeaders(size_t sectionHeaderOffset, WORD numberOfSections);
        BOOL parseDataDirectory();
        BOOL parseEAT();
        BOOL parseIAT();
        BOOL parseImportDescriptor32(IMAGE_IMPORT_DESCRIPTOR& importDescriptor);
        BOOL parseImportDescriptor64(IMAGE_IMPORT_DESCRIPTOR& importDescriptor);
        BOOL parseTLS();
        BOOL parseTLS32();
        BOOL parseTLS64();
        BOOL parseDebug();

        BOOL tryReadExportDirectoryInfo(IMAGE_EXPORT_DIRECTORY exportDirectory,
            DWORD* pFuncAddress, DWORD* pNameAddress, WORD* pNameOrdinal);
        DWORD getAddressOfEntryPoint();

        BOOL findSectionAsName(const TCHAR* sectionName, SectionInfo& info);
        BOOL findSectionAsOffset(DWORD rva, SectionInfo& info);

        BOOL tryMakeHashAsRVA(DWORD, SIZE_T, tstring&);
        BOOL tryMakeHashAsBytes(const BYTE*, SIZE_T, tstring&);
    public:
        PEParser();
        ~PEParser() override;
        void close() override;
        BOOL parsePEFile(const TCHAR* pfilePath, PEElement elements);
        BOOL parsePEProcess(DWORD pid, PEElement elements);
        BOOL parsePE(DWORD pid, const TCHAR* pfilePath, PEElement elements) override;


        shared_ptr<PEStructure> getPEStructure() override;
        BOOL tryGetSectionHash(const TCHAR* sectionName, tstring& hash) override;
        BOOL tryGetEntryPointSectionHash(tstring& hash) override;
        BOOL tryGetPDBFilePathHash(tstring& hash) override;
        BOOL tryGetCodeSectionHash(tstring& hash) override;
        BOOL tryGetPEHash(tstring& hash) override;
    };
}




