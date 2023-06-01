#pragma once
#include <filesystem>
#include "FileSearch.h"

using namespace std;
using namespace std::filesystem;

namespace PEScan {

    FileSearch::FileSearch() {
        m_pathBuffer.assign(8192, 0); // 8k ���� ����
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
            startPos += startMarker.length();  // ���� ��Ŀ ������ ù ��° ���� ��ġ

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
            // ȯ�� ������ ���Ե� ��ο��� ���� ��� ���
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

        // ������ ������ �����ϴ� ��츸 ���� ��� ����
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
    * Ŭ���� ��� �Լ��� function�� ���ؼ� ���� �� �� �Ķ���Ͱ� 2���� "placeholders::_1, placeholders::_2"�̷� ������ �ø�
      fileUtil.searchFiles(parameter, curFileCount, bind(&ScanFile, placeholders::_1));
    */
    void FileSearch::search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction)
    {
        tstring lastFile;
        tstring lastDir;

        try
        {
            // Cllback �Լ� ����
            function<void(tstring)> scanCallback = move(scanFunction);

            // �ֻ��� ���丮 ����
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
                    // ��ĵ ����
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

