#pragma once

#include <vector>

class TimeTable {
public:
    static double lower(double ms);
    static double upper(double ms);

private:
    static double round(double ms, bool toLower);

    TimeTable();
};
