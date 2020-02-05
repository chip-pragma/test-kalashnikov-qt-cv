#include "FrameProvider.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace chip {

FrameProvider::FrameProvider(const char *shmName, const FrameInfo &info) {
    init(shmName, info);
}

FrameProvider::~FrameProvider() {
    final();
}

bool FrameProvider::init(const char *shmName, const FrameInfo &info) {
    if (mFrameInfo)
        return true;

    // FRAME INFO
    // open
    auto fInfoShm = ::shm_open(shmName, O_RDWR | O_CREAT | O_EXCL, 0777);
    if (fInfoShm < 0) {
        mError = {"FrameInfo.shm_open", 101};
        return false;
    }
    // truncate
    auto fInfoTrunc = ::ftruncate(fInfoShm, FRAMES_INFO_SIZE);
    if (fInfoTrunc < 0) {
        mError = {"FrameInfo.ftruncate", 102};
        return false;
    }
    // map
    auto fInfoMap = ::mmap(nullptr, FRAMES_INFO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fInfoShm, 0);
    if (fInfoMap == MAP_FAILED) {
        mError = {"FrameInfo.mmap", 103};
        return false;
    }

    // MATS
    // open
    auto matShm = ::shm_open(shmName, O_RDWR | O_CREAT | O_EXCL, 0777);
    if (matShm < 0) {
        mError = {"FrameInfo.shm_open", 101};
        return false;
    }
    // truncate
    auto matTrunc = ftruncate(fInfoShm, FRAMES_INFO_SIZE);
    if (matTrunc < 0) {
        mError = {"FrameInfo.ftruncate", 102};
        return false;
    }
    // map
    auto matMap = ::mmap(nullptr, FRAMES_INFO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fInfoShm, 0);
    if (matMap == MAP_FAILED) {
        mError = {"FrameInfo.mmap", 103};
        return false;
    }

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
    mShmInfo = shmName;
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
    auto rUnlink =::shm_unlink(mShmInfo);
    if (rUnlink < 0) {
        mError = {"shm_unlink", 103};
        return false;
    }

    mShmInfo = nullptr;
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