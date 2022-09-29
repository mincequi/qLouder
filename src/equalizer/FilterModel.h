#pragma once

#include <complex>

#include <QObject>
#include <QtCharts/QXYSeries>

#include "AudioFilter.h"
#include "EqualizerModel.h"
#include "FilterInterface.h"
#include "LowHighPassStrategy.h"
#include "NoneStrategy.h"
#include "PeakingStrategy.h"
#include "ShelvingStrategy.h"

class FilterModel : public QObject, public FilterInterface {
    Q_OBJECT

    // Filter model needs to provide response series, so that it can be identified
    // and removed from the chart when this filter gets deleted.
    Q_PROPERTY(QtCharts::QAbstractSeries* response READ response CONSTANT)

    Q_PROPERTY(int type READ type NOTIFY valuesChanged)
    Q_PROPERTY(QString f READ fAsString NOTIFY valuesChanged)
    Q_PROPERTY(QString fUnit READ fUnit NOTIFY valuesChanged)
    Q_PROPERTY(double q READ q NOTIFY valuesChanged)
    Q_PROPERTY(double g MEMBER _g NOTIFY valuesChanged)
    Q_PROPERTY(bool isFrequencyAvailable READ isFrequencyAvailable NOTIFY valuesChanged)
    Q_PROPERTY(bool isQAvailable READ isQAvailable NOTIFY valuesChanged)
    Q_PROPERTY(bool isGainAvailable READ isGainAvailable NOTIFY valuesChanged)

public:
    explicit FilterModel(EqualizerModel& eq,
                         FilterType type, int f, double q, double g,
                         QtCharts::QAbstractSeries* response,
                         QObject *parent = nullptr);

    /**
     * @brief type
     * @return the current filter type
     */
    int type() const;

    /**
     * @brief setType
     *
     * This sets the filter type and inits this class. Do not call this within
     * the constructor.
     *
     * @param type
     */
    void setType(FilterType type);

    QString fAsString() const;
    QString fUnit() const;
    double f() const;
    double q() const override;
    double g() const;
    bool isFrequencyAvailable() const override;
    bool isQAvailable() const override;
    bool isGainAvailable() const override;

    QtCharts::QAbstractSeries* response() const;

    void onMainHandleMoved(int x, double y) override;
    void onLeftHandleMoved(double qIndex) override;
    void onRightHandleMoved(double qIndex) override;

    void setQ(double q);

signals:
    void nameChanged();
    void valuesChanged();

private:
    void init(const FilterInterface&) override;

    void stepF(int f);
    void stepQ(double q) override;
    void stepG(double g);

    void computeResponse();

    EqualizerModel& _eq;
    QtCharts::QXYSeries* _response = nullptr;

    std::vector<double> _frequencyTable;
    FilterType _type = FilterType::Invalid;
    int _fIndex = 120;
    double _qIndex = 3.0;
    double _g = -3.0;

    LowHighPassStrategy _lowHighPassStrategy;
    NoneStrategy _noneStrategy;
    PeakingStrategy _peakingStrategy;
    ShelvingStrategy _shelvingStrategy;
    std::reference_wrapper<AbstractStrategy> _strategy;

    friend class AbstractStrategy;
    friend class EqualizerModel;
};
