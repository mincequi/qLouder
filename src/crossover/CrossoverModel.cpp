#include "CrossoverModel.h"

#include <utility>

#include <QAudioDeviceInfo>
#include <QDebug>
#include <QSaveFile>
#include <QTimer>
#include <QUrl>
#include <QVariant>

#include "AudioFilter.h"
#include "FrequencyTable.h"

CrossoverModel::CrossoverModel(QObject *parent)
    : ChartModel(parent) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    _highPassF = _frequencyTable.size()-1;
}

double CrossoverModel::lowPassFrequencySlider() const {
    return _lowPassF;
}

double CrossoverModel::highPassFrequencySlider() const {
    return _highPassF;
}

void CrossoverModel::setLowPassFrequencySlider(double value) {
    _lowPassF = std::min(value, (double)(_frequencyTable.size()-1));
    if (_highPassF <= _lowPassF) {
        _highPassF = _lowPassF + 1;
	}
	emit rangeChanged();
}

void CrossoverModel::setHighPassFrequencySlider(double value) {
    _highPassF = std::max(value, 1.0);
    if (_lowPassF >= _highPassF) {
        _lowPassF = _highPassF - 1;
	}
	emit rangeChanged();
}

QString CrossoverModel::lowPassFrequencyReadout() const {
    const auto f = _frequencyTable.at(_lowPassF);
    if (f < 100.0) {
        return "<b>" + QString::number(f, 'f', 1) + "</b><size=8> Hz</size>";
    } else if (f < 1000.0) {
        return "<b>" + QString::number(f, 'f', 0) + "</b><size=8> Hz</size>";
    } else if (f < 10000.0) {
        return "<b>" + QString::number(f/1000.0, 'f', 2) + "</b><size=8> kHz</size>";
    } else {
        return "<b>" + QString::number(f/1000.0, 'f', 1) + "</b><size=8> kHz</size>";
    }
}

double CrossoverModel::lowPassQ() const {
    return pow(10, _lowPassQ/20.0);
}

QString CrossoverModel::highPassFrequencyReadout() const {
    const auto f = _frequencyTable.at(_highPassF);
    if (f < 100.0) {
        return "<b>" + QString::number(f, 'f', 1) + "</b><size=10> Hz</size>";
    } else if (f < 1000.0) {
        return "<b>" + QString::number(f, 'f', 0) + "</b><size=10> Hz</size>";
    } else if (f < 10000.0) {
        return "<b>" + QString::number(f/1000.0, 'f', 2) + "</b><size=10> kHz</size>";
    } else {
        return "<b>" + QString::number(f/1000.0, 'f', 1) + "</b><size=10> kHz</size>";
    }
}

double CrossoverModel::highPassQ() const {
    return pow(10, _highPassQ/20.0);
}

void CrossoverModel::setHandles(QtCharts::QAbstractSeries* series) {
    if (series) {
        _handles = static_cast<QtCharts::QXYSeries*>(series);
        _lowPassF = _handles->at(0).x();
        _lowPassQ = _handles->at(0).y();
        _highPassF = _handles->at(1).x();
        _highPassQ = _handles->at(1).y();
    }
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
        _handles->replace(index, _handles->at(index).x(), _lowPassG);
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 3 && _highPassG != yIndex) {
        _highPassG = yIndex;
        _handles->replace(index, _handles->at(index).x(), _highPassG);
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
        _handles->replace(index, _handles->at(index).x(), _lowPassG);
        computeLowPassResponse();
        computeSumResponse();
        emit rangeChanged();
    } else if (index == 3) {
        _highPassG += y;
        _handles->replace(index, _handles->at(index).x(), _highPassG);
        computeHighPassResponse();
        computeSumResponse();
        emit rangeChanged();
    }
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
    _handles->replace(0, _lowPassF, _lowPassQ + _lowPassG);

    AudioFilter lp(FilterType::LowPass, _frequencyTable.at(_lowPassF), 0.0, pow(10, _lowPassQ/20.0));
    _lowPassResponse = lp.response(_frequencyTable);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_lowPassResponse.at(i))) + _lowPassG } );
    }
    _lowPassSeries->replace(points);
}

void CrossoverModel::computeHighPassResponse() {
    _handles->replace(1, _highPassF, _highPassQ + _highPassG);

    AudioFilter lp(FilterType::HighPass, _frequencyTable.at(_highPassF), 0.0, pow(10, _highPassQ/20.0));
    _highPassResponse = lp.response(_frequencyTable);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_highPassResponse.at(i))) + _highPassG } );
    }
    _highPassSeries->replace(points);
}

void CrossoverModel::computeSumResponse() {
    double lpFactor = pow(10, _lowPassG/20.0);
    double hpFactor = pow(10, _highPassG/20.0);
    std::vector<double> sum;
    sum.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        sum.push_back(20 * log10(abs(_lowPassResponse.at(i) - _highPassResponse.at(i))));
    }
    _sumMax = -144.0;
    _sumMin = 144.0;
    for (const auto& s : sum) {
        _sumMax = std::max(_sumMax, s);
        _sumMin = std::min(_sumMin, s);
    }

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_lowPassResponse.at(i)*lpFactor - _highPassResponse.at(i)*hpFactor)) } );
    }
    _sumSeries->replace(points);
}
