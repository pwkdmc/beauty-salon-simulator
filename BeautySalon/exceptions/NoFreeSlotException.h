#ifndef NO_FREE_SLOT_EXCEPTION_H
#define NO_FREE_SLOT_EXCEPTION_H

#include <stdexcept>
#include <string>

class NoFreeSlotException : public std::runtime_error
{
public:
    explicit NoFreeSlotException(const std::string &message)
        : std::runtime_error("Ошибка: " + message) {}
};

#endif
