#include "ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent)
    : QObject{parent} {
}

QString ProjectModel::name() const {
    return "Empty";
}
