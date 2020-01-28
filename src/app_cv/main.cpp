#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <csignal>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/Exception.h>
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

bool EXIT = false;

int shareData(int num, const std::string &str) {
    using namespace chip;

    try {
        // prepare
        cv::VideoCapture cam(num);
        if (not cam.isOpened())
            throw Exception("OpenCV");

        SharedMemory<uint8_t> sMem(CHIP_SHM_NAME, O_RDWR | O_CREAT);
        auto dataPtr = sMem.map(2, PROT_WRITE | PROT_READ);

        // write
        for(;;) {
            std::cout << dataPtr << std::endl;

            if (EXIT) {
                std::cout << "Exit...\n";
                break;
            }
            usleep(50);
        }

        // close
        sMem.unlink();
    }
    catch (Exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // return
    return EXIT_SUCCESS;
}

void sigIntHandler(int sig) {
//    std::cout << "SIGINT!!!" <<  std::endl;
    EXIT = true;
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

    struct sigaction sa{};
    sa.sa_handler = &sigIntHandler;
    sigaction(SIGINT, &sa, nullptr);

    return shareData(deviceId, shmName);
}