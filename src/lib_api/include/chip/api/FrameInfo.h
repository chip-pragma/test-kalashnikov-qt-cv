#pragma once

namespace chip {

struct FrameInfo {
    int width = 0;
    int height = 0;
    int channels = 0;

    int size() const;
};

}