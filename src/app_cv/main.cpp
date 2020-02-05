#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/common.h>
#include <chip/api.h>

#include "core/options.h"

namespace {

#define PRINT_ERROR(msg) \
(std::cerr << msg << std::endl)

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

void processMats(cv::Mat &mat1, cv::Mat &mat2) {
    cv::imshow("One", mat1);
    cv::imshow("Two", mat2);
}

}

int main(int argc, char **argv) {
    using namespace chip;

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
        PRINT_ERROR("Camera.Open");
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
        PRINT_ERROR("FrameInfo.SharedMemory.Open: " << infoShm.lastError().print());
        return RCode::ERR_INFO_SHM_OPEN;
    }
    // map
    MappedData<chip::FrameInfo> infoMap(1, PROT_WRITE | PROT_READ, infoShm.descriptor());
    if (not infoMap.isMapped()) {
        PRINT_ERROR("FrameInfo.MappedData.Map: " << infoMap.lastError().print());
        return RCode::ERR_INFO_MAP_MAP;
    }
    // trunc
    if (not infoShm.truncate(infoMap.size)) {
        PRINT_ERROR("FrameInfo.SharedMemory.Truncate: " << infoShm.lastError().print());
        return RCode::ERR_INFO_SHM_TRUNC;
    };
    // init
    auto infoPtr = infoMap.data();
    infoPtr->size = camSize;
    infoPtr->type = camType;

    /*
     * MATS PAIR
     */
    const auto MAT_DEPTH = CV_MAT_CN(camType);
    const auto MAT_DATA_SIZE = camSize.area() * MAT_DEPTH;
    const auto MAT_COUNT = 2;
    const auto PAIR_SIZE = MAT_DATA_SIZE * MAT_COUNT;
    // shared mem
    SharedMemory pairShm(opts.shmName, O_RDWR | O_CREAT, 0777);
    if (not pairShm.isOpen()) {
        PRINT_ERROR("MatsPair.SharedMemory.Open: " << pairShm.lastError().print());
        return RCode::ERR_PAIR_SHM_OPEN;
    }
    // map
    MappedData<uint8_t> pairMap(PAIR_SIZE, PROT_WRITE | PROT_READ, pairShm.descriptor());
    if (not pairMap.isMapped()) {
        PRINT_ERROR("MatsPair.MappedData.Map: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_MAP_MAP;
    }
    // trunc
    if (not pairShm.truncate(PAIR_SIZE)) {
        PRINT_ERROR("MatsPair.SharedMemory.Truncate: " << pairShm.lastError().print());
        return RCode::ERR_PAIR_SHM_TRUNC;
    };
    // init
    auto pairPtr = pairMap.data();
    cv::Mat matOne(camSize, camType, pairPtr[0]);
    cv::Mat matTwo(camSize, camType, pairPtr[MAT_DATA_SIZE]);

    /*
     * LOG
     */
    std::cout
            << "[OPTIONS]\n"
            << "device id: " << opts.deviceId << "\n"
            << "shm name: " << infoShmName << " | " << pairShmName << "\n"
            << "[FRAME]\n"
            << "resolution: " << infoPtr->size.width << "x" << infoPtr->size.height << " (" << MAT_DEPTH * 8 << " bit)\n"
            << "size: " << MAT_DATA_SIZE << " bytes" << std::endl;

    /*
     * PROCESSING
     */
    for (;;) {
        camera >> matOne;
        matOne.copyTo(matTwo);

        processMats(matOne, matTwo);

        if (cv::waitKey(25) == 'q')
            break;
    };

    /*
     * CLEAR
     */
    // mats pair
    if (not pairMap.unmap()) {
        PRINT_ERROR("MatsPair.MappedData.Unmap: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_MAP_UNMAP;
    }
    if (not pairShm.unlink()) {
        PRINT_ERROR("MatsPair.SharedMemory.Unlink: " << pairMap.lastError().print());
        return RCode::ERR_PAIR_SHM_UNLINK;
    }
    // frame info
    if (not infoMap.unmap()) {
        PRINT_ERROR("FrameInfo.MappedData.Unmap: " << pairMap.lastError().print());
        return RCode::ERR_INFO_MAP_UNMAP;
    }
    if (not infoShm.unlink()) {
        PRINT_ERROR("FrameInfo.SharedMemory.Unlink: " << pairMap.lastError().print());
        return RCode::ERR_INFO_SHM_UNLINK;
    }

    return RCode::SUCCESS;
}