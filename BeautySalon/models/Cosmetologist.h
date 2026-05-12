#ifndef COSMETOLOGIST_H
#define COSMETOLOGIST_H

#include "Employee.h"

class Cosmetologist : public Employee
{
public:
    Cosmetologist();
    Cosmetologist(const std::string &fullName, int experience, double rating,
                  double salaryBase, double salaryPercent);
    Cosmetologist(int id, const std::string &fullName, int experience, double rating,
                  double salaryBase, double salaryPercent, const std::string &status, bool isDeleted);
    Cosmetologist(const Cosmetologist &other);
    ~Cosmetologist() override;

    int getMaxClientsPerDay() const override;
    int getAvgServiceTime() const override;
    std::string getType() const override;
};

#endif
