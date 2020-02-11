#include "options.h"

#include <cstdlib>
#include <argp.h>

namespace core {
namespace {

#define CHIP_OPT_DEVICE_ID  'd'
#define CHIP_OPT_SHM_NAME  'm'
#define CHIP_OPT_SHOW_PAIR  's'
#define CHIP_OPT_NO_PROC 'p'

argp_option _options[] = {
        {"device-id",  CHIP_OPT_DEVICE_ID, "INT", 0,                     "OpenCV camera device ID"},
        {"shm-name",   CHIP_OPT_SHM_NAME,  "STRING", 0,                  "Shared memory name"},
        {"show",       CHIP_OPT_SHOW_PAIR, nullptr, OPTION_ARG_OPTIONAL, "Show result mats pair"},
        {"no-process", CHIP_OPT_NO_PROC,   nullptr, OPTION_ARG_OPTIONAL, "Disable image processing"},
        {nullptr}
};

error_t _parserFunc(int key, char *arg, struct argp_state *state) {
    auto opts = static_cast<Options *>(state->input);
    char *end;

    switch (key) {
        case CHIP_OPT_DEVICE_ID:
            opts->deviceId = strtol(arg, &end, 10);
            if (end == arg)
                return ARGP_KEY_ERROR;
            break;
        case CHIP_OPT_SHM_NAME:
            opts->shmName = arg;
            break;
        case CHIP_OPT_SHOW_PAIR:
            opts->show = true;
            break;
        case CHIP_OPT_NO_PROC:
            opts->processing = false;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

char _argsDoc[] = "ARG1 ARG2";

char _doc[] =
        "FrameProvider. Cheltsov Ivan, 2020.";

argp _argp = {_options, _parserFunc, _argsDoc, _doc};

}

Options parseArgs(int argc, char **argv) {
    Options opts;
    argp_parse(&_argp, argc, argv, 0, nullptr, &opts);
    return opts;
}
}