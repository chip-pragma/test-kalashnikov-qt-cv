#include "common.h"

#include <iostream>

namespace chip {

void logError(const std::string &msg, int code) {
    std::cerr << msg << "(" << code << ")" << std::endl;
}

}