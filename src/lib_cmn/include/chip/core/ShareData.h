#pragma once

#include <sys/types.h>

#include <opencv2/core.hpp>

#define CHIP_CAMERA_INFO_SHM ("chip_camera_info")
#define CHIP_MAT1_SHM ("chip_mat1")
#define CHIP_MAT2_SHM ("chip_mat2")

namespace chip {
struct CameraInfo {
    cv::Size size;
    int type;
};
}