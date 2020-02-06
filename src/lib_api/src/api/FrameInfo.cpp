#include "chip/api/FrameInfo.h"

namespace chip {

int FrameInfo::total() const {
    return width * height * channels;
}

int FrameInfo::step() const {
    return width * channels;
}

}