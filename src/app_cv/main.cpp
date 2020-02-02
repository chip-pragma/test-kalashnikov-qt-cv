#include <cstdlib>

#include "options.h"
#include "CameraWorker.h"

int main(int argc, char **argv) {
    auto opts = chip::parseArgs(argc, argv);
    if (not opts.shmName)
        opts.shmName = CHIP_API_DEFAULT_SHM_NAME;

    chip::CameraWorker worker(opts.deviceId, opts.shmName);
    if (worker.lastError().log())
        return EXIT_FAILURE;

    chip::runLoop(worker);

    if (worker.lastError().log())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}