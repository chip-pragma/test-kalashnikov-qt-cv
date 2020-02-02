#include "FrameProvider.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace chip {

FrameProvider::FrameProvider(const char *shmName, const char *semName, const FrameInfo& info) {
    // open shm
    auto shmFd = ::shm_open(shmName, O_RDWR | O_CREAT , 0777);
    if (shmFd < 0) {
        raiseError(CHIP_ERR_SHM_OPEN, errno);
        return;
    }

    // truncate
    auto matSize = info.size.area();

    auto memTrunc = ftruncate(shmFd, FRAMES_INFO_SIZE + matSize * 2);
    if (memTrunc < 0) {
        raiseError(CHIP_ERR_FTRUNCATE, errno);
        return;
    }

    // map
    auto prot = PROT_READ | PROT_WRITE;

    auto infoMap = ::mmap(nullptr, FRAMES_INFO_SIZE, prot, MAP_SHARED, shmFd, 0);
    if (infoMap == MAP_FAILED) {
        raiseError(CHIP_ERR_MMAP_FRAME_INFO, errno);
        return;
    }
    auto infoPtr = static_cast<FrameInfo *>(infoMap);
    *infoPtr = info;
    auto mat1Ptr = ::mmap(nullptr, matSize, prot, MAP_SHARED, shmFd, FRAMES_INFO_SIZE);
    if (mat1Ptr == MAP_FAILED) {
        raiseError(CHIP_ERR_MMAP_MAT1, errno);
        return;
    }
    auto mat2Ptr = ::mmap(nullptr, matSize, prot, MAP_SHARED, shmFd, FRAMES_INFO_SIZE + matSize);
    if (mat2Ptr == MAP_FAILED) {
        raiseError(CHIP_ERR_MMAP_MAT2, errno);
        return;
    }

    // done
    mShmName = shmName;
    mShmFd = shmFd;
    mMatSize = matSize;
    mInfo = infoPtr;
    mMat1 = static_cast<uint8_t *>(mat1Ptr);
    mMat2 = static_cast<uint8_t *>(mat2Ptr);
}

FrameProvider::~FrameProvider() {
    ::munmap(mInfo, FRAMES_INFO_SIZE);
    ::munmap(mMat1, mMatSize);
    ::munmap(mMat2, mMatSize);
    ::close(mShmFd);
    ::shm_unlink(mShmName);
}

const FrameInfo &FrameProvider::frameInfo() const {
    return *mInfo;
}

cv::Mat FrameProvider::mat1() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat1);
}

cv::Mat FrameProvider::mat2() const {
    return cv::Mat(mInfo->size, mInfo->type, mMat2);
}

}