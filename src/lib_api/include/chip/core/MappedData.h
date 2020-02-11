#pragma once

#include <sys/mman.h>

#include "BaseErrorable.h"

namespace chip {

template<class TData>
class MappedData : public BaseErrorable {
public:
    static constexpr size_t size = sizeof(TData);

    MappedData() = default;

    MappedData(size_t count, int protection, int fileDescriptor);

    MappedData(MappedData&&o) noexcept;

    MappedData &operator=(MappedData &&o) noexcept;

    ~MappedData();

    bool map(size_t count, int protection, int fileDescriptor);

    bool unmap();

    bool isMapped() const;

    TData *data();

    const TData *data() const;

    size_t count() const;

    // delete copy
    MappedData(const MappedData &) = delete;

    MappedData &operator=(const MappedData &) = delete;

private:
    TData *mData = nullptr;
    size_t mCount = 0;

    void _resetMembers();
};

template<class TData>
MappedData<TData>::MappedData(size_t count, int protection, int fileDescriptor) {
    map(count, protection, fileDescriptor);
}

template<class TData>
MappedData<TData>::MappedData(MappedData &&o) noexcept {
    mData = o.mData;
    mCount = o.mCount;
    o._resetMembers();
}

template<class TData>
MappedData<TData> &MappedData<TData>::operator=(MappedData &&o) noexcept {
    mData = o.mData;
    mCount = o.mCount;
    o._resetMembers();
    return *this;
}

template<class TData>
MappedData<TData>::~MappedData() {
    unmap();
}

template<class TData>
bool MappedData<TData>::map(size_t count, int protection, int fileDescriptor) {
    // check
    if (mData) {
        mError = {"already mapped", 101};
        return false;
    }
    // map
    auto ptr = ::mmap(nullptr, size * count, protection, MAP_SHARED, fileDescriptor, 0);
    if (ptr == MAP_FAILED) {
        mError = {"map", 102};
        return false;
    }
    // success
    mCount = count;
    mData = static_cast<TData *>(ptr);
    return true;
}

template<class TData>
bool MappedData<TData>::unmap() {
    // check
    if (not mData)
        return true;
    // unmap
    auto result = ::munmap(mData, size * mCount);
    if (result < 0) {
        mError = {"unmap", 101};
        return false;
    }
    // success
    _resetMembers();
    return true;
}

template<class TData>
bool MappedData<TData>::isMapped() const {
    return static_cast<bool>(mData);
}

template<class TData>
TData *MappedData<TData>::data() {
    return mData;
}

template<class TData>
const TData *MappedData<TData>::data() const {
    return mData;
}

template<class TData>
size_t MappedData<TData>::count() const {
    return mCount;
}

template<class TData>
void MappedData<TData>::_resetMembers() {
    mData = nullptr;
    mCount = 0;
}

}

