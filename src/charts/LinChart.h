#pragma once

#include "AbstractChart.h"

class LinChart : public AbstractChart {
public:
    LinChart();

private:
    void reset() override;
    void zoom(double zoom) override;
    void pan(double pan) override;
};
