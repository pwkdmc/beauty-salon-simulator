#include "Appointment.h"
#include "../database/DatabaseManager.h"
#include "../exceptions/NotFoundException.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Appointment::Appointment()
    : id(-1), clientId(-1), employeeId(-1), serviceId(-1),
      appointmentDate(""), appointmentTime(""), status("записана"), priceAtBooking(0.0)
{
    std::cout << "\033[1;31mСоздана запись\033[0m\n";
}

Appointment::Appointment(int id, int clientId, int employeeId, int serviceId,
                         const std::string &appointmentDate, const std::string &appointmentTime,
                         const std::string &status, double priceAtBooking)
    : id(id), clientId(clientId), employeeId(employeeId), serviceId(serviceId),
      appointmentDate(appointmentDate), appointmentTime(appointmentTime),
      status(status), priceAtBooking(priceAtBooking)
{
    std::cout << "\033[1;31mСоздана запись ID=" << id << "\033[0m\n";
}

Appointment::~Appointment()
{
    std::cout << "\033[1;31mУдалена запись ID=" << id << "\033[0m\n";
}

int Appointment::getId() const { return id; }
int Appointment::getClientId() const { return clientId; }
int Appointment::getEmployeeId() const { return employeeId; }
int Appointment::getServiceId() const { return serviceId; }
std::string Appointment::getAppointmentDate() const { return appointmentDate; }
std::string Appointment::getAppointmentTime() const { return appointmentTime; }
std::string Appointment::getStatus() const { return status; }
double Appointment::getPriceAtBooking() const { return priceAtBooking; }

void Appointment::setStatus(const std::string &st) { status = st; }

void Appointment::saveToDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "INSERT INTO appointments (client_id, employee_id, service_id, appointment_date, "
        << "appointment_time, status, price_at_booking) VALUES ("
        << clientId << ", " << employeeId << ", " << serviceId << ", '"
        << appointmentDate << "', '" << appointmentTime << "', '"
        << status << "', " << priceAtBooking << ") RETURNING id";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) id = std::stoi(row[0]); });
}

void Appointment::updateInDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE appointments SET status='" << status << "' WHERE id=" << id;
    db->execute(oss.str());
}

std::vector<Appointment> Appointment::loadAllFromDatabase()
{
    std::vector<Appointment> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking FROM appointments",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 8)
            {
                list.emplace_back(std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                                  std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
            }
        });
    return list;
}

std::vector<Appointment> Appointment::loadByDate(const std::string &date)
{
    std::vector<Appointment> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking "
        << "FROM appointments WHERE appointment_date='" << date << "'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            list.emplace_back(std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                              std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
        } });
    return list;
}

std::vector<Appointment> Appointment::loadByEmployeeAndDate(int empId, const std::string &date)
{
    std::vector<Appointment> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking "
        << "FROM appointments WHERE employee_id=" << empId << " AND appointment_date='" << date << "'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            list.emplace_back(std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                              std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
        } });
    return list;
}

std::vector<Appointment> Appointment::loadByClient(int clientId)
{
    std::vector<Appointment> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking "
        << "FROM appointments WHERE client_id=" << clientId;
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            list.emplace_back(std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                              std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
        } });
    return list;
}

Appointment Appointment::loadFromDatabase(int appId)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    Appointment a;
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, client_id, employee_id, service_id, appointment_date, appointment_time, status, price_at_booking "
        << "FROM appointments WHERE id=" << appId;
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            a = Appointment(std::stoi(row[0]), std::stoi(row[1]), std::stoi(row[2]),
                            std::stoi(row[3]), row[4], row[5], row[6], std::stod(row[7]));
            found = true;
        } });
    if (!found)
    {
        throw NotFoundException("Запись с ID=" + std::to_string(appId) + " не найдена");
    }
    return a;
}

static int timeToMinutes(const std::string &time)
{
    int h = std::stoi(time.substr(0, 2));
    int m = std::stoi(time.substr(3, 2));
    return h * 60 + m;
}

bool Appointment::hasOverlap(int employeeId, const std::string &date,
                             const std::string &time, int duration)
{
    auto list = loadByEmployeeAndDate(employeeId, date);
    int start = timeToMinutes(time);
    int end = start + duration;
    for (const auto &a : list)
    {
        if (a.getStatus() == "отменена")
            continue;
        int existingDuration = 0;
        try
        {
            DatabaseManager *db = DatabaseManager::getInstance();
            std::ostringstream oss;
            oss << "SELECT duration FROM services WHERE id=" << a.getServiceId();
            db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                                    {
                if (!row.empty()) existingDuration = std::stoi(row[0]); });
        }
        catch (...)
        {
            existingDuration = 0;
        }
        int aStart = timeToMinutes(a.getAppointmentTime());
        int aEnd = aStart + existingDuration;
        if (start < aEnd && end > aStart)
        {
            return true;
        }
    }
    return false;
}

bool Appointment::clientHasAppointmentAt(int clientId, const std::string &date,
                                         const std::string &time)
{
    auto list = loadByClient(clientId);
    for (const auto &a : list)
    {
        if (a.getAppointmentDate() == date && a.getAppointmentTime() == time && a.getStatus() != "отменена")
        {
            return true;
        }
    }
    return false;
}
