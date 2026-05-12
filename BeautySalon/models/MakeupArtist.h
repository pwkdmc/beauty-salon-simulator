#ifndef MAKEUP_ARTIST_H
#define MAKEUP_ARTIST_H

#include "Employee.h"

class MakeupArtist : public Employee
{
public:
    MakeupArtist();
    MakeupArtist(const std::string &fullName, int experience, double rating,
                 double salaryBase, double salaryPercent);
    MakeupArtist(int id, const std::string &fullName, int experience, double rating,
                 double salaryBase, double salaryPercent, const std::string &status, bool isDeleted);
    MakeupArtist(const MakeupArtist &other);
    ~MakeupArtist() override;

    int getMaxClientsPerDay() const override;
    int getAvgServiceTime() const override;
    std::string getType() const override;
};

#endif
