#include <iostream>

#include <signal.h>

#include <chip/ShareData.h>
#include <chip/SharedMemory.h>
#include <chip/ShmException.h>

bool EXIT = false;

int readShareData() {
    using namespace chip;

    try {
        // open
        SharedMemory<ShareData> shm(CHIP_SHM_NAME, O_RDWR);
        auto dp = shm.map(PROT_WRITE | PROT_READ);

        int id = std::rand() % 9000 + 1000;
        std::cout << "Working with ID: " << id << std::endl;
        for (;;) {
            dp->proc_id = id;
            dp->updated = true;

            if (EXIT) {
                std::cout << "Exit...\n";
                break;
            }
            usleep(50);
        }

        // close
        shm.close();
    }
    catch (ShmException &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void sigIntHandler(int sig) {
//    std::cout << "SIGINT!!!" <<  std::endl;
    EXIT = true;
}

int main(int argc, char **argv) {

    struct sigaction sa{};
    sa.sa_handler = &sigIntHandler;
    sigaction(SIGINT, &sa, nullptr);

    std::cout << "I'm CV client!" << std::endl;
    return readShareData();
}