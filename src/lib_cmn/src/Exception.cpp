#include "chip/Exception.h"

#include <utility>

namespace chip {

Exception::Exception(const std::string& what, int ecode)
    : std::exception(), mErrno(ecode) {
    mWhat = what + " (code: " + std::to_string(mErrno) + ")";
}

const char *Exception::what() const noexcept {
    return mWhat.c_str();
}

int Exception::errorCode() const noexcept {
    return mErrno;
}

}