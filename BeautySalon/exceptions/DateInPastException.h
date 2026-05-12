#ifndef DATE_IN_PAST_EXCEPTION_H
#define DATE_IN_PAST_EXCEPTION_H

#include <stdexcept>
#include <string>

class DateInPastException : public std::runtime_error
{
public:
    explicit DateInPastException(const std::string &message)
        : std::runtime_error("Ошибка: " + message) {}
};

#endif
