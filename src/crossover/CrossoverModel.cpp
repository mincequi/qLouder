#include "CrossoverModel.h"

#include <common/AudioFilter.h>
#include <common/FrequencyTable.h>
#include <common/UiUtil.h>

CrossoverModel::CrossoverModel(QObject *parent)
    : ChartModel(parent) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    _highPassF = _frequencyTable.size()-1;
}

QString CrossoverModel::lowPassFrequencyReadout() const {
    return UiUtil::fToStr(_frequencyTable.at(_lowPassF));
}

QString CrossoverModel::lowPassFrequencyUnitReadout() const {
    return UiUtil::fToUnit(_frequencyTable.at(_lowPassF));
}

double CrossoverModel::lowPassQ() const {
    return pow(10, _lowPassQ/20.0);
}

QString CrossoverModel::highPassFrequencyReadout() const {
    return UiUtil::fToStr(_frequencyTable.at(_highPassF));
}

QString CrossoverModel::highPassFrequencyUnitReadout() const {
    return UiUtil::fToUnit(_frequencyTable.at(_highPassF));
}

double CrossoverModel::highPassQ() const {
    return pow(10, _highPassQ/20.0);
}

QString CrossoverModel::ripple() const {
    double ripple = abs(_sumMax - _sumMin);
    if (ripple < 10.0) {
        return QString::number(ripple, 'f', 1);
    } else {
        return QString::number(ripple, 'f', 0);
    }
}

void CrossoverModel::setHandles(QtCharts::QAbstractSeries* series) {
    ChartModel::setHandles(series);

    _lowPassF = handles()->at(0).x();
    _lowPassQ = handles()->at(0).y();
    _highPassF = handles()->at(1).x();
    _highPassQ = handles()->at(1).y();
}

void CrossoverModel::moveHandle(int index, double x, double y) {
    int xIndex = std::round(x);
    double yIndex = std::round(y * 5.0) / 5.0;

    if (index == 0 && (_lowPassF != xIndex || _lowPassQ != yIndex)) {
        _lowPassF = xIndex;
        _lowPassQ = yIndex - _lowPassG;
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 1 && (_highPassF != xIndex || _highPassQ != yIndex)) {
        _highPassF = xIndex;
        _highPassQ = yIndex - _highPassG;
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 2 && _lowPassG != yIndex) {
        _lowPassG = yIndex;
        handles()->replace(index, handles()->at(index).x(), _lowPassG);
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 3 && _highPassG != yIndex) {
        _highPassG = yIndex;
        handles()->replace(index, handles()->at(index).x(), _highPassG);
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    }
}

void CrossoverModel::stepParam(int index, double x, double y) {
    if (index == 0) {
        _lowPassF += x;
        _lowPassQ += y;
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 1) {
        _highPassF += x;
        _highPassQ += y;
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 2) {
        _lowPassG += y;
        handles()->replace(index, handles()->at(index).x(), _lowPassG);
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 3) {
        _highPassG += y;
        handles()->replace(index, handles()->at(index).x(), _highPassG);
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    }
}

void CrossoverModel::setOrder(int index, int orderIndex) {
    if (index == 0) {
        _lowPassC = orderIndex + 1;
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 1) {
        _highPassC = orderIndex + 1;
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    }
}

void CrossoverModel::invert(bool invert) {
    _invert = invert;
    computeSumResponse();
    emit rangeChanged();
}

void CrossoverModel::setLowPassSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _lowPassSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeLowPassResponse();
    }
}

void CrossoverModel::setHighPassSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _highPassSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeHighPassResponse();
    }
}

void CrossoverModel::setSumSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _sumSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit rangeChanged();
    }
}

double CrossoverModel::yMin() const {
    return -18.0;
}

double CrossoverModel::yMax() const {
    return 6.0;
}

void CrossoverModel::computeLowPassResponse() {
    if (!handles()) return;

    handles()->replace(0, _lowPassF, _lowPassQ + _lowPassG);

    AudioFilter lp(FilterType::LowPass, _frequencyTable.at(_lowPassF), 0.0, pow(10, _lowPassQ/20.0));
    _lowPassResponse = lp.response(_frequencyTable, _lowPassC);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_lowPassResponse.at(i))) + _lowPassG } );
    }
    _lowPassSeries->replace(points);
}

void CrossoverModel::computeHighPassResponse() {
    if (!handles()) return;

    handles()->replace(1, _highPassF, _highPassQ + _highPassG);

    AudioFilter hp(FilterType::HighPass, _frequencyTable.at(_highPassF), 0.0, pow(10, _highPassQ/20.0));
    _highPassResponse = hp.response(_frequencyTable, _highPassC);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_highPassResponse.at(i))) + _highPassG } );
    }
    _highPassSeries->replace(points);
}

void CrossoverModel::computeSumResponse() {
    if (!_sumSeries) return;

    double lpFactor = pow(10, _lowPassG/20.0);
    double hpFactor = pow(10, _highPassG/20.0);
    _fr.clear();
    _fr.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        _fr.push_back(20 * log10(abs(_lowPassResponse.at(i) + (_invert ? -1.0 : 1.0) * _highPassResponse.at(i))));
    }
    _sumMax = -144.0;
    _sumMin = 144.0;
    for (const auto& s : _fr) {
        _sumMax = std::max(_sumMax, s);
        _sumMin = std::min(_sumMin, s);
    }

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_lowPassResponse.at(i)*lpFactor + (_invert ? -1.0 : 1.0) * _highPassResponse.at(i)*hpFactor)) } );
    }
    _sumSeries->replace(points);

    emit frChanged(_fr);
}
