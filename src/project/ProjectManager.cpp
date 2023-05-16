#include "ProjectManager.h"

#include <project/Project.h>

#include <rxcpp/operators/rx-combine_latest.hpp>

ProjectManager& ProjectManager::instance() {
    static ProjectManager instance;
    return instance;
}

ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent} {
}

ProjectManager::~ProjectManager() {
}

void ProjectManager::setFrCalibration(Calibration calibration) {
    if (!_currentProject) return;

    _currentProject->setCalibration(calibration);

    _calibratedFr.get_subscriber().on_next(_currentProject->calibratedFr());
}

rxcpp::observable<std::vector<double>> ProjectManager::calibratedFr() const {
    return _calibratedFr.get_observable();
}

void ProjectManager::add(Project* project) {
    if (!project) return;

    _projects.push_back(project);
    _currentProject = project;
    emit currentProjectChanged(_currentProject);

    _calibratedFr.get_subscriber().on_next(_currentProject->calibratedFr());
}

void ProjectManager::remove(int index) {

}
