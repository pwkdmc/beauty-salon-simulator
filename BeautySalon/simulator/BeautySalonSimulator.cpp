#include "BeautySalonSimulator.h"
#include "../database/DatabaseManager.h"
#include "../factory/EmployeeFactory.h"
#include "../utils/DateUtils.h"
#include "../exceptions/DateInPastException.h"
#include "../exceptions/ForbiddenActionException.h"
#include "../exceptions/NotFoundException.h"
#include "../exceptions/InvalidInputException.h"
#include "../exceptions/NoFreeSlotException.h"
#include "../exceptions/InsufficientFundsException.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <thread>
#include <chrono>
#include <conio.h>

BeautySalonSimulator::BeautySalonSimulator()
    : currentDate(""), cashBalance(50000.0), totalRevenue(0.0), totalExpenses(0.0),
      lastMonthSalaryPaid(""), initialized(false)
{
    std::cout << "\033[1;31mСоздан BeautySalonSimulator\033[0m\n";
}

BeautySalonSimulator::~BeautySalonSimulator()
{
    std::cout << "\033[1;31mУдален BeautySalonSimulator\033[0m\n";
}

void BeautySalonSimulator::newSimulation(const std::string &startDate)
{
    if (!DateUtils::isValidDateFormat(startDate))
    {
        throw InvalidInputException("Некорректный формат даты, ожидается ДД.ММ.ГГГГ");
    }
    if (!DateUtils::isDateNotEarlierThanToday(startDate))
    {
        throw DateInPastException("Дата начала симуляции не может быть раньше сегодняшней");
    }
    DatabaseManager *db = DatabaseManager::getInstance();
    db->initializeSchema();
    db->initializeSaveSchema();
    db->clearMainTables();
    currentDate = startDate;
    cashBalance = 50000.0;
    totalRevenue = 0.0;
    totalExpenses = 0.0;
    lastMonthSalaryPaid = "";
    initialized = true;

    std::ostringstream oss;
    oss << "INSERT INTO salon_state (\"current_date\", cash_balance, last_month_salary_paid, total_revenue, total_expenses) VALUES ('"
        << currentDate << "', " << cashBalance << ", NULL, " << totalRevenue << ", " << totalExpenses << ")";
    db->execute(oss.str());

    employees.clear();
    clients.clear();
    services.clear();
    appointments.clear();

    std::cout << "Новая симуляция начата с даты " << currentDate << "\n";
}

void BeautySalonSimulator::loadSimulation()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    db->initializeSchema();
    db->initializeSaveSchema();
    if (!db->hasSaveData())
    {
        throw NotFoundException("Сохранение не найдено");
    }
    db->copyFromSave();
    loadStateFromDatabase();
    loadEmployees();
    loadClients();
    loadServices();
    loadAppointments();
    initialized = true;
    std::cout << "Симуляция загружена из базы данных\n";
}

void BeautySalonSimulator::saveSimulation()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    db->initializeSaveSchema();
    saveStateToDatabase();
    db->copyToSave();
    std::cout << "Состояние симуляции сохранено\n";
}

void BeautySalonSimulator::clearAllData()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    db->clearMainTables();
    employees.clear();
    clients.clear();
    services.clear();
    appointments.clear();
    currentDate = "";
    cashBalance = 50000.0;
    totalRevenue = 0.0;
    totalExpenses = 0.0;
    lastMonthSalaryPaid = "";
    initialized = false;
    std::cout << "Данные симуляции очищены\n";
}

void BeautySalonSimulator::loadStateFromDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback(
        "SELECT \"current_date\", cash_balance, last_month_salary_paid, total_revenue, total_expenses FROM salon_state LIMIT 1",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 5)
            {
                currentDate = row[0];
                cashBalance = std::stod(row[1]);
                lastMonthSalaryPaid = row[2].empty() ? "" : row[2];
                totalRevenue = std::stod(row[3]);
                totalExpenses = std::stod(row[4]);
                found = true;
            }
        });
    if (!found)
    {
        throw NotFoundException("Состояние симуляции не найдено в базе данных");
    }
}

void BeautySalonSimulator::saveStateToDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    db->execute("DELETE FROM salon_state");
    std::ostringstream oss;
    oss << "INSERT INTO salon_state (\"current_date\", cash_balance, last_month_salary_paid, total_revenue, total_expenses) VALUES ('"
        << currentDate << "', " << cashBalance << ", "
        << (lastMonthSalaryPaid.empty() ? "NULL" : "'" + lastMonthSalaryPaid + "'") << ", "
        << totalRevenue << ", " << totalExpenses << ")";
    db->execute(oss.str());
}

void BeautySalonSimulator::loadEmployees()
{
    employees.clear();
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, full_name, specialization, experience, rating, salary_base, salary_percent, status, is_deleted FROM employees WHERE is_deleted=0",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 9)
            {
                Employee *emp = EmployeeFactory::createEmployeeFromDatabase(
                    row[2], std::stoi(row[0]), row[1], std::stoi(row[3]),
                    std::stod(row[4]), std::stod(row[5]), std::stod(row[6]),
                    row[7], row[8] == "1");
                employees.emplace_back(emp);
            }
        });
}

void BeautySalonSimulator::loadClients()
{
    clients.clear();
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, full_name, phone, first_visit_date, total_spent, visits_count, bonus_points, is_deleted FROM clients WHERE is_deleted=0",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 8)
            {
                Client *c = new Client(
                    std::stoi(row[0]), row[1], row[2],
                    row[3].empty() ? "" : row[3],
                    std::stod(row[4]), std::stoi(row[5]),
                    std::stoi(row[6]), row[7] == "1");
                clients.emplace_back(c);
            }
        });
}

