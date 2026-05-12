#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <vector>

class Employee
{
protected:
    int id;
    std::string fullName;
    std::string specialization;
    int experience;
    double rating;
    double salaryBase;
    double salaryPercent;
    std::string status;
    bool isDeleted;
    static int totalCreated;

public:
    Employee();
    Employee(int id, const std::string &fullName, const std::string &specialization,
             int experience, double rating, double salaryBase, double salaryPercent,
             const std::string &status, bool isDeleted);
    Employee(const Employee &other);
    virtual ~Employee();

    int getId() const;
    std::string getFullName() const;
    std::string getSpecialization() const;
    int getExperience() const;
    double getRating() const;
    double getSalaryBase() const;
    double getSalaryPercent() const;
    std::string getStatus() const;
    bool getIsDeleted() const;

    void setFullName(const std::string &name);
    void setExperience(int exp);
    void setRating(double rat);
    void setSalaryBase(double base);
    void setSalaryPercent(double percent);
    void setStatus(const std::string &st);
    void setIsDeleted(bool del);

    virtual double calculateSalary(double servicesSum) const;
    virtual int getMaxClientsPerDay() const = 0;
    virtual int getAvgServiceTime() const = 0;
    virtual std::string getType() const = 0;
    virtual void printInfo() const;

    void saveToDatabase();
    void updateInDatabase();
    void softDeleteFromDatabase();

    static std::vector<Employee *> loadAllFromDatabase();
    static Employee *loadFromDatabase(int employeeId);
    static int getTotalCreated();
    static void resetTotalCreated();
};

#endif
