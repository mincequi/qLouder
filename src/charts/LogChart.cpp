#include "LogChart.h"

#include <assert.h>

#include <common/FrequencyTable.h>

LogChart::LogChart() {
    _xMin = 20.0;
    _xMax = 20000.0;
    _yMin = -36.0;
    _yMax = 6.0;
    _tickInterval = 10;

    FrequencyTable<double> table(3);//, 20.0, 20000.0);
    _frequencies = table.frequencies();
    assert(_frequencies.size() == 31);
    _fMaxIndex = _frequencies.size() - 1;
}

void LogChart::reset() {
}

void LogChart::zoom(double zoom) {
    const int distance = _fMaxIndex - _fMinIndex;
    if (zoom > 1.0 && distance <= 2) {
        return;
    }

    if (zoom < 1.0 && (distance) >= _frequencies.size()) {
        return;
    }

    const int leftShift = zoom < 1.0 ? -1 : +1;
    _fMinIndex = std::max(0, _fMinIndex + leftShift);
    _fMaxIndex = std::min(int(_frequencies.size()-1), _fMaxIndex - leftShift);

    _xMin = _frequencies.at(_fMinIndex);
    _xMax = _frequencies.at(_fMaxIndex);
}

void LogChart::pan(double pan) {
    const int distance = _fMaxIndex - _fMinIndex;
    const int rightShift = pan >= 0.0 ? +1 : -1;

    _fMinIndex = std::max(0, _fMinIndex + rightShift);
    _fMaxIndex = std::min(int(_frequencies.size()-1), _fMaxIndex + rightShift);

    if (distance != (_fMaxIndex - _fMinIndex)) {
        if (pan >= 0.0) {
            _fMinIndex = _fMaxIndex - distance;
        } else {
            _fMaxIndex = _fMinIndex + distance;
        }
    }

    _xMin = _frequencies.at(_fMinIndex);
    _xMax = _frequencies.at(_fMaxIndex);
}
