#ifndef DATABASE_EXCEPTION_H
#define DATABASE_EXCEPTION_H

#include <stdexcept>
#include <string>

class DatabaseException : public std::runtime_error
{
public:
    explicit DatabaseException(const std::string &message)
        : std::runtime_error("Ошибка: " + message) {}
};

#endif
