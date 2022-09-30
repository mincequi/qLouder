#include "LowHighPassStrategy.h"

#include <QDebug>

#include "FilterModel.h"

void LowHighPassStrategy::init(const FilterInterface&) {
    // Clamp to sane values
    setQIndex(std::clamp(qIndex(), -36.0, 12.0));

    // Move side handles out of sight
    moveLeftHandle(-99, 99.0);
    moveRightHandle(-99, 99.0);

    updateHandles();
}

void LowHighPassStrategy::onMainHandleMoved(int fIndex, double qIndex_) {
    if (frequencyIndex() != fIndex || qIndex() != qIndex_) {
        setFrequencyIndex(fIndex);
        setQIndex(qIndex_);
        //qDebug() << "qIndex:" << qIndex_;
    }
}

void LowHighPassStrategy::onLeftHandleMoved(double) {
}

void LowHighPassStrategy::onRightHandleMoved(double) {
}

double LowHighPassStrategy::q() const {
    return pow(10, qIndex()/20.0);
}

void LowHighPassStrategy::stepQ(double q) {
    setQIndex(qIndex() - q);
}

bool LowHighPassStrategy::isFrequencyAvailable() const {
    return true;
}

bool LowHighPassStrategy::isQAvailable() const {
    return true;
}

bool LowHighPassStrategy::isGainAvailable() const {
    return false;
}

void LowHighPassStrategy::updateHandles() {
    moveMainHandle(frequencyIndex(), qIndex());
}
