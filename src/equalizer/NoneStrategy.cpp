#include "NoneStrategy.h"

#include "FilterModel.h"

void NoneStrategy::init(const FilterInterface& previousFilter) {
    // Move handles out of sight
    moveMainHandle(-99, 99.0);
    moveLeftHandle(-99, 99.0);
    moveRightHandle(-99, 99.0);

    _prevQ = previousFilter.q();
}

void NoneStrategy::onMainHandleMoved(int, double) {
}

void NoneStrategy::onLeftHandleMoved(double) {
}

void NoneStrategy::onRightHandleMoved(double) {
}

double NoneStrategy::q() const {
    // Just for cosmetic reasons: we show the q of the previous filter type
    return _prevQ;
}

void NoneStrategy::stepQ(double q) {
}

bool NoneStrategy::isFrequencyAvailable() const {
    return false;
}

bool NoneStrategy::isQAvailable() const {
    return false;
}

bool NoneStrategy::isGainAvailable() const {
    return false;
}

void NoneStrategy::updateHandles() {
}
