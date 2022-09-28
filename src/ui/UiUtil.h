#pragma once

#include <QString>

class UiUtil {
public:
    static QString fToStr(double f);
    static QString fToUnit(double f);

private:
    UiUtil();
};
