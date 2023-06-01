#pragma once
#include <Windows.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <iostream>

using namespace std;

#if defined(UNICODE) || defined(_UNICODE)
#define tstring wstring
#define tcout wcout
#define OutputDebugStringT OutputDebugStringW
#else
#define tstring string
#define tcout cout
#define OutputDebugStringT OutputDebugStringA
#endif

#define DLL_PROC_LOAD(var_name, load_name, load_type) ((var_name = (load_type)GetProcAddress(m_module, load_name)) != NULL)
#define DLL_PROC_CHECK(var_name, load_proc) if (var_name == NULL && !load_proc()) error()

#define NL _T("\n")
#define IMAGE_PDB_SIGNATURE 0x53445352 // "RSDS"

#define BITFLAG_CHECK(X, B) (((X) & (B)) == (B))

#define MD5_LENGTH 16
#define CHAR_IS_TCHAR (sizeof(TCHAR) == sizeof(char))
#define CHECK_FLAG(x, flag) ((x) & (flag) == (flag))
typedef ULONGLONG QWORD;

typedef vector<BYTE> BinaryData;

typedef struct _LoadedDllInfo {
    tstring Path;
    ULONGLONG DllBase;
} LoadedDllInfo;

typedef struct _SectionInfo {
    tstring Name;
    DWORD VirtualAddress;
    DWORD PointerToRawData;
    DWORD SizeOfRawData;
    DWORD Characteristics;
} SectionInfo;

typedef struct _PEFunctionInfo {
    QWORD AddressOfIAT;
    DWORD Ordinal;
    tstring Name;
} PEFunctionInfo;

typedef struct _PEExportImportInfo {
    tstring Name;
    vector<PEFunctionInfo> FunctionInfo;
} PEExportImportInfo, PEImportInfo, PEExportInfo;

typedef vector<SectionInfo> SectionsInfo;
typedef vector<LoadedDllInfo> LoadedDllsInfo;

enum MACHINE_TYPE {
    x64,
    x86,
    other,
};

typedef struct _IMAGE_PDB_INFO {
    DWORD Signature;
    BYTE Guid[16];
    DWORD Age;
    CHAR PdbFileName[1];
} IMAGE_PDB_INFO;

typedef struct _PEStructure {
    MACHINE_TYPE machineType;
    LPVOID baseAddress = NULL;
    tstring filePath;
    tstring pdbPath;
    IMAGE_DOS_HEADER dosHeader = { 0, };
    IMAGE_NT_HEADERS32 ntHeader32 = { 0, };
    IMAGE_NT_HEADERS64 ntHeader64 = { 0, };
    IMAGE_DATA_DIRECTORY dataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = { 0, };
    SectionsInfo sectionList;
    vector<PEImportInfo> importList;
    vector<PEExportInfo> exportList;
    vector<QWORD> tlsCallbackList;
} PEStructure;
