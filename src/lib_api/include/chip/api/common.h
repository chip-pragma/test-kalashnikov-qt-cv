#pragma once

#define CHIP_API_DEFAULT_SHM_NAME "chip_frames"

#define CHIP_ERR_SHM_OPEN "shm_open"
#define CHIP_ERR_FTRUNCATE "ftruncate"
#define CHIP_ERR_MMAP_FRAME_INFO "mmap.FrameInfo"
#define CHIP_ERR_MMAP_MAT1 "mmap.Mat1"
#define CHIP_ERR_MMAP_MAT2 "mmap.Mat2"

namespace chip {

struct FrameInfo {
    cv::Size size;
    int type;
};

constexpr size_t FRAMES_INFO_SIZE = sizeof(FrameInfo);

}