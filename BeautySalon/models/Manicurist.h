#ifndef MANICURIST_H
#define MANICURIST_H

#include "Employee.h"

class Manicurist : public Employee
{
public:
    Manicurist();
    Manicurist(const std::string &fullName, int experience, double rating,
               double salaryBase, double salaryPercent);
    Manicurist(int id, const std::string &fullName, int experience, double rating,
               double salaryBase, double salaryPercent, const std::string &status, bool isDeleted);
    Manicurist(const Manicurist &other);
    ~Manicurist() override;

    int getMaxClientsPerDay() const override;
    int getAvgServiceTime() const override;
    std::string getType() const override;
};

#endif
