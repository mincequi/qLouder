#include "FilterModel.h"

#include "FrequencyTable.h"
#include "ui/UiUtil.h"

FilterModel::FilterModel(FilterType t, int f, double q, double g,
                         QtCharts::QAbstractSeries* response,
                         QObject *parent)
    : QObject{parent},
      _type(t),
      _f(f),
      _qIndex(q),
      _g(g),
      _response(static_cast<QtCharts::QXYSeries*>(response)) {
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    computeResponse();
}

int FilterModel::type() const {
    return (int)_type;
}

QString FilterModel::fAsString() const {
    return UiUtil::fToStr(_frequencyTable.at(_f));
}

QString FilterModel::fUnit() const {
    return UiUtil::fToUnit(_frequencyTable.at(_f));
}

double FilterModel::f() const {
    return _frequencyTable.at(_f);
}

double FilterModel::q() const {
    switch (_type) {
    case FilterType::LowPass:
    case FilterType::HighPass:
        return pow(10, _qIndex/20.0);
    case FilterType::Peak:
    default:
        break;
    }

    auto pow2N = pow(2, _qIndex/12.0);
    return sqrt(pow2N)/(pow2N - 1);
}

double FilterModel::g() const {
    return _g;
}

bool FilterModel::isGainEnabled() const {
    switch (_type) {
    case FilterType::LowPass:
    case FilterType::HighPass:
        return false;
    case FilterType::Peak:
    default:
        return true;
    }
}

QtCharts::QAbstractSeries* FilterModel::response() const {
    return _response;
}

void FilterModel::moveHandle(int xIndex, double yIndex) {
    switch (_type) {
    case FilterType::LowPass:
    case FilterType::HighPass:
        if (_f != xIndex || _qIndex != yIndex) {
            _f = xIndex;
            _qIndex = yIndex;
            emit valuesChanged();
            computeResponse();
        }
        break;
    case FilterType::Peak:
    default:
        if (_f != xIndex || _g != yIndex) {
            _f = xIndex;
            _g = yIndex;
            emit valuesChanged();
            computeResponse();
        }
        break;
    }
}

void FilterModel::setType(FilterType type) {
    if (_type == type) {
        return;
    }

    _type = type;
    switch (_type) {
    case FilterType::LowPass:
    case FilterType::HighPass:
        _qIndex = -3;
        break;
    case FilterType::Peak:
    default:
        _qIndex = 12;
        break;
    }

    emit valuesChanged();
    computeResponse();
}

void FilterModel::setQ(double q) {
    if (q != _qIndex) {
        _qIndex = q;
        emit valuesChanged();
        computeResponse();
    }
}

void FilterModel::stepF(int index) {
    _f += index;
    emit valuesChanged();
    computeResponse();
}

void FilterModel::stepQ(double d) {
    _qIndex += d;
    emit valuesChanged();
    computeResponse();
}

void FilterModel::stepG(double d) {
    _g += d;
    emit valuesChanged();
    computeResponse();
}

void FilterModel::computeResponse() {
    AudioFilter lp(_type, _frequencyTable.at(_f), _g, q());
    auto response = lp.response(_frequencyTable, 1);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (size_t i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(response.at(i))) } );
    }
    _response->replace(points);
}
