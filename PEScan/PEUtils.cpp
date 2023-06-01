#include "PEUtils.h"
#define _MY_DEBUG

namespace PEUtils {
    void debugPrint(tstring debugMessage) {
#ifdef _MY_DEBUG
        OutputDebugStringT(debugMessage.c_str());
        OutputDebugStringT(_T("\n"));
#endif
    }

    tstring copyStringToTString(string& source) {
        tstring dest;
        if (CHAR_IS_TCHAR) {
            dest.assign(source.begin(), source.end());
        }
        else {
            int bufferLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(source.data()), -1, NULL, 0);

            TCHAR* buffer = new TCHAR[bufferLen];

            MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(source.data()), -1, buffer, bufferLen);
            dest = buffer;
            delete[] buffer;
        }

        return dest;
    }

    void printBuffer(BYTE* buffer, SIZE_T size) {
        for (SIZE_T i = 0; i < size; i += 16) {
            for (SIZE_T j = i; j < size && j < i + 16; j++) {
                printf("%02x ", buffer[j]);
                if (j % 8 == 7) printf(" ");
            }
            for (SIZE_T j = i; j < size && j < i + 16; j++) {
                unsigned char ch = buffer[j];
                if (isprint(ch)) tcout << (char)ch;
                else tcout << ".";
                if (j % 8 == 7) printf(" ");
            }

            printf("\n");
        }
    }

    void deleteStruct(void** pStruct) {
        if ((*pStruct) != NULL) {
            delete (*pStruct);
            (*pStruct) = NULL;
        }
    };

    tstring convertToUTF8(BYTE* byteBuffer, size_t srcLength) {
        tstring readString;
        // 필요한 문자열 길이를 먼저 얻음
        int bufferLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(byteBuffer), -1, NULL, 0);
        // 필요한 문자열 길이 만큼 메모리 할당
        TCHAR* readBuffer = new TCHAR[bufferLen];
        // readString 메모리 주소를 리턴하는 .data()를 이용하여 readString 메모리에 변환된 문자열 저장
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(byteBuffer), -1, readBuffer, bufferLen);
        readString = readBuffer;
        delete[] readBuffer;
        return readString;
    }

    vector<tstring> splitString(tstring str) {
        vector<basic_string<TCHAR>> tokens;
        basic_string<TCHAR> temp;

        for (const auto& ch : str) {
            if (ch != _T(' ')) {
                temp += ch;
            }
            else if (!temp.empty()) {
                tokens.push_back(temp);
                temp.clear();
            }
        }
        if (!temp.empty()) {
            tokens.push_back(temp);
        }
        return tokens;
    }

    void LargeInteger2QWORD(LARGE_INTEGER& large, QWORD& qword) {

    }
}