#pragma once

class FilterInterface {
public:
    /**
     * @brief init
     *
     * This is called each time when this strategy gets activated
     */
    virtual void init(const FilterInterface& previousFilter) = 0;

    /**
     * @brief onMainHandleMoved
     * @param fIndex the frequency index
     * @param y Gain value for peaking filter, otherwise Q index
     */
    virtual void onMainHandleMoved(int fIndex, double y) = 0;

    /**
     * @brief onLeftHandleMoved
     * @param qIndex
     */
    virtual void onLeftHandleMoved(double qIndex) = 0;

    /**
     * @brief onRightHandleMoved
     * @param qIndex
     */
    virtual void onRightHandleMoved(double qIndex) = 0;

    /**
     * @brief q
     * @return current q value
     */
    virtual double q() const = 0;

    /**
     * @brief stepQ
     * @param q the delta value to increment/decrement q
     */
    virtual void stepQ(double q) = 0;

    /**
     * @brief isFrequencyAvailable
     * @return wether frequency setting is available for this filter
     */
    virtual bool isFrequencyAvailable() const = 0;

    /**
     * @brief isQAvailable
     * @return wether q setting is available for this filter
     */
    virtual bool isQAvailable() const = 0;

    /**
     * @brief isGainAvailable
     * @return wether gain setting is available for this filter
     */
    virtual bool isGainAvailable() const = 0;
};
