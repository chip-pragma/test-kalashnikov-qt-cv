#pragma once

#include <iostream>
#include <string>

namespace chip {

struct Error {
    std::string msg;
    int code = 0;

    bool log() const;

    operator bool() const;
};

std::ostream &operator<<(std::ostream &os, const Error& error);


class BaseErrorable {
public:
    Error lastError() const;

protected:
    Error mError;
};

}