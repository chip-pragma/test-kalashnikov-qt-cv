#pragma once

#include <string>

#include <opencv2/core.hpp>

#include "common.h"

#include "BaseErrorable.h"

namespace chip {

class FrameConsumer : public BaseErrorable {
public:
    explicit FrameConsumer(const char* shmName = CHIP_API_DEFAULT_SHM_NAME);

    ~FrameConsumer();

    bool isInit() const;

    const FrameInfo& framesInfo() const;

    cv::Mat makeMat1() const;

    cv::Mat makeMat2() const;

private:
    int mShmFd = -1;
    size_t mMatSize = 0;
    FrameInfo *mInfo = nullptr;
    uint8_t *mMat1 = nullptr;
    uint8_t *mMat2 = nullptr;
};

}
