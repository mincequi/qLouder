#include "StatusModel.h"

#include <QCoreApplication>

StatusModel* StatusModel::instance() {
    static StatusModel* instance = new StatusModel();
    return instance;
}

StatusModel::StatusModel(QObject *parent)
    : QObject{parent} {
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
