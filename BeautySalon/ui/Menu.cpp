#include "Menu.h"
#include "../utils/DateUtils.h"
#include "../factory/EmployeeFactory.h"
#include "../models/Hairdresser.h"
#include "../models/Manicurist.h"
#include "../models/Cosmetologist.h"
#include "../models/MakeupArtist.h"
#include "../models/Client.h"
#include "../models/Service.h"
#include "../models/Appointment.h"
#include "../database/DatabaseManager.h"
#include "../exceptions/InvalidInputException.h"
#include "../exceptions/NotFoundException.h"
#include "../exceptions/ForbiddenActionException.h"
#include "../exceptions/DateInPastException.h"
#include "../exceptions/NoFreeSlotException.h"
#include "../exceptions/InsufficientFundsException.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>

Menu::Menu() : running(true), inMainMenu(true)
{
    std::cout << "\033[1;31mСоздан Menu\033[0m\n";
}

Menu::~Menu()
{
    std::cout << "\033[1;31mУдален Menu\033[0m\n";
}

int Menu::getValidatedInt(const std::string &prompt, int min, int max)
{
    int value;
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidInputException("Неверный ввод: нужно целое число");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (value < min || value > max)
    {
        throw InvalidInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    return value;
}

double Menu::getValidatedDouble(const std::string &prompt, double min, double max)
{
    double value;
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidInputException("Неверный ввод: нужно число");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (value < min || value > max)
    {
        throw InvalidInputException("Значение должно быть в заданном диапазоне");
    }
    return value;
}

std::string Menu::getValidatedString(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty())
    {
        throw InvalidInputException("Поле не может быть пустым");
    }
    return value;
}

std::string Menu::getValidatedDate(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (!DateUtils::isValidDateFormat(value))
    {
        throw InvalidInputException("Некорректный формат даты, ожидается ДД.ММ.ГГГГ");
    }
    return value;
}

std::string Menu::getValidatedTime(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.size() != 5 || value[2] != ':' ||
        value[0] < '0' || value[0] > '2' || value[1] < '0' || value[1] > '9' ||
        value[3] < '0' || value[3] > '5' || value[4] < '0' || value[4] > '9')
    {
        throw InvalidInputException("Некорректный формат времени, ожидается ЧЧ:ММ");
    }
    int h = std::stoi(value.substr(0, 2));
    int m = std::stoi(value.substr(3, 2));
    if (h < 0 || h > 23 || m < 0 || m > 59)
    {
        throw InvalidInputException("Некорректное время");
    }
    return value;
}

bool Menu::hasEmployees() const
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM employees WHERE is_deleted=0 LIMIT 1", [&](const std::vector<std::string> &)
                            { found = true; });
    return found;
}

bool Menu::hasClients() const
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM clients WHERE is_deleted=0 LIMIT 1", [&](const std::vector<std::string> &)
                            { found = true; });
    return found;
}

bool Menu::hasServices() const
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM services LIMIT 1", [&](const std::vector<std::string> &)
                            { found = true; });
    return found;
}

bool Menu::hasAppointments() const
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM appointments LIMIT 1", [&](const std::vector<std::string> &)
                            { found = true; });
    return found;
}

bool Menu::hasScheduledAppointments() const
{
    DatabaseManager *db = DatabaseManager::getInstance();
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM appointments WHERE status='записана' LIMIT 1", [&](const std::vector<std::string> &)
                            { found = true; });
    return found;
}

void Menu::displayMainMenu()
{
    std::cout << "=== СИМУЛЯТОР САЛОНА КРАСОТЫ ===\n";
    std::cout << "1. Начать новую симуляцию\n";
    std::cout << "2. Загрузить сохранение\n";
    std::cout << "0. Выход\n";
}

