#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#include <opencv2/opencv.hpp>

#include <chip/common.h>
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
    // prepare
    chip::ShareData data;

    const auto dataSize = sizeof(chip::ShareData);

    // open
    auto handle = shm_open(CHIP_SHM_NAME, O_RDWR | O_CREAT, 0777);
    if (handle == -1) {
        chip::logError("shm_open", errno);
        return EXIT_FAILURE;
    }

    auto rFTrunc = ftruncate(handle, dataSize);
    if (rFTrunc == -1) {
        chip::logError("ftruncate", errno);
        return EXIT_FAILURE;
    }

    auto mappedPtr = mmap(nullptr, dataSize, PROT_WRITE, MAP_SHARED, handle, 0);
    auto dataPtr = static_cast<chip::ShareData *>(mappedPtr);
    if (mappedPtr == reinterpret_cast<void *>(-1)) {
        chip::logError("mmap", errno);
        return EXIT_FAILURE;
    }

    // write
    dataPtr->num = num;
    strncpy(dataPtr->str, str.data(), SHD_STR_MAX_LENGTH - 1);

    // status
    std::cout
        << "num = " << dataPtr->num
        << "\nstr = " << dataPtr->str
        << "\nmat.elemSize = " << dataPtr->mat.elemSize();

    std::string anykey;
    std::getline(std::cin, anykey);

    // close
    auto rUnmap = munmap(dataPtr, dataSize);
    if (rUnmap == -1)
        chip::logError("munmap", errno);

    auto rClose = close(handle);
    if (rClose == -1)
        chip::logError("close", errno);

    auto rShmUnlink = shm_unlink(CHIP_SHM_NAME);
    if (rShmUnlink == -1)
        chip::logError("shm_unlink", errno);

    // return
    return EXIT_SUCCESS;
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