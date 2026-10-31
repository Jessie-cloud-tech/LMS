#include "datetime.h"
#include <cstdio>
#include <ctime>
#include <stdexcept>
using namespace std;

long long nowSeconds() { return (long long)time(0); }

string secondsToText(long long seconds) {
    time_t t = (time_t)seconds;
    tm* parts = localtime(&t);
    char buffer[32];
    strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M", parts);
    return string(buffer);
}

string nowText() { return secondsToText(nowSeconds()); }

long long toSeconds(string text) {
    int year, month, day, hour, minute;
    if (sscanf(text.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute) != 5) {
        throw runtime_error("Wrong date format. Use YYYY-MM-DD HH:MM  (example: 2026-11-30 14:00)");
    }
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_isdst = -1;
    time_t seconds = mktime(&t);
    // mktime turns impossible dates into real ones (31 Feb becomes 3 Mar), so we check for that
    if (year < 1970 || seconds == -1 || t.tm_mday != day || t.tm_mon != month - 1 || t.tm_hour != hour) {
        throw runtime_error("That date or time does not exist: " + text);
    }
    return (long long)seconds;
}