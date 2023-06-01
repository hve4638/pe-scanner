#include "loaddll.h"
#define DLL_LOADER_LOAD_PROC(loader, proc) if (!loader.proc()) {    \
        tcout << _T("fail to load proc : ") << #proc << endl;       \
        exit(-1);                                                   \
    }                                                               \

void loadPEScan(PEScan::PEScanLoader& loader) {
    if (!loader.isLoaded()) {
        tcout << _T("fail to load library") << endl;
        exit(-1);
    }
    DLL_LOADER_LOAD_PROC(loader, loadLogger);
    DLL_LOADER_LOAD_PROC(loader, loadPEParser);
    DLL_LOADER_LOAD_PROC(loader, loadPEPrinter);
    DLL_LOADER_LOAD_PROC(loader, loadCodeLogInfo);
    DLL_LOADER_LOAD_PROC(loader, loadFileSearch);
    DLL_LOADER_LOAD_PROC(loader, loadRegWatch);
    DLL_LOADER_LOAD_PROC(loader, loadProcWatch);
    DLL_LOADER_LOAD_PROC(loader, loadIPCUtil);
    DLL_LOADER_LOAD_PROC(loader, loadFileWatch);
}