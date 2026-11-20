#include "DateUtils.h"

bool isValidCalendarDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    for (int i = 0; i < 10; ++i) {
        if (i != 4 && i != 7 &&
            (date[i] < '0' || date[i] > '9')) {
            return false;
        }
    }

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    if (year == 0 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    int daysPerMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear =
        year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);

    if (leapYear) {
        daysPerMonth[1] = 29;
    }

    return day <= daysPerMonth[month - 1];
}