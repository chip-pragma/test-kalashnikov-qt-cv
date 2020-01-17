#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <opencv2/opencv.hpp>

#include <libcmn/ShareData.h>

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

#define error(msg) \
    std::cerr << "ERROR: " << msg << std::endl;


int shareData(int num, const std::string &str) {
    chip::ShareData data;
    data.num = num;
    data.str = str;

    const auto dataSize = sizeof(chip::ShareData);

    auto handle = shm_open(_DATA_NAME, O_RDWR | O_CREAT, 0777);
    if (handle == -1) {
        error("shm_open" << errno);
        return EXIT_FAILURE;
    }

    auto rFTrunc = ftruncate(handle, dataSize);
    if (rFTrunc == -1) {
        error("ftruncate: " << errno);
        return EXIT_FAILURE;
    }

    auto dataPtr = mmap(nullptr, dataSize, PROT_WRITE, MAP_SHARED, handle, 0);
    if (dataPtr == reinterpret_cast<void*>(-1)) {
        error("mmap: " << errno);
        return EXIT_FAILURE;
    }

    memcpy(dataPtr, &data, dataSize);


    std::cout << "num = " << data.num << "\nstr = " << data.str << "\nmat.elemSize = " << data.mat.elemSize();

    std::string anykey;
    std::getline(std::cin, anykey);

    auto rUnmap = munmap(dataPtr, dataSize);
    if (rUnmap == -1)
        error("munmap" << errno);

    auto rClose = close(handle);
    if (rClose == -1)
        error("close" << errno);

    auto rShmUnlink = shm_unlink(_DATA_NAME);
    if (rShmUnlink == -1)
        error("shm_unlink" << errno);

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