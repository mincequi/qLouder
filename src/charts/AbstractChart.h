#ifndef ICHART_H
#define ICHART_H

class AbstractChart {
public:
    virtual void reset();
    virtual void zoom(double zoom);
    virtual void pan(double pan);

    double _xMin = 0.0;
    double _xMax = 0.0;
    double _yMin = -1.0;
    double _yMax = +1.0;
    int    _tickInterval = 10;
};

#endif // ICHART_H
