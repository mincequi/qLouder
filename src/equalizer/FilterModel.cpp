#include "FilterModel.h"

#include "AudioFilter.h"
#include "FrequencyTable.h"
#include "ui/UiUtil.h"

FilterModel::FilterModel(int f, double q, double g,
                         QtCharts::QAbstractSeries* response,
                         QObject *parent)
    : QObject{parent},
      _f(f),
      _q(q),
      _g(g),
      _response(static_cast<QtCharts::QXYSeries*>(response)) {
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    computeResponse();
}

QString FilterModel::f() const {
    return UiUtil::fToStr(_frequencyTable.at(_f));
}

double FilterModel::q() const {
    auto pow2N = pow(2, _q/12.0);
    return sqrt(pow2N)/(pow2N - 1);
}

QtCharts::QAbstractSeries* FilterModel::response() const {
    return _response;
}

void FilterModel::moveHandle(int x, double y) {
    if (_f != x || _g != y) {
        _f = x;
        _g = y;
        emit valuesChanged();
        computeResponse();
    }
}

void FilterModel::setQ(double q) {
    if (q != _q) {
        _q = q;
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
    _q += d;
    emit valuesChanged();
    computeResponse();
}

void FilterModel::stepG(double d) {
    _g += d;
    emit valuesChanged();
    computeResponse();
}

void FilterModel::computeResponse() {
    AudioFilter lp(FilterType::Peak, _frequencyTable.at(_f), _g, q());
    auto response = lp.response(_frequencyTable, 1);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(response.at(i))) } );
    }
    _response->replace(points);
}
