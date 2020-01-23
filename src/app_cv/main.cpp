#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <opencv2/opencv.hpp>

#include <chip/ShmException.h>
#include <chip/SharedMemory.h>
#include <chip/ShareData.h>

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

int shareData(int num, const std::string &str) {
    using namespace chip;

    try {
        // prepare
        SharedMemory<ShareData> shm(CHIP_SHM_NAME, O_RDWR | O_CREAT);
        auto dp = shm.map(PROT_WRITE);

        // write
        dp->num = num;
        strncpy(dp->str, str.data(), SHD_STR_MAX_LENGTH - 1);

        // status
        std::cout
            << "num = " << dp->num
            << "\nstr = " << dp->str
            << "\nmat.elemSize = " << dp->mat.elemSize();

        std::string anykey;
        std::getline(std::cin, anykey);

        // close
        shm.unlink();
    }
    catch (ShmException &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // return
    return EXIT_SUCCESS;
}

void sigIntHandler(int sig) {

}

int main(int argc, char **argv) {
    using namespace chip;

    int deviceId = 0;
    std::string shmName;

    CmdLineHelper cmdLn(argc, argv);
    try {
        shmName = cmdLn.getOptionValue("-s", makeShmName());
        deviceId = std::stoi(cmdLn.getOptionValue("-d", "0"));
    }
    catch (std::invalid_argument &e) {
        std::cerr << e.what();
        exit(EXIT_FAILURE);
    }

//    std::cout << "Device ID: " << deviceId << "\nShm Name: " << shmName << std::endl;

    return shareData(deviceId, shmName);
}