#pragma once

#include <string>
#include <vector>

namespace chip {

class CmdLineHelper {
public:
    CmdLineHelper(int argc, char **argv);

    const std::string& getOptionValue(const std::string& option, const std::string& def = "");

private:
    using _ArgList = std::vector<std::string>;

    _ArgList mArgv;
};

}


