#pragma once
#include "Common/IFileSearch.h"
#include "Logger.h"
#include <functional>

using namespace std;

namespace PEScan {
    class FileSearch : public IFileSearch {
    private:
        Logger m_logger;
        BinaryData m_pathBuffer;
        BOOL m_scanContinue = FALSE;

    private:
        BOOL extractString(const tstring& orgPath, const tstring& startMarker, const tstring& endMarker, tstring& subString);

    public:
        FileSearch();
        ~FileSearch() override;
        void stop() override;
        BOOL getRealPath(tstring orgPath, tstring& realPath) override;
        void search(tstring parentDirectory, DWORD& currentFileCount, function<void(tstring)> scanFunction) override;
    };
};
