#pragma once

#include <string>

#include <opencv2/core.hpp>

#include "chip/api.h"


namespace chip {

class FrameProvider : public BaseErrorable {
public:
    ~FrameProvider();

    bool init(const char *shmName, const FrameInfo &info);

    bool final();

    bool isInit() const;

    const FrameInfo& frameInfo() const;

    cv::Mat mat1() const;

    cv::Mat mat2() const;

private:
    char const *mShmName = nullptr;
    int mShmFd = -1;
    size_t mFullSize = 0;
    FrameInfo *mFrameInfo = nullptr;
    uint8_t *mMat1Data = nullptr;
    uint8_t *mMat2Data = nullptr;
};

}