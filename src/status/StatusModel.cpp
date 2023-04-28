#include "StatusModel.h"

#include <QCoreApplication>

#include <crossover/CrossoverModel.h>
#include <equalizer/EqualizerModel.h>

static StatusModel* _instance = nullptr;

StatusModel* StatusModel::instance() {
    return _instance;
}

StatusModel* StatusModel::init(const CrossoverModel &crossoverModel, const EqualizerModel &equalizerModel) {
    if (!_instance)
        _instance = new StatusModel(crossoverModel, equalizerModel);
    return _instance;
}

StatusModel::StatusModel(const CrossoverModel& crossoverModel,
                         const EqualizerModel& equalizerModel,
                         QObject *parent)
    : QObject{parent},
    _crossoverModel(crossoverModel),
    _equalizerModel(equalizerModel) {
    _timer.setSingleShot(true);
    _timer.callOnTimeout([this]{ clearMessage(); });
}

void StatusModel::clearMessage() {
    _timer.stop();
    _text.clear();
    emit statusChanged();
    QCoreApplication::processEvents();
}

void StatusModel::showMessage(const QString& text, int timeout) {
    if (timeout == 0) {
        _timer.stop();
    } else {
        _timer.start(timeout);
    }
    _text = text;
    emit statusChanged();
    QCoreApplication::processEvents();
}

void StatusModel::setTabIndex(int index) {
    disconnect(this);
    if (index == 2) {
        connect(&_equalizerModel, &EqualizerModel::rangeChanged, this, &StatusModel::rangeChanged);
        _propertyTitle = "Sum";
    } else if (index == 3) {
        connect(&_crossoverModel, &CrossoverModel::rangeChanged, this, &StatusModel::rangeChanged);
        _propertyTitle = "Deviation";
    } else {
        _propertyTitle = "";
    }

    _tabIndex = index;
    emit rangeChanged();
}

double StatusModel::sumMin() const {
    if (_tabIndex == 2)
        return _equalizerModel._sumMin;
    else if (_tabIndex == 3)
        return _crossoverModel._sumMin;

    return 0.0;
}

double StatusModel::sumMax() const {
    if (_tabIndex == 2)
        return _equalizerModel._sumMax;
    else if (_tabIndex == 3)
        return _crossoverModel._sumMax;

    return 0.0;

}

QString StatusModel::ripple() const {
    if (_tabIndex == 3)
        return _crossoverModel.ripple();

    return {};
}
