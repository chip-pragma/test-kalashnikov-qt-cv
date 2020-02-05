#include "options.h"

#include <iostream>
#include <string>

#include <cstdlib>
#include <argp.h>

namespace core {
namespace {

argp_option _options[] = {
        {"device-id", 'd', "INT", 0, "OpenCV camera device ID"},
        {"shm-name", 'm', "STRING", 0, "Shared memory name"},
        {nullptr}
};

error_t _parserFunc(int key, char *arg, struct argp_state *state) {
    auto opts = static_cast<Options *>(state->input);
    char *end;

    switch (key) {
        case 'd':
            opts->deviceId = strtol(arg, &end, 10);
            if (end == arg)
                return ARGP_KEY_ERROR;
            break;
        case 'm':
            opts->shmName = arg;
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