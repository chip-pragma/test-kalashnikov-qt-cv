#pragma once

#include <csignal>
#include <functional>

namespace core {

class IWorker {
public:
    virtual bool step(const int& sig) = 0;
};

void runLoop(IWorker& worker);

}