void BeautySalonSimulator::loadServices()
{
    services.clear();
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, name, duration, cost, specialization, materials_cost FROM services",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 6)
            {
                Service *s = new Service(
                    std::stoi(row[0]), row[1], std::stoi(row[2]),
                    std::stod(row[3]), row[4], std::stod(row[5]));
                services.emplace_back(s);
            }
        });
}

void BeautySalonSimulator::loadAppointments()
{
    appointments.clear();
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking FROM appointments",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 8)
            {
                Appointment *a = new Appointment(
                    std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                    std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
                appointments.emplace_back(a);
            }
        });
}

bool BeautySalonSimulator::isWorkDay(const std::string &date) const
{
    return !DateUtils::isSunday(date);
}

bool BeautySalonSimulator::isWorkTime(const std::string &date, const std::string &time) const
{
    int minutes = std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(3, 2));
    bool saturday = DateUtils::dayOfWeek(date) == 6;
    int open = 10 * 60;
    int close = saturday ? 18 * 60 : 20 * 60;
    return minutes >= open && minutes < close;
}

void BeautySalonSimulator::nextDay()
{
    if (!initialized)
    {
        throw ForbiddenActionException("Симуляция не инициализирована");
    }
    DatabaseManager *db = DatabaseManager::getInstance();

    std::string oldDate = currentDate;
    double oldCashBalance = cashBalance;
    double oldTotalRevenue = totalRevenue;
    double oldTotalExpenses = totalExpenses;
    std::string oldLastMonthSalaryPaid = lastMonthSalaryPaid;

    db->beginTransaction();
    try
    {
        processDayEnd();
        std::string previousDate = currentDate;
        currentDate = DateUtils::nextDay(currentDate);
        if (DateUtils::isSunday(currentDate))
        {
            currentDate = DateUtils::nextDay(currentDate);
            std::cout << "Воскресенье — выходной, пропущено\n";
        }
        std::string prevMonth = previousDate.substr(3, 7);
        std::string currMonth = currentDate.substr(3, 7);
        if (prevMonth != currMonth)
        {
            std::string lastDayPrevMonth = DateUtils::lastDayOfMonth(prevMonth);
            paySalariesAndRent(prevMonth, lastDayPrevMonth);
        }
        saveStateToDatabase();
        db->commitTransaction();
        std::cout << "Перешли на " << currentDate << "\n";
    }
    catch (const std::exception &e)
    {
        db->rollbackTransaction();
        currentDate = oldDate;
        cashBalance = oldCashBalance;
        totalRevenue = oldTotalRevenue;
        totalExpenses = oldTotalExpenses;
        lastMonthSalaryPaid = oldLastMonthSalaryPaid;
        throw;
    }
}

void BeautySalonSimulator::processDayEnd()
{
    if (!isWorkDay(currentDate))
        return;

    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT a.id, a.client_id, a.employee_id, a.service_id, a.price_at_booking "
        "FROM appointments a "
        "WHERE a.appointment_date='" +
            currentDate + "' AND a.status='записана'",
        [&](const std::vector<std::string> &row)
        {
            int appId = std::stoi(row[0]);
            int clientId = std::stoi(row[1]);
            int empId = std::stoi(row[2]);
            int serviceId = std::stoi(row[3]);
            double price = std::stod(row[4]);

            std::string clientName, empName, empSpec, serviceName;
            double materialsCost = 0.0;

            db->executeWithCallback(
                "SELECT full_name, total_spent, visits_count, bonus_points FROM clients WHERE id=" + std::to_string(clientId) + " AND is_deleted=0",
                [&](const std::vector<std::string> &crow)
                {
                    if (crow.size() >= 4)
                    {
                        clientName = crow[0];
                    }
                });

            db->executeWithCallback(
                "SELECT full_name, specialization FROM employees WHERE id=" + std::to_string(empId),
                [&](const std::vector<std::string> &erow)
                {
                    if (erow.size() >= 2)
                    {
                        empName = erow[0];
                        empSpec = erow[1];
                    }
                });

            db->executeWithCallback(
                "SELECT name, materials_cost FROM services WHERE id=" + std::to_string(serviceId),
                [&](const std::vector<std::string> &srow)
                {
                    if (srow.size() >= 2)
                    {
                        serviceName = srow[0];
                        materialsCost = std::stod(srow[1]);
                    }
                });

            double totalSpent = 0.0;
            int visitsCount = 0;
            int bonusPoints = 0;
            db->executeWithCallback(
                "SELECT total_spent, visits_count, bonus_points FROM clients WHERE id=" + std::to_string(clientId) + " AND is_deleted=0",
                [&](const std::vector<std::string> &crow)
                {
                    if (crow.size() >= 3)
                    {
                        totalSpent = std::stod(crow[0]);
                        visitsCount = std::stoi(crow[1]);
                        bonusPoints = std::stoi(crow[2]);
                    }
                });

            double loyaltyDiscount = 0.0;
            if (totalSpent >= 50000.0)
                loyaltyDiscount = 0.15;
            else if (totalSpent >= 20000.0)
                loyaltyDiscount = 0.10;
            else if (totalSpent >= 5000.0)
                loyaltyDiscount = 0.05;

            double afterDiscount = price * (1.0 - loyaltyDiscount);

            int maxBonusDiscount = static_cast<int>(price * 0.5);
            int useBonus = std::min(bonusPoints, maxBonusDiscount);
            double finalPrice = afterDiscount - useBonus;
            if (finalPrice < 0)
                finalPrice = 0;

            int earnedBonuses = static_cast<int>(finalPrice * 0.05);
            int newBonusPoints = bonusPoints - useBonus + earnedBonuses;

            cashBalance += finalPrice;
            totalRevenue += finalPrice;

            double newTotalSpent = totalSpent + finalPrice;
            int newVisitsCount = visitsCount + 1;
            db->execute(
                "UPDATE clients SET total_spent=" + std::to_string(newTotalSpent) +
                ", visits_count=" + std::to_string(newVisitsCount) +
                ", bonus_points=" + std::to_string(newBonusPoints) +
                " WHERE id=" + std::to_string(clientId));

            totalExpenses += materialsCost;
            cashBalance -= materialsCost;

            db->execute(
                "UPDATE appointments SET status='выполнена', price_at_booking=" + std::to_string(finalPrice) +
                " WHERE id=" + std::to_string(appId));

            std::cout << "== Выполнена запись ==\n";
            std::cout << "Клиент: " << clientName << "\n";
            std::cout << "Сотрудник: " << empName << " (" << empSpec << ")\n";
            std::cout << "Услуга: " << serviceName << "\n";
            std::cout << "Базовая стоимость: " << price << " руб\n";
            std::cout << "Скидка лояльности: " << (loyaltyDiscount * 100) << "% (потрачено: " << totalSpent << " руб)\n";
            std::cout << "После скидки: " << afterDiscount << " руб\n";
            std::cout << "Использовано бонусов: " << useBonus << " руб\n";
            std::cout << "Итоговая стоимость: " << finalPrice << " руб\n";
            std::cout << "Начислено бонусов: " << earnedBonuses << "\n";
            std::cout << "Списаны материалы: " << materialsCost << " руб\n";
        });
}

