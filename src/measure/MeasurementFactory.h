#pragma once

#include <string>

class Measurement;

class MeasurementFactory {
public:
    static Measurement* fromDisk(const std::string& fileName);
    static void toDisk(Measurement& measurement, const std::string& fileName);

private:

};
