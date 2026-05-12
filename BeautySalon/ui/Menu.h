#ifndef MENU_H
#define MENU_H

#include <string>
#include "../simulator/BeautySalonSimulator.h"

class Menu
{
private:
    BeautySalonSimulator simulator;
    bool running;
    bool inMainMenu;

    int getValidatedInt(const std::string &prompt, int min, int max);
    double getValidatedDouble(const std::string &prompt, double min, double max);
    std::string getValidatedString(const std::string &prompt);
    std::string getValidatedDate(const std::string &prompt);
    std::string getValidatedTime(const std::string &prompt);

    bool hasEmployees() const;
    bool hasClients() const;
    bool hasServices() const;
    bool hasAppointments() const;
    bool hasScheduledAppointments() const;

    void displayMainMenu();
    void displaySimulationMenu();

    void createNewSimulation();
    void loadSimulation();

    void addEmployeeMenu();
    void editEmployeeMenu();
    void fireEmployeeMenu();
    void listEmployeesMenu();

    void addClientMenu();
    void editClientMenu();
    void deleteClientMenu();
    void searchClientMenu();
    void clientHistoryMenu();

    void addServiceMenu();
    void editServiceMenu();
    void listServicesMenu();

    void createAppointmentMenu();
    void cancelAppointmentMenu();
    void showScheduleMenu();
    void showTomorrowScheduleMenu();
    void showWeekScheduleMenu();

    void showFinanceMenu();
    void showRevenueAndExpensesMenu();
    void showMasterLoadMenu();
    void showTopServicesMenu();
    void showClientBonusesMenu();
    void showClientRetentionMenu();
    void saveSimulationMenu();

public:
    Menu();
    ~Menu();
    void run();
};

#endif
