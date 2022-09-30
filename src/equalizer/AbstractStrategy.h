#pragma once

#include "AudioFilter.h"
#include "FilterInterface.h"

class EqualizerModel;
class FilterModel;

class AbstractStrategy : public FilterInterface {
public:
    AbstractStrategy(EqualizerModel& eq, FilterModel& filter);

protected:
    /**
     * @brief moveMainHandle
     * @param x
     * @param y
     */
    void moveMainHandle(int x, double y);

    /**
     * @brief moveLeftHandle
     * @param x
     * @param y
     */
    void moveLeftHandle(double x, double y);

    /**
     * @brief moveRightHandle
     * @param x
     * @param y
     */
    void moveRightHandle(double x, double y);

    /**
     * @brief updateHandles
     *
     * This function shall update the handles of this filter based in its
     * frequency, gain and q factor.
     */
    virtual void updateHandles() = 0;

    /**
     * @brief type
     * @return the filter's type
     */
    FilterType type() const;

    /**
     * @brief frequencyIndex
     * @return the filter's frequency index
     */
    int frequencyIndex() const;

    /**
     * @brief setFrequencyIndex
     * @param i
     */
    void setFrequencyIndex(int i);

    /**
     * @brief qIndex
     * @return the filter's q factor index
     */
    double qIndex() const;

    /**
     * @brief setQIndex
     * @param i
     */
    void setQIndex(double i);

    /**
     * @brief gainDb
     * @return the filter's gain in decibel
     */
    double gainDb() const;

    /**
     * @brief setGainDb
     * @param g
     */
    void setGainDb(double g);

    EqualizerModel& _eq;
    FilterModel& _filter;

    friend class FilterModel;
};
