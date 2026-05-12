#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <functional>

class DatabaseManager
{
private:
    PGconn *connection;
    static DatabaseManager *instance;

    DatabaseManager();
    void checkResult(PGresult *res, const std::string &context);

public:
    static DatabaseManager *getInstance();
    static void destroyInstance();

    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    ~DatabaseManager();

    void execute(const std::string &sql);
    void executeWithCallback(const std::string &sql,
                             const std::function<void(const std::vector<std::string> &)> &callback);

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();

    int getLastInsertId();

    PGconn *getConnection() const;

    void initializeSchema();
    void initializeSaveSchema();
    bool hasSaveData();
    void copyToSave();
    void copyFromSave();
    void clearMainTables();
};

#endif
