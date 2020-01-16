#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "CmdLineHelper.h"

namespace {
const char *SHM_NAME_PREFIX = "APPCV_";
const uint SHM_NAME_LENGTH = 7;

std::string makeShmName() {
    std::string shmName = SHM_NAME_PREFIX;
    std::srand(std::time(nullptr));
    auto prefixLen = strlen(SHM_NAME_PREFIX);
    shmName.resize(prefixLen + SHM_NAME_LENGTH);

    std::for_each(
            shmName.begin() + prefixLen, shmName.end(),
            [&](char &c) {
                c = '0' + std::rand() % 10;
            }
    );
    return shmName;
}
}


int main(int argc, char **argv) {
    using namespace chip;

    int deviceId = 0;
    std::string shmName;

    CmdLineHelper cmdLn(argc, argv);
    try {
        shmName = cmdLn.getOptionValue("--shm-name", makeShmName());
        deviceId = std::stoi(cmdLn.getOptionValue("--device", "0"));
    }
    catch (std::invalid_argument &e) {
        std::cerr << e.what();
        exit(EXIT_FAILURE);
    }

    std::cout << "Device ID: " << deviceId << "\nShm Name: " << shmName << std::endl;

    return EXIT_SUCCESS;
}