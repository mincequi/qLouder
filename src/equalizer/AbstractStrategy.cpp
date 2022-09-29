#include "AbstractStrategy.h"

#include "FilterModel.h"

AbstractStrategy::AbstractStrategy(EqualizerModel& eq,
                             FilterModel& filter) :
    _eq(eq),
    _filter(filter) {
}

void AbstractStrategy::moveMainHandle(int x, double y) {
    const auto filterCount = _eq.filters().size();
    const auto pointCount = _eq.handles()->points().size();
    assert(filterCount*3 == _eq.handles()->points().size());

    const auto index = _eq._filters.indexOf((QObject*)&_filter)*3;
    assert (pointCount > index);
    _eq.handles()->replace(index, x, y);
}

void AbstractStrategy::moveLeftHandle(int x, double y) {
    const auto filterCount = _eq.filters().size();
    const auto pointCount = _eq.handles()->points().size();
    assert(filterCount*3 == _eq.handles()->points().size());

    const auto index = _eq._filters.indexOf((QObject*)&_filter)*3 + 1;
    assert (pointCount > index);
    _eq.handles()->replace(index, x, y);
}

void AbstractStrategy::moveRightHandle(int x, double y) {
    const auto filterCount = _eq.filters().size();
    const auto pointCount = _eq.handles()->points().size();
    assert(filterCount*3 == _eq.handles()->points().size());

    const auto index = _eq._filters.indexOf((QObject*)&_filter)*3 + 2;
    assert (pointCount > index);
    _eq.handles()->replace(index, x, y);
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
