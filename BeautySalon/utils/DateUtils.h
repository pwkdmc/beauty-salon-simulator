#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <tuple>

namespace DateUtils
{
    std::string getCurrentDate();
    bool isValidDateFormat(const std::string &date);
    bool isDateNotEarlierThanToday(const std::string &date);
    bool isDateNotEarlierThan(const std::string &date, const std::string &reference);
    int compareDates(const std::string &date1, const std::string &date2);
    std::string addMonths(const std::string &date, int months);
    int calculateAge(const std::string &birthDate, const std::string &onDate);
    std::tuple<int, int, int> parseDate(const std::string &date);
    bool isSunday(const std::string &date);
    bool isLastDayOfMonth(const std::string &date);
    std::string lastDayOfMonth(const std::string &monthYear);
    std::string nextDay(const std::string &date);
    int dayOfWeek(const std::string &date);
    bool isSameDate(const std::string &date1, const std::string &date2);
    int daysInMonth(int month, int year);
}

#endif
