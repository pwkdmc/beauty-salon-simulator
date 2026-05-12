#include "Employee.h"
#include "../database/DatabaseManager.h"
#include "../exceptions/DatabaseException.h"
#include "../exceptions/NotFoundException.h"
#include <iostream>
#include <sstream>

int Employee::totalCreated = 0;

Employee::Employee()
    : id(-1), fullName(""), specialization(""), experience(0), rating(0.0),
      salaryBase(0.0), salaryPercent(0.0), status("работает"), isDeleted(false)
{
    std::cout << "\033[1;31mСоздан сотрудник\033[0m\n";
}

Employee::Employee(int id, const std::string &fullName, const std::string &specialization,
                   int experience, double rating, double salaryBase, double salaryPercent,
                   const std::string &status, bool isDeleted)
    : id(id), fullName(fullName), specialization(specialization), experience(experience),
      rating(rating), salaryBase(salaryBase), salaryPercent(salaryPercent),
      status(status), isDeleted(isDeleted)
{
    totalCreated++;
    std::cout << "\033[1;31mСоздан сотрудник " << fullName << "\033[0m\n";
}

Employee::Employee(const Employee &other)
    : id(other.id), fullName(other.fullName), specialization(other.specialization),
      experience(other.experience), rating(other.rating), salaryBase(other.salaryBase),
      salaryPercent(other.salaryPercent), status(other.status), isDeleted(other.isDeleted)
{
    totalCreated++;
    std::cout << "\033[1;31mСкопирован сотрудник " << fullName << "\033[0m\n";
}

Employee::~Employee()
{
    std::cout << "\033[1;31mУдален сотрудник " << fullName << "\033[0m\n";
}

int Employee::getId() const { return id; }
std::string Employee::getFullName() const { return fullName; }
std::string Employee::getSpecialization() const { return specialization; }
int Employee::getExperience() const { return experience; }
double Employee::getRating() const { return rating; }
double Employee::getSalaryBase() const { return salaryBase; }
double Employee::getSalaryPercent() const { return salaryPercent; }
std::string Employee::getStatus() const { return status; }
bool Employee::getIsDeleted() const { return isDeleted; }

void Employee::setFullName(const std::string &name) { fullName = name; }
void Employee::setExperience(int exp) { experience = exp; }
void Employee::setRating(double rat) { rating = rat; }
void Employee::setSalaryBase(double base) { salaryBase = base; }
void Employee::setSalaryPercent(double percent) { salaryPercent = percent; }
void Employee::setStatus(const std::string &st) { status = st; }
void Employee::setIsDeleted(bool del) { isDeleted = del; }

double Employee::calculateSalary(double servicesSum) const
{
    return salaryBase + (servicesSum * salaryPercent / 100.0);
}

void Employee::printInfo() const
{
    std::cout << fullName << " — " << specialization
              << ", стаж: " << experience << " лет"
              << ", рейтинг: " << rating
              << ", оклад: " << salaryBase
              << ", процент: " << salaryPercent
              << ", статус: " << status << "\n";
}

void Employee::saveToDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "INSERT INTO employees (full_name, specialization, experience, rating, "
        << "salary_base, salary_percent, status, is_deleted) VALUES ('"
        << fullName << "', '" << specialization << "', " << experience << ", "
        << rating << ", " << salaryBase << ", " << salaryPercent << ", '"
        << status << "', " << (isDeleted ? 1 : 0) << ") RETURNING id";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) id = std::stoi(row[0]); });
    std::ostringstream typeOss;
    typeOss << "INSERT INTO employee_types (employee_id, type, max_clients_per_day, avg_service_time) VALUES ("
            << id << ", '" << getType() << "', " << getMaxClientsPerDay() << ", " << getAvgServiceTime() << ")";
    db->execute(typeOss.str());
}

void Employee::updateInDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE employees SET full_name='" << fullName << "', specialization='"
        << specialization << "', experience=" << experience << ", rating=" << rating
        << ", salary_base=" << salaryBase << ", salary_percent=" << salaryPercent
        << ", status='" << status << "', is_deleted=" << (isDeleted ? 1 : 0)
        << " WHERE id=" << id;
    db->execute(oss.str());
}

void Employee::softDeleteFromDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE employees SET status='уволен', is_deleted=1 WHERE id=" << id;
    db->execute(oss.str());
    isDeleted = true;
    status = "уволен";
}

std::vector<Employee *> Employee::loadAllFromDatabase()
{
    std::vector<Employee *> list;
    return list;
}

Employee *Employee::loadFromDatabase(int employeeId)
{
    return nullptr;
}

int Employee::getTotalCreated() { return totalCreated; }
void Employee::resetTotalCreated() { totalCreated = 0; }
