#include "common/FrameConsumer.h"

#include <cstring>

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>


namespace chip {

FrameConsumer::FrameConsumer(const char *shmName) {
    init(shmName);
}

FrameConsumer::~FrameConsumer() {
    ::munmap(mInfo, FRAMES_INFO_SIZE);
    ::munmap(mMat1, mMatSize);
    ::munmap(mMat2, mMatSize);
    ::close(mShmFd);
}

bool FrameConsumer::init(const char *shmName) {
    // open shm
    auto shmFd = ::shm_open(shmName, O_RDONLY , 0777);
    if (shmFd < 0) {
        mError = {"shm_open", 101};
        return false;
    }

    // map
    auto fullPtr = ::mmap(nullptr, fullSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (fullPtr == MAP_FAILED) {
        mError = {"mmap", 103};
        return false;
    }

    auto infoPtr = static_cast<FrameInfo *>(infoMap);
    cv::Mat mat(infoPtr->size, infoPtr->type, nullptr);
    auto matSize = mat.rows * mat.step;


    // done
    mShmFd = shmFd;
    mMatSize = matSize;
    mInfo = infoPtr;
    mMat1 = static_cast<uint8_t *>(mat1Ptr);
    mMat2 = static_cast<uint8_t *>(mat2Ptr);

    return true;
}

bool FrameConsumer::final() {
    return true;
}

bool FrameConsumer::isInit() const {
    return mInfo;
}

const FrameInfo &FrameConsumer::framesInfo() const {
    return *mInfo;
}

cv::Mat FrameConsumer::mat1() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat1);
}

cv::Mat FrameConsumer::mat2() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat2);
}

}