void BeautySalonSimulator::paySalariesAndRent(const std::string &monthYear, const std::string &lastDayOfMonth)
{
    DatabaseManager *db = DatabaseManager::getInstance();

    double monthExpenses = 0.0;

    for (auto &emp : employees)
    {
        if (emp->getStatus() != "работает")
            continue;

        double servicesSum = 0.0;
        std::ostringstream oss;
        oss << "SELECT COALESCE(SUM(price_at_booking),0) FROM appointments "
            << "WHERE employee_id=" << emp->getId()
            << " AND status='выполнена' AND appointment_date LIKE '__." << monthYear << "'";
        db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                                {
            if (!row.empty()) servicesSum = std::stod(row[0]); });

        std::string firstWorkDate;
        db->executeWithCallback(
            "SELECT MIN(appointment_date) FROM appointments "
            "WHERE employee_id=" +
                std::to_string(emp->getId()) + " AND status='выполнена'",
            [&](const std::vector<std::string> &row)
            {
                if (!row.empty())
                    firstWorkDate = row[0];
            });

        double salary = emp->calculateSalary(servicesSum);
        if (!firstWorkDate.empty())
        {
            std::string workMonth = firstWorkDate.substr(3, 7);
            if (workMonth == monthYear)
            {
                int workingDaysInMonth = 0;
                std::string d = std::string("01.") + monthYear;
                std::string lastDayOfMonthCalc = DateUtils::lastDayOfMonth(monthYear);
                while (DateUtils::compareDates(d, lastDayOfMonthCalc) <= 0)
                {
                    if (!DateUtils::isSunday(d))
                        workingDaysInMonth++;
                    d = DateUtils::nextDay(d);
                }
                int workedDays = 0;
                std::ostringstream workedDaysOss;
                workedDaysOss << "SELECT COUNT(DISTINCT appointment_date) FROM appointments "
                              << "WHERE employee_id=" << emp->getId() << " AND status='выполнена' AND appointment_date >= '" << firstWorkDate << "' "
                              << "AND appointment_date <= '" << lastDayOfMonth << "'";
                db->executeWithCallback(workedDaysOss.str(),
                                        [&](const std::vector<std::string> &row)
                                        {
                                            if (!row.empty())
                                                workedDays = std::stoi(row[0]);
                                        });
                if (workingDaysInMonth > 0 && workedDays > 0)
                {
                    double basePart = emp->getSalaryBase() * (double)workedDays / workingDaysInMonth;
                    salary = basePart + (servicesSum * emp->getSalaryPercent() / 100.0);
                }
            }
        }

        if (cashBalance < salary)
        {
            throw InsufficientFundsException("Недостаточно средств в кассе для выплаты зарплаты");
        }
        cashBalance -= salary;
        totalExpenses += salary;
        monthExpenses += salary;

        bool exists = false;
        std::ostringstream checkOss;
        checkOss << "SELECT 1 FROM salary_log WHERE employee_id=" << emp->getId() << " AND month_year='" << monthYear << "'";
        db->executeWithCallback(checkOss.str(), [&](const std::vector<std::string> &)
                                { exists = true; });

        std::ostringstream logOss;
        if (exists)
        {
            logOss << "UPDATE salary_log SET total_services_sum=" << servicesSum
                   << ", salary_calculated=" << salary << ", paid_status=1 "
                   << "WHERE employee_id=" << emp->getId() << " AND month_year='" << monthYear << "'";
        }
        else
        {
            logOss << "INSERT INTO salary_log (employee_id, month_year, total_services_sum, salary_calculated, paid_status) VALUES ("
                   << emp->getId() << ", '" << monthYear << "', " << servicesSum << ", " << salary << ", 1)";
        }
        db->execute(logOss.str());

        std::cout << "Выплачена зарплата: " << emp->getFullName() << " - " << salary << " руб\n";
    }

    double rent = 5000.0;
    if (cashBalance < rent)
    {
        throw InsufficientFundsException("Недостаточно средств в кассе для оплаты аренды");
    }
    cashBalance -= rent;
    totalExpenses += rent;
    monthExpenses += rent;

    std::cout << "Оплачена аренда: " << rent << " руб\n";

    lastMonthSalaryPaid = lastDayOfMonth;
    saveStateToDatabase();

    std::cout << "Выплачены зарплаты и аренда за " << monthYear
              << ", расходы: " << monthExpenses << "\n";
}

