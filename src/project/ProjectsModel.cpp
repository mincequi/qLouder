#include "ProjectsModel.h"

#include "ProjectModel.h"

ProjectsModel::ProjectsModel(QObject *parent)
    : QObject{parent} {
}

QQmlListProperty<ProjectModel> ProjectsModel::projects() {
    return QQmlListProperty<ProjectModel>(this, _projects);
}

void ProjectsModel::addProject() {
    const auto project = new ProjectModel;
    _projects.append(project);
    emit projectsChanged();
}

void ProjectsModel::removeProject(int index) {
    auto obj = _projects.at(index);
    _projects.removeAt(index);
    obj->deleteLater();
    emit projectsChanged();
}
