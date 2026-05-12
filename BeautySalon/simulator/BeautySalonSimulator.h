#ifndef BEAUTY_SALON_SIMULATOR_H
#define BEAUTY_SALON_SIMULATOR_H

#include <string>
#include <vector>
#include <memory>
#include "../models/Employee.h"
#include "../models/Client.h"
#include "../models/Service.h"
#include "../models/Appointment.h"

class BeautySalonSimulator
{
private:
    std::string currentDate;
    double cashBalance;
    double totalRevenue;
    double totalExpenses;
    std::string lastMonthSalaryPaid;
    bool initialized;

    std::vector<std::unique_ptr<Employee>> employees;
    std::vector<std::unique_ptr<Client>> clients;
    std::vector<std::unique_ptr<Service>> services;
    std::vector<std::unique_ptr<Appointment>> appointments;

    void loadStateFromDatabase();
    void saveStateToDatabase();
    void loadEmployees();
    void loadClients();
    void loadServices();
    void loadAppointments();

    void processDayEnd();
    void paySalariesAndRent(const std::string &monthYear, const std::string &lastDayOfMonth);
    bool isWorkDay(const std::string &date) const;
    bool isWorkTime(const std::string &date, const std::string &time) const;

public:
    BeautySalonSimulator();
    ~BeautySalonSimulator();

    void newSimulation(const std::string &startDate);
    void loadSimulation();
    void saveSimulation();
    void clearAllData();

    void nextDay();
    void nextMonth();
    void autoMode();

    void addEmployee(Employee *emp);
    void editEmployee(int id, int field, const std::string &value);
    void fireEmployee(int id);
    void listEmployees();
    std::string getEmployeeSpecialization(int id);

    void addClient(Client *client);
    void editClient(int id, const std::string &field, const std::string &value);
    void deleteClient(int id);
    void searchClientById(int id);
    void searchClientByName(const std::string &name);
    void searchClientByPhone(const std::string &phone);
    void showClientHistory(int id);
    void listClients();

    void addService(Service *service);
    void editService(int id, double newCost);
    void listServices();
    void listServicesBySpecialization(const std::string &spec);
    bool serviceExists(int id);
    bool serviceMatchesSpecialization(int serviceId, const std::string &spec);

    void addAppointment(Appointment *app);
    bool createAppointment(int clientId, int empId, int serviceId, const std::string &date, const std::string &time, double &outPrice);
    void cancelAppointment(int id);
    void listAppointments(const std::string &date = "");
    void showSchedule(const std::string &date);
    void showTomorrowSchedule();
    void showWeekSchedule();

    void showBalance();
    void showRevenueAndExpenses(const std::string &fromDate, const std::string &toDate);
    void showMasterLoad();
    void showMasterLoadDetailed();
    void showTopServices();
    void showClientBonuses();
    void showClientRetention();
    void showStatistics();

    std::string getCurrentDate() const;
    double getCashBalance() const;
    int getTodayAppointmentsCount() const;
    bool isInitialized() const;
};

#endif
