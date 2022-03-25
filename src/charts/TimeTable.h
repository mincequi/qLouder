#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <vector>

class TimeTable {
public:
    static double lower(double ms);
    static double upper(double ms);

private:
    static double round(double ms, bool toLower);

    TimeTable();
};

#endif // TIMETABLE_H
