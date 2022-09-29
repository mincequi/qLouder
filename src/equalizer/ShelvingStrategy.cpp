#include "ShelvingStrategy.h"

#include <cmath>

void ShelvingStrategy::init(const FilterInterface& previousFilter) {
    // Clamp to sane values
    setQIndex(std::clamp(qIndex(), 1.0, 120.0));
    updateHandles();
}

void ShelvingStrategy::onMainHandleMoved(int fIndex, double gainDb_) {
    gainDb_ *= 2.0;
    if (frequencyIndex() != fIndex || gainDb() != gainDb_) {
        setFrequencyIndex(fIndex);
        setGainDb(gainDb_);
    }
}

void ShelvingStrategy::onLeftHandleMoved(double) {
}

void ShelvingStrategy::onRightHandleMoved(double) {
}

double ShelvingStrategy::q() const {
    auto pow2N = pow(2, qIndex()/12.0);
    return sqrt(pow2N)/(pow2N - 1);
}

void ShelvingStrategy::stepQ(double q) {
    setQIndex(qIndex() + q);
}

bool ShelvingStrategy::isFrequencyAvailable() const {
    return true;
}

bool ShelvingStrategy::isQAvailable() const {
    return true;
}

bool ShelvingStrategy::isGainAvailable() const {
    return true;
}

void ShelvingStrategy::updateHandles() {
    moveMainHandle(frequencyIndex(), gainDb() / 2.0);
    if (type() == FilterType::LowShelf) {
        moveLeftHandle(frequencyIndex() - qIndex(), gainDb());
        moveRightHandle(frequencyIndex() + qIndex(), 0.0);
    } else {
        moveLeftHandle(frequencyIndex() - qIndex(), 0.0);
        moveRightHandle(frequencyIndex() + qIndex(), gainDb());
    }
}
