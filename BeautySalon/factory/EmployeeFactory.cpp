#include "EmployeeFactory.h"
#include "../models/Hairdresser.h"
#include "../models/Manicurist.h"
#include "../models/Cosmetologist.h"
#include "../models/MakeupArtist.h"
#include "../exceptions/InvalidInputException.h"

Employee *EmployeeFactory::createEmployee(const std::string &type,
                                          const std::string &fullName, int experience,
                                          double rating, double salaryBase,
                                          double salaryPercent)
{
    if (type == "парикмахер" || type == "hairdresser")
    {
        return new Hairdresser(0, fullName, experience, rating, salaryBase, salaryPercent, "работает", false);
    }
    else if (type == "маникюр" || type == "manicurist")
    {
        return new Manicurist(0, fullName, experience, rating, salaryBase, salaryPercent, "работает", false);
    }
    else if (type == "косметолог" || type == "cosmetologist")
    {
        return new Cosmetologist(0, fullName, experience, rating, salaryBase, salaryPercent, "работает", false);
    }
    else if (type == "визажист" || type == "makeup_artist")
    {
        return new MakeupArtist(0, fullName, experience, rating, salaryBase, salaryPercent, "работает", false);
    }
    else
    {
        throw InvalidInputException("Неизвестный тип сотрудника: " + type);
    }
}

Employee *EmployeeFactory::createEmployeeFromDatabase(const std::string &type, int id,
                                                      const std::string &fullName, int experience,
                                                      double rating, double salaryBase,
                                                      double salaryPercent, const std::string &status,
                                                      bool isDeleted)
{
    if (type == "парикмахер" || type == "hairdresser")
    {
        return new Hairdresser(id, fullName, experience, rating, salaryBase, salaryPercent, status, isDeleted);
    }
    else if (type == "маникюр" || type == "manicurist")
    {
        return new Manicurist(id, fullName, experience, rating, salaryBase, salaryPercent, status, isDeleted);
    }
    else if (type == "косметолог" || type == "cosmetologist")
    {
        return new Cosmetologist(id, fullName, experience, rating, salaryBase, salaryPercent, status, isDeleted);
    }
    else if (type == "визажист" || type == "makeup_artist")
    {
        return new MakeupArtist(id, fullName, experience, rating, salaryBase, salaryPercent, status, isDeleted);
    }
    else
    {
        throw InvalidInputException("Неизвестный тип сотрудника в БД: " + type);
    }
}
