#include "datetime.h"
#include <cstdio>
#include <ctime>
#include <stdexcept>
using namespace std;

static bool usingTestTime = false;
static long long testTime = 0;

void setTestTime(long long seconds) {
    usingTestTime = true;
    testTime = seconds;
}

void clearTestTime() { usingTestTime = false; }

long long nowSeconds() {
    if (usingTestTime) return testTime;
    return (long long)time(0);
}

string secondsToText(long long seconds) {
    time_t t = (time_t)seconds;
    tm* parts = localtime(&t);
    char buffer[32];
    strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M", parts);
    return string(buffer);
}

string nowText() { return secondsToText(nowSeconds()); }

long long toSeconds(string text) {
    int day, month, year, hour, minute;

    if (sscanf(text.c_str(), "%d-%d-%d %d:%d",
               &day, &month, &year, &hour, &minute) != 5) {
        throw runtime_error("Wrong date format. Use DD-MM-YYYY HH:MM (example: 30-11-2026 14:00)");
    }
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_isdst = -1;
    time_t seconds = mktime(&t);
    // mktime turns impossible dates into real ones (31 Feb becomes 3 Mar)
    if (year < 1970 || seconds == -1 || t.tm_mday != day || t.tm_mon != month - 1 || t.tm_hour != hour) {
        throw runtime_error("That date or time does not exist: " + text);
    }
    return (long long)seconds;
}