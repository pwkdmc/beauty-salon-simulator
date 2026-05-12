#include "Client.h"
#include "../database/DatabaseManager.h"
#include "../exceptions/DatabaseException.h"
#include "../exceptions/NotFoundException.h"
#include <iostream>
#include <sstream>

Client::Client()
    : id(-1), fullName(""), phone(""), firstVisitDate(""), totalSpent(0.0),
      visitsCount(0), bonusPoints(0), isDeleted(false)
{
    std::cout << "\033[1;31mСоздан клиент\033[0m\n";
}

Client::Client(int id, const std::string &fullName, const std::string &phone,
               const std::string &firstVisitDate, double totalSpent, int visitsCount,
               int bonusPoints, bool isDeleted)
    : id(id), fullName(fullName), phone(phone), firstVisitDate(firstVisitDate),
      totalSpent(totalSpent), visitsCount(visitsCount), bonusPoints(bonusPoints),
      isDeleted(isDeleted)
{
    std::cout << "\033[1;31mСоздан клиент " << fullName << "\033[0m\n";
}

Client::~Client()
{
    std::cout << "\033[1;31mУдален клиент " << fullName << "\033[0m\n";
}

int Client::getId() const { return id; }
std::string Client::getFullName() const { return fullName; }
std::string Client::getPhone() const { return phone; }
std::string Client::getFirstVisitDate() const { return firstVisitDate; }
double Client::getTotalSpent() const { return totalSpent; }
int Client::getVisitsCount() const { return visitsCount; }
int Client::getBonusPoints() const { return bonusPoints; }
bool Client::getIsDeleted() const { return isDeleted; }

void Client::setFullName(const std::string &name) { fullName = name; }
void Client::setPhone(const std::string &ph) { phone = ph; }
void Client::setIsDeleted(bool del) { isDeleted = del; }

void Client::addVisit(double amount)
{
    visitsCount++;
    totalSpent += amount;
    int earned = static_cast<int>(amount * 0.05);
    bonusPoints += earned;
}

void Client::deductBonuses(int points)
{
    if (points > bonusPoints)
        points = bonusPoints;
    bonusPoints -= points;
}

double Client::getLoyaltyDiscount() const
{
    if (totalSpent >= 50000)
        return 0.15;
    if (totalSpent >= 20000)
        return 0.10;
    if (totalSpent >= 5000)
        return 0.05;
    return 0.0;
}

void Client::saveToDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "INSERT INTO clients (full_name, phone, first_visit_date, total_spent, "
        << "visits_count, bonus_points, is_deleted) VALUES ('"
        << fullName << "', '" << phone << "', "
        << (firstVisitDate.empty() ? "NULL" : "'" + firstVisitDate + "'") << ", "
        << totalSpent << ", " << visitsCount << ", " << bonusPoints << ", "
        << (isDeleted ? 1 : 0) << ") RETURNING id";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (!row.empty()) id = std::stoi(row[0]); });
}

void Client::updateInDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE clients SET full_name='" << fullName << "', phone='" << phone
        << "', first_visit_date=" << (firstVisitDate.empty() ? "NULL" : "'" + firstVisitDate + "'")
        << ", total_spent=" << totalSpent << ", visits_count=" << visitsCount
        << ", bonus_points=" << bonusPoints << ", is_deleted=" << (isDeleted ? 1 : 0)
        << " WHERE id=" << id;
    db->execute(oss.str());
}

void Client::softDeleteFromDatabase()
{
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "UPDATE clients SET is_deleted=1 WHERE id=" << id;
    db->execute(oss.str());
    isDeleted = true;
}

std::vector<Client> Client::loadAllFromDatabase()
{
    std::vector<Client> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    db->executeWithCallback(
        "SELECT id, full_name, phone, first_visit_date, total_spent, visits_count, bonus_points, is_deleted FROM clients WHERE is_deleted=0",
        [&](const std::vector<std::string> &row)
        {
            if (row.size() >= 8)
            {
                list.emplace_back(
                    std::stoi(row[0]), row[1], row[2],
                    row[3].empty() ? "" : row[3],
                    std::stod(row[4]), std::stoi(row[5]),
                    std::stoi(row[6]), row[7] == "1");
            }
        });
    return list;
}

Client Client::loadFromDatabase(int clientId)
{
    DatabaseManager *db = DatabaseManager::getInstance();
    Client c;
    bool found = false;
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, first_visit_date, total_spent, visits_count, bonus_points, is_deleted "
        << "FROM clients WHERE id=" << clientId;
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            c = Client(std::stoi(row[0]), row[1], row[2],
                       row[3].empty() ? "" : row[3],
                       std::stod(row[4]), std::stoi(row[5]),
                       std::stoi(row[6]), row[7] == "1");
            found = true;
        } });
    if (!found)
    {
        throw NotFoundException("Клиент с ID=" + std::to_string(clientId) + " не найден");
    }
    return c;
}

std::vector<Client> Client::searchByName(const std::string &name)
{
    std::vector<Client> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, first_visit_date, total_spent, visits_count, bonus_points, is_deleted "
        << "FROM clients WHERE is_deleted=0 AND full_name ILIKE '%" << name << "%'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            list.emplace_back(
                std::stoi(row[0]), row[1], row[2],
                row[3].empty() ? "" : row[3],
                std::stod(row[4]), std::stoi(row[5]),
                std::stoi(row[6]), row[7] == "1"
            );
        } });
    return list;
}

std::vector<Client> Client::searchByPhone(const std::string &phone)
{
    std::vector<Client> list;
    DatabaseManager *db = DatabaseManager::getInstance();
    std::ostringstream oss;
    oss << "SELECT id, full_name, phone, first_visit_date, total_spent, visits_count, bonus_points, is_deleted "
        << "FROM clients WHERE is_deleted=0 AND phone ILIKE '%" << phone << "%'";
    db->executeWithCallback(oss.str(), [&](const std::vector<std::string> &row)
                            {
        if (row.size() >= 8) {
            list.emplace_back(
                std::stoi(row[0]), row[1], row[2],
                row[3].empty() ? "" : row[3],
                std::stod(row[4]), std::stoi(row[5]),
                std::stoi(row[6]), row[7] == "1"
            );
        } });
    return list;
}
