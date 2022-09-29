#include "PeakingStrategy.h"

#include <cmath>

void PeakingStrategy::init(const FilterInterface& previousFilter) {
    // Clamp to sane values
    setGainDb(std::clamp(gainDb(), -36.0, 12.0));
    setQIndex(std::clamp(qIndex(), 1.0, 120.0));
    updateHandles();
}

void PeakingStrategy::onMainHandleMoved(int fIndex, double gainDb_) {
    if (frequencyIndex() != fIndex || gainDb() != gainDb_) {
        setFrequencyIndex(fIndex);
        setGainDb(gainDb_);
    }
}

void PeakingStrategy::onLeftHandleMoved(double) {
}

void PeakingStrategy::onRightHandleMoved(double) {
}

double PeakingStrategy::q() const {
    auto pow2N = pow(2, qIndex()/12.0);
    return sqrt(pow2N)/(pow2N - 1);
}

void PeakingStrategy::stepQ(double q) {
    setQIndex(qIndex() + q);
}

bool PeakingStrategy::isFrequencyAvailable() const {
    return true;
}

bool PeakingStrategy::isQAvailable() const {
    return true;
}

bool PeakingStrategy::isGainAvailable() const {
    return true;
}

void PeakingStrategy::updateHandles() {
    moveMainHandle(frequencyIndex(), gainDb());
    moveLeftHandle(frequencyIndex() - qIndex(), gainDb() / 2);
    moveRightHandle(frequencyIndex() + qIndex(), gainDb() / 2);
}
