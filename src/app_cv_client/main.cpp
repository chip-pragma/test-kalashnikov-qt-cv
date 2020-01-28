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
        SharedMemory<char> shm(CHIP_SHM_NAME, O_RDWR);
        auto dp = shm.map(2, PROT_WRITE | PROT_READ);

        for (;;) {
            dp[0] = rand() % 10 + '0';

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