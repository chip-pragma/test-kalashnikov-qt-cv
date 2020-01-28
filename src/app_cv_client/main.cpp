#include <iostream>

#include <csignal>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/core/ShareData.h>
#include <chip/common/SharedMemory.h>
#include <chip/common/Exception.h>

bool EXIT = false;

int readShareData() {
    using namespace chip;

    try {
        // open
        SharedMemory<CameraInfo> sCamInfo(CHIP_CAMERA_INFO_SHM, O_RDWR);
        auto camInfoPtr = sCamInfo.map(1, PROT_READ);
        cv::Mat mat1(camInfoPtr->size, camInfoPtr->type);
        sCamInfo.close();

        SharedMemory<uint8_t> sMat1(CHIP_MAT1_SHM, O_RDONLY);
        mat1.data = sMat1.map(mat1.rows * mat1.step, PROT_READ);

        for (;;) {
            if (EXIT) {
                std::cout << "Exit...\n";
                break;
            }

            cv::imshow("Test", mat1);
            if (cv::waitKey(30) >= 0)
                break;
        }

        // close
        sMat1.close();
    }
    catch (Exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void sigIntHandler(int sig) {
//    std::cout << "SIGINT!!!" <<  std::endl;
    EXIT = true;
}

int main(int argc, char **argv) {

    struct sigaction sa{};
    sa.sa_handler = &sigIntHandler;
    sigaction(SIGINT, &sa, nullptr);

    std::cout << "I'm CV client!" << std::endl;
    return readShareData();
}