#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>

class Client
{
private:
    int id;
    std::string fullName;
    std::string phone;
    std::string firstVisitDate;
    double totalSpent;
    int visitsCount;
    int bonusPoints;
    bool isDeleted;

public:
    Client();
    Client(int id, const std::string &fullName, const std::string &phone,
           const std::string &firstVisitDate, double totalSpent, int visitsCount,
           int bonusPoints, bool isDeleted);
    ~Client();

    int getId() const;
    std::string getFullName() const;
    std::string getPhone() const;
    std::string getFirstVisitDate() const;
    double getTotalSpent() const;
    int getVisitsCount() const;
    int getBonusPoints() const;
    bool getIsDeleted() const;

    void setFullName(const std::string &name);
    void setPhone(const std::string &ph);
    void setIsDeleted(bool del);

    void addVisit(double amount);
    void deductBonuses(int points);
    double getLoyaltyDiscount() const;

    void saveToDatabase();
    void updateInDatabase();
    void softDeleteFromDatabase();

    static std::vector<Client> loadAllFromDatabase();
    static Client loadFromDatabase(int clientId);
    static std::vector<Client> searchByName(const std::string &name);
    static std::vector<Client> searchByPhone(const std::string &phone);
};

#endif
