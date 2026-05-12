#include "Cosmetologist.h"
#include <iostream>

Cosmetologist::Cosmetologist()
    : Employee(-1, "", "косметолог", 0, 0.0, 0.0, 0.0, "работает", false)
{
    std::cout << "\033[1;31mСоздан косметолог\033[0m\n";
}

Cosmetologist::Cosmetologist(const std::string &fullName, int experience, double rating,
                             double salaryBase, double salaryPercent)
    : Employee(-1, fullName, "косметолог", experience, rating, salaryBase, salaryPercent, "работает", false)
{
    std::cout << "\033[1;31mСоздан косметолог " << fullName << "\033[0m\n";
}

Cosmetologist::Cosmetologist(int id, const std::string &fullName, int experience, double rating,
                             double salaryBase, double salaryPercent, const std::string &status, bool isDeleted)
    : Employee(id, fullName, "косметолог", experience, rating, salaryBase, salaryPercent, status, isDeleted)
{
    std::cout << "\033[1;31mСоздан косметолог " << fullName << "\033[0m\n";
}

Cosmetologist::Cosmetologist(const Cosmetologist &other)
    : Employee(other)
{
    std::cout << "\033[1;31mСкопирован косметолог " << fullName << "\033[0m\n";
}

Cosmetologist::~Cosmetologist()
{
    std::cout << "\033[1;31mУдален косметолог " << fullName << "\033[0m\n";
}

int Cosmetologist::getMaxClientsPerDay() const
{
    return 5;
}

int Cosmetologist::getAvgServiceTime() const
{
    return 90;
}

std::string Cosmetologist::getType() const
{
    return "cosmetologist";
}
