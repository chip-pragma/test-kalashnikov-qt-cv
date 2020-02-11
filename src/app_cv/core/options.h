#pragma once

#include <string>

namespace core {

struct Options {
    int deviceId = 0;
    std::string shmName = "";
    bool show = false;
    bool processing = true;
};

Options parseArgs(int argc, char** argv);

}




