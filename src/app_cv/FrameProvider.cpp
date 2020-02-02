#include "FrameProvider.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace chip {

FrameProvider::~FrameProvider() {
    final();
}

bool FrameProvider::init(const char *shmName, const FrameInfo &info) {
    if (mFrameInfo)
        return true;

    // open shm
    auto shmFd = ::shm_open(shmName, O_RDWR | O_CREAT , 0777);
    if (shmFd < 0) {
        mError = {"shm_open", 101};
        return false;
    }

    // truncate
    auto matSize = info.size.area();
    auto fullSize = FRAMES_INFO_SIZE + matSize * 2;

    auto memTrunc = ftruncate(shmFd, fullSize);
    if (memTrunc < 0) {
        mError = {"ftruncate", 102};
        return false;
    }

    // map
    auto fullPtr = ::mmap(nullptr, fullSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (fullPtr == MAP_FAILED) {
        mError = {"mmap", 103};
        return false;
    }

    // done
    mShmName = shmName;
    mShmFd = shmFd;
    mFullSize = fullSize;
    mFrameInfo = static_cast<FrameInfo*>(fullPtr);
    mMat1Data = &(static_cast<uint8_t *>(fullPtr)[FRAMES_INFO_SIZE]);
    mMat2Data = &(static_cast<uint8_t *>(fullPtr)[FRAMES_INFO_SIZE + matSize]);

    return true;
}

bool FrameProvider::final() {
    if (not mFrameInfo)
        return true;

    auto rUnmap = ::munmap(mFrameInfo, mFullSize);
    if (rUnmap < 0) {
        mError = {"munmap", 101};
        return false;
    }
    auto rClose = ::close(mShmFd);
    if (rClose < 0) {
        mError = {"close", 102};
        return false;
    }
    auto rUnlink =::shm_unlink(mShmName);
    if (rUnlink < 0) {
        mError = {"shm_unlink", 103};
        return false;
    }

    mShmName = nullptr;
    mShmFd = -1;
    mFullSize = 0;
    mFrameInfo = nullptr;
    mMat1Data = nullptr;
    mMat2Data = nullptr;

    return true;
}

bool FrameProvider::isInit() const {
    return mFrameInfo;
}

const FrameInfo &FrameProvider::frameInfo() const {
    return *mFrameInfo;
}

cv::Mat FrameProvider::mat1() const {
    return cv::Mat(mFrameInfo->size, mFrameInfo->type, mMat1Data);
}

cv::Mat FrameProvider::mat2() const {
    return cv::Mat(mFrameInfo->size, mFrameInfo->type, mMat2Data);
}

}