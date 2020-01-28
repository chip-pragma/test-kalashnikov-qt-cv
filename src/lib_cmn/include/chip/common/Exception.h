#pragma once

#include <string>
#include <exception>

namespace chip {

class Exception : public std::exception {
public:
    explicit Exception(const std::string& what = "Exception", int ecode = 0);

    const char *what() const noexcept override;

    int errorCode() const noexcept;

private:
    std::string mWhat;
    int mErrno = 0;
};

}




