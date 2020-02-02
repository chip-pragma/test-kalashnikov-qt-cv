#pragma once

#include <iostream>
#include <string>

namespace chip {

struct Error {
    std::string msg;
    int code;

    operator bool() const;

    std::ostream &operator<<(std::ostream &os);
};

class BaseErrorable {
public:
    Error lastError() const;

protected:
    void raiseError(const char *msg, int code);

private:
    Error mError;

};

}