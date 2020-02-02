#pragma once

#define CHIP_API_DEFAULT_SHM_NAME "chip_frames"

namespace chip {

struct FrameInfo {
    cv::Size size;
    int type;
};

constexpr size_t FRAMES_INFO_SIZE = sizeof(FrameInfo);

}