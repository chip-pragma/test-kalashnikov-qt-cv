#include "main.h"

#include <iostream>

#include <csignal>

#include <chip/core.h>
#include <chip/api.h>

#define CHIP_WIN_ONE "One"
#define CHIP_WIN_TWO "Two"

namespace {

struct RCode {
    enum {
        SUCCESS = 0,
        ERR_CAMERA_OPEN = 101,
        ERR_INFO_SHM_OPEN,
        ERR_INFO_MAP_MAP,
        ERR_INFO_SHM_TRUNC,
        ERR_PAIR_SHM_OPEN,
        ERR_PAIR_MAP_MAP,
        ERR_PAIR_SHM_TRUNC,
        ERR_PAIR_MAP_UNMAP,
        ERR_PAIR_SHM_CLOSE,
        ERR_INFO_MAP_UNMAP,
        ERR_INFO_SHM_CLOSE,
    };
};

}

// SIGNALS
namespace {

int _lastSig = 0;

void _sigHandler(int sig) {
    _lastSig = sig;
}

}

int main(int argc, char **argv) {
    using namespace chip;

    /*
     * PREPARE
     */
    // set signal
    struct sigaction sigAct{};
    sigAct.sa_handler = &_sigHandler;
    sigaction(SIGINT, &sigAct, nullptr);
    sigaction(SIGTERM, &sigAct, nullptr);

    /*
     * OPTIONS
     */
    auto opts = core::parseArgs(argc, argv);
    auto infoShmName = opts.shmName + CHIP_SHM_INFO_NAME_POSTFIX;
    auto pairShmName = opts.shmName + CHIP_SHM_PAIR_NAME_POSTFIX;

    /*
     * INIT
     */
    cv::VideoCapture camera;
    camera.open(opts.deviceId);
    if (not camera.isOpened()) {
        PRINT_ERR("Camera.Open");
        return RCode::ERR_CAMERA_OPEN;
    }
    // cam info
    auto camSize = cv::Size(camera.get(cv::CAP_PROP_FRAME_WIDTH), camera.get(cv::CAP_PROP_FRAME_HEIGHT));
    auto camType = static_cast<int>(camera.get(cv::CAP_PROP_FORMAT));
    auto camFps = static_cast<int>(camera.get(cv::CAP_PROP_FPS));

    /*
     * FRAME INFO
     */
    // shared mem
    SharedMemory infoShm(infoShmName, O_RDWR | O_CREAT, 0777);
    if (not infoShm.isOpen()) {
        PRINT_ERR("FrameInfo.SharedMemory.Open: " << infoShm.lastError());
        return RCode::ERR_INFO_SHM_OPEN;
    }
    // map
    MappedData<chip::FrameInfo> infoMap(1, PROT_WRITE | PROT_READ, infoShm.descriptor());
    if (not infoMap.isMapped()) {
        PRINT_ERR("FrameInfo.MappedData.Map: " << infoMap.lastError());
        return RCode::ERR_INFO_MAP_MAP;
    }
    // trunc
    if (not infoShm.truncate(infoMap.size)) {
        PRINT_ERR("FrameInfo.SharedMemory.Truncate: " << infoShm.lastError());
        return RCode::ERR_INFO_SHM_TRUNC;
    };
    // init
    auto infoPtr = infoMap.data();
    infoPtr->width = camSize.width;
    infoPtr->height = camSize.height;
    infoPtr->channels = CV_MAT_CN(camType);
    infoPtr->depth = CV_MAT_DEPTH(camType);
    infoPtr->fps = camFps;

    /*
     * MATS PAIR
     */
    const auto MAT_DATA_SIZE = infoPtr->total();
    const auto PAIR_SIZE = MAT_DATA_SIZE * 2;
    // shared mem
    SharedMemory pairShm(pairShmName, O_RDWR | O_CREAT, 0777);
    if (not pairShm.isOpen()) {
        PRINT_ERR("MatsPair.SharedMemory.Open: " << pairShm.lastError());
        return RCode::ERR_PAIR_SHM_OPEN;
    }
    // map
    MappedData<uint8_t> pairMap(PAIR_SIZE, PROT_WRITE | PROT_READ, pairShm.descriptor());
    if (not pairMap.isMapped()) {
        PRINT_ERR("MatsPair.MappedData.Map: " << pairMap.lastError());
        return RCode::ERR_PAIR_MAP_MAP;
    }
    // trunc
    if (not pairShm.truncate(PAIR_SIZE)) {
        PRINT_ERR("MatsPair.SharedMemory.Truncate: " << pairShm.lastError());
        return RCode::ERR_PAIR_SHM_TRUNC;
    }
    // init
    auto pairPtr = pairMap.data();
    cv::Mat matOne(camSize, camType, pairPtr);
    cv::Mat matTwo(camSize, camType, pairPtr + MAT_DATA_SIZE * pairMap.size);

    /*
     * LOG
     */
    PRINT_STD("[OPTIONS]");
    PRINT_STD("device id: " << opts.deviceId);
    PRINT_STD("shm name: " << infoShmName << " | " << pairShmName);
    PRINT_STD("processing: " << (opts.processing ? "enable" : "disable"));
    PRINT_STD("[FRAME]");
    PRINT_STD("resolution: " << infoPtr->width << "x" << infoPtr->height << ", " << infoPtr->channels * 8
                             << "bit, " << infoPtr->fps << "fps");
    PRINT_STD("size: " << MAT_DATA_SIZE << " bytes");

    /*
     * PROCESSING
     */
    // work
    PRINT_STD(">> WORKING...");
    for (;;) {
        if (camera.read(matOne)) {
            if (opts.processing)
                processMats(matOne, matTwo);
        }

        if (opts.show) {
            cv::imshow(CHIP_WIN_ONE, matOne);
            cv::imshow(CHIP_WIN_TWO, matTwo);
        }

        if (_lastSig != 0 or cv::waitKey(25) == 'q') {
            PRINT_STD(">> EXIT...");
            break;
        }
    }

    /*
     * CLEAR
     */

    // mats pair
    if (not pairMap.unmap()) {
        PRINT_ERR("MatsPair.MappedData.Unmap: " << pairMap.lastError());
        return RCode::ERR_PAIR_MAP_UNMAP;
    }
    if (not pairShm.unlink()) {
        PRINT_ERR("MatsPair.SharedMemory.Unlink: " << pairShm.lastError());
        return RCode::ERR_PAIR_SHM_CLOSE;
    }
    // frame info
    if (not infoMap.unmap()) {
        PRINT_ERR("FrameInfo.MappedData.Unmap: " << infoMap.lastError());
        return RCode::ERR_INFO_MAP_UNMAP;
    }
    if (not infoShm.unlink()) {
        PRINT_ERR("FrameInfo.SharedMemory.Unlink: " << infoShm.lastError());
        return RCode::ERR_INFO_SHM_CLOSE;
    }

    PRINT_STD("\nGOODBYE!");
    return RCode::SUCCESS;
}

void processMats(cv::Mat &mat1, cv::Mat &mat2) {
    // test process
    cv::flip(mat1, mat2, 1);
    cv::cvtColor(mat1, mat1, cv::COLOR_BGR2RGB);
}