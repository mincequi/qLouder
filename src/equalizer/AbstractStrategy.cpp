#include "AbstractStrategy.h"

#include <QDebug>

#include "FilterModel.h"

AbstractStrategy::AbstractStrategy(EqualizerModel& eq,
                             FilterModel& filter) :
    _eq(eq),
    _filter(filter) {
}

void AbstractStrategy::moveMainHandle(int x, double y) {
    const auto filterCount = _eq.filterCount();
    const auto pointCount = _eq.handles()->points().size();
    assert(filterCount*3 == _eq.handles()->points().size());

    const auto index = _eq._filters.indexOf(&_filter)*3;
    assert (pointCount > index);
    _eq.handles()->replace(index, x, y);
}

void AbstractStrategy::moveLeftHandle(double x, double y) {
    const auto index = _eq._filters.indexOf(&_filter)*3 + 1;
    _eq.handles()->replace(index, x, y);
    //qDebug() << "moveLeftHandle> f: " << x << ", q:" << y;
}

void AbstractStrategy::moveRightHandle(double x, double y) {
    const auto index = _eq._filters.indexOf(&_filter)*3 + 2;
    _eq.handles()->replace(index, x, y);
    //qDebug() << "moveRightHandle> f: " << x << ", q:" << y;
}

FilterType AbstractStrategy::type() const {
    return _filter._type;
}

int AbstractStrategy::frequencyIndex() const {
    return _filter._fIndex;
}

void AbstractStrategy::setFrequencyIndex(int i) {
    _filter._fIndex = i;
}

double AbstractStrategy::qIndex() const {
    return _filter._qIndex;
}

void AbstractStrategy::setQIndex(double i) {
    _filter._qIndex = i;
}

double AbstractStrategy::gainDb() const {
    return _filter._g;
}

void AbstractStrategy::setGainDb(double g) {
    _filter._g = g;
}
