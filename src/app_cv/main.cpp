#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <csignal>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include "options.h"
#include "FrameProvider.h"

bool EXIT = false;

chip::Error shareData(int deviceId, const char* shmName = CHIP_API_DEFAULT_SHM_NAME) {

    // init cam
    cv::VideoCapture camera(deviceId);
    if (not camera.isOpened())
        return {"OpenCV", -1};

    // cam info
    auto camSize = cv::Size(camera.get(cv::CAP_PROP_FRAME_WIDTH), camera.get(cv::CAP_PROP_FRAME_HEIGHT));
    auto camType = static_cast<int>(camera.get(cv::CAP_PROP_FORMAT));
    std::cout
            << "Size:" << camSize.width << "x" << camSize.height
            << "; Type:" << camType
            << std::endl;

    // share mem
    chip::FrameProvider frameProvider(shmName, nullptr, {camSize, camType});
    if (frameProvider.lastError())
        return frameProvider.lastError();

    auto mat1 = frameProvider.mat1();
    auto mat2 = frameProvider.mat2();

    // write
    std::cout << "Working..." << std::endl;
    for (;;) {
        if (not camera.read(mat1))
            return {"read", -2};

        mat1.copyTo(mat2);

        if (EXIT) {
            std::cout << "Exit...\n";
            break;
        }

    }

    // return
    std::cout << "Finish." << std::endl;
    return {};
}

void sigHandler(int sig) {
    std::cout << "[SIG]" << std::endl;
    EXIT = true;
}

int main(int argc, char **argv) {
    auto opts = chip::parseArgs(argc, argv);

    struct sigaction sigActInt{}, sigActTerm{};
    sigActInt.sa_handler = &sigHandler;
    sigActTerm.sa_handler = &sigHandler;
    sigaction(SIGINT, &sigActInt, nullptr);
    sigaction(SIGTERM, &sigActTerm, nullptr);

    auto err = shareData(opts.deviceId, opts.shmName);
    if (err) {
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}