#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <vector>

class Service
{
private:
    int id;
    std::string name;
    int duration;
    double cost;
    std::string specialization;
    double materialsCost;

public:
    Service();
    Service(int id, const std::string &name, int duration, double cost,
            const std::string &specialization, double materialsCost);
    ~Service();

    int getId() const;
    std::string getName() const;
    int getDuration() const;
    double getCost() const;
    std::string getSpecialization() const;
    double getMaterialsCost() const;

    void setName(const std::string &n);
    void setDuration(int d);
    void setCost(double c);
    void setSpecialization(const std::string &s);
    void setMaterialsCost(double m);

    void saveToDatabase();
    void updateInDatabase();
    void deleteFromDatabase();

    static std::vector<Service> loadAllFromDatabase();
    static Service loadFromDatabase(int serviceId);
    static std::vector<Service> loadBySpecialization(const std::string &spec);
};

#endif
