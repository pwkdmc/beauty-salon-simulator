#ifndef INVALID_INPUT_EXCEPTION_H
#define INVALID_INPUT_EXCEPTION_H

#include <stdexcept>
#include <string>

class InvalidInputException : public std::runtime_error
{
public:
    explicit InvalidInputException(const std::string &message)
        : std::runtime_error("Ошибка: " + message) {}
};

#endif
