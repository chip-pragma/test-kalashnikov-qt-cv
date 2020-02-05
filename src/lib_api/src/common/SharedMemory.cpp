#include "common/SharedMemory.h"

namespace chip {

SharedMemory::SharedMemory(const string &name, int oflag, mode_t mode) {
    open(name, oflag, mode);
}

SharedMemory::~SharedMemory() {
    close();
}

SharedMemory::SharedMemory(SharedMemory &&o) noexcept {
    mDescriptor = o.mDescriptor;
    mName = o.mName;
    o._resetMembers();
}

SharedMemory &SharedMemory::operator=(SharedMemory &&o) noexcept {
    mDescriptor = o.mDescriptor;
    mName = o.mName;
    o._resetMembers();
    return *this;
}

bool SharedMemory::open(const string &name, int oflag, mode_t mode) {
    // check
    if (mDescriptor >= 0) {
        mError = {"already opened", 101};
        return false;
    }
    // open
    auto fd = ::shm_open(name.c_str(), oflag, mode);
    if (fd < 0) {
        mError = {"open", 102};
        return false;
    }
    // success
    mName = name;
    mDescriptor = fd;
    return true;
}

bool SharedMemory::close() {
    // check
    if (mDescriptor < 0)
        return true;
    // close
    auto rClose = ::close(mDescriptor);
    if (rClose < 0) {
        mError = {"close", 101};
        return false;
    }
    // success
    _resetMembers();
    return true;
}

bool SharedMemory::unlink() {
    // check
    if (mDescriptor < 0) {
        mError = {"is not open", 101};
        return false;
    }
    // close
    auto rClose = ::close(mDescriptor);
    if (rClose < 0) {
        mError = {"close in unlink", 102};
        return false;
    }
    // unlink
    auto rUnlink= ::shm_unlink(mName.c_str());
    if (rUnlink < 0) {
        mError = {"unlink", 103};
        return false;
    }
    // success
    _resetMembers();
    return true;
}

bool SharedMemory::isOpen() const {
    return (mDescriptor != -1);
}

int SharedMemory::descriptor() const {
    return mDescriptor;
}

bool SharedMemory::truncate(size_t length) {
    // check
    if (mDescriptor < 0) {
        mError = {"is not open", 101};
        return false;
    }
    // trunc
    auto result = ftruncate(mDescriptor, length);
    if (result < 0) {
        mError = {"truncate", 101};
        return false;
    }

    return true;
}

void SharedMemory::_resetMembers() {
    mDescriptor = -1;
    mName.clear();
}

}