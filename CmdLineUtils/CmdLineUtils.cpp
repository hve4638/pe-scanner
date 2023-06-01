#include "CmdLineUtils.h"
#include "CmdLineParser.h"

namespace CmdLineUtils {
    ICmdLineParser* createCmdLineParser() {
        return new CmdLineParser();
    }
}