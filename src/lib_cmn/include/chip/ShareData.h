#pragma once

#include <sys/types.h>

#include <opencv2/core.hpp>

#define CHIP_SHM_NAME ("chip_shm")
#define CHIP_SEM_NAME ("chip_sem")
#define SHD_STR_MAX_LENGTH (255)

namespace chip {
struct ShareData {
    int proc_id = 0;
    bool updated = false;
};
}