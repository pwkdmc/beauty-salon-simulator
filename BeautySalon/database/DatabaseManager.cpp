#include "DatabaseManager.h"
#include "../exceptions/DatabaseException.h"
#include <iostream>

DatabaseManager *DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager() : connection(nullptr)
{
    const char *conninfo = "dbname= user= password= host= port=";
    connection = PQconnectdb(conninfo);
    if (PQstatus(connection) != CONNECTION_OK)
    {
        std::string msg = PQerrorMessage(connection);
        PQfinish(connection);
        connection = nullptr;
        throw DatabaseException("Ошибка подключения: " + msg);
    }
    std::cout << "\033[1;31mСоздан DatabaseManager и подключен к PostgreSQL\033[0m\n";
}

DatabaseManager::~DatabaseManager()
{
    if (connection)
    {
        PQfinish(connection);
        connection = nullptr;
    }
    std::cout << "\033[1;31mУдален DatabaseManager, соединение закрыто\033[0m\n";
}

DatabaseManager *DatabaseManager::getInstance()
{
    if (!instance)
    {
        instance = new DatabaseManager();
    }
    return instance;
}

void DatabaseManager::destroyInstance()
{
    delete instance;
    instance = nullptr;
}

void DatabaseManager::checkResult(PGresult *res, const std::string &context)
{
    if (!res)
    {
        std::string err = PQerrorMessage(connection);
        throw DatabaseException(context + ": " + err);
    }
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
    {
        std::string err = PQresultErrorMessage(res);
        PQclear(res);
        throw DatabaseException(context + ": " + err);
    }
    PQclear(res);
}

void DatabaseManager::execute(const std::string &sql)
{
    PGresult *res = PQexec(connection, sql.c_str());
    checkResult(res, "Ошибка выполнения запроса");
}

void DatabaseManager::executeWithCallback(const std::string &sql,
                                          const std::function<void(const std::vector<std::string> &)> &callback)
{
    PGresult *res = PQexec(connection, sql.c_str());
    if (!res)
    {
        std::string err = PQerrorMessage(connection);
        throw DatabaseException("Ошибка выполнения SELECT: " + err);
    }
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::string err = PQresultErrorMessage(res);
        PQclear(res);
        throw DatabaseException("Ошибка выполнения SELECT: " + err);
    }
    int rows = PQntuples(res);
    int cols = PQnfields(res);
    for (int i = 0; i < rows; ++i)
    {
        std::vector<std::string> row;
        for (int j = 0; j < cols; ++j)
        {
            char *val = PQgetvalue(res, i, j);
            row.push_back(val ? val : "");
        }
        callback(row);
    }
    PQclear(res);
}

void DatabaseManager::beginTransaction()
{
    execute("BEGIN");
}

void DatabaseManager::commitTransaction()
{
    execute("COMMIT");
}

void DatabaseManager::rollbackTransaction()
{
    execute("ROLLBACK");
}

int DatabaseManager::getLastInsertId()
{
    int id = -1;
    executeWithCallback("SELECT lastval()", [&](const std::vector<std::string> &row)
                        {
        if (!row.empty()) id = std::stoi(row[0]); });
    return id;
}

PGconn *DatabaseManager::getConnection() const
{
    return connection;
}

