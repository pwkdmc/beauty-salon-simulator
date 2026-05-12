#include "DateUtils.h"
#include "../exceptions/InvalidInputException.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>

std::string DateUtils::getCurrentDate()
{
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%d.%m.%Y", now);
    return std::string(buf);
}

bool DateUtils::isValidDateFormat(const std::string &date)
{
    if (date.size() != 10)
        return false;
    if (date[2] != '.' || date[5] != '.')
        return false;
    for (int i = 0; i < 10; ++i)
    {
        if (i == 2 || i == 5)
            continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }
    int d = std::stoi(date.substr(0, 2));
    int m = std::stoi(date.substr(3, 2));
    int y = std::stoi(date.substr(6, 4));
    if (m < 1 || m > 12)
        return false;
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    if (d < 1 || d > daysInMonth[m - 1])
        return false;
    return true;
}

bool DateUtils::isDateNotEarlierThanToday(const std::string &date)
{
    return compareDates(date, getCurrentDate()) >= 0;
}

bool DateUtils::isDateNotEarlierThan(const std::string &date, const std::string &reference)
{
    return compareDates(date, reference) >= 0;
}

int DateUtils::compareDates(const std::string &date1, const std::string &date2)
{
    auto [d1, m1, y1] = parseDate(date1);
    auto [d2, m2, y2] = parseDate(date2);
    if (y1 != y2)
        return y1 - y2;
    if (m1 != m2)
        return m1 - m2;
    return d1 - d2;
}

std::string DateUtils::addMonths(const std::string &date, int months)
{
    auto [d, m, y] = parseDate(date);
    int totalMonths = (y * 12 + (m - 1)) + months;
    int newY = totalMonths / 12;
    int newM = (totalMonths % 12) + 1;
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (newY % 4 == 0 && newY % 100 != 0) || (newY % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    int newD = d;
    if (newD > daysInMonth[newM - 1])
        newD = daysInMonth[newM - 1];
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << newD << "."
        << std::setw(2) << std::setfill('0') << newM << "."
        << newY;
    return oss.str();
}

int DateUtils::calculateAge(const std::string &birthDate, const std::string &onDate)
{
    auto [bd, bm, by] = parseDate(birthDate);
    auto [od, om, oy] = parseDate(onDate);
    int age = oy - by;
    if (om < bm || (om == bm && od < bd))
        age--;
    return age;
}

std::tuple<int, int, int> DateUtils::parseDate(const std::string &date)
{
    int d = std::stoi(date.substr(0, 2));
    int m = std::stoi(date.substr(3, 2));
    int y = std::stoi(date.substr(6, 4));
    return {d, m, y};
}

bool DateUtils::isSunday(const std::string &date)
{
    return dayOfWeek(date) == 0;
}

bool DateUtils::isLastDayOfMonth(const std::string &date)
{
    auto [d, m, y] = parseDate(date);
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    return d == daysInMonth[m - 1];
}

std::string DateUtils::lastDayOfMonth(const std::string &monthYear)
{
    int m = std::stoi(monthYear.substr(0, 2));
    int y = std::stoi(monthYear.substr(3, 4));
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    int d = daysInMonth[m - 1];
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << d << "."
        << std::setw(2) << std::setfill('0') << m << "."
        << y;
    return oss.str();
}

std::string DateUtils::nextDay(const std::string &date)
{
    auto [d, m, y] = parseDate(date);
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    d++;
    if (d > daysInMonth[m - 1])
    {
        d = 1;
        m++;
        if (m > 12)
        {
            m = 1;
            y++;
        }
    }
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << d << "."
        << std::setw(2) << std::setfill('0') << m << "."
        << y;
    return oss.str();
}

int DateUtils::dayOfWeek(const std::string &date)
{
    auto [d, m, y] = parseDate(date);
    std::tm tm = {};
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = y - 1900;
    std::mktime(&tm);
    return tm.tm_wday;
}

bool DateUtils::isSameDate(const std::string &date1, const std::string &date2)
{
    return compareDates(date1, date2) == 0;
}

int DateUtils::daysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (leap)
        days[1] = 29;
    if (month < 1 || month > 12)
        return 0;
    return days[month - 1];
}
