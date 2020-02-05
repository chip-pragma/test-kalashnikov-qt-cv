#pragma once

#include <string>

namespace core {

struct Options {
    int deviceId = 0;
    std::string shmName = "";
};

Options parseArgs(int argc, char** argv);

}




