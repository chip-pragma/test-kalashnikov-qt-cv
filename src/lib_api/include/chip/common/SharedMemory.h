#pragma once

#include <string>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "BaseErrorable.h"

namespace chip {

namespace { using namespace std; }

class SharedMemory : public BaseErrorable {
public:
    SharedMemory() = default;

    SharedMemory(const string& name, int oflag, mode_t mode);

    ~SharedMemory();

    SharedMemory(SharedMemory&&o) noexcept;

    SharedMemory &operator=(SharedMemory &&o) noexcept;

    bool open(const string& name, int oflag,  mode_t mode);

    bool close();

    bool unlink();

    bool isOpen() const;

    int descriptor() const;

    bool truncate(size_t length);

    // delete copy
    SharedMemory(const SharedMemory &) = delete;

    SharedMemory &operator=(const SharedMemory &) = delete;

private:
    string mName;
    int mDescriptor = -1;

    void _resetMembers();
};

}




