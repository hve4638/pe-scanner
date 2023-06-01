#pragma once
#include "Logger.h"

namespace PEScan {
    class HashMD5Utils {
        HCRYPTPROV m_prov = NULL;
        HCRYPTHASH m_hash = NULL;
        Logger m_logger = { LogDirection::DEBUGVIEW, LogLevel::DEBUG };

        BOOL bytesToString(const BYTE* hashBytes, DWORD srcLength, tstring& outString);
        BOOL open();
        BOOL close();
    public:
        HashMD5Utils();
        ~HashMD5Utils();
        BOOL tryGetMD5ToBytes(const BYTE* data, size_t len, BYTE* hashBytes);
        BOOL tryGetMD5ToString(const BYTE* data, size_t len, tstring& hashString);
        BOOL tryGetMD5(const BYTE* data, size_t len, BYTE* hashBytes);
        BOOL tryGetMD5(const BYTE* data, size_t len, tstring& hashString);
    };
}