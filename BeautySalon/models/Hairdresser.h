#ifndef HAIRDRESSER_H
#define HAIRDRESSER_H

#include "Employee.h"

class Hairdresser : public Employee
{
public:
    Hairdresser();
    Hairdresser(const std::string &fullName, int experience, double rating,
                double salaryBase, double salaryPercent);
    Hairdresser(int id, const std::string &fullName, int experience, double rating,
                double salaryBase, double salaryPercent, const std::string &status, bool isDeleted);
    Hairdresser(const Hairdresser &other);
    ~Hairdresser() override;

    int getMaxClientsPerDay() const override;
    int getAvgServiceTime() const override;
    std::string getType() const override;
};

#endif
