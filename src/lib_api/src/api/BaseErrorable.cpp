#include "chip/api/BaseErrorable.h"

namespace chip {

Error::operator bool() const {
    return not (msg.empty() and code == 0);
}

std::ostream &Error::operator<<(std::ostream &os) {
    return os << msg << " (code:" << code << ")";
}

Error BaseErrorable::lastError() const {
    return mError;
}

void BaseErrorable::raiseError(const char *msg, int code) {
    mError = {msg, code};
}

}