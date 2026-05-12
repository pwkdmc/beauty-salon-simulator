#include "MakeupArtist.h"
#include <iostream>

MakeupArtist::MakeupArtist()
    : Employee(-1, "", "визажист", 0, 0.0, 0.0, 0.0, "работает", false)
{
    std::cout << "\033[1;31mСоздан визажист\033[0m\n";
}

MakeupArtist::MakeupArtist(const std::string &fullName, int experience, double rating,
                           double salaryBase, double salaryPercent)
    : Employee(-1, fullName, "визажист", experience, rating, salaryBase, salaryPercent, "работает", false)
{
    std::cout << "\033[1;31mСоздан визажист " << fullName << "\033[0m\n";
}

MakeupArtist::MakeupArtist(int id, const std::string &fullName, int experience, double rating,
                           double salaryBase, double salaryPercent, const std::string &status, bool isDeleted)
    : Employee(id, fullName, "визажист", experience, rating, salaryBase, salaryPercent, status, isDeleted)
{
    std::cout << "\033[1;31mСоздан визажист " << fullName << "\033[0m\n";
}

MakeupArtist::MakeupArtist(const MakeupArtist &other)
    : Employee(other)
{
    std::cout << "\033[1;31mСкопирован визажист " << fullName << "\033[0m\n";
}

MakeupArtist::~MakeupArtist()
{
    std::cout << "\033[1;31mУдален визажист " << fullName << "\033[0m\n";
}

int MakeupArtist::getMaxClientsPerDay() const
{
    return 4;
}

int MakeupArtist::getAvgServiceTime() const
{
    return 75;
}

std::string MakeupArtist::getType() const
{
    return "makeup_artist";
}
