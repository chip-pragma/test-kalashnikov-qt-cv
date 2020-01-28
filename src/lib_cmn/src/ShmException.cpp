#include "chip/ShmException.h"

#include <utility>

namespace chip {

ShmException::ShmException(const std::string& what, int ecode)
    : std::exception(), mErrno(ecode) {
    mWhat = what + " (code: " + std::to_string(mErrno) + ")";
}

const char *ShmException::what() const noexcept {
    return mWhat.c_str();
}

int ShmException::errorCode() const noexcept {
    return mErrno;
}

}