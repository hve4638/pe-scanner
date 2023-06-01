#pragma once
#include <filesystem>
#include "FileSearch.h"

using namespace std;
using namespace std::filesystem;

namespace PEScan {

    FileSearch::FileSearch() {
        m_pathBuffer.assign(8192, 0); // 8k 버퍼 생성
    };

    FileSearch::~FileSearch(void) {

    };

    void FileSearch::stop(void) {
        m_scanContinue = FALSE;
    };

    BOOL FileSearch::extractString(const tstring& orgPath, const tstring& startMarker, const tstring& endMarker, tstring& subString)
    {
        BOOL result = FALSE;

        size_t startPos = orgPath.find(startMarker);
        if (startPos != std::string::npos)
        {
            startPos += startMarker.length();  // 시작 마커 이후의 첫 번째 문자 위치

            size_t endPos = orgPath.find(endMarker, startPos);
            if (endPos != std::string::npos)
            {
                subString = orgPath.substr(startPos, endPos - startPos);
                result = TRUE;
            }
        }
        return result;
    };

    BOOL FileSearch::getRealPath(tstring orgPath, tstring& realPath)
    {
        BOOL result = FALSE;

        if (orgPath.find(_T('%')) != tstring::npos)
        {
            // 환경 변수가 포함된 경로에서 실제 경로 얻기
            DWORD covertResult = ExpandEnvironmentStrings(orgPath.c_str(), reinterpret_cast<LPTSTR>(m_pathBuffer.data()), MAX_PATH);
            if (covertResult > 0 && (covertResult < (MAX_PATH * 8)))
            {
                realPath = reinterpret_cast<LPTSTR>(m_pathBuffer.data());
            }
        }
        else if (orgPath.find(_T('\"')) != tstring::npos)
        {
            if (extractString(orgPath, _T("\""), _T("\""), realPath))
            {
                realPath = filesystem::canonical(realPath).c_str();
            }
        }
        else
        {
            realPath = orgPath.c_str();
        }

        // 파일이 실제로 존재하는 경우만 파일 경로 리턴
        if (!realPath.empty())
        {
            if (filesystem::exists(realPath))
            {
                result = TRUE;
            }
            else
            {
                realPath.clear();
            }
            memset(m_pathBuffer.data(), 0, sizeof(m_pathBuffer));
        }
        return result;
    };

    /*
    * 클래스 멤버 함수를 function을 통해서 전달 할 때 파라메터가 2개면 "placeholders::_1, placeholders::_2"이런 식으로 늘림
      fileUtil.searchFiles(parameter, curFileCount, bind(&ScanFile, placeholders::_1));
    */
    void FileSearch::search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction)
    {
        tstring lastFile;
        tstring lastDir;

        try
        {
            // Cllback 함수 설정
            function<void(tstring)> scanCallback = move(scanFunction);

            // 최상위 디렉토리 설정
            path parent(parentDirectory);

            // Traverse all directories and files recursively
            for (const auto& entry : recursive_directory_iterator(parent, directory_options::skip_permission_denied))
            {
                try
                {
                    if (entry.is_directory())
                    {
                        lastDir = entry.path().c_str();
                    }
                    else if (entry.is_regular_file())
                    {
                        lastFile = entry.path().c_str();
                        scanCallback(lastFile);
                        currentFileCount++;
                    }
                    Sleep(0);
                }
                catch (const std::exception& ex)
                {
                    m_logger << LogLevel::ERR;
                    m_logger << ExceptionLogInfo(ex) << NL;
                    m_logger << format(_T("    {:s}"), entry.path().c_str());
                    continue;
                }
                if (m_scanContinue)
                {
                    // 스캔 중지
                    break;
                }
            }
        }
        catch (const std::exception& ex)
        {
            m_logger << LogLevel::ERR;
            m_logger << ExceptionLogInfo(ex) << NL;
            m_logger << _T("    ") << lastDir << NL;
            m_logger << _T("    ") << lastFile << NL;
        }
    };

};

