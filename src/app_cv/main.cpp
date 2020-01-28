#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <csignal>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/common/Exception.h>
#include <chip/common/SharedMemory.h>
#include <chip/core/ShareData.h>

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
        cv::VideoCapture camera(num);
        if (not camera.isOpened())
            throw Exception("OpenCV");

        cv::Size camSize(
                camera.get(cv::CAP_PROP_FRAME_WIDTH),
                camera.get(cv::CAP_PROP_FRAME_HEIGHT)
        );
        auto camType = static_cast<int>(camera.get(cv::CAP_PROP_FORMAT));

        cv::Mat mat1(camSize, camType);

        SharedMemory<CameraInfo> sCamInfo(CHIP_CAMERA_INFO_SHM, O_RDWR | O_CREAT);
        auto camInfoPtr = sCamInfo.map(1, PROT_WRITE | PROT_READ);
        camInfoPtr->size = camSize;
        camInfoPtr->type = camType;

        SharedMemory<uint8_t> aMat1(CHIP_MAT1_SHM, O_RDWR | O_CREAT);
        mat1.data = aMat1.map(mat1.rows * mat1.step, PROT_WRITE | PROT_READ);

        // write
        std::cout << "Working..." << std::endl;
        for (;;) {
            camera >> mat1;

            if (EXIT) {
                std::cout << "Exit...\n";
                break;
            }
        }

        // close
        aMat1.unlink();
        sCamInfo.unlink();
    }
    catch (Exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // return
    std::cout << "Finish." << std::endl;
    return EXIT_SUCCESS;
}

void sigHandler(int sig) {
    std::cout << "[SIG]" << std::endl;
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

    struct sigaction sigActInt{}, sigActTerm{};
    sigActInt.sa_handler = &sigHandler;
    sigActTerm.sa_handler = &sigHandler;
    sigaction(SIGINT, &sigActInt, nullptr);
    sigaction(SIGTERM, &sigActTerm, nullptr);

    return shareData(deviceId, shmName);
}