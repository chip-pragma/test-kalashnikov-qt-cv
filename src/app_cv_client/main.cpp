#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <chip/common.h>
#include <chip/ShareData.h>

int readShareData() {
    // prepare
    chip::ShareData data;

    const auto dataSize = sizeof(chip::ShareData);

    // open
    auto handle = shm_open(_DATA_NAME, O_RDONLY, 0777);
    if (handle == -1) {
        chip::logError("shm_open", errno);
        return EXIT_FAILURE;
    }

    auto mappedPtr = mmap(nullptr, dataSize, PROT_READ, MAP_SHARED, handle, 0);
    auto dataPtr = static_cast<chip::ShareData*>(mappedPtr);
    if (mappedPtr == reinterpret_cast<void*>(-1)) {
        chip::logError("mmap", errno);
        return EXIT_FAILURE;
    }

    // status
    std::cout << "num = " << dataPtr->num << "\nstr = " << dataPtr->str << "\nmat.elemSize = " << dataPtr->mat.elemSize();

    std::string anykey;
    std::getline(std::cin, anykey);

    // close
    auto rUnmap = munmap(dataPtr, dataSize);
    if (rUnmap == -1)
        chip::logError("munmap", errno);

    auto rClose = close(handle);
    if (rClose == -1)
        chip::logError("close", errno);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {

    std::cout << "I'm CV client!" << std::endl;
    return readShareData();
}