#include "TimeTable.h"

#include <cmath>
#include <map>

double TimeTable::lower(double ms) {
    return round(ms, true);
}

double TimeTable::upper(double ms) {
    return round(ms, false);
}

double TimeTable::round(double ms, bool toLower) {
    if (toLower) ms -= 0.05; else ms += 0.05;
    static std::map<double, double> table = {
        { 500.0, 50.0 },
        { 200.0, 20.0 },
        { 100.0, 10.0 },
        { 50.0, 5.0 },
        { 20.0, 2.0 },
        { 10.0, 1.0 },
        { 5.0, 0.5 },
        { 2.0, 0.2 },
        { 1.0, 0.1 },
        { 0.0, 0.1 },
    };

    for (auto it = table.rbegin(); it != table.rend(); ++it) {
        if (!toLower && ms >= it->first) {
            return std::floor((ms + it->second) / it->second) * it->second;
        } else if (toLower && ms > it->first) {
            return std::ceil((ms - it->second) / it->second) * it->second;
        }
    }

    return 0.0;
}

TimeTable::TimeTable() {
}
