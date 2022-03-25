#ifndef LINCHART_H
#define LINCHART_H

#include "AbstractChart.h"

class LinChart : public AbstractChart {
public:
    LinChart();

private:
    void reset() override;
    void zoom(double zoom) override;
    void pan(double pan) override;
};

#endif // LINCHART_H
