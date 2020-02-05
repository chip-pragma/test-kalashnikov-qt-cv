#pragma once

#include <string>

#include <opencv2/core.hpp>

#include "chip/api.h"
#include "BaseErrorable.h"

namespace chip {

class FrameConsumer : public BaseErrorable {
public:
    FrameConsumer() = default;

    explicit FrameConsumer(const char* shmName);

    ~FrameConsumer();

    bool init(const char* shmName);

    bool final();

    bool isInit() const;

    const FrameInfo& framesInfo() const;

    cv::Mat mat1() const;

    cv::Mat mat2() const;

private:
    int mShmFd = -1;
    size_t mMatSize = 0;
    FrameInfo *mInfo = nullptr;
    uint8_t *mMat1 = nullptr;
    uint8_t *mMat2 = nullptr;
};

}
