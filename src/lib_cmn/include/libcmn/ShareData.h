#pragma once

#include <string>
#include <opencv2/core.hpp>

#define _DATA_NAME ("chip")

namespace chip {
struct ShareData {
    int num = 0;
    cv::Mat mat;
    std::string str;
};
}