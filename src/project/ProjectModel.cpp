#include "ProjectModel.h"

#include <project/ProjectFactory.h>
#include <project/ProjectManager.h>

ProjectModel::ProjectModel(const QString& fileName, QObject *parent)
    : QObject{parent},
    _name(fileName) {

    auto project = ProjectFactory::fromDisk(fileName.toStdString());
    ProjectManager::instance().add(project);
}

QString ProjectModel::name() const {
    return _name;
}
