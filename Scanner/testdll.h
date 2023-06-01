#pragma once
#include "loaddll.h"

using namespace std;
using namespace PEScan;
using namespace CmdLineUtils;

void testLogger(PEScanLoader&);
void testScan(PEScanLoader&);
void testCmd(CmdLineUtilsLoader&);
void testFileSearch(PEScanLoader&);
void testRegWatch(PEScanLoader&);
void testProcWatch(PEScanLoader&);
void testFileWatch(PEScanLoader&);