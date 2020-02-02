#include "api/FrameConsumer.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace chip {

FrameConsumer::FrameConsumer(const char *shmName) {
    // open shm
    auto shmFd = ::shm_open(shmName, O_RDONLY , 0777);
    if (shmFd < 0) {
        raiseError("shm_open", errno);
        return;
    }

    // map
    auto infoMap = ::mmap(nullptr, FRAMES_INFO_SIZE, PROT_READ, MAP_SHARED, shmFd, 0);
    if (infoMap == MAP_FAILED) {
        raiseError("mmap.FrameInfo", errno);
        return;
    }
    auto infoPtr = static_cast<FrameInfo *>(infoMap);
    cv::Mat mat(infoPtr->size, infoPtr->type, nullptr);
    auto matSize = mat.rows * mat.step;

    auto mat1Ptr = ::mmap(nullptr, matSize, PROT_READ, MAP_SHARED, shmFd, FRAMES_INFO_SIZE);
    if (mat1Ptr == MAP_FAILED) {
        raiseError("mmap.Mat1", errno);
        return;
    }
    auto mat2Ptr = ::mmap(nullptr, matSize, PROT_READ, MAP_SHARED, shmFd, FRAMES_INFO_SIZE + matSize);
    if (mat2Ptr == MAP_FAILED) {
        raiseError("mmap.Mat2", errno);
        return;
    }

    // done
    mShmFd = shmFd;
    mMatSize = matSize;
    mInfo = infoPtr;
    mMat1 = static_cast<uint8_t *>(mat1Ptr);
    mMat2 = static_cast<uint8_t *>(mat2Ptr);
}

FrameConsumer::~FrameConsumer() {
    ::munmap(mInfo, FRAMES_INFO_SIZE);
    ::munmap(mMat1, mMatSize);
    ::munmap(mMat2, mMatSize);
    ::close(mShmFd);
}

bool FrameConsumer::isInit() const {
    return mInfo;
}

const FrameInfo &FrameConsumer::framesInfo() const {
    return *mInfo;
}

cv::Mat FrameConsumer::makeMat1() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat1);
}

cv::Mat FrameConsumer::makeMat2() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat2);
}

}