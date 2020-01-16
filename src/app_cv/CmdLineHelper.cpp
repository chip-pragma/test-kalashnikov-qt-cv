#include "CmdLineHelper.h"

#include <algorithm>

namespace chip {


CmdLineHelper::CmdLineHelper(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        mArgv.push_back(std::string(argv[i]));
    }
}

const std::string &CmdLineHelper::getOptionValue(const std::string &option, const std::string &def) {
    auto it = std::find(mArgv.begin(), mArgv.end(), option);
    if (it != mArgv.end() and ++it != mArgv.end()) {
        return *it;
    }
    return def;
}


}