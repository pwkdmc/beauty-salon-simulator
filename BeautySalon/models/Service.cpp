#include "Service.h"
#include "../database/DatabaseManager.h"
#include "../exceptions/NotFoundException.h"
#include <iostream>
#include <sstream>

Service::Service()
    : id(-1), name(""), duration(0), cost(0.0), specialization(""), materialsCost(0.0)
{
    std::cout << "\033[1;31mСоздана услуга\033[0m\n";
}

Service::Service(int id, const std::string &name, int duration, double cost,
                 const std::string &specialization, double materialsCost)
    : id(id), name(name), duration(duration), cost(cost),
      specialization(specialization), materialsCost(materialsCost)
{
    std::cout << "\033[1;31mСоздана услуга " << name << "\033[0m\n";
}

Service::~Service()
{
    std::cout << "\033[1;31mУдалена услуга " << name << "\033[0m\n";
}

int Service::getId() const { return id; }
std::string Service::getName() const { return name; }
int Service::getDuration() const { return duration; }
double Service::getCost() const { return cost; }
std::string Service::getSpecialization() const { return specialization; }
double Service::getMaterialsCost() const { return materialsCost; }

void Service::setName(const std::string &n) { name = n; }
void Service::setDuration(int d) { duration = d; }
void Service::setCost(double c) { cost = c; }
void Service::setSpecialization(const std::string &s) { specialization = s; }
void Service::setMaterialsCost(double m) { materialsCost = m; }

void Service::saveToDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "INSERT INTO services (name, duration, cost, specialization, materials_cost) VALUES ('"
        << name << "', " << duration << ", " << cost << ", '" << specialization << "', "
        << materialsCost << ") RETURNING id";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) id = std::stoi(row[0]); });
}

void Service::updateInDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE services SET name='" << name << "', duration=" << duration
        << ", cost=" << cost << ", specialization='" << specialization
        << "', materials_cost=" << materialsCost << " WHERE id=" << id;
    db->execute(oss.str());
}

void Service::deleteFromDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "DELETE FROM services WHERE id=" << id;
    db->execute(oss.str());
}

std::vector<Service> Service::loadAllFromDatabase()
{
    std::vector<Service> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, name, duration, cost, specialization, materials_cost FROM services",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 6)
            {
                list.emplace_back(std::stoi(row[0]), row[1], std::stoi(row[2]),
                                  std::stod(row[3]), row[4], std::stod(row[5]));
            }
        });
    return list;
}

Service Service::loadFromDatabase(int serviceId)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    Service s;
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, name, duration, cost, specialization, materials_cost FROM services WHERE id=" << serviceId;
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 6) {
            s = Service(std::stoi(row[0]), row[1], std::stoi(row[2]),
                        std::stod(row[3]), row[4], std::stod(row[5]));
            found = true;
        } });
    if (!found)
    {
        throw NotFoundException("Услуга с ID=" + std::to_string(serviceId) + " не найдена");
    }
    return s;
}

std::vector<Service> Service::loadBySpecialization(const std::string &spec)
{
    std::vector<Service> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, name, duration, cost, specialization, materials_cost FROM services "
        << "WHERE specialization='" << spec << "'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 6) {
            list.emplace_back(std::stoi(row[0]), row[1], std::stoi(row[2]),
                              std::stod(row[3]), row[4], std::stod(row[5]));
        } });
    return list;
}
