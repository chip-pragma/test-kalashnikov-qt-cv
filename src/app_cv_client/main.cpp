#include <iostream>

#include <chip/ShareData.h>
#include <chip/SharedMemory.h>
#include <chip/ShmException.h>

int readShareData() {
    using namespace chip;

    try {
        // open
        SharedMemory<ShareData> shm(CHIP_SHM_NAME, O_RDONLY);
        auto dp = shm.map(PROT_READ);

        // status
        std::cout
            << "num = " << dp->num
            << "\nstr = " << dp->str
            << "\nmat.elemSize = " << dp->mat.elemSize();

        std::string anykey;
        std::getline(std::cin, anykey);

        // close
        shm.close();
    }
    catch (ShmException &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {

    std::cout << "I'm CV client!" << std::endl;
    return readShareData();
}