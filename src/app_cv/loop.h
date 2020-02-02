#pragma once

#include <csignal>
#include <functional>

namespace chip {

class IWorker {
public:
    virtual bool step(const int& sig) = 0;
};

void runLoop(IWorker& worker);

}




