#pragma once

#include <opencv2/opencv.hpp>

namespace chip {

struct FrameInfo {
    cv::Size size;
    int type;
    uint64_t seq = 0;
};

}