void Menu::displaySimulationMenu()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::cout << "=== УПРАВЛЕНИЕ САЛОНОМ ===\n";
    std::cout << "Текущая дата: " << simulator.getCurrentDate()
              << ", Баланс: " << simulator.getCashBalance()
              << ", Записей сегодня: " << simulator.getTodayAppointmentsCount() << "\n";
    std::cout << "1. Перейти на следующий день\n";
    std::cout << "2. Перейти на следующий месяц\n";
    std::cout << "3. Автоматический режим\n";
    std::cout << "4. Добавить сотрудника\n";
    std::cout << "5. Редактировать сотрудника\n";
    std::cout << "6. Уволить сотрудника\n";
    std::cout << "7. Список сотрудников\n";
    std::cout << "8. Добавить клиента\n";
    std::cout << "9. Редактировать клиента\n";
    std::cout << "10. Удалить клиента\n";
    std::cout << "11. Поиск клиента\n";
    std::cout << "12. История посещений клиента\n";
    std::cout << "13. Добавить услугу\n";
    std::cout << "14. Редактировать услугу\n";
    std::cout << "15. Список услуг\n";
    std::cout << "16. Создать запись\n";
    std::cout << "17. Отменить запись\n";
    std::cout << "18. Расписание на сегодня\n";
    std::cout << "19. Расписание на завтра\n";
    std::cout << "20. Расписание на неделю\n";
    std::cout << "21. Баланс кассы\n";
    std::cout << "22. Выручка и расходы\n";
    std::cout << "23. Загруженность мастеров\n";
    std::cout << "24. Топ услуг\n";
    std::cout << "25. Бонусы клиентов\n";
    std::cout << "26. Удержание клиентов\n";
    std::cout << "27. Сохранить состояние\n";
    std::cout << "0. Выход\n";
}

