#include "UiUtil.h"

UiUtil::UiUtil() {
}

QString UiUtil::fToStr(double f) {
    if (f < 100.0) {
        return QString::number(f, 'f', 1);
    } else if (f < 10000.0) {
        return QString::number(f, 'f', 0);
    } else {
        return QString::number(f/1000.0, 'f', 1);
    }
}

QString UiUtil::fToUnit(double f) {
    if (f < 100.0) {
        return "Hz";
    } else if (f < 10000.0) {
        return "Hz";
    } else {
        return "kHz";
    }
}
