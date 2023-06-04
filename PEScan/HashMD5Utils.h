#pragma once
#include "Logger.h"

namespace PEScan {
    class HashMD5Utils {
        HCRYPTPROV m_prov = NULL;
        HCRYPTHASH m_hash = NULL;
        Logger m_logger = { LogDirection::DEBUGVIEW, LogLevel::DEBUG };
        DWORD m_hashSize = 0;
        DWORD m_hashSizeBufferLength = sizeof(DWORD);

        BOOL bytesToString(const BYTE* hashBytes, DWORD srcLength, tstring& outString);
    public:
        HashMD5Utils();
        ~HashMD5Utils();

        BOOL open(void);
        BOOL close(void);
        BOOL compareBytes(const BYTE* srcBytes, DWORD srcLength, const BYTE* destBytes, DWORD destLength);
        BOOL calculateHash(const BYTE* srcBytes, DWORD srcLength);
        BOOL getMD5Hash(BYTE* md5Bytes, DWORD* md5BufferLength);
        BOOL getMD5Hash(tstring& md5String);
    };
}