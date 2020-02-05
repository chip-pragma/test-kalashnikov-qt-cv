#include "chip/api/FrameInfo.h"

namespace chip {

int FrameInfo::size() const {
    return width * height * channels;
}

}