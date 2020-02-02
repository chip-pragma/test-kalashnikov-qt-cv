#pragma once

#include <string>

#include <opencv2/core.hpp>

#include "chip/api.h"


namespace chip {

class FrameProvider : public BaseErrorable {
public:
    explicit FrameProvider(const char* shmName, const char* semName, const FrameInfo& info);

    ~FrameProvider();

    const FrameInfo& frameInfo() const;

    cv::Mat mat1() const;

    cv::Mat mat2() const;

private:
    char const *mShmName = nullptr;
    int mShmFd = -1;
    size_t mMatSize = 0;
    FrameInfo *mInfo = nullptr;
    uint8_t *mMat1 = nullptr;
    uint8_t *mMat2 = nullptr;
};

}
