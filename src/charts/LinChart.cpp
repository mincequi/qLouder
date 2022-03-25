#include "LinChart.h"

#include <algorithm>
#include <cmath>

LinChart::LinChart() {
    _xMin = -200;
    _xMax = 800.0;
    _yMin = -1.05;
    _yMax = 1.05;
    _tickInterval = 100.0;
}

void LinChart::reset() {
}

void LinChart::zoom(double zoom) {
    if (zoom > 1.0 && (_xMax - _xMin) <= 50.0) {
        return;
    }

    if (zoom < 1.0 && (_xMax - _xMin) >= 1000.0) {
        return;
    }

    _xMin /= zoom;
    _xMax /= zoom;
    _tickInterval = floor((_xMax - _xMin)/50.0) * 5.0;
    _tickInterval = std::max(_tickInterval, 5);
}

void LinChart::pan(double pan) {
    const double distance = _xMax - _xMin;
    _xMin += pan*(distance);
    _xMin = std::max(_xMin, -200.0);

    _xMax += pan*(distance);
    _xMax = std::min(_xMax, 800.0);

    if (distance != (_xMax - _xMin)) {
        if (pan >= 0.0) {
            _xMin = _xMax - distance;
        } else {
            _xMax = _xMin + distance;
        }
    }
}

