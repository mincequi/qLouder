#pragma once

#include <map>

#include <QString>

class FrFile {
public:
    static std::map<double,double> read(const QString& file);

private:
    FrFile();
};
