#ifndef FORBIDDEN_ACTION_EXCEPTION_H
#define FORBIDDEN_ACTION_EXCEPTION_H

#include <stdexcept>
#include <string>

class ForbiddenActionException : public std::runtime_error
{
public:
    explicit ForbiddenActionException(const std::string &message)
        : std::runtime_error("Ошибка: " + message) {}
};

#endif
