#include "CodeLogInfo.h"
#include "HashMD5Utils.h"
#include <format>

namespace PEScan {
    HashMD5Utils::HashMD5Utils() {

    }
    HashMD5Utils::~HashMD5Utils() {
        close();
    }
    BOOL HashMD5Utils::open() {
        if (!CryptAcquireContext(&m_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            std::cerr << "Error: CryptAcquireContext failed." << std::endl;
            close();
            return FALSE;
        }
        else if (!CryptCreateHash(m_prov, CALG_MD5, 0, 0, &m_hash)) {
            std::cerr << "Error: CryptCreateHash failed." << std::endl;
            close();
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    BOOL HashMD5Utils::close() {
        if (m_prov != NULL) CryptReleaseContext(m_prov, 0);
        if (m_hash != NULL) CryptDestroyHash(m_hash);

        m_prov = NULL;
        m_hash = NULL;
        return TRUE;
    }

    BOOL HashMD5Utils::compareBytes(const BYTE* srcBytes, DWORD srcLength, const BYTE* destBytes, DWORD destLength)
    {
        return ((srcLength == destLength) && equal(srcBytes, srcBytes + srcLength, destBytes));
    };
    BOOL HashMD5Utils::calculateHash(const BYTE* srcBytes, DWORD srcLength)
    {
        BOOL result = FALSE;

        // Get the hash from the bytes
        if (CryptHashData(m_hash, srcBytes, srcLength, 0) != 0)
        {
            result = TRUE;
        }
        else
        {
            m_logger.log(_T("CryptHashData fail"), GetLastError(), LOG_LEVEL_ERROR);
        }
        return result;
    };

    BOOL HashMD5Utils::getMD5Hash(BYTE* md5Bytes, DWORD* md5BufferLength)
    {
        BOOL result = FALSE;

        // Get the hash size
        if ((m_hash != NULL) && (CryptGetHashParam(m_hash, HP_HASHSIZE, (BYTE*)&m_hashSize, &m_hashSizeBufferLength, 0) != 0))
        {
            // Check buufer size
            if (*md5BufferLength >= m_hashSize)
            {
                // Get the hash value
                if (CryptGetHashParam(m_hash, HP_HASHVAL, md5Bytes, md5BufferLength, 0))
                {
                    result = TRUE;
                }
            }
        }
        return result;
    };

    BOOL HashMD5Utils::getMD5Hash(tstring& md5String)
    {
        BYTE md5HashBytes[MD5_LENGTH] = { 0, };
        DWORD md5BufferLength = MD5_LENGTH;

        return (getMD5Hash(md5HashBytes, &md5BufferLength) && bytesToString(md5HashBytes, MD5_LENGTH, md5String));
    };


    BOOL HashMD5Utils::bytesToString(const BYTE* hashBytes, DWORD srcLength, tstring& outString) {
        outString = _T("");

        if (hashBytes == NULL) {
            return FALSE;
        }
        else {
            for (DWORD i = 0; i < srcLength; i++) {
                outString.append(format(_T("{:02x}"), hashBytes[i]));
            }
            return (!outString.empty());
        }
    };
}