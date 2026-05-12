#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <vector>

class Appointment
{
private:
    int id;
    int clientId;
    int employeeId;
    int serviceId;
    std::string appointmentDate;
    std::string appointmentTime;
    std::string status;
    double priceAtBooking;

public:
    Appointment();
    Appointment(int id, int clientId, int employeeId, int serviceId,
                const std::string &appointmentDate, const std::string &appointmentTime,
                const std::string &status, double priceAtBooking);
    ~Appointment();

    int getId() const;
    int getClientId() const;
    int getEmployeeId() const;
    int getServiceId() const;
    std::string getAppointmentDate() const;
    std::string getAppointmentTime() const;
    std::string getStatus() const;
    double getPriceAtBooking() const;

    void setStatus(const std::string &st);

    void saveToDatabase();
    void updateInDatabase();

    static std::vector<Appointment> loadAllFromDatabase();
    static std::vector<Appointment> loadByDate(const std::string &date);
    static std::vector<Appointment> loadByEmployeeAndDate(int empId, const std::string &date);
    static std::vector<Appointment> loadByClient(int clientId);
    static Appointment loadFromDatabase(int appId);
    static bool hasOverlap(int employeeId, const std::string &date,
                           const std::string &time, int duration);
    static bool clientHasAppointmentAt(int clientId, const std::string &date,
                                       const std::string &time);
};

#endif
