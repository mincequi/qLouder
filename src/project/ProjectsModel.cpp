#include "ProjectsModel.h"

#include "ProjectModel.h"

#include <QUrl>

ProjectsModel::ProjectsModel(QObject *parent)
    : QObject{parent},
    _projectObjects(this, &_projects) {
}

QQmlListProperty<ProjectModel> ProjectsModel::projects() {
    return _projectObjects;
}

void ProjectsModel::loadProject(const QUrl& url) {
    const auto fileName = url.path();
    const auto project = new ProjectModel(fileName);
    _projects.append(project);
    _index = _projects.size()-1;
    emit projectsChanged();
    emit indexChanged();
}

void ProjectsModel::setIndex(int index) {
    _index = index;
    emit indexChanged();
}

void ProjectsModel::removeProject(int index) {
    auto obj = _projects.at(index);
    _projects.removeAt(index);
    obj->deleteLater();
    _index = std::min(_index, _projects.size()-1);
    emit projectsChanged();
    emit indexChanged();
}
