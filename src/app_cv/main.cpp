#include "main.h"

#include <iostream>

#include <csignal>

#include <chip/common.h>
#include <chip/api.h>

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
        ERR_PAIR_SHM_UNLINK,
        ERR_INFO_MAP_UNMAP,
        ERR_INFO_SHM_UNLINK,
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
     * INIT CAMERA
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

    /*
     * FRAME INFO
     */
    // shared mem
    SharedMemory infoShm(infoShmName, O_RDWR | O_CREAT, 0777);
    if (not infoShm.isOpen()) {
        PRINT_ERR("FrameInfo.SharedMemory.Open: " << infoShm.lastError().print());
        return RCode::ERR_INFO_SHM_OPEN;
    }
    // map
    MappedData<chip::FrameInfo> infoMap(1, PROT_WRITE | PROT_READ, infoShm.descriptor());
    if (not infoMap.isMapped()) {
        PRINT_ERR("FrameInfo.MappedData.Map: " << infoMap.lastError().print());
        return RCode::ERR_INFO_MAP_MAP;
    }
    // trunc
    if (not infoShm.truncate(infoMap.size)) {
        PRINT_ERR("FrameInfo.SharedMemory.Truncate: " << infoShm.lastError().print());
        return RCode::ERR_INFO_SHM_TRUNC;
    };
    // init
    auto infoPtr = infoMap.data();
    infoPtr->size = camSize;
    infoPtr->channels = CV_MAT_CN(camType);

    /*
     * MATS PAIR
     */
    const auto MAT_DATA_SIZE = infoPtr->size.area() * infoPtr->channels;
    const auto MAT_COUNT = 2;
    const auto PAIR_SIZE = MAT_DATA_SIZE * MAT_COUNT;
    // shared mem
    SharedMemory pairShm(opts.shmName, O_RDWR | O_CREAT, 0777);
    if (not pairShm.isOpen()) {
        PRINT_ERR("MatsPair.SharedMemory.Open: " << pairShm.lastError().print());
        return RCode::ERR_PAIR_SHM_OPEN;
    }
    // map
    MappedData<uint8_t> pairMap(PAIR_SIZE, PROT_WRITE | PROT_READ, pairShm.descriptor());
    if (not pairMap.isMapped()) {
        PRINT_ERR("MatsPair.MappedData.Map: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_MAP_MAP;
    }
    // trunc
    if (not pairShm.truncate(PAIR_SIZE)) {
        PRINT_ERR("MatsPair.SharedMemory.Truncate: " << pairShm.lastError().print());
        return RCode::ERR_PAIR_SHM_TRUNC;
    }
    // init
    auto pairPtr = pairMap.data();
    cv::Mat matOne(camSize, camType, pairPtr[0]);
    cv::Mat matTwo(camSize, camType, pairPtr[MAT_DATA_SIZE]);

    /*
     * LOG
     */
    PRINT_STD("[OPTIONS]");
    PRINT_STD("device id: " << opts.deviceId);
    PRINT_STD("shm name: " << infoShmName << " | " << pairShmName);
    PRINT_STD("[FRAME]");
    PRINT_STD("resolution: " << infoPtr->size.width << "x" << infoPtr->size.height << " (" << infoPtr->channels * 8
                             << " bit)");
    PRINT_STD("size: " << MAT_DATA_SIZE << " bytes");

    /*
     * PROCESSING
     */
    PRINT_STD(">> WORKING...");
    for (;;) {
        camera >> matOne;
        matOne.copyTo(matTwo);

        processMats(matOne, matTwo);

        if (_lastSig != 0) {
            PRINT_STD(">> EXIT...");
            break;
        }
    }

    /*
     * CLEAR
     */
    // mats pair
    if (not pairMap.unmap()) {
        PRINT_ERR("MatsPair.MappedData.Unmap: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_MAP_UNMAP;
    }
    if (not pairShm.unlink()) {
        PRINT_ERR("MatsPair.SharedMemory.Unlink: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_SHM_UNLINK;
    }
    // frame info
    if (not infoMap.unmap()) {
        PRINT_ERR("FrameInfo.MappedData.Unmap: " << pairMap.lastError().print());
        return RCode::ERR_INFO_MAP_UNMAP;
    }
    if (not infoShm.unlink()) {
        PRINT_ERR("FrameInfo.SharedMemory.Unlink: " << pairMap.lastError().print());
        return RCode::ERR_INFO_SHM_UNLINK;
    }

    PRINT_STD("\nGOODBYE!");
    return RCode::SUCCESS;
}

void processMats(cv::Mat &mat1, cv::Mat &mat2) {
//    cv::imshow("One", mat1);
//    cv::imshow("Two", mat2);
}