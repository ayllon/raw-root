#ifndef __INFERREREXCEPTION_HPP
#define __INFERREREXCEPTION_HPP

#include <cstdarg>
#include <exception>

class InferrerException: public std::exception
{
protected:
    char msg[512];

public:

    InferrerException(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(msg, sizeof(msg), fmt, args);
        va_end(args);
    }

    virtual const char* what() const throw() {
        return msg;
    }
};

#endif // __INFERREREXCEPTION_HPP
