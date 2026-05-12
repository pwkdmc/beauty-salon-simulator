#include "Manicurist.h"
#include <iostream>

Manicurist::Manicurist()
    : Employee(-1, "", "маникюр", 0, 0.0, 0.0, 0.0, "работает", false)
{
    std::cout << "\033[1;31mСоздан мастер маникюра\033[0m\n";
}

Manicurist::Manicurist(const std::string &fullName, int experience, double rating,
                       double salaryBase, double salaryPercent)
    : Employee(-1, fullName, "маникюр", experience, rating, salaryBase, salaryPercent, "работает", false)
{
    std::cout << "\033[1;31mСоздан мастер маникюра " << fullName << "\033[0m\n";
}

Manicurist::Manicurist(int id, const std::string &fullName, int experience, double rating,
                       double salaryBase, double salaryPercent, const std::string &status, bool isDeleted)
    : Employee(id, fullName, "маникюр", experience, rating, salaryBase, salaryPercent, status, isDeleted)
{
    std::cout << "\033[1;31mСоздан мастер маникюра " << fullName << "\033[0m\n";
}

Manicurist::Manicurist(const Manicurist &other)
    : Employee(other)
{
    std::cout << "\033[1;31mСкопирован мастер маникюра " << fullName << "\033[0m\n";
}

Manicurist::~Manicurist()
{
    std::cout << "\033[1;31mУдален мастер маникюра " << fullName << "\033[0m\n";
}

int Manicurist::getMaxClientsPerDay() const
{
    return 6;
}

int Manicurist::getAvgServiceTime() const
{
    return 60;
}

std::string Manicurist::getType() const
{
    return "manicurist";
}
