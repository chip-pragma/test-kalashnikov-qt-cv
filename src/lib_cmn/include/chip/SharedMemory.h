#pragma once

#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ShmException.h"

namespace chip {

template<class TData>
class SharedMemory {
public:
    static constexpr size_t dataSize = sizeof(TData);

    SharedMemory() = default;

    SharedMemory(const std::string &name, int oflag);

    ~SharedMemory();

    void open(const std::string &name, int oflag);

    void close();

    bool isOpen() const noexcept;

    void unlink();

    const std::string &name() const noexcept;

    TData *map(size_t count, int prot, int flags = MAP_SHARED);

    void unmap();

    bool isMapped() const noexcept;

    size_t count() const noexcept;

    TData *data() noexcept;

    const TData *data() const noexcept;

private:
    std::string mName;
    int mShmFd = -1;
    int mFlag = 0;
    TData *mDataPtr = nullptr;
    size_t mCount = 0;
};

template<class TData>
SharedMemory<TData>::SharedMemory(const std::string &name, int oflag) {
    open(name, oflag);
}

template<class TData>
SharedMemory<TData>::~SharedMemory() {
    try {
        close();
    }
    catch (...) {
        std::cout << "SharedMemory close error: " << errno << std::endl;
    }
}

template<class TData>
void SharedMemory<TData>::open(const std::string &name, int oflag) {
    mShmFd = shm_open(name.c_str(), oflag, 0777);
    if (mShmFd == -1) {
        throw ShmException("shm_open", errno);
    }
    mName = name;
    mFlag = oflag;
}

template<class TData>
void SharedMemory<TData>::close() {
    if (mShmFd == -1)
        return;

    unmap();

    auto rc = ::close(mShmFd);
    if (rc == -1)
        throw ShmException("close", errno);

    mShmFd = -1;
}

template<class TData>
bool SharedMemory<TData>::isOpen() const noexcept {
    return mShmFd != -1;
}

template<class TData>
void SharedMemory<TData>::unlink() {
    close();
    auto rc = shm_unlink(mName.c_str());
    if (rc == -1)
        throw ShmException("shm_unlink", errno);

    mName.clear();
}

template<class TData>
const std::string &SharedMemory<TData>::name() const noexcept {
    return mName;
}

template<class TData>
TData *SharedMemory<TData>::map(size_t count, int prot, int flags) {
    if (mShmFd == -1)
        return nullptr;
    if (mDataPtr) {
        if (count == mCount)
            return mDataPtr;
        else
            throw ShmException("count");
    }

    const auto fullSize = count * dataSize;
    if (mFlag & O_CREAT and
        prot & PROT_WRITE) {
        auto rc = ftruncate(mShmFd, fullSize);
        if (rc == -1)
            throw ShmException("ftruncate", errno);
    }

    auto mappedPtr = mmap(nullptr, fullSize, prot, flags, mShmFd, 0);
    if (mappedPtr == MAP_FAILED)
        throw ShmException("mmap", errno);

    mDataPtr = static_cast<TData *>(mappedPtr);
    mCount = count;
    return mDataPtr;
}

template<class TData>
void SharedMemory<TData>::unmap() {
    if (not mDataPtr)
        return;

    const auto fullSize = mCount * dataSize;
    auto rc = munmap(mDataPtr, fullSize);
    if (rc == -1)
        throw ShmException("munmap", errno);

    mDataPtr = nullptr;
    mCount = 0;
}

template<class TData>
bool SharedMemory<TData>::isMapped() const noexcept {
    return mDataPtr;
}

template<class TData>
size_t SharedMemory<TData>::count() const noexcept {
    return mCount;
}

template<class TData>
TData *SharedMemory<TData>::data() noexcept {
    return mDataPtr;
}

template<class TData>
const TData *SharedMemory<TData>::data() const noexcept {
    return mDataPtr;
}

}




