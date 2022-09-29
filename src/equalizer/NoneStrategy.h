#pragma once

#include "AbstractStrategy.h"

class NoneStrategy : public AbstractStrategy {
public:
    using AbstractStrategy::AbstractStrategy;

    void init(const FilterInterface& previousFilter) override;

    void onMainHandleMoved(int fIndex, double y) override;
    void onLeftHandleMoved(double qIndex) override;
    void onRightHandleMoved(double qIndex) override;
    double q() const override;
    void stepQ(double q) override;
    bool isFrequencyAvailable() const override;
    bool isQAvailable() const override;
    bool isGainAvailable() const override;

private:
    void updateHandles() override;

    double _prevQ;
};
