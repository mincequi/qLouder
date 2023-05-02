#include "TargetModel.h"

#include <common/AudioFilter.h>

TargetModel::TargetModel(QObject *parent)
    : ChartModel(parent) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
}

double TargetModel::loudness() const {
    return _loudness * 2.0;
}

rxcpp::observable<std::vector<double>> TargetModel::fr() const {
    return _fr.get_observable();
}

void TargetModel::moveHandle(int index, double x, double y) {
    double yIndex = std::round(y * 4.0) / 4.0;

    if (index == 0 && _loudness != yIndex) {
        _loudness = yIndex;
        _loudness = std::max(_loudness, 0.0);
        _loudness = std::min(_loudness, 20.0);
        handles()->replace(index, handles()->at(index).x(), _loudness);
        computeLoudnessResponse();
        computeSumResponse();
        emit valuesChanged();
    } else if (index == 1 && _harman != yIndex) {
        _harman = yIndex;
        handles()->replace(index, handles()->at(index).x(), _harman);
        computeHarmanResponse();
        computeSumResponse();
        emit valuesChanged();
    }
}

void TargetModel::stepParam(int index, double x, double y) {
    if (index == 0) {
        _loudness += y;
        _loudness = std::max(_loudness, 0.0);
        _loudness = std::min(_loudness, 20.0);
        handles()->replace(index, handles()->at(index).x(), _loudness);
        computeLoudnessResponse();
        computeSumResponse();
        emit valuesChanged();
    } else if (index == 1) {
        _harman += y;
        handles()->replace(index, handles()->at(index).x(), _harman);
        computeHarmanResponse();
        computeSumResponse();
        emit valuesChanged();
    }
}

void TargetModel::setLoudness(int phon) {
    _loudness = phon/2.0;
    //handles()->replace(0, handles()->at(0).x(), _loudness);
    computeLoudnessResponse();
    computeSumResponse();
    emit valuesChanged();
}


void TargetModel::setLoudnessSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _loudnessSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeLoudnessResponse();
    }
}

void TargetModel::setHarmanSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _harmanSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeHarmanResponse();
    }
}

void TargetModel::setSumSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _sumSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit valuesChanged();
    }
}

double TargetModel::yMin() const {
    return -6.0;
}

double TargetModel::yMax() const {
    return 24.0;
}

void TargetModel::computeLoudnessResponse() {
    if (!handles()) return;

    AudioFilter lp(FilterType::Loudness, 0.0, _loudness * 2.0, 0.0);
    _loudnessResponse = lp.response(_frequencyTable, 1);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_loudnessResponse.at(i))) } );
    }
    _loudnessSeries->replace(points);
}

void TargetModel::computeHarmanResponse() {
    if (!handles()) return;

    AudioFilter hp(FilterType::Loudness, 0.0, _harman, 0.0);
    _harmanResponse = hp.response(_frequencyTable, 1);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), 20 * log10(abs(_harmanResponse.at(i))) } );
    }
    _harmanSeries->replace(points);
}

void TargetModel::computeSumResponse() {
    if (!_sumSeries) return;

    std::vector<double> fr;
    fr.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        fr.push_back(20 * log10(abs(_loudnessResponse.at(i) /** _harmanResponse.at(i)*/ )));
    }
    _fr.get_subscriber().on_next(fr);

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), fr.at(i) } );
    }
    _sumSeries->replace(points);

    _sumMax = -144.0;
    _sumMin = 144.0;
    QPointF p;
    foreach (p, points) {
        _sumMax = std::max(_sumMax, p.y());
        _sumMin = std::min(_sumMin, p.y());
    }

    emit frChanged(fr);
}
