#include "FileUtil.h"
#include <filesystem>

using namespace std;
using namespace std::filesystem;


FileUtil::FileUtil() {

}

FileUtil::~FileUtil() {

}

BOOL FileUtil::search(tstring parentDirectory, DWORD& fileCount, ScanFileCallback callback) {
    BOOL result = FALSE;
    tstring lastFile;
    tstring lastDir;

    try {
        path parent(parentDirectory);

        for (const auto& entry : recursive_directory_iterator(parent, directory_options::skip_permission_denied)) {
            try {
                if (entry.is_regular_file()) {
                    lastFile = entry.path().c_str();
                    if (callback != NULL) {
                        callback(lastFile);
                    }
                    fileCount++;
                }
                else if (entry.is_directory()) {
                    lastDir = entry.path().c_str();
                }

                Sleep(0);
            }
            catch (const std::exception& ex) {
                string what = ex.what();
                tstring reason = tstring().assign(what.begin(), what.end());
                tstring errPath = entry.path().c_str();

                m_logger << LogLevel::ERR;
                m_logger << ErrorLogInfo() << NL;
                m_logger << _T("  reason : ") << reason << NL;
                m_logger << _T("  path : ") << errPath << NL; 

                continue;
            }
        }
    }
    catch (const std::exception& ex) {
        string what = ex.what();
        tstring reason = tstring().assign(what.begin(), what.end());

        m_logger << LogLevel::ERR;
        m_logger << ErrorLogInfo(ERRLOG_EXTRA_FULL) << NL;
        m_logger << _T("  reason : ") << reason << NL;
    }
    return result;
}