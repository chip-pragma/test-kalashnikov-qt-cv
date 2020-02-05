#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/common.h>

#include "core/options.h"

#define PRINT_ERROR(msg) \
(std::cerr << msg << std::endl)

struct RCode {
    enum {
        SUCCESS = 0,
        ERR_CAMERA_OPEN = 101,
        ERR_MAT_SHM_OPEN,
        ERR_MAY_SHM_TRUNC,
        ERR_MAT_MAP_MAP,
        ERR_MAT_MAP_UNMAP,
        ERR_MAT_SHM_UNLINK,
    };
};


int main(int argc, char **argv) {
    using namespace chip;

    /*
     * OPTIONS
     */
    auto opts = core::parseArgs(argc, argv);

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
    std::cout
            << "Size:" << camSize.width << "x" << camSize.height
            << "; "
            << "Type:" << camType
            << std::endl;

    /*
     * FRAME MEMORY
     */
    auto matSize = camSize.area() * CV_MAT_CN(camType);
    // shared mem
    SharedMemory matMem("chip_Mat", O_RDWR | O_CREAT, 0777);
    if (not matMem.isOpen()) {
        PRINT_ERROR("Mat.SharedMemory.Open: " << matMem.lastError().print());
        return RCode::ERR_MAT_SHM_OPEN;
    }
    if (not matMem.truncate(matSize)) {
        PRINT_ERROR("Mat.SharedMemory.Truncate: " << matMem.lastError().print());
        return RCode::ERR_MAY_SHM_TRUNC;
    };
    // map
    MappedData<uint8_t> matMap(matSize, PROT_WRITE | PROT_READ, matMem.descriptor());
    if (not matMap.isMapped()) {
        PRINT_ERROR("Mat.MappedData.Map: " << matMap.lastError().print());
        return RCode::ERR_MAT_MAP_MAP;
    }
    cv::Mat mat(camSize, camType, matMap.data());

    /*
     * TEST WORK
     */
    std::cout << "FD:" << matMem.descriptor() << " | Count:" << matMap.count() << std::endl;

    for (;;) {
        camera >> mat;
        cv::imshow("", mat);
        if (cv::waitKey(10) == 'q')
            break;
    };

//    auto intArray = matMap.data();
//    for (size_t i = 0; i < matSize; i++) {
//        intArray[i] = i * i;
//        std::cout << intArray[i] << "; ";
//    }

    /*
     * CLEAR
     */
    if (not matMap.unmap()) {
        PRINT_ERROR("Mat.MappedData.Unmap: " << matMap.lastError().print());
        return RCode::ERR_MAT_MAP_UNMAP;
    }
    if (not matMem.unlink()) {
        PRINT_ERROR("Mat.SharedMemory.Unlink: " << matMap.lastError().print());
        return RCode::ERR_MAT_SHM_UNLINK;
    }

    return RCode::SUCCESS;
}