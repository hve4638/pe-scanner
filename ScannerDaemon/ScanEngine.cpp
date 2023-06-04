#pragma once
#include "ScanEngine.h"
#include <format>

PEScanLoader* ScanEngine::peScan = NULL;

ScanEngine::ScanEngine()
{
    Initialize();
};

ScanEngine::~ScanEngine(void)
{
};

void ScanEngine::loadMalwarePattern(void)
{
    m_malwarePattern.insert(pair(_T("dd7fb8fad0636cc90d409350401d0416"), _T("DetectMe.Process")));
    m_malwarePattern.insert(pair(_T("f5750e84c9b79a586bf223f857365c6f"), _T("DetectMe.File")));
    m_malwarePattern.insert(pair(_T("60754a02d83c8dca4384b1f2bdeb86a8"), _T("DetectMe.Code")));
    m_malwarePattern.insert(pair(_T("58647356627c8d7c56845c81df86b5ea"), _T("DetectMe.PDB")));
    initialized = TRUE;
};

void ScanEngine::Initialize(void)
{
    if (!initialized)
    {
        loadMalwarePattern();
    }

    if (peScan != NULL) {
        m_logger = peScan->Logger();
        m_peParser = peScan->PEParser();
        m_fileSearch = peScan->FileSearch();
    }
    else {
        abort();
    }
};

BOOL ScanEngine::scanPE(ScanFileType scnaFileType, tstring& detectName)
{
    BOOL result = FALSE;
    tstring hashValue;

    switch (scnaFileType)
    {
    case SCAN_TYPE_FILE:
    case SCAN_TYPE_PROCESS:
        m_peParser->tryGetPEHash(hashValue);
        break;
    case SCAN_TYPE_CODE:
        m_peParser->tryGetEntryPointSectionHash(hashValue);
        break;
    case SCAN_TYPE_PDB:
        m_peParser->tryGetPDBFilePathHash(hashValue);
        break;
    }

    if (m_malwarePattern.find(hashValue) != m_malwarePattern.end())
    {
        detectName = m_malwarePattern[hashValue];
        result = TRUE;
    }
    return result;
};

BOOL ScanEngine::scanFile(const tstring filePath, tstring& detectName, ScanFileType scnaFileType)
{
    BOOL result = FALSE;
    tstring realPath;
    PEElement peElement = PE_PARSE_HEADER;

    if ((scnaFileType & SCAN_TYPE_PDB) == SCAN_TYPE_PDB)
    {
        peElement = static_cast<PEElement>(peElement | PE_PARSE_DEBUG);
        
    }
    if (m_fileSearch->getRealPath(filePath, realPath))
    {
        peElement = static_cast<PEElement>(peElement);
        if (m_peParser->parsePE(NULL, realPath.c_str(), peElement))
        {
            result = scanPE(scnaFileType, detectName);
        }
        m_peParser->close();
    }
    return result;
};

BOOL ScanEngine::scanProcess(const DWORD pid, tstring& detectName, ScanFileType scnaFileType)
{
    BOOL result = FALSE;
    tstring realPath;

    if (pid > 0x4)
    {
        if (m_peParser->parsePE(pid, NULL, (PEElement)(PE_PARSE_HEADER)))
        {
            result = scanPE(scnaFileType, detectName);
        }
        m_peParser->close();
    }
    return result;
};