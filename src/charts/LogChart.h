#pragma once

#include "AbstractChart.h"

#include <vector>

class LogChart : public AbstractChart {
public:
    LogChart();

private:
    void reset() override;
    void zoom(double zoom) override;
    void pan(double pan) override;

    std::vector<double> _frequencies;
    int _fMinIndex = 0;
    int _fMaxIndex = 31;
};
