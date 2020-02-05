#pragma once

namespace core {

struct Options {
    int deviceId = 0;
    const char* shmName = nullptr;
};

Options parseArgs(int argc, char** argv);

}