void DatabaseManager::initializeSchema()
{
    beginTransaction();
    try
    {
        execute(
            "CREATE TABLE IF NOT EXISTS salon_state ("
            "id SERIAL PRIMARY KEY,"
            "\"current_date\" VARCHAR(10) NOT NULL,"
            "cash_balance DOUBLE PRECISION NOT NULL DEFAULT 50000,"
            "last_month_salary_paid VARCHAR(10) DEFAULT NULL,"
            "total_revenue DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "total_expenses DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS employees ("
            "id SERIAL PRIMARY KEY,"
            "full_name VARCHAR(255) NOT NULL,"
            "specialization VARCHAR(50) NOT NULL,"
            "experience INT NOT NULL DEFAULT 0,"
            "rating DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_base DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_percent DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "status VARCHAR(50) NOT NULL DEFAULT 'работает',"
            "is_deleted INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS employee_types ("
            "id SERIAL PRIMARY KEY,"
            "employee_id INT REFERENCES employees(id) ON DELETE CASCADE,"
            "type VARCHAR(50) NOT NULL,"
            "max_clients_per_day INT NOT NULL DEFAULT 0,"
            "avg_service_time INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS clients ("
            "id SERIAL PRIMARY KEY,"
            "full_name VARCHAR(255) NOT NULL,"
            "phone VARCHAR(50) NOT NULL,"
            "first_visit_date VARCHAR(10) DEFAULT NULL,"
            "total_spent DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "visits_count INT NOT NULL DEFAULT 0,"
            "bonus_points INT NOT NULL DEFAULT 0,"
            "is_deleted INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS services ("
            "id SERIAL PRIMARY KEY,"
            "name VARCHAR(255) NOT NULL,"
            "duration INT NOT NULL DEFAULT 0,"
            "cost DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "specialization VARCHAR(50) NOT NULL,"
            "materials_cost DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS appointments ("
            "id SERIAL PRIMARY KEY,"
            "client_id INT REFERENCES clients(id) ON DELETE CASCADE,"
            "employee_id INT REFERENCES employees(id) ON DELETE CASCADE,"
            "service_id INT REFERENCES services(id) ON DELETE CASCADE,"
            "appointment_date VARCHAR(10) NOT NULL,"
            "appointment_time VARCHAR(5) NOT NULL,"
            "status VARCHAR(50) NOT NULL DEFAULT 'записана',"
            "price_at_booking DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS salary_log ("
            "id SERIAL PRIMARY KEY,"
            "employee_id INT REFERENCES employees(id) ON DELETE CASCADE,"
            "month_year VARCHAR(7) NOT NULL,"
            "total_services_sum DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_calculated DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "paid_status INT NOT NULL DEFAULT 0,"
            "UNIQUE (employee_id, month_year))");
        execute(
            "CREATE INDEX IF NOT EXISTS idx_appointments_date ON appointments(appointment_date)");
        execute(
            "CREATE INDEX IF NOT EXISTS idx_appointments_employee ON appointments(employee_id)");
        execute(
            "CREATE INDEX IF NOT EXISTS idx_clients_name ON clients(full_name)");
        execute(
            "CREATE INDEX IF NOT EXISTS idx_employees_status ON employees(status)");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}

void DatabaseManager::initializeSaveSchema()
{
    beginTransaction();
    try
    {
        execute(
            "CREATE TABLE IF NOT EXISTS save_salon_state ("
            "id SERIAL PRIMARY KEY,"
            "\"current_date\" VARCHAR(10) NOT NULL,"
            "cash_balance DOUBLE PRECISION NOT NULL DEFAULT 50000,"
            "last_month_salary_paid VARCHAR(10) DEFAULT NULL,"
            "total_revenue DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "total_expenses DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_employees ("
            "id SERIAL PRIMARY KEY,"
            "full_name VARCHAR(255) NOT NULL,"
            "specialization VARCHAR(50) NOT NULL,"
            "experience INT NOT NULL DEFAULT 0,"
            "rating DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_base DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_percent DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "status VARCHAR(50) NOT NULL DEFAULT 'работает',"
            "is_deleted INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_employee_types ("
            "id SERIAL PRIMARY KEY,"
            "employee_id INT NOT NULL,"
            "type VARCHAR(50) NOT NULL,"
            "max_clients_per_day INT NOT NULL DEFAULT 0,"
            "avg_service_time INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_clients ("
            "id SERIAL PRIMARY KEY,"
            "full_name VARCHAR(255) NOT NULL,"
            "phone VARCHAR(50) NOT NULL,"
            "first_visit_date VARCHAR(10) DEFAULT NULL,"
            "total_spent DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "visits_count INT NOT NULL DEFAULT 0,"
            "bonus_points INT NOT NULL DEFAULT 0,"
            "is_deleted INT NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_services ("
            "id SERIAL PRIMARY KEY,"
            "name VARCHAR(255) NOT NULL,"
            "duration INT NOT NULL DEFAULT 0,"
            "cost DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "specialization VARCHAR(50) NOT NULL,"
            "materials_cost DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_appointments ("
            "id SERIAL PRIMARY KEY,"
            "client_id INT NOT NULL,"
            "employee_id INT NOT NULL,"
            "service_id INT NOT NULL,"
            "appointment_date VARCHAR(10) NOT NULL,"
            "appointment_time VARCHAR(5) NOT NULL,"
            "status VARCHAR(50) NOT NULL DEFAULT 'записана',"
            "price_at_booking DOUBLE PRECISION NOT NULL DEFAULT 0)");
        execute(
            "CREATE TABLE IF NOT EXISTS save_salary_log ("
            "id SERIAL PRIMARY KEY,"
            "employee_id INT NOT NULL,"
            "month_year VARCHAR(7) NOT NULL,"
            "total_services_sum DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "salary_calculated DOUBLE PRECISION NOT NULL DEFAULT 0,"
            "paid_status INT NOT NULL DEFAULT 0)");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}

bool DatabaseManager::hasSaveData()
{
    bool found = false;
    executeWithCallback("SELECT 1 FROM save_salon_state LIMIT 1", [&](const std::vector<std::string> &)
                        { found = true; });
    return found;
}

void DatabaseManager::copyToSave()
{
    beginTransaction();
    try
    {
        execute("TRUNCATE TABLE save_salon_state, save_employees, save_employee_types, save_clients, save_services, save_appointments, save_salary_log RESTART IDENTITY CASCADE");
        execute("INSERT INTO save_salon_state SELECT * FROM salon_state");
        execute("INSERT INTO save_employees SELECT * FROM employees");
        execute("INSERT INTO save_employee_types SELECT * FROM employee_types");
        execute("INSERT INTO save_clients SELECT * FROM clients");
        execute("INSERT INTO save_services SELECT * FROM services");
        execute("INSERT INTO save_appointments SELECT * FROM appointments");
        execute("INSERT INTO save_salary_log SELECT * FROM salary_log");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}

void DatabaseManager::copyFromSave()
{
    beginTransaction();
    try
    {
        execute("TRUNCATE TABLE salon_state, employees, employee_types, clients, services, appointments, salary_log RESTART IDENTITY CASCADE");
        execute("INSERT INTO salon_state SELECT * FROM save_salon_state");
        execute("INSERT INTO employees SELECT * FROM save_employees");
        execute("INSERT INTO employee_types SELECT * FROM save_employee_types");
        execute("INSERT INTO clients SELECT * FROM save_clients");
        execute("INSERT INTO services SELECT * FROM save_services");
        execute("INSERT INTO appointments SELECT * FROM save_appointments");
        execute("INSERT INTO salary_log SELECT * FROM save_salary_log");
        execute("SELECT setval('salary_log_id_seq', COALESCE((SELECT MAX(id) FROM salary_log), 0) + 1, false)");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}

void DatabaseManager::clearMainTables()
{
    beginTransaction();
    try
    {
        execute("TRUNCATE TABLE salon_state, employees, employee_types, clients, services, appointments, salary_log RESTART IDENTITY CASCADE");
        execute("SELECT setval('salary_log_id_seq', 1, false)");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}
