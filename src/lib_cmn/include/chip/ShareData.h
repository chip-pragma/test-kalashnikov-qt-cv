#pragma once

#include <sys/types.h>

#include <opencv2/core.hpp>

#define _DATA_NAME ("chip")

#define SHD_STR_MAX_LENGTH 255

namespace chip {
struct ShareData {
    int num = 0;
    cv::Mat mat;
    char str[SHD_STR_MAX_LENGTH];
};
}