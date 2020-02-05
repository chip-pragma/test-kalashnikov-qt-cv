#include "loop.h"

#include <csignal>

namespace core {

namespace {

int _lastSig = 0;

void _sigHandler(int sig) {
    _lastSig = sig;
}

}

void runLoop(IWorker &worker) {
    // set signal
    struct sigaction sigAct{};
    sigAct.sa_handler = &_sigHandler;
    sigaction(SIGINT, &sigAct, nullptr);
    sigaction(SIGTERM, &sigAct, nullptr);

    // loop
    for (;;) {
        if (not worker.step(_lastSig))
            break;
    }

    // restore signal
    sigAct.sa_handler = SIG_DFL;
    sigaction(SIGINT, &sigAct, nullptr);
    sigaction(SIGTERM, &sigAct, nullptr);
}

}