void Menu::run()
{
    while (running)
    {
        try
        {
            if (inMainMenu)
            {
                displayMainMenu();
                int choice = getValidatedInt("Выберите действие:\n", 0, 2);
                switch (choice)
                {
                case 1:
                    createNewSimulation();
                    break;
                case 2:
                    loadSimulation();
                    break;
                case 0:
                    running = false;
                    break;
                }
            }
            else
            {
                displaySimulationMenu();
                int choice = getValidatedInt("Выберите действие:\n", 0, 27);
                switch (choice)
                {
                case 1:
                    simulator.nextDay();
                    break;
                case 2:
                    simulator.nextMonth();
                    break;
                case 3:
                    simulator.autoMode();
                    break;
                case 4:
                    addEmployeeMenu();
                    break;
                case 5:
                    editEmployeeMenu();
                    break;
                case 6:
                    fireEmployeeMenu();
                    break;
                case 7:
                    listEmployeesMenu();
                    break;
                case 8:
                    addClientMenu();
                    break;
                case 9:
                    editClientMenu();
                    break;
                case 10:
                    deleteClientMenu();
                    break;
                case 11:
                    searchClientMenu();
                    break;
                case 12:
                    clientHistoryMenu();
                    break;
                case 13:
                    addServiceMenu();
                    break;
                case 14:
                    editServiceMenu();
                    break;
                case 15:
                    listServicesMenu();
                    break;
                case 16:
                    createAppointmentMenu();
                    break;
                case 17:
                    cancelAppointmentMenu();
                    break;
                case 18:
                    showScheduleMenu();
                    break;
                case 19:
                    showTomorrowScheduleMenu();
                    break;
                case 20:
                    showWeekScheduleMenu();
                    break;
                case 21:
                    showFinanceMenu();
                    break;
                case 22:
                    showRevenueAndExpensesMenu();
                    break;
                case 23:
                    showMasterLoadMenu();
                    break;
                case 24:
                    showTopServicesMenu();
                    break;
                case 25:
                    showClientBonusesMenu();
                    break;
                case 26:
                    showClientRetentionMenu();
                    break;
                case 27:
                    saveSimulationMenu();
                    break;
                case 0:
                    simulator.clearAllData();
                    inMainMenu = true;
                    break;
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

void Menu::createNewSimulation()
{
    std::cout << "== Начало новой симуляции ==\n";
    std::string date;
    while (true)
    {
        try
        {
            date = getValidatedDate("Введите стартовую дату (ДД.ММ.ГГГГ):\n");
            simulator.newSimulation(date);
            inMainMenu = false;
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

void Menu::loadSimulation()
{
    std::cout << "== Загрузка сохранения ==\n";
    try
    {
        simulator.loadSimulation();
        inMainMenu = false;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }
}

void Menu::addEmployeeMenu()
{
    std::cout << "== Добавление сотрудника ==\n";
    std::cout << "Тип сотрудника:\n";
    std::cout << "1. Парикмахер\n";
    std::cout << "2. Мастер маникюра\n";
    std::cout << "3. Косметолог\n";
    std::cout << "4. Визажист\n";
    int type = 0;
    while (true)
    {
        try
        {
            type = getValidatedInt("Выберите тип:\n", 1, 4);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string typeStr;
    switch (type)
    {
    case 1:
        typeStr = "hairdresser";
        break;
    case 2:
        typeStr = "manicurist";
        break;
    case 3:
        typeStr = "cosmetologist";
        break;
    case 4:
        typeStr = "makeup_artist";
        break;
    }
    std::string name;
    while (true)
    {
        try
        {
            name = getValidatedString("Введите ФИО:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    int exp = 0;
    while (true)
    {
        try
        {
            exp = getValidatedInt("Введите стаж (0-50):\n", 0, 50);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double rating = 0.0;
    while (true)
    {
        try
        {
            rating = getValidatedDouble("Введите рейтинг (0.0-5.0):\n", 0.0, 5.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double base = 0.0;
    while (true)
    {
        try
        {
            base = getValidatedDouble("Введите базовый оклад:\n", 0.0, 1000000.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double percent = 0.0;
    while (true)
    {
        try
        {
            percent = getValidatedDouble("Введите процент от услуг (0-50):\n", 0.0, 50.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    Employee *emp = EmployeeFactory::createEmployee(typeStr, name, exp, rating, base, percent);
    if (emp)
    {
        emp->saveToDatabase();
        std::cout << "Сотрудник добавлен с ID=" << emp->getId() << "\n";
        simulator.addEmployee(emp);
    }
}

void Menu::editEmployeeMenu()
{
    std::cout << "== Редактирование сотрудника ==\n";
    if (!hasEmployees())
    {
        throw NotFoundException("Нет сотрудников для редактирования");
    }
    simulator.listEmployees();
    int id = getValidatedInt("Введите ID сотрудника:\n", 1, 1000000);
    std::cout << "1. ФИО\n2. Стаж\n3. Рейтинг\n4. Оклад\n5. Процент\n6. Статус\n";
    int field = getValidatedInt("Выберите поле для редактирования (1-6):\n", 1, 6);
    std::string value;
    switch (field)
    {
    case 1:
        value = getValidatedString("Введите новое ФИО:\n");
        break;
    case 2:
        value = std::to_string(getValidatedInt("Введите новый стаж (0-50):\n", 0, 50));
        break;
    case 3:
        value = std::to_string(getValidatedDouble("Введите новый рейтинг (0.0-5.0):\n", 0.0, 5.0));
        break;
    case 4:
        value = std::to_string(getValidatedDouble("Введите новый оклад:\n", 0.0, 1000000.0));
        break;
    case 5:
        value = std::to_string(getValidatedDouble("Введите новый процент (0-50):\n", 0.0, 50.0));
        break;
    case 6:
        value = getValidatedString("Введите новый статус (работает/в отпуске/уволен):\n");
        break;
    }
    simulator.editEmployee(id, field, value);
    std::cout << "Сотрудник обновлен\n";
}

void Menu::fireEmployeeMenu()
{
    std::cout << "== Увольнение сотрудника ==\n";
    if (!hasEmployees())
    {
        throw NotFoundException("Нет сотрудников для увольнения");
    }
    simulator.listEmployees();
    int id = getValidatedInt("Введите ID сотрудника:\n", 1, 1000000);
    simulator.fireEmployee(id);
    std::cout << "Сотрудник уволен\n";
}

void Menu::listEmployeesMenu()
{
    std::cout << "== Список сотрудников ==\n";
    simulator.listEmployees();
}

void Menu::addClientMenu()
{
    std::cout << "== Добавление клиента ==\n";
    std::string name;
    while (true)
    {
        try
        {
            name = getValidatedString("Введите ФИО клиента:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string phone;
    while (true)
    {
        try
        {
            phone = getValidatedString("Введите телефон:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    Client *c = new Client();
    c->setFullName(name);
    c->setPhone(phone);
    c->saveToDatabase();
    simulator.addClient(c);
    std::cout << "Клиент добавлен с ID=" << c->getId() << "\n";
}

void Menu::editClientMenu()
{
    std::cout << "== Редактирование клиента ==\n";
    if (!hasClients())
    {
        throw NotFoundException("Нет клиентов для редактирования");
    }
    simulator.listClients();
    int id = getValidatedInt("Введите ID клиента:\n", 1, 1000000);
    std::string newName = getValidatedString("Введите новое ФИО:\n");
    std::string newPhone = getValidatedString("Введите новый телефон:\n");
    simulator.editClient(id, "name", newName);
    simulator.editClient(id, "phone", newPhone);
    std::cout << "Клиент обновлен\n";
}

void Menu::deleteClientMenu()
{
    std::cout << "== Удаление клиента ==\n";
    if (!hasClients())
    {
        throw NotFoundException("Нет клиентов для удаления");
    }
    simulator.listClients();
    int id = getValidatedInt("Введите ID клиента:\n", 1, 1000000);
    simulator.deleteClient(id);
    std::cout << "Клиент удален\n";
}

void Menu::searchClientMenu()
{
    std::cout << "== Поиск клиента ==\n";
    if (!hasClients())
    {
        throw NotFoundException("Нет клиентов для поиска");
    }
    std::cout << "1. По ID\n2. По ФИО\n3. По телефону\n";
    int choice = getValidatedInt("Выберите:\n", 1, 3);
    if (choice == 1)
    {
        int id = getValidatedInt("Введите ID клиента:\n", 1, 1000000);
        simulator.searchClientById(id);
    }
    else if (choice == 2)
    {
        std::string name = getValidatedString("Введите ФИО:\n");
        simulator.searchClientByName(name);
    }
    else if (choice == 3)
    {
        std::string phone = getValidatedString("Введите телефон:\n");
        simulator.searchClientByPhone(phone);
    }
}

void Menu::clientHistoryMenu()
{
    std::cout << "== История посещений клиента ==\n";
    if (!hasClients())
    {
        throw NotFoundException("Нет клиентов");
    }
    simulator.listClients();
    int id = getValidatedInt("Введите ID клиента:\n", 1, 1000000);
    simulator.showClientHistory(id);
}

void Menu::addServiceMenu()
{
    std::cout << "== Добавление услуги ==\n";
    std::string name;
    while (true)
    {
        try
        {
            name = getValidatedString("Введите название услуги:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::cout << "Специализация:\n";
    std::cout << "1. Парикмахер\n";
    std::cout << "2. Маникюр\n";
    std::cout << "3. Косметолог\n";
    std::cout << "4. Визажист\n";
    int specChoice = 0;
    while (true)
    {
        try
        {
            specChoice = getValidatedInt("Выберите:\n", 1, 4);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string spec;
    int minDur = 0, maxDur = 0;
    switch (specChoice)
    {
    case 1:
        spec = "парикмахер";
        minDur = 30;
        maxDur = 90;
        break;
    case 2:
        spec = "маникюр";
        minDur = 45;
        maxDur = 120;
        break;
    case 3:
        spec = "косметолог";
        minDur = 15;
        maxDur = 180;
        break;
    case 4:
        spec = "визажист";
        minDur = 15;
        maxDur = 180;
        break;
    }
    int duration = 0;
    while (true)
    {
        try
        {
            duration = getValidatedInt("Введите длительность (мин, " + std::to_string(minDur) + "-" + std::to_string(maxDur) + "):\n", minDur, maxDur);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double cost = 0;
    while (true)
    {
        try
        {
            cost = getValidatedDouble("Введите стоимость:\n", 0.0, 100000.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double materials = 0;
    while (true)
    {
        try
        {
            materials = getValidatedDouble("Введите стоимость материалов:\n", 0.0, 100000.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    Service *s = new Service(0, name, duration, cost, spec, materials);
    s->saveToDatabase();
    simulator.addService(s);
    std::cout << "Услуга добавлена с ID=" << s->getId() << "\n";
}

void Menu::editServiceMenu()
{
    std::cout << "== Редактирование услуги ==\n";
    if (!hasServices())
    {
        throw NotFoundException("Нет услуг для редактирования");
    }
    simulator.listServices();
    int id = getValidatedInt("Введите ID услуги:\n", 1, 1000000);
    double newCost = getValidatedDouble("Введите новую стоимость:\n", 0.0, 100000.0);
    simulator.editService(id, newCost);
    std::cout << "Услуга обновлена\n";
}

void Menu::listServicesMenu()
{
    std::cout << "== Список услуг ==\n";
    simulator.listServices();
}

void Menu::createAppointmentMenu()
{
    std::cout << "== Создание записи ==\n";
    if (!hasClients())
    {
        throw NotFoundException("Нет клиентов для создания записи");
    }
    if (!hasEmployees())
    {
        throw NotFoundException("Нет сотрудников для создания записи");
    }
    if (!hasServices())
    {
        throw NotFoundException("Нет услуг для создания записи");
    }
    simulator.listClients();
    int clientId = getValidatedInt("Введите ID клиента:\n", 1, 1000000);
    simulator.listEmployees();
    int empId = getValidatedInt("Введите ID сотрудника:\n", 1, 1000000);
    std::string empSpec = simulator.getEmployeeSpecialization(empId);
    std::cout << "Доступные услуги (" << empSpec << "):\n";
    DatabaseManager *db = DatabaseManager::getInstance();
    bool hasServicesForSpec = false;
    std::ostringstream oss;
    oss << "SELECT COUNT(*) FROM services WHERE specialization='" << empSpec << "'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) hasServicesForSpec = (std::stoi(row[0]) > 0); });
    if (!hasServicesForSpec)
    {
        std::cerr << "Нет услуг для специализации " << empSpec << "\n";
        return;
    }
    simulator.listServicesBySpecialization(empSpec);
    int serviceId = 0;
    while (true)
    {
        try
        {
            serviceId = getValidatedInt("Введите ID услуги:\n", 1, 1000000);
            if (!simulator.serviceExists(serviceId))
            {
                throw InvalidInputException("Услуга с таким ID не существует, введите корректный ID");
            }
            if (!simulator.serviceMatchesSpecialization(serviceId, empSpec))
            {
                throw InvalidInputException("Услуга не соответствует специализации мастера, выберите другую услугу");
            }
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string date;
    while (true)
    {
        try
        {
            date = getValidatedDate("Введите дату записи (ДД.ММ.ГГГГ):\n");
            if (DateUtils::compareDates(date, simulator.getCurrentDate()) < 0)
            {
                throw InvalidInputException("Нельзя записываться на дату раньше текущей, выберите другую дату");
            }
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string time;
    while (true)
    {
        try
        {
            time = getValidatedTime("Введите время (ЧЧ:ММ):\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double price = 0.0;
    simulator.createAppointment(clientId, empId, serviceId, date, time, price);
    std::cout << "Запись создана\n";
}

void Menu::cancelAppointmentMenu()
{
    std::cout << "== Отмена записи ==\n";
    if (!hasScheduledAppointments())
    {
        throw NotFoundException("Нет записей для отмены");
    }
    simulator.listAppointments();
    int id = getValidatedInt("Введите ID записи:\n", 1, 1000000);
    simulator.cancelAppointment(id);
    std::cout << "Запись отменена\n";
}

void Menu::showScheduleMenu()
{
    std::cout << "== Расписание на сегодня ==\n";
    if (!hasAppointments())
    {
        throw NotFoundException("Нет записей");
    }
    simulator.showSchedule(simulator.getCurrentDate());
}

void Menu::showTomorrowScheduleMenu()
{
    std::cout << "== Расписание на завтра ==\n";
    if (!hasAppointments())
    {
        throw NotFoundException("Нет записей");
    }
    simulator.showTomorrowSchedule();
}

void Menu::showWeekScheduleMenu()
{
    std::cout << "== Расписание на неделю ==\n";
    if (!hasAppointments())
    {
        throw NotFoundException("Нет записей");
    }
    simulator.showWeekSchedule();
}

void Menu::showFinanceMenu()
{
    std::cout << "== Баланс кассы ==\n";
    simulator.showBalance();
}

void Menu::showRevenueAndExpensesMenu()
{
    std::cout << "== Выручка и расходы ==\n";
    std::string fromDate = getValidatedDate("Введите начальную дату (ДД.ММ.ГГГГ):\n");
    std::string toDate = getValidatedDate("Введите конечную дату (ДД.ММ.ГГГГ):\n");
    if (DateUtils::compareDates(fromDate, toDate) > 0)
    {
        throw InvalidInputException("Начальная дата не может быть позже конечной");
    }
    simulator.showRevenueAndExpenses(fromDate, toDate);
}

void Menu::showMasterLoadMenu()
{
    std::cout << "== Загруженность мастеров ==\n";
    simulator.showMasterLoadDetailed();
}

void Menu::showTopServicesMenu()
{
    std::cout << "== Топ услуг ==\n";
    simulator.showTopServices();
}

void Menu::showClientBonusesMenu()
{
    std::cout << "== Бонусы клиентов ==\n";
    simulator.showClientBonuses();
}

void Menu::showClientRetentionMenu()
{
    std::cout << "== Удержание клиентов ==\n";
    simulator.showClientRetention();
}

void Menu::saveSimulationMenu()
{
    std::cout << "== Сохранение состояния ==\n";
    simulator.saveSimulation();
}