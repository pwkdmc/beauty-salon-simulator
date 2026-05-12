#ifndef EMPLOYEE_FACTORY_H
#define EMPLOYEE_FACTORY_H

#include <string>
#include <memory>
#include "../models/Employee.h"

class EmployeeFactory
{
public:
    static Employee *createEmployee(const std::string &type,
                                    const std::string &fullName, int experience,
                                    double rating, double salaryBase,
                                    double salaryPercent);
    static Employee *createEmployeeFromDatabase(const std::string &type, int id,
                                                const std::string &fullName, int experience,
                                                double rating, double salaryBase,
                                                double salaryPercent, const std::string &status,
                                                bool isDeleted);
};

#endif
