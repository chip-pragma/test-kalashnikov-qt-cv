#include "core/BaseErrorable.h"

#include <iostream>
#include <sstream>

namespace chip {

bool Error::print() const {
    if (code != 0) {
        std::cerr << *this << std::endl;
        return true;
    }
    return false;
}

Error::operator bool() const {
    return code != 0;
}

std::string Error::toStr() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Error &error) {
    return os << error.msg << " (code: " << error.code << ")";
}

Error BaseErrorable::lastError() const {
    return mError;
}

}