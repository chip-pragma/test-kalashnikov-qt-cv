#include <string>

#include <opencv2/opencv.hpp>

#include <chip/core.h>
#include <chip/api.h>

#include <chip/common/defines.h>

#define CHIP_WIN_ONE "One"
#define CHIP_WIN_TWO "Two"

namespace {

struct RCode {
    enum {
        SUCCESS = 0,
        ERR_ARGS_COUNT = 101,
        ERR_INFO_SHM_OPEN,
        ERR_INFO_MAP_MAP,
        ERR_PAIR_SHM_OPEN,
        ERR_PAIR_MAP_MAP,
        ERR_PAIR_MAP_UNMAP,
        ERR_PAIR_SHM_CLOSE,
        ERR_INFO_MAP_UNMAP,
        ERR_INFO_SHM_CLOSE,
    };
};

}

int main(int argc, char** argv) {
    using namespace chip;

    if (argc != 2) {
        PRINT_ERR("Arguments number " << argc << "!=2");
        return RCode::ERR_ARGS_COUNT;
    }

    /*
     * PREPARE
     */
    auto shmName = std::string(argv[1]);
    auto infoShmName = shmName + CHIP_SHM_INFO_NAME_POSTFIX;
    auto pairShmName = shmName + CHIP_SHM_PAIR_NAME_POSTFIX;

    /*
     * FRAME INFO
     */
    // shared mem
    SharedMemory infoShm(infoShmName, O_RDONLY, 0777);
    if (not infoShm.isOpen()) {
        PRINT_ERR("FrameInfo.SharedMemory.Open: " << infoShm.lastError());
        return RCode::ERR_INFO_SHM_OPEN;
    }
    // map
    MappedData<chip::FrameInfo> infoMap(1, PROT_READ, infoShm.descriptor());
    if (not infoMap.isMapped()) {
        PRINT_ERR("FrameInfo.MappedData.Map: " << infoMap.lastError());
        return RCode::ERR_INFO_MAP_MAP;
    }
    // init
    auto infoPtr = infoMap.data();
    cv::Size camSize = {infoPtr->width, infoPtr->height};
    auto camType = CV_MAKETYPE(infoPtr->depth, infoPtr->channels);

    /*
     * MATS PAIR
     */
    const auto MAT_DATA_SIZE = infoPtr->total();
    const auto PAIR_SIZE = MAT_DATA_SIZE * 2;
    // shared mem
    SharedMemory pairShm(pairShmName, O_RDONLY, 0777);
    if (not pairShm.isOpen()) {
        PRINT_ERR("MatsPair.SharedMemory.Open: " << pairShm.lastError());
        return RCode::ERR_PAIR_SHM_OPEN;
    }
    // map
    MappedData<uint8_t> pairMap(PAIR_SIZE, PROT_READ, pairShm.descriptor());
    if (not pairMap.isMapped()) {
        PRINT_ERR("MatsPair.MappedData.Map: " << pairMap.lastError());
        return RCode::ERR_PAIR_MAP_MAP;
    }
    // init
    auto pairPtr = pairMap.data();
    cv::Mat matOne(camSize, camType, pairPtr);
    cv::Mat matTwo(camSize, camType, pairPtr + MAT_DATA_SIZE * pairMap.size);

    /*
     * LOG
     */
    PRINT_STD("[OPTIONS]");
    PRINT_STD("shm name: " << infoShmName << " | " << pairShmName);
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
        cv::imshow(CHIP_WIN_ONE, matOne);
        cv::imshow(CHIP_WIN_TWO, matTwo);

        if (cv::waitKey(25) == 'q') {
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
    if (not pairShm.close()) {
        PRINT_ERR("MatsPair.SharedMemory.Close: " << pairShm.lastError());
        return RCode::ERR_PAIR_SHM_CLOSE;
    }
    // frame info
    if (not infoMap.unmap()) {
        PRINT_ERR("FrameInfo.MappedData.Unmap: " << infoMap.lastError());
        return RCode::ERR_INFO_MAP_UNMAP;
    }
    if (not infoShm.close()) {
        PRINT_ERR("FrameInfo.SharedMemory.Close: " << infoShm.lastError());
        return RCode::ERR_INFO_SHM_CLOSE;
    }

    PRINT_STD("\nGOODBYE!");
    return RCode::SUCCESS;
}