void BeautySalonSimulator::nextMonth()
{
    if (!initialized)
    {
        throw ForbiddenActionException("Симуляция не инициализирована");
    }
    std::string targetDate = DateUtils::addMonths(currentDate, 1);
    while (DateUtils::compareDates(currentDate, targetDate) < 0)
    {
        nextDay();
    }
}

void BeautySalonSimulator::autoMode()
{
    if (!initialized)
    {
        throw ForbiddenActionException("Симуляция не инициализирована");
    }
    std::cout << "Автоматический режим запущен (для остановки нажмите 'q')\n";
    while (true)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q')
            {
                std::cout << "Автоматический режим остановлен\n";
                break;
            }
        }
        std::cout << "Текущая дата: " << currentDate << ", касса: " << cashBalance << "\n";
        nextDay();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void BeautySalonSimulator::addEmployee(Employee *emp)
{
    if (emp)
    {
        employees.emplace_back(emp);
    }
}

void BeautySalonSimulator::editEmployee(int id, int field, const std::string &value)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream check;
    check << "SELECT 1 FROM employees WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(check.str(), [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found)
        throw NotFoundException("Сотрудник не найден");

    std::ostringstream upd;
    switch (field)
    {
    case 1:
        upd << "UPDATE employees SET full_name='" << value << "' WHERE id=" << id;
        break;
    case 2:
        upd << "UPDATE employees SET experience=" << value << " WHERE id=" << id;
        break;
    case 3:
        upd << "UPDATE employees SET rating=" << value << " WHERE id=" << id;
        break;
    case 4:
        upd << "UPDATE employees SET salary_base=" << value << " WHERE id=" << id;
        break;
    case 5:
        upd << "UPDATE employees SET salary_percent=" << value << " WHERE id=" << id;
        break;
    case 6:
        upd << "UPDATE employees SET status='" << value << "' WHERE id=" << id;
        break;
    default:
        throw InvalidInputException("Неверное поле для редактирования");
    }
    db->execute(upd.str());
}

void BeautySalonSimulator::fireEmployee(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream check;
    check << "SELECT 1 FROM employees WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(check.str(), [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found)
        throw NotFoundException("Сотрудник не найден");

    std::ostringstream oss;
    oss << "UPDATE employees SET status='уволен', is_deleted=1 WHERE id=" << id;
    db->execute(oss.str());
}

std::string BeautySalonSimulator::getEmployeeSpecialization(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::string spec;
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT specialization FROM employees WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) {
            spec = row[0];
            found = true;
        } });
    if (!found)
        throw NotFoundException("Сотрудник не найден");
    return spec;
}

void BeautySalonSimulator::listEmployees()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT id, full_name, specialization, experience, rating, salary_base, salary_percent, status "
        "FROM employees WHERE is_deleted=0 ORDER BY id",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << "ID=" << row[0] << ": " << row[1] << " - " << row[2]
                      << ", стаж: " << row[3] << " лет"
                      << ", рейтинг: " << row[4]
                      << ", оклад: " << row[5]
                      << ", процент: " << row[6]
                      << ", статус: " << row[7] << "\n";
        });
    if (!hasData)
    {
        std::cout << "Нет сотрудников\n";
    }
}

void BeautySalonSimulator::addClient(Client *client)
{
    if (client)
    {
        clients.emplace_back(client);
    }
}

void BeautySalonSimulator::editClient(int id, const std::string &field, const std::string &value)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream check;
    check << "SELECT 1 FROM clients WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(check.str(), [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found)
        throw NotFoundException("Клиент не найден");

    std::ostringstream upd;
    if (field == "name")
    {
        upd << "UPDATE clients SET full_name='" << value << "' WHERE id=" << id;
    }
    else if (field == "phone")
    {
        upd << "UPDATE clients SET phone='" << value << "' WHERE id=" << id;
    }
    else
    {
        throw InvalidInputException("Неверное поле для редактирования");
    }
    db->execute(upd.str());
}

void BeautySalonSimulator::deleteClient(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream check;
    check << "SELECT 1 FROM clients WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(check.str(), [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found)
        throw NotFoundException("Клиент не найден");

    std::ostringstream oss;
    oss << "UPDATE clients SET is_deleted=1 WHERE id=" << id;
    db->execute(oss.str());
}

void BeautySalonSimulator::searchClientById(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, visits_count, total_spent, bonus_points FROM clients "
        << "WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        std::cout << "ID=" << row[0] << ": " << row[1] << ", " << row[2]
                    << ", визитов: " << row[3]
                    << ", потрачено: " << row[4]
                    << ", бонусы: " << row[5] << "\n";
        found = true; });
    if (!found)
        std::cout << "Клиент не найден\n";
}

void BeautySalonSimulator::searchClientByName(const std::string &name)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, visits_count, total_spent, bonus_points FROM clients WHERE full_name ILIKE '%" << name << "%' AND is_deleted=0";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        std::cout << "ID=" << row[0] << ": " << row[1] << ", " << row[2]
                    << ", визитов: " << row[3]
                    << ", потрачено: " << row[4]
                    << ", бонусы: " << row[5] << "\n";
        found = true; });
    if (!found)
        std::cout << "Клиент не найден\n";
}

void BeautySalonSimulator::searchClientByPhone(const std::string &phone)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, visits_count, total_spent, bonus_points FROM clients WHERE phone ILIKE '%" << phone << "%' AND is_deleted=0";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        std::cout << "ID=" << row[0] << ": " << row[1] << ", " << row[2]
                    << ", визитов: " << row[3]
                    << ", потрачено: " << row[4]
                    << ", бонусы: " << row[5] << "\n";
        found = true; });
    if (!found)
        std::cout << "Клиент не найден\n";
}

void BeautySalonSimulator::showClientHistory(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::string clientName;
    bool clientFound = false;
    std::ostringstream nameOss;
    nameOss << "SELECT full_name FROM clients WHERE id=" << id << " AND is_deleted=0";
    db->executeWithCallback(nameOss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) { clientName = row[0]; clientFound = true; } });
    if (!clientFound)
    {
        std::cout << "Клиент не найден\n";
        return;
    }
    bool found = false;
    std::ostringstream appOss;
    appOss << "SELECT appointment_date, appointment_time, price_at_booking FROM appointments "
           << "WHERE client_id=" << id << " AND status='выполнена' ORDER BY appointment_date, appointment_time";
    db->executeWithCallback(appOss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!found) {
            std::cout << "История посещений клиента " << clientName << ":\n";
            found = true;
        }
        std::cout << "Дата: " << row[0] << ", время: " << row[1] << ", сумма: " << row[2] << "\n"; });
    if (!found)
        std::cout << "Нет посещений\n";
}

void BeautySalonSimulator::listClients()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT id, full_name, phone FROM clients WHERE is_deleted=0 ORDER BY id",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << "ID=" << row[0] << ": " << row[1] << ", " << row[2] << "\n";
        });
    if (!hasData)
        std::cout << "Нет клиентов\n";
}

void BeautySalonSimulator::addService(Service *service)
{
    if (service)
    {
        services.emplace_back(service);
    }
}

void BeautySalonSimulator::editService(int id, double newCost)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    std::ostringstream check;
    check << "SELECT 1 FROM services WHERE id=" << id;
    db->executeWithCallback(check.str(), [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found)
        throw NotFoundException("Услуга не найдена");

    std::ostringstream oss;
    oss << "UPDATE services SET cost=" << newCost << " WHERE id=" << id;
    db->execute(oss.str());
}

void BeautySalonSimulator::listServices()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT id, name, duration, cost, specialization FROM services ORDER BY id",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << "ID=" << row[0] << ": " << row[1]
                      << ", длительность: " << row[2] << " мин"
                      << ", цена: " << row[3]
                      << ", специализация: " << row[4] << "\n";
        });
    if (!hasData)
    {
        std::cout << "Нет услуг\n";
    }
}

void BeautySalonSimulator::listServicesBySpecialization(const std::string &spec)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    std::ostringstream oss;
    oss << "SELECT id, name, duration, cost, specialization FROM services "
        << "WHERE specialization='" << spec << "' ORDER BY id";
    db->executeWithCallback(oss.str(),
                            [&](const std::vector<std::string> &row)
                            {
                                hasData = true;
                                std::cout << "ID=" << row[0] << ": " << row[1]
                                          << ", длительность: " << row[2] << " мин"
                                          << ", цена: " << row[3]
                                          << ", специализация: " << row[4] << "\n";
                            });
    if (!hasData)
    {
        std::cout << "Нет услуг\n";
    }
}

bool BeautySalonSimulator::serviceExists(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool exists = false;
    std::ostringstream oss;
    oss << "SELECT 1 FROM services WHERE id=" << id;
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &)
                            { exists = true; });
    return exists;
}

bool BeautySalonSimulator::serviceMatchesSpecialization(int serviceId, const std::string &spec)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool matches = false;
    std::ostringstream oss;
    oss << "SELECT 1 FROM services WHERE id=" << serviceId << " AND specialization='" << spec << "'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &)
                            { matches = true; });
    return matches;
}

void BeautySalonSimulator::addAppointment(Appointment *app)
{
    if (app)
    {
        appointments.emplace_back(app);
    }
}

bool BeautySalonSimulator::createAppointment(int clientId, int empId, int serviceId,
                                             const std::string &date, const std::string &time,
                                             double &outPrice)
{
    DatabaseManager *db = DatabaseManager::getInstance();

    bool clientExists = false;
    db->executeWithCallback("SELECT 1 FROM clients WHERE id=" + std::to_string(clientId) + " AND is_deleted=0",
                            [&](const std::vector<std::string> &)
                            { clientExists = true; });
    if (!clientExists)
        throw NotFoundException("Клиент не найден");

    bool empExists = false;
    db->executeWithCallback("SELECT 1 FROM employees WHERE id=" + std::to_string(empId) + " AND is_deleted=0 AND status='работает'",
                            [&](const std::vector<std::string> &row)
                            { empExists = true; });
    if (!empExists)
        throw NotFoundException("Сотрудник не найден");

    std::string empSpec;
    db->executeWithCallback("SELECT specialization FROM employees WHERE id=" + std::to_string(empId) + " AND is_deleted=0",
                            [&](const std::vector<std::string> &row)
                            {
                                if (!row.empty())
                                    empSpec = row[0];
                            });
    if (empSpec.empty())
        throw NotFoundException("Сотрудник не найден");

    int duration = 0;
    double cost = 0.0;
    std::string serviceSpec;
    bool serviceFound = false;
    db->executeWithCallback("SELECT duration, cost, specialization FROM services WHERE id=" + std::to_string(serviceId),
                            [&](const std::vector<std::string> &row)
                            {
                                if (row.size() >= 3)
                                {
                                    duration = std::stoi(row[0]);
                                    cost = std::stod(row[1]);
                                    serviceSpec = row[2];
                                    serviceFound = true;
                                }
                            });
    if (!serviceFound)
        throw NotFoundException("Услуга не найдена");
    if (serviceSpec != empSpec)
        throw ForbiddenActionException("Услуга не соответствует специализации мастера");

    if (DateUtils::compareDates(date, currentDate) < 0)
    {
        throw DateInPastException("Дата записи не может быть в прошлом");
    }
    if (DateUtils::isSunday(date))
    {
        throw ForbiddenActionException("Воскресенье - выходной");
    }

    int minutes = std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(3, 2));
    bool saturday = DateUtils::dayOfWeek(date) == 6;
    int open = 10 * 60;
    int close = saturday ? 18 * 60 : 20 * 60;
    if (minutes < open || minutes >= close)
    {
        throw ForbiddenActionException("Время вне рабочего диапазона");
    }
    if (minutes + duration > close)
    {
        throw ForbiddenActionException("Услуга не укладывается в рабочее время");
    }

    int start = minutes;
    int end = start + duration;
    bool overlap = false;
    std::ostringstream overlapOss;
    overlapOss << "SELECT a.appointment_time, s.duration FROM appointments a "
               << "JOIN services s ON a.service_id = s.id "
               << "WHERE a.employee_id=" << empId << " AND a.appointment_date='" << date << "' AND a.status!='отменена'";
    db->executeWithCallback(overlapOss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 2) {
            int aStart = std::stoi(row[0].substr(0, 2)) * 60 + std::stoi(row[0].substr(3, 2));
            int aEnd = aStart + std::stoi(row[1]);
            if (start < aEnd && end > aStart) overlap = true;
        } });
    if (overlap)
        throw NoFreeSlotException("У мастера уже есть запись на это время");

    bool clientBusy = false;
    std::ostringstream clientOss;
    clientOss << "SELECT 1 FROM appointments WHERE client_id=" << clientId
              << " AND appointment_date='" << date << "' AND appointment_time='" << time << "' AND status!='отменена'";
    db->executeWithCallback(clientOss.str(), [&](const std::vector<std::string> &)
                            { clientBusy = true; });
    if (clientBusy)
        throw ForbiddenActionException("Клиент уже записан на это время");

    int maxWorkload = 0;
    db->executeWithCallback(
        "SELECT max_clients_per_day FROM employee_types WHERE employee_id=" + std::to_string(empId),
        [&](const std::vector<std::string> &row)
        {
            maxWorkload = std::stoi(row[0]);
        });
    int workload = 0;
    db->executeWithCallback(
        "SELECT COUNT(*) FROM appointments WHERE employee_id=" + std::to_string(empId) +
            " AND status = 'записана'"
            " AND appointment_date='" +
            date + "'",
        [&](const std::vector<std::string> &row)
        {
            workload = std::stoi(row[0]);
        });
    if (workload >= maxWorkload)
    {
        throw NoFreeSlotException("У мастера уже максимальное количество записей в этот день");
    }

    std::ostringstream insOss;
    insOss << "INSERT INTO appointments (client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking) VALUES ("
           << clientId << ", " << empId << ", " << serviceId << ", '" << date << "', '" << time << "', 'записана', " << cost << ")";
    try
    {
        db->execute(insOss.str());
    }
    catch (const std::exception &e)
    {
        if (std::string(e.what()).find("duplicate key") != std::string::npos)
        {
            std::ostringstream resetSeq;
            resetSeq << "SELECT setval('appointments_id_seq', (SELECT COALESCE(MAX(id), 0) FROM appointments))";
            db->execute(resetSeq.str());
            db->execute(insOss.str());
        }
        else
        {
            throw;
        }
    }

    int newId = 0;
    db->executeWithCallback("SELECT currval('appointments_id_seq')", [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) newId = std::stoi(row[0]); });

    outPrice = cost;
    Appointment *app = new Appointment(newId, clientId, empId, serviceId, date, time, "записана", cost);
    appointments.emplace_back(app);
    return true;
}

void BeautySalonSimulator::cancelAppointment(int id)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::string appDate, appStatus;
    bool found = false;
    std::ostringstream checkOss;
    checkOss << "SELECT appointment_date, status FROM appointments WHERE id=" << id;
    db->executeWithCallback(checkOss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 2) {
            appDate = row[0];
            appStatus = row[1];
            found = true;
        } });
    if (!found)
        throw NotFoundException("Запись не найдена");
    if (appStatus == "выполнена")
        throw ForbiddenActionException("Нельзя отменить выполненную запись");
    if (DateUtils::compareDates(appDate, currentDate) < 0)
        throw ForbiddenActionException("Нельзя отменить запись в прошлом");

    std::ostringstream updOss;
    updOss << "UPDATE appointments SET status='отменена' WHERE id=" << id;
    db->execute(updOss.str());
}

void BeautySalonSimulator::listAppointments(const std::string &date)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    std::ostringstream oss;
    oss << "SELECT a.id, a.appointment_date, a.appointment_time, c.full_name, e.full_name, s.name, a.status "
        << "FROM appointments a "
        << "JOIN clients c ON a.client_id = c.id "
        << "JOIN employees e ON a.employee_id = e.id "
        << "JOIN services s ON a.service_id = s.id ";
    if (!date.empty())
    {
        oss << "WHERE a.appointment_date='" << date << "' ";
    }
    oss << "ORDER BY a.appointment_date, a.appointment_time";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        hasData = true;
        std::cout << "ID=" << row[0] << ": " << row[1] << " " << row[2]
                  << ", клиент: " << row[3]
                  << ", мастер: " << row[4]
                  << ", услуга: " << row[5]
                  << ", статус: " << row[6] << "\n"; });
    if (!hasData)
        std::cout << "Нет записей\n";
}

void BeautySalonSimulator::showSchedule(const std::string &date)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    std::ostringstream oss;
    oss << "SELECT a.appointment_time, c.full_name, e.full_name, e.specialization, s.name, a.status "
        << "FROM appointments a "
        << "JOIN clients c ON a.client_id = c.id "
        << "JOIN employees e ON a.employee_id = e.id "
        << "JOIN services s ON a.service_id = s.id "
        << "WHERE a.appointment_date = '" << date << "' "
        << "ORDER BY a.appointment_time";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        hasData = true;
        std::cout << row[0] << " - " << row[1]
                  << ", мастер: " << row[2] << " (" << row[3] << ")"
                  << ", услуга: " << row[4]
                  << ", статус: " << row[5] << "\n"; });
    if (!hasData)
    {
        std::cout << "Нет записей\n";
    }
}

void BeautySalonSimulator::showTomorrowSchedule()
{
    std::string tomorrow = DateUtils::nextDay(currentDate);
    showSchedule(tomorrow);
}

void BeautySalonSimulator::showWeekSchedule()
{
    std::string d = currentDate;
    for (int i = 0; i < 7; ++i)
    {
        std::cout << "--- " << d << " ---\n";
        showSchedule(d);
        d = DateUtils::nextDay(d);
    }
}

void BeautySalonSimulator::showBalance()
{
    std::cout << "Текущий баланс кассы: " << cashBalance << " руб\n";
}

void BeautySalonSimulator::showRevenueAndExpenses(const std::string &fromDate, const std::string &toDate)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    double periodRevenue = 0.0;
    double periodExpenses = 0.0;

    std::cout << "== Услуги за период " << fromDate << " - " << toDate << " ==\n";
    bool hasServices = false;
    std::ostringstream query;
    query << "SELECT a.appointment_date, a.appointment_time, c.full_name, e.full_name, e.specialization, "
          << "s.name, a.price_at_booking, s.materials_cost "
          << "FROM appointments a "
          << "JOIN clients c ON a.client_id = c.id "
          << "JOIN employees e ON a.employee_id = e.id "
          << "JOIN services s ON a.service_id = s.id "
          << "WHERE TO_DATE(a.appointment_date, 'DD.MM.YYYY') >= TO_DATE('" << fromDate << "', 'DD.MM.YYYY') "
          << "AND TO_DATE(a.appointment_date, 'DD.MM.YYYY') <= TO_DATE('" << toDate << "', 'DD.MM.YYYY') "
          << "AND a.status = 'выполнена' "
          << "ORDER BY a.appointment_date, a.appointment_time";
    db->executeWithCallback(query.str(),
                            [&](const std::vector<std::string> &row)
                            {
                                hasServices = true;
                                double price = std::stod(row[6]);
                                double materials = std::stod(row[7]);
                                periodRevenue += price;
                                periodExpenses += materials;
                                std::cout << row[0] << " " << row[1] << " | Клиент: " << row[2]
                                          << " | Мастер: " << row[3] << " (" << row[4] << ")"
                                          << " | Услуга: " << row[5]
                                          << " | Стоимость: " << price
                                          << " | Материалы: " << materials << "\n";
                            });
    if (!hasServices)
    {
        std::cout << "Услуг не оказано\n";
    }

    std::cout << "== Зарплаты за период ==\n";
    bool hasSalaries = false;
    db->executeWithCallback(
        "SELECT e.full_name, s.month_year, s.salary_calculated FROM salary_log s "
        "JOIN employees e ON s.employee_id = e.id "
        "ORDER BY s.month_year, e.full_name",
        [&](const std::vector<std::string> &row)
        {
            std::string lastDay = DateUtils::lastDayOfMonth(row[1]);
            if (DateUtils::compareDates(lastDay, fromDate) >= 0 &&
                DateUtils::compareDates(lastDay, toDate) <= 0 &&
                DateUtils::compareDates(lastDay, currentDate) <= 0)
            {
                hasSalaries = true;
                double salary = std::stod(row[2]);
                periodExpenses += salary;
                std::cout << row[1] << " | " << row[0] << " - " << salary << " руб\n";
            }
        });
    if (!hasSalaries)
    {
        std::cout << "Зарплат не выплачивалось\n";
    }

    std::cout << "== Аренда за период ==\n";
    double rentTotal = 0.0;
    int rentMonths = 0;
    std::string d = fromDate;
    while (DateUtils::compareDates(d, toDate) <= 0)
    {
        if (DateUtils::isLastDayOfMonth(d) && DateUtils::compareDates(d, currentDate) <= 0)
        {
            rentMonths++;
            rentTotal += 5000.0;
        }
        d = DateUtils::nextDay(d);
    }
    periodExpenses += rentTotal;
    if (rentMonths > 0)
    {
        std::cout << "Аренда: " << rentTotal << " руб\n";
    }
    else
    {
        std::cout << "Аренда не списывалась\n";
    }

    std::cout << "== Итого ==\n";
    std::cout << "Выручка: " << periodRevenue << " руб\n";
    std::cout << "Расходы: " << periodExpenses << " руб\n";
    std::cout << "Прибыль: " << (periodRevenue - periodExpenses) << " руб\n";
}

