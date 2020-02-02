#include "chip/api/BaseErrorable.h"

#include <iostream>

namespace chip {

bool Error::log() const {
    if (code != 0) {
        std::cerr << *this << std::endl;
        return true;
    }
    return false;
}

Error::operator bool() const {
    return code != 0;
}

std::ostream &operator<<(std::ostream &os, const Error &error) {
    return os << error.msg << " (code: " << error.code << ")";
}

Error BaseErrorable::lastError() const {
    return mError;
}

}