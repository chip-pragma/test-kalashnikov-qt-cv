#pragma once

#include <string>
#include <exception>

namespace chip {

class ShmException : public std::exception {
public:
    explicit ShmException(const std::string& what = "ShmException", int ecode = 0);

    const char *what() const noexcept override;

    int errorCode() const noexcept;

private:
    std::string mWhat;
    int mErrno = 0;
};

}