void BeautySalonSimulator::showMasterLoad()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT e.full_name, e.specialization, COALESCE(a.cnt,0), t.max_clients_per_day "
        "FROM employees e "
        "JOIN employee_types t ON e.id = t.employee_id "
        "LEFT JOIN (SELECT employee_id, COUNT(*) as cnt FROM appointments WHERE status='записана' GROUP BY employee_id) a "
        "ON e.id = a.employee_id "
        "WHERE e.status = 'работает' AND e.is_deleted =0",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            int count = std::stoi(row[2]);
            int maxClients = std::stoi(row[3]);
            double load = maxClients > 0 ? (double)count / maxClients * 100.0 : 0.0;
            std::cout << row[0] << " (" << row[1] << "): "
                      << count << "/" << maxClients
                      << " (" << std::fixed << std::setprecision(1) << load << "%)\n";
        });
    if (!hasData)
    {
        std::cout << "Нет сотрудников\n";
    }
}

void BeautySalonSimulator::showMasterLoadDetailed()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;

    db->executeWithCallback(
        "SELECT DISTINCT e.id, e.full_name, e.specialization, t.max_clients_per_day "
        "FROM employees e "
        "JOIN employee_types t ON e.id = t.employee_id "
        "WHERE e.status = 'работает' AND e.is_deleted = 0 "
        "AND EXISTS (SELECT 1 FROM appointments a WHERE a.employee_id = e.id AND a.status = 'записана') "
        "ORDER BY e.id",
        [&](const std::vector<std::string> &empRow)
        {
            hasData = true;
            int empId = std::stoi(empRow[0]);
            std::string empName = empRow[1];
            std::string empSpec = empRow[2];
            int maxClients = std::stoi(empRow[3]);

            std::cout << empName << " (" << empSpec << "):\n";

            db->executeWithCallback(
                "SELECT appointment_date, COUNT(*) as cnt "
                "FROM appointments "
                "WHERE employee_id = " +
                    std::to_string(empId) + " AND status = 'записана' "
                                            "GROUP BY appointment_date "
                                            "ORDER BY appointment_date",
                [&](const std::vector<std::string> &dateRow)
                {
                    std::string appDate = dateRow[0];
                    int count = std::stoi(dateRow[1]);
                    double load = maxClients > 0 ? (double)count / maxClients * 100.0 : 0.0;
                    std::cout << "  " << appDate << ": " << count << "/" << maxClients
                              << " (" << std::fixed << std::setprecision(1) << load << "%)\n";
                });
        });

    if (!hasData)
    {
        std::cout << "Нет информации\n";
    }
}

void BeautySalonSimulator::showTopServices()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT s.name, COUNT(*) as cnt "
        "FROM appointments a "
        "JOIN services s ON a.service_id = s.id "
        "WHERE a.status = 'выполнена' "
        "GROUP BY s.name "
        "ORDER BY cnt DESC "
        "LIMIT 5",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << row[0] << ": " << row[1] << " раз\n";
        });
    if (!hasData)
    {
        std::cout << "Нет выполненных услуг\n";
    }
}

void BeautySalonSimulator::showClientBonuses()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasData = false;
    db->executeWithCallback(
        "SELECT full_name, bonus_points FROM clients WHERE is_deleted=0 ORDER BY id",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << row[0] << ": " << row[1] << " баллов\n";
        });
    if (!hasData)
    {
        std::cout << "Нет клиентов\n";
    }
}

void BeautySalonSimulator::showClientRetention()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    int totalClients = 0;
    int returningClients = 0;

    db->executeWithCallback(
        "SELECT COUNT(*) FROM clients WHERE is_deleted=0",
        [&](const std::vector<std::string> &row)
        {
            if (!row.empty())
                totalClients = std::stoi(row[0]);
        });

    if (totalClients == 0)
    {
        std::cout << "Нет клиентов\n";
        return;
    }

    db->executeWithCallback(
        "SELECT COUNT(*) FROM clients WHERE is_deleted=0 AND visits_count > 1",
        [&](const std::vector<std::string> &row)
        {
            if (!row.empty())
                returningClients = std::stoi(row[0]);
        });

    double retention = totalClients > 0 ? (double)returningClients / totalClients * 100.0 : 0.0;
    std::cout << "Удержание клиентов:\n";
    std::cout << "Всего клиентов: " << totalClients << "\n";
    std::cout << "Вернувшихся повторно: " << returningClients << "\n";
    std::cout << "Процент удержания: " << std::fixed << std::setprecision(1) << retention << "%\n";
}

void BeautySalonSimulator::showStatistics()
{
    showBalance();
    showMasterLoad();
}

std::string BeautySalonSimulator::getCurrentDate() const
{
    return currentDate;
}

double BeautySalonSimulator::getCashBalance() const
{
    return cashBalance;
}

int BeautySalonSimulator::getTodayAppointmentsCount() const
{
    int count = 0;
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT COUNT(*) FROM appointments WHERE appointment_date='" + currentDate + "' AND status='записана'",
        [&](const std::vector<std::string> &row)
        {
            if (!row.empty())
                count = std::stoi(row[0]);
        });
    return count;
}

bool BeautySalonSimulator::isInitialized() const
{
    return initialized;
}
