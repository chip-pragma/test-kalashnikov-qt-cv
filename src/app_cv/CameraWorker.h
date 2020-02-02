#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <chip/api.h>

#include "loop.h"
#include "FrameProvider.h"

namespace chip {

class CameraWorker : public chip::IWorker,
                     public chip::BaseErrorable {
public:
    CameraWorker() = default;

    CameraWorker(int deviceId, const char *shmName);

    bool init(int deviceId, const char *shmName);

    bool final();

    bool isInit() const;

    bool step(const int& sig) override;

private:
    cv::VideoCapture mCamera;
    chip::FrameProvider mFrameProvider;
    cv::Mat mMat1, mMat2;

    void _processFrame();
};

}


