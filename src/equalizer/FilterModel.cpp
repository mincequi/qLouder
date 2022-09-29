#include "FilterModel.h"

#include <QDebug>

#include "FrequencyTable.h"
#include "FilterInterface.h"
#include "ui/UiUtil.h"

FilterModel::FilterModel(EqualizerModel& eq,
                         FilterType t, int f, double q, double g,
                         QtCharts::QAbstractSeries* response,
                         QObject *parent)
    : _eq(eq),
      _type(t),
      _fIndex(f),
      _qIndex(q),
      _g(g),
      _response(static_cast<QtCharts::QXYSeries*>(response)),
      _lowHighPassStrategy(_eq, *this),
      _noneStrategy(_eq, *this),
      _peakingStrategy(_eq, *this),
      _shelvingStrategy(_eq, *this),
      _strategy(_noneStrategy) {
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
}

int FilterModel::type() const {
    return (int)_type;
}

void FilterModel::setType(FilterType type) {
    _type = type;

    std::reference_wrapper<AbstractStrategy> previousStrategy(_strategy);

    switch (type) {
    case FilterType::Peak:
        _strategy = _peakingStrategy;
        break;
    case FilterType::LowPass:
    case FilterType::HighPass:
        _strategy = _lowHighPassStrategy;
        break;
    case FilterType::LowShelf:
    case FilterType::HighShelf:
        _strategy = _shelvingStrategy;
        break;
    default:
        _strategy = _noneStrategy;
        break;
    }

    _strategy.get().init(previousStrategy);

    emit valuesChanged();
    computeResponse();
}

QString FilterModel::fAsString() const {
    return UiUtil::fToStr(_frequencyTable.at(_fIndex));
}

QString FilterModel::fUnit() const {
    return UiUtil::fToUnit(_frequencyTable.at(_fIndex));
}

double FilterModel::f() const {
    return _frequencyTable.at(_fIndex);
}

double FilterModel::q() const {
    return _strategy.get().q();
}

double FilterModel::g() const {
    return _g;
}

bool FilterModel::isFrequencyAvailable() const {
    return _strategy.get().isFrequencyAvailable();
}

bool FilterModel::isQAvailable() const {
    return _strategy.get().isQAvailable();
}

bool FilterModel::isGainAvailable() const {
    return _strategy.get().isGainAvailable();
}

QtCharts::QAbstractSeries* FilterModel::response() const {
    return _response;
}

void FilterModel::onMainHandleMoved(int xIndex, double yIndex) {
    _strategy.get().onMainHandleMoved(xIndex, yIndex);
    _strategy.get().updateHandles();
    emit valuesChanged();
    computeResponse();
}

void FilterModel::onLeftHandleMoved(double qIndex) {
    auto q = abs(_fIndex - qIndex);
    setQ(q);
    //qDebug() << "q: " << q;

    _strategy.get().updateHandles();
}

void FilterModel::onRightHandleMoved(double qIndex) {
    auto q = abs(_fIndex - qIndex);
    setQ(q);
    //qDebug() << "q: " << q;

    _strategy.get().updateHandles();
}

void FilterModel::setQ(double q) {
    if (q != _qIndex) {
        _qIndex = q;
        emit valuesChanged();
        computeResponse();
    }
}

void FilterModel::init(const FilterInterface&) {
}

void FilterModel::stepF(int index) {
    _fIndex += index;
    emit valuesChanged();
    computeResponse();
    _strategy.get().updateHandles();
}

void FilterModel::stepQ(double d) {
    _strategy.get().stepQ(d);
    emit valuesChanged();
    computeResponse();
    _strategy.get().updateHandles();
}

void FilterModel::stepG(double d) {
    _g += d;
    emit valuesChanged();
    computeResponse();
    _strategy.get().updateHandles();
}

void FilterModel::computeResponse() {
    AudioFilter lp(_type, _frequencyTable.at(_fIndex), _g, q());
    auto response = lp.response(_frequencyTable, 1);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (size_t i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(response.at(i))) } );
    }
    _response->replace(points